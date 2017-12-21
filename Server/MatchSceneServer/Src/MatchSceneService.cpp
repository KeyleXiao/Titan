#include "stdafx.h"
#include "MatchSceneService.h"
#include "ISchemeCenter.h"
#include "IServerGlobal.h"
#include "IMessageDispatch.h"
#include "MatchDef.h"
#include "IGatewayAcceptorService.h"
#include "IWarManager.h"
#include "EntityDef.h"
#include "IActorService.h"
#include "IWarService.h"
#include "IServiceMgr.h"
#include "IFramework.h"
#include "ICenterConnectorService.h"
#include "EntityHelper.h"
#include "ILoginServerProtocol.h"
#include "IMigrateService.h"
#include "IActorMgr.h"
#include "ISceneManager.h"
#include "MatchSceneHelper.h"
#include "MigrateDef.h"
#include "ExternalHelper.h"
#include "PlayerKillMessageDef.h"
#include "TeamHelper.h"

atomic<hprecord*> hprecord::head(NULL);
atomic<hazard*> hazard::s_p(NULL);

#if MatchScene_DisableShowDebug
#undef MATCH_TRACELN
#define MATCH_TRACELN(x)
#endif


MatchSceneService::MatchSceneService()
    : m_u16ServerID( 0 )
    , m_mutex(0)
{
 
}

void MatchSceneService::release()
{
	MATCH_TRACELN("MatchSceneService::release");
    IMessageDispatch* pMessageDispatch = gServerGlobal->getMessageDispatch();
    if (nullptr == pMessageDispatch)
    {
        ErrorLn(__FUNCTION__": failed! can not found the IMessageDispatch! ");
        return;
    }

    // 自己需要处理消息
    pMessageDispatch->unregisterTransmitHandler(MSG_MODULEID_MATCH);

    // 取消注册响应客户端消息
    gServerGlobal->getMessageDispatch()->unregisterMessageHandler( MSG_MODULEID_MATCH );

    rkt::IEventEngine* pEventEngine = gServerGlobal->getEventEngine();
    if (pEventEngine)
    {
        // 订阅角色登出事件
        pEventEngine->UnSubscibe(this, EVENT_GAME_ACTOR_LOGOUT, SOURCE_TYPE_PERSON, 0);

        // EVENT_SYSTEM_SERVERINFOUPDATED
        pEventEngine->UnSubscibe(this, EVENT_SYSTEM_SERVERINFOUPDATED, SOURCE_TYPE_SYSTEM, 0);

        pEventEngine->UnSubscibe(this, EVENT_CONTINUE_PLAY_CLIENT_INCOMMING, SOURCE_TYPE_SYSTEM, 0);
    }

    // 移除还未响应的定时器
    vector<int>::iterator iter = m_vecTimerID.begin();
    for (;iter != m_vecTimerID.end();++iter)
    {
        // 删除定时器
        gServerGlobal->getTimerService()->KillTimer(*iter, this);
    }
    m_vecTimerID.clear();

    map<DWORD, obuf>::iterator iterRole = m_mapCreateRoomRoleInfo.begin();
    for (;iterRole != m_mapCreateRoomRoleInfo.end();++iterRole)
    {
        // 删除数据
        iterRole->second.clear();
    }
    m_mapCreateRoomRoleInfo.clear();


    map<DWORD, obuf>::iterator iterOBRole = m_mapCreateRoomOBRoleInfo.begin();
    for (;iterOBRole != m_mapCreateRoomOBRoleInfo.end();++iterOBRole)
    {
        // 删除数据
        iterOBRole->second.clear();
    }
    m_mapCreateRoomOBRoleInfo.clear();
    

    //清空
    m_mapCreateRoomContext.clear();
    m_WarID2RoomID.clear();
    m_RoomID2Room.clear();
    m_mapWarID2Room.clear();
	delete this;
}

bool MatchSceneService::create()
{
	MATCH_TRACELN("MatchSceneService::create");
    IMessageDispatch* pMessageDispatch = gServerGlobal->getMessageDispatch();
    if (nullptr == pMessageDispatch)
    {
        ErrorLn("MatchScene::load failed! can not found the IMessageDispatch! ");
        return false;
    }
    // 自己需要处理消息
    pMessageDispatch->registerTransmitHandler(MSG_MODULEID_MATCH, this);

    // 处理客户端发送过来的消息
    gServerGlobal->getMessageDispatch()->registerMessageHandler(MSG_MODULEID_MATCH, this);

    rkt::IEventEngine* pEventEngine = gServerGlobal->getEventEngine();
    if (pEventEngine)
    {
        // 订阅角色登出事件
        pEventEngine->Subscibe(this, EVENT_GAME_ACTOR_LOGOUT, SOURCE_TYPE_PERSON, 0, __FUNCTION__);

        pEventEngine->Subscibe(this, EVENT_SYSTEM_SERVERINFOUPDATED, SOURCE_TYPE_SYSTEM, 0, __FUNCTION__);

        pEventEngine->Subscibe(this, EVENT_CONTINUE_PLAY_CLIENT_INCOMMING, SOURCE_TYPE_SYSTEM, 0, __FUNCTION__);
    }

	m_u16ServerID = gServerGlobal->getServerID();

	return true;
}

void MatchSceneService::start()
{
	MATCH_TRACELN(__FUNCTION__);
}


void MatchSceneService::warEnd(DWORD dwWarID, LONGLONG llDBWarID)
{
    
	DWORD RoomID = getRoomIDByWarID( dwWarID );
    MATCH_TRACELN(__FUNCTION__": RoomID = "<<RoomID<<" nWarID ="<<dwWarID);
	if (0 == RoomID)
	{
		ErrorLn("MatchSceneService::end RoomID=0 nWarID=" << dwWarID);
		return;
	}
	MatchSceneRoom* pRoom = getRoomByID( RoomID );
	if (nullptr == pRoom)
	{
		ErrorLn("MatchSceneService::end nullptr == pRoom RoomID=" << RoomID);
		return;
	}

    pRoom->send2SocialWarEnd();

    removeWarID2RoomID( dwWarID );
    removeRoom( RoomID );
    m_sceneID2RoomID.erase(pRoom->getSceneID());
    pRoom->realese();
    m_mapWarID2Room.erase(dwWarID);

    gServerGlobal->getTimerService()->KillTimer(dwWarID, this);
}

void MatchSceneService::onMsgCreateMoba( void* pData, size_t nLen )
{
	if (pData == NULL || nLen < sizeof(SMsgCreateRoom_OC))
	{
		ErrorLn(__FUNCTION__ ": the message is invalid, size="<< nLen);
		return;
	}

	SMsgCreateRoom_OC* pMsg = (SMsgCreateRoom_OC*) pData;
	if (nLen != sizeof(SMsgCreateRoom_OC) || pMsg->byRoleCount==0 )
	{
		ErrorLn(__FUNCTION__ ": the message length is too short, size="<< nLen <<", count="<< pMsg->byRoleCount);
		return;
	}

    DWORD dwMatchTypeID = pMsg->dwMatchTypeID;  // 撮合匹配类型
	DWORD dwRoomID = pMsg->dwRoomID;			// 房间ID
	BYTE  byRoomType = pMsg->nRoomType;			// 房间类型
	// 房间在战斗中
	if ( getRoomByID(dwRoomID) != NULL)
	{
		ErrorLn( __FUNCTION__": exist the same room id! dwRoomID=" << dwRoomID);
		return;
	}

    // 记录起来 所有玩家数据到来之后再创建其他相关
    m_mapCreateRoomContext.insert(pair<DWORD, SMsgCreateRoom_OC>(dwRoomID, *pMsg));
}

void MatchSceneService::onMsgCreateMobaOBRoleData(void* pData, size_t nLen)
{
    ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
    if ( pSchemeCenter == NULL )
    {
        ErrorLn(__FUNCTION__ << ":get SchemeCenter failed!");
        return;
    }

    // 记录起来 所有玩家数据到来之后再创建其他相关
    if (pData == NULL || nLen < sizeof(SMsgCreateRoomRoleData))
    {
        ErrorLn(__FUNCTION__ ": the message is invalid, size="<< nLen);
        return;
    }

    SMsgCreateRoomRoleData* pMsg = (SMsgCreateRoomRoleData*) pData;
    if (nLen < sizeof(SMsgCreateRoomRoleData) )
    {
        return;
    }
    DWORD dwRoomID = pMsg->dwRoomID;
    map<DWORD, obuf>::iterator iterRole = m_mapCreateRoomOBRoleInfo.find(dwRoomID);
    if (iterRole == m_mapCreateRoomOBRoleInfo.end())
    {
        //
        char *pRoleData = (char *)(pMsg+1);
        obuf roleBuf;
        roleBuf.push_back(pRoleData, pMsg->nBufSize);
        pair<map<DWORD, obuf>::iterator,bool> bRet = m_mapCreateRoomOBRoleInfo.insert(pair<DWORD,obuf>(dwRoomID,roleBuf));
        if(!bRet.second)
        {
            ErrorLn(__FUNCTION__": insert error roomID ="<<dwRoomID);
            return;
        }

        iterRole = bRet.first;
    }
    else
    {
        char *pRoleData = (char *)(pMsg+1);
        iterRole->second.push_back(pRoleData, pMsg->nBufSize);
    }
}

void MatchSceneService::onMsgCreateMobaRoleData(void* pData, size_t nLen)
{
    ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
    if ( pSchemeCenter == NULL )
    {
        ErrorLn(__FUNCTION__ << ":get SchemeCenter failed!");
        return;
    }

    // 记录起来 所有玩家数据到来之后再创建其他相关
    if (pData == NULL || nLen < sizeof(SMsgCreateRoomRoleData))
    {
        ErrorLn(__FUNCTION__ ": the message is invalid, size="<< nLen);
        return;
    }

    SMsgCreateRoomRoleData* pMsg = (SMsgCreateRoomRoleData*) pData;
    if (nLen < sizeof(SMsgCreateRoomRoleData) )
    {
        return;
    }
    DWORD dwRoomID = pMsg->dwRoomID;
    map<DWORD, obuf>::iterator iterRole = m_mapCreateRoomRoleInfo.find(dwRoomID);
    if (iterRole == m_mapCreateRoomRoleInfo.end())
    {
        //
        char *pRoleData = (char *)(pMsg+1);
        obuf roleBuf;
        roleBuf.push_back(pRoleData, pMsg->nBufSize);
        pair<map<DWORD, obuf>::iterator,bool> bRet = m_mapCreateRoomRoleInfo.insert(pair<DWORD,obuf>(dwRoomID,roleBuf));
        if(!bRet.second)
        {
            ErrorLn(__FUNCTION__": insert error roomID ="<<dwRoomID);
            return;
        }
            
        iterRole = bRet.first;
    }
    else
    {
        char *pRoleData = (char *)(pMsg+1);
        iterRole->second.push_back(pRoleData, pMsg->nBufSize);
    }

    // 所有玩家数据下发
    if (!pMsg->bIsEnd)
    {
        return;
    }

    // todo 如果创建不成功要通知来源场景移除房间所有玩家
    // 创建战场
    map<DWORD, SMsgCreateRoom_OC>::iterator iterWarContext = m_mapCreateRoomContext.find(dwRoomID);
    if (iterWarContext == m_mapCreateRoomContext.end())
    {
        ErrorLn(__FUNCTION__": can't find RoomContext roomID ="<<dwRoomID);
        return;
    }

	SSchemeMatchRoom* pSchemeMatchRoom = pSchemeCenter->getSchemeMatchRoom()->getSchemeMatchRoomByMatchTypeID(iterWarContext->second.dwMatchTypeID);
    if(pSchemeMatchRoom == NULL)
    {
        ErrorLn(__FUNCTION__ << ": pSchemeMatchRoom == NULL dwMatchTypeID ="<<iterWarContext->second.dwMatchTypeID);
        return;
    }

    // 获取OB人员信息
    SRoomRoleBase* pOBRole = NULL;
    map<DWORD, obuf>::iterator iterOBRole = m_mapCreateRoomOBRoleInfo.find(dwRoomID);
    if (iterOBRole != m_mapCreateRoomOBRoleInfo.end())
    {
        pOBRole = (SRoomRoleBase*)iterOBRole->second.data();
    }

    // 普通玩家数据
    SRoomRoleBase* pRole = (SRoomRoleBase*)iterRole->second.data();
    if (pRole == NULL)
    {
        ErrorLn(__FUNCTION__": can't find RoomContext pRole == NULL roomID ="<<dwRoomID);
        return;
    }
    SMsgCreateRoom_OC* pWarContext = &iterWarContext->second;
    if (pWarContext == NULL)
    {
        ErrorLn(__FUNCTION__": can't find RoomContext pWarContext == NULL roomID ="<<dwRoomID);
        return;
    }

	// 更新场景服召唤师技能数据
    Room_Create_War_Context context;
    context.byRoomType      = iterWarContext->second.nRoomType;
    context.dwMatchTypeID   = iterWarContext->second.dwMatchTypeID;
    context.nRoomIndex      = iterWarContext->second.dwRoomID;
    context.nMapID          = iterWarContext->second.dwMapID;
    context.nSchemeConfigWarID= iterWarContext->second.nConfigWarID;
    context.nMode           = iterWarContext->second.nModeID;
    context.byRoleCount     = iterWarContext->second.byRoleCount;
    context.dwAvgRankScore  = iterWarContext->second.dwAvgRank;
    context.byCampCount     = pSchemeMatchRoom->m_CampCount;
    
    int nWarID = gServerGlobal->getWarManager()->CreateWar(context);
    if(nWarID < 0)
    {
        ErrorLn(__FUNCTION__": CreateWar failed! nWarID < 0");
        return;
    }

    if (0 != getRoomIDByWarID( nWarID ))
    {
        ErrorLn(__FUNCTION__": failed! have SceneID =" << nWarID);
        return;
    }

    // 记录房间数据
    MatchSceneRoom* pRoom = new MatchSceneRoom();
    if(pRoom == NULL)
    {
        ErrorLn(__FUNCTION__": new MatchSceneRoom failed!");
        return;
    }
    
    pRoom->inital(this, pWarContext, nWarID);
    // OB成员数据加载到房间
    if (pOBRole != NULL)
    {
        pRoom->initalOBPlayer(pOBRole);
        iterOBRole->second.clear();
        m_mapCreateRoomOBRoleInfo.erase(iterOBRole);
    }
    
    // 比赛人员加载到房间
    if (pRole != NULL)
    {
        pRoom->initalPlayer(pRole);
        iterRole->second.clear();
        m_mapCreateRoomRoleInfo.erase(iterRole);
    }

    m_WarID2RoomID.insert(nWarID, pMsg->dwRoomID);
    m_RoomID2Room.insert(pMsg->dwRoomID, pRoom);
    m_mapWarID2Room.insert(nWarID, pRoom);

    // 移除记录数据
    m_mapCreateRoomContext.erase(iterWarContext);
}

