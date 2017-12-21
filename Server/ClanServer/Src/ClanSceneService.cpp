#include "stdafx.h"
#include "ClanSceneService.h"
#include "ICenterConnectorService.h"
#include "IServerGlobal.h"
#include "EntityHelper.h"
#include "IDBEngineService.h"
#include "ExternalHelper.h"
#include "EventDef.h"
#include "IManagerConnectorService.h"
#include "Chat_ManagedDef.h"
#include "TaskStorageDef.h"
#include "IMessageDispatch.h"
#include "ChatHelper.h"
#include "OssLogDef.h"
#include "TitleDef.h"
#include "ClanHelper.h"
#include "IOSSLogService.h"
#include "TaskDef.h"

// 构造函数
ClanScenService::ClanScenService()
{
	
}

// 析构函数
ClanScenService::~ClanScenService()
{
	
}

void ClanScenService::onStop()
{
    IEventEngine *pEventEngine = gServerGlobal->getEventEngine();
    if (pEventEngine != NULL)
    {
        // 取消订阅角色事件
        pEventEngine->UnSubscibe(this, EVENT_GAME_ACTOR_LOGIN, SOURCE_TYPE_PERSON, 0);
        pEventEngine->UnSubscibe(this, EVENT_GAME_ACTOR_READTASKDATA, SOURCE_TYPE_PERSON, 0);
    }

    IMessageDispatch *pMessageDispatch = gServerGlobal->getMessageDispatch();
    if (pMessageDispatch != NULL)
    {
        // 自己需要处理消息
        pMessageDispatch->unregisterTransmitHandler(MSG_MODULEID_CLAN);
        // 取消注册响应客户端消息
        pMessageDispatch->unregisterMessageHandler(MSG_MODULEID_CLAN);
    }

    IDBEngineService *pDBEngine = gServerGlobal->getDBEngineService();
    if (pDBEngine != NULL)
    {
        pDBEngine->unregisterHandler(this);
    }
}

void ClanScenService::release()
{
	TraceLn(__FUNCTION__);
    delete this;
}

bool ClanScenService::create()
{
    TraceLn(__FUNCTION__);

	gServerGlobal->getDBEngineService()->registerHandler(this, __FUNCTION__);

	IMessageDispatch *pMessageDispatch = gServerGlobal->getMessageDispatch();
	if (pMessageDispatch == NULL)
		return false;
	// 自己需要处理消息
	pMessageDispatch->registerTransmitHandler(MSG_MODULEID_CLAN, this);
	// 处理客户端发送过来的消息
	pMessageDispatch->registerMessageHandler(MSG_MODULEID_CLAN, this);

	IEventEngine *pEventEngine = gServerGlobal->getEventEngine();
	if (pEventEngine == NULL)
		return false;
	// 订阅角色事件
	pEventEngine->Subscibe(this, EVENT_GAME_ACTOR_LOGIN, SOURCE_TYPE_PERSON, 0, __FUNCTION__);
	pEventEngine->Subscibe(this, EVENT_GAME_ACTOR_READTASKDATA, SOURCE_TYPE_PERSON, 0, __FUNCTION__);

    return true;
}

// 获取联盟名称
string ClanScenService::getClanName(DWORD nClanID)
{
	CClan *pClan = findClan(nClanID);
	if (pClan == NULL)
		return "";
	return pClan->getName();
}

// 通知服务器列表已经更新
void ClanScenService::onServerListUpdated()
{

}

// 通知服务器信息更新
void ClanScenService::onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData)
{

}

void ClanScenService::handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg)
{
    void *data = (void *)msg->c_str();
    int nLen = (int)msg->size();

    switch (head.byKeyAction)
    {
        // 建立联盟
    case MSG_CLAN_CREATE:
        {
            onTransmitCreateClan((LPCSTR)data, nLen);
        }
        break;
        // 删除联盟
    case MSG_CLAN_REMOVECLAN_ZONE:
        {
            onTransmitRemoveClan((LPCSTR)data, nLen);
        }
        break;
        // 更新联盟（没有则添加）
    case MSG_CLAN_UPDATECLAN_ZONE:
        {
            onTransmitUpdateClan((LPCSTR)data, nLen);
        }
        break;
        // 社会服向场景服更新联盟属性
    case MSG_CLAN_UPDATECLANPROP_ZONE:
        {
            onTransmitUpdateClanProp((LPCSTR)data, nLen);
        }
        break;
        // 社会服向场景服删除成员
    case MSG_CLAN_REMOVEMEMBER_ZONE:
        {
            onTransmitRemoveMember((LPCSTR)data, nLen);
        }
        break;
        // 社会服向场景服更新成员数据
    case MSG_CLAN_UPDATEMEMBER_ZONE:
        {
            onTransmitUpdateZoneMember((LPCSTR)data, nLen);
        }
        break;
    case MSG_CLAN_UPDATEID:
        {
            onTransmitUpdateClanID((LPCSTR)data, nLen);
        }
        break;
    case MSG_CLAN_ZONEPOSTMSG:
        {
            onTransmitPostMsg((LPCSTR)data, nLen);
        }
        break;
    case MSG_CLAN_UPDATESELFCLANINFO:
        {
            onTransmitUpdateSelfClanInfo(head.byKeyAction, (LPCSTR)data, nLen);
        }
        break;
    case MSG_CLAN_SENDREQUESTCARDINFO:
        {
            onTransmitSendRequestCardInfo((LPCSTR)data, nLen);
        }
        break;
    case MSG_CLAN_REQUEST_CARD:
        {
            onTransmitRequestCard((LPCSTR)data, nLen);
        }
        break;
    case MSG_CLAN_DONATE_CARD:
        {
            onTransmitDonateCard((LPCSTR)data, nLen);
        }
        break;
    case MSG_CLAN_ADD_DONATE_CARD:
        {
            onTransmitAddDonateCard((LPCSTR)data, nLen);
        }
        break;
    case MSG_CLAN_REQUEST_CARD_LIST:
        {
            onTransmitClanRequestCardList((LPCSTR)data, nLen);
        }
        break;
    case MSG_CLAN_UPDATE_REQUEST_CARD:
        {
            onTransmitClanUpdateRequestCard((LPCSTR)data, nLen);
        }
        break;
    default:
        break;
    }
}

void ClanScenService::handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg)
{
    if (isPublicGameWorld() == true)
        return;
	/*
	switch (head->byKeyAction)
	{
	default:
		{
			ErrorLn(__FUNCTION__": unknown pHead->byKeyAction=" << head->byKeyAction);
		}
		break;
	}
	*/

}

int ClanScenService::getNumProp(DWORD dwClaneID, DWORD dwProp)
{
    CClan* pClan = findClan(dwClaneID);
    if(pClan == NULL)
    {
        return 0;
    }

    return pClan->getNumProp(dwProp);
}

