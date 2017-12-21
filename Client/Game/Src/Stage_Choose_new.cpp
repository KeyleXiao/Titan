#include "stdafx.h"
#include "Stage_Choose.h"
#include "IClientGlobal.h"
#include "IClientEntity.h"
#include "Stage_Manager.h"
#include "IRenderView.h"
#include "NetMessageDef.h"
#include "SchemeMapInfo.h"
#include "IFreezeClient.h"
#include "ExternalHelper.h"
#include "ActorClient.h"
#include "IChatClient.h"
#include "DistrictList.h"


////////////////////////////////////////////////////////////////////////////////
// 采用创建角色分步骤模式
#ifdef USE_CREATE_STEP_MODE


void Stage_Choose::onEnter(void * context, int len)
{
	TraceLn(__FUNCTION__);

	IEntityClient * pEntityClient = gClientGlobal->getEntityClient();
	if (pEntityClient)
	{
		pEntityClient->registerGameViewEventHandler(GVIEWCMD_USER_SELECT_HERO, this);
		pEntityClient->registerGameViewEventHandler(GVIEWCMD_ENTER_TEST_MAP, this);
		//pEntityClient->registerGameViewEventHandler( GVIEWCMD_LOAD_CHOOSE_VOCATION,this );
		pEntityClient->registerGameViewEventHandler(GVIEWCMD_USER_CREATE_PLAYER, this);
		pEntityClient->registerGameViewEventHandler(GVIEWCMD_USER_DELETE_PLAYER, this);
		pEntityClient->registerGameViewEventHandler(GVIEWCMD_RETURN_LOGIN, this);
		pEntityClient->registerGameViewEventHandler(GVIEWCMD_USER_RANDOM_PLAYER_NAME, this);
		pEntityClient->registerGameViewEventHandler(GVIEWCMD_USER_CREATE_PLAYER_EXTEND, this);

	}

	IMessageDispatch * pMessageDispatch = gClientGlobal->getMessageDispatch();
	if (pMessageDispatch)
	{
		pMessageDispatch->registerMessageHandler(MSG_MODULEID_LOGIN, this);
	}
}

void Stage_Choose::onLeave()
{
	TraceLn(__FUNCTION__);

	IEntityClient * pEntityClient = gClientGlobal->getEntityClient();
	if (pEntityClient)
	{

		pEntityClient->unregisterGameViewEventHandler(GVIEWCMD_USER_CREATE_PLAYER_EXTEND);

		pEntityClient->unregisterGameViewEventHandler(GVIEWCMD_USER_SELECT_HERO);
		//pEntityClient->unregisterGameViewEventHandler(GVIEWCMD_LOAD_CHOOSE_VOCATION);
		pEntityClient->unregisterGameViewEventHandler(GVIEWCMD_ENTER_TEST_MAP);
		pEntityClient->unregisterGameViewEventHandler(GVIEWCMD_USER_CREATE_PLAYER);
		pEntityClient->unregisterGameViewEventHandler(GVIEWCMD_USER_DELETE_PLAYER);
		pEntityClient->unregisterGameViewEventHandler(GVIEWCMD_RETURN_LOGIN);
		pEntityClient->unregisterGameViewEventHandler(GVIEWCMD_USER_RANDOM_PLAYER_NAME);
	}

	IMessageDispatch * pMessageDispatch = gClientGlobal->getMessageDispatch();
	if (pMessageDispatch)
	{
		pMessageDispatch->unregisterMessageHandler(MSG_MODULEID_LOGIN, this);
	}
}

void Stage_Choose::update()
{

}

