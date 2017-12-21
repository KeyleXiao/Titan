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

// 构造函数
MailSceneService::MailSceneService()
{

}

// 析构函数
MailSceneService::~MailSceneService()
{
	
}

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

 //   IDBEngineService *pDBEngine = gServerGlobal->getDBEngineService();
 //   if (pDBEngine != NULL)
 //   {
 //       pDBEngine->unregisterHandler(this);
 //   }

	//IEventEngine * pEventEngine = gServerGlobal->getEventEngine();
	//if (pEventEngine)
	//{
	//	pEventEngine->UnSubscibe(this, EVENT_GAME_ACTOR_TOPUP_SUCCESS, SOURCE_TYPE_PERSON, 0);
	//}
}

void MailSceneService::release()
{
	TraceLn(__FUNCTION__);
    delete this;
}

bool MailSceneService::create()
{
    TraceLn(__FUNCTION__);

	//gServerGlobal->getDBEngineService()->registerHandler(this, __FUNCTION__);

	IMessageDispatch *pMessageDispatch = gServerGlobal->getMessageDispatch();
	if (pMessageDispatch == NULL)
		return false;
	// 自己需要处理消息
	pMessageDispatch->registerTransmitHandler(MSG_MODULEID_MAIL, this);
	// 处理客户端发送过来的消息
	pMessageDispatch->registerMessageHandler(MSG_MODULEID_MAIL, this);

	//IEventEngine * pEventEngine = gServerGlobal->getEventEngine();
	//if (pEventEngine)
	//{
	//	pEventEngine->Subscibe(this, EVENT_GAME_ACTOR_TOPUP_SUCCESS, SOURCE_TYPE_PERSON, 0, __FUNCTION__);
	//}

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

void MailSceneService::handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg)
{
    void *data = (void *)msg->c_str();
    size_t nLen = msg->size();

    switch (head.byKeyAction)
    {
    default:
		{
			ErrorLn(__FUNCTION__": unknown pHead->byKeyAction=" << head.byKeyAction);
		}
    }
}
//
//void MailSceneService::handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg)
//{
//	void *data = (void *)msg->c_str();
//	size_t stLen = msg->size();
//
//	switch (head.byKeyAction)
//	{
//	case MSG_MAIL_REQUESTMAILLIST:
//		{
//			onMsgRequestMailList(client, data, stLen);
//		}
//		break;
//	case MSG_MAIL_REQUESTREADMAIL:
//		{
//			onMsgRequestReadMail(client, data, stLen);
//		}
//		break;
//	case MSG_MAIL_REQUESTFASTRECV:
//		{
//			onMsgRequestFastRecv(client, data, stLen);
//		}
//		break;
//	default:
//		{
//			ErrorLn(__FUNCTION__": unknown pHead->byKeyAction=" << head.byKeyAction);
//		}
//	}
//}

// 删除邮件
//bool MailSceneService::delMail(DWORD dwPDBID, LONGLONG llMailID)
//{
//	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
//	if (pDBEngine == NULL)
//	{
//		ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
//		return false;
//	}
//
//	DBREQ_PARAM_DELMAIL refer;
//	refer.dwPDBID = dwPDBID;
//	refer.llMailID = llMailID;
//	if (pDBEngine->executeSP(GAMEDB_REQUEST_DEL_MAIL, dwPDBID, (LPCSTR)&refer, sizeof(DBREQ_PARAM_DELMAIL), static_cast<IDBRetSink *>(this)) == false)
//	{
//		ErrorLn(__FUNCTION__": GAMEDB_REQUEST_DEL_MAIL to DB failed");
//		return false;
//	}
//
//	obuf ob;
//	SMsgMail_DelMail_SO sendData;
//	sendData.dwPDBID = dwPDBID;
//	sendData.llMailID = llMailID;
//
//	ob.push_back(&sendData, sizeof(SMsgMail_DelMail_SO));
//	g_EHelper.sendMailSocial(MSG_MAIL_DELMAIL_SO, ob.data(), ob.size());
//
//	return true;
//}

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
    MailHelper MailSceneHelper;
    IMailSceneService *pMailSceneService = MailSceneHelper.m_ptr;
    if (pMailSceneService == NULL)
    {
        ErrorLn(__FUNCTION__": server ="<<server<<" uMsgID ="<<uMsgID);
        return;
    }

    PACKAGE_PTR pkg( new string((const char*)data,len));
    pMailSceneService->handleServerMsg( server, *head, pkg );

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