void MatchSceneService::onMsgMatchRoomBanHeroData(void* pData, size_t nLen)
{
    if (pData == NULL || nLen < sizeof(SMsgMatchRoomBanHeroData))
    {
        ErrorLn(__FUNCTION__ ": the message is invalid, size="<< nLen);
        return;
    }

    SMsgMatchRoomBanHeroData* pMsg = (SMsgMatchRoomBanHeroData*) pData;
    if (nLen != sizeof(SMsgMatchRoomBanHeroData) + pMsg->nCount * sizeof(SMsgMatchRoomBanInfo))
    {
        ErrorLn(__FUNCTION__ ": the message is invalid, size="<< nLen);
        return;
    }

    if (pMsg->nCount == 0)
    {
        return;
    }

    MatchSceneRoom *pRoom = getRoomByID(pMsg->dwRoomID);
    if (pRoom == NULL)
    {
        ErrorLn(__FUNCTION__ ": pRoom is null, roomID="<< pMsg->dwRoomID);
        return;
    }

    // 通知战场角色进入事件
    SERVICE_PTR pContainer = gServerGlobal->getWarManager()->GetWar(pRoom->m_nWarID);
    if(pContainer == 0)
    {
        ErrorLn(__FUNCTION__ << ", set actor base info failed");
        return;
    }
    IWarService* pWarService = (IWarService*)pContainer->query_interface();
    if(pWarService == NULL)
    {
        ErrorLn(__FUNCTION__ << ", set actor base info failed");
        return;
    }

    pWarService->setBanHeroList((char *)pData, nLen);
}

void MatchSceneService::onMsgMatchRoomExData(void* pData, size_t nLen)
{
	if (pData == NULL || nLen < sizeof(SMsgMatchRoomExData))
	{
		ErrorLn(__FUNCTION__ ": onMsgMatchRoomExData the message is invalid, size="<< nLen);
		return;
	}

	SMsgMatchRoomExData* pMsg = (SMsgMatchRoomExData*) pData;

	MatchSceneRoom *pRoom = getRoomByID(pMsg->dwRoomID);
	if (pRoom == NULL)
	{
		ErrorLn(__FUNCTION__ ": onMsgMatchRoomExData pRoom is null, roomID="<< pMsg->dwRoomID);
		return;
	}
	
	if (pMsg->nRoomType == HallRoomType_LegendCup)
	{
		if (nLen < (sizeof(SMsgMatchRoomExData) + sizeof(SMsgLegendCupRoomInfo)))
		{
			ErrorLn(__FUNCTION__ ": SMsgLegendCupRoomInfo the message length is too short, size="<< nLen);
			return;
		}
		SMsgLegendCupRoomInfo *pExData = (SMsgLegendCupRoomInfo*)(pMsg + 1);
		pRoom->setLegendCupRoomInfo(pExData);
	}
}

// 设置玩家场景加载进度
void MatchSceneService::setClientSceneLoadingProgress(PDBID idActor, int nProgress, long lPing)
{
    DWORD RoomID = getRoomIDByPDBID(idActor);
    if (RoomID == 0)
    {
        ErrorLn(__FUNCTION__": RoomID == 0 idActor="<<idActor)
        return;
    }
    MatchSceneRoom* pRoom = getRoomByID( RoomID );
    if (nullptr == pRoom)
    {
        WarningLn(__FUNCTION__": nullptr == pRoom RoomID=" << RoomID);
        return;
    }

    if (pRoom->isOBPlayer(idActor))
    {
        if(nProgress < 100)
        {
            return;
        }

        MatchScenePlayer* pPlayer = pRoom->findOBPlayer(idActor);
        if(pPlayer == NULL)
        {
            ErrorLn(__FUNCTION__": pPlayer == NULL")
            return;
        }

        // 设置玩家准备状态
        if (pPlayer->m_eState != ES_READY)
        {
            pRoom->setOBPlayerReady(idActor);
            if (!pRoom->createMobaOBActor(idActor))
            {
                ErrorLn(__FUNCTION__": can't create obActor idActor="<<idActor);
                return;
            }
        }
    }
    else
    {
        int nWarID = pRoom->m_nWarID;

        // 广播玩家加载进度
        broadSceneLoadingProgress(nWarID, idActor, nProgress, lPing);

        if(nProgress < 100)
        {
            return;
        }

        // 设置玩家准备状态
        pRoom->setPlayerReady(idActor);
    }
    
}

void MatchSceneService::createWarTimeOutTimer(int nWarID)
{
    Simple_Atom_Lock lock(&m_mutex);
    if( find(m_vecTimerID.begin(), m_vecTimerID.end(), nWarID) == m_vecTimerID.end() )
    {
        MatchSceneRoom  *pSceneRoom = NULL;
        if( !m_mapWarID2Room.find(nWarID, pSceneRoom) || pSceneRoom==NULL)
        {
            ErrorLn(__FUNCTION__ << ", war=" << nWarID << " not found");
            return;
        }

        if (pSceneRoom->getState() > SRS_StateUnknown)
        {
            return;
        }

       // 设置房间开始迁移的时间 加载超时根据这个判定
        pSceneRoom->setStartMigrateTime();
        // 设置房间状态
        pSceneRoom->setState(SRS_StateWaitLoading);
        gServerGlobal->getTimerService()->SetTimer(nWarID, 10000, this, INFINITY_CALL, __FUNCTION__);
        m_vecTimerID.push_back(nWarID);
    }
}

// 战场玩家执行迁移
void MatchSceneService::createMobaActors(int nWarID)
{
    MatchSceneRoom* pRoom = NULL;
    if( !m_mapWarID2Room.find(nWarID, pRoom) || pRoom==NULL)
    {
        ErrorLn(__FUNCTION__ << ", war=" << nWarID << " not found");
        return;
    }

    // 开始创建所有角色
    pRoom->onCreatePlayers();  
}

void MatchSceneService::onActorLogin(void* pLoginEvent)
{
    event_actor_login* pEvent = (event_actor_login*)pLoginEvent;
    MATCH_TRACELN(__FUNCTION__": ActorLogout! nScene = "<<pEvent->nSceneID<<" PDBID = "<<pEvent->dwDBID);

    if (!IS_DYNAMIC_SCENE(pEvent->nSceneID))
    {
        return;
    }

    Simple_Atom_Lock lock(&m_mutex);
    DWORD nSceneID = (DWORD) pEvent->nSceneID;
    map<int, DWORD>::iterator itRoom = m_sceneID2RoomID.find(nSceneID);
    if(itRoom == m_sceneID2RoomID.end())
    {
        WarningLn(__FUNCTION__": roomID can't find ! nSceneID="<<pEvent->nSceneID<<" PDBID = "<<pEvent->dwDBID);
        return;
    }

    DWORD dwRoomID = itRoom->second;
    MatchSceneRoom* pRoom = NULL;
    if(!m_RoomID2Room.find(dwRoomID, pRoom) || pRoom == NULL)
    {
        return;
    }

    pRoom->setPlayerLogin(pEvent->dwDBID);
}

// 处理战斗中登出
void MatchSceneService::onActorLogout(void* pLogoutEvent)
{
    
    event_actor_logout* pEvent = (event_actor_logout*)pLogoutEvent;
    MATCH_TRACELN(__FUNCTION__": ActorLogout! nScene = "<<pEvent->nSceneID<<" PDBID = "<<pEvent->dwDBID);

    if (!IS_DYNAMIC_SCENE(pEvent->nSceneID))
    {
        return;
    }

    // 走到这里相当于已经有以下条件了 
    // 1、要么没有真人玩家存在了 2、要么战场已经结束了 3、要么可以主动退出战场
    // 战场玩家destory之后会发logout事件走这里 从这里判定释放战场
    Simple_Atom_Lock lock(&m_mutex);
    DWORD nSceneID = (DWORD) pEvent->nSceneID;
    map<int, DWORD>::iterator itRoom = m_sceneID2RoomID.find(nSceneID);
    if(itRoom == m_sceneID2RoomID.end())
    {
        WarningLn(__FUNCTION__": roomID can't find ! nSceneID="<<pEvent->nSceneID<<" PDBID = "<<pEvent->dwDBID);
        return;
    }

    DWORD dwRoomID = itRoom->second;
    MatchSceneRoom* pRoom = NULL;
    if(!m_RoomID2Room.find(dwRoomID, pRoom) || pRoom == NULL)
    {
        ErrorLn(__FUNCTION__": room can't find ! dwRoomID="<<dwRoomID);
        return;
    }

    // 最后一个真人玩家退出
    bool isOBPlayer = pRoom->isOBPlayer(pEvent->dwDBID);
    MatchScenePlayer* pLogOutPlayer = isOBPlayer == true ? pRoom->findOBPlayer(pEvent->dwDBID): pRoom->findPlayer(pEvent->dwDBID);
    if (pLogOutPlayer ==NULL)
    {
        ErrorLn(__FUNCTION__": pLogOutPlayer ==NULL dwDBID = "<<pEvent->dwDBID);
        return;
    }

    // 设置退出状态
    pLogOutPlayer->m_role.clientID = INVALID_CLIENT;
    pLogOutPlayer->m_UID = INVALID_UID;
    isOBPlayer == true? pRoom->setOBPlayerLogout(pEvent->dwDBID) : pRoom->setPlayerLogout(pEvent->dwDBID);
    
    
    // 移除房间关系
    MATCH_TRACELN(__FUNCTION__": removePDBID2RoomID() PDBID= "<<pEvent->dwDBID<<" dwRoomID="<<dwRoomID);
    removePDBID2RoomID(pEvent->dwDBID, dwRoomID);

    // 通知社会服删除对应房间关系
    SMsgRemoveActorRoomInfo_SO msg;
    msg.dwRoomID = dwRoomID;					// 房间ID 
    msg.dwActorID = pEvent->dwDBID;             // 玩家ID

    sendToSocial(MSG_MATCH_ROOM_MODULEMSG, SO_MSG_MATCH_REMOVE_ACTOR_ROOMINFO, &msg, sizeof(msg) );

    // 所有玩家client都无效了 把所有不在线的玩家和电脑玩家释放
    if(pRoom->getAllPlayerCount() == pRoom->getClientInvalidCount() && pRoom->allOBPlayerIsInValid())
    {
        pRoom->destoryRoomPlayer();
    }

    ///////////////////////////注意！//////////////////////////////////////
    // 所有角色(包括电脑和玩家还有OB玩家)都释放了之后才释放战场
    // 根据 MatchScenePlayer.m_eState = ES_EXIT_BATTLE_FIELD 状态来释放战场
    // 必须保证只有这个函数调用的setPlayerLogout 设置 ES_EXIT_BATTLE_FIELD状态 
    // 其他任何地方都不要设置 m_eState ES_EXIT_BATTLE_FIELD
    ////////////////////////////////////////////////////////////////////////
    if(pRoom->getAllLogoutCount() == pRoom->getAllPlayerCount() && pRoom->isAllOBLogout())
    {
        // 定时器移除
        gServerGlobal->getTimerService()->KillTimer(pRoom->m_nWarID, this);

        vector<int>::iterator iter = m_vecTimerID.begin();
        for (;iter != m_vecTimerID.end();++iter)
        {
            if (*iter == pRoom->m_nWarID)
            {
                m_vecTimerID.erase(iter);
                break;
            }
        }

        // 销毁战场
        EmphasisLn(__FUNCTION__": DestroyWar! nScene = "<<pRoom->m_nSceneID<<" WarID = "<<pRoom->m_nWarID);
        gServerGlobal->getWarManager()->DestroyWar(pRoom->m_nWarID);
    }
}

