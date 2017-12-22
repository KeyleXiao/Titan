#include "stdafx.h"
#include "ISchemeCenter.h"
#include "MatchSceneRoom.h"
#include "MatchSceneService.h"
#include "IServerGlobal.h"
#include "IFramework.h"
#include "IWarManager.h"
#include "IWarService.h"
#include "IGatewayAcceptorService.h"
#include "EntityHelper.h"
#include "IServerGlobal.h"
#include "IMigrateService.h"
#include "Property.h"
#include "IDBEngineService.h"
#include "LegendCupMsgDef.h"
#include "Simple_Atom_Lock.h"
#include "MathHelper.h"
#include "HeightFieldMgr.h"

#if MatchScene_DisableShowDebug
#undef MATCH_TRACELN
#define MATCH_TRACELN(x)
#endif


MatchSceneRoom::MatchSceneRoom()
	: m_pService(NULL)
	, m_eRoomState(SRS_StateUnknown)
	, m_nWarID(INVALID_WAR_ID)
    , m_nSceneID(INVALID_SCENE_ID)
    , m_mutex(0)
    , m_dwComputerCount(0)
    , m_dwPlayerCount(0)
    , m_bWarProcessEnd(false)
    , m_dwStartMigrateTime(0)
    , m_bAllActorOut(false)
    , m_bAutoEnd(false)
    , m_arrayPlayer(NULL)
    , m_arrayOBPlayer(NULL)
	, m_nMaxRoleIndex(0)

{
    memset(m_nSummonerSlot, 0, sizeof(m_nSummonerSlot));
    setSummonerSkillSlot();
}

void MatchSceneRoom::initalPlayer(SRoomRoleBase* pFirstRole)
{
    BYTE byRoleCount = m_msg.byRoleCount;
    if (byRoleCount == 0)
    {
        WarningLn( __FUNCTION__ ": role count is zero! m_nWarID="<<m_nWarID);
        return;
    }

    DWORD dwRoomID = getRoomID();
    // 记录房间的角色数据
    TraceLn( "------------------------"<<"roomid="<<getRoomID()<<", WarID="<< m_nWarID <<"-------------------------");
    SRoomRoleBase* pRole = (SRoomRoleBase*)(pFirstRole);
    for (int i = 0; i < byRoleCount; ++i)
    {
        char szBuf[256] = {0};
        int* pCollectBufSize = (int*)(pRole + 1);
        char* pCollectBuf = (char *)(pCollectBufSize + 1);
        ssprintf_s(szBuf, sizeof(szBuf), "%s,pdbid=%d,hero=%d,clientID=%d,index=%d,computer=%d,BuffSize=%d", pRole->szName, pRole->dwPDBID, pRole->dwHeroID, pRole->clientID, pRole->nIndex, pRole->isComputerPlayer, (*pCollectBufSize) );
        EmphasisLn(szBuf);
        MatchScenePlayer & player = m_arrayPlayer[i];
        memcpy(&(player.m_role), pRole, sizeof(player.m_role));
        player.m_role.clientID = INVALID_CLIENT;
        memcpy(player.m_SummonerSlot, m_nSummonerSlot, sizeof(player.m_SummonerSlot));        
        m_pService->addPDBID2RoomID(player.m_role.dwPDBID, dwRoomID);

        // 统计数量
        player.m_role.isComputerPlayer ? ++m_dwComputerCount : ++m_dwPlayerCount;

        // 动态分配一块内存
        player.m_nCollectDataSize = *pCollectBufSize;
        player.m_pCollectionRoomData = (char *)malloc(player.m_nCollectDataSize);
        memcpy(player.m_pCollectionRoomData, pCollectBuf, player.m_nCollectDataSize);

        pRole = (SRoomRoleBase*)(pCollectBuf + (*pCollectBufSize));

		// 计算最大下标
		if (player.m_role.nIndex > m_nMaxRoleIndex)
		{
			m_nMaxRoleIndex = player.m_role.nIndex;
		}
    }
    TraceLn( "-----------------------------------PlayerCount["<<m_dwPlayerCount<<"],ComputerCount["<<m_dwComputerCount<<"]------------------------------------------");
}

void MatchSceneRoom::initalOBPlayer(SRoomRoleBase* pFirstRole)
{
    BYTE byRoleCount = m_msg.byOBRoleCount;
    if (byRoleCount == 0)
    {
        TraceLn( __FUNCTION__ ": OBRole count is zero! nWarID="<<m_nWarID);
        return;
    }

    DWORD dwRoomID = getRoomID();
    // 记录房间的角色数据
    TraceLn( "------------------------"<<"roomid="<<getRoomID()<<", WarID="<< m_nWarID <<"-------------------------");
    SRoomRoleBase* pRole = (SRoomRoleBase*)(pFirstRole);
    for (int i = 0; i < byRoleCount; ++i)
    {
        char szBuf[256] = {0};
        int* pCollectBufSize = (int*)(pRole + 1);
        char* pCollectBuf = (char *)(pCollectBufSize + 1);
        ssprintf_s(szBuf, sizeof(szBuf), "%s,pdbid=%d,hero=%d,clientID=%d,index=%d,computer=%d,BuffSize=%d", pRole->szName, pRole->dwPDBID, pRole->dwHeroID, pRole->clientID, pRole->nIndex, pRole->isComputerPlayer, (*pCollectBufSize) );
        EmphasisLn(szBuf);
        MatchScenePlayer & player = m_arrayOBPlayer[i];
        memcpy(&(player.m_role), pRole, sizeof(player.m_role));
        player.m_role.clientID = INVALID_CLIENT;       
        m_pService->addPDBID2RoomID(player.m_role.dwPDBID, dwRoomID);

        // 动态分配一块内存
        player.m_nCollectDataSize = *pCollectBufSize;
        player.m_pCollectionRoomData = (char *)malloc(player.m_nCollectDataSize);
        memcpy(player.m_pCollectionRoomData, pCollectBuf, player.m_nCollectDataSize);

        pRole = (SRoomRoleBase*)(pCollectBuf + (*pCollectBufSize));

    }
    TraceLn( "-----------------------------------------------------------------------------");
}

void MatchSceneRoom::inital( MatchSceneService* pMatchSceneService, SMsgCreateRoom_OC* pCreateRoom, int nWarID )
{
	EmphasisLn( __FUNCTION__ ": roomid="<< pCreateRoom->dwRoomID<<" nWarID="<< nWarID);
	
	m_pService	    = pMatchSceneService;
	m_nWarID	    = nWarID;
	m_msg		    = *pCreateRoom;
	m_eRoomState    = SRS_StateUnknown;

    if (pCreateRoom->byOBRoleCount > 0)
    {
        int nOBMemSize = pCreateRoom->byOBRoleCount * sizeof(MatchScenePlayer);
        m_arrayOBPlayer = (MatchScenePlayer*)malloc( nOBMemSize );
        memset(m_arrayOBPlayer, 0, nOBMemSize);
    }
    
    if (pCreateRoom->byRoleCount > 0)
    {
        int nMemSize = pCreateRoom->byRoleCount * sizeof(MatchScenePlayer);
        m_arrayPlayer = (MatchScenePlayer*)malloc( nMemSize );
        memset(m_arrayPlayer, 0, nMemSize);
    }
}

