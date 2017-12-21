#include "stdafx.h"
#include "SNSService.h"
#include "SNSDef.h"
#include "ISocialGlobal.h"
#include "IGatewayAcceptorService.h"
#include "ISNSConnectorService.h"
#include "IShareServer.h"
#include "IMessageDispatch.h"
#include "Entity_ManagedDef.h"
#include "Event_ManagedDef.h"
#include "SNSHelper.h"
#include "ISocialGlobal.h"
#include "ChatHelper.h"
#include "ClanHelper.h"
#include "IOSSLogService.h"
#include "OssLogDef.h"
#include "IDBEngine.h"
#include "IGameDatabaseDef.h"
#include "IDBEngineService.h"
#include "EventDef.h"
#include "ICenterConnectorService.h"
#include "TaskMessageDef.h"
#include "ExternalHelper.h"

SNSService::SNSService()
{
}

SNSService::~SNSService()
{
}

bool SNSService::create()
{
    IMessageDispatch* pMessageDispatch = gSocialGlobal->getMessageDispatch();
    if (pMessageDispatch == NULL)
        return false;

    pMessageDispatch->registerMessageHandler(MSG_MODULEID_SNS,this);

	IShareReceiver *pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver != NULL)
	{
		pShareReceiver->RegisterHandler(this, "SNSService::create");
	}

	IEventEngine *pEventEngine = gSocialGlobal->getEventEngine();
	if (pEventEngine == NULL)
		return false;

	// 订阅系统事件
	pEventEngine->Subscibe(this, EVENT_SYSTEM_STARTFINISH, SOURCE_TYPE_SYSTEM, 0, __FUNCTION__);

	// 注册数据库
	IDBEngineService* pDBEngine = (IDBEngineService*)gSocialGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
		return false;
	}
	pDBEngine->registerHandler(this, __FUNCTION__);
	

    return true;
}


void SNSService::OnExecute( WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen )
{
	switch(wEventID)
	{

	}
}


void SNSService::release()
{
    IMessageDispatch* pMessageDispatch = gSocialGlobal->getMessageDispatch();
    if(pMessageDispatch != NULL)
    {
        pMessageDispatch->unregisterTransmitHandler(MSG_MODULEID_SNS);
        pMessageDispatch->unregisterMessageHandler(MSG_MODULEID_SNS);
    }
    
	IShareReceiver *pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver != NULL)
	{
		pShareReceiver->UnregisterHandler(this);
	}

	IDBEngineService* pDBEngine = (IDBEngineService*)gSocialGlobal->getDBEngineService();
	if (pDBEngine != NULL)
	{
		pDBEngine->unregisterHandler(this);
	}

	IEventEngine *pEventEngine = gSocialGlobal->getEventEngine();
	if (pEventEngine != NULL)
	{
		// 订阅系统事件
		pEventEngine->UnSubscibe(this, EVENT_SYSTEM_STARTFINISH, SOURCE_TYPE_SYSTEM, 0);
	}

    delete this;
}

////////////////////////////////ISNSService//////////////////////////////////////////

void SNSService::invitePlayGame(DWORD dwInviterActorID, DWORD dwTargetActorID, DWORD dwRoomID)
{
	SSharePersonProperty inviterharePerson = gSocialGlobal->getShareReceiver()->GetSharePerson(dwInviterActorID);
	if (inviterharePerson.dwPDBID != dwInviterActorID)
	{
		ErrorLn(__FUNCTION__":inviterharePerson.dwPDBID == INVALID_PDBID");
		return;
	}

	SSharePersonProperty targetSharePerson = gSocialGlobal->getShareReceiver()->GetSharePerson(dwTargetActorID);
    if(targetSharePerson.dwPDBID != dwTargetActorID)
    {
        ErrorLn(__FUNCTION__":targetSharePerson.clientID == INVALID_CLIENT");
		return;
    }

	SMsgSNSInvitePlayGame_Notify notify;
	notify.nRoomID = dwRoomID;
    notify.dwInviteActorID = dwInviterActorID;
	sstrcpyn(notify.szInviterName, inviterharePerson.szName, sizeof(notify.szInviterName));

	obuf ob;
	fillSNSHead(ob, MSG_ENDPOINT_CLIENT, MSG_SNS_INVITE_PLAY_GAME_NOTIFY);
	ob.push_back(&notify, sizeof(notify));

	gSocialGlobal->getGatewayAcceptorService()->sendData(targetSharePerson.clientID, PACKAGE_PTR(new std::string((char const *)ob.data(), ob.size())));

}