bool ClanScenService::setNumProp(DWORD dwClanID, DWORD dwProp, int nValue)
{
	CClan* pClan = findClan(dwClanID);
	if(pClan == NULL)
	{
		return false;
	}

	switch (dwProp)
	{
		case emClanProp_WeekActivity:
		{
			nValue = nValue < 0 ? 0 : nValue;
			
			if (!pClan->coverNumProp(dwProp, nValue + pClan->getNumProp(dwProp)))
			{
				return false;
			}

			msg_clan_updateintprop_os sendData;
			sendData.dwClanID = dwClanID;
			sendData.nPropID = dwProp;
			sendData.nValue = nValue;
			g_EHelper.sendClanSocial(MSG_CLAN_CLAN, MSG_CLAN_UPDATECLANPROP_ZONE, dwClanID, &sendData, sizeof(sendData));
			TraceLn(__FUNCTION__" set clan weekActivity success=" << nValue);
		}
		break;
		case emClanProp_LegendCupCount:
		{
			if (!pClan->coverNumProp(dwProp, nValue))
			{
				return false;
			}

			msg_clan_updateintprop_os sendData;
			sendData.dwClanID = dwClanID;
			sendData.nPropID = dwProp;
			sendData.nValue = nValue;
			g_EHelper.sendClanSocial(MSG_CLAN_CLAN, MSG_CLAN_UPDATECLANPROP_ZONE, dwClanID, &sendData, sizeof(sendData));
		}
		break;
		default:
			break;
	}

	return true;
}

// 检查创建联盟杯赛
bool ClanScenService::checkCreateClanLegendCup(DWORD dwClanID,int LegendCupConfigID)
{
	CClan* pClan = findClan(dwClanID);
	if(pClan == NULL)
	{
		return false;
	}

	return pClan->checkCreateLegendCup(LegendCupConfigID);
}

////////////////////////////////////////ITransmitHandler//////////////////////////////////////////
// 消息处理
void ClanScenService::onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void *data, size_t len)
{
    
    ClanHelper clanSceneHelper;
    IClanSceneService *pClanSceneService = clanSceneHelper.m_ptr;
    if (pClanSceneService == NULL)
    {
        ErrorLn(__FUNCTION__": server ="<<server<<" uMsgID ="<<uMsgID);
        return;
    }

    PACKAGE_PTR pkg( new string((const char*)data,len));
    pClanSceneService->handleServerMsg( server, *head, pkg );

}


///////////////////////////////////////IMessageHandler/////////////////////////////////////////
// 消息处理
void ClanScenService::onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead *head, void *data, size_t len)
{
    ClanHelper clanSceneHelper;
    IClanSceneService *pClanSceneService = clanSceneHelper.m_ptr;
    if (pClanSceneService == NULL)
    {
        ErrorLn(__FUNCTION__": clientID ="<<clientID<<" uMsgID ="<<uMsgID);
        return;
    }

    PACKAGE_PTR pkg( new string((const char*)data,len));
    pClanSceneService->handleClientMsg( clientID, *head, pkg );

	
}


/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
void ClanScenService::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	if (bSrcType == SOURCE_TYPE_PERSON)
	{
		switch (wEventID)
		{
		case EVENT_GAME_ACTOR_LOGIN:
			{
				if (nLen < sizeof(event_actor_login))
					return;

				event_actor_login *pEvent = (event_actor_login*)pszContext;
				if (pEvent->eMode == elogin_online)
				{
					// 登入时请求捐赠记录
					IDBEngineService* pDBEngine	= gServerGlobal->getDBEngineService();
					if (pDBEngine != NULL)
					{
						DBREQ_PARAM_GETCLANDONATEINFO infoData;
						infoData.dwPDBID = pEvent->dwDBID;
						pDBEngine->executeSP(GAMEDB_REQUEST_GETCLANDONATEINFO, pEvent->dwDBID, (LPCSTR)&infoData, sizeof(infoData), static_cast<IDBRetSink*>(this));
					}
				}
			}
			break;
		case EVENT_GAME_ACTOR_READTASKDATA:
			{
				if (nLen < sizeof(event_actor_read_taskdata_s))
					return;

				event_actor_read_taskdata_s *pEvent = (event_actor_read_taskdata_s*)pszContext;
				SendRequestCardInfo(pEvent->dwHolder);
			}
			break;
		default:
			break;
		}
	}
}

