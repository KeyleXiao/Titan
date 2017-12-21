﻿#include "stdafx.h"
#include "ExternalHelper.h"
#include "ICenterConnectorService.h"
#include "ClanMessageDef.h"
#include "IServerGlobal.h"
#include "IActorMgr.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include "ChatHelper.h"
#include "IChatSceneService.h"
#include "EntityHelper.h"
#include "TaskStorageDef.h"

using namespace rkt;

CExternalHelper	g_EHelper;

// 构造函数
CExternalHelper::CExternalHelper(void)
{

}

// 析构函数
CExternalHelper::~CExternalHelper(void)
{

}

// 发送消息
void CExternalHelper::sendClanSocial(BYTE byKeyAction, int nActionID, DWORD dwClanID,void *pData, int nLen)
{
    // 社会服数据
    ICenterConnectorService *pCenterConnector = gServerGlobal->getCenterConnectorService();
    if (nullptr == pCenterConnector)
    {
        ErrorLn(__FUNCTION__" failed( void * buf, size_t len )! nullptr == pCenterConnector");
        return;
    }

    obuf256 obMsg;

    msg_clan_sub_head subMsg;
    subMsg.dwMsgCode = nActionID;
    subMsg.dwClanID = dwClanID;

    obMsg.push_back(&subMsg, sizeof(msg_clan_sub_head));
    obMsg.push_back(pData, nLen);

    pCenterConnector->sendToSocial(MSG_MODULEID_CLAN, byKeyAction, obMsg.data(), obMsg.size());
}

// 发送数据
void CExternalHelper::send2Client(DWORD dwPDBID, BYTE byKeyModule, BYTE byKeyAction, LPCSTR pszData, size_t nDataLen)
{
	ActorHelper actorHelper(dwPDBID, ACTOR_DBID);
	if (actorHelper.m_ptr == NULL)
		return;

    // 通知客户端
    obuf256 obufData;

    SNetMsgHead head;
    head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
    head.byDestEndPoint= MSG_ENDPOINT_CLIENT;	
    head.byKeyModule   = byKeyModule;	
    head.byKeyAction   = byKeyAction;

    obufData << head ;
    obufData.push_back(pszData,nDataLen);

    actorHelper.m_ptr->send2Client(PACKAGE_PTR(new string((const char *)obufData.data(),obufData.size())));
}

// 取得帮会每级信息
SClanLevel* CExternalHelper::getLevelInfo(int nClanLevel)
{
    ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
        return NULL;

    ISchemeClanDataCenter *pSchemeClanDataCenter = pSchemeCenter->getSchemeClanDataCenter();
    if (pSchemeClanDataCenter == NULL)
        return NULL;

    return pSchemeClanDataCenter->getLevelInfo(nClanLevel);
}

// 取得帮会常量信息
SClanConfig* CExternalHelper::getConfig()
{
    ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
        return NULL;

    ISchemeClanDataCenter *pSchemeClanDataCenter = pSchemeCenter->getSchemeClanDataCenter();
    if (pSchemeClanDataCenter == NULL)
        return NULL;

    return pSchemeClanDataCenter->getConfig();
}

// 帮会卡牌信息
SClanCardSchemeInfo* CExternalHelper::getClanCardInfo(int nLevel)
{
	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return NULL;

	ISchemeClanDataCenter *pSchemeClanDataCenter = pSchemeCenter->getSchemeClanDataCenter();
	if (pSchemeClanDataCenter == NULL)
		return NULL;

	return pSchemeClanDataCenter->getClanCardInfo(nLevel);
}

// 获取角色任务数据辅助类
int CExternalHelper::getActorTaskData(IActorService *pActor, int nKey, int nDefault)
{
	int nResult = pActor->getTaskDataInt(nKey);
	if (nResult == TASKDATA_DEFAULT_VALUE)
		nResult = nDefault;
	return nResult;
}

// 显示提示文字
void CExternalHelper::showSystemMessage(DWORD dwPDBID, EMChatTipID nTipID, LPCSTR szParam)
{
    ChatHelper chatHelper;
    IChatSceneService *pChatSceneService = chatHelper.m_ptr;
    if (pChatSceneService == NULL)
    {
        return;
    }

    pChatSceneService->broadcastSystemMessage(CHAT_BROADCASTTYPE_SINGLE, dwPDBID, nTipID, szParam);
}