////////////////////////////////ISharePersonHandler//////////////////////////////////////////
// 上线或者跨进程切换地图后，也是在这里得到通知
void SNSService::OnLogin(ISharePerson *pSharePerson, ELoginMode nLineType)
{
	// 先获取下SNS服务，不然接收SNSServer消息的时候会失败
	SNSHelper helper;

	if (pSharePerson == NULL)
		return;

	switch(nLineType)
	{
	case elogin_online:// 登录
		{
			onReqLogin(pSharePerson->GetProperty());
		}
		break;
	case elogin_cross_gameworld:
	case elogin_cross_process:
	case elogin_in_process:
		{
			// 更新好友场景信息
			SMsgSNSUpdateScene msg;
			msg.dwUserID = pSharePerson->GetUDBID();
			msg.dwWorldID = pSharePerson->GetWorldID();
			msg.dwSceneID = pSharePerson->GetSceneID();
			transferToSNSServer(pSharePerson->GetUDBID(), MSG_SNS_UPDATE_SCENE, &msg, sizeof(msg));
		}
		break;
	default:break;
	}

}

// 下线或者跨进程切换地图后，也是在这里得到通知，当调完以后，就无法再取到
void SNSService::OnLogout(ISharePerson *pSharePerson, ELogoutMode nLineType)
{
	// 屏蔽非玩家上线通知
	if (nLineType != elogout_offline)
	{
		return;
	}

	SSharePersonProperty shareProperty = pSharePerson->GetProperty();

	obuf ob;
	SONMsgSNSHead head;
	head.dwUserID = shareProperty.dwUDBID;
	head.dwGameWorldID = shareProperty.dwWorldID;
	head.dwGameID = EMSNS_GAME_ID_TITAN;
	ob << head;

	SONMsgSNSLogout msg;
	fillActorInfo(&msg.actorInfo, shareProperty); 

	ob<<msg;

	ISNSConnectorService* pService = gSocialGlobal->getSNSConnectorService();
	if (NULL != pService)
	{
		pService->sendMessage(MSG_SNS_USER_LOGOUT, ob.data(), ob.size());
	}	
}

void SNSService::OnPre_Update(ISharePerson * pSharePerson, SHAREUPDATE_REASON nUpdateReason)
{

}

void SNSService::OnPost_Update(ISharePerson * pSharePerson, SHAREUPDATE_REASON nUpdateReason)
{
	bool bUpdate = false;

	SMsgSNSUpdateActorInfo msg;
	obuf ob;
	switch (nUpdateReason)
	{
	case SHAREUPDATE_REASON_CHANGE_ACTORNAME:
		{
			msg.nUpdateReson = ESNSActorInfoUpdateType_Name;

			char szName[ACTORNAME_MAXSIZE] = { 0 };
			sstrcpyn(szName, pSharePerson->GetName(), sizeof(szName));

			ob << msg;
			ob.push_back(szName, sizeof(szName));

			bUpdate = true;
		}
		break;
	case SHAREUPDATE_REASON_CHANGE_GAMESTATE:
		{
			msg.nUpdateReson = ESNSActorInfoUpdateType_GameState;

			int nGameState = pSharePerson->GetGameState();

			ob << msg;
			ob << nGameState;

			bUpdate = true;
		}
		break;
	case SHAREUPDATE_REASON_CHANGE_CLANID:
		{
			msg.nUpdateReson = ESNSActorInfoUpdateType_ClanName;

			SSharePersonProperty& Prop = pSharePerson->GetProperty();
			if (Prop.dwPDBID == INVALID_PDBID)
				break;
		
			string clanName = g_EHelper.getClanName(Prop.dwClanID);

			ob << msg;
			ob.push_back(clanName.c_str(), clanName.size());

			bUpdate = true;
		}
		break;
	case SHAREUPDATE_REASON_UPDATE_MATCHRANK:
		{
			msg.nUpdateReson = ESNSActorInfoUpdateType_MatchRankInfo;

			DWORD dwMatchScore = 0;	// 排位赛分数
			DWORD dwTotalMatchNum = 0;	// 所有类型总场数
			DWORD dwTotalWinNum = 0;	// 所有类型胜利场数
			for (int i = MatchType_None; i < MatchType_MaxID; ++i)
			{
				const ShareMatchRankInfo* pInfo = pSharePerson->GetMatchRankInfo((EMMatchType)i);
				if (pInfo != NULL)
				{
					if (i == MatchType_Rank)
					{
						// 排位赛分数
						dwMatchScore = pInfo->wMatchScore;
					}

					dwTotalMatchNum += pInfo->wMatchNum;
					dwTotalWinNum += pInfo->wWinNum;
				}
			}

			ob << msg;
			ob << dwMatchScore;
			ob << dwTotalMatchNum;
			ob << dwTotalWinNum;

			bUpdate = true;
		}
	default:break;
	}

	if (bUpdate)
	{
		transferToSNSServer(pSharePerson->GetUDBID(), MSG_SNS_UPDATE_ACTOR_INFO, ob.data(), ob.size());
	}
}

