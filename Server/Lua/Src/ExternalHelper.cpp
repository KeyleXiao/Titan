/*******************************************************************
** 文件名:	ExternalHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	11/25/2016
** 版  本:	1.0
** 描  述:	辅助功能类
********************************************************************/

#include "StdAfx.h"
#include "ExternalHelper.h"
#include "ITimerService.h"
#include "IServerGlobal.h"
#include "entityHelper.h"
#include "ICenterConnectorService.h"
#include "ChatHelper.h"

CExternalHelper g_EHelper;

CExternalHelper::CExternalHelper()
{

}


CExternalHelper::~CExternalHelper()
{

}


/** 启用定时器
*/
bool CExternalHelper::SetTimer(DWORD dwTimerID, DWORD dwInterval, ITimerHandler *pHandler, DWORD dwCallTimes, const char *debugInfo)
{
	ITimerService *pTimerAxis = gServerGlobal->getTimerService();
	if (pTimerAxis == NULL)
	{
		return false;
	}

	return pTimerAxis->SetTimer(dwTimerID, dwInterval, pHandler, dwCallTimes);
}


/** 销毁定时器
*/
bool CExternalHelper::KillTimer(DWORD dwTimerID, ITimerHandler *pHandler)
{
	ITimerService* pTimerAxis = gServerGlobal->getTimerService();
	if (pTimerAxis == NULL)
	{
		return false;
	}

	return pTimerAxis->KillTimer(dwTimerID, pHandler);
}

/** 发送数据
*/
void CExternalHelper::Send2Client(DWORD dwPDBID, BYTE byKeyModule, BYTE byKeyAction, void *pData, size_t nDataLen)
{
    ActorHelper actorHelper(dwPDBID, ACTOR_DBID);
    IActorService *pActor = actorHelper.m_ptr;
    if (pActor == NULL)
    {
        return;
    }

    SNetMsgHead head;
    head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
    head.byDestEndPoint= MSG_ENDPOINT_CLIENT;	
    head.byKeyModule   = byKeyModule;	
    head.byKeyAction   = byKeyAction;

    obuf data;
    data.push_back(&head, sizeof(head));
    data.push_back(pData, nDataLen);
    pActor->send2Client( PACKAGE_PTR(new std::string(data.data(), data.size())));
}

/** 发送社会服
*/
void CExternalHelper::Send2Social(BYTE byKeyModule, BYTE byKeyAction, void * pData, size_t stLen)
{
	ICenterConnectorService *pCenterConnector = gServerGlobal->getCenterConnectorService();
	if (nullptr == pCenterConnector)
	{
		ErrorLn(__FUNCTION__"failed! nullptr == pCenterConnector");
		return;
	}

	obuf obMsg;
	obMsg.push_back(pData, stLen);
	pCenterConnector->sendToSocial(byKeyModule, byKeyAction, obMsg.data(), obMsg.size());
}

/** 显示提示文字
*/
void CExternalHelper::showSystemMessage(DWORD dwDBID, int nTipID, LPCSTR szParam /* = NULL */)
{
	ChatHelper chatHelper;
	IChatSceneService *pChatSceneService = chatHelper.m_ptr;
	if (pChatSceneService == NULL)
	{
		return;
	}

	pChatSceneService->broadcastSystemMessage(CHAT_BROADCASTTYPE_SINGLE, dwDBID, (EMChatTipID)nTipID, szParam);
}