void ClanScenService::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
    switch (nCmdID)
    {
		// 往中心服创建联盟		    -- 数据库返回处理
	case GAMEDB_REQUEST_CENTERDBCREATECLAN:
		{
			onReturnCenterCreateClan(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
		}
		break;
		// 创建联盟					-- 数据库返回处理
	case GAMEDB_REQUEST_CREATECLAN:
		{
			onReturnCreateClan(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
		}
		break;
		// 请求捐赠记录返回
	case GAMEDB_REQUEST_GETCLANDONATEINFO:
		{
			onReturnGetClanDonateInfo(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
		}
		break;
	default:
		break;
    }
}

// 根据联盟ID查找联盟
CClan* ClanScenService::findClan(DWORD dwClanID)
{
    map<DWORD, CClan*>::iterator it = m_ClanList.find(dwClanID);
    if (it == m_ClanList.end())
        return NULL;

    return (*it).second;
}

// 根据战队ID查找联盟成员
SZoneClanMember* ClanScenService::findClanKin(DWORD dwKinID)
{
    map<DWORD, SZoneClanMember>::iterator it = m_ClanMemberList.find(dwKinID);
    if (it == m_ClanMemberList.end())
        return NULL;

    return &(*it).second;
}

// 调用DB存储过程创建联盟
void ClanScenService::onDBProcCreateClan(msg_clan_create_os *pInfo)
{
	ActorHelper actorHelper(pInfo->dwPDBID, ACTOR_DBID);
    IActorService *pPerson = actorHelper.m_ptr;
    if (pPerson == NULL)
        return;

    IDBEngineService* pDBEngine	= gServerGlobal->getDBEngineService();
    SClanConfig* pConfig = g_EHelper.getConfig();
    if (pDBEngine == NULL || pConfig == NULL)
        return;

    // 检查玩家联盟有联盟
    int nClanID = pPerson->getProperty_Integer(PROPERTY_CLAN);
    if (nClanID > 0)
    {
        g_EHelper.showSystemMessage(pInfo->dwPDBID, CHAT_TIP_CLAN_CREATE_HAVE_CLAN);
        return;
    }

    // 检查玩家等级
    if (pPerson->getProperty_Integer(PROPERTY_LEVEL) < pConfig->nCreateLevel)
    {
        char szText[32] = {0};
        sprintf_s(szText, sizeof(szText), "%d", pConfig->nCreateLevel);
        g_EHelper.showSystemMessage(pInfo->dwPDBID, CHAT_TIP_CLAN_CREATE_LIMIT_LEVEL, szText);
        return;
    }

    // 检查玩家金币数
    if (pConfig->nCreateMoney < 0)
    {
        ErrorLn(__FUNCTION__": config check create clan money is < 0!")
        return;
    }

    if (!pPerson->canAddMoney(0, -(pConfig->nCreateMoney)))	
    {
        char szText[32] = {0};
        sprintf_s(szText, sizeof(szText), "%d", pConfig->nCreateMoney);
        g_EHelper.showSystemMessage(pInfo->dwPDBID, CHAT_TIP_CLAN_CREATE_LIMIT_MONEY, szText);
        return;
    }


	//char szLogInfo[256] = { 0 };
	//ssprintf_s(szLogInfo, sizeof(szLogInfo), a2utf8("创建联盟:%s"), pInfo->szName);

 //   // 扣除玩家创建联盟
 //   pPerson->addMoney(0, -pConfig->nCreateMoney, OSSRES_DEL_CREATE_LEAGUES, szLogInfo, 0, 0);

    // 往中心数据库提交创建联盟请求
    DBREQ_PARAM_CENTERDBCREATECLAN referData;
    if (gServerGlobal->getManagerConnectorService() != NULL)
    {
        referData.dwWorldID = gServerGlobal->getManagerConnectorService()->GetWorldID();
    }

    referData.dwPDBID			= pInfo->dwPDBID;
    referData.dwKinID			= pInfo->dwKinID;
    sstrcpyn(referData.szName, pInfo->szName, sizeof(referData.szName));
    referData.nEstablishTime	= (int)(time(NULL));

    if (pDBEngine->executeSP(GAMEDB_REQUEST_CENTERDBCREATECLAN, pInfo->dwPDBID,
        (LPCSTR)&referData, sizeof(referData), static_cast<IDBRetSink *>(this)))
    {
		SCreateClanInfo sInfo;
		sInfo.nActorID = pInfo->dwPDBID;
		sstrcpyn(sInfo.szClanManifesto, pInfo->szManifesto, sizeof(sInfo.szClanManifesto));
		sInfo.nFlag = pInfo->nClanFlag;
        m_mapCrtClanPlayer[pInfo->dwPDBID] = sInfo;
    }
    else
    {
        ErrorLn(_GT("GAMEDB_REQUEST_CENTERDBCREATECLAN 请求失败，dwPDBID = ") << pInfo->dwPDBID);	
    }
}

// 回退联盟操作
//@ note   : 当通知数据库创建联盟成功返回，场景服发现创建联盟的条件已经不成立，需要取消先前的创建操作
void ClanScenService::backUpCreateClan(DWORD dwClanID, LPCSTR szTrackNote)
{
	IDBEngineService* pDBEngine = gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
		return;

	// 往中心库数据库解散联盟操作
	DBREQ_PARAM_CENTERDBDISMISSCLAN centerDismiss;
	centerDismiss.dwClanID = dwClanID;
	if(!pDBEngine->executeSP(GAMEDB_REQUEST_CENTERDBDISMISSCLAN, dwClanID, (LPCSTR)&centerDismiss,
		sizeof(centerDismiss), static_cast<IDBRetSink *>(this)))
	{
		ErrorLn(_GT("GAMEDB_REQUEST_CENTERDBDISMISSCLAN 请求失败，dwClanID = ") << dwClanID);
		return;
	}

	// 往游戏数据库提交解散联盟
	DBREQ_PARAM_DISMISSCLAN referData;
	referData.dwClanID = dwClanID;
	if(!pDBEngine->executeSP(GAMEDB_REQUEST_DISMISSCLAN, dwClanID, (LPCSTR)&referData,
		sizeof(referData), static_cast<IDBRetSink *>(this)))
	{
		ErrorLn(_GT("GAMEDB_REQUEST_DISMISSCLAN 请求失败，dwClanID = ") << dwClanID);
		return;
	}
}

// 往中心服创建联盟			-- 来自数据库
void ClanScenService::onReturnCenterCreateClan(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
	DWORD dwPDBID = (DWORD)nQueueIndex;
	map<DWORD, SCreateClanInfo>::iterator itr = m_mapCrtClanPlayer.find(dwPDBID);
	if (itr == m_mapCrtClanPlayer.end())
	{
		ErrorLn(__FUNCTION__" 创建联盟异常dwPDBID = " << dwPDBID);
		return;
	}

	bool bSucc = false;
    do
    {
        IDBEngineService* pDBEngine = gServerGlobal->getDBEngineService();
        if (pDBEngine == NULL)
            break;

        // 验证数据库返回结果
        if (nDBRetCode < DBRET_CODE_SUCCEED)
        {
            if (nDBRetCode == -1 && pszDBRetDesc != NULL && pszDBRetDesc[0] != '\0')
            {
                g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_CREATE_TIP, pszDBRetDesc);
            }
            break;
        }

        if (nOutLen != sizeof(DBREQ_RESULT_CENTERDBCREATECLAN))
        {
            ErrorLn(_GT("[onReturnCenterCreateClan] 长度不对,数据库返回长度：")
                << nOutLen << _GT("结构体长度：") << sizeof(DBREQ_RESULT_CENTERDBCREATECLAN));
            break;
        }
        DBREQ_RESULT_CENTERDBCREATECLAN *pRetData = (DBREQ_RESULT_CENTERDBCREATECLAN*)pOutData;

		// 如果玩家不在线了
		ActorHelper actorHelper(dwPDBID, ACTOR_DBID);
        IActorService *pPerson = actorHelper.m_ptr;
        if (pPerson == NULL)
        {
            backUpCreateClan(pRetData->dwClanID, "联盟解散：创建联盟数据库返回，玩家不在线");
            break;
        }

        // 游戏服创建联盟
		int curTime = (int)(time(NULL));
        DBREQ_PARAM_CREATECLAN referData;
        referData.dwClanID		= pRetData->dwClanID;
        referData.dwPDBID		= pRetData->dwPDBID;
        sstrcpyn(referData.szActorName, pPerson->getName().c_str(), sizeof(referData.szActorName));
        referData.dwKinID		= pRetData->dwKinID;
        referData.nState		= Clan_ManagerDef::EMClanState::emClanState_Normal;
		referData.nLevel		= 1;
		referData.nFlag			= itr->second.nFlag;
		sstrcpyn(referData.szManifesto, itr->second.szClanManifesto, sizeof(referData.szManifesto));
        referData.nEstablishTime= curTime;
        referData.nKickTime		= curTime;
        referData.nKickNum		= 0;
		referData.nLegendCupCount = 1;
        sstrcpyn(referData.szName, pRetData->szName, sizeof(referData.szName));

        // 往数据库创建联盟操作
        if (!pDBEngine->executeSP(GAMEDB_REQUEST_CREATECLAN, pRetData->dwPDBID, (LPCSTR)&referData, 
            sizeof(referData), static_cast<IDBRetSink *>(this)))
        {
            ErrorLn(_GT("GAMEDB_REQUEST_CREATECLAN 请求失败，nPDBID = ") << pRetData->dwPDBID);
            break;
        }

        bSucc = true;
    } while (0);

    if (!bSucc)
    {
        m_mapCrtClanPlayer.erase(dwPDBID);
    }
}

// 创建联盟		-- 来自数据库
void ClanScenService::onReturnCreateClan(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
    // 验证数据库返回结果
    DWORD dwPDBID = (DWORD)nQueueIndex;

    map<DWORD, SCreateClanInfo>::iterator itr = m_mapCrtClanPlayer.find(dwPDBID);
    if (itr == m_mapCrtClanPlayer.end())
    {
        ErrorLn(__FUNCTION__" 创建联盟异常dwPDBID = " << dwPDBID);
        return;
    }

    do
    {
        if (nDBRetCode < DBRET_CODE_SUCCEED)
        {
            if (nDBRetCode == -1 && pszDBRetDesc != NULL && pszDBRetDesc[0] != '\0')
            {
                g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_CLAN_CREATE_TIP, pszDBRetDesc);
            }
            break;
        }

        if (nOutLen != sizeof(DBREQ_RESULT_CREATECLAN))
        {
            ErrorLn(_GT("[onReturnCreateClan] 返回长度不对！ nOutLen = ") 
                << nOutLen << _GT("结构体长度= ") << sizeof(DBREQ_RESULT_CREATECLAN));
            break;
        }

        SClanConfig* pConfig = g_EHelper.getConfig();
        if (pConfig == NULL)
            break;

        DBREQ_RESULT_CREATECLAN* pRetData = (DBREQ_RESULT_CREATECLAN*)pOutData;

		// 如果玩家不在线了
		ActorHelper actorHelper(dwPDBID, ACTOR_DBID);
        IActorService* pPerson = actorHelper.m_ptr;
        if (pPerson == NULL)
        {
            backUpCreateClan(pRetData->dwClanID, "创建联盟：创建联盟数据库返回，玩家不在线");
            break;
        }

        // 如果玩家加入其他联盟了
        int nClanID = pPerson->getProperty_Integer(PROPERTY_CLAN);
        if (nClanID > 0)
        {
            backUpCreateClan(pRetData->dwClanID, "创建联盟：创建联盟数据库返回，玩家已经加入其他联盟了");
            break;
        }

        // 扣除玩家金钱
		char szLogInfo[256] = { 0 };
		ssprintf_s(szLogInfo, sizeof(szLogInfo), a2utf8("创建联盟:%s"), getClanName(nClanID));
		pPerson->addMoney(0, -pConfig->nCreateMoney, OSSRES_DEL_CREATE_LEAGUES, szLogInfo, 0, 0);

        // 通知社会服创建联盟对象
        msg_clan_create_so createData;
        sstrcpyn(createData.szShaikhName, pPerson->getName().c_str(), sizeof(createData.szShaikhName));
        sstrcpyn(createData.szName, pRetData->szName, sizeof(createData.szName));
		sstrcpyn(createData.szManifesto, itr->second.szClanManifesto, sizeof(createData.szManifesto));
		createData.nClanFlag		= itr->second.nFlag;
        createData.dwPDBID			= dwPDBID;
        createData.dwKinID			= pRetData->dwKinID;
        createData.dwClanID			= pRetData->dwClanID;
        createData.nEstablishTime	= (int)(time(NULL));

        g_EHelper.sendClanSocial(MSG_CLAN_MODULE, MSG_CLAN_CREATE, 0, &createData, sizeof(createData));

		// 记录创建联盟日志
		char szText[512] = {0};
        ssprintf_s(szText, sizeof(szText), "创建联盟成功, 联盟ID：%d 联盟名：%s", createData.dwClanID, createData.szName);
        //pOSSLogServer->TrackGameNote(ENoteTrackType_Clan, createData.dwClanID, szText);

        // 发出创建联盟事件
        IEventEngine * pEventEngine = gServerGlobal->getEventEngine();
        if (pEventEngine != NULL)
        {
            event_clan_create eventData;
            eventData.dwClanID = pRetData->dwClanID;
            // 发送ID改变事件
            pEventEngine->FireExecute(EVENT_CLAN_CREATE, SOURCE_TYPE_CLAN, pRetData->dwClanID, (LPCSTR)&eventData, sizeof(eventData));
        }
    } while (0);

    m_mapCrtClanPlayer.erase(itr);
}

