#include "stdafx.h"
#include "MailService.h"
//#include "ICenterConnectorService.h"
#include "ISocialGlobal.h"
#include "Event_ManagedDef.h"
#include "IShareServer.h"
#include "ExternalHelper.h"
#include "ISchemeCenter.h"
#include "IMessageDispatch.h"
#include "ISocialGlobal.h"
#include "MailHelper.h"
#include "OrderEvent.h"
#include "EventDef.h"
#include "Chat_ManagedDef.h"
#include "Simple_Atom_Lock.h"
#include "Mail_ManagedDef.h"
#include "OssLogDef.h"
#include "IOSSLogService.h"
#include "ISNSService.h"
#include "SNSHelper.h"
#include "ChatDef.h"

using namespace Mail_ManagerDef;

struct IDBEngineService;

MailService::MailService()
{
	m_MailIndex = 0;
	m_nQueueIndex = 0;
	m_DelMailList.clear();
	m_SendNodeList.clear();
	m_PlayerMailList.clear();
}

MailService::~MailService()
{

}

void MailService::release()
{
	TraceLn(__FUNCTION__);
 
    // 数据库接口反注册
    IDBEngineService* pDBEngineService = gSocialGlobal->getDBEngineService();
    if (pDBEngineService != NULL)
    {
        pDBEngineService->unregisterHandler(this);
    }

    // 共享信息注册
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver != NULL)
    {
        pShareReceiver->UnregisterHandler(this);
    }

    IMessageDispatch* pMessageDispatch = gSocialGlobal->getMessageDispatch();
    if(pMessageDispatch)
    {
        // 服务器消息分发反注册
        pMessageDispatch->unregisterTransmitHandler(MSG_MODULEID_MAIL);
        // 客户端消息分发反注册
        pMessageDispatch->unregisterMessageHandler(MSG_MODULEID_MAIL);
    }

	delete this;
}

bool MailService::create()
{
	TraceLn(__FUNCTION__);

    // 数据库接口注册
    IDBEngineService* pDBEngineService = gSocialGlobal->getDBEngineService();
    if (pDBEngineService != NULL)
    {
        pDBEngineService->registerHandler(this);
    }

    // 共享信息注册
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver != NULL)
    {
        pShareReceiver->RegisterHandler(this, __FUNCTION__);
    }

    IMessageDispatch* pMessageDispatch = gSocialGlobal->getMessageDispatch();
    if(pMessageDispatch)
    {
        // 服务器消息分发注册
        pMessageDispatch->registerTransmitHandler(MSG_MODULEID_MAIL,this);
        // 客户端消息分发注册
        pMessageDispatch->registerMessageHandler(MSG_MODULEID_MAIL, this);
    }

	return true;
}