// 发送社会服消息
void MatchSceneService::sendToSocial(BYTE byKeyAction, BYTE nMsgID, void * data, size_t len)
{
	MATCH_TRACELN( __FUNCTION__ ": msgid="<<nMsgID <<", len="<< len);
	ICenterConnectorService * pConnector = gServerGlobal->getCenterConnectorService();
	if(pConnector == NULL)
	{
		ErrorLn( __FUNCTION__ ": CenterConnectorService==NULL");
		return;
	}

    // 子消息相关
    SMsgMatchDataSubMsg subMsg;
    subMsg.dwMsgCode = nMsgID;

    // 数据打包
    obuf256 obMsg;
    obMsg<<subMsg;
    if (len > 0)
    {
        obMsg.push_back(data, len);
    }

	pConnector->sendToSocial(MSG_MODULEID_MATCH, byKeyAction, obMsg.data(), obMsg.size());
}

// 发送创建结果
void MatchSceneService::sendCreateMobaResult(DWORD dwRoomID, int nWarID, int nSceneID, bool bResult)
{
	TraceLn(__FUNCTION__ ": dwRoomID="<<dwRoomID);

	SMsgCreateMobaResult_SO msg;
	msg.dwRoomID = dwRoomID;					// 房间ID 
    msg.nBattleFieldID = nWarID; 
    msg.nSceneID = nSceneID;
    msg.nSerialNo = gServerGlobal->getSceneManager()->sceneIDToSerialNo(nSceneID);
	msg.bResult = bResult;						// 结果

	// 然后通知客户端加载场景
	sendToSocial(MSG_MATCH_ROOM_MODULEMSG,SO_MSG_MATCH_CREATE_MOBA_RESULT, &msg, sizeof(msg) );

    MatchSceneRoom* pRoom = NULL;
    if(!m_RoomID2Room.find(dwRoomID, pRoom) || pRoom == NULL)
    {
        ErrorLn(__FUNCTION__ << " room not found,room id=" << dwRoomID);
        return;
    }
    if(bResult)
    {
        m_sceneID2RoomID[nSceneID] = dwRoomID;
        pRoom->m_nSceneID = nSceneID; 
    }
    pRoom->setWarActorBaseInfo();

    if (pRoom->getRoomType() == HallRoomType_LegendCup)
    {
        pRoom->setWarLegendCupInfo();
    }

    // 设置续玩
    if(bResult)
    {        
        // 超时创建所有战场角色
        createWarTimeOutTimer(nWarID);
    }
}


void MatchSceneRoom::setWarLegendCupInfo()
{
    SERVICE_PTR pContainer = gServerGlobal->getWarManager()->GetWar(m_nWarID);
    if(pContainer == 0)
    {
        ErrorLn(__FUNCTION__ << ", set actor base info failed");
        return;
    }
    IWarService* pWarService = (IWarService*)pContainer->query_interface();
    if(pWarService == NULL)
    {
        ErrorLn(__FUNCTION__ << ", set actor base info failed");
        return;
    }

    pWarService->setLegendCupInfo(m_sLegendCupRoomInfo);
}


DWORD MatchSceneService::getRoomIDByWarID( DWORD nWarID )
{
    DWORD dwRoomID = 0;
	if (m_WarID2RoomID.find( nWarID, dwRoomID))
	{
		return dwRoomID;
	}
	return 0;
}

MatchSceneRoom* MatchSceneService::getRoomByID( DWORD RoomID )
{
    MatchSceneRoom* pRoom = NULL;
	if (m_RoomID2Room.find( RoomID, pRoom))
	{
		return pRoom;
	}
	return nullptr;
}

void MatchSceneService::removeRoom(DWORD RoomID)
{
    m_RoomID2Room.erase(RoomID);
}

void MatchSceneService::addPDBID2RoomID(PDBID idActor, DWORD RoomID)
{
    //m_PDBID2RoomID[ idActor ] = RoomID;
    if (!m_PDBID2RoomID.insert(idActor, RoomID))
    {
        DWORD dwRoomID = 0;
        m_PDBID2RoomID.find(idActor, dwRoomID);
        ErrorLn(__FUNCTION__": dwRoomID = "<<dwRoomID<<" updateRoomID ="<<RoomID);
    }
    
}

void MatchSceneService::removePDBID2RoomID(PDBID idActor, DWORD dwRoomID)
{
    DWORD dwFindRoomID;
    m_PDBID2RoomID.find(idActor, dwFindRoomID);
    if (dwRoomID == dwFindRoomID)
    {
        m_PDBID2RoomID.erase(idActor);
    }
}

void MatchSceneService::addWarID2RoomID(DWORD dwWarID, DWORD RoomID)
{
    //m_WarID2RoomID[ dwWarID ] = RoomID;
    m_WarID2RoomID.insert(dwWarID, RoomID);
}

void MatchSceneService::removeWarID2RoomID(DWORD SceneID)
{
    m_WarID2RoomID.erase(SceneID);
}

DWORD MatchSceneService::getRoomIDByPDBID( PDBID idActor )
{
    DWORD dwRoomID = 0;
	if ( !m_PDBID2RoomID.find( idActor, dwRoomID) || dwRoomID==0)
	{
		return 0;
	}

	return dwRoomID;
}

SMapLandSchemeInfo* MatchSceneService::getSchemeMapLand( uchar ModeID, DWORD MapID, uint32 u32Camp)
{
    ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
    if (pSchemeCenter)
    {
        ISchemeMapLand* pSchemeMapLand = pSchemeCenter->getSchemeMapLand();
        if (pSchemeMapLand)
        {
            return pSchemeMapLand->GetMapLandSchemeInfo(MapID, ModeID, u32Camp);
        }			
    }
	return nullptr;
}

void MatchSceneService::onMsgCustomerInviteRespon(ClientID clientID, void* pData, size_t stLen)
{
    if (clientID == INVALID_CLIENT )
    {
        ErrorLn(__FUNCTION__": pData is invalid, clientID == INVALID_CLIENT");
        return;
    }

    if (sizeof(SMsg_MasterInviteRespon_CS) != stLen)
    {
        ErrorLn(__FUNCTION__": pData is invalid, len not right right len ="<< sizeof(SMsg_MasterInviteRespon_CS)<<" stLen="<<stLen);
        return;
    }

    SMsg_MasterInviteRespon_CS *pMsg = (SMsg_MasterInviteRespon_CS *)pData;
    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActor = helper.m_ptr;
    if (pActor == NULL)
    {
        ErrorLn(__FUNCTION__": pActor==NULL, clientID="<<clientID);
        return;
    }
    DWORD dwActorID = pActor->getProperty_Integer(PROPERTY_ID);
    //判定下状态
    if (pActor->getGameState() != GAME_STATE_IDLE)
    {
        g_EHelper.showSystemMessage(dwActorID,CHAT_TIP_MATCH_PLAYER_NOT_IDLE);
        return;
    }


    if(pActor->getPKState() != PK_STATE_NOMAL)
    {
        // 通知PK部件停止
        pActor->forceEndPK();
    }

    do 
    {
        TeamHelper teamHelper;
        ITeamSceneService *pTeamService = teamHelper.m_ptr;
        if (pTeamService != NULL)
        {
            DWORD dwResponTeamID = pTeamService->getTeamIDByPlayer(dwActorID);
            // 响应请求的玩家没有队伍 直接进入房间
            if (dwResponTeamID == INVALID_TEAM_ID)
            {
                break;
            }

            DWORD dwInviteTeamID = pTeamService->getTeamIDByPlayer(pMsg->dwInviteActorID);
            if (dwInviteTeamID  == dwResponTeamID)
            {
                // 队伍相同不用退出队伍
                break;
            }

            // 退出自己的队伍
            pTeamService->leaveTeam(dwActorID);
        }
    } while (false);

    //准备进入大厅的数据
    SMsg_MasterInviteRespon_SO* pSendMsg = new SMsg_MasterInviteRespon_SO();
    if (pSendMsg ==NULL)
    {
        ErrorLn(__FUNCTION__": SMsg_MasterInviteRespon_SO()")
        return;
    }

    pSendMsg->dwRoomID      = pMsg->dwRoomID;
    pSendMsg->idActor       = dwActorID;
    pSendMsg->dwSrcScene    = pActor->getSceneID();
    pSendMsg->dwSrcWorldID  = pActor->getFromGameWorldID();
    pSendMsg->bySex         = pActor->getProperty_Integer(PROPERTY_SEX);

    //房间所需附加数据
    pActor->getPlayerRoomExData(&(pSendMsg->sExData));
    pSendMsg->sExData.clientID = clientID;

    // 召唤师技能
    for (int i = 0; i < SUMMONER_SKILL_COUNT; ++i )
    {
        pSendMsg->nSummonerSkill[i] = pActor->getSummonerSkillInfo(i);
    }

    sstrcpyn(pSendMsg->szPlayerName, pActor->getProperty_String(PROPERTY_NAME).c_str(), sizeof(pSendMsg->szPlayerName));

    // 转发到社会服
    sendToSocial(MSG_MATCH_ROOM_MODULEMSG, CS_MSG_MATCH_CUSTOMER_INVITE_RESPON, pSendMsg, sizeof(SMsg_MasterInviteRespon_SO));
    delete pSendMsg;
}



void MatchSceneService::onMsgActorViewLoaded(ClientID clientID, void* pData, size_t stLen)
{
    // 转发到社会服
    if (clientID == INVALID_CLIENT )
    {
        ErrorLn(__FUNCTION__": pData is invalid, clientID == INVALID_CLIENT");
        return;
    }

    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActor = helper.m_ptr;
    if (pActor == NULL)
    {
        ErrorLn(__FUNCTION__": pActor==NULL, clientID="<<clientID);
        return;
    }
    DWORD dwActorID = pActor->getProperty_Integer(PROPERTY_ID);
    DWORD dwFromWorld = pActor->getFromGameWorldID();

    obuf ob;
    ob<<dwActorID<<dwFromWorld;

    sendToSocial(MSG_MATCH_SERVICE_MODULEMSG, CS_MSG_MATCH_ACTOR_VIEW_LOADED, ob.data(), ob.size());
}



void MatchSceneService::onMsgEnterHall(ClientID clientID, void* pData, size_t stLen)
{
	if (clientID == INVALID_CLIENT )
	{
        ErrorLn(__FUNCTION__": pData is invalid, clientID == INVALID_CLIENT");
		return;
	}

    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActor = helper.m_ptr;
	if (pActor == NULL)
	{
		ErrorLn(__FUNCTION__": pActor==NULL, clientID="<<clientID);
		return;
	}

    //准备进入大厅的数据
    SMsgEnterHall_CS* pMsgInfo = new SMsgEnterHall_CS;
    if (pMsgInfo ==NULL)
    {
        return;
    }

	pMsgInfo->idActor       = pActor->getProperty_Integer(PROPERTY_ID);
    pMsgInfo->dwSrcScene    = pActor->getSceneID();
    pMsgInfo->dwSrcWorldID  = pActor->getFromGameWorldID();
	pMsgInfo->clientID      = clientID;
    pMsgInfo->bySex         = pActor->getProperty_Integer(PROPERTY_SEX);
    //房间所需附加数据
    pActor->getPlayerRoomExData(&(pMsgInfo->sExData));
    pMsgInfo->sExData.clientID = clientID;
	sstrcpyn(pMsgInfo->szCreateName, pActor->getProperty_String(PROPERTY_NAME).c_str(), sizeof(pMsgInfo->szCreateName));

    // 召唤师技能
    for (int i = 0; i < SUMMONER_SKILL_COUNT; ++i )
    {
        pMsgInfo->nSummonerSkill[i] = pActor->getSummonerSkillInfo(i);
    }

	// 转发到社会服
	sendToSocial(MSG_MATCH_ROOM_MODULEMSG, CS_MSG_ENTER_HALL, pMsgInfo, sizeof(SMsgEnterHall_CS));
    delete pMsgInfo;
}

