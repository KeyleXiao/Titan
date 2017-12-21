#include "StdAfx.h"
#include "MailClient.h"
#include "ExternalHelper.h"
#include "MailMsgDef.h"
#include "IClientEntity.h"
#include "GameViewDef.h"
#include "IBankPart.h"
#include "IChatClient.h"
#include "Mail_ManagedDef.h"
#include "Entity_inl.h"

using namespace Mail_ManagerDef;

CMailClient *g_pMailClient = NULL;

// 构造函数
CMailClient::CMailClient()
{
}

// 虚构函数
CMailClient::~CMailClient()
{

}

bool CMailClient::create(void)
{
	// 注册消息处理者
	gClientGlobal->getMessageDispatch()->registerMessageHandler(MSG_MODULEID_MAIL, static_cast<IMessageHandler*>(this));

    // 注册显示层事件
    registerGameViewEvent();

	return true;
}

// 释放
void CMailClient::release(void)
{
	// 取消注册消息处理者
	gClientGlobal->getMessageDispatch()->unregisterMessageHandler(MSG_MODULEID_MAIL, static_cast<IMessageHandler*>(this));

    // 注销显示层事件
    unregisterGameViewEvent();

	g_pMailClient = NULL;

	delete this;
}

////////////////////////IMessageHandler////////////////////
/** 消息处理
@param head : 消息头，通常为一个消息包的前4个字节
@param data : 消息的实际内容，除去了消息头的
@param len  : 内容长度
*/
void CMailClient::onMessage(SNetMsgHead *pHead, void *pData, size_t nLen)
{
    // 客户端消息case宏
#define MSG_CASE(proc, msgID) \
		case msgID:\
    { proc((LPCSTR)pData, nLen);	break;}

    switch (pHead->byKeyAction)
    {
		// 邮件基础信息列表
		MSG_CASE(onMailBaseDataList, MSG_MAIL_REQUESTMAILLIST)

		// 邮件详细信息
		MSG_CASE(onMailDetailData, MSG_MAIL_RESPONSEREADMAIL)

		// 玩家未读邮件总数
		MSG_CASE(onNewMailCountData, MSG_MAIL_NEWMAILCOUNT)

		// 快速收件信息
		//MSG_CASE(onMailFastRecvPlusData, MSG_MAIL_REQUESTFASTRECV)

		// 收件成功
		MSG_CASE(onMailRecvPlusSuccess, MSG_MAIL_MAILRECVSUCCESS)

    default:
        break;
    }
}


bool CMailClient::onViewEvent(int32 eventID,int nParam,const char *strParam,void *ptrParam)
{
    switch (eventID)
    {
	case GVIEWCMD_MAIL_REQUESTBASEDATALIST:
		{
			reqMailBaseDataList();
		}
		break;

    case GVIEWCMD_MAIL_REQUESTDETAILDATA:
        {
			if (strParam == NULL)
			{
				return false;
			}
            gameview_mail_req_detaildata *pData = (gameview_mail_req_detaildata*)ptrParam;
            reqReadMailData(pData->nMailID);
        }
        break;

	case GVIEWCMD_MAIL_REQUESTFASTRECVPLUS:
		{
			if (strParam == NULL)
			{
				return false;
			}
			gameview_mail_req_fastrecvplus *pData = (gameview_mail_req_fastrecvplus*)ptrParam;
			reqFastRecvMailPlus(pData);
		}
		break;

    default:
        break;
    }

    return true;
}

// 注册显示层事件
void CMailClient::registerGameViewEvent(void)
{
	IEntityClient *pEntityClient = gClientGlobal->getEntityClient();
	if (pEntityClient == NULL)
		return;

	pEntityClient->registerGameViewEventHandler( GVIEWCMD_MAIL_REQUESTBASEDATALIST, this );
	pEntityClient->registerGameViewEventHandler( GVIEWCMD_MAIL_REQUESTDETAILDATA, this );
	pEntityClient->registerGameViewEventHandler( GVIEWCMD_MAIL_REQUESTFASTRECVPLUS, this );
}

// 注销显示层事件
void CMailClient::unregisterGameViewEvent(void)
{
	IEntityClient *pEntityClient = gClientGlobal->getEntityClient();
	if (pEntityClient == NULL)
		return;

	pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_MAIL_REQUESTBASEDATALIST );
	pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_MAIL_REQUESTDETAILDATA );
	pEntityClient->unregisterGameViewEventHandler( GVIEWCMD_MAIL_REQUESTFASTRECVPLUS );
}

