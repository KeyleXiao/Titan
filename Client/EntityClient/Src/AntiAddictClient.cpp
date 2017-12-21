

#include "stdafx.h"
#include "AntiAddictClient.h"
#include "GameViewDef.h"
#include "AntiAddictDef.h"

CAntiAddictClient *g_pAntiAddictClient = NULL;

CAntiAddictClient::CAntiAddictClient(void)
{

}

CAntiAddictClient::~CAntiAddictClient(void)
{

}

bool CAntiAddictClient::Create()
{
	// 注册消息分发器
	gClientGlobal->getMessageDispatch()->registerMessageHandler(MSG_MODULEID_ANTIADDICT, (IMessageHandler *)this);

	// 注册显示层需要处理命令
	gClientGlobal->getEntityClient()->registerGameViewEventHandler( GVIEWCMD_ANTIADDICT_REGIST,this );

	return true;
}

bool CAntiAddictClient::Release()
{
	// 反注册消息分发器
	gClientGlobal->getMessageDispatch()->unregisterMessageHandler(MSG_MODULEID_ANTIADDICT, this);

	// 反注册显示层需要处理命令
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler( GVIEWCMD_ANTIADDICT_REGIST);

	g_pAntiAddictClient = NULL;
	delete this;
	return true;
}

void CAntiAddictClient::onMessage( SNetMsgHead *head, void *data, size_t len )
{
	if (head->byKeyModule != MSG_MODULEID_ANTIADDICT)
		return;

	switch (head->byKeyAction)
	{
	case MSG_ADDICT_GAME_Z2C:
		{
			OnMsgAuthResult(data, len);
		}
		break;
	case MSG_ADDICT_TIP_S2C:
		{
			OnMsgAddictTip(data, len);
		}
		break;
	default:
		break;
	}
}


bool CAntiAddictClient::onViewEvent( int eventID,int nParam,const char * strParam,void * ptrParam )
{
	switch ( eventID )
	{
		// 注册防沉迷
	case GVIEWCMD_ANTIADDICT_REGIST:
		{
			AntiAddictAuth(ptrParam);
		}
		break;
	default:
		break;
	}
	return true;
}

void CAntiAddictClient::AntiAddictAuth(void * ptrParam)
{
	if (ptrParam == NULL)
	{
		return;
	}

	gameview_antiaddict_regist * pData = (gameview_antiaddict_regist *)ptrParam;

	SNetMsgHead gamemsghead;
	gamemsghead.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
	gamemsghead.byDestEndPoint= MSG_ENDPOINT_SCENE;
	gamemsghead.byKeyModule  = MSG_MODULEID_ANTIADDICT;
	gamemsghead.byKeyAction  = MSG_ADDICT_AUTH;

	SMsgAddictAuth_CS msgRegist;
	sstrcpyn(msgRegist.entityname, pData->entityName, sizeof(msgRegist.entityname));
	sstrcpyn(msgRegist.card, pData->card, sizeof(msgRegist.card));

	obuf ob;
	ob.push_back(&gamemsghead, sizeof(gamemsghead));
	ob.push_back(&msgRegist, sizeof(msgRegist));

    net_senddata( ob.data(), ob.size() );
}

void CAntiAddictClient::OnMsgAuthResult(void *data, size_t len )
{
	if ( data == NULL)
	{
		return;
	}

	if (len < sizeof(SMsgAddictGame_Z2C))
	{
		return;
	}

	SMsgAddictGame_Z2C *pMsg = (SMsgAddictGame_Z2C *)data;
	gamelogic_antiaddict_regist_result cmdData;
	cmdData.ret = pMsg->ret;
	sstrcpyn(cmdData.szRetDes, pMsg->szRetDes, sizeof(cmdData.szRetDes));

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_ANTIADDICT_REGIST_RESULT, 0, 0, &cmdData, sizeof(cmdData));
}

void CAntiAddictClient::OnMsgAddictTip(void *data, size_t len )
{
	if ( data == NULL)
	{
		return;
	}

	if (len < sizeof(SMsgAddictTip_S2C))
	{
		return;
	}

	SMsgAddictTip_S2C *pMsg = (SMsgAddictTip_S2C *)data;
	gamelogic_antiaddict_tip cmdData;
	sstrcpyn(cmdData.szRetDes, a2utf8(pMsg->szRetDes), sizeof(cmdData.szRetDes));

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_ANTIADDICT_TIP, 0, 0, &cmdData, sizeof(cmdData));
}

API_EXPORT IAntiAddictClient *CreateAntiAddictClient(void)
{
	// 如果创建了，直接返回
	if (g_pAntiAddictClient != NULL)
	{
		return g_pAntiAddictClient;
	}

	// 实例化防沉迷客户端
	CAntiAddictClient *pAntiAddictClient = new CAntiAddictClient();
	if (pAntiAddictClient == NULL || !pAntiAddictClient->Create())
	{
		SAFE_RELEASE(pAntiAddictClient);
		return NULL;
	}

	g_pAntiAddictClient = pAntiAddictClient;
	return g_pAntiAddictClient;
}