void MatchSceneService::onMsgCreateGuideRoom(ClientID clientID, void* pData, size_t stLen)
{
    if (clientID == INVALID_CLIENT )
    {
        ErrorLn(__FUNCTION__": pData is invalid, clientID == INVALID_CLIENT");
        return;
    }
    if (sizeof(SMsgCreateGuideRoom) != stLen)
    {
        ErrorLn(__FUNCTION__": pData is invalid, len not right right len ="<< sizeof(SMsgCreateGuideRoom)<<" stLen="<<stLen);
        return;
    }

    SMsgCreateGuideRoom *pMsg = (SMsgCreateGuideRoom *)pData;
    if (pMsg ==NULL)
    {
        return;
    }

    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActor = helper.m_ptr;
    if (pActor == NULL)
    {
        ErrorLn(__FUNCTION__": pActor==NULL, clientID="<<clientID);
        return;
    }

    //准备进入大厅的数据
    pMsg->idActor       = pActor->getProperty_Integer(PROPERTY_ID);
    pMsg->dwSrcScene    = pActor->getSceneID();
    pMsg->dwSrcWorldID  = pActor->getFromGameWorldID();
    pMsg->bySex     = pActor->getProperty_Integer(PROPERTY_SEX);

    //房间所需附加数据
    pActor->getPlayerRoomExData(&(pMsg->sExData));
    pMsg->sExData.clientID = clientID;
    sstrcpyn(pMsg->szCreateName, pActor->getProperty_String(PROPERTY_NAME).c_str(), sizeof(pMsg->szCreateName));

    // 召唤师技能
    for (int i = 0; i < SUMMONER_SKILL_COUNT; ++i )
    {
        pMsg->nSummonerSkill[i] = pActor->getSummonerSkillInfo(i);
    }

    // 转发到社会服
    sendToSocial(MSG_MATCH_ROOM_MODULEMSG, CS_MSG_CREATE_GUIDE_ROOM, pMsg, sizeof(SMsgCreateGuideRoom));
}

void MatchSceneService::onMsgCreateRoom(ClientID clientID, void* pData, size_t stLen)
{
	// 判断参数
	if (clientID == INVALID_CLIENT || pData == NULL || stLen != sizeof(SMsgCreateRoom_CS))
	{
        ErrorLn(__FUNCTION__": pData is Wrong");
		return;
	}

	SMsgCreateRoom_CS *pMsg = (SMsgCreateRoom_CS *)pData;
	//获取PDBID
    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActor = helper.m_ptr;
	if (pActor == NULL)
	{
		ErrorLn(__FUNCTION__": -- pActor == NULL clientID = "<<clientID);
		return;
	}
	//设置PDBID
	pMsg->idActor = pActor->getProperty_Integer(PROPERTY_ID);

	// 转发到社会服
	sendToSocial(MSG_MATCH_ROOM_MODULEMSG, CS_MSG_CREATE_ROOM, pMsg, sizeof(SMsgCreateRoom_CS));

}

void MatchSceneService::onMsgEnterRoom(ClientID clientID, void* pData, size_t stLen)
{
	// 判断参数
	if (clientID == INVALID_CLIENT || pData == NULL || stLen != sizeof(SMsgEnterRoom))
	{
        ErrorLn(__FUNCTION__": pData is Wrong");
		return;
	}

	SMsgEnterRoom *pMsg = (SMsgEnterRoom *)pData;
	//获取PDBID
    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActor = helper.m_ptr;
	if (pActor == NULL)
	{
		ErrorLn(__FUNCTION__": -- pActor == NULL clientID = "<<clientID);
		return;
	}
	//设置PDBID
	pMsg->idActor = pActor->getProperty_Integer(PROPERTY_ID);

    if (IsGM(pMsg->idActor))
    {
        pMsg->bOBEnter = true;
    }

	// 转发到社会服
	sendToSocial(MSG_MATCH_ROOM_MODULEMSG, CS_MSG_ENTER_ROOM, pMsg, sizeof(SMsgEnterRoom));
}

void MatchSceneService::onMsgCustomerAddComputer(ClientID clientID, void* pData, size_t stLen)
{
    // 判断参数
    if (clientID == INVALID_CLIENT)
    {
        return;
    }

    SMsgStartGame pMsg;
    //获取PDBID
    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActor = helper.m_ptr;
    if (pActor == NULL)
    {
        ErrorLn(__FUNCTION__": -- pActor == NULL clientID = "<<clientID);
        return;
    }
    //设置PDBID
    pMsg.idActor = pActor->getProperty_Integer(PROPERTY_ID);

    // 转发到社会服
    sendToSocial(MSG_MATCH_ROOM_MODULEMSG, CS_MSG_MASTER_COVER_COMPUTER, &pMsg, sizeof(SMsgStartGame));

}


void MatchSceneService::onMsgStartGame(ClientID clientID, void* pData, size_t stLen)
{
	// 判断参数
	if (clientID == INVALID_CLIENT)
	{
		return;
	}

	SMsgStartGame pMsg;
	//获取PDBID
    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActor = helper.m_ptr;
	if (pActor == NULL)
	{
		ErrorLn(__FUNCTION__": -- pActor == NULL clientID = "<<clientID);
		return;
	}
	//设置PDBID
	pMsg.idActor = pActor->getProperty_Integer(PROPERTY_ID);

	// 转发到社会服
	sendToSocial(MSG_MATCH_ROOM_MODULEMSG, CS_MSG_START_GAME, &pMsg, sizeof(SMsgStartGame));
	
}

void MatchSceneService::onMsgSelectHero(ClientID clientID, void* pData, size_t stLen)
{
	// 判断参数
	if (clientID == INVALID_CLIENT || pData == NULL || stLen != sizeof(SMsgSelectHero))
	{
        ErrorLn(__FUNCTION__": pData is Wrong");
		return;
	}

	SMsgSelectHero *pMsg = (SMsgSelectHero *)pData;
	//获取PDBID
    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActor = helper.m_ptr;
	if (pActor == NULL)
	{
		ErrorLn(__FUNCTION__": -- pActor == NULL clientID = "<<clientID);
		return;
	}
	//设置PDBID
	pMsg->idActor = pActor->getProperty_Integer(PROPERTY_ID);
    pMsg->bIsTestPlayer = pActor->isTester();
	
	// 转发到社会服
	sendToSocial(MSG_MATCH_ROOM_MODULEMSG, CS_MSG_SELECT_HERO, pMsg, sizeof(SMsgSelectHero));
	
}


void MatchSceneService::onMsgPreSelectHero(ClientID clientID, void* pData, size_t stLen)
{
    // 判断参数
    if (clientID == INVALID_CLIENT || pData == NULL || stLen != sizeof(SMsgSelectHero))
    {
        ErrorLn(__FUNCTION__": pData is Wrong");
        return;
    }

    SMsgSelectHero *pMsg = (SMsgSelectHero *)pData;
    //获取PDBID
    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActor = helper.m_ptr;
    if (pActor == NULL)
    {
        ErrorLn(__FUNCTION__": -- pActor == NULL clientID = "<<clientID);
        return;
    }
    //设置PDBID
    pMsg->idActor = pActor->getProperty_Integer(PROPERTY_ID);
    pMsg->bIsTestPlayer = pActor->isTester();

    // 转发到社会服
    sendToSocial(MSG_MATCH_ROOM_MODULEMSG, CS_MSG_PRE_SELECT_HERO, pMsg, sizeof(SMsgSelectHero));

}

void MatchSceneService::onMsgPosBanHero(ClientID clientID, void* pData, size_t stLen)
{
    // 判断参数
    if (clientID == INVALID_CLIENT || pData == NULL || stLen != sizeof(SMsgPosBanHero_CS))
    {
        ErrorLn(__FUNCTION__": pData is Wrong");
        return;
    }

    SMsgPosBanHero_CS *pMsg = (SMsgPosBanHero_CS *)pData;
    //获取PDBID
    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActor = helper.m_ptr;
    if (pActor == NULL)
    {
        ErrorLn(__FUNCTION__": -- pActor == NULL clientID = "<<clientID);
        return;
    }
    //设置PDBID
    pMsg->dwActorID = pActor->getProperty_Integer(PROPERTY_ID);

    // 转发到社会服
    sendToSocial(MSG_MATCH_ROOM_MODULEMSG, CS_MSG_POS_BAN_HERO, pMsg, sizeof(SMsgPosBanHero_CS));
}


void MatchSceneService::onMsgReadyToFight(ClientID clientID, void* pData, size_t stLen)
{
	// 判断参数
	if (clientID == INVALID_CLIENT || pData == NULL || stLen != sizeof(SMsgReadyToFight))
	{
        ErrorLn(__FUNCTION__": pData is Wrong");
		return;
	}

	SMsgReadyToFight *pMsg = (SMsgReadyToFight *)pData;
	//获取PDBID
    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActor = helper.m_ptr;
	if (pActor == NULL)
	{
		ErrorLn(__FUNCTION__": -- pActor == NULL clientID = "<<clientID);
		return;
	}
	//设置PDBID
	pMsg->idActor = pActor->getProperty_Integer(PROPERTY_ID);

	// 转发到社会服
	sendToSocial(MSG_MATCH_ROOM_MODULEMSG, CS_MSG_READY_TO_FIGHT, pMsg, sizeof(SMsgReadyToFight));

}


// 响应选择英雄皮肤消息
void MatchSceneService::onMsgSelectHeroSkin( ClientID clientID, void* pData, size_t stLen )
{
	// 判断参数
	if (clientID == INVALID_CLIENT || pData == NULL || stLen != sizeof(SMsgSelectHeroSkin))
	{
		ErrorLn(__FUNCTION__": pData is Wrong");
		return;
	}

	SMsgSelectHeroSkin *pMsg = (SMsgSelectHeroSkin *)pData;
	//获取PDBID
	ActorHelper helper(clientID, ACTOR_CLIENT);
	IActorService *pActor = helper.m_ptr;
	if (pActor == NULL)
	{
		ErrorLn(__FUNCTION__": -- pActor == NULL clientID = "<<clientID);
		return;
	}
	//设置PDBID
	pMsg->idActor = pActor->getProperty_Integer(PROPERTY_ID);

	// 校验玩家选择的英雄皮肤是否可用
	if ( pActor->canUseHeroSkin(pMsg->dwHeroID, pMsg->dwSkinID) == false )
	{
		return;
	}

	// 转发到社会服
	sendToSocial(MSG_MATCH_ROOM_MODULEMSG, CS_MSG_SELECT_HERO_SKIN, pMsg, sizeof(SMsgSelectHeroSkin));
}

void MatchSceneService::onMsgPosChange(ClientID clientID, void* pData, size_t stLen)
{
	// 判断参数
	if (clientID == INVALID_CLIENT || pData == NULL || stLen != sizeof(SMsgPosChange))
	{
        ErrorLn(__FUNCTION__": pData is Wrong");
		return;
	}

	SMsgPosChange *pMsg = (SMsgPosChange *)pData;
	//获取PDBID
    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActor = helper.m_ptr;
	if (pActor == NULL)
	{
		ErrorLn(__FUNCTION__": -- pActor == NULL clientID = "<<clientID);
		return;
	}
	//设置PDBID
	pMsg->idActor = pActor->getProperty_Integer(PROPERTY_ID);

	// 转发到社会服
	sendToSocial(MSG_MATCH_ROOM_MODULEMSG, CS_MSG_POS_CHANGE, pMsg, sizeof(SMsgPosChange));
	
}

void MatchSceneService::onMsgExitRoom(ClientID clientID, void* pData, size_t stLen)
{
	// 判断参数
	if (clientID == INVALID_CLIENT)
	{
		return;
	}

	SMsgExitRoom pMsg;
	//获取PDBID
    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActor = helper.m_ptr;
	if (pActor == NULL)
	{
		ErrorLn(__FUNCTION__": -- pActor == NULL clientID = "<<clientID);
		return;
	}
	//设置PDBID
	pMsg.idActor = pActor->getProperty_Integer(PROPERTY_ID);

	// 转发到社会服
	sendToSocial(MSG_MATCH_ROOM_MODULEMSG, CS_MSG_EXIT_ROOM, &pMsg, sizeof(SMsgExitRoom));
	
	// 在房间内状态
	pActor->setGameState(GAME_STATE_IDLE);
}

void MatchSceneService::onMsgKickOutPlayer(ClientID clientID, void* pData, size_t stLen)
{
	// 判断参数
	if (clientID == INVALID_CLIENT || pData == NULL || stLen != sizeof(SMsgKickOutPlayer))
	{
        ErrorLn(__FUNCTION__": pData is Wrong");
		return;
	}

	SMsgKickOutPlayer *pMsg = (SMsgKickOutPlayer *)pData;
	//获取PDBID
    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActor = helper.m_ptr;
	if (pActor == NULL)
	{
		ErrorLn("MatchSceneService::onMsgKickOutPlayer() -- pActor == NULL");
		return;
	}
	//设置PDBID
	pMsg->idActor = pActor->getProperty_Integer(PROPERTY_ID);

	// 转发到社会服
	sendToSocial(MSG_MATCH_ROOM_MODULEMSG, CS_MSG_KICK_OUT_PLAYER, pMsg, sizeof(SMsgKickOutPlayer));
	
}