void MailService::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	switch (nCmdID)
	{ 
		// 读取邮件列表                 -- 数据库返回处理
	case GAMEDB_REQUEST_READ_MAIL_LIST:
		{
			onReturnReadMailList(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
			break;
		}
		// 删除邮件
	case GAMEDB_REQUEST_DEL_MAIL:
		{
			onReturnDelMail(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
			break;
		}
		// 添加邮件
	case GAMEDB_REQUEST_ADD_MAIL:
		{
			onReturnAddMail(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
			break;
		}
	default:
		break;
	}
}

/// purpose: 读取邮件列表         					-- 来自数据库
void MailService::onReturnReadMailList(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
	// 验证数据库返回结果
	if(nDBRetCode < DBRET_CODE_SUCCEED)
	{
		ErrorLn("[onReturnReadMailList] read MailList error！ pszDBRetDesc = " << a2utf8(pszDBRetDesc));
		return;
	}
	// 检查数据返回长度
	if (nOutLen < sizeof(DBREQ_RESULT_READMAILLIST))
	{
		int tempLen = sizeof(DBREQ_RESULT_READMAILLIST);
		ErrorLn("[onReturnReadMailList] error length outlength = " << nOutLen << " sizeof(DBREQ_RESULT_READMAILLIST) = " << tempLen);
		return;
	}
	
	DBREQ_RESULT_READMAILLIST* pRetData = (DBREQ_RESULT_READMAILLIST*)pOutData;
	EmphasisLn("return Mail count =" << pRetData->nCurCount);
	if (nOutLen != sizeof(DBREQ_RESULT_READMAILLIST) + pRetData->nCurCount * sizeof(DBREQ_TABLE_MAIL_BASEDATA))
	{
		int tempLen = sizeof(DBREQ_RESULT_READMAILLIST) + pRetData->nCurCount * sizeof(DBREQ_TABLE_MAIL_BASEDATA);
		ErrorLn("[onReturnReadMailList] error lengthlist outlength = " << nOutLen << " sizeof(DBREQ_RESULT_READMAILLIST) = " << tempLen);
		return;
	}

	CPlayerMail* pPlayerMail  = findPlayerMail(pRetData->dwPDBID);
	if (pPlayerMail == NULL)
	{
		return;
	}

	int offset = sizeof(DBREQ_RESULT_READMAILLIST);
	for (int i=0; i < pRetData->nCurCount; ++i)
	{
		DBREQ_TABLE_MAIL_BASEDATA* pMailData = (DBREQ_TABLE_MAIL_BASEDATA*)(pOutData + offset);

		if ((DWORD)time(NULL) >= pMailData->dwRecvTime + pMailData->dwLifeTime * 3600)
		{
			SMailDelInfo delInfo;
			delInfo.llMailID = pMailData->llMailID;
			delInfo.dwPDBID = pRetData->dwPDBID;
			m_DelMailList.push_back(delInfo);
			offset += sizeof(DBREQ_TABLE_MAIL_BASEDATA);
			continue;
		}

		SMailBaseData baseData;
		baseData.llMailID = pMailData->llMailID;
		baseData.nType = pMailData->nType;
		baseData.nSourceType = pMailData->nSourceType;
		sstrcpyn(baseData.szTitle, pMailData->szTitle, sizeof(baseData.szTitle));
		sstrcpyn(baseData.szSenderName, pMailData->szSenderName, sizeof(baseData.szSenderName));
		baseData.dwSenderPDBID = pMailData->dwSenderPDBID;
		baseData.bRead = false;
		baseData.bIsPlusData = pMailData->bIsPlusData;
		baseData.dwRecvTime = pMailData->dwRecvTime;
		baseData.dwLifeTime = pMailData->dwLifeTime;

		pPlayerMail->addMailBaseData(&baseData);
		offset += sizeof(DBREQ_TABLE_MAIL_BASEDATA);
	}

	pPlayerMail->notifyPlayerNewMailCount();

	delOverTimeMailFromDB();
}

/// purpose: 删除邮件     								-- 来自数据库
void MailService::onReturnDelMail(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
	// 验证数据库返回结果
	if(nDBRetCode < DBRET_CODE_SUCCEED)
	{
		ErrorLn("[onReturnDelMail] del Mail error！ pszDBRetDesc = " << a2utf8(pszDBRetDesc));
		return;
	}
}

/// purpose: 添加邮件     								-- 来自数据库
void MailService::onReturnAddMail(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
	SMailDBNodeData *pNodeData = getSendNodeData(nQueueIndex);
	if (pNodeData == NULL)
	{
		ErrorLn("[onReturnAddMail] add Mail error！pNodeData = NULL");
		return;
	}

	// 验证数据库返回结果
	if(nDBRetCode < DBRET_CODE_SUCCEED)
	{
		ErrorLn("[onReturnAddMail] add Mail error！MailID = " << pNodeData->llMailID << " MailTitle = " << pNodeData->sendData.szTitle << " pszDBRetDesc = " << a2utf8(pszDBRetDesc));
		if (pNodeData->sendData.bIsPlusData)
		{
			ErrorLn("[onReturnAddMail] add Mail error！PlusMoney = " << pNodeData->sendData.nPlusMoney << " PLusExp = " << pNodeData->sendData.nPlusExp);
			for (int i = 0; i < pNodeData->sendData.nPlusGoodsNum; ++i)
			{
				ErrorLn("[onReturnAddMail] add Mail error！GoodsID = " << pNodeData->sendData.sGoodsList[i].dwGoodsID << " Count =" << pNodeData->sendData.sGoodsList[i].nCount);
			}
		}

		removeSendNodeData(nQueueIndex);

		return;
	}

	CPlayerMail *pPlayerMail = findPlayerMail(pNodeData->sendData.dwMasterPDBID);
	if (pPlayerMail != NULL)
	{
		SMailBaseData baseData;
		baseData.llMailID = pNodeData->llMailID;
		baseData.nType = pNodeData->sendData.nType;
		baseData.nSourceType = pNodeData->sendData.nSourceType;
		sstrcpyn(baseData.szTitle, pNodeData->sendData.szTitle, sizeof(baseData.szTitle));
		sstrcpyn(baseData.szSenderName, pNodeData->sendData.szSenderName, sizeof(baseData.szSenderName));
		baseData.dwSenderPDBID = pNodeData->sendData.dwSenderPDBID;
		baseData.bIsPlusData = pNodeData->sendData.bIsPlusData;
		baseData.bRead = false;
		baseData.dwRecvTime = pNodeData->sendData.dwRecvTime;
		baseData.dwLifeTime = pNodeData->sendData.dwLifeTime;

		pPlayerMail->addMailBaseData(&baseData);
		pPlayerMail->notifyPlayerNewMailCount();
	}

	removeSendNodeData(nQueueIndex);
}

/// purpose: 添加邮件消息处理					-- 来自场景服
void MailService::onSceneAddMail(DWORD dwServerID, void* pData, size_t stLen)
{
	if (pData == NULL || stLen != sizeof(SMsgMail_AddMail_SO))
		return;

	SMsgMail_AddMail_SO* pSendData = (SMsgMail_AddMail_SO*)pData;

	sendMailToDB(&(pSendData->sMailData), pSendData->nMailFillID, pSendData->szParam);
}

/// purpose: 删除邮件消息处理					-- 来自场景服
void MailService::onSceneDelMail(DWORD dwServerID, void* pData, size_t stLen)
{
	if (pData == NULL || stLen != sizeof(SMsgMail_DelMail_SO))
		return;

	SMsgMail_DelMail_SO* pRecvData = (SMsgMail_DelMail_SO*)pData;

	CPlayerMail *pPlayerMail = findPlayerMail(pRecvData->dwPDBID);
	if (pPlayerMail == NULL)
	{
		return;
	}

	pPlayerMail->delMail(pRecvData->llMailID);
}

/// purpose: 设置邮件已读消息处理					-- 来自场景服
void MailService::onSceneSetMailRead(DWORD dwServerID, void* pData, size_t stLen)
{
	if (pData == NULL || stLen != sizeof(SMsgMail_SetMailRead_SO))
		return;

	SMsgMail_SetMailRead_SO* pRecvData = (SMsgMail_SetMailRead_SO*)pData;

	CPlayerMail *pPlayerMail = findPlayerMail(pRecvData->dwPDBID);
	if (pPlayerMail == NULL)
	{
		return;
	}

	pPlayerMail->setMailRead(pRecvData->llMailID);
}

/// purpose: 请求玩家邮件列表
void MailService::onMsgRequestMailList(DWORD dwServerID, void* pData, size_t stLen)
{
	if (pData == NULL || stLen != sizeof(SMsgMail_MailList))
		return;

	SMsgMail_MailList* pRecvData = (SMsgMail_MailList*)pData;

	CPlayerMail *pPlayerMail = findPlayerMail(pRecvData->dwPDBID);
	if (pPlayerMail == NULL)
	{
		return;
	}

	pPlayerMail->sendMailListToClient();
}

void MailService::onMsgRefreshMailList(DWORD dwServerID, void* pData, size_t stLen)
{
	if (pData == NULL || stLen != sizeof(SMsgMail_RefreshMailList))
		return;

	SMsgMail_RefreshMailList* pRecvData = (SMsgMail_RefreshMailList*)pData;

	CPlayerMail *pPlayerMail = findPlayerMail(pRecvData->dwPDBID);
	if (pPlayerMail == NULL)
	{
		return;
	}

	readPlayerMailList(pRecvData->dwPDBID);
}

void MailService::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    switch(wEventID)
    {
    default:
        break;
    }
}

void MailService::OnLogin(ISharePerson * pSharePerson, ELoginMode nLineType)
{
    if (pSharePerson == NULL)
        return;

    PDBID PersonDBID = pSharePerson->GetNumProp(PROPERTY_ID);

	if (g_EHelper.IsComputer(PersonDBID) == true)
		return;

	CPlayerMail *pPlayerMail = findPlayerMail(PersonDBID);
	if (pPlayerMail != NULL)
	{
		pPlayerMail->notifyPlayerNewMailCount();
	}
	else
	{
		if (nLineType != elogin_online)
			return;

		pPlayerMail = new CPlayerMail(PersonDBID);
		if (pPlayerMail == NULL)
		{
			ErrorLn(__FUNCTION__": pPlayerMail == NULL, PDBID = " << PersonDBID);
			return;
		}

		m_PlayerMailList[PersonDBID] = pPlayerMail;

		readPlayerMailList(PersonDBID);
	}
}

void MailService::OnLogout(ISharePerson * pSharePerson, ELogoutMode nLineType)
{
	if (nLineType != elogout_offline)
	{
		return;
	}

	if (pSharePerson == NULL)
	{
		return;
	}

	PDBID PersonDBID = pSharePerson->GetNumProp(PROPERTY_ID);

	if (g_EHelper.IsComputer(PersonDBID) == true)
	{
		return;
	}

	removePlayerMail(PersonDBID);
}

void MailService::OnPre_Update(ISharePerson * pSharePerson, SHAREUPDATE_REASON nUpdateReason)
{
    return;
}

void MailService::OnPost_Update(ISharePerson * pSharePerson, SHAREUPDATE_REASON nUpdateReason)
{
    PDBID PersonDBID = pSharePerson->GetNumProp(PROPERTY_ID);
    if (PersonDBID == INVALID_PDBID)
    {
        return;
    }

    // 属性更新
    switch(nUpdateReason)
    {
    case SHAREUPDATE_REASON_CHANGE_FACEID:
        {
        }
        break;
    case SHAREUPDATE_REASON_CHANGE_SEX:
        {
        }
        break; 
    default:
        break;
    }

    return;
}

////////////////////////////////ITransmitHandler//////////////////////////////////////////
/** 消息处理
@param server	源服务器ID
@param uMsgID	消息ID
@param head		消息头
@param data		消息
@param len		消息长度
*/
void MailService::onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len)
{
    MailHelper MailHelper;
    IMailService *pMailService = MailHelper.m_ptr;
    if (pMailService == NULL)
    {
        ErrorLn(__FUNCTION__": server ="<<server<<" uMsgID ="<<uMsgID);
        return;
    }

	PACKAGE_PTR pkg( new string((const char*)data,len));
	pMailService->handleServerMsg( server, *head, pkg );
}

