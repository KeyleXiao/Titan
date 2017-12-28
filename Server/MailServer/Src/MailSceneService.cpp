#include "stdafx.h"
#include "MailSceneService.h"
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
#include "MailHelper.h"
#include "Mail_ManagedDef.h"

using namespace Mail_ManagerDef;

//// 构造函数
//MailSceneService::MailSceneService()
//{
//
//}
//
//// 析构函数
//MailSceneService::~MailSceneService()
//{
//	
//}

void MailSceneService::onStop()
{
    IMessageDispatch *pMessageDispatch = gServerGlobal->getMessageDispatch();
    if (pMessageDispatch != NULL)
    {
        // 自己需要处理消息
        pMessageDispatch->unregisterTransmitHandler(MSG_MODULEID_MAIL);
        // 取消注册响应客户端消息
        pMessageDispatch->unregisterMessageHandler(MSG_MODULEID_MAIL);
    }
}

void MailSceneService::release()
{
	//TraceLn(__FUNCTION__);
    delete this;
}

bool MailSceneService::create()
{
    //TraceLn(__FUNCTION__);

	IMessageDispatch *pMessageDispatch = gServerGlobal->getMessageDispatch();
	if (pMessageDispatch == NULL)
		return false;
	// 自己需要处理消息
	pMessageDispatch->registerTransmitHandler(MSG_MODULEID_MAIL, this);
	// 处理客户端发送过来的消息
	pMessageDispatch->registerMessageHandler(MSG_MODULEID_MAIL, this);

    return true;
}

// 通知服务器列表已经更新
void MailSceneService::onServerListUpdated()
{

}

// 通知服务器信息更新
void MailSceneService::onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData)
{

}

//void MailSceneService::handleServerMsg(DWORD serverID, SNetMsgHead head, void * data, size_t len)
//{
//    switch (head.byKeyAction)
//    {
//    default:
//		{
//			ErrorLn(__FUNCTION__": unknown pHead->byKeyAction=" << head.byKeyAction);
//		}
//    }
//}
//


// 发送邮件
bool MailSceneService::sendMail(SMailSendData sMailSendData, int nMailFillID, LPCSTR szParam)
{
	return sendMailToSocial(&sMailSendData, nMailFillID, szParam);
}

// 发送填充邮件(数据由配置表里面填充)
bool MailSceneService::sendFillMail(DWORD dwPDBID, int nMailFillID, LPCSTR szParam)
{
	if (nMailFillID <= emMailFill_Begin)
		return false;

	SMailSendData sMailSendData;
	sMailSendData.dwMasterPDBID = dwPDBID;
	sMailSendData.nType = emMailType_Notify;
	sMailSendData.nSourceType = emMailSourceType_System;

	return sendMailToSocial(&sMailSendData, nMailFillID, szParam);
}

////////////////////////////////////////ITransmitHandler//////////////////////////////////////////
// 消息处理
void MailSceneService::onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void *data, size_t len)
{
    //MailHelper MailSceneHelper;
    //IMailSceneService *pMailSceneService = MailSceneHelper.m_ptr;
    //if (pMailSceneService == NULL)
    //{
    //    ErrorLn(__FUNCTION__": server ="<<server<<" uMsgID ="<<uMsgID);
    //    return;
    //}

    //PACKAGE_PTR pkg( new string((const char*)data,len));
    //pMailSceneService->handleServerMsg( server, *head, pkg );

}


///////////////////////////////////////IMessageHandler/////////////////////////////////////////
// 消息处理
void MailSceneService::onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead *head, void *data, size_t len)
{
    //MailHelper MailSceneHelper;
    //IMailSceneService *pMailSceneService = MailSceneHelper.m_ptr;
    //if (pMailSceneService == NULL)
    //{
    //    ErrorLn(__FUNCTION__": clientID ="<<clientID<<" uMsgID ="<<uMsgID);
    //    return;
    //}

    //PACKAGE_PTR pkg( new string((const char*)data,len));
    //pMailSceneService->handleClientMsg( clientID, *head, pkg );
}


/// purpose: 发送邮件
bool MailSceneService::sendMailToSocial(SMailSendData *pSendData, int nMailFillID, LPCSTR szParam)
{
	if (isPublicGameWorld() == true || pSendData == NULL)
		return false;

	ICenterConnectorService *pCenterConnector = gServerGlobal->getCenterConnectorService();
	if (nullptr == pCenterConnector)
	{
		ErrorLn(__FUNCTION__" failed! nullptr == pCenterConnector");
		return false;
	}

	if(pSendData->nPlusGoodsNum > MAIL_GOODS_MAX_NUM)
		return false;

	if(pSendData->nType == emMailType_Player)
	{
		// 处理发送玩家数据
		ActorHelper helper(pSendData->dwSenderPDBID, ACTOR_DBID);
		IActorService * pActorService = helper.m_ptr;
		if(pActorService == NULL)
		{
			ErrorLn(__FUNCTION__": sender not find pActorService == NULL");
			return false;
		}

		if (!pActorService->dealSendMailPlusData(*pSendData))
		{
			ErrorLn(__FUNCTION__": sender dealSendMailPlusData failed !");
			return false;
		}
	}

	if (pSendData->nType != emMailType_System)
	{
		pSendData->nPlusMoney = 0;
		pSendData->nPlusExp = 0;
	}

	SMsgMail_AddMail_SO addMailData;
	addMailData.nMailFillID = nMailFillID;
	memcpy(&(addMailData.sMailData), pSendData, sizeof(SMailSendData));
	string strParam = (szParam == NULL ? "" : szParam);
	sstrcpyn(addMailData.szParam, strParam.c_str(), sizeof(addMailData.szParam));	

	pCenterConnector->sendToSocial(MSG_MODULEID_MAIL, MSG_MAIL_ADDMAIL_SO, (LPCSTR)(&addMailData), sizeof(SMsgMail_AddMail_SO));

	return true;
}