void MatchSceneService::onMsgExitHall(ClientID clientID, void* pData, size_t stLen)
{
	// 判断参数
	if (clientID == INVALID_CLIENT)
	{
		return;
	}

	SMsgExitHall pMsg;
	//获取PDBID
    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActor = helper.m_ptr;
	if (pActor == NULL)
	{
		ErrorLn("MatchSceneService::onMsgExitHall() -- pActor == NULL");
		return;
	}
	//设置PDBID
	pMsg.idActor = pActor->getProperty_Integer(PROPERTY_ID);

	// 转发到社会服
	sendToSocial(MSG_MATCH_ROOM_MODULEMSG, CS_MSG_EXIT_HALL, &pMsg, sizeof(SMsgExitHall));
}

void MatchSceneService::onMsgConfirmSelectHero(ClientID clientID, void* pData, size_t stLen)
{
    // 判断参数
	if (clientID == INVALID_CLIENT)
	{
        ErrorLn(__FUNCTION__": confirm select hero message occur error! client="<<clientID);
		return;
	}

	SMsgConfirmSelectHero pMsg;
	//获取PDBID
    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActor = helper.m_ptr;
	if (pActor == NULL)
	{
        ErrorLn(__FUNCTION__": confirm select hero message occur error! actor service is null, client="<<clientID);
		return;
	}
	//设置PDBID
	pMsg.idActor = pActor->getProperty_Integer(PROPERTY_ID);

	// 转发到社会服
	sendToSocial(MSG_MATCH_ROOM_MODULEMSG, CS_MSG_CONFIRM_SELECT_HERO, &pMsg, sizeof(SMsgConfirmSelectHero));
}


void MatchSceneService::onMsgMatchDateMessage(ClientID clientID, void* pData, size_t stLen)
{
	m_WarMatch.onMessage(clientID, pData, stLen);
}

void MatchSceneService::onMsgMatchServiceMessage(ClientID clientID, void* pData, size_t stLen)
{
    if(pData==NULL|| stLen < sizeof(SMsgMatchDataSubMsg))
    {
        return;
    }

    SMsgMatchDataSubMsg * pMsg = (SMsgMatchDataSubMsg *)(pData);

    LPSTR pMsgData =(LPSTR) pData + sizeof(SMsgMatchDataSubMsg);
    size_t nNewlen = stLen - sizeof(SMsgMatchDataSubMsg);

    switch(pMsg->dwMsgCode)
    {

    case CS_MSG_MATCH_ACTOR_VIEW_LOADED:
        {
            onMsgActorViewLoaded(clientID, pMsgData, nNewlen);
        }
        break;

    default:
        break;
    }
}


void MatchSceneService::onMsgRoomDateMessage(ClientID clientID, void* pData, size_t stLen)
{
    if(pData==NULL|| stLen < sizeof(SMsgMatchDataSubMsg))
    {
        return;
    }

    SMsgMatchDataSubMsg * pMsg = (SMsgMatchDataSubMsg *)(pData);

    LPSTR pMsgData =(LPSTR) pData + sizeof(SMsgMatchDataSubMsg);
    size_t nNewlen = stLen - sizeof(SMsgMatchDataSubMsg);

    switch(pMsg->dwMsgCode)
    {
        
    case CS_MSG_MATCH_CUSTOMER_INVITE_RESPON:
        {
            onMsgCustomerInviteRespon(clientID, pMsgData, nNewlen);
        }
        break;

    case CS_MSG_ENTER_HALL:
        {
            onMsgEnterHall(clientID, pMsgData, nNewlen);
        }
        break;
    case CS_MSG_CREATE_GUIDE_ROOM:
        {
            onMsgCreateGuideRoom(clientID, pMsgData, nNewlen);
        }
        break;

        // 进入大厅需要带一些数据过去， 
        // 下面这些消息是不是应该放匹配服处理，因为匹配服都有必要的数据了
    case CS_MSG_CREATE_ROOM:
        {
            onMsgCreateRoom(clientID, pMsgData, nNewlen);
        }
        break;

    case CS_MSG_ENTER_ROOM:
        {
            onMsgEnterRoom(clientID, pMsgData, nNewlen);
        }
        break;

    case CS_MSG_START_GAME:	// 开始游戏
        {
            onMsgStartGame(clientID, pMsgData, nNewlen);
        }
        break;

    case CS_MSG_MASTER_COVER_COMPUTER:	// 添加电脑开始游戏
        {
            onMsgCustomerAddComputer(clientID, pMsgData, nNewlen);
        }
        break;

    case CS_MSG_SELECT_HERO:	// 选英雄
        {
            onMsgSelectHero(clientID, pMsgData, nNewlen);
        }
        break;

    case CS_MSG_PRE_SELECT_HERO:	// 预选英雄
        {
            onMsgPreSelectHero(clientID, pMsgData, nNewlen);
        }
        break;

    case CS_MSG_POS_BAN_HERO:	// ban选英雄
        {
            onMsgPosBanHero(clientID, pMsgData, nNewlen);
        }
        break;

    case CS_MSG_READY_TO_FIGHT:	// 准备战斗
        {
            onMsgReadyToFight(clientID, pMsgData, nNewlen);
        }
        break;
	case CS_MSG_SELECT_HERO_SKIN:
		{
			onMsgSelectHeroSkin(clientID, pMsgData, nNewlen);
		}
		break;
    case CS_MSG_POS_CHANGE:	// 位置改变
        {
            onMsgPosChange(clientID, pMsgData, nNewlen);
        }
        break;
    case CS_MSG_EXIT_ROOM:	// 离开房间
        {
            onMsgExitRoom(clientID, pMsgData, nNewlen);
        }
        break;

    case CS_MSG_KICK_OUT_PLAYER:	// 踢人
        {
            onMsgKickOutPlayer(clientID, pMsgData, nNewlen);
        }
        break;

    case CS_MSG_EXIT_HALL:	// 离开大厅
        {
            onMsgExitHall(clientID, pMsgData, nNewlen);
        }
        break;

    case CS_MSG_CONFIRM_SELECT_HERO:	// 确定所选英雄 
        {
            onMsgConfirmSelectHero(clientID, pMsgData, nNewlen);
        }
        break;
    case CS_MSG_SUMMONER_SKILL:
        {
            onMsgSummonerTalentSkillMessage( clientID, pMsgData, nNewlen);
        }
        break;

    case CS_MSG_EXIT_WAR:	// 离开战场 
        {
            // 主动退出战场
            onMsgExitWar(clientID, pMsgData, nNewlen);
            
        }
        break;

    case CS_MSG_ROOM_COMPUTERPLAYER_ADD:             
        {
             // 添加假人消息 
            onMsgComputerPlayerAdd(clientID, pMsgData, nNewlen);
        }
        break;

    case CS_MSG_ROOM_COMPUTERPLAYER_REMOVE:               
        {
            // 移除假人
           onMsgComputerPlayerRemove(clientID, pMsgData, nNewlen);
        }
        break;

    case CS_MSG_ROOM_COMPUTERPLAYER_HEROCHANGE:               
        {
            // 假人英雄改变消息
            onMsgComputerPlayerHeroChange(clientID, pMsgData, nNewlen);     
        }
        break;
    case CS_MSG_ROOM_COMPUTERPLAYER_HEROAICHANGE:               
        {
            // 假人英雄改变消息
            onMsgComputerPlayerHeroAIChange(clientID, pMsgData, nNewlen);     
        }
        break;

    case CS_MSG_UPDATE_GUIDE_INFO:               
        {
            onMsgUpdateGuideInfo(clientID, pMsgData, nNewlen);     
        }
        break;
        
    default:
        {
            ErrorLn(__FUNCTION__": unknown pHead->byKeyAction=" << pMsg->dwMsgCode);
        }
    }
}


void MatchSceneService::onMsgSummonerTalentSkillMessage( ClientID clientID, void* pData, size_t stLen )
{
    // 判断参数
    if (clientID == INVALID_CLIENT || pData == NULL || stLen != sizeof(SMsgSummoner_Skill))
    {
        ErrorLn(__FUNCTION__": pData is Wrong");
        return;
    }

    SMsgSummoner_Skill* pMsgInfo = (SMsgSummoner_Skill*)pData;

    //获取PDBID
    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActor = helper.m_ptr;
    if (pActor == NULL)
    {
        ErrorLn("MatchSceneService::onMsgConfirmSelectHero() -- pActor == NULL");
        return;
    }
    //设置PDBID
    pMsgInfo->pDbid = pActor->getProperty_Integer(PROPERTY_ID);

	pActor->setActorSummonerOrGlory( pMsgInfo->nSummonerSkill, SUMMONER_SKILL_COUNT, ENPLAYERINFOTYPE_SUMMONER, 0);

    // 转发到社会服
    sendToSocial(MSG_MATCH_ROOM_MODULEMSG, SO_MSG_BROAD_SUMMONER, pMsgInfo, sizeof(SMsgSummoner_Skill));
}

// 房间内广播场景加载进度
void MatchSceneService::broadSceneLoadingProgress(int nWarID, PDBID idActor, int nProgress, long lPing)
{
    MatchSceneRoom* pRoom = NULL;
    if(!m_mapWarID2Room.find(nWarID, pRoom) || pRoom == NULL)
    {
        ErrorLn(__FUNCTION__ << ", war=" << nWarID << " not found");
        return;
    }

    rkt::obuf256 buf;
    SNetMsgHead head;
    head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
    head.byDestEndPoint= MSG_ENDPOINT_CLIENT;
    head.byKeyModule   = MSG_MODULEID_MIGRATE;
    head.byKeyAction   = MSG_SCENE_LOADING_PROGRESS_SC;

    SMsgSceneLoadingProgress_SC msg;
    msg.idActor = idActor;
    msg.nProgress = nProgress;
    msg.lPing = lPing;

    buf << head << msg;
    pRoom->broadcast2Client( buf.data(), buf.size());
}

void MatchSceneService::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    switch(wEventID)
    {
    case EVENT_GAME_ACTOR_LOGOUT:
        {
            if(pszContext == NULL || nLen != sizeof(event_actor_logout))
            {
                return;
            }
            onActorLogout((void*)pszContext);
        }
        break;
    case EVENT_GAME_ACTOR_LOGIN:
        {
            if(nLen != sizeof(event_actor_login))
            {
                return;
            }
            event_actor_login* pEvent = (event_actor_login *)pszContext;
            onActorLogin((void*)pszContext);
        }
        break;
    case EVENT_SYSTEM_SERVERINFOUPDATED:
        {
            onEventServerInfoUpdated(bSrcType, dwSrcID, pszContext, nLen);
        }
        break;
    case EVENT_CONTINUE_PLAY_CLIENT_INCOMMING:
        {
            onEventContinuePlayClientIncomming(pszContext, nLen);
        }
        break;
    default:
        break;
    }
}

void MatchSceneService::OnTimer(unsigned long dwTimerID)
{
    MatchSceneRoom* pRoom = NULL;
    if(!m_mapWarID2Room.find(dwTimerID, pRoom) || pRoom==NULL)
    {
        ErrorLn(__FUNCTION__ << ", war=" << dwTimerID << " not found");
        return;
    }

    // 有超时或者所有玩家都准备好则迁移
    if(  pRoom->checkPlayersReady() || pRoom->isLoadingSceneTimeOut())
    {
        
        gServerGlobal->getTimerService()->KillTimer(dwTimerID, this);

        // 响应定时器
        // todo.发现所有人都掉线了 直接销毁战场.
        createMobaActors(dwTimerID);

        //移除
        vector<int>::iterator iter = m_vecTimerID.begin();
        for (;iter != m_vecTimerID.end();++iter)
        {
            if (*iter == dwTimerID)
            {
                m_vecTimerID.erase(iter);
                break;
            }
        }

        //设置房间在战斗状态
        pRoom->setState(SRS_StateFighting);
    }
}

void MatchSceneService::setWarProcessEnd(int nWarID)
{
    MatchSceneRoom* pRoom = NULL;
    if (!m_mapWarID2Room.find(nWarID, pRoom) || pRoom==NULL)
    {
        ErrorLn(__FUNCTION__": pRoom = NULL nWarID="<<nWarID);
        return;
    }
    pRoom->setWarProcessEnd();
    obuf256 obMsg;
    SMsgWar_Process_End Data;
    Data.m_RoomID = pRoom->getRoomID();
    obMsg << Data;

    // 设置战场流程结束 通知社会服
    sendToSocial(MSG_MATCH_ROOM_MODULEMSG,SO_MSG_WAR_PROCESS_END, obMsg.data(), sizeof(SMsgWar_Process_End));
}