// 请求捐赠记录DB返回
void ClanScenService::onReturnGetClanDonateInfo(int nDBRetCode, char *pszDBRetDesc, int nQueueIndex, char *pOutData, int nOutLen)
{
	IDBEngineService *pDBEngine = gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
		return;

	if (nDBRetCode < DBRET_CODE_SUCCEED)
	{
		CLAN_ERRORLN(__FUNCTION__" nDBRetCode = " << nDBRetCode);
		return;
	}

	if (nOutLen < sizeof(DBREQ_RESULT_GETCLANDONATEINFO))
	{
		CLAN_ERRORLN(__FUNCTION__" error1 nOutLen = " << nOutLen);
		return;
	}

	DBREQ_RESULT_GETCLANDONATEINFO *pDonateInfo = (DBREQ_RESULT_GETCLANDONATEINFO*)pOutData;
	pOutData += sizeof(DBREQ_RESULT_GETCLANDONATEINFO);
	if (pDonateInfo->nCount == 0)
		return;

	if (nOutLen != sizeof(DBREQ_RESULT_GETCLANDONATEINFO) + pDonateInfo->nCount * sizeof(DBREQ_TABLE_CLANDONATEINFO))
	{
		CLAN_ERRORLN(__FUNCTION__" error2 nOutLen = " << nOutLen << " nCount = " << pDonateInfo->nCount);
		return;
	}

	ActorHelper actorHelper(pDonateInfo->dwPDBID, ACTOR_DBID);
	IActorService *pActor = actorHelper.m_ptr;
	if (pActor == NULL)
		return;

	DBREQ_TABLE_CLANDONATEINFO *pDonateSubInfo = (DBREQ_TABLE_CLANDONATEINFO*)pOutData;
	for (int i = 0; i < pDonateInfo->nCount; ++i)
	{
		DWORD dwCardID = pDonateSubInfo->dwCardID;
		int nCardCount = pDonateSubInfo->dwCardNum;
		pActor->addHeroCardInfo(&dwCardID, &nCardCount);

		// 移除捐赠记录
		DBREQ_PARAM_REMOVECLANDONATEINFO removeData;
		removeData.dwPDBID = pDonateInfo->dwPDBID;
		removeData.dwIndex = pDonateSubInfo->dwIndex;
		pDBEngine->executeSP(GAMEDB_REQUEST_REMOVECLANDONATEINFO, pDonateInfo->dwPDBID, (LPCSTR)&removeData, sizeof(removeData), static_cast<IDBRetSink*>(this));

		// 提示客户端
		ChatHelper chatHelper;
		if (chatHelper.m_ptr != NULL)
		{
			char szParam[32] = {0};
			ssprintf_s(szParam, sizeof(szParam), "%d;%d", pDonateSubInfo->dwCardNum, pDonateSubInfo->dwCardID);
			chatHelper.m_ptr->broadcastActorTipMessage(pDonateSubInfo->dwDonatePDBID, pDonateSubInfo->szDonateActorName, CHAT_CHANNEL_CLAN, 0, CHAT_TIP_CLAN_ACTOR_DONATE_CARD_TIP, szParam, pDonateInfo->dwPDBID);
		}

		++pDonateSubInfo;
	}
}

// 创建联盟
void ClanScenService::onTransmitCreateClan(LPCSTR pszMsg, int nLen)
{
    if (pszMsg == NULL || nLen != sizeof(msg_clan_create_os))
        return;

    msg_clan_create_os *pRecvData = (msg_clan_create_os*)pszMsg;
    onDBProcCreateClan(pRecvData);
}