////////////////////////////////IMessageHandler//////////////////////////////////////////
void SNSService::onMessage( ClientID clientID, ulong uMsgID, SNetMsgHead* head, void* data, size_t len )
{
	if (len < 0 || NULL == data)
	{
		return;
	}

	IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver==NULL)
	{
		return;
	}
	SSharePersonProperty shareProperty = pShareReceiver->GetSharePersonByClientID(clientID);
	if (shareProperty.dwUDBID == INVALID_UDBID)
	{
		return;
	}

	SNS_TRACELN(__FUNCTION__":MsgID=" << uMsgID << ",len=" << len << ",name=" << shareProperty.szName << ",pdbid=" << shareProperty.dwPDBID);

	switch(uMsgID)
	{
	case MSG_SNS_USER_LOGIN:
		{
			//onReqLogin(shareProperty, head, data, len);
		}
		break;
	case MSG_SNS_UPDATE_REMARK:
		{
			onReqUpdateRemark(shareProperty, head, data, len);
		}
		break;
	case MSG_SNS_UPDATE_BUDDY:
		{
			onReqUpdateBuddy(shareProperty, head, data, len);
		}
		break;
	case MSG_SNS_RSP_ADD_BUDDY:
		{
			onReqRspAddBuddy(shareProperty, head, data, len);
		}
		break;
	case MSG_SNS_REQ_ADD_BUDDY_BY_NAME:
		{
			onReqAddBuddyByName(shareProperty, head, data, len);
		}
		break;
	case MSG_SNS_REQ_INVITE_PLAYGAME:
		{
			onReqInvitePlayGame(shareProperty, head, data, len);
		}
		break;
	case MSG_SNS_REQ_INVITE_MATCH_TEAM:
		{
			onReqInviteMatchTeam(shareProperty, head, data, len);
		}
		break;
	case MSG_SNS_REQ_RECOMMAND_BUDDY:
	case MSG_SNS_REQ_BUDDY_INFO:
	case MSG_SNS_REQ_BUDDYRELATION:
	case MSG_SNS_SEND_MSG_DATA:		
	case MSG_SNS_UPDATE_BUDDYGROUP:
	case MSG_SNS_CHANGE_BUDDYGROUP:
	case MSG_SNS_REQ_USERINFO:
	case MSG_SNS_REQ_CHANGE_PERMISSION:
	case MSG_SNS_REQ_CHANGE_MOOD:
	case MSG_SNS_REQ_UPDATE_BUDDY_STATUS:
		{
			transferToSNSServer(shareProperty, uMsgID, data, len);
		}
		break;
	}
}

// 填充聊天消息头
void SNSService::fillSNSHead(obuf &ob, int destEnd, int msg) const
{
	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SOCIAL;
	head.byDestEndPoint = destEnd;
	head.byKeyModule = MSG_MODULEID_SNS;
	head.byKeyAction = msg;
	ob << head;
}