// 设置战场结果
void MatchSceneService::setWarResult(SMsgSetWarResultData resultData)
{
	MatchSceneRoom* pRoom = NULL;
	if (!m_mapWarID2Room.find(resultData.nWarID, pRoom) || pRoom==NULL)
	{
		return;
	}

	pRoom->sendWarResult2SocialLegendCup(resultData);
}

void MatchSceneService::onMsgComputerPlayerAdd(ClientID clientID, void* pData, size_t stLen)
{
    // 添加假人
    if (clientID == INVALID_CLIENT)
    {
        return;
    }

    if (pData == NULL || stLen != sizeof(SMsg_ComputerPlayerAdd))
    {
        ErrorLn(__FUNCTION__"SMsg_ComputerPlayerAdd MsgData is Wrong!");
        return;
    }

    SMsg_ComputerPlayerAdd *pMsg = (SMsg_ComputerPlayerAdd *)pData;
    //获取PDBID
    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActor = helper.m_ptr;
    if (pActor == NULL)
    {
        ErrorLn(__FUNCTION__": -- pActor == NULL");
        return;
    }
    //设置房主PDBID
    pMsg->idActor = pActor->getProperty_Integer(PROPERTY_ID);
    pMsg->dwSrcScene = pActor->getSceneID();
    pMsg->dwSrcWorldID = pActor->getFromGameWorldID();

    sendToSocial(MSG_MATCH_ROOM_MODULEMSG, CS_MSG_ROOM_COMPUTERPLAYER_ADD, pMsg, sizeof(SMsg_ComputerPlayerAdd));

}

void MatchSceneService::onMsgUpdateGuideInfo(ClientID clientID, void* pData, size_t stLen)
{
    // 添加假人
    if (clientID == INVALID_CLIENT)
    {
        return;
    }

    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActor = helper.m_ptr;
    if (pActor == NULL)
    {
        ErrorLn(__FUNCTION__": -- pActor == NULL");
        return;
    }

    if (pData == NULL || stLen != sizeof(SMsg_UpdateGuideInfo))
    {
        ErrorLn(__FUNCTION__"SMsg_ComputerPlayerAdd MsgData is Wrong!");
        return;
    }

    SMsg_UpdateGuideInfo *pMsg = (SMsg_UpdateGuideInfo *)pData;

    ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__" nullptr == pSchemeCenter");
        return;
    }
    ISchemeGuideStep * pGuideInfo = pSchemeCenter->getSchemeGuideStep();
    if (pGuideInfo== NULL)
    {
        return;
    }
    int nGuideStepID = pGuideInfo->getStepByID(pMsg->nGuidID);

    if (nGuideStepID == 0)
    {
        WarningLn(__FUNCTION__": nGuideStepID == 0 serch nGuidID = "<<pMsg->nGuidID);
    }

    pActor->saveGuideStep(nGuideStepID);

}

void MatchSceneService::onMsgComputerPlayerRemove(ClientID clientID, void* pData, size_t stLen)
{
    // 移除假人
    if (clientID == INVALID_CLIENT)
    {
        return;
    }

    if (pData == NULL || stLen != sizeof(SMsg_ComputerPlayerRemove))
    {
        ErrorLn(__FUNCTION__": SMsg_ComputerPlayerRemove MsgData is Wrong!");
        return;
    }

    SMsg_ComputerPlayerRemove *pMsg = (SMsg_ComputerPlayerRemove *)pData;
    //获取PDBID
    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActor = helper.m_ptr;
    if (pActor == NULL)
    {
        ErrorLn("MatchSceneService::onMsgConfirmSelectHero() -- pActor == NULL");
        return;
    }
    //设置PDBID
    pMsg->idActor = pActor->getProperty_Integer(PROPERTY_ID);

    // 转发到社会服
    sendToSocial(MSG_MATCH_ROOM_MODULEMSG, CS_MSG_ROOM_COMPUTERPLAYER_REMOVE, pMsg, sizeof(SMsg_ComputerPlayerRemove));
}

void MatchSceneService::onMsgComputerPlayerHeroChange(ClientID clientID, void* pData, size_t stLen)
{
    // 房主改变假人所选英雄
    if (clientID == INVALID_CLIENT)
    {
        return;
    }

    if (pData == NULL || stLen != sizeof(SMsg_ComputerPlayerHeroChange))
    {
        ErrorLn(__FUNCTION__": SMsg_ComputerPlayerHeroChange MsgData is Wrong!");
        return;
    }

    SMsg_ComputerPlayerHeroChange *pMsg = (SMsg_ComputerPlayerHeroChange *)pData;

    //获取PDBID
    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActor = helper.m_ptr;
    if (pActor == NULL)
    {
        ErrorLn(__FUNCTION__": pActor == NULL");
        return;
    }
    //设置PDBID
    pMsg->idActor = pActor->getProperty_Integer(PROPERTY_ID);

    // 转发到社会服
    sendToSocial(MSG_MATCH_ROOM_MODULEMSG, CS_MSG_ROOM_COMPUTERPLAYER_HEROCHANGE, pMsg, sizeof(SMsg_ComputerPlayerHeroChange));
}

void MatchSceneService::onMsgComputerPlayerHeroAIChange(ClientID clientID, void* pData, size_t stLen)
{
    // 房主改变假人所选英雄
    if (clientID == INVALID_CLIENT)
    {
        return;
    }

    if (pData == NULL || stLen != sizeof(SMsg_ComputerPlayerHeroAIChange))
    {
        ErrorLn(__FUNCTION__": SMsg_ComputerPlayerHeroAIChange MsgData is Wrong!");
        return;
    }

    SMsg_ComputerPlayerHeroAIChange *pMsg = (SMsg_ComputerPlayerHeroAIChange *)pData;

    //获取PDBID
    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActor = helper.m_ptr;
    if (pActor == NULL)
    {
        ErrorLn(__FUNCTION__": pActor == NULL");
        return;
    }
    //设置PDBID
    pMsg->idActor = pActor->getProperty_Integer(PROPERTY_ID);

    // 转发到社会服
    sendToSocial(MSG_MATCH_ROOM_MODULEMSG, CS_MSG_ROOM_COMPUTERPLAYER_HEROAICHANGE, pMsg, sizeof(SMsg_ComputerPlayerHeroAIChange));
}

void MatchSceneService::setRuneTalentPageIndex(PDBID idActor,int nRunePageIndex,int nTalentPageIndex)
{
    if (idActor == INVALID_PDBID)
    {
        return;
    }
    //获取PDBID
    ActorHelper helper(idActor, ACTOR_DBID);
    IActorService *pActor = helper.m_ptr;
    if (pActor == NULL)
    {
        ErrorLn(__FUNCTION__": pActor == NULL");
        return;
    }
    
    //设置PDBID
    SMsgSetTalnetAndRune MsgData;
    MsgData.PlayerID = idActor;
    MsgData.nTalentEffectPageIndex = nTalentPageIndex;                 // 当前选择天赋页
    MsgData.nRuneEffectPageIndex = nRunePageIndex;                   // 当前选择符文页
    // 转发到社会服
    sendToSocial(MSG_MATCH_ROOM_MODULEMSG, SO_MSG_SET_TALENT_AND_RUNE, &MsgData, sizeof(SMsgSetTalnetAndRune));
}

int MatchSceneService::getTalentPageSelectIndex(PDBID idActor)
{
    DWORD RoomID = getRoomIDByPDBID(idActor);
    if (RoomID == 0)
    {
        return 0;
    }
    MatchSceneRoom* pRoom = getRoomByID( RoomID );
    if (nullptr == pRoom)
    {
        WarningLn(__FUNCTION__": nullptr == pRoom RoomID=" << RoomID);
        return 0;
    }
    
    return pRoom->getTalentPageSelectIndex(idActor);
}

int MatchSceneService::getRunePageSelectIndex(PDBID idActor)
{
    DWORD RoomID = getRoomIDByPDBID(idActor);
    if (RoomID == 0)
    {
        return 0;
    }
    MatchSceneRoom* pRoom = getRoomByID( RoomID );
    if (nullptr == pRoom)
    {
        WarningLn(__FUNCTION__": nullptr == pRoom RoomID=" << RoomID);
        return 0;
    }

    return pRoom->getRunePageSelectIndex(idActor);
}

void MatchSceneService::onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len)
{
    MatchSceneHelper matchSceneHelper;
    IMatchSceneService *pMatchSceneService = matchSceneHelper.m_ptr;
    if (pMatchSceneService == NULL)
    {
        ErrorLn(__FUNCTION__": server ="<<server<<" uMsgID ="<<uMsgID);
        return;
    }

    PACKAGE_PTR pkg( new string((const char*)data,len));
    pMatchSceneService->handleServerMsg( server, *head, pkg );
}


void MatchSceneService::onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead* head, void* data, size_t len)
{

    MatchSceneHelper matchSceneHelper;
    IMatchSceneService *pMatchSceneService = matchSceneHelper.m_ptr;
    if (pMatchSceneService == NULL)
    {
        ErrorLn(__FUNCTION__": clientID ="<<clientID<<" uMsgID ="<<uMsgID);
        return;
    }

    PACKAGE_PTR pkg( new string((const char*)data,len));
    pMatchSceneService->handleClientMsg( clientID, *head, pkg );
}

void MatchSceneService::handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg)
{
    size_t len = msg->size();
    void *data = (void *)msg->c_str();
    switch ( head.byKeyAction )
    {
    case OS_MSG_MATCH_CREATE_MOBA:			// 创建MOBA战场
        {
            onMsgCreateMoba(data, len);
        }
        break;

    case OS_MSG_MATCH_CREATE_MOBA_ROLE_DATA:			// 房间人物信息
        {
            onMsgCreateMobaRoleData(data, len);
        }
        break;

    case OS_MSG_MATCH_CREATE_MOBA_OB_ROLE_DATA:			// 房间人物信息
        {
            onMsgCreateMobaOBRoleData(data, len);
        }
        break;

    case OS_MSG_MATCH_REQ_PLAYER_BANKINFO:	// 收集玩家的需要打包的信息
        {
            onMsgCollectionBankData(data, len);
        }
        break;

    case MSG_MATCH_DATE_MODULEMSG:
        {
            m_WarMatch.onTransmit(serverID, MSG_MATCH_DATE_MODULEMSG, &head, data, len);
        }
        break;

	case OS_MSG_MATCH_ROOM_EX_DATA:
		{
			onMsgMatchRoomExData(data, len);
		}
		break;

    case OS_MSG_MATCH_ROOM_BANHERO_DATA:
        {
            onMsgMatchRoomBanHeroData(data, len);
        }
        break;

    case SS_MSG_WAR_END_TESTER_EXIT_WAR:
        {
            onMsgTesterExitWar(data, len);
        }
        break;
	case OS_MSG_MATCH_SEASON_BEGIN:
		{
			onMsgMatchSeasonBegin(data, len);
		}
		break;
	case OS_MSG_MATCH_SEASON_END:
		{
			onMsgMatchSeasonEnd(data, len);
		}
		break;
    default:
        {
            ErrorLn("MatchSceneService::onMatchServerMsg unknown KeyAction=" << head.byKeyAction);
        }
        break;
    }
}


void MatchSceneService::onMsgTesterExitWar(void* pData, size_t stLen)
{
    if (stLen != sizeof(PDBID))
    {
        return;
    }

    PDBID* pActorID = (PDBID*)pData;
    DWORD idActor = *pActorID;
    // 压力测试玩家主动退出
    accordLeaveWar(idActor);
}

void MatchSceneService::handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg)
{
    size_t len = msg->size();
    void *data = (void *)msg->c_str();
    switch ( head.byKeyAction )
    {
    case MSG_MATCH_ROOM_MODULEMSG:      // 房间消息
        {
            onMsgRoomDateMessage(client, data, len);
        }
        break;

    case MSG_MATCH_DATE_MODULEMSG:      // 撮合消息
        {
            onMsgMatchDateMessage(client, data, len);
        }
        break;

    case MSG_MATCH_SERVICE_MODULEMSG:   // 模块消息
        {
            onMsgMatchServiceMessage(client, data, len);
        }
        break;
        
    default:
        {
            MATCH_ERRORLN(__FUNCTION__" unknown pHead->byKeyAction=" << head.byKeyAction);
        }
        break;
    }
}

void MatchSceneService::onServerListUpdated()
{

}

void MatchSceneService::onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData)
{

}

void MatchSceneService::onEventServerInfoUpdated(BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    if(nLen != sizeof(event_systemserverinfo_updated_s))
    {
        return;
    }
    event_systemserverinfo_updated_s * pEvent = (event_systemserverinfo_updated_s *)(pszContext);

    ServerData * pServerData = (ServerData *)pEvent->pInfo;
    if(pEvent->nState != SERVER_CONNECTED || pServerData->wServerType != MSG_ENDPOINT_SOCIAL)
    {
        return;
    }

    dataRestoreToSocialSvr();
}