void MatchSceneRoom::setWarActorBaseInfo()
{
    ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
    if ( pSchemeCenter == NULL )
    {
        ErrorLn(__FUNCTION__ << ":get SchemeCenter failed!");
        return;
    }
    SSchemeMatchRoom* pSchemeMatchRoom = pSchemeCenter->getSchemeMatchRoom()->getSchemeMatchRoomByMatchTypeID(m_msg.dwMatchTypeID);
    if(pSchemeMatchRoom == NULL)
    {
        ErrorLn(__FUNCTION__ << ":pSchemeMatchRoom == NULL dwMatchTypeID ="<<m_msg.dwMatchTypeID);
        return;
    }

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

    DWORD dwTotal = m_dwComputerCount + m_dwPlayerCount;
    for (DWORD i=0; i<dwTotal; ++i)
    {
        MatchScenePlayer & player = m_arrayPlayer[i];
        if (player.m_role.isComputerPlayer) // 电脑
        {
            player.m_dwLoadOkTick   = getTickCount();
            player.m_eState         = ES_READY; 
        }

        SActorPreaperEnterWar sActorPreaperEnterWar;
        sActorPreaperEnterWar.uPDBID        = player.m_role.dwPDBID;
        sActorPreaperEnterWar.nHeroID       = player.m_role.dwHeroID;
		sActorPreaperEnterWar.nHeroSkinID	= player.m_role.dwSkinID;
        sActorPreaperEnterWar.nCamp         = pSchemeMatchRoom->getPosCamp(player.m_role.nIndex) + 1;
        sActorPreaperEnterWar.nIndex        = player.m_role.nIndex;
        sActorPreaperEnterWar.nSrcWorldID   = getThisGameWorldID();
        sActorPreaperEnterWar.nSrcSceneID   = 0;
        sActorPreaperEnterWar.vAngle        = Vector3();
        sActorPreaperEnterWar.vPosition     = Vector3();
        sActorPreaperEnterWar.isComputer    = player.m_role.isComputerPlayer;
        sActorPreaperEnterWar.nAIID         = player.m_role.nAIID;
        sActorPreaperEnterWar.nHeroXPSkillID     = player.m_role.nHeroXPSkillID;
        sActorPreaperEnterWar.byHeroStarLv  = player.m_role.byHeroStarLv;                             // 英雄星级
        sActorPreaperEnterWar.byHeroStaticLv = player.m_role.byHeroStaticLv;                            // 英雄等级
        sActorPreaperEnterWar.byActorSex    = player.m_role.bySex;
        sActorPreaperEnterWar.nHideRank    = player.m_role.nHideRankScore;
        sstrcpyn(sActorPreaperEnterWar.szName,player.m_role.szName ,sizeof(sActorPreaperEnterWar.szName));
		memcpy(sActorPreaperEnterWar.nSummonserSkill, player.m_role.nSummonserSkill, sizeof(sActorPreaperEnterWar.nSummonserSkill));

        pWarService->PrepareEnterWar(sActorPreaperEnterWar);
    }
}