void SNSService::recv_message( int msgID, const char* data, int nLen )
{
	IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver == nullptr)
	{
		return;
	}

	if (msgID == MSG_SNS_CONNECTED)
	{
		WarningLn(__FUNCTION__":Connect SNSServer Successful.");

		
		// 用2万人员
		const DWORD ARRAY_SIZE = 20480;
		SPersonInfo* pPersonArray = new SPersonInfo[ARRAY_SIZE];
		DWORD dwPersonCount = pShareReceiver->GetALLPersonInfo(pPersonArray, ARRAY_SIZE);
		if (dwPersonCount == 0)
		{
			return ;
		}
		WarningLn(__FUNCTION__":" << dwPersonCount << " people request login SNSServer...");
		for (int i = 0; i < dwPersonCount; ++i)
		{
			SSharePersonProperty pSharePerson = pShareReceiver->GetSharePerson(pPersonArray[i].dwPlayerID);
			onReqLogin(pSharePerson);
		}

		return;
	}

	if (NULL == data || nLen < sizeof(SNOMsgSNSHead))
	{
		return;
	}

	SNOMsgSNSHead* pSNSHead = (SNOMsgSNSHead*)data; 
	DWORD dwCurAccount = pSNSHead->dwUserID;
	DWORD dwCurPdbID = pSNSHead->dwPdbID;

	int nNewLen = nLen - sizeof(SNOMsgSNSHead);
	const char* pNewData = data + sizeof(SNOMsgSNSHead);

	switch(msgID)
	{
	case MSG_SNS_SYSTEM_MESSAGE:	
		{
			if (nNewLen < sizeof(SMsgSNSSystemMsg))
			{
				return;
			}

			SMsgSNSSystemMsg* msg = (SMsgSNSSystemMsg*)pNewData;

			if (nNewLen < msg->nLen)
			{
				return;
			}
			
			if (msg->nLen > SYSTEMMESSAGE_MAX_LEN)
			{
				return;
			}

			ChatHelper helper;
			string str(pNewData + sizeof(SMsgSNSSystemMsg), msg->nLen);
			
			if (NULL != helper.m_ptr)
			{
				helper.m_ptr->broadcastSystemMessage(CHAT_BROADCASTTYPE_SINGLE, dwCurPdbID, msg->nTipID, str.data());
			}

		}
		break;
	case MSG_SNS_MSG_DATA:
	case MSG_SNS_UPDATE_RELATIONSHIP:
	case MSG_SNS_BUDDY_STATUS:
	case MSG_SNS_BUDDY_LIST:		
	case MSG_SNS_BUDDY_GROUP_LIST:
	case MSG_SNS_BUDDY_RELATION_LIST:
	case MSG_SNS_BUDDY_INFO_SELF:
	case MSG_SNS_BUDDY_INFO:		
	case MSG_SNS_BUDDY_RELATIONSHIP:
	case MSG_SNS_BUDDY_REMARK:
	case MSG_SNS_USERINFO_ACK:
	case MSG_SNS_BUDDYINFO_CHANGE_NOTIFY:
	case MSG_SNS_INVITE_PLAY_GAME_NOTIFY:
	case MSG_SNS_UPDATE_SCENE_NOTIFY:
		{
			if (nNewLen < 0)
			{
				return;
			}

			obuf ob;
			fillSNSHead(ob, MSG_ENDPOINT_CLIENT, msgID);

			SSharePersonProperty pSharePerson = pShareReceiver->GetSharePerson(dwCurPdbID);
			DWORD dwClientID = pSharePerson.clientID;

			ob.push_back(pNewData, nNewLen);
			gSocialGlobal->getGatewayAcceptorService()->sendData(dwClientID, PACKAGE_PTR(new std::string((char const *)ob.data(), ob.size())));
		}
		break;
	}
}

void SNSService::fillActorInfo( SSNSActorInfo* pActorInfo, const SSharePersonProperty& shareProperty)
{
	// 获取联盟名称
	ClanHelper helper;
	IClanService *pClanService = helper.m_ptr;
	if (pClanService != NULL)
	{
		DWORD dwClanID = pClanService->findKinClan(shareProperty.dwKinID);
		string clanName = pClanService->getClanName(dwClanID);
		if (clanName.size() != 0)
		{
			sstrcpyn(pActorInfo->szClan, clanName.c_str(), sizeof(pActorInfo->szClan));
		}
	}

	pActorInfo->dwUserID = shareProperty.dwUDBID;
	pActorInfo->dwGameID = EMSNS_GAME_ID_TITAN;
	pActorInfo->dwPdbID = shareProperty.dwPDBID;
	pActorInfo->dwWorldID = shareProperty.dwWorldID;
	pActorInfo->nLevel = shareProperty.wLevel;
	pActorInfo->nMatchNum = 0;
	pActorInfo->nWinNum= 0;
	strncpy(pActorInfo->szName, shareProperty.szName, sizeof(pActorInfo->szName));
}

void SNSService::transferToSNSServer( SSharePersonProperty& shareProperty, ulong uMsgID, void* data, size_t len)
{
	SONMsgSNSHead head;
	head.dwUserID = shareProperty.dwUDBID;
	head.dwGameWorldID = shareProperty.dwWorldID;
	head.dwGameID = EMSNS_GAME_ID_TITAN;

	obuf ob;
	ob << head;
	ob.push_back(data, len);

	gSocialGlobal->getSNSConnectorService()->sendMessage(uMsgID, ob.data(), ob.size());
}

void SNSService::transferToSNSServer(UDBID dwUserID, ulong uMsgID, void * data, size_t len)
{
	SONMsgSNSHead head;
	head.dwUserID = dwUserID;
	head.dwGameWorldID = getThisGameWorldID();
	head.dwGameID = EMSNS_GAME_ID_TITAN;

	obuf ob;
	ob << head;
	ob.push_back(data, len);

	gSocialGlobal->getSNSConnectorService()->sendMessage(uMsgID, ob.data(), ob.size());
}