// 删除联盟
void ClanScenService::onTransmitRemoveClan(LPCSTR pszMsg, int nLen)
{
    if (pszMsg == NULL || nLen != sizeof(msg_clan_removeclan_os))
        return;

    msg_clan_removeclan_os* pRecvData = (msg_clan_removeclan_os*)pszMsg;
	if (pRecvData->dwClanID == 0)
	{
		// 清除所有联盟内容
		map<DWORD, CClan*>::iterator iter = m_ClanList.begin();
		for ( ; iter != m_ClanList.end(); ++iter)
		{
			SAFE_RELEASE(iter->second);
		}
		m_ClanList.clear();
	}
	else
	{
		CClan *pClan = findClan(pRecvData->dwClanID);
		if (pClan == NULL)
			return;

		m_ClanList.erase(pRecvData->dwClanID);

		// 释放联盟对象
		SAFE_RELEASE(pClan);
	}
}

// 更新联盟（没有则添加）
void ClanScenService::onTransmitUpdateClan(LPCSTR pszMsg, int nLen)
{
    if (pszMsg == NULL || nLen < sizeof(msg_clan_updateclan_os))
        return;

    msg_clan_updateclan_os *pRecvData = (msg_clan_updateclan_os*)pszMsg;
    CClan *pClan = (CClan*)findClan(pRecvData->dwID);
    if (pClan == NULL)
    {
        pClan = new CClan();
        if (pClan == NULL)
        {
            EmphasisLn("new CClan() 失败")
            return;
        }

        if (pClan->create(pszMsg, nLen) == false)
        {
            SAFE_RELEASE(pClan);
            EmphasisLn("创建联盟对象失败");
            return;
        }

        m_ClanList[pClan->getNumProp(emClanProp_ID)] = pClan;
    }
    else
    {
        pClan->updateData(pszMsg, nLen);
    }
}
// 更新联盟属性
void ClanScenService::onTransmitUpdateClanProp(LPCSTR pszMsg, int nLen)
{
    if (pszMsg == NULL || nLen != sizeof(msg_clan_updateintprop_os))
        return;

    msg_clan_updateintprop_os *pRecvData = (msg_clan_updateintprop_os*)pszMsg;
    CClan *pClan = (CClan*)findClan(pRecvData->dwClanID);
    if (pClan == NULL)
        return;

    pClan->coverNumProp(pRecvData->nPropID, pRecvData->nValue);
}

// 同步数据
void ClanScenService::onTransmitSynchroFinish(LPCSTR pszMsg, int nLen)
{
    IEventEngine *pEventEngine = gServerGlobal->getEventEngine();
    if (pEventEngine == NULL)
        return;

    // 发送ID改变事件
    pEventEngine->FireExecute(EVENT_CLAN_SYNCHRO_FINISH, SOURCE_TYPE_CLAN, 0, "", 0);
}

// 删除成员
void ClanScenService::onTransmitRemoveMember(LPCSTR pszMsg, int nLen)
{
    if (pszMsg == NULL || nLen != sizeof(msg_clan_removekin_os))
        return;

    msg_clan_removekin_os *pRecvData = (msg_clan_removekin_os*)pszMsg;
    m_ClanMemberList.erase(pRecvData->dwKinID);

    // 添加到战队成员列表
    CClan *pClan = (CClan*)findClan(pRecvData->dwClanID);
    if (pClan != NULL)
    {
        pClan->removeMember(pRecvData->dwKinID);
    }
}

// 更新成员（没有则添加）
void ClanScenService::onTransmitUpdateZoneMember(LPCSTR pszMsg, int nLen)
{
    if (pszMsg == NULL || nLen != sizeof(SZoneClanMember))
        return;

    SZoneClanMember* pRecvData = (SZoneClanMember*)pszMsg;
    m_ClanMemberList[pRecvData->dwKinID] = *pRecvData;

    // 添加到战队成员列表
    CClan *pClan = (CClan*)findClan(pRecvData->dwClanID);
    if (pClan != NULL)
    {
        pClan->addMember(pRecvData->dwKinID);
    }
}

// 设置玩家的联盟ID
void ClanScenService::onTransmitUpdateClanID(LPCSTR pszMsg, int nLen)
{
    if (pszMsg == NULL || nLen != sizeof(msg_clan_setclanid_os))
        return;

    msg_clan_setclanid_os *pRecvData = (msg_clan_setclanid_os*)pszMsg;

	// 设置联盟ID属性
	ActorHelper actorHelper(pRecvData->dwPDBID, ACTOR_DBID);
    IActorService *pActorService = actorHelper.m_ptr;
    if (pActorService != NULL)
    {
        pActorService->setProperty_Integer(PROPERTY_CLAN, pRecvData->dwClanID);

        if (pRecvData->dwClanID > 0)
        {
            string clanName = getClanName(pRecvData->dwClanID);
            if (clanName.length() > 0)
            {
                pActorService->AddTitle(TITLE_INDEX_CLAN, (char *)clanName.c_str(), clanName.length() + 1, true);
            }
            
        }
        else
        {
            pActorService->deleteTitle(TITLE_INDEX_CLAN);
        }
    }
}

// 转发消息
void ClanScenService::onTransmitPostMsg(LPCSTR pszMsg, int nLen)
{
    if (pszMsg == NULL || nLen < sizeof(msg_clan_zonepostmsg_os))
        return;

    msg_clan_zonepostmsg_os* pRecvData = (msg_clan_zonepostmsg_os*)pszMsg;

    obuf512 ob;
    ob.push_back((pszMsg + sizeof(msg_clan_zonepostmsg_os)), nLen - sizeof(msg_clan_zonepostmsg_os));
    g_EHelper.send2Client(pRecvData->dwPDBID, MSG_MODULEID_CLAN, (BYTE)pRecvData->dwMsgID, ob.data(), ob.size());
}

// 更新主角联盟信息终转
void ClanScenService::onTransmitUpdateSelfClanInfo(DWORD dwMsgID, LPCSTR pszMsg, int nLen)
{
    if (pszMsg == NULL || nLen < sizeof(DWORD))
        return;
    DWORD dwPDBID = *(DWORD*)pszMsg;

    obuf512 ob;
    ob.push_back((pszMsg + sizeof(DWORD)), nLen - sizeof(DWORD));
    g_EHelper.send2Client(dwPDBID, MSG_MODULEID_CLAN, dwMsgID, ob.data(), ob.size());
}

// 发送捐卡信息到客户端
void ClanScenService::onTransmitSendRequestCardInfo(LPCSTR pszMsg, int nLen)
{
	if (pszMsg == NULL || nLen < sizeof(msg_clan_sendrequestcardinfo_os))
		return;

	msg_clan_sendrequestcardinfo_os *pRequestCard = (msg_clan_sendrequestcardinfo_os*)pszMsg;
	SendRequestCardInfo(pRequestCard->dwPDBID);
}