// 邮件基础信息列表
void CMailClient::onMailBaseDataList(LPCSTR pszMsg, int nLen)
{
	IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
		return;

	IEventEngine *pEventEngine = gClientGlobal->getEventEngine();
	if (pEventEngine == NULL)
		return;

	if (pszMsg == NULL || nLen < sizeof(SMsgMail_MailList))
		return;

	SMsgMail_MailList *pRecvData = (SMsgMail_MailList*)pszMsg;

	int offSet = sizeof(SMsgMail_MailList);
	if (nLen != offSet + pRecvData->nCount * sizeof(SMailBaseData))
	{
		return;
	}

	obuf4096 obData;
	gameview_mail_req_basedatalist sendData;
	sendData.nPDBID = pRecvData->dwPDBID;
	sendData.nCount = pRecvData->nCount;
	obData.push_back(&sendData, sizeof(sendData));

	for (int i=0; i < pRecvData->nCount; i++)
	{
		SMailBaseData *pNode = (SMailBaseData*)(pszMsg + offSet);
		gameview_mail_baseinfo sendNode;
		sendNode.nMailID = pNode->llMailID;
		sendNode.nType = pNode->nType;
		sendNode.nSourceType = pNode->nSourceType;
		sstrcpyn(sendNode.szTitle, pNode->szTitle, sizeof(sendNode.szTitle));
		sstrcpyn(sendNode.szSenderName, pNode->szSenderName, sizeof(sendNode.szSenderName));
		sendNode.nSenderPDBID = pNode->dwSenderPDBID;
		sendNode.nIsPlusData = (pNode->bIsPlusData == true ? 1 : 0);
		sendNode.nIsRead = (pNode->bRead ? 1 : 0);
		sendNode.nRecvTime = pNode->dwRecvTime;
		sendNode.nLifeTime = pNode->dwLifeTime;
		sendNode.nReadFlag = 0;
		obData.push_back(&sendNode, sizeof(sendNode));

		offSet += sizeof(SMailBaseData);
	}

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MAIL_REQUESTBASEDATALIST, obData.size(), 0, obData.data(), obData.size());
}

// 邮件详细信息
void CMailClient::onMailDetailData(LPCSTR pszMsg, int nLen)
{
	IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
		return;

	IEventEngine *pEventEngine = gClientGlobal->getEventEngine();
	if (pEventEngine == NULL)
		return;

	if (pszMsg == NULL || nLen != sizeof(SMailDetailData))
		return;

	SMailDetailData *pRecvData = (SMailDetailData*)pszMsg;

	obuf obData;
	gameview_mail_rsp_detaildata sendData;
	sendData.nMailID = pRecvData->llMailID;
	sstrcpyn(sendData.szContext, pRecvData->szContext, sizeof(sendData.szContext));
	sendData.nPlusMoney = pRecvData->nPlusMoney;
	sendData.nPlusExp = pRecvData->nPlusExp;
	
	int nPlusGoodsNum = 0;
	for (int i=0; i < pRecvData->nPlusGoodsNum; ++i)
	{
		bool bFindFlag = false;
		for (int j = 0; j < nPlusGoodsNum; ++j)
		{
			if (pRecvData->sGoodsList[i].dwGoodsID == sendData.sGoodsList[j].nGoodsID)
			{
				++sendData.sGoodsList[j].nCount;
				bFindFlag = true;
				break;
			}
		}

		if (!bFindFlag)
		{
			sendData.sGoodsList[nPlusGoodsNum].nGoodsID = pRecvData->sGoodsList[i].dwGoodsID;
			sendData.sGoodsList[nPlusGoodsNum].nCount = pRecvData->sGoodsList[i].nCount;
			++nPlusGoodsNum;
		}		
	}

	sendData.nPlusGoodsNum = nPlusGoodsNum;

	obData.push_back(&sendData, sizeof(sendData));

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MAIL_RESPONSEREADMAIL, obData.size(), 0, obData.data(), obData.size());
}

// 玩家未读邮件总数
void CMailClient::onNewMailCountData(LPCSTR pszMsg, int nLen)
{
	IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
		return;

	IEventEngine *pEventEngine = gClientGlobal->getEventEngine();
	if (pEventEngine == NULL)
		return;

	if (pszMsg == NULL || nLen != sizeof(SMsgMail_NewMailCount))
		return;

	SMsgMail_NewMailCount *pRecvData = (SMsgMail_NewMailCount*)pszMsg;

	obuf obData;
	gameview_mail_newmailcount sendData;
	sendData.nCount = pRecvData->nCount;
	sendData.nPlusCount = pRecvData->nPlusCount;
	obData.push_back(&sendData, sizeof(sendData));

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MAIL_NEWMAILCOUNT, obData.size(), 0, obData.data(), obData.size());
}