////////////////////////////////IMessageHandler//////////////////////////////////////////
/** 消息处理
@param msg 网络消息
*/
void MailService::onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead* head, void* data, size_t len)
{
    MailHelper MailHelper;
    IMailService *pMailService = MailHelper.m_ptr;
    if (pMailService == NULL)
    {
        ErrorLn(__FUNCTION__": clientID ="<<clientID<<" uMsgID ="<<uMsgID);
        return;
    }

    PACKAGE_PTR pkg( new string((const char*)data,len));
    pMailService->handleClientMsg( clientID, *head, pkg );
}


void MailService::handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg)
{
    size_t len = msg->size();
    void *data = (void *)msg->c_str();

    // 服务器发过来的消息
    switch ( head.byKeyAction )
    {
    case MSG_MAIL_REQUESTMAILLIST:
        {
            onMsgRequestMailList(serverID, data, len);
        }
        break;
	case MSG_MAIL_ADDMAIL_SO:
		{
			onSceneAddMail(serverID, data, len);
		}
		break;
	case MSG_MAIL_DELMAIL_SO:
		{
			onSceneDelMail(serverID, data, len);
		}
		break;
	case MSG_MAIL_SETMAILREAD_SO:
		{
			onSceneSetMailRead(serverID, data, len);
		}
		break;
	case MSG_MAIL_REFRESHMAILLIST:
		{
			onMsgRefreshMailList(serverID, data, len);
		}
		break;
    default:
        {
            // 其他场景服消息
            ErrorLn(__FUNCTION__": unknown byKeyAction="<< head.byKeyAction);
        }
    }
}