// 请求卡牌支援
void ClanScenService::onTransmitRequestCard(LPCSTR pszMsg, int nLen)
{
    if (pszMsg == NULL || nLen < sizeof(msg_clan_requestcard_os))
        return;
    msg_clan_requestcard_os *pRequestCard = (msg_clan_requestcard_os*)pszMsg;

	ActorHelper actorHelper(pRequestCard->dwPDBID, ACTOR_DBID);
    IActorService* pActor = actorHelper.m_ptr;
    if (pActor == NULL)
        return;

	int nClanID = pActor->getProperty_Integer(PROPERTY_CLAN);
	if (nClanID <= 0)
	{
		g_EHelper.showSystemMessage(pRequestCard->dwPDBID, CHAT_TIP_CLAN_CARD_NO_CLAN);
		return;
	}

	int nLevel = pActor->getProperty_Integer(PROPERTY_LEVEL);
	SClanCardSchemeInfo *pClanCardInfo = g_EHelper.getClanCardInfo(nLevel);
	if (pClanCardInfo == NULL)
	{
		CLAN_ERRORLN(__FUNCTION__" pClanCardInfo == NULL nLevel=" << nLevel << "dbid = " << pRequestCard->dwPDBID);
		return;
	}

	DWORD dwLastRequestTime = g_EHelper.getActorTaskData(pActor, TASKKEY_ACTOR_CLANLASTREQUESTCARDTIME);
	// 在冷却中
	DWORD dwCurTime = (DWORD)time(NULL);
	if (dwCurTime < dwLastRequestTime + pClanCardInfo->nRequestCooldownTime)
	{
		g_EHelper.showSystemMessage(pRequestCard->dwPDBID, CHAT_TIP_CLAN_CARD_COOL);
		return;
	}

    int nDayRequestCount = g_EHelper.getActorTaskData(pActor, TASKKEY_ACTOR_CLANREQUESTCARDCOUNT);
    if (g_EHelper.DiffDay(dwLastRequestTime, dwCurTime) > 0)
    {
        nDayRequestCount = 0;
    }
	// 超过日次数
	if (nDayRequestCount >= pClanCardInfo->nRequestDayCountLimit)
	{
		g_EHelper.showSystemMessage(pRequestCard->dwPDBID, CHAT_TIP_CLAN_CARD_DAY_COUNT);
		return;
	}

    int nDayGetCount = g_EHelper.getActorTaskData(pActor, TASKKEY_ACTOR_CLANGETCARDCOUNT);
    int dwLastGetCardTime = g_EHelper.getActorTaskData(pActor, TASKKEY_ACTOR_CLANLASTGETCARDTIME);
    if (g_EHelper.DiffDay(dwLastGetCardTime, dwCurTime) > 0)
    {
        nDayGetCount = 0;
    }
	if (nDayGetCount >= pClanCardInfo->nRequestDayNumLimit)
	{
		g_EHelper.showSystemMessage(pRequestCard->dwPDBID, CHAT_TIP_CLAN_CARD_DAY_COUNT);
		return;
	}

	ISchemeHeroCard *pHeroCard = gServerGlobal->getSchemeCenter()->getSchemeHeroCard();
	if (pHeroCard == NULL)
		return;

	SHeroCardScheme *pCardInfo = pHeroCard->getByCardID(pRequestCard->nCardID);
	if (pCardInfo == NULL)
	{
		CLAN_ERRORLN(__FUNCTION__" pCardInfo == NULL nCardID =" << pRequestCard->nCardID << "dbid = " << pRequestCard->dwPDBID);
		return;
	}

	bool bFound = false;
	for (int i = 0; i < CARD_HERO_MAXCOUNT; ++i)
	{
		if (pCardInfo->arrayHeroID[i] == 0)
			break;

		SHeroInfo heroInfo = pActor->getHeroInfo(pCardInfo->arrayHeroID[i]);
		if (heroInfo.nHeroID > 0)
		{
			bFound = true;
			break;
		}
	}

	if (!bFound)
	{
		g_EHelper.showSystemMessage(pRequestCard->dwPDBID, CHAT_TIP_CLAN_CARD_NO_FORMALHERO);
		return;
	}

	// 假定已经成功了，直接增加请求计数
	pActor->addTaskDataInt(TASKKEY_ACTOR_CLANLASTREQUESTCARDTIME, dwCurTime);
	pActor->addTaskDataInt(TASKKEY_ACTOR_CLANREQUESTCARDCOUNT, nDayRequestCount + 1);

    msg_clan_requestcard_so requestCard;
    requestCard.dwPDBID = pRequestCard->dwPDBID;
	requestCard.nCardID = pRequestCard->nCardID;
	requestCard.nLevel = nLevel;
	requestCard.nDayGetCount = nDayGetCount;
    g_EHelper.sendClanSocial(MSG_CLAN_CLAN, MSG_CLAN_REQUEST_CARD, nClanID, &requestCard, sizeof(requestCard));
}