void SNSService::addBuddy(UDBID dwUserID, PDBID dwPdbid, BYTE bySex, char* szName, UDBID dwOptUserID, PDBID dwOptPdbid)
{
	if (dwUserID == INVALID_UDBID)
	{
		ChatHelper helper;

		if (NULL != helper.m_ptr)
		{
			helper.m_ptr->broadcastSystemMessage(CHAT_BROADCASTTYPE_SINGLE, dwOptPdbid, CHAT_TIP_CUSTOMER_TIP, a2utf8("该玩家名称不存在"));
		}
	}
	if (dwUserID == INVALID_UDBID || dwPdbid == INVALID_PDBID || dwOptUserID == INVALID_UDBID || dwOptPdbid == INVALID_PDBID)
	{
		WarningLn(__FUNCTION__": There is invalid ID. dwUserID=" << dwUserID << ", dwPdbid=" << dwPdbid << ",dwOptUserID=" << dwOptUserID << ",dwOptPdbid" << dwOptPdbid);
		return;
	}

	SMsgSNSUpdateBuddy msg;
	msg.dwOtherAccountID = dwUserID;
	msg.nOptType = EMSNS_BUDDY_UPDATE_TYPE_ADD;
	sstrcpyn(msg.szActorName, szName, sizeof(msg.szActorName));

	transferToSNSServer(dwOptUserID, MSG_SNS_UPDATE_BUDDY, &msg, sizeof(msg));

	// 加好友发事件
	IEventEngine *pEventEngine = gSocialGlobal->getEventEngine();
	if (pEventEngine != NULL)
	{
		event_sns_add_buddy event;
		event.dwOptPdbid = dwOptPdbid;
		event.dwPdbid = dwPdbid;
		event.bySex = bySex;
		pEventEngine->FireExecute(EVENT_SNS_ADD_BUDDY, SOURCE_TYPE_SNS, 0, (LPCSTR)&event, sizeof(event));
	}

	SSharePersonProperty person = gSocialGlobal->getShareReceiver()->GetSharePersonByUDBID(dwOptUserID);
	SContactApplyFriend_OC msg2;
	msg2.dwActorID = dwOptPdbid;
	msg2.dwTargetID = dwPdbid;
	msg2.bIsDifSex = person.bySex != bySex;

	obuf ob;
	
	SNetMsgHead head;
	head.byDestEndPoint = MSG_ENDPOINT_SCENE;
	head.bySrcEndPoint = MSG_ENDPOINT_SOCIAL;
	head.byKeyModule = MSG_MODULEID_TASKSTORAGE;
	head.byKeyAction = TASK_MSG_CONTACT_APPLY_FRIEND_OS;

	ob << head;
	ob << msg2;
	
	gSocialGlobal->getCenterConnectorService()->sendDataToZoneSvr(ob.data(), ob.size(), person.serverID);
}

void SNSService::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	if (nDBRetCode != DBRET_CODE_SUCCEED)
	{
		ErrorLn(__FUNCTION__": nDBRetCode != DBRET_CODE_SUCCEED, nDBRetCode=" << nDBRetCode << "," << pszDBRetDesc);
		return;
	}

	// TODO
	switch (nCmdID)
	{
	case GAMEDB_REQUEST_SNS_READ_PLAYERINFO_BY_NAME:
		{
			if (nOutLen != sizeof(DBREQ_RESULT_SNS_READ_PLAYINFO_BY_NAME))
			{
				ErrorLn(__FUNCTION__": nOutLen != sizeof(DBREQ_PARAM_READ_PLAYINFO_BY_NAME), nOutLen=" << nOutLen);
				return;
			}
			DBREQ_RESULT_SNS_READ_PLAYINFO_BY_NAME* pResult = (DBREQ_RESULT_SNS_READ_PLAYINFO_BY_NAME*)pOutData;
			if (pResult->byOptType == EMSNS_DB_OPT_TYPE_READ_PLAYERINFO_ADDBUDDY)
			{
				addBuddy(pResult->dwUserID, pResult->dwPdbid, pResult->bySex, pResult->szName, pResult->dwOptUserID, pResult->dwOptPdbid);
			}

		}
		break;
	case GAMEDB_REQUEST_SNS_READ_PLAYERINFO_BY_USERID:
		{
			if (nOutLen != sizeof(DBREQ_RESULT_SNS_READ_PLAYINFO_BY_USERID))
			{
				ErrorLn(__FUNCTION__": nOutLen != sizeof(DBREQ_PARAM_READ_PLAYINFO_BY_NAME), nOutLen=" << nOutLen);
				return;
			}
			DBREQ_RESULT_SNS_READ_PLAYINFO_BY_USERID* pResult = (DBREQ_RESULT_SNS_READ_PLAYINFO_BY_USERID*)pOutData;
			if (pResult->byOptType == EMSNS_DB_OPT_TYPE_READ_PLAYERINFO_ADDBUDDY)
			{
				addBuddy(pResult->dwUserID, pResult->dwPdbid, pResult->bySex, pResult->szName, pResult->dwOptUserID, pResult->dwOptPdbid);
			}
		}
		break;
	}
}

