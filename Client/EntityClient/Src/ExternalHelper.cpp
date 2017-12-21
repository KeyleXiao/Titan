/*******************************************************************
** 文件名:	ExternalHelper.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/12/2015
** 版  本:	1.0
** 描  述:	辅助功能类
********************************************************************/

#include "stdafx.h"
#include "ExternalHelper.h"
#include "IRenderView.h"
#include "IClientEntity.h"
#include "IClientGlobal.h"
#include "IChatClient.h"

CExternalHelper				g_EHelper;

/** 取得实体
*/
IEntity *CExternalHelper::GetEntity(UID uidEntity)
{
	IEntityFactory * pEntityWorld = gClientGlobal->getEntityFactory();
	if(pEntityWorld == NULL)
	{
		return NULL;
	}

	return pEntityWorld->getEntity(uidEntity);
}


/** 启用定时器
*/
bool CExternalHelper::SetTimer(DWORD dwTimerID, DWORD dwInterval, ITimerHandler *pHandler, DWORD dwCallTimes)
{
	TimerAxis *pTimerAxis = gClientGlobal->getTimerAxis();
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
	TimerAxis *pTimerAxis = gClientGlobal->getTimerAxis();
	if (pTimerAxis == NULL)
	{
		return false;
	}

	return pTimerAxis->KillTimer(dwTimerID, pHandler);
}


/** 显示提示文字
*/
void CExternalHelper::showFloatText(IEntity *pEntity, EMChatTipID nTextID)
{
    if (pEntity == NULL)
    {
        return;
    }

    /// 取得主角对象
    IClientEntity *pClientEntity = gClientGlobal->getHero();
    if (pClientEntity == NULL)
    {
        return;
    }

    // 是自己
    if (pClientEntity->getUID() != pEntity->getUID())
    {
        return;
    }
    IChatClient *pChatClient = gClientGlobal->getChatClient();
    if (pChatClient == NULL)
    {
        return;
    }

    pChatClient->showSystemMessage(nTextID);
}

/** 发消息给场景服
*/
void CExternalHelper::sendMessageToScene(BYTE byKeyModule, BYTE byKeyAction, void *pData, int nDataLen)
{
    SNetMsgHead head;
    head.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
    head.byDestEndPoint= MSG_ENDPOINT_SCENE;
    head.byKeyModule  = byKeyModule;
    head.byKeyAction  = byKeyAction;

    obuf ob;
    ob.push_back(&head, sizeof(head));
    ob.push_back(pData, nDataLen);

    net_senddata( ob.data(), ob.size() );
}

/** 发消息给社会服
*/
void CExternalHelper::sendMessageToSocial(BYTE byKeyModule, BYTE byKeyAction, void *pData, int nDataLen)
{
	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_CLIENT;
	head.byDestEndPoint= MSG_ENDPOINT_SOCIAL;
	head.byKeyModule  = byKeyModule;
	head.byKeyAction  = byKeyAction;

	obuf ob;
	ob.push_back(&head, sizeof(head));
	ob.push_back(pData, nDataLen);

    net_senddata( ob.data(), ob.size() );
}