void MailService::handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg)
{
    size_t len = msg->size();
    void *data = (void *)msg->c_str();

    // 服务器发过来的消息
    switch ( head.byKeyAction )
    {
    default:
        {
            // 其他场景服消息
            ErrorLn(__FUNCTION__": unknown byKeyAction= "<< head.byKeyAction);
        }
    }
}

/// 发送邮件
bool MailService::sendMail(SMailSendData sSendData, int nMailFillID, LPCSTR szParam)
{
	return sendMailToDB(&sSendData, nMailFillID, szParam);
}

/**
@name         : 通知服务器列表已经更新
@brief        : 服务器列表更新只会在有服务器进入退出时触发
@note         : 具体的列表调用ICenterServerConnector::GetServerList获得
*/
void MailService::onServerListUpdated()
{

}

/** 通知服务器信息更新
@param nState		: SERVER_STATE
@param pServerData	: ServerData
@brief        : 例如服务器人数变化等等
*/
void MailService::onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData)
{
    ServerData *pInfo = (ServerData *)pServerData;
    if (nState == SERVER_CONNECTED && pInfo->wServerType == MSG_ENDPOINT_SCENE)
    {
    }
    else if (nState == SERVER_CONNECTED && pInfo->wServerType == MSG_ENDPOINT_SOCIAL)
    {
    }
    return;
}