void SNSService::onReqLogin(SSharePersonProperty& shareProperty)
{
	if (shareProperty.dwPDBID == INVALID_PDBID)
	{
		ErrorLn("OnReqLogin,shareProperty.dwPDBID == INVALID_PDBID");
		return;
	}
	
	// 填充游戏世界信息
	SMsgSNSUserLogin msg;
	msg.gameWorldInfo.dwGameID = EMSNS_GAME_ID_TITAN;
	msg.gameWorldInfo.dwWorldID = shareProperty.dwWorldID;
	msg.gameWorldInfo.wSubID = shareProperty.serverID;

	// 填充玩家信息
	msg.actorInfo.dwUserID = shareProperty.dwUDBID;
	msg.actorInfo.dwGameID = EMSNS_GAME_ID_TITAN;
	msg.actorInfo.dwWorldID = shareProperty.dwWorldID;
	msg.actorInfo.dwServerID = shareProperty.serverID;
	msg.actorInfo.dwSceneID = shareProperty.dwSceneID;
	msg.actorInfo.dwPdbID = shareProperty.dwPDBID;
	msg.actorInfo.dwHeadID= shareProperty.nFaceID;
	sstrcpyn(msg.actorInfo.szName, shareProperty.szName, sizeof(msg.actorInfo.szName));
	msg.actorInfo.nLevel = shareProperty.wLevel;
	msg.actorInfo.dwLastLogout = time(NULL);
	//msg.actorInfo.nMatchNum
	//msg.actorInfo.nWinNum
	string clanName = g_EHelper.getClanName(shareProperty.dwClanID);
	sstrcpyn(msg.actorInfo.szClan, clanName.c_str(), sizeof(msg.actorInfo.szClan));
	//msg.actorInfo.szTitle
	IShareReceiver* pIShareReceiver = gSocialGlobal->getShareReceiver();
	if (pIShareReceiver != NULL)
	{
		DWORD dwRankScore = pIShareReceiver->GetPersonShareMatchRank(shareProperty.dwPDBID, EMMatchType::MatchType_Rank, SHARE_NUM_PARAM::SHARE_PARAM_PDBID);
		msg.actorInfo.nRankGrade = shareProperty.nRankGrade;
        msg.actorInfo.nRankScore = shareProperty.nRankScore;
	}
	//msg.actorInfo.szRankName
	msg.actorInfo.nSex = shareProperty.bySex;
	sstrcpyn(msg.actorInfo.szWorldName, g_EHelper.getWorldName().c_str(), sizeof(msg.actorInfo.szWorldName));
	
	//msg.actorInfo.dwUpdateTime


	// 填充IP
	IGatewayAcceptorService* pGatewayAService = gSocialGlobal->getGatewayAcceptorService();
	if (NULL != pGatewayAService)
	{
		sstrcpyn(msg.clientIP, pGatewayAService->getIpAddress(shareProperty.clientID).c_str(), sizeof(msg.clientIP));
	}

	SONMsgSNSHead head;
	head.dwUserID = shareProperty.dwUDBID;
	head.dwGameWorldID = shareProperty.dwWorldID;
	head.dwGameID = EMSNS_GAME_ID_TITAN;

	obuf ob;
	ob << head;
	ob << msg;

	ISNSConnectorService* pSNSCService = gSocialGlobal->getSNSConnectorService();
	if (NULL != pSNSCService)
	{
		pSNSCService->sendMessage(MSG_SNS_USER_LOGIN, ob.data(), ob.size());
	}
}

void SNSService::onReqUpdateRemark(SSharePersonProperty & shareProperty, SNetMsgHead * head, void * data, size_t len)
{
	transferToSNSServer(shareProperty, head->byKeyAction, data, len);

	if (len != sizeof(SMsgSNSUpdateRemark))
	{
		ErrorLn(__FUNCTION__ "The length of SMsgSNSUpdateRemark is too short. len=" << len);
		return;
	}

	char szText[255] = {0};
	SMsgSNSUpdateRemark *pMsg = (SMsgSNSUpdateRemark*)data;
	ssprintf_s(szText, sizeof(szText), _GT("%s[%u]把%s改成%s备注"), shareProperty.szName, shareProperty.dwUDBID, pMsg->szActorName, pMsg->szName);

	IOSSLogService * pOSSLogService = gSocialGlobal->getOSSLogService();
	if (pOSSLogService == NULL)
	{
		ErrorLn(__FUNCTION__": pOSSLogServer = NULL");
		return;
	}
	pOSSLogService->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_PERSON, shareProperty.dwPDBID, szText);

}