//////////////////////////////////////////////////////////////////////////
/** 
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
void MatchSceneRoom::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
}

void MatchSceneRoom::clear()
{
	MATCH_TRACELN("MatchSceneService::clear");

	m_msg.dwRoomID = 0;
}

void MatchSceneRoom::send2SocialWarEnd()
{
    MATCH_TRACELN(__FUNCTION__": RoomID ="<<getRoomID());

    obuf256 obMsg;
    SMsgEnd msg;
    msg.m_RoomID = getRoomID();
    obMsg << msg;

	m_pService->sendToSocial(MSG_MATCH_ROOM_MODULEMSG,SO_MSG_END,obMsg.data(),obMsg.size());
}

void MatchSceneRoom::setSummonerSkillSlot()
{
    ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
    if ( pSchemeCenter == NULL )
    {
        ErrorLn(__FUNCTION__ << ":get SchemeCenter failed!");
        return;
    }

    ISchemeSlotMapedShortcut* pSchemeSlot = pSchemeCenter->getSchemeSlotInfo();
    if ( pSchemeSlot  == NULL )
    {
        ErrorLn(__FUNCTION__<< ": get pSchemeSlot  failed!");
        return;
    }

    m_nSummonerSlot[0] = pSchemeSlot ->getSlotAccordSkillType( EST_SUMMONER1);
    m_nSummonerSlot[1] = pSchemeSlot ->getSlotAccordSkillType( EST_SUMMONER2);
}


void MatchSceneRoom::broadcast2Client( char* pData, int nSize )
{
    IGatewayAcceptorService* pGatewayAcceptorService = gServerGlobal->getGatewayAcceptorService();
    if (nullptr == pGatewayAcceptorService)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pGatewayAcceptorService!");
        return;
    }

    DWORD dwTotal = m_dwComputerCount + m_dwPlayerCount;
    for (DWORD i=0; i<dwTotal; ++i)
    {
        MatchScenePlayer & player = m_arrayPlayer[i];
        if(player.m_role.clientID != 0 && player.m_role.clientID != INVALID_CLIENT)
        {
            pGatewayAcceptorService->sendData( player.m_role.clientID, PACKAGE_PTR(new std::string((char const *)pData, nSize)));
        }
    }

    for (DWORD i=0; i< m_msg.byOBRoleCount; ++i)
    {
        MatchScenePlayer & player = m_arrayOBPlayer[i];
        if(player.m_role.clientID != 0 && player.m_role.clientID != INVALID_CLIENT)
        {
            pGatewayAcceptorService->sendData( player.m_role.clientID, PACKAGE_PTR(new std::string((char const *)pData, nSize)));
        }
    }
}


MatchSceneRoom::~MatchSceneRoom()
{
	MATCH_TRACELN(__FUNCTION__": RoomID=" << getRoomID());
    
}

void MatchSceneRoom::realese()
{
    // 开启释放定时器
    gServerGlobal->getTimerService()->KillTimer(ETIMER_CLIENT_AUTO_END_TIMER, this);
    for(int i = 0; i < getAllPlayerCount(); ++i)
    {
        if (m_arrayPlayer[i].m_pCollectionRoomData)
        {
            free(m_arrayPlayer[i].m_pCollectionRoomData);
            m_arrayPlayer[i].m_pCollectionRoomData = NULL;
            m_arrayPlayer[i].m_nCollectDataSize =0;
        }

        MATCH_TRACELN(__FUNCTION__": removePDBID2RoomID() PDBID= "<<m_arrayPlayer[i].m_role.dwPDBID);
        m_pService->removePDBID2RoomID(m_arrayPlayer[i].m_role.dwPDBID, m_msg.dwRoomID);
    }

    if(m_arrayPlayer)
    {
        free(m_arrayPlayer);
    }

    for(int i = 0; i < m_msg.byOBRoleCount; ++i)
    {
        if (m_arrayOBPlayer[i].m_pCollectionRoomData)
        {
            free(m_arrayOBPlayer[i].m_pCollectionRoomData);
            m_arrayOBPlayer[i].m_pCollectionRoomData = NULL;
            m_arrayOBPlayer[i].m_nCollectDataSize =0;
        }

        MATCH_TRACELN(__FUNCTION__": removePDBID2RoomID() PDBID= "<<m_arrayOBPlayer[i].m_role.dwPDBID);
        m_pService->removePDBID2RoomID(m_arrayOBPlayer[i].m_role.dwPDBID, m_msg.dwRoomID);
    }

    if(m_arrayOBPlayer)
    {
        free(m_arrayOBPlayer);
    }

    m_pService      = NULL;
    m_eRoomState    = SRS_StateEnd;
    m_nWarID        = INVALID_WAR_ID;
    m_nSceneID      = INVALID_SCENE_ID;
    m_mutex         = 0;
    m_dwComputerCount =0;
    m_dwPlayerCount =0;
    m_bWarProcessEnd =false;

	delete this;
}

DWORD MatchSceneRoom::getPlayerCount()
{
    return m_dwPlayerCount;
}

DWORD MatchSceneRoom::getAllPlayerCount()
{
    return m_dwPlayerCount + m_dwComputerCount;
}

// 设置人物状态
bool MatchSceneRoom::setPlayerReady(PDBID idActor)
{
    MatchScenePlayer* pPlayer = findPlayer(idActor);
    if(pPlayer == NULL)
    {
        return false;
    }
    MatchScenePlayer & player = *pPlayer;
    player.m_dwLoadOkTick = getTickCount();
    player.m_eState = ES_READY; 

    return true;
}

// 设置OB人物状态
bool MatchSceneRoom::setOBPlayerReady(PDBID idActor)
{
    MatchScenePlayer* pPlayer = findOBPlayer(idActor);
    if(pPlayer == NULL)
    {
        return false;
    }
    MatchScenePlayer & player = *pPlayer;
    player.m_dwLoadOkTick = getTickCount();
    player.m_eState = ES_READY; 

    return true;
}


void MatchSceneRoom::setOBPlayerLogout(PDBID idActor)
{
    for (DWORD i=0; i<m_msg.byOBRoleCount; ++i)
    {
        MatchScenePlayer & player = m_arrayOBPlayer[i];

        if(player.m_role.dwPDBID == idActor)
        {
            player.m_eState = ES_EXIT_BATTLE_FIELD; 
            return;
        }
    }
}


// 角色下线处理
void MatchSceneRoom::setPlayerLogout(PDBID idActor)
{
    DWORD dwTotal = m_dwComputerCount + m_dwPlayerCount;
    for (DWORD i=0; i<dwTotal; ++i)
    {
        MatchScenePlayer & player = m_arrayPlayer[i];
          
        if(player.m_role.dwPDBID == idActor)
        {
            player.m_eState = ES_EXIT_BATTLE_FIELD; 
            return;
        }
    }

}


// 角色下线处理
DWORD MatchSceneRoom::getAllLogoutCount()
{
    DWORD dwAllPlayerLogoutCount = 0;
    DWORD dwTotal = m_dwComputerCount + m_dwPlayerCount;
    for (DWORD i=0; i<dwTotal; ++i)
    {
        MatchScenePlayer & player = m_arrayPlayer[i];
        if( player.m_eState == ES_EXIT_BATTLE_FIELD)
        {
            ++dwAllPlayerLogoutCount;
        }
    }

    return dwAllPlayerLogoutCount;
}

void MatchSceneRoom::setPlayerLogin(PDBID idActor)
{
    typedef vector<MatchScenePlayer>    COMPUTER_VECTOR;
    DWORD  dwAllPlayerLogoutCount = 0;

    DWORD dwTotal = m_dwComputerCount + m_dwPlayerCount;
    for (DWORD i=0; i<dwTotal; ++i)
    {
        MatchScenePlayer & player = m_arrayPlayer[i];

        if(player.m_role.dwPDBID == idActor)
        {
            player.m_eState = ES_FIGHTING; 
        }
    }

}

void MatchSceneRoom::warEndDestoryInvalidActor()
{
    if (m_pService == NULL)
    {
        return;
    }

    DWORD dwTotal = m_dwComputerCount + m_dwPlayerCount;
    for (DWORD i=0; i<dwTotal; ++i)
    {
        MatchScenePlayer & player = m_arrayPlayer[i];

        if(player.m_role.clientID == INVALID_CLIENT)
        {
            // 检查战场结束后的玩家退出是否能释放战场
            m_pService->warEndPersonLeaveCheck(player.m_role.dwPDBID , m_msg.dwRoomID);
        }
    }
}

void MatchSceneRoom::setWarProcessEnd()
{
    if (m_bWarProcessEnd)
    {
        return;
    }
    
    MATCH_TRACELN(__FUNCTION__": RoomID = "<<getRoomID());
    setState(SRS_StateEnd);
    m_bWarProcessEnd = true;

	// 计算推荐返回的主城场景
	if(!isPublicGameWorld())
		calcRecommandScene();

    // 移除已经掉线不在线的真实玩家
    removeDisConnectedTruePlayer();
}

bool MatchSceneRoom::getWarProcessEnd()
{
    return m_bWarProcessEnd;
}


DWORD MatchSceneRoom::getClientInvalidCount()
{
    DWORD  dwPlayerCount = 0;

    DWORD dwTotal = m_dwComputerCount + m_dwPlayerCount;
    for (DWORD i=0; i<dwTotal; ++i)
    {
        MatchScenePlayer & player = m_arrayPlayer[i];

        if(player.m_role.clientID == INVALID_CLIENT)
        {
            ++dwPlayerCount;
        }
    }

    return dwPlayerCount;
}



int MatchSceneRoom::getTalentPageSelectIndex(PDBID PlayerID)
{
    int nIndex = 0;
    {
        MatchScenePlayer*  pPlayer = findPlayer(PlayerID);
        if(pPlayer != NULL)
        {
            nIndex = pPlayer->m_role.nTalentEffectPageIndex;
        }
    }
    return nIndex;
}

int MatchSceneRoom::getRunePageSelectIndex(PDBID PlayerID)
{
    int nIndex = 0;
    {
        MatchScenePlayer*  pPlayer = findPlayer(PlayerID);
        if(pPlayer != NULL)
        {
            nIndex = pPlayer->m_role.nRuneEffectPageIndex;
        }
    }
    return nIndex;
}

bool MatchSceneRoom::createMobaActor(MatchScenePlayer * pSceneRoomPlayer)
{
    ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
    if ( pSchemeCenter == NULL )
    {
        ErrorLn(__FUNCTION__ << ":get SchemeCenter failed!");
        return false;
    }
    SSchemeMatchRoom* pSchemeMatchRoom = pSchemeCenter->getSchemeMatchRoom()->getSchemeMatchRoomByMatchTypeID(m_msg.dwMatchTypeID);
    if(pSchemeMatchRoom == NULL)
    {
        ErrorLn(__FUNCTION__ << ": pSchemeMatchRoom == NULL dwMatchTypeID ="<<m_msg.dwMatchTypeID);
        return false;
    }

    PlayerRoleProperty property;
    property.nPDBID = pSceneRoomPlayer->m_role.dwPDBID;
    sstrcpyn(property.szName, pSceneRoomPlayer->m_role.szName, sizeof(property.szName));
    property.nSceneID = m_nSceneID;
    property.nVocation = pSceneRoomPlayer->m_role.dwHeroID;
	property.nSkin = pSceneRoomPlayer->m_role.dwSkinID;
    property.byCamp = pSchemeMatchRoom->getPosCamp(pSceneRoomPlayer->m_role.nIndex) + 1;
    property.nLevel = 1;

    //位置相关
	SPosWithAngle ptBornPos;
	if (getBornPos(property.byCamp, pSceneRoomPlayer->m_role.nIndex, ptBornPos) == false)
	{
		ErrorLn(__FUNCTION__":getBornPos failed.nCamp=" << property.byCamp << ",roleIndex=" << pSceneRoomPlayer->m_role.nIndex);
	}

    property.location = ptBornPos.ptPos;
    property.angle = Vector3(0, ptBornPos.fAngleY, 0);
    property.nAISchemeID = pSceneRoomPlayer->m_role.nAIID;

	ISchemePersonModel* pSchemeModel = pSchemeCenter->getSchemePersonModel();
	if(pSchemeModel == NULL)
	{
		return false;
	}

    const PersonSkinModelItem * pPersonSkinModelItem = pSchemeModel->getPersonSkinModelItem(property.nVocation);
	if(!pPersonSkinModelItem)
	{
		ErrorLn(__FUNCTION__ << " failed,name="<< property.szName <<", pdbid="<< property.nPDBID <<", vocation=" << property.nVocation<<" check the config PersonModel.csv!");
		return false;
	}
    property.nSkin = (property.nSkin == 0) ? pPersonSkinModelItem->nDefaultSkinID : property.nSkin;
    property.bySex = pPersonSkinModelItem->bySex;

    obuf o;
    short partid = PART_PROPERTY;
    o << partid;
    int nLenOffset = o.size();
    int nLen = 0;
    o << nLen;  // 压入部件长度
    int nBegin = o.size();
    property.getDataBuffer(o, TO_OTHER_SEVER);
    *((int*)(o.data() + nLenOffset)) = o.size() - nBegin;

    partid = PART_WAR;
    o << partid;
    nLenOffset = o.size();
    nLen = 0;
    o << nLen;  // 压入部件长度
    nBegin = o.size();
    o << m_nWarID;
    o << (int)pSceneRoomPlayer->m_role.nIndex; 

    // 取召唤师技能
    int arraySkill[SUMMONER_SKILL_COUNT];
    int arraySlot[SUMMONER_SKILL_COUNT];
    int nCount = SUMMONER_SKILL_COUNT; 
    memcpy(arraySlot, pSceneRoomPlayer->m_SummonerSlot,sizeof(arraySlot));
    memcpy(arraySkill, pSceneRoomPlayer->m_role.nSummonserSkill,sizeof(arraySkill));

    o << nCount;
    for(int i = 0; i < nCount; ++i)
    {
        o << arraySlot[i] << arraySkill[i];
    }
    *((int*)(o.data() + nLenOffset)) = o.size() - nBegin; // 保存部件长度

    SActorCreateContext context;
    context.client = pSceneRoomPlayer->m_role.clientID;
    if(pSceneRoomPlayer->m_role.isComputerPlayer)
    {
        context.nFlag = CONTEXT_FLAG_ATTACH_THREAD;
    }
    context.udbid = pSceneRoomPlayer->m_role.dwUserID;
    context.pdbid = pSceneRoomPlayer->m_role.dwPDBID; 
    context.nSceneID = m_nSceneID;
    context.nSceneSerialNo = gServerGlobal->getSceneManager()->sceneIDToSerialNo(context.nSceneID);
    context.locBorn = ptBornPos.ptPos;
    context.fAngle = ptBornPos.fAngleY;
    context.eDStype = eds_type_cache;
    context.nCreateDataLen = o.size();
    context.nCreateDataFlag = TO_OTHER_SEVER;  
    if(pSceneRoomPlayer->m_pCollectionRoomData)
    {
        context.nUpdateDataLen = pSceneRoomPlayer->m_nCollectDataSize;
        context.nUpdateDataFlag = ACTOR_TO_HERO;
    }

    obuf buf;
    buf << context;
    buf.push_back(o.data(), o.size());

    // ACTOR->HERO 部分
    if(pSceneRoomPlayer->m_pCollectionRoomData)
    {
        buf.push_back(pSceneRoomPlayer->m_pCollectionRoomData, pSceneRoomPlayer->m_nCollectDataSize);

        free(pSceneRoomPlayer->m_pCollectionRoomData);
        pSceneRoomPlayer->m_pCollectionRoomData = NULL;
        pSceneRoomPlayer->m_nCollectDataSize =0;
    }

    int nReason = elogin_in_process;
    if(pSceneRoomPlayer->m_role.isComputerPlayer)
    {
        nReason = elogin_online;
    }
    else
    {
        if(pSceneRoomPlayer->m_role.nWorldID != getThisGameWorldID())
        {
            nReason = elogin_cross_gameworld;
        }
        else
        {
            if(pSceneRoomPlayer->m_role.nZoneServerID != gServerGlobal->getCenterConnectorService()->getLocalServerCSID())
            {
                nReason = elogin_cross_process;
            }
        }
    }

    pSceneRoomPlayer->m_UID = gServerGlobal->getActorMgr()->createActor((SActorCreateContext*)buf.data(), nReason);

    return pSceneRoomPlayer->m_UID != INVALID_UID;
}


bool MatchSceneRoom::createMobaOBActor(PDBID PlayerID)
{
    MatchScenePlayer* pSceneRoomPlayer = getOBPlayer(PlayerID);
    if (pSceneRoomPlayer == NULL)
    {
        ErrorLn(__FUNCTION__": pSceneRoomPlayer == NULL PlayerID ="<<PlayerID);
        return false;
    }
    ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
    if ( pSchemeCenter == NULL )
    {
        ErrorLn(__FUNCTION__ << ":get SchemeCenter failed!");
        return false;
    }
    SSchemeMatchRoom* pSchemeMatchRoom = pSchemeCenter->getSchemeMatchRoom()->getSchemeMatchRoomByMatchTypeID(m_msg.dwMatchTypeID);
    if(pSchemeMatchRoom == NULL)
    {
        ErrorLn(__FUNCTION__ << ": pSchemeMatchRoom == NULL dwMatchTypeID ="<<m_msg.dwMatchTypeID);
        return false;
    }

    ISchemePersonModel* pSchemeModel = pSchemeCenter->getSchemePersonModel();
    if(pSchemeModel == NULL)
    {
        return false;
    }

    PlayerRoleProperty property;
    property.nPDBID = pSceneRoomPlayer->m_role.dwPDBID;
    sstrcpyn(property.szName, pSceneRoomPlayer->m_role.szName, sizeof(property.szName));
    property.nSceneID = m_nSceneID;
    property.nVocation = 1;
    property.nSkin = 0;
    property.byCamp = 0;
    property.nLevel = 1;

    const PersonSkinModelItem * pPersonSkinModelItem = pSchemeModel->getPersonSkinModelItem(1);
    if(!pPersonSkinModelItem)
    {
        ErrorLn(__FUNCTION__ << " failed,name="<< property.szName <<", pdbid="<< property.nPDBID <<", vocation=" << property.nVocation<<" check the config PersonModel.csv!");
        return false;
    }

	//位置相关
	SPosWithAngle ptBornPos;
	if (getBornPos(property.byCamp, pSceneRoomPlayer->m_role.nIndex, ptBornPos) == false)
	{
		ErrorLn(__FUNCTION__":getBornPos failed.nCamp=" << property.byCamp << ",roleIndex=" << pSceneRoomPlayer->m_role.nIndex);
	}
    property.location = ptBornPos.ptPos;
    property.angle = Vector3(0, ptBornPos.fAngleY, 0);
    property.nAISchemeID = -1;
    property.nSkin = (property.nSkin == 0) ? pPersonSkinModelItem->nDefaultSkinID : property.nSkin;
    property.bySex = pPersonSkinModelItem->bySex;
    property.byObserver = 1;

    obuf o;
    short partid = PART_PROPERTY;
    o << partid;
    int nLenOffset = o.size();
    int nLen = 0;
    o << nLen;  // 压入部件长度
    int nBegin = o.size();
    property.getDataBuffer(o, TO_OTHER_SEVER);
    *((int*)(o.data() + nLenOffset)) = o.size() - nBegin;

    partid = PART_WAR;
    o << partid;
    nLenOffset = o.size();
    nLen = 0;
    o << nLen;  // 压入部件长度
    nBegin = o.size();
    o << m_nWarID;
    o << (int)pSceneRoomPlayer->m_role.nIndex; 

    // 取召唤师技能
    int arraySkill[SUMMONER_SKILL_COUNT] = {0};
    int arraySlot[SUMMONER_SKILL_COUNT] = {0};
    int nCount = SUMMONER_SKILL_COUNT;
    memcpy(arraySlot, pSceneRoomPlayer->m_SummonerSlot,sizeof(arraySlot));
    memcpy(arraySkill, pSceneRoomPlayer->m_role.nSummonserSkill,sizeof(arraySkill));

    o << nCount;
    for(int i = 0; i < nCount; ++i)
    {
        o << arraySlot[i] << arraySkill[i];
    }
    *((int*)(o.data() + nLenOffset)) = o.size() - nBegin; // 保存部件长度

    SActorCreateContext context;
    context.client = pSceneRoomPlayer->m_role.clientID;
    if(pSceneRoomPlayer->m_role.isComputerPlayer)
    {
        context.nFlag = CONTEXT_FLAG_ATTACH_THREAD;
    }
    context.udbid = pSceneRoomPlayer->m_role.dwUserID;
    context.pdbid = pSceneRoomPlayer->m_role.dwPDBID; 
    context.nSceneID = m_nSceneID;
    context.nSceneSerialNo = gServerGlobal->getSceneManager()->sceneIDToSerialNo(context.nSceneID);
    context.locBorn = ptBornPos.ptPos;
    context.fAngle = ptBornPos.fAngleY;
    context.eDStype = eds_type_cache;
    context.nCreateDataLen = o.size();
    context.nCreateDataFlag = TO_OTHER_SEVER;  
    if(pSceneRoomPlayer->m_pCollectionRoomData)
    {
        context.nUpdateDataLen = pSceneRoomPlayer->m_nCollectDataSize;
        context.nUpdateDataFlag = ACTOR_TO_HERO;
    }

    obuf buf;
    buf << context;
    buf.push_back(o.data(), o.size());

    // ACTOR->HERO 部分
    if(pSceneRoomPlayer->m_pCollectionRoomData)
    {
        buf.push_back(pSceneRoomPlayer->m_pCollectionRoomData, pSceneRoomPlayer->m_nCollectDataSize);

        free(pSceneRoomPlayer->m_pCollectionRoomData);
        pSceneRoomPlayer->m_pCollectionRoomData = NULL;
        pSceneRoomPlayer->m_nCollectDataSize =0;
    }

    int nReason = elogin_in_process;
    if(pSceneRoomPlayer->m_role.isComputerPlayer)
    {
        nReason = elogin_online;
    }
    else
    {
        if(pSceneRoomPlayer->m_role.nWorldID != getThisGameWorldID())
        {
            nReason = elogin_cross_gameworld;
        }
        else
        {
            if(pSceneRoomPlayer->m_role.nZoneServerID != gServerGlobal->getCenterConnectorService()->getLocalServerCSID())
            {
                nReason = elogin_cross_process;
            }
        }
    }

    pSceneRoomPlayer->m_UID = gServerGlobal->getActorMgr()->createActor((SActorCreateContext*)buf.data(), nReason);

    return pSceneRoomPlayer->m_UID != INVALID_UID;
}


// 开始创建所有角色
void MatchSceneRoom::onCreatePlayers()
{
    ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
    if ( pSchemeCenter == NULL )
    {
        ErrorLn(__FUNCTION__ << ":get SchemeCenter failed!");
        return;
    }
    SSchemeMatchRoom* pSchemeMatchRoom = pSchemeCenter->getSchemeMatchRoom()->getSchemeMatchRoomByMatchTypeID(m_msg.dwMatchTypeID);
    if(pSchemeMatchRoom == NULL)
    {
        ErrorLn(__FUNCTION__ << ": pSchemeMatchRoom == NULL dwMatchTypeID ="<<m_msg.dwMatchTypeID);
        return;
    }

    ISchemePersonModel* pSchemePersonModel = pSchemeCenter->getSchemePersonModel();
    if(pSchemePersonModel == NULL)
    {
        ErrorLn(__FUNCTION__ << ": pSchemePersonModel == NULL dwMatchTypeID ="<<m_msg.dwMatchTypeID);
        return;
    }


	// 随机出生点位置（如果需要的话）
	randomBornPos(getModeID(), getMapID());

    {
        DWORD dwTotal = m_dwComputerCount + m_dwPlayerCount;
        for (DWORD i=0; i<dwTotal; ++i)
        {
            MatchScenePlayer* player = m_arrayPlayer+i;
            if(player->m_eState == ES_READY)
            {
                player->m_eState = ES_FIGHTING; 
            }
           
            // 创建电脑以及玩家服务
            if (!createMobaActor(player))
            {
                ErrorLn(__FUNCTION__": CreateMobaActor failed actor ="<<player->m_role.dwPDBID);
            }
        }
    }
}

// 检测所有角色是否都准备好了
bool  MatchSceneRoom::checkPlayersReady()
{

    // 没有准备数量
    DWORD dwNotReadyCount = 0;
    {
        DWORD dwTotal = m_dwComputerCount + m_dwPlayerCount;
        if(dwTotal == 0)
        {
            return false;
        }

        for (DWORD i=0; i<dwTotal; ++i)
        {
            MatchScenePlayer & player = m_arrayPlayer[i];
            if(player.m_eState == ES_NONE)
            {
                ++dwNotReadyCount;
            }
        }
    }

    // 还有在加载的，并且没超时，直接返回
    if(dwNotReadyCount > 0 )
    {
        return false;
    }

    return true;
}

bool MatchSceneRoom::isLoadingSceneTimeOut()
{
    // 判定超时
    if (getState() == SRS_StateWaitLoading &&  m_dwStartMigrateTime > 0 && getTickCount() - m_dwStartMigrateTime < 1000 * 90)
    {
        return false;
    }
    
    return true;
}

MatchScenePlayer* MatchSceneRoom::findPlayer(DWORD dwPlayerID)
{
    DWORD dwTotal = m_dwComputerCount + m_dwPlayerCount;
    for (DWORD i=0; i<dwTotal; ++i)
    {
        MatchScenePlayer & player = m_arrayPlayer[i];
        if(player.m_role.dwPDBID == dwPlayerID)
        {
            return &m_arrayPlayer[i];
        }
    }
    return NULL;
}


MatchScenePlayer* MatchSceneRoom::findOBPlayer(DWORD dwPlayerID)
{
    BYTE byCount = m_msg.byOBRoleCount;
    for (DWORD i=0; i<byCount; ++i)
    {
        MatchScenePlayer & player = m_arrayOBPlayer[i];
        if(player.m_role.dwPDBID == dwPlayerID)
        {
            return &m_arrayOBPlayer[i];
        }
    }
    return NULL;
}


DWORD MatchSceneRoom::getRoomAllMatchScenePlayer(obuf &PlayerInfo)
{
    DWORD dwTotal = m_dwComputerCount + m_dwPlayerCount;
    for (DWORD i=0; i<dwTotal; ++i)
    {
        MatchScenePlayer &player = m_arrayPlayer[i];
        PlayerInfo.push_back(&player,sizeof(MatchScenePlayer));
    }
    return dwTotal;
}

// 设置杯赛房间数据
void MatchSceneRoom::setLegendCupRoomInfo(SMsgLegendCupRoomInfo *pExData)
{
	m_sLegendCupRoomInfo = *pExData;
}

void MatchSceneRoom::setState(ESceneRoomState state)
{
    m_eRoomState = state;
}

void MatchSceneRoom::setStartMigrateTime()
{
    m_dwStartMigrateTime = getTickCount();
}

// 发送战斗结果to社会服杯赛模块
void MatchSceneRoom::sendWarResult2SocialLegendCup(SMsgSetWarResultData resultData)
{
	if (getRoomType() != HallRoomType_LegendCup)
	{
		return;
	}

	ICenterConnectorService * pConnector = gServerGlobal->getCenterConnectorService();
	if(pConnector == NULL)
	{
		ErrorLn( __FUNCTION__ ": sendWarResult2SocialLegendCup CenterConnectorService==NULL");
		return;
	}

	SMsgRecordNodeWarResult sendData;
	sendData.llLegendCupID = m_sLegendCupRoomInfo.dwLedendCupID;
	sendData.llWarID = resultData.llDBWarID;
	sendData.nRoundID = m_sLegendCupRoomInfo.nRoundID;
	sendData.nSerchID = m_sLegendCupRoomInfo.nSerchID;
    sendData.nBoCount = m_sLegendCupRoomInfo.nBoCount;
	sendData.dwKin1KDA = 0;
	sendData.dwKin2KDA = 0;
	sendData.byJudgeFlag = JUDGE_AllKnockOut;
	sendData.dwWinnerKinID = 0;
	sendData.dwBeginTime = resultData.dwBeginTime;
	sendData.dwKin1ID = m_sLegendCupRoomInfo.dwKin1;
	sendData.dwKin2ID = m_sLegendCupRoomInfo.dwKin2;

	if (resultData.nFailedCamp == (m_sLegendCupRoomInfo.nKin1Camp + 1))
	{
		sendData.byJudgeFlag = JUDGE_ConfirmNode;
		sendData.dwWinnerKinID = m_sLegendCupRoomInfo.dwKin2;
		sendData.dwKin1KDA = resultData.nFailedKDA;
		sendData.dwKin2KDA = resultData.nWinKDA;
		memcpy(sendData.nKin1Glory, resultData.nFailGlory, sizeof(sendData.nKin1Glory));
		memcpy(sendData.nKin2Glory, resultData.nWinGlory, sizeof(sendData.nKin2Glory));
	}
	else if (resultData.nFailedCamp == (m_sLegendCupRoomInfo.nKin2Camp + 1))
	{
		sendData.byJudgeFlag = JUDGE_ConfirmNode;
		sendData.dwWinnerKinID = m_sLegendCupRoomInfo.dwKin1;
		sendData.dwKin1KDA = resultData.nWinKDA;
		sendData.dwKin2KDA = resultData.nFailedKDA;
		memcpy(sendData.nKin1Glory, resultData.nWinGlory, sizeof(sendData.nKin1Glory));
		memcpy(sendData.nKin2Glory, resultData.nFailGlory, sizeof(sendData.nKin2Glory));
	}

	obuf oData;
	oData.push_back(&sendData, sizeof(SMsgRecordNodeWarResult));
	pConnector->sendToSocial(MSG_MODULEID_LEGENDCUP, SO_CUP_RECORD_WAR_RESULT, oData.data(), oData.size());
}

// 推荐玩家返回的主城场景
void MatchSceneRoom::calcRecommandScene()
{
	IMigrateService* pMigrate = (IMigrateService*)gServerGlobal->getMigrateService();
	if (!pMigrate)
		return;

	// 所有预进入玩家信息
	vector<SPreEnterScenePlayerInfo> playerList;
	int nTotal = getAllPlayerCount();
	for (int i = 0; i < nTotal; ++i)
	{
		SRoomRoleBase& role = m_arrayPlayer[i].m_role;
		if (role.isComputerPlayer)
			continue;

		// 断线玩家
		if (role.clientID == INVALID_CLIENT)
			continue;

		SPreEnterScenePlayerInfo info;
		info.client = role.clientID;
		info.idActor = role.dwPDBID;
		info.nDstSceneID = m_RecommandScene.nSceneID;

		playerList.push_back(info);
	}
	
	// 预进入战场人数
	int nPreEnterCount = playerList.size();

	if (nPreEnterCount == 0)
	{
		WarningLn(__FUNCTION__":switchscene, The count of pre enter scene equal 0.");
		return;
	}
		

	// 获取合适的主城场景
	if (!pMigrate->getAvailableScene(emaptype_main, nPreEnterCount, &m_RecommandScene))
		return;

	// 预进入场景
	SPreEnterSceneContext context;
	context.destNode = m_RecommandScene;
	context.nPlayerCount = nPreEnterCount;

	obuf ob;
	ob << context;
	for (vector<SPreEnterScenePlayerInfo>::iterator iter = playerList.begin(); iter != playerList.end(); ++iter)
	{
		iter->nDstSceneID = m_RecommandScene.nSceneID;
		ob << *iter;
	}

	pMigrate->preEnterScene(ob.data(), ob.size());
}

void MatchSceneRoom::removeDisConnectedTruePlayer()
{
    for(int i = 0; i < getAllPlayerCount(); ++i)
    {
        MatchScenePlayer* p = m_arrayPlayer + i;

        // 正在释放中的以及已经退出了战场的则不处理了
        if (p->m_eState == ES_DESTORY_ACTOR || p->m_eState == ES_EXIT_BATTLE_FIELD)
        {
            continue;
        }

        if(p->m_role.clientID == INVALID_CLIENT && !p->m_role.isComputerPlayer)
        {
            DWORD dwRomvePlayerID = p->m_role.dwPDBID;
            UID removeUID = p->m_UID;
            if(removeUID != INVALID_UID)
            {
                SceneLogicHelper helper(UID_2_SCENE(removeUID));
                if(!helper.m_ptr)
                {
                    ErrorLn(__FUNCTION__ << __LINE__ << ", uid=" << removeUID << ", destroy failed, scenelogic not found");
                }
                else
                {
                    helper.m_ptr->destroyActor(removeUID, elogout_offline);
                } 
                // 设置状态在销毁玩家中
                p->m_eState = ES_DESTORY_ACTOR;
            }
        }
    }
}

void MatchSceneRoom::randomBornPos(DWORD dwModeID, DWORD dwMapID)
{
	m_mapRandBornPos.clear();

	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return;
	}
	ISchemeMapLand* pSchemeMapLand = pSchemeCenter->getSchemeMapLand();
	if (pSchemeMapLand == NULL)
	{
		return;
	}

	SMapLandSchemeInfo* pMapLandSchemeInfo = pSchemeMapLand->GetRandMapLandSchemeInfo(dwMapID, dwModeID);
	if (pMapLandSchemeInfo == NULL)
	{
		return;
	}

	SSchemeMatchRoom* pSchemeMatchRoom = pSchemeCenter->getSchemeMatchRoom()->getSchemeMatchRoomByMatchTypeID(m_msg.dwMatchTypeID);
	if (pSchemeMatchRoom == NULL)
	{
		ErrorLn(__FUNCTION__ << ": pSchemeMatchRoom == NULL dwMatchTypeID =" << m_msg.dwMatchTypeID);
		return ;
	}

	int nCampCount = pSchemeMatchRoom->getPosCamp(m_nMaxRoleIndex) + 1;
	Vector3 center = pMapLandSchemeInfo->ptCenterZone;
	float radius = pMapLandSchemeInfo->fRadiusZone;
	float distance = pMapLandSchemeInfo->fCampDistance;
	Cicle circle(center, radius);

	vector<Vector3> campPosList;
	if (distance < -0.01f) // < 0
	{
		// 均匀分布
		campPosList = CMathHelper::getRandomPosOfCicleOnAvg(circle, nCampCount);
	}
    else if(distance > -0.01f && distance < 0.01f) // =0
    {
        // 直接读配置的点
        SMapLandSchemeInfo* pLandInfo = pSchemeMapLand->GetMapLandSchemeInfo(dwMapID, m_msg.dwMatchTypeID, -1);
        if(pLandInfo == NULL)
        {
            ErrorLn(__FUNCTION__ << " born point scheme not found, map=" << dwMapID << ", type=" << m_msg.dwMatchTypeID << "camp=-1");
            return;
        }

        std::vector<Vector3> vecOptPos;
        for(int i = 0; i < pLandInfo->vecBornPos.size(); ++i) vecOptPos.push_back(pLandInfo->vecBornPos[i].ptPos);
        for(int i = 0; i < nCampCount && vecOptPos.size() > 0; ++i)
        {
            int nSelIndex = mt_range_rand(0, vecOptPos.size() - 1);
            campPosList.push_back(vecOptPos[nSelIndex]);
            vecOptPos.erase(vecOptPos.begin() + nSelIndex);
        }
    }
	else // > 0
	{
		// 保持至少distance的间隔分布
		campPosList = CMathHelper::getRandomPosOfCicleFixNumEx(circle, distance, nCampCount);
	}

	if (campPosList.size() != nCampCount)
	{
		ErrorLn(__FUNCTION__":campPosList.size() != nCampCount");
	}

	// 保存每个阵营的出生点
	int nCampID = 1;   // 默认阵营从1开始增长
	for (vector<Vector3>::iterator iter = campPosList.begin(); iter != campPosList.end(); ++iter)
	{
		WarningLn(__FUNCTION__":" << iter->x << "," << iter->y << "," << iter->z);

		m_mapRandBornPos[nCampID++] = *iter;
	}

	WarningLn(__FUNCTION__":nCampCount" << nCampCount << ",posNum=" << campPosList.size());
}

bool MatchSceneRoom::getBornPos(int nCamp, int nRoleIndex, SPosWithAngle& pos)
{
	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return false;
	}
	ISchemeMapLand* pSchemeMapLand = pSchemeCenter->getSchemeMapLand();
	if (pSchemeMapLand == NULL)
	{
		return false;
	}
	
	// 看下是否需要随机出生点
	SMapLandSchemeInfo* pRandMapLandSchemeInfo = pSchemeMapLand->GetRandMapLandSchemeInfo(getMapID(), getModeID());
	if (pRandMapLandSchemeInfo != NULL)  // 随机出生位置
	{
		if (m_mapRandBornPos.find(nCamp) == m_mapRandBornPos.end())
		{
			ErrorLn(__FUNCTION__":m_mapRandBornPos.find(nCamp) == m_mapRandBornPos.end(),not found camp.");
			return false;
		}

		Vector3 campCenter = m_mapRandBornPos[nCamp];

		// 同阵营玩家的位置不能一样
		Vector3 rolePos(campCenter.x + nRoleIndex * 1.f, campCenter.y, campCenter.z + nRoleIndex * 1.f);

		// 修正坐标
		WarningLn(__FUNCTION__":before " << rolePos.x << "," << rolePos.y << "," << rolePos.z);
		gServerGlobal->getHeightFieldMgr()->correctPosFromPhysic(getMapID(), rolePos, 200, 200);
		WarningLn(__FUNCTION__":after " << rolePos.x << "," << rolePos.y << "," << rolePos.z);

		pos.ptPos = rolePos;
		pos.fAngleY = 0.f;
	}
	else
	{
		// 出生位置读取配置
		SMapLandSchemeInfo* pMapLandSchemeInfo = pSchemeMapLand->GetMapLandSchemeInfo(getMapID(), getModeID(), nCamp);
		if (pMapLandSchemeInfo == NULL)
		{
			ErrorLn(__FUNCTION__ << ": pMapLandSchemeInfo == NULL m_dwMapID =" << getMapID() << " byCamp=" << nCamp);
			return false;
		}

		vector<SPosWithAngle> vecPoses = pMapLandSchemeInfo->vecBornPos;
		if(vecPoses.size() == 0)
		{
		    ErrorLn(__FUNCTION__ << ": check the mapland.csv m_dwMapID =" << getMapID() << " byCamp=" << nCamp);
		    return false;
		}
		// 获取出生位置
		int nPos = 0;
		if (nRoleIndex < 0) 
		{
		    // 以场景玩家数为范围随机出一个玩家位置
		    nPos = rand() % vecPoses.size();
		}
		else 
		{
		    nPos = nRoleIndex % vecPoses.size();
		}
		// 取得玩家在战场内的出生点
		pos.ptPos = vecPoses[nPos].ptPos;
		pos.fAngleY = vecPoses[nPos].fAngleY;
	}

	return true;
}

void MatchSceneRoom::setAutoEnd()
{
    if(m_bAutoEnd)
    {
        return;
    }

    // 开启释放定时器
    gServerGlobal->getTimerService()->KillTimer(ETIMER_CLIENT_AUTO_END_TIMER, this);
    gServerGlobal->getTimerService()->SetTimer(ETIMER_CLIENT_AUTO_END_TIMER, 1000*60*3, this, 1, __FUNCTION__);

    m_bAutoEnd = true;
}

void MatchSceneRoom::clearAutoEnd()
{
    // 不在自动结束退出
    if(!m_bAutoEnd)
    {
        return;
    }

    // 移除释放定时器
    gServerGlobal->getTimerService()->KillTimer(ETIMER_CLIENT_AUTO_END_TIMER, this);
    m_bAutoEnd = false;
}


void MatchSceneRoom::destoryRoomPlayer()
{
    
    destoryTrueRoomPlayer();
    destoryOBRoomPlayer();
}


void MatchSceneRoom::destoryTrueRoomPlayer()
{
    // 循环遍历删除玩家
    for(int i = 0; i < getAllPlayerCount(); ++i)
    {
        MatchScenePlayer* p = m_arrayPlayer + i;

        // 正在释放中的以及已经退出了战场的则不处理了
        if (p->m_eState == ES_DESTORY_ACTOR || p->m_eState == ES_EXIT_BATTLE_FIELD)
        {
            continue;
        }

        if(p->m_role.clientID == INVALID_CLIENT)
        {
            DWORD dwRomvePlayerID = p->m_role.dwPDBID;
            UID removeUID = p->m_UID;
            if(removeUID != INVALID_UID)
            {
                SceneLogicHelper helper(UID_2_SCENE(removeUID));
                if(!helper.m_ptr)
                {
                    ErrorLn(__FUNCTION__ << __LINE__ << ", uid=" << removeUID << ", destroy failed, scenelogic not found");
                }
                else
                {
                    helper.m_ptr->destroyActor(removeUID, elogout_offline);
                } 
                // 设置状态在销毁玩家中
                p->m_eState = ES_DESTORY_ACTOR;
            }
        }
    }
}

void MatchSceneRoom::destoryPlayer(DWORD dwActorID)
{
    for(int i = 0; i < getAllPlayerCount(); ++i)
    {
        MatchScenePlayer* p = m_arrayPlayer + i;

        // 正在释放中的以及已经退出了战场的则不处理了
        if (p->m_eState == ES_DESTORY_ACTOR || p->m_eState == ES_EXIT_BATTLE_FIELD)
        {
            continue;
        }

        if(p->m_role.dwPDBID == dwActorID)
        {
            DWORD dwRomvePlayerID = p->m_role.dwPDBID;
            UID removeUID = p->m_UID;
            if(removeUID != INVALID_UID)
            {
                SceneLogicHelper helper(UID_2_SCENE(removeUID));
                if(!helper.m_ptr)
                {
                    ErrorLn(__FUNCTION__ << __LINE__ << ", uid=" << removeUID << ", destroy failed, scenelogic not found");
                }
                else
                {
                    helper.m_ptr->destroyActor(removeUID, elogout_offline);
                } 
                // 设置状态在销毁玩家中
                p->m_eState = ES_DESTORY_ACTOR;
            }
            break;
        }
    }
}


void MatchSceneRoom::destoryOBPlayer(DWORD dwActorID)
{
    for (DWORD i=0; i < m_msg.byOBRoleCount; ++i)
    {
        MatchScenePlayer* p = m_arrayOBPlayer + i;
        // 正在释放中的以及已经退出了战场的则不处理了
        if (p->m_eState == ES_DESTORY_ACTOR || p->m_eState == ES_EXIT_BATTLE_FIELD)
        {
            continue;
        }

        if(p->m_role.dwPDBID == dwActorID)
        {
            DWORD dwRomvePlayerID = p->m_role.dwPDBID;
            UID removeUID = p->m_UID;
            if(removeUID != INVALID_UID)
            {
                SceneLogicHelper helper(UID_2_SCENE(removeUID));
                if(!helper.m_ptr)
                {
                    ErrorLn(__FUNCTION__ << __LINE__ << ", uid=" << removeUID << ", destroy failed, scenelogic not found");
                }
                else
                {
                    helper.m_ptr->destroyActor(removeUID, elogout_offline);
                } 
                // 设置状态在销毁玩家中
                p->m_eState = ES_DESTORY_ACTOR;
            }
            break;
        }
    }
}

void MatchSceneRoom::destoryOBRoomPlayer()
{
    for (DWORD i=0; i < m_msg.byOBRoleCount; ++i)
    {
        MatchScenePlayer* p = m_arrayOBPlayer + i;
        // 正在释放中的以及已经退出了战场的则不处理了
        if (p->m_eState == ES_DESTORY_ACTOR || p->m_eState == ES_EXIT_BATTLE_FIELD)
        {
            continue;
        }

        if(p->m_role.clientID == INVALID_CLIENT)
        {
            DWORD dwRomvePlayerID = p->m_role.dwPDBID;
            UID removeUID = p->m_UID;
            if(removeUID != INVALID_UID)
            {
                SceneLogicHelper helper(UID_2_SCENE(removeUID));
                if(!helper.m_ptr)
                {
                    ErrorLn(__FUNCTION__ << __LINE__ << ", uid=" << removeUID << ", destroy failed, scenelogic not found");
                }
                else
                {
                    helper.m_ptr->destroyActor(removeUID, elogout_offline);
                } 
                // 设置状态在销毁玩家中
                p->m_eState = ES_DESTORY_ACTOR;
            }
        }
    }
}

void MatchSceneRoom::OnTimer(unsigned long dwTimerID)
{
    destoryRoomPlayer();
}

void MatchSceneRoom::getCreateRoomInfo(SMsgMatchRoomDataRestore &sRestoreInfo)
{
    sRestoreInfo.dwMatchTypeID  = m_msg.dwMatchTypeID;                      // 撮合类型ID
    sRestoreInfo.dwRoomID       = m_msg.dwRoomID;							// 房间ID
    sRestoreInfo.dwMapID        = m_msg.dwMapID;							// 地图ID
    sRestoreInfo.nModeID        = m_msg.nModeID;							// 模式的类型 EGameMode
    sRestoreInfo.nConfigWarID   = m_msg.nConfigWarID;						// scheme配置WarID
    sRestoreInfo.byRoleCount    = m_msg.byRoleCount;						// 角色数量
    sRestoreInfo.bIsMatchRoom   = m_msg.bIsMatchRoom;                       // 是否是撮合房间
    sRestoreInfo.nRoomType      = m_msg.nRoomType;							// 房间类型 EMHallRoomType
    sRestoreInfo.bOnlineLegendCup= m_msg.bOnlineLegendCup;					// 是否是线上杯赛
    sRestoreInfo.bGuidRoom      = m_msg.bGuidRoom;                          // 是否是引导房间
    sRestoreInfo.nSceneID       = m_nSceneID;
    DWORD dwServerID = gServerGlobal->getCenterConnectorService()->getLocalServerCSID();
    sRestoreInfo.nSceneSerialNo = gServerGlobal->getCenterConnectorService()->getSceneSerialNo(dwServerID, m_nSceneID);
}

void MatchSceneRoom::accordLeaveWar(PDBID idActor)
{
    if (idActor == INVALID_PDBID)
    {
        ErrorLn(__FUNCTION__": idActor == INVALID_PDBID");
        return;
    }

    // 是否开启了能主动退出战场的标识
    ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__" nullptr == pSchemeCenter");
        return;
    }

    ISchemeMatchRoom * pSchemeMatchRoom = pSchemeCenter->getSchemeMatchRoom();
    if (pSchemeMatchRoom == NULL)
    {
        ErrorLn(__FUNCTION__" can't pSchemeMatchRoom == NULL");
        return;
    }

    SSchemeMatchRoom* pMatchRoom = pSchemeMatchRoom->getSchemeMatchRoomByMatchTypeID(m_msg.dwMatchTypeID);
    if (pMatchRoom == nullptr )
    {
        ErrorLn(__FUNCTION__": can't find SchemeInfo dwMatchTypeID ="<<m_msg.dwMatchTypeID);
        return;
    }

    if (!m_bWarProcessEnd && !pMatchRoom->m_bCanLogOutInWar)
    {
        ErrorLn(__FUNCTION__": bCanLogOutInWar == false dwMatchTypeID ="<<m_msg.dwMatchTypeID);
        return;
    }

    MatchScenePlayer* pScenePlayer = findPlayer(idActor);
    if (pScenePlayer ==NULL)
    {
        WarningLn(__FUNCTION__": pScenePlayer ==NULL idActor=" << idActor);
        return;
    }
    
    // 推荐回到的主城场景
    SSceneNode mainScene;
    if (isPublicGameWorld())
    {
        mainScene.nWorldID = pScenePlayer->m_role.nWorldID;
        mainScene.nZoneServerID = pScenePlayer->m_role.nZoneServerID;
        mainScene.nSceneID = pScenePlayer->m_role.nSceneID;
        mainScene.nSceneSN = pScenePlayer->m_role.nSceneSN;
    }
    else
    {
        mainScene = getRecommandScene();
        if (mainScene.nSceneID <= 0)
        {
            ErrorLn(__FUNCTION__":switchScene, mainScene.nSceneID <= 0, RoomID=" << getRoomID()<<",idActor="<<idActor <<",nDestSceneID=" << mainScene.nSceneID);

			mainScene.nWorldID = pScenePlayer->m_role.nWorldID;
			mainScene.nZoneServerID = pScenePlayer->m_role.nZoneServerID;
			mainScene.nSceneID = pScenePlayer->m_role.nSceneID;
			mainScene.nSceneSN = pScenePlayer->m_role.nSceneSN;
        }
    }

#ifdef __SWITCH_SCENE_PRINT__

    TraceLn(__FUNCTION__":switchScene, RoomID=" << getRoomID() << ",idActor=" << idActor << ",nDestWorldID=" 
        << mainScene.nWorldID << ",nDestServerID="  << mainScene.nZoneServerID << ",nDestSceneID=" << mainScene.nSceneID);
#endif

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

bool MatchSceneRoom::isOBPlayer(PDBID idActor)
{
    for (DWORD i=0; i < m_msg.byOBRoleCount; ++i)
    {
        MatchScenePlayer & player = m_arrayOBPlayer[i];
        if (idActor == player.m_role.dwPDBID)
        {
            return player.m_role.bIsOBPos;
        }
    }

    return false;
}

MatchScenePlayer* MatchSceneRoom::getOBPlayer(PDBID idActor)
{
    for (DWORD i=0; i < m_msg.byOBRoleCount; ++i)
    {
        MatchScenePlayer* player = m_arrayOBPlayer + i;
        if (idActor == player->m_role.dwPDBID)
        {
            return player;
        }
    }

    return NULL;
}

bool MatchSceneRoom::allOBPlayerIsInValid()
{
    bool InValid = true;
    for (DWORD i=0; i < m_msg.byOBRoleCount; ++i)
    {
        MatchScenePlayer* player = m_arrayOBPlayer + i;
        if (player->m_role.clientID != INVALID_CLIENT)
        {
            InValid = false;
            break;
        }
    }

    return InValid;
}

bool MatchSceneRoom::isAllOBLogout()
{
    bool bAllLogout = true;
    for (DWORD i=0; i< m_msg.byOBRoleCount; ++i)
    {
        MatchScenePlayer & player = m_arrayOBPlayer[i];
        if( player.m_eState == ES_EXIT_BATTLE_FIELD || player.m_eState == ES_NONE)
        {
           continue;;
        }

        bAllLogout = false;
        break;
    }

    return bAllLogout;
}