// 捐赠卡牌
void ClanScenService::onTransmitDonateCard(LPCSTR pszMsg, int nLen)
{
    if (pszMsg == NULL || nLen < sizeof(msg_clan_donatecard_os))
        return;

    msg_clan_donatecard_os *pDonateCard = (msg_clan_donatecard_os*)pszMsg;

    // 判断参数
    if (pDonateCard->nCardCount <= 0)
        return;

	// 捐赠玩家不在线，不处理
	ActorHelper actorHelper(pDonateCard->dwPDBID, ACTOR_DBID);
    IActorService *pActor = actorHelper.m_ptr;
    if (pActor == NULL)
        return;

    int nClanID = pActor->getProperty_Integer(PROPERTY_CLAN);
    if (nClanID <= 0)
    {
        g_EHelper.showSystemMessage(pDonateCard->dwPDBID, CHAT_TIP_CLAN_CARD_DONATE_NO_CLAN);
        return;
	}

	SClanCardSchemeInfo *pSelfCardInfo = g_EHelper.getClanCardInfo(pDonateCard->nDonateLevel);
	if (pSelfCardInfo == NULL)
		return;

	
    DWORD dwCurTime = (DWORD)time(NULL);
    int nDayDonateCount = g_EHelper.getActorTaskData(pActor, TASKKEY_ACTOR_CLANDONATECARDCOUNT);
    int dwLastDonateCardTime = g_EHelper.getActorTaskData(pActor, TASKKEY_ACTOR_CLANLASTDONATECARDTIME);
    if (g_EHelper.DiffDay(dwLastDonateCardTime, dwCurTime) > 0)
    {
        nDayDonateCount = 0;
    }
    // 判断自己单日捐献是否达到上限
	if (nDayDonateCount >= pSelfCardInfo->nDonateDayNumLimit)
	{
		g_EHelper.showSystemMessage(pDonateCard->dwPDBID, CHAT_TIP_CLAN_CARD_DAY_DONATE_COUNT);
		return;
	}

    // 删除捐赠玩家卡牌
    DWORD dwCardID = pDonateCard->nCardID;
    int nCardCount = -pDonateCard->nCardCount;
    if (!pActor->addHeroCardInfo( &dwCardID, &nCardCount ))
        return;

	// 记录日志(记录两条，捐赠者、被捐着各一条)
	IOSSLogService *pOSSLogService = gServerGlobal->getOSSLogService();
	pOSSLogService->addActorGoodsLog(pActor->getFromGameWorldID(), OSS_OPT_TYPE_PERSON, pDonateCard->dwPDBID, OSS_OPT_TYPE_PERSON, pDonateCard->dwByDBID,  OSS_GOODS_HEROCARD, OSS_RESOURCE_OPT_DEL, OSSRES_DEL_CLANDONATE, dwCardID, nCardCount, (int)time(NULL), a2utf8("联盟捐赠"));
	ActorHelper byActorHelper(pDonateCard->dwByDBID, ACTOR_DBID);
	IActorService *pByActor = byActorHelper.m_ptr;
	if (pByActor != NULL)
		pOSSLogService->addActorGoodsLog(pByActor->getFromGameWorldID(), OSS_OPT_TYPE_PERSON, pDonateCard->dwPDBID, OSS_OPT_TYPE_PERSON, pDonateCard->dwByDBID,  OSS_GOODS_HEROCARD, OSS_RESOURCE_OPT_ADD, OSSRES_ADD_CLANDONATE, dwCardID, nCardCount, (int)time(NULL), a2utf8("联盟捐赠"));


	// 记录捐赠数量
	pActor->addTaskDataInt(TASKKEY_ACTOR_CLANDONATECARDCOUNT, nDayDonateCount + pDonateCard->nCardCount);
    pActor->addTaskDataInt(TASKKEY_ACTOR_CLANLASTDONATECARDTIME, dwCurTime);

    // 给捐赠玩家增加经验和金币
    if (pSelfCardInfo->nDonatePrizeExp != 0)
	{
		pActor->addExp(pSelfCardInfo->nDonatePrizeExp, OSSRES_ADD_CLANDONATE, dwCardID);
    }
    if (pSelfCardInfo->nDonatePrizeGold != 0)
    {
		char* cardName = "";
		ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
		if (pSchemeCenter)
		{
			ISchemeHeroCard* pHeroCard = pSchemeCenter->getSchemeHeroCard();
			if (pHeroCard)
			{
				SHeroCardScheme* pCardScheme = pHeroCard->getByCardID(dwCardID);
				if (pCardScheme)
				{
					cardName = pCardScheme->szCardName;
				}
				else
				{
					ErrorLn("not find card scheme by ID=" << dwCardID);
				}
			}
		}

		char szLogInfo[256] = { 0 };
		ssprintf_s(szLogInfo, sizeof(szLogInfo), a2utf8("捐赠卡牌%s(%d)"), cardName, dwCardID);
        pActor->addMoney(0, pSelfCardInfo->nDonatePrizeGold, OSSRES_ADD_CLANDONATE, szLogInfo, dwCardID, nCardCount);
    }

	char szParam[256] = { 0 };
	ssprintf_s(szParam, sizeof(szParam), "%d", pSelfCardInfo->nDonatePrizeGold);
    g_EHelper.showSystemMessage(pDonateCard->dwPDBID, CHAT_TIP_CLAN_CARD_DONATE_SUCCEED, szParam);

    // 因捐赠玩家和被捐赠的玩家可能不同一场景，这里只操作捐赠玩家
    msg_clan_donatecard_so donateCard;
	donateCard.dwPDBID = pDonateCard->dwPDBID;
	sstrcpyn(donateCard.szName, pActor->getName().c_str(), sizeof(donateCard.szName));
    donateCard.dwByDBID = pDonateCard->dwByDBID;
    donateCard.nCardID = pDonateCard->nCardID;
    donateCard.nCardCount = pDonateCard->nCardCount;
    g_EHelper.sendClanSocial(MSG_CLAN_CLAN, MSG_CLAN_DONATE_CARD, nClanID, &donateCard, sizeof(donateCard));

	// 通知任务
	pActor->sendActorDataTaskEvent(emTaskLuaEventType_ClanDonate, 0, 0, 0);
}

// 增加捐赠卡牌
void ClanScenService::onTransmitAddDonateCard(LPCSTR pszMsg, int nLen)
{
    if (pszMsg == NULL || nLen < sizeof(msg_clan_adddonatecard_os))
        return;

    msg_clan_adddonatecard_os *pDonateCard = (msg_clan_adddonatecard_os*)pszMsg;

    // 判断参数
    if (pDonateCard->nCardCount <= 0)
        return;

	// 捐赠玩家不在线,本应社会服直接操作数据库
	ActorHelper actorHelper(pDonateCard->dwByDBID, ACTOR_DBID);
    IActorService *pActor = actorHelper.m_ptr;
    if (pActor == NULL)
    {
        ErrorLn(__FUNCTION__"dwByDBID="<<pDonateCard->dwByDBID<<",nCardID="<<pDonateCard->nCardID<<",nCardCount="<<pDonateCard->nCardCount);
        return;
	}

    // 在冷却中
    DWORD dwCurTime = (DWORD)time(NULL);
    int nDayGetCount = g_EHelper.getActorTaskData(pActor, TASKKEY_ACTOR_CLANGETCARDCOUNT);
    int dwLastGetCardTime = g_EHelper.getActorTaskData(pActor, TASKKEY_ACTOR_CLANLASTGETCARDTIME);
    if (g_EHelper.DiffDay(dwLastGetCardTime, dwCurTime) > 0)
    {
        nDayGetCount = 0;
    }
	pActor->addTaskDataInt(TASKKEY_ACTOR_CLANGETCARDCOUNT, nDayGetCount + pDonateCard->nCardCount);
    pActor->addTaskDataInt(TASKKEY_ACTOR_CLANLASTGETCARDTIME, dwCurTime);

    // 增加玩家卡牌
    DWORD dwCardID = pDonateCard->nCardID;
    int nCardCount = pDonateCard->nCardCount;
    pActor->addHeroCardInfo(&dwCardID, &nCardCount);

	char szParam[64] = {0};
	ssprintf_s(szParam, sizeof(szParam), "%s;%d", pDonateCard->szName, nCardCount);
	g_EHelper.showSystemMessage(pDonateCard->dwByDBID, CHAT_TIP_CLAN_CARD_GET_DONATE_SUCCEED, szParam);

	ChatHelper chatHelper;
	if (chatHelper.m_ptr != NULL)
	{
		char szParam[32] = {0};
		ssprintf_s(szParam, sizeof(szParam), "%d;%d", pDonateCard->nCardCount, pDonateCard->nCardID);
		chatHelper.m_ptr->broadcastActorTipMessage(pDonateCard->dwPDBID, pDonateCard->szName, CHAT_CHANNEL_CLAN, 0, CHAT_TIP_CLAN_ACTOR_DONATE_CARD_TIP, szParam, pDonateCard->dwByDBID);
	}
}