/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
//void MailSceneService::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
//{
//	if (bSrcType == SOURCE_TYPE_PERSON)
//	{
//		switch (wEventID)
//		{
//		case EVENT_GAME_ACTOR_TOPUP_SUCCESS:
//			{
//				if (pszContext == NULL || nLen != sizeof(event_player_topup_success))
//					return;
//
//				event_player_topup_success* pData = (event_player_topup_success*)pszContext;
//
//				SMsgMail_RefreshMailList msgData;
//				msgData.dwPDBID = pData->dwPDBID;
//
//				g_EHelper.sendMailSocial(MSG_MAIL_REFRESHMAILLIST, (const char*)&msgData, sizeof(msgData));
//			}
//			break;
//		default:
//			break;
//		}
//	}
//}

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

//void MailSceneService::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
//{
//	switch(nCmdID)
//	{
//	case GAMEDB_REQUEST_READ_MAIL:
//		{
//			onReturnReadMailData(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
//		}
//		break;
//	case GAMEDB_REQUEST_DEL_MAIL:
//		{
//			onReturnDelMail(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
//		}
//		break;
//	default:
//		break;
//	}
//}
//
//// purpose: 读取邮件详细数据				-- 来自数据库
//void MailSceneService::onReturnReadMailData(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
//{
//	if(nDBRetCode < DBRET_CODE_SUCCEED)
//	{
//		ErrorLn(__FUNCTION__": data of db is invalid! dwHolder = "<<nQueueIndex<<", retCode="<<nDBRetCode << ", desc="<<pszDBRetDesc);
//		return;
//	}
//
//	if (nOutLen != sizeof(DBREQ_RESULT_READMAIL))
//	{
//		int tempLen = sizeof(DBREQ_RESULT_READMAIL);
//		ErrorLn("[onReturnReadMailData] error length outlength = " << nOutLen << " sizeof(DBREQ_RESULT_READMAIL) = " << tempLen);
//		return;
//	}
//
//	DBREQ_RESULT_READMAIL *pRetData = (DBREQ_RESULT_READMAIL*)pOutData;
//
//	ActorHelper helper(pRetData->dwPDBID, ACTOR_DBID);
//	IActorService * pActorService = helper.m_ptr;
//	if(pActorService == NULL)
//		return;
//
//	SMailDetailData detailData;
//	detailData.llMailID = pRetData->llMailID;
//	sstrcpyn(detailData.szContext, pRetData->szContext, sizeof(detailData.szContext));
//	detailData.nPlusExp = pRetData->nPlusExp;
//	detailData.nPlusMoney = pRetData->nPlusMoney;
//	detailData.nPlusGoodsNum = pRetData->nPlusGoodsNum;
//
//	for (int i = 0; i < pRetData->nPlusGoodsNum; ++i)
//	{
//		detailData.sGoodsList[i].dwGoodsID = pRetData->sGoodsList[i].dwGoodsID;
//		detailData.sGoodsList[i].nCount = pRetData->sGoodsList[i].nCount;
//	}
//
//	g_EHelper.send2Client(pRetData->dwPDBID, MSG_MODULEID_MAIL, MSG_MAIL_RESPONSEREADMAIL, (LPCSTR)&detailData, sizeof(SMailDetailData));
//
//	if (pRetData->nType == emMailType_Notify)
//	{
//		delMail(pRetData->dwPDBID, pRetData->llMailID);
//		pActorService->setLastRecvPlusTime(0);
//	}
//	else
//	{
//		if (pRetData->nPlusExp > 0 || pRetData->nPlusMoney > 0 || pRetData->nPlusGoodsNum > 0)
//		{
//			pActorService->addPlayerMailDetailData(detailData, false);
//			setMailReadToSocial(pRetData->dwPDBID, pRetData->llMailID);
//		}
//		else
//		{
//			delMail(pRetData->dwPDBID, pRetData->llMailID);
//			pActorService->setLastRecvPlusTime(0);
//		}
//	}	
//}
//
//// purpose: 删除邮件						-- 来自数据库
//void MailSceneService::onReturnDelMail(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
//{
//	// 验证数据库返回结果
//	if(nDBRetCode < DBRET_CODE_SUCCEED)
//	{
//		ErrorLn("[onReturnDelMail] Del Mail error！MailID = " << nQueueIndex << " pszDBRetDesc = " << pszDBRetDesc);
//		return;
//	}
//}

//// 请求玩家邮件列表
//void MailSceneService::onMsgRequestMailList(ClientID clientID, void* pData, size_t stLen)
//{
//	// 判断参数
//	if (clientID == INVALID_CLIENT || pData == NULL || stLen != sizeof(SMsgMail_MailList))
//	{
//		return;
//	}
//
//	ActorHelper helper(clientID, ACTOR_CLIENT);
//	IActorService * pActorService = helper.m_ptr;
//	if(pActorService == NULL)
//	{
//		ErrorLn(__FUNCTION__": pActorService == NULL clientID= "<<clientID);
//		return;
//	}
//
//	SMsgMail_MailList *pRecvData = (SMsgMail_MailList*)pData;
//
//	g_EHelper.sendMailSocial(MSG_MAIL_REQUESTMAILLIST, (const char*)pData, stLen);
//}