void MailService::ExecSP(DWORD cmdID,LPCSTR pData, int len)
{
    if (pData == NULL || len <= 0)
    {
        return;
    }

    IDBEngineService* pDBEngine = (IDBEngineService*)gSocialGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
        return;
    }

    obuf oBuff;
    oBuff.push_back(pData, len);

    if (pDBEngine->executeSP(cmdID, 0, oBuff.data(), SIZE_INT(oBuff.size()), static_cast<IDBRetSink *>(this)) == false)
    {
        ErrorLn(__FUNCTION__": pDBEngine->ExecuteSP()  cmdID = "<<cmdID<<" failed");
    }
}

/// purpose: 发送邮件
bool MailService::sendMailToDB(SMailSendData *pSendData, int nMailFillID, LPCSTR szParam)
{
	if (isPublicGameWorld() == true)
		return false;

	if (pSendData == NULL)
		return false;

	IDBEngineService* pDBEngine = (IDBEngineService*)gSocialGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
		return false;
	}

	if(pSendData->nPlusGoodsNum > MAIL_GOODS_MAX_NUM)
		return false;

	LONGLONG llMailID = CreateMailID();
	pSendData->dwRecvTime = (DWORD)time(NULL);
	pSendData->dwLifeTime = MAIL_LIFE_TIME_LEN;
	pSendData->bIsPlusData = false;

	if (nMailFillID > emMailFill_Begin && !fillMailContent(pSendData, nMailFillID, szParam))
	{
		return false;
	}

	// 只有系统邮件才允许金币和经验
	if (pSendData->nType != emMailType_System)
	{
		pSendData->nPlusMoney = 0;
		pSendData->nPlusExp = 0;
	}

	if (pSendData->nType != emMailType_Notify && (pSendData->nPlusExp > 0 || pSendData->nPlusMoney > 0 || pSendData->nPlusGoodsNum > 0))
	{
		pSendData->bIsPlusData = true;
	}

	SMailDBNodeData nodeData;
	nodeData.nQueueIndex = getQueueIndex();
	nodeData.llMailID = llMailID;
	memcpy(&nodeData.sendData, pSendData, sizeof(SMailSendData));
	m_SendNodeList.push_back(nodeData);

	DBREQ_PARAM_ADDMAIL refer;
	refer.llMailID = llMailID;
	refer.nType = pSendData->nType;
	refer.nSourceType = pSendData->nSourceType;
	sstrcpyn(refer.szTitle, pSendData->szTitle, sizeof(refer.szTitle));
	sstrcpyn(refer.szContext, pSendData->szContext, sizeof(refer.szContext));
	sstrcpyn(refer.szSenderName, pSendData->szSenderName, sizeof(refer.szSenderName));
	refer.dwSenderPDBID = pSendData->dwSenderPDBID;
	refer.dwMasterPDBID = pSendData->dwMasterPDBID;
	refer.bIsPlusData = pSendData->bIsPlusData;
	refer.dwRecvTime = pSendData->dwRecvTime;
	refer.dwLifeTime = pSendData->dwLifeTime;
	refer.nPlusMoney = pSendData->nPlusMoney;
	refer.nPlusExp = pSendData->nPlusExp;
	refer.nPlusGoodsNum = pSendData->nPlusGoodsNum;

	for (int i=0; i < pSendData->nPlusGoodsNum; ++i)
	{
		refer.sGoodsList[i].dwGoodsID = pSendData->sGoodsList[i].dwGoodsID;
		refer.sGoodsList[i].nCount = pSendData->sGoodsList[i].nCount;
	}

	if (pDBEngine->executeSP(GAMEDB_REQUEST_ADD_MAIL, nodeData.nQueueIndex, (LPCSTR)&refer, sizeof(DBREQ_PARAM_ADDMAIL), this) == false)
	{
		ErrorLn(__FUNCTION__": GAMEDB_REQUEST_ADD_MAIL failed！MailID = " << llMailID << " MailTitle = " << pSendData->szTitle);
		if (pSendData->bIsPlusData)
		{
			ErrorLn(__FUNCTION__": GAMEDB_REQUEST_ADD_MAIL failed！PlusMoney = " << pSendData->nPlusMoney << " PLusExp = " << pSendData->nPlusExp);
			for (int i = 0; i < pSendData->nPlusGoodsNum; ++i)
			{
				ErrorLn(__FUNCTION__": GAMEDB_REQUEST_ADD_MAIL failed！GoodsID = " << pSendData->sGoodsList[i].dwGoodsID << " Count =" << pSendData->sGoodsList[i].nCount);
			}
		}

		removeSendNodeData(nodeData.nQueueIndex);
		return false;
	}

	recordSendMailLog(llMailID, pSendData);

	return true;
}