bool Stage_Choose::onViewEvent(int32 eventID, int nParam, const char * strParam, void * ptrParam)
{
	TraceLn("Stage_Choose::onViewEvent eventID=" << eventID << " nParam=" << nParam);

	switch (eventID)
	{
	case GVIEWCMD_ENTER_TEST_MAP:           // 进入测试场景地图
	{
		ISchemeMapInfo* pSchemeMapInfo = gClientGlobal->getSchemeCenter()->getSchemeMapInfo();
		if (!pSchemeMapInfo)
		{
			ErrorLn("Stage_Choose::onViewEvent nullptr pSchemeMapInfo");
			return false;
		}
		SMapSchemeInfo* pSMapSchemeInfo = pSchemeMapInfo->GetMapSchemeInfo(1);
		if (!pSMapSchemeInfo)
		{
			ErrorLn("Stage_Choose::onViewEvent nullptr pSMapSchemeInfo");
			return false;
		}

		gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_ENTER_TEST_MAP, pSMapSchemeInfo->lMapID, pSMapSchemeInfo->szMapFile, 0, 0);
	}
	break;

	case GVIEWCMD_USER_CREATE_PLAYER:       // 创建角色
	{
		gameview_create_player * pCmd = (gameview_create_player *)ptrParam;

		SMsgLoginServer_CreateActor msg;
		sstrcpyn(msg.szActorName, pCmd->name, sizeof(msg.szActorName));
		msg.bSex = pCmd->sex;

		SNetMsgHead head;
		head.byDestEndPoint = MSG_ENDPOINT_LOGIN;
		head.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
		head.byKeyModule = MSG_MODULEID_LOGIN;
		head.byKeyAction = MSG_LOGINSERVER_CREATE_ACTOR;

		obuf ob;
		ob << head << msg;
        net_senddata( ob.data(),ob.size() );
	}
	break;
	case GVIEWCMD_USER_CREATE_PLAYER_EXTEND:    // 创建角色附加消息
	{
		gameview_create_player_extend * pCmd = (gameview_create_player_extend *)ptrParam;

		SMsgLoginServer_CreateActor_Extend msg;
		msg.nSkinID = pCmd->skinid;     // 皮肤ID
		msg.nHeroID = pCmd->heroid;

		// 取缺省皮肤ID
		if (msg.nSkinID == 0)
		{
			ISchemeCenter *pSchemeCenter = gClientGlobal->getSchemeCenter();
			if (pSchemeCenter)
			{
				ISchemePersonModel* pPersonScheme = pSchemeCenter->getSchemePersonModel();
				if (pPersonScheme)
				{
					const PersonSkinModelItem* pItem = pPersonScheme->getPersonSkinModelItem(pCmd->heroid);

					msg.nSkinID  = pItem ? pItem->nDefaultSkinID : 0;
				}
			}
		}

		SNetMsgHead head;
		head.byDestEndPoint = MSG_ENDPOINT_LOGIN;
		head.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
		head.byKeyModule = MSG_MODULEID_LOGIN;
		head.byKeyAction = MSG_LOGINSERVER_CREATE_ACTOR_EXTEND;

		obuf ob;
		ob << head << msg;
        net_senddata( ob.data(),ob.size() );
	}
	break;

	case GVIEWCMD_USER_SELECT_HERO:         // 选中某个英雄，修改针对该英雄的描述信息
	{
		selectHero(nParam);
	}
	break;

	case GVIEWCMD_RETURN_LOGIN:               // 返回登录状态
	{
		onCmdReturnLogin();
	}
	break;

	case GVIEWCMD_USER_RANDOM_PLAYER_NAME:
	{
		onReqSearchActorName(ptrParam);
	}
	break;

	default:
		return false;
	}

	return true;
}

Stage_Choose::Stage_Choose()
{

}

void Stage_Choose::onMessage(SNetMsgHead* head, void* data, size_t len)
{
	switch (head->byKeyAction)
	{
	case MSG_LOGINSERVER_MESSAGE:
	{
		onMsgLoginNotifyClient(data, len);
	}
	break;

	case MSG_LOGINSERVER_UPDATE_ACTOR:
	{
		onMsgUpdatePlayers(data, len);
	}
	break;

	case MSG_LOGINSERVER_SEARCH_ACTORNAME_RESULT:
	{
		onMsgRecvActorName(data, len);
	}
	break;

	case MSG_LOGINSERVER_CREATE_RESULT:
	{
		if (data == NULL || len < sizeof(SMsgLoginServer_CreateActor_Result))
		{
			ErrorLn("choose state receive invalid message, len=" << len << ", sizeof=" << sizeof(SMsgLoginServer_CreateActor_Result));
			return;
		}

		SMsgLoginServer_CreateActor_Result * pMsg = (SMsgLoginServer_CreateActor_Result *)data;
		if (pMsg->nReason == CREATE_RESULT_SUCCESS)
		{
			gamelogic_create_player_result cmd;
			cmd.nResult = 1;    // 创建成功
			gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_USER_CREATE_PLAYER_RESULT, 0, 0, &cmd, sizeof(cmd));
		}
	}
	break;

	case MSG_LOGINSERVER_SWITCH_STATE:  //登陆服务器通知状态切换
	{
		if (data == NULL || len < sizeof(SMsgLoginServer_SwitchState))
		{
			ErrorLn("choose state receive invalid message, len=" << len << ", sizeof=" << sizeof(SMsgLoginServer_SwitchState));
			return;
		}

		SMsgLoginServer_SwitchState * pMsg = (SMsgLoginServer_SwitchState *)data;
		Stage_Manager *  pStageManager = gClientGlobal->getStageManager();
		if (pStageManager)
		{
			pStageManager->gotoState(pStageManager->loginServerStateSwitchLocal(pMsg->state));
		}
	}
	break;
	// 排队序号
	case MSG_LOGINSERVER_QUEUE_ORDER:
	{
		if (len < sizeof(SMsgLoginServer_QueueOrder))
		{
			ErrorLn("排队状态收到错误的数据包,MSG_LOGINSERVER_QUEUE_ORDER");
			return;
		}

		SMsgLoginServer_QueueOrder * pData = (SMsgLoginServer_QueueOrder*)data;
		// 通知客户端显示排队相关
		cmd_open_queue_wnd cmdData;
		cmdData.order = pData->order;
		cmdData.RemainTime = pData->RemainTime;
		gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_LOGIN_OPENQUEUE_VIEW, 0, 0, &cmdData, sizeof(cmdData));
	}
	break;

	case MSG_LOGINSERVER_ANNOUNCE:
	{
		if (len < sizeof(SMsgLoginServer_Announce))
		{
			ErrorLn("排队状态收到错误的数据包,MSG_LOGINSERVER_ANNOUNCE");
			return;
		}

		SMsgLoginServer_Announce * pData = (SMsgLoginServer_Announce*)data;

	}
	break;
	default:
		break;
	}
}

