#include "stdafx.h"
#include "IClientGlobal.h"
#include "Stage_Manager.h"
#include "MigrateDef.h"
#include "Migrate.h"


///////////////////////////////////////////////////////////////////
void Migrate::create()
{
    // 消息处理注册
    IMessageDispatch * pMessageDispatch = gClientGlobal->getMessageDispatch();
    if(pMessageDispatch)
    {
        pMessageDispatch->registerMessageHandler(MSG_MODULEID_MIGRATE, this);
    }

	IEntityClient * pEntityClient = gClientGlobal->getEntityClient();
	if (pEntityClient)
	{
		pEntityClient->registerGameViewEventHandler(GVIEWCMD_MIGRATE_SWITCH_SCENE, this);
	}
}

void Migrate::release()
{
    IMessageDispatch * pMessageDispatch = gClientGlobal->getMessageDispatch();
    if(pMessageDispatch)
    {
        pMessageDispatch->unregisterMessageHandler(MSG_MODULEID_MIGRATE, this);
    }

	IEntityClient * pEntityClient = gClientGlobal->getEntityClient();
	if (pEntityClient)
	{
		pEntityClient->unregisterGameViewEventHandler(GVIEWCMD_MIGRATE_SWITCH_SCENE);
	}

    releaseInstance();
}

void Migrate::onMessage( SNetMsgHead* head,void* data,size_t len )
{
    switch(MAKEWORD(head->byKeyModule, head->byKeyAction))
    {
    case MAKEWORD(MSG_MODULEID_MIGRATE, MSG_SWITCH_SCENE_SC):	// 通知客户端切换地图
        {
            gClientGlobal->getStageManager()->gotoState(ESS_LOADSCENE, data, len);
        }
        break;
    case MAKEWORD(MSG_MODULEID_MIGRATE, MSG_NOTIFY_CLIENT_SWITCH_STATE):	// 让客户端切状态
        {
            if(len != sizeof(SMsgNotifySwitchState_SC))
            {
                ErrorLn(__FUNCTION__ << ", MSG_NOTIFY_CLIENT_SWITCH_STATE msg format error!");
                return;
            }

            SMsgNotifySwitchState_SC* pMsg = (SMsgNotifySwitchState_SC*)data;

            gClientGlobal->getStageManager()->gotoState((EStage_State)pMsg->nNextState);
        }
        break;
    case MAKEWORD(MSG_MODULEID_MIGRATE, MSG_NOTIFY_CLIENT_RELOGIN):		// 通知客户端重新登录
        {
            // 自动登录
            obuf256 buf;
            buf << true;
            gClientGlobal->getStageManager()->gotoState(ESS_LOGIN, buf.data(), buf.size());
        }
        break;
    }
}

bool Migrate::onViewEvent(int32 eventID, int nParam, const char * strParam, void * ptrParam)
{
	switch (eventID)
	{
		case GVIEWCMD_MIGRATE_SWITCH_SCENE:
		{
			onViewSwitchScene(nParam, strParam, ptrParam);
		}
		return true;
	default:
		ErrorLn(__FUNCTION__ << ",unhandled eventID=" << eventID);
		return false;
	};
}

void Migrate::onViewSwitchScene(int nParam, const char * strParam, void * ptrParam)
{
	if (strParam == NULL)
		return;

	gamelogic_migrate_switch_scene* pCmd = (gamelogic_migrate_switch_scene*)ptrParam;

	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
	head.byDestEndPoint = MSG_ENDPOINT_SCENE;
	head.byKeyModule = MSG_MODULEID_MIGRATE;
	head.byKeyAction = MSG_REQ_SWITCH_SCENE_CS;

	SMsgReqSwitchScene_CS msg;
	msg.dwGameWorldID = pCmd->nGameWorldID;
	msg.dwServerID = pCmd->nServerID;
	msg.dwSceneID = pCmd->nSceneID;

	obuf256 ob;
	ob << head << msg;
    net_senddata( ob.data(), ob.size() );
}