//// 请求阅读邮件
//void MailSceneService::onMsgRequestReadMail(ClientID clientID, void* pData, size_t stLen)
//{
//	if (isPublicGameWorld() == true)
//		return;
//
//	// 判断参数
//	if (clientID == INVALID_CLIENT || pData == NULL || stLen != sizeof(SMsgMail_ReadMail))
//	{
//		return;
//	}
//
//	ActorHelper helper(clientID, ACTOR_CLIENT);
//	IActorService * pActorService = helper.m_ptr;
//	if(pActorService == NULL)
//	{
//		ErrorLn(__FUNCTION__": pActorService == NULL clientID= "<<clientID);
//		return;
//	}
//
//	PDBID idActor = pActorService->getProperty_Integer(PROPERTY_ID);
//
//	SMsgMail_ReadMail *pRecvData = (SMsgMail_ReadMail*)pData;
//
//	if ( pActorService->isAreadyRecvMail(pRecvData->llMailID) )
//		return;
//
//	if((DWORD)time(NULL) < pActorService->getLastRecvPlusTime() + MAIL_RECV_PLUS_CD_LEN)
//	{
//		g_EHelper.showSystemMessage(idActor, CHAT_TIP_MAIL_RECV_PLUS_ING);
//		return;
//	}
//
//	if (!pActorService->dealPlayerMailDetailData(pRecvData->llMailID, false))
//	{
//		// 读取邮件详细数据
//		IDBEngineService*  pDBEngine = gServerGlobal->getDBEngineService();
//		if(pDBEngine == NULL)
//		{
//			ErrorLn(__FUNCTION__" pDBEngine == NULL");
//			return;
//		}
//
//		DBREQ_PARAM_READMAIL refer;
//		refer.dwPDBID = idActor;
//		refer.llMailID = pRecvData->llMailID;
//		if (pDBEngine->executeSP(GAMEDB_REQUEST_READ_MAIL, idActor, (LPCSTR)&refer, sizeof(DBREQ_PARAM_READMAIL), static_cast<IDBRetSink *>(this)) == false)
//		{
//			ErrorLn(__FUNCTION__": GAMEDB_REQUEST_READ_MAIL to DB failed");
//			return;
//		}
//
//		pActorService->setLastRecvPlusTime((DWORD)time(NULL));
//	}
//}

// 请求快速领取
//void MailSceneService::onMsgRequestFastRecv(ClientID clientID, void* pData, size_t stLen)
//{
//	if (isPublicGameWorld() == true)
//		return;
//
//	// 判断参数
//	if (clientID == INVALID_CLIENT || pData == NULL || stLen != sizeof(SMsgMail_FastRecv))
//	{
//		return;
//	}
//
//	ActorHelper helper(clientID, ACTOR_CLIENT);
//	IActorService * pActorService = helper.m_ptr;
//	if(pActorService == NULL)
//	{
//		ErrorLn(__FUNCTION__": pActorService == NULL clientID= "<<clientID);
//		return;
//	}
//
//	PDBID idActor = pActorService->getProperty_Integer(PROPERTY_ID);
//
//	SMsgMail_FastRecv *pRecvData = (SMsgMail_FastRecv*)pData;
//
//	if((DWORD)time(NULL) < pActorService->getLastRecvPlusTime() + MAIL_RECV_PLUS_CD_LEN)
//	{
//		g_EHelper.showSystemMessage(idActor, CHAT_TIP_MAIL_RECV_PLUS_ING);
//		return;
//	}
//
//	if ( pActorService->isAreadyRecvMail(pRecvData->llMailID) )
//	{
//		return;
//	}
//
//	pActorService->dealPlayerMailDetailData(pRecvData->llMailID, true);
//}

//// 更新邮件
//void MailSceneService::setMailReadToSocial(DWORD dwPDBID, LONGLONG llMailID)
//{
//	obuf ob;
//	SMsgMail_SetMailRead_SO sendData;
//	sendData.dwPDBID = dwPDBID;
//	sendData.llMailID = llMailID;
//
//	ob.push_back(&sendData, sizeof(SMsgMail_SetMailRead_SO));
//	g_EHelper.sendMailSocial(MSG_MAIL_SETMAILREAD_SO, ob.data(), ob.size());
//}
//