void Stage_Choose::onMsgUpdatePlayers(void * data, size_t len)
{
	if (len < sizeof(SMsgLoginServer_UpdateActor))
	{
		ErrorLn("Stage_Choose::onMsgUserLogin sizeof(SMsgLoginServer_UpdateActor) error=" << sizeof(SMsgLoginServer_UpdateActor));
		return;
	}

	SMsgLoginServer_UpdateActor* login_msg = (SMsgLoginServer_UpdateActor*)data;
	if (login_msg->dwLoginCode != 0)
	{
		if (login_msg->wMsgLen > 0)
		{
			string strMsg((char*)(login_msg + 1), login_msg->wMsgLen);
			ActorClient::getInstance().getPlayersError(login_msg->dwLoginCode, strMsg.c_str(), login_msg->nReason);
		}
		return;
	}

	if (login_msg->nPlayerCount == 0)
	{
		gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_JUDGE_CREATE, 0, "", NULL, 0);
	}
	else
	{
        // 更新账号信息
        ActorClient::getInstance().setUserInfo(login_msg->udbid, login_msg->szUserName);
		// 更新全局角色列表
		ActorClient::getInstance().updatePlayers(login_msg->nPlayerCount, login_msg->players, -1/*login_msg->nReason */);

		for (int i = 0; i < login_msg->nPlayerCount; ++i)
		{
			char * pszActorName = login_msg->players[i].szActorName;
			if (strlen(pszActorName) > 0)
			{
				requestEnterGame(pszActorName);
				break;
			}
		}
	}

	//登陆区ID到客户端保存
	const SClientGameServerInfo * serverInfo = DistrictList::getInstance().GetGameServerInfoByWorldID(login_msg->dwGameWorldID);
	if (serverInfo)
	{
		DistrictList::getInstance().OnEnterServer(serverInfo->nGroup, serverInfo->nDistrict, serverInfo->nServer);
	}

}

void Stage_Choose::onMsgRecvActorName(void * data, size_t len)
{
	if (len < sizeof(SMsgLoginServer_SearchActorNameResult))
	{
		ErrorLn(__FUNCTION__": sizeof(SMsgLoginServer_SearchActorNameResult) error=" << sizeof(SMsgLoginServer_SearchActorNameResult));
		return;
	}
	SMsgLoginServer_SearchActorNameResult* pInfo = (SMsgLoginServer_SearchActorNameResult*)data;

	game_player_request_name_result cmdInfo;
	cmdInfo.nSex = pInfo->nSex;
	sstrcpyn(cmdInfo.szName, pInfo->szName, sizeof(cmdInfo.szName));
	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_USER_RANDOM_PLAYER_NAME_RESULT, 0, "", &cmdInfo, sizeof(cmdInfo));
}

// 登陆服通知客户端信息
void Stage_Choose::onMsgLoginNotifyClient(void * data, size_t len)
{
	if (sizeof(SMsgLoginServer_Message) > len)
	{
		ErrorLn(__FUNCTION__ << "sizeof(SMsgLoginServer_Message) > len");
		return;
	}
	SMsgLoginServer_Message* pMsg = (SMsgLoginServer_Message*)data;
	if (len != sizeof(SMsgLoginServer_Message) + pMsg->wMsgLen)
	{
		ErrorLn(__FUNCTION__ << " need length=" << sizeof(SMsgLoginServer_Message) << ", message length= " << pMsg->wMsgLen);
		return;
	}

	char* pContent = (char*)(pMsg + 1);
	if (pContent == NULL)
	{
		return;
	}

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_LOGIN_CONNECTING, 0, 0, 0, 0);
    if (NoError == pMsg->dwErrorCode)
    {
        g_EHelper.showSystemMessage(CHAT_TIP_LOGIN_SERVER_MESSAGE, pContent);
    }
    else
    {
        cmd_open_servermsg_wnd cmdData;
        sstrcpyn(cmdData.szMsg,  pContent, sizeof(cmdData.szMsg));
        gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_LOGIN_OPEN_SERVERMSG_VIEW,0,0,&cmdData, sizeof(cmdData));
    }
}