void MatchSceneService::onEventContinuePlayClientIncomming(LPCSTR pszContext, int nLen)
{
    if(nLen < sizeof(event_continue_play_client_incomming))
    {
        return;
    }

    event_continue_play_client_incomming* pEvent = (event_continue_play_client_incomming*)pszContext;
   
    DWORD nRoomID = getRoomIDByPDBID(pEvent->dwDBID);
    if (nRoomID == 0)
    {
        WarningLn(__FUNCTION__": nullptr == pRoom RoomID == 0");
        return;
    }
    MatchSceneRoom* pRoom = getRoomByID( nRoomID );
    if (pRoom == NULL)
    {
        WarningLn(__FUNCTION__": nullptr == pRoom RoomID=" << nRoomID);
        return;
    }
    MatchScenePlayer* pScenePlayer = pRoom->isOBPlayer(pEvent->dwDBID)? pRoom->findOBPlayer(pEvent->dwDBID): pRoom->findPlayer(pEvent->dwDBID); 
    if (pScenePlayer == NULL)
    {
        ErrorLn(__FUNCTION__": pScenePlayer == NULL ActorID ="<<pEvent->dwDBID);
        return;
    }

    // 先让客户端切到游戏状态 才能收到下面的消息
    obuf buf;
    SNetMsgHead head;
    head.bySrcEndPoint	= MSG_ENDPOINT_SCENE;
    head.byDestEndPoint	= MSG_ENDPOINT_CLIENT;
    head.byKeyModule = MSG_MODULEID_MIGRATE;
    head.byKeyAction = MSG_NOTIFY_CLIENT_SWITCH_STATE;
    buf << head;
    SMsgNotifySwitchState_SC msgSwitchState;
    msgSwitchState.nNextState = ESS_GAME;
    buf << msgSwitchState;
    PACKAGE_PTR pkg2( new string(buf.data(),buf.size()) );
    gServerGlobal->getGatewayAcceptorService()->sendData(pEvent->client, pkg2);

    // 给客户端发房间基本信息
    buf.clear();
    head.bySrcEndPoint	= MSG_ENDPOINT_SCENE;
    head.byDestEndPoint	= MSG_ENDPOINT_CLIENT;
    head.byKeyModule = MSG_MODULEID_MATCH;
    head.byKeyAction		= OC_MSG_ROOM;
    buf << head << pRoom->m_msg;
    PACKAGE_PTR pkg( new string(buf.data(),buf.size()) );
    gServerGlobal->getGatewayAcceptorService()->sendData(pEvent->client, pkg);

    // 给客户端发房间详细信息
    buf.clear();
    head.bySrcEndPoint	= MSG_ENDPOINT_SCENE;
    head.byDestEndPoint	= MSG_ENDPOINT_CLIENT;
    head.byKeyModule = MSG_MODULEID_MATCH;
    head.byKeyAction		= OC_MSG_ENTER_ROOM;

    buf << head;

    SRoomRoleBase RoomRoleBase;
    SMsgSelfEnterRoomData msgEx;
    msgEx.m_Result = true;
    msgEx.m_RoomID		= nRoomID;
    msgEx.m_MasterPos	= 1;
    msgEx.m_RoleCount	= pScenePlayer->m_role.bIsOBPos? pRoom->getAllPlayerCount(): pRoom->getAllPlayerCount() - 1;
    msgEx.m_SelfPos		= pScenePlayer->m_role.nIndex;
    msgEx.m_PDBID       = pScenePlayer->m_role.dwPDBID;
    msgEx.m_SelfHeroID  = pScenePlayer->m_role.dwHeroID;
    msgEx.m_bySex       = pScenePlayer->m_role.bySex;
    msgEx.m_nHeroXPSkill = pScenePlayer->m_role.nHeroXPSkillID;
    msgEx.m_bIsOBEnter  = pScenePlayer->m_role.bIsOBPos;
    sstrcpyn(msgEx.m_szName, pScenePlayer->m_role.szName, sizeof(msgEx.m_szName));
    memcpy(msgEx.m_SummonerSKill, pScenePlayer->m_role.nSummonserSkill, sizeof(msgEx.m_SummonerSKill));
    buf << msgEx;

    for(int i = 0; i < pRoom->getAllPlayerCount(); ++i)
    {
        MatchScenePlayer* pMatchPlayer = pRoom->m_arrayPlayer + i;
        if(pMatchPlayer->m_role.dwPDBID == pScenePlayer->m_role.dwPDBID)
        {
            continue;
        }

        buf << (SRoomRoleBase)pMatchPlayer->m_role;
    }

    PACKAGE_PTR pkg1( new string(buf.data(),buf.size()) );
    gServerGlobal->getGatewayAcceptorService()->sendData(pEvent->client, pkg1);

    // 通知客户端进入战场
    head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
    head.byDestEndPoint= MSG_ENDPOINT_CLIENT;
    head.byKeyModule   = MSG_MODULEID_MIGRATE;
    head.byKeyAction   = MSG_SWITCH_SCENE_SC;

    SMsgSwitchScene_SC msg;
    msg.idActor = pEvent->dwDBID;
    msg.nSceneID = pRoom->m_nSceneID;
    msg.nMapID = gServerGlobal->getSceneManager()->sceneIDToMapID(msg.nSceneID); 
    ActorHelper helper(pEvent->dwDBID, ACTOR_DBID);
    if(helper.m_ptr != 0)
    {
        msg.vecBornPos = helper.m_ptr->getLocation();
    }
    msg.isContinuePlay = true;

    buf.clear();
    buf << head << msg;
    PACKAGE_PTR package(new string(buf.data(),buf.size()));
    gServerGlobal->getGatewayAcceptorService()->sendData(pEvent->client, package);
}

void MatchSceneService::dataRestoreToSocialSvr()
{
    ICenterConnectorService * pConnector = gServerGlobal->getCenterConnectorService();
    if(pConnector == NULL)
    {
	    ErrorLn( __FUNCTION__ ": CenterConnectorService==NULL");
	    return;
    }

    DWORD dwServerID = pConnector->getLocalServerCSID();
    obuf obRoomData;
    SMsgMatchRoomDataRestoreHead sendHead;
    for (MAP_Rooms::iterator iter = m_RoomID2Room.begin(); iter != m_RoomID2Room.end(); ++iter)
    {
        MatchSceneRoom *pSceneRoom = NULL;
        if(!m_RoomID2Room.find(iter->first, pSceneRoom) || pSceneRoom == NULL)
        {
            ErrorLn(__FUNCTION__ << " room not found,room id=" << iter->first);
            continue;
        }

        // 已经结束的就不要还原了
        if (pSceneRoom->getWarProcessEnd() == true)
        {
            continue;
        }
        
        DWORD dwPlayerCount = pSceneRoom->getAllPlayerCount();
        // 整体包数据大小判定
        if(sizeof(SMsgMatchRoomDataRestoreHead) + obRoomData.size() + sizeof(SMsgMatchRoomDataRestore) + dwPlayerCount*sizeof(MatchScenePlayer)  > 8192)
        {
            // 组合包数据 发送数据包一次
            obuf ob;
            sendHead.bHaveSendAll = false;
            ob.push_back(&sendHead, sizeof(SMsgMatchRoomDataRestoreHead));
            ob.push_back(obRoomData.data(), obRoomData.size());

            // 转发到社会服
            sendToSocial(MSG_MATCH_SERVICE_MODULEMSG, MSG_MATCHROOM_DATARESTORE, ob.data(), ob.size());
	        //pConnector->sendToSocial(MSG_MODULEID_MATCH, MSG_MATCHROOM_DATARESTORE, ob.data(), ob.size());

            // 清空组包数据
            obRoomData.clear();
            sendHead.nCount =0;
            sendHead.bHaveSendAll =false;
            sendHead.dwTotalPlayerCount = 0;
        }
        
        // 还原需要的数据 每个需要还原的房间数据
        SMsgMatchRoomDataRestore msgrestore;
        pSceneRoom->getCreateRoomInfo(msgrestore);
        msgrestore.dwPlayerCount = dwPlayerCount;
        // 获取成员数据
        obuf obPlayerData;
        obRoomData.push_back(&msgrestore,  sizeof(msgrestore));
        pSceneRoom->getRoomAllMatchScenePlayer(obPlayerData);
        obRoomData.push_back(obPlayerData.data(), obPlayerData.size());
        sendHead.nCount++;
        sendHead.dwTotalPlayerCount += dwPlayerCount;

        // 输出本场景每个还原房间的信息
        EmphasisLn(__FUNCTION__":serverID="<<dwServerID<<"RoomID="<<pSceneRoom->getRoomID()<<"dwPlayerCount="<<dwPlayerCount);
    }

    obuf ob;
    sendHead.bHaveSendAll = true;
    ob.push_back(&sendHead, sizeof(SMsgMatchRoomDataRestoreHead));
    ob.push_back(obRoomData.data(), obRoomData.size());

    sendToSocial(MSG_MATCH_SERVICE_MODULEMSG, MSG_MATCHROOM_DATARESTORE, ob.data(), ob.size());
	//pConnector->sendToSocial(MSG_MODULEID_MATCH, MSG_MATCHROOM_DATARESTORE, ob.data(), ob.size());
}

void MatchSceneService::onMsgExitWar(ClientID clientID, void* pData, size_t stLen)
{
    if (NULL == clientID)
    {
        return;
    }

    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActorService = helper.m_ptr;
    if(pActorService == NULL)
    {
        ErrorLn(__FUNCTION__" pActorService == NULL");
        return;
    }
    DWORD dwActorID = pActorService->getProperty_Integer(PROPERTY_ID);
    bool  bOBServer = pActorService->getProperty_Integer(PROPERTY_OBSERVER) == 1;
    // 更新对应的Client
    bool bCanExit = true;
    MatchSceneRoom* pRoom = nullptr;
    MatchScenePlayer* pScenePlayer = NULL;
    DWORD RoomID = 0;
    do 
    {
        RoomID = getRoomIDByPDBID(dwActorID);
        if (RoomID == 0)
        {
            WarningLn(__FUNCTION__": nullptr == pRoom RoomID == 0 idActor ="<<dwActorID);
            bCanExit = false;
            break;
        }
        pRoom = getRoomByID( RoomID );
        if (nullptr == pRoom)
        {
            WarningLn(__FUNCTION__": nullptr == pRoom RoomID=" << RoomID<<"idActor="<<dwActorID);
            bCanExit = false;
            break;
        }

        pScenePlayer = bOBServer == true? pRoom->findOBPlayer(dwActorID): pRoom->findPlayer(dwActorID);
        if (pScenePlayer ==NULL)
        {
            WarningLn(__FUNCTION__": pScenePlayer ==NULL idActor=" << dwActorID);
            bCanExit = false;
            break;
        }
    } while (false);

    SSceneNode mainScene;
    if (!bCanExit)
    {
        IMigrateService* pMigrate = (IMigrateService*)gServerGlobal->getMigrateService();
        if (!pMigrate)
        {
            ErrorLn(__FUNCTION__": pMigrate == NULL idActor ="<<dwActorID);
            return;
        }
            
        // 获取合适的主城场景
        if (!pMigrate->getAvailableScene(emaptype_main, 1, &mainScene))
        {
            ErrorLn(__FUNCTION__": pMigrate == NULL idActor ="<<dwActorID);
            return;
        }
    }
    else
    {
        // 推荐回到的主城场景
        if (isPublicGameWorld())
        {
            mainScene.nWorldID = pScenePlayer->m_role.nWorldID;
            mainScene.nZoneServerID = pScenePlayer->m_role.nZoneServerID;
            mainScene.nSceneID = pScenePlayer->m_role.nSceneID;
            mainScene.nSceneSN = pScenePlayer->m_role.nSceneSN;
        }
        else
        {
            mainScene = pRoom->getRecommandScene();
            if (mainScene.nSceneID <= 0)
            {
                ErrorLn(__FUNCTION__":switchScene, mainScene.nSceneID <= 0, RoomID=" << RoomID<<",idActor="<<dwActorID <<",nDestSceneID=" << mainScene.nSceneID);
                return;
            }
        }
    }

#ifdef __SWITCH_SCENE_PRINT__
	TraceLn(__FUNCTION__":switchScene, RoomID=" << RoomID << ",idActor=" << dwActorID << ",nDestWorldID=" 
		<< mainScene.nWorldID << ",nDestServerID="  << mainScene.nZoneServerID << ",nDestSceneID=" << mainScene.nSceneID);
#endif

	// 迁出
    SNetMsgHead head;
    head.bySrcEndPoint	= MSG_ENDPOINT_SCENE;
    head.byDestEndPoint	= MSG_ENDPOINT_SCENE;
    head.byKeyModule	= MSG_MODULEID_MIGRATE;
    head.byKeyAction	= MSG_REQ_SWITCH_SCENE_XS;

    SMsgReqSwitchScene_XS msg;
    msg.idActor = dwActorID;
    msg.destScene.nWorldID = mainScene.nWorldID;
    msg.destScene.nZoneServerID = mainScene.nZoneServerID;
    msg.destScene.nSceneID = mainScene.nSceneID;
    msg.destScene.nSceneSN = mainScene.nSceneSN;
    msg.eReason = eswitchscene_out_battlefield;

    obuf256 buf;
    buf << head << msg; 
    gServerGlobal->getCenterConnectorService()->postMessage(gServerGlobal->getCenterConnectorService()->getLocalServerCSID(), buf.data(), buf.size());
}