void SNSService::onReqUpdateBuddy(SSharePersonProperty & shareProperty, SNetMsgHead * head, void * data, size_t len)
{
	if (len != sizeof(SMsgSNSUpdateBuddy))
	{
		ErrorLn(__FUNCTION__ "The length of SONMsgSNSAddBuddy is too short. len=" << len);
		return;
	}

	IShareReceiver *pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver == NULL)
		return;

	SMsgSNSUpdateBuddy* pMsg = (SMsgSNSUpdateBuddy*)data;
	BYTE nOptType = pMsg->nOptType;

	if (nOptType == EMSNS_BUDDY_UPDATE_TYPE_ADD)
	{
		if (pMsg->dwOtherAccountID == INVALID_UDBID)
		{
			ErrorLn(__FUNCTION__": pMsg->dwOtherAccountID == INVALID_UDBID");
			return;
		}

		// 对方在线
		SSharePersonProperty otherProperty = pShareReceiver->GetSharePersonByUDBID(pMsg->dwOtherAccountID);
		if (otherProperty.dwPDBID != INVALID_PDBID)
		{
			addBuddy(otherProperty.dwUDBID, otherProperty.dwPDBID, otherProperty.bySex, otherProperty.szName, shareProperty.dwUDBID, shareProperty.dwPDBID);
		}
		else   // 对方不在线
		{
			DBREQ_PARAM_SNS_READ_PLAYINFO_BY_USERID dbParam;
			dbParam.byOptType = EMSNS_DB_OPT_TYPE_READ_PLAYERINFO_ADDBUDDY;
			dbParam.dwOptUserID = shareProperty.dwUDBID;
			dbParam.dwOptPdbid = shareProperty.dwPDBID;
			dbParam.dwUserID = pMsg->dwOtherAccountID;

			IDBEngineService* pDBEngine = (IDBEngineService*)gSocialGlobal->getDBEngineService();
			if (pDBEngine == NULL)
			{
				ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
				return;
			}
			if (!pDBEngine->executeSP(GAMEDB_REQUEST_SNS_READ_PLAYERINFO_BY_USERID, shareProperty.clientID, (LPCSTR)&dbParam, sizeof(dbParam), static_cast<IDBRetSink *>(this)))
			{
				ErrorLn(__FUNCTION__" : pDBEngine->executeSP() GAMEDB_REQUEST_SNS_READ_PLAYERINFO_BY_USERID failed");
				return;
			}
		}
	}
	else
	{
		transferToSNSServer(shareProperty, head->byKeyAction, data, len);
	}

	char szText[255] = { 0 };
	// OSS日志
	if (nOptType == EMSNS_BUDDY_UPDATE_TYPE_DEL)
	{
		ssprintf_s(szText, sizeof(szText), _GT("%s[%u]删除好友%s"), shareProperty.szName, shareProperty.dwUDBID, pMsg->szActorName);
	}
	else if (nOptType == EMSNS_BUDDY_UPDATE_TYPE_ADD)
	{
		ssprintf_s(szText, sizeof(szText), _GT("%s[%u]请求添加好友%s"), shareProperty.szName, shareProperty.dwUDBID, pMsg->szActorName);
	}

	IOSSLogService * pOSSLogService = gSocialGlobal->getOSSLogService();
	if (pOSSLogService == NULL)
	{
		ErrorLn(__FUNCTION__": pOSSLogServer = NULL");
		return;
	}
	pOSSLogService->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_PERSON, shareProperty.dwPDBID, szText);
}

void SNSService::onReqRspAddBuddy(SSharePersonProperty & shareProperty, SNetMsgHead * head, void * data, size_t len)
{
	transferToSNSServer(shareProperty, head->byKeyAction, data, len);

	if (len != sizeof(SMsgSNSRspAddBuddy))
	{
		ErrorLn(__FUNCTION__ "The length of SONMsgResAddBuddy is too short. len=" << len);
		return;
	}

	char szText[255] = { 0 };
	SMsgSNSRspAddBuddy *pMsg = (SMsgSNSRspAddBuddy*)data;
	if (pMsg->nFlag == EMSNS_RSP_ADDBUDDY_TYPEE_AGGRE)
	{
		ssprintf_s(szText, sizeof(szText), _GT("%s[%u]同意添加%s为好友"), shareProperty.szName, shareProperty.dwUDBID, pMsg->szActorName);
	}

	IOSSLogService * pOSSLogService = gSocialGlobal->getOSSLogService();
	if (pOSSLogService == NULL)
	{
		ErrorLn(__FUNCTION__": pOSSLogServer = NULL");
		return;
	}
	pOSSLogService->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_PERSON, shareProperty.dwPDBID, szText);
}