/// purpose: 填充邮件内容
bool MailService::fillMailContent(SMailSendData *pSendData, int nMailFillID, LPCSTR szParam)
{
	if (pSendData == NULL)
		return false;

	if (nMailFillID <= emMailFill_Begin || nMailFillID >= emMailFill_Max)
		return false;

	SMailConfig *pMailConfig = g_EHelper.getMailConfig(nMailFillID);
	if (pMailConfig == NULL)
		return false;

	pSendData->dwLifeTime = pMailConfig->nMailLifeTime;
	sstrcpyn(pSendData->szTitle, pMailConfig->szTitle, sizeof(pSendData->szTitle));
	sstrcpyn(pSendData->szSenderName, pMailConfig->szSenderName, sizeof(pSendData->szSenderName));

	if (szParam != NULL)
	{
		sstrcpyn(pSendData->szContext, g_EHelper.getFillMailContent(pMailConfig->szContext, szParam).c_str(), sizeof(pSendData->szContext));
	}
	else
	{
		sstrcpyn(pSendData->szContext, pMailConfig->szContext, sizeof(pSendData->szContext));
	}

	if (pSendData->nType == emMailType_Notify && pMailConfig->nPrizeIDCount > 0 && pMailConfig->nPrizeIDCount <= MAIL_GOODS_MAX_NUM)
	{
		pSendData->nPlusGoodsNum = 0;
		for (int i = 0; i < pMailConfig->nPrizeIDCount; ++i)
		{
			if (pMailConfig->arrayPrizeID[i] > 0)
			{
				pSendData->sGoodsList[i].dwGoodsID = pMailConfig->arrayPrizeID[i];
				pSendData->sGoodsList[i].nCount = 1;
				++pSendData->nPlusGoodsNum;
			}
		}
	}

	return true;
}

/// purpose: 添加邮件日志
void MailService::recordSendMailLog(LONGLONG llMailID, SMailSendData *pSendData)
{
	if (pSendData == NULL)
		return;

	// 记录日志
	IOSSLogService* pOSSLogService = gSocialGlobal->getOSSLogService();
	if (pOSSLogService == NULL)
		return;

	char szTmpText[512] = {0};
	if (pSendData->nPlusGoodsNum > 0)
	{
		for (int i = 0; i < pSendData->nPlusGoodsNum; ++i)
		{
			if (pSendData->sGoodsList[i].dwGoodsID > 0)
			{
				if (i == 0)
				{
					ssprintf_s(szTmpText, sizeof(szTmpText), a2utf8("奖励id：%u"), pSendData->sGoodsList[i].dwGoodsID);
				}
				else
				{
					ssprintf_s(szTmpText, sizeof(szTmpText), a2utf8("%s,奖励id：%u"), szTmpText, pSendData->sGoodsList[i].dwGoodsID);
				}
			}
		}
	}

	char szText[1024] = {0};
	ssprintf_s(szText, sizeof(szText), a2utf8("邮件id：[%lld]%s[%u]寄给[%u]%s的邮件，金币：%d，经验：%d，%s"), llMailID, pSendData->szSenderName, pSendData->dwSenderPDBID, 
		pSendData->dwMasterPDBID, pSendData->szTitle, pSendData->nPlusMoney, pSendData->nPlusExp, szTmpText);

	pOSSLogService->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_MAIL, 0, szText);
}