void MatchSceneService::onUpdateRoomPlayerClient( PDBID idActor, ClientID client )
{
    // 掉线或者重连会走这里
    EmphasisLn(__FUNCTION__": idActor="<<idActor<<" client="<<client);
    // 更新对应的Client
    DWORD dwRoomID = getRoomIDByPDBID(idActor);
    if (dwRoomID == 0)
    {
        WarningLn(__FUNCTION__": nullptr == pRoom RoomID == 0 idActor ="<<idActor);
        return;
    }
    MatchSceneRoom* pRoom = getRoomByID( dwRoomID );
    if (nullptr == pRoom)
    {
        WarningLn(__FUNCTION__": nullptr == pRoom RoomID=" << dwRoomID<<" idActor ="<<idActor);
        return;
    }

    
    MatchScenePlayer* pScenePlayer = pRoom->isOBPlayer(idActor)? pRoom->findOBPlayer(idActor): pRoom->findPlayer(idActor);
    if (pScenePlayer ==NULL)
    {
        WarningLn(__FUNCTION__": pScenePlayer ==NULL idActor=" << idActor);
        return;
    }

    pScenePlayer->m_role.clientID = client;
    notifySocialActorLogined(client, idActor);
    if (pScenePlayer->m_role.clientID == INVALID_CLIENT)
    {

        // 这里只设置玩家的clientID为INVALID_CLIENT 所有玩家都是INVALID_CLIENT 销毁所有玩家
        warEndPersonLeaveCheck( idActor, dwRoomID);
    }
    else
    {
        pRoom->clearAutoEnd();
    }
}


void MatchSceneService::warEndPersonLeaveCheck(DWORD idActor, DWORD dwRoomID)
{
    MatchSceneRoom* pRoom = getRoomByID( dwRoomID );
    if (nullptr == pRoom)
    {
        WarningLn(__FUNCTION__": nullptr == pRoom RoomID=" << dwRoomID<<" idActor ="<<idActor);
        return;
    }
    if (pRoom->getWarProcessEnd())
    {
        // 已经结束移除玩家实体
        pRoom->isOBPlayer(idActor)? pRoom->destoryOBPlayer(idActor): pRoom->destoryPlayer(idActor);  
    }
    else
    {
        // 战场状态下真人玩家都离开了
        if(pRoom->getAllPlayerCount() == pRoom->getClientInvalidCount() && pRoom->allOBPlayerIsInValid())
        {
            // 启动延迟关闭战场时间
            pRoom->setAutoEnd();
        }
    }
    // 加载过程中真人玩家都离开了 释放战场
    //if (pRoom->getState() == SRS_StateWaitLoading && pRoom->getAllPlayerCount() == pRoom->getClientInvalidCount() && pRoom->allOBPlayerIsInValid())
    //{
    //    // 定时器移除
    //    gServerGlobal->getTimerService()->KillTimer(pRoom->m_nWarID, this);

    //    vector<int>::iterator iter = m_vecTimerID.begin();
    //    for (;iter != m_vecTimerID.end();++iter)
    //    {
    //        if (*iter == pRoom->m_nWarID)
    //        {
    //            m_vecTimerID.erase(iter);
    //            break;
    //        }
    //    }

    //    // 移除玩家所有对应关系
    //    MATCH_TRACELN(__FUNCTION__": removePDBID2RoomID() PDBID= "<<idActor<<" dwRoomID="<<dwRoomID);
    //    removePDBID2RoomID(idActor, dwRoomID);

    //    // 通知社会服删除对应房间关系
    //    SMsgRemoveActorRoomInfo_SO msg;
    //    msg.dwRoomID = dwRoomID;					// 房间ID 
    //    msg.dwActorID = idActor;                    // 玩家ID

    //    // 然后通知客户端加载场景
    //    sendToSocial(MSG_MATCH_ROOM_MODULEMSG, SO_MSG_MATCH_REMOVE_ACTOR_ROOMINFO, &msg, sizeof(msg) );

    //    // 销毁战场
    //    EmphasisLn(__FUNCTION__": DestroyWar! nScene = "<<pRoom->m_nSceneID<<" WarID = "<<pRoom->m_nWarID);
    //    gServerGlobal->getWarManager()->DestroyWar(pRoom->m_nWarID);
    //}
    //else
    //{
        //// 战场状态下真人玩家都离开了
        //if(pRoom->getAllPlayerCount() == pRoom->getClientInvalidCount() && pRoom->allOBPlayerIsInValid())
        //{
        //    // 启动延迟关闭战场时间
        //    pRoom->setAutoEnd();
        //}
    //}
}


// 通知社会服玩家上线
void MatchSceneService::notifySocialActorLogined(ClientID client, DWORD dwDBID)
{
    obuf buf;
    buf << dwDBID << client;

    ICenterConnectorService* pCenterConnectorService =  gServerGlobal->getCenterConnectorService();
    if(pCenterConnectorService == NULL)
    {
        return;
    }

    sendToSocial(MSG_MATCH_SERVICE_MODULEMSG, MSG_MATCH_ACTOR_LOGIN_OK, buf.data(), buf.size());
}

bool MatchSceneService::isActorInBattleField( PDBID idActor )
{
    DWORD RoomID = getRoomIDByPDBID(idActor);
    if (RoomID == 0)
    {
        ErrorLn(__FUNCTION__": RoomID == 0 idActor="<<idActor)
        return false;
    }

    MatchSceneRoom* pRoom = getRoomByID( RoomID );
    if (nullptr == pRoom)
    {
        WarningLn(__FUNCTION__": nullptr == pRoom RoomID=" << RoomID);
        return false;
    }

    return true;
}

// 获取角色的房间ID
int MatchSceneService::getRoomID(PDBID idActor)
{
    return getRoomIDByPDBID(idActor); 
}

void MatchSceneService::onMsgCollectionBankData(void* pData, size_t nLen)
{
    if (pData == NULL || nLen < sizeof(SMsgMatchReqPlayerBankInfo_OS))
    {
        ErrorLn(__FUNCTION__ ": the message is invalid, size="<< nLen);
        return;
    }

    SMsgMatchReqPlayerBankInfo_OS* pMsg = (SMsgMatchReqPlayerBankInfo_OS*) pData;

    // 打包BankPart数据
    PDBID ActorID = pMsg->dwActorID;
    int   nHeroID = pMsg->nHeroID;

    ActorHelper helper(ActorID, ACTOR_DBID);
    IActorService *pActorService = helper.m_ptr;
    if(pActorService == NULL)
    {
        ErrorLn(__FUNCTION__" pActorService == NULL");
        return;
    }

    // 打包房间的数据
    obuf bufData;
    SMsgMatchRetPlayerBankInfo_SO retMsg;
    retMsg.dwActorID = ActorID;
    retMsg.nHeroID = nHeroID;
    retMsg.nHeroXPID = pActorService->getXPSKillID(nHeroID);
    bufData.push_back(&retMsg,sizeof(retMsg));

    string strToHeroData = pActorService->exportActorData(ACTOR_TO_HERO);
    bufData.push_back( strToHeroData.c_str(), strToHeroData.size());

    // 回复打包数据
    sendToSocial(MSG_MATCH_ROOM_MODULEMSG, SO_MSG_MATCH_RET_PLAYER_BANKINFO, bufData.data(), bufData.size());

}

void MatchSceneService::warEndDestoryInvalidActor(int nWarID)
{
    MatchSceneRoom* pRoom = NULL;
    if (!m_mapWarID2Room.find(nWarID, pRoom) || pRoom==NULL)
    {
        ErrorLn(__FUNCTION__": pRoom = NULL nWarID="<<nWarID);
        return;
    }
    pRoom->warEndDestoryInvalidActor();
}

void MatchSceneService::accordLeaveWar(PDBID idActor)
{
    DWORD RoomID = getRoomIDByPDBID(idActor);
    if (RoomID == 0)
    {
        ErrorLn(__FUNCTION__": RoomID == 0 idActor="<<idActor)
        return;
    }
    bool bFindMainScene = true;
    MatchSceneRoom* pRoom = getRoomByID( RoomID );
    do 
    {
        if (pRoom == nullptr)
        {
            bFindMainScene = false;
            break;
        }

        SSceneNode mainScene = pRoom->getRecommandScene();
        if (mainScene.nSceneID <= 0)
        {
            bFindMainScene = false;
            break;
        }
    } while (false);

    // 有推荐回到的主城场景
    if (bFindMainScene)
    {
        pRoom->accordLeaveWar(idActor);
    }
    else
    {
        // 找不到房间信息了 直接退出到默认的查找场景里面去
        IMigrateService* pMigrate = (IMigrateService*)gServerGlobal->getMigrateService();
        if (!pMigrate)
        {
            ErrorLn(__FUNCTION__": pMigrate == NULL idActor ="<<idActor);
            return;
        }

        // 获取合适的主城场景
        SSceneNode mainScene;
        if (!pMigrate->getAvailableScene(emaptype_main, 1, &mainScene))
        {
            ErrorLn(__FUNCTION__": pMigrate == NULL idActor ="<<idActor);
            return;
        }

        // 迁出
        SNetMsgHead head;
        head.bySrcEndPoint	= MSG_ENDPOINT_SCENE;
        head.byDestEndPoint	= MSG_ENDPOINT_SCENE;
        head.byKeyModule	= MSG_MODULEID_MIGRATE;
        head.byKeyAction	= MSG_REQ_SWITCH_SCENE_XS;

        SMsgReqSwitchScene_XS msg;
        msg.idActor = idActor;
        msg.destScene.nWorldID = mainScene.nWorldID;
        msg.destScene.nZoneServerID = mainScene.nZoneServerID;
        msg.destScene.nSceneID = mainScene.nSceneID;
        msg.destScene.nSceneSN = mainScene.nSceneSN;
        msg.eReason = eswitchscene_out_battlefield;

        obuf256 buf;
        buf << head << msg; 
        gServerGlobal->getCenterConnectorService()->postMessage(gServerGlobal->getCenterConnectorService()->getLocalServerCSID(), buf.data(), buf.size());
    }
}

// 开启赛季
void MatchSceneService::onMsgMatchSeasonBegin(void* pData, size_t stLen)
{
	if (pData == NULL || stLen < sizeof(SMsgMatchSeasonBegin_OS))
	{
		ErrorLn(__FUNCTION__ ": the message is invalid, size="<< stLen);
		return;
	}

	SMsgMatchSeasonBegin_OS* pMsg = (SMsgMatchSeasonBegin_OS*) pData;

	event_match_season_begin eventData;
	eventData.nBeginSeasonIndex = pMsg->nBeginSeasonIndex;

	IEventEngine * pEventEngine = gServerGlobal->getEventEngine();
	if(pEventEngine == nullptr)
	{
		ErrorLn(__FUNCTION__": pEventEngine == nullptr");
		return;
	}

	pEventEngine->FireExecute(EVENT_MATCH_RANK_SEASON_BEGIN, SOURCE_TYPE_SYSTEM, 0, (LPCSTR)&eventData, sizeof(eventData));
}

// 结束赛季
void MatchSceneService::onMsgMatchSeasonEnd(void* pData, size_t stLen)
{
	if (pData == NULL || stLen < sizeof(SMsgMatchSeasonEnd_OS))
	{
		ErrorLn(__FUNCTION__ ": the message is invalid, size="<< stLen);
		return;
	}

	SMsgMatchSeasonEnd_OS* pMsg = (SMsgMatchSeasonEnd_OS*) pData;

	event_match_season_end eventData;
	eventData.nEndSeasonIndex = pMsg->nEndSeasonIndex;

	IEventEngine * pEventEngine = gServerGlobal->getEventEngine();
	if(pEventEngine == nullptr)
	{
		ErrorLn(__FUNCTION__": pEventEngine == nullptr");
		return;
	}

	pEventEngine->FireExecute(EVENT_MATCH_RANK_SEASON_END, SOURCE_TYPE_SYSTEM, 0, (LPCSTR)&eventData, sizeof(eventData));
}