// 请求进入游戏
void Stage_Choose::requestEnterGame(const char* szActorName)
{
	if (szActorName == NULL)
		return;

	obuf256 ob;
	SMsgLoginServer_SelectActor data;
	sstrcpyn(data.szActorName, szActorName, sizeof(data.szActorName));

	SNetMsgHead head;
	head.byDestEndPoint = MSG_ENDPOINT_LOGIN;
	head.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
	head.byKeyModule = MSG_MODULEID_LOGIN;
	head.byKeyAction = CS_MSG_USER_ENTER_GAME;

	ob << head << data;
    net_senddata( ob.data(),ob.size() );
}

void Stage_Choose::selectHero(int nParam)
{
	TraceLn("Choose::select hero_id=" << nParam);
	int HeroID = nParam;

	ISpellFactory *pSpellFactory = gClientGlobal->getSpellFactory();
	if (!pSpellFactory)
	{
		return;
	}
	// 取得技能槽信息
	SPELL_SLOT *pSlots = g_EHelper.getSpellSlot(HeroID);
	if (pSlots == NULL)
	{
		return;
	}

	IFreezeClient *pFreezeClient = gClientGlobal->getFreezeClient();

	cmd_choose_hero_sync cmd;
	cmd.HeroID = HeroID;		                    // 英雄ID
	cmd.nHp = pSlots->nHp;					    // 生命
	cmd.nPA = pSlots->nPA;					    // 物理攻击
	cmd.nMA = pSlots->nMA;					    // 魔法攻击
	cmd.nOperateFactor = pSlots->nOperateFactor;	// 操作系数难度

	sstrcpyn(cmd.szImage, pSlots->szImage, sizeof(cmd.szImage));
	sstrcpyn(cmd.szVocation, pSlots->szVocation, sizeof(cmd.szVocation));
	sstrcpyn(cmd.szLocate, pSlots->szLocate, sizeof(cmd.szLocate));

	// 组装信息到显示层
	for (int i = 0; i < pSlots->nCount; ++i)
	{
		int nSpellID = pSlots->nSpellID[i];
		SPELL_DATA * pSpellData = pSpellFactory->getSpellData(nSpellID);
		if (!pSpellData)
		{
			continue;
		}

		cmd.byIndex = i;	// 槽位
		cmd.nCD = pFreezeClient ? pFreezeClient->GetFreezeTime(EFreeze_ClassID_Spell, pSpellData->nCoolID) / 1000 : 0;

		memcpy(&(cmd.SpellData), pSpellData, sizeof(cmd.SpellData));

		string strName = a2utf8(pSpellData->szName);
		ssprintf_s(cmd.SpellData.szName, sizeof(cmd.SpellData.szName), "%s", strName.c_str());

		gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_USER_SELECT_HERO, 0, 0, &cmd, sizeof(cmd));
	}

}


// 返回登录状态
void Stage_Choose::onCmdReturnLogin(void)
{
	SNetMsgHead head;
	head.byDestEndPoint = MSG_ENDPOINT_LOGIN;
	head.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
	head.byKeyModule = MSG_MODULEID_LOGIN;
	head.byKeyAction = MSG_LOGINSERVER_SWITCH_STATE;

	SMsgLoginServer_SwitchState data;
	data.state = CLIENT_CONN_STATE_LOGIN;

	obuf ob;
	ob << head << data;
    net_senddata( ob.data(),ob.size() );
}

void Stage_Choose::onReqSearchActorName(void * data)
{
	// 请求随机名字
	game_player_request_name * pCmd = (game_player_request_name *)data;
	if (pCmd == NULL)
	{
		return;
	}

	SMsgLoginServer_SearchActorName msgData;
	msgData.nSex = pCmd->nSex;
	sstrcpyn(msgData.szOldName, pCmd->szOldName, sizeof(msgData.szOldName));
	msgData.nOldSex = pCmd->nOldSex;
	msgData.nFlag = pCmd->nFlag;

	SNetMsgHead head;
	head.byDestEndPoint = MSG_ENDPOINT_LOGIN;
	head.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
	head.byKeyModule = MSG_MODULEID_LOGIN;
	head.byKeyAction = MSG_LOGINSERVER_SEARCH_ACTORNAME;

	obuf ob;
	ob << head << msgData;
    net_senddata( ob.data(),ob.size() );
}

#endif