// purpose: 生成邮件ID
LONGLONG MailService::CreateMailID()
{
	time_t tt = time(NULL);

	char buf[256] = {0};
	strftime(buf, sizeof(buf), "%y%m%d%H%M", localtime(&tt));
	sprintf_s(buf, "%s%05d", buf, getMailIndex());

	LONGLONG llTime;
	stringstream ss;
	ss << buf;
	ss >> llTime;

	LONGLONG llMailID = (1000 + getThisGameWorldID()) * 1000000000000000 + llTime;

	return llMailID;
}

// purpose: 获取邮件自增index
int MailService::getMailIndex()
{
	if (m_MailIndex >= MAIL_INDEX_MAX)
		m_MailIndex = 0;

	return ++m_MailIndex;
}

// purpose: 获取节点序号
int MailService::getQueueIndex()
{
	return ++m_nQueueIndex;
}

// purpose: 获取节点数据
SMailDBNodeData* MailService::getSendNodeData(int nQueueIndex)
{
	list<SMailDBNodeData>::iterator it = m_SendNodeList.begin();
	for (; it != m_SendNodeList.end(); ++it)
	{
		if ((*it).nQueueIndex == nQueueIndex)
		{
			return &(*it);
		}
	}

	return NULL;
}

// purpose: 删除节点数据
void MailService::removeSendNodeData(int nQueueIndex)
{
	list<SMailDBNodeData>::iterator it = m_SendNodeList.begin();
	for (; it != m_SendNodeList.end(); ++it)
	{
		if ((*it).nQueueIndex == nQueueIndex)
		{
			m_SendNodeList.erase(it);
			return;
		}
	}
}

/// purpose: 读取邮件列表信息
bool MailService::readPlayerMailList(DWORD dwPDBID)
{
    IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        return false;
    }

    DBREQ_PARAM_READMAILLIST refer;
    refer.dwPDBID = dwPDBID;
	refer.nCurDate = (int)time(NULL);

    if (pDBEngine->executeSP(GAMEDB_REQUEST_READ_MAIL_LIST, 0, (LPCSTR)&refer, sizeof(refer), this) == false)
    {
        ErrorLn(__FUNCTION__": GAMEDB_REQUEST_READ_MAIL_LIST failed!!");
        return false;
    }

    return true;
}

/// purpose: 删除超时邮件数据
void MailService::delOverTimeMailFromDB()
{
	IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		return;
	}

	list<SMailDelInfo>::iterator it = m_DelMailList.begin();
	while (it != m_DelMailList.end())
	{
		DBREQ_PARAM_DELMAIL refer;
		refer.dwPDBID = (*it).dwPDBID;
		refer.llMailID = (*it).llMailID;

		if (pDBEngine->executeSP(GAMEDB_REQUEST_DEL_MAIL, 0, (LPCSTR)&refer, sizeof(refer), this) == true)
		{
			it = m_DelMailList.erase(it);
		}
		else
		{
			++it;
		}
	}
}

/// purpose: 移除玩家邮件数据
void MailService::removePlayerMail(DWORD dwPDBID)
{
	TMap_PlayerMailIt it = m_PlayerMailList.find(dwPDBID);
	if (it == m_PlayerMailList.end())
		return;

	(*it).second->release();

	m_PlayerMailList.erase(it);
}

/// purpose: 查找玩家邮件数据
CPlayerMail* MailService::findPlayerMail(DWORD dwPDBID)
{
	TMap_PlayerMailIt it = m_PlayerMailList.find(dwPDBID);
	if (it == m_PlayerMailList.end())
	{
		return NULL;
	}

	return (*it).second;
}





