#include "stdafx.h"
#include "ExternalHelper.h"
#include "ICenterConnectorService.h"
#include "MailMsgDef.h"
#include "IServerGlobal.h"
#include "IActorMgr.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include "ChatHelper.h"
#include "IChatSceneService.h"
#include "EntityHelper.h"
#include "Mail_ManagedDef.h"

using namespace Mail_ManagerDef;

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
void CExternalHelper::sendMailSocial(BYTE byKeyAction, LPCSTR pData, size_t nLen)
{
    // 社会服数据
    ICenterConnectorService *pCenterConnector = gServerGlobal->getCenterConnectorService();
    if (nullptr == pCenterConnector)
    {
        ErrorLn(__FUNCTION__" failed( void * buf, size_t len )! nullptr == pCenterConnector");
        return;
    }

    pCenterConnector->sendToSocial(MSG_MODULEID_MAIL, byKeyAction, pData, nLen);
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