// 快速收件信息
void CMailClient::onMailFastRecvPlusData(LPCSTR pszMsg, int nLen)
{
	IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
		return;

	IEventEngine *pEventEngine = gClientGlobal->getEventEngine();
	if (pEventEngine == NULL)
		return;

	if (pszMsg == NULL || nLen != sizeof(SMsgMail_FastRecv))
		return;

	SMsgMail_FastRecv *pRecvData = (SMsgMail_FastRecv*)pszMsg;

	obuf obData;
	gameview_mail_req_fastrecvplus sendData;
	sendData.nMailID = pRecvData->llMailID;
	obData.push_back(&sendData, sizeof(sendData));

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MAIL_REQUESTFASTRECVPLUS, obData.size(), 0, obData.data(), obData.size());
}

// 收件成功
void CMailClient::onMailRecvPlusSuccess(LPCSTR pszMsg, int nLen)
{
	IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
		return;

	IEventEngine *pEventEngine = gClientGlobal->getEventEngine();
	if (pEventEngine == NULL)
		return;

	if (pszMsg == NULL || nLen != sizeof(SMsgMail_RecvSuccess))
		return;

	SMsgMail_RecvSuccess *pRecvData = (SMsgMail_RecvSuccess*)pszMsg;

	obuf obData;
	gameview_mail_recvsuccess sendData;
	sendData.nMailID = pRecvData->llMailID;
	obData.push_back(&sendData, sizeof(sendData));

	gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_MAIL_RECVPLUSSUCCESS, obData.size(), 0, obData.data(), obData.size());
}

// 请求邮件基础数据列表
void CMailClient::reqMailBaseDataList()
{
	IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
	{
		return;
	}

	SMsgMail_MailList sendData;
	sendData.dwPDBID = pClientEntity->getIntProperty(PROPERTY_ID);
	sendData.nCount = 0;
    //g_EHelper.sendMailZone(MSG_MAIL_REQUESTMAILLIST, &sendData, sizeof(sendData));
    // 发送到邮件部件
    SendMessageToEntityScene(pClientEntity->getUID(), PART_MAIL, MSG_MAIL_REQUESTMAILLIST, &sendData, sizeof(sendData));
}

// 请求读取邮件
void CMailClient::reqReadMailData(LONGLONG llMailID)
{
    IClientEntity *pClientEntity = gClientGlobal->getHero();
    if (pClientEntity == NULL)
    {
        return;
    }

	SMsgMail_ReadMail sendData;
	sendData.llMailID = llMailID;
	//g_EHelper.sendMailZone(MSG_MAIL_REQUESTREADMAIL, &sendData, sizeof(sendData));
    // 发送到邮件部件
    SendMessageToEntityScene(pClientEntity->getUID(), PART_MAIL, MSG_MAIL_REQUESTREADMAIL, &sendData, sizeof(sendData));
}

// 请求快速领取
void CMailClient::reqFastRecvMailPlus(gameview_mail_req_fastrecvplus *pReqData)
{
    IClientEntity *pClientEntity = gClientGlobal->getHero();
    if (pClientEntity == NULL)
    {
        return;
    }

	if (pReqData == NULL)
		return;

	SMsgMail_FastRecv sendData;
	sendData.llMailID = pReqData->nMailID;
	//g_EHelper.sendMailZone(MSG_MAIL_REQUESTFASTRECV, &sendData, sizeof(sendData));
    // 发送到邮件部件
    SendMessageToEntityScene(pClientEntity->getUID(), PART_MAIL, MSG_MAIL_REQUESTFASTRECV, &sendData, sizeof(sendData));
}

API_EXPORT IMailClient* CreateMailClient(void)
{
	// 如果创建了，则直接返回
	if(g_pMailClient != NULL)
	{
		return g_pMailClient;
	}

	// 实例化系统模块
	g_pMailClient = new CMailClient();
	if(g_pMailClient == NULL || !g_pMailClient->create())
	{
		SAFE_RELEASE(g_pMailClient);

		return NULL;
	}

	return g_pMailClient;
}