// 请求卡牌列表
void ClanScenService::onTransmitClanRequestCardList(LPCSTR pszMsg, int nLen)
{
    int nRequestcardlistLen = sizeof(msg_clan_requestcardlist_os);
    if (pszMsg == NULL || nLen < nRequestcardlistLen)
    {
        return;
    }
    msg_clan_requestcardlist_os *pRequestCardList = (msg_clan_requestcardlist_os*)pszMsg;
    
    ActorHelper actorHelper(pRequestCardList->dwPDBID, ACTOR_DBID);
    IActorService *pActor = actorHelper.m_ptr;
    if (pActor == NULL)
    {
        return;
    }

    int nLevel = pActor->getProperty_Integer(PROPERTY_LEVEL);
    SClanCardSchemeInfo *pSelfCardInfo = g_EHelper.getClanCardInfo(nLevel);
    if (pSelfCardInfo == NULL)
    {
        return;
    }

    obuf data;
    msg_clan_requestcardlist_sc cardlist;
    cardlist.nCount = pRequestCardList->nCount;
    data.push_back(&cardlist, sizeof(cardlist));

    // 取得当天捐卡次数
    DWORD dwCurTime = (DWORD)time(NULL);
    int nDayDonateCount = g_EHelper.getActorTaskData(pActor, TASKKEY_ACTOR_CLANDONATECARDCOUNT);
    int dwLastDonateCardTime = g_EHelper.getActorTaskData(pActor, TASKKEY_ACTOR_CLANLASTDONATECARDTIME);
    if (g_EHelper.DiffDay(dwLastDonateCardTime, dwCurTime) > 0)
    {
        nDayDonateCount = 0;
    }
    int nDonateCardCount = pSelfCardInfo->nDonateDayNumLimit - nDayDonateCount;

    int nCardListSingleLen = sizeof(msg_clan_requestcardlist_single);
    for (WORD i=0; i<pRequestCardList->nCount; ++i)
    {
        msg_clan_requestcardlist_single *pData = (msg_clan_requestcardlist_single *)(pszMsg + nRequestcardlistLen + i*nCardListSingleLen);
        pData->nDayDonateCardCount = nDonateCardCount;

        data.push_back(&(*pData), sizeof(msg_clan_requestcardlist_single));
    }

    // 发送到客户端
    g_EHelper.send2Client(pRequestCardList->dwPDBID, MSG_MODULEID_CLAN, MSG_CLAN_REQUEST_CARD_LIST, data.data(), data.size());
}

// 更新请求卡牌
void ClanScenService::onTransmitClanUpdateRequestCard(LPCSTR pszMsg, int nLen)
{
    if (pszMsg == NULL || nLen != sizeof(msg_clan_updaterequestcard_os))
    {
        return;
    }
    msg_clan_updaterequestcard_os *pData = (msg_clan_updaterequestcard_os*)pszMsg;

    ActorHelper actorHelper(pData->dwSendPDBID, ACTOR_DBID);
    IActorService *pActor = actorHelper.m_ptr;
    if (pActor == NULL)
    {
        return;
    }

    int nLevel = pActor->getProperty_Integer(PROPERTY_LEVEL);
    SClanCardSchemeInfo *pSelfCardInfo = g_EHelper.getClanCardInfo(nLevel);
    if (pSelfCardInfo == NULL)
    {
        return;
    }

    // 取得当天捐卡次数
    DWORD dwCurTime = (DWORD)time(NULL);
    int nDayDonateCount = g_EHelper.getActorTaskData(pActor, TASKKEY_ACTOR_CLANDONATECARDCOUNT);
    int dwLastDonateCardTime = g_EHelper.getActorTaskData(pActor, TASKKEY_ACTOR_CLANLASTDONATECARDTIME);
    if (g_EHelper.DiffDay(dwLastDonateCardTime, dwCurTime) > 0)
    {
        nDayDonateCount = 0;
    }
    int nDonateCardCount = pSelfCardInfo->nDonateDayNumLimit - nDayDonateCount;

    obuf data;
    msg_clan_updaterequestcard_sc updaterequestcard;
    updaterequestcard.dwPDBID = pData->dwPDBID;
    sstrcpyn(updaterequestcard.szName ,pData->szName, sizeof(updaterequestcard.szName));
    updaterequestcard.nCardID = pData->nCardID;
    updaterequestcard.nCurCount = pData->nCurCount;
    updaterequestcard.nMaxCount = pData->nMaxCount;
    updaterequestcard.nDonateCardCount = pData->nDonateCardCount;
    updaterequestcard.nDayDonateCardCount = nDonateCardCount;
    data.push_back(&updaterequestcard, sizeof(updaterequestcard));
    // 发送到客户端
    g_EHelper.send2Client(pData->dwSendPDBID, MSG_MODULEID_CLAN, MSG_CLAN_UPDATE_REQUEST_CARD, data.data(), data.size());
}

// 辅助函数
// 发送捐卡信息到客户端
void ClanScenService::SendRequestCardInfo(PDBID nActorID)
{
	ActorHelper actorHelper(nActorID, ACTOR_DBID);
	IActorService *pActor = actorHelper.m_ptr;
	if (pActor == NULL)
		return;

	int nClanID = pActor->getProperty_Integer(PROPERTY_CLAN);
	if (nClanID == 0)
		return;

    CClan *pClan = (CClan*)findClan(nClanID);
    if (pClan == NULL)
        return;
	int nLevel = pClan->getNumProp(emClanProp_Level);
	SClanCardSchemeInfo *pClanCardInfo = g_EHelper.getClanCardInfo(nLevel);
	if (pClanCardInfo == NULL)
	{
		CLAN_ERRORLN(__FUNCTION__" pClanCardInfo == NULL nLevel=" << nLevel << "dbid = " << nActorID);
		return;
	}

	int nLastRequestTime = g_EHelper.getActorTaskData(pActor, TASKKEY_ACTOR_CLANLASTREQUESTCARDTIME);
	int nCurTime = (int)time(NULL);
	int nLeftCooldownTime = nLastRequestTime + pClanCardInfo->nRequestCooldownTime - nCurTime;
	if (nLeftCooldownTime < 0)
		nLeftCooldownTime = 0;

    int nDayRequestCount = g_EHelper.getActorTaskData(pActor, TASKKEY_ACTOR_CLANREQUESTCARDCOUNT);
    if (g_EHelper.DiffDay(nLastRequestTime, nCurTime) > 0)
    {
        nDayRequestCount = 0;
    }
	int nLeftRequestNum = pClanCardInfo->nRequestDayCountLimit - nDayRequestCount;
	if (nLeftRequestNum < 0)
		nLeftRequestNum = 0;
	else if (nLeftRequestNum > 0)
	{
        int nDayGetCount = g_EHelper.getActorTaskData(pActor, TASKKEY_ACTOR_CLANGETCARDCOUNT);
        int dwLastGetCardTime = g_EHelper.getActorTaskData(pActor, TASKKEY_ACTOR_CLANLASTGETCARDTIME);
        if (g_EHelper.DiffDay(dwLastGetCardTime, nCurTime) > 0)
        {
            nDayGetCount = 0;
        }
		if (nDayGetCount >= (int)pClanCardInfo->nRequestDayNumLimit)
			nLeftRequestNum = 0;
	}

	msg_clan_sendrequestcardinfo_sc sendData;
	sendData.nCooldownTime = nLeftCooldownTime;
	sendData.nLeftRequestNum = nLeftRequestNum;
	g_EHelper.send2Client(nActorID, MSG_MODULEID_CLAN, MSG_CLAN_SENDREQUESTCARDINFO, (LPCSTR)&sendData, sizeof(sendData));
}