void SNSService::onReqAddBuddyByName(SSharePersonProperty & shareProperty, SNetMsgHead * head, void * data, size_t len)
{
	if (len != sizeof(SMsgSNSReqAddBuddyByName))
	{
		ErrorLn(__FUNCTION__ "The length of SMsgSNSReqAddBuddyByName is too short. len=" << len);
		return;
	}

	IShareReceiver *pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver == NULL)
		return;

	SMsgSNSReqAddBuddyByName* pMsg = (SMsgSNSReqAddBuddyByName*)data;

	// 对方在线
	SSharePersonProperty otherProperty = pShareReceiver->GetSharePerson(pMsg->szActorName);
	if (otherProperty.dwPDBID != INVALID_PDBID)
	{
		addBuddy(otherProperty.dwUDBID, otherProperty.dwPDBID, otherProperty.bySex, otherProperty.szName, shareProperty.dwUDBID, shareProperty.dwPDBID);
	}
	else  // 对方不在线
	{
		DBREQ_PARAM_SNS_READ_PLAYINFO_BY_NAME dbParam;
		dbParam.byOptType = EMSNS_DB_OPT_TYPE_READ_PLAYERINFO_ADDBUDDY;
		dbParam.dwOptUserID = shareProperty.dwUDBID;
		dbParam.dwOptPdbid = shareProperty.dwPDBID;
		sstrcpyn(dbParam.szName, pMsg->szActorName, sizeof(dbParam.szName));

		IDBEngineService* pDBEngine = (IDBEngineService*)gSocialGlobal->getDBEngineService();
		if (pDBEngine == NULL)
		{
			ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
			return;
		}
		if (!pDBEngine->executeSP(GAMEDB_REQUEST_SNS_READ_PLAYERINFO_BY_NAME, shareProperty.clientID, (LPCSTR)&dbParam, sizeof(dbParam), static_cast<IDBRetSink *>(this)))
		{
			ErrorLn(__FUNCTION__" : pDBEngine->executeSP() GAMEDB_REQUEST_SNS_READ_PLAYERINFO_BY_NAME failed");
			return;
		}
	}
}

void SNSService::onReqInvitePlayGame(SSharePersonProperty & shareProperty, SNetMsgHead * head, void * data, size_t len)
{
	if (len != sizeof(SMsgSNSReqInvitePlayGame))
	{
		ErrorLn(__FUNCTION__ "The length of SMsgSNSReqInvitePlayGame is too short. len=" << len);
		return;
	}

	IShareReceiver *pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver == NULL)
		return;

	SMsgSNSReqInvitePlayGame* pMsg = (SMsgSNSReqInvitePlayGame*)data;

	SSharePersonProperty targetShareProperty = pShareReceiver->GetSharePersonByUDBID(pMsg->nUserID);
	if (targetShareProperty.dwUDBID != pMsg->nUserID)
	{
		ErrorLn(__FUNCTION__":targetShareProperty.dwUDBID != pMsg->nUserID, nUserID=" << pMsg->nUserID);
		return;
	}

	invitePlayGame(shareProperty.dwPDBID, targetShareProperty.dwPDBID, pMsg->nRoomID);
}

void SNSService::onReqInviteMatchTeam(SSharePersonProperty& shareProperty, SNetMsgHead* head, void* data, size_t len)
{
	if (len != sizeof(SMsgSNSReqInviteMatchTeam))
	{
		ErrorLn(__FUNCTION__ "The length of SMsgSNSReqInviteMatchTeam is too short. len=" << len);
		return;
	}

	IShareReceiver *pShareReceiver = gSocialGlobal->getShareReceiver();
	if (pShareReceiver == NULL)
		return;

	SMsgSNSReqInviteMatchTeam* pMsg = (SMsgSNSReqInviteMatchTeam*)data;

	SSharePersonProperty targetShareProperty = pShareReceiver->GetSharePersonByUDBID(pMsg->nUserID);
	if (targetShareProperty.dwUDBID != pMsg->nUserID)
	{
		ErrorLn(__FUNCTION__":targetShareProperty.dwUDBID != pMsg->nUserID, nUserID=" << pMsg->nUserID);
		return;
	}

	SMsgSNSInviteMatchTeam_Notify notify;
	notify.dwTargetActorID = targetShareProperty.dwPDBID;

	obuf ob;
	fillSNSHead(ob, MSG_ENDPOINT_CLIENT, MSG_SNS_INVITE_MATCH_TEAM_NOTIFY);
	ob.push_back(&notify, sizeof(notify));

	gSocialGlobal->getGatewayAcceptorService()->sendData(shareProperty.clientID, PACKAGE_PTR(new std::string((char const *)ob.data(), ob.size())));
}

void SNSService::addBuddy( DWORD dwUserID1, DWORD dwUserID2,  const char* szActorName1, const char* szActorName2 )
{
	SONMsgSNSReqDirectAddBudy msg;
	msg.dwUserID1 = dwUserID1;
	msg.dwUserID2 = dwUserID2;

	sstrcpyn(msg.szActorName1, szActorName1, sizeof(msg.szActorName1));
	sstrcpyn(msg.szActorName2, szActorName2, sizeof(msg.szActorName2));

	SSharePersonProperty shareProperty;
	transferToSNSServer(shareProperty, MSG_SNS_REQ_DIRECT_ADD_BUDDY, &msg, sizeof(msg));
}
