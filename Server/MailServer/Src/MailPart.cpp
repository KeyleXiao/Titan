#include "stdafx.h"
#include "IDBEngineService.h"
#include "EventDef.h"
#include "IEntityEvent.h"
#include "IBankPart.h"
#include "IPlayerRole.h"
//#include "EntityHelper.h"
//#include "IPlayerRole.h"
#include "ICenterConnectorService.h"
#include "IMailScene.h"
#include "IMailSceneService.h"
#include "ExternalHelper.h"
#include "MailHelper.h"
#include "OssLogDef.h"
#include "MailPart.h"


///////////////////////////////////////////////////////
MailPart::MailPart()
	:m_pMaster(nullptr)
{
	m_dwLastRecvPlusTime = 0;
	m_bIsFastRecv = false;
	m_FastRecvCount = 0;
	m_FastRecvOverTime = 0;

	m_MailList.clear();
	m_RecvMailList.clear();
}

MailPart::~MailPart()
{

}

short MailPart::getPartID()
{
	return PART_MAIL;
}

bool MailPart::onLoad( __IEntity* pEntity, int nLoginMode)
{
	if ( !pEntity )
	{
		return false;
	}

	m_pMaster = pEntity;
    __IEntityEvent *pEntityEvent = m_pMaster->getEntityEvent();
    if (pEntityEvent == NULL)
    {
        // 注册实体创建消息
        pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CREATE, __FUNCTION__);
    }

    // 数据库接口注册
    gServerGlobal->getDBEngineService()->registerHandler(this, __FUNCTION__);

	return true;
}

__IEntity * MailPart::getHost()
{
	return m_pMaster;
}

void MailPart::release()
{
    // 数据库接口反注册
    gServerGlobal->getDBEngineService()->unregisterHandler(this);

	m_pMaster = 0;
	delete this;
}

void MailPart::restore()
{

}

bool MailPart::onSerialize( rkt::ibuffer & in,int flag )
{
	return true;
}

bool MailPart::deSerialize( rkt::obuf & out,int flag )
{
	return true;
}

// 切换英雄
void MailPart::onChangeHero(int nOldHeroID, int nNewHeroID, bool isBefore)
{

}

////////////////////////////////////////////////////////////////////////////////////////////
bool MailPart::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
    switch (msgId)
    {
    case MSG_MAIL_REQUESTMAILLIST:  // 请求邮件列表
        {
            onMessageRequestMailList(data, len);
        }
        break;

    case MSG_MAIL_REQUESTREADMAIL:  // 请求阅读邮件
        {
            onMessageRequestReadMail(data, len);
        }
        break;

    case MSG_MAIL_REQUESTFASTRECV:  // 请求快速领取
        {
            onMessageRequestAsfRecv(data, len);
        }
        break;

    default:break;
    }
    return true;
}


// 收到客户端请求邮件列表
void MailPart::onMessageRequestMailList(void * data, size_t len)
{
    // 判断参数
    if (nullptr == data || len != sizeof(SMsgMail_MailList))
    {
        return;
    }

    g_EHelper.sendMailSocial(MSG_MAIL_REQUESTMAILLIST, (const char*)data, len);
}

// 收到客户端请求读取邮件
void MailPart::onMessageRequestReadMail(void * data, size_t len)
{
    if (isPublicGameWorld())
        return;

    // 判断参数
    if ( nullptr == data || len != sizeof(SMsgMail_ReadMail) || nullptr == m_pMaster )
    {
        return;
    }

    SMsgMail_ReadMail *pRecvData = (SMsgMail_ReadMail*)data;

    // 已经读取过此邮件
    if (isAreadyRecvMail(pRecvData->llMailID))
        return;

    PDBID dwPDBID = m_pMaster->getIntProperty(PROPERTY_ID);

    // 检查收邮件冷却时间
    if ((DWORD)time(NULL) < getLastRecvPlusTime() + MAIL_RECV_PLUS_CD_LEN)
    {
        g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_MAIL_RECV_PLUS_ING);
        return;
    }

    // 处理邮件详细数据
    if (dealPlayerMailDetailData(pRecvData->llMailID, false))
    {
        return;
    }

    // 读取邮件详细数据
    IDBEngineService*  pDBEngine = gServerGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        ErrorLn(__FUNCTION__" pDBEngine == NULL");
        return;
    }

    DBREQ_PARAM_READMAIL refer;
    refer.dwPDBID = dwPDBID;
    refer.llMailID = pRecvData->llMailID;
    if (pDBEngine->executeSP(GAMEDB_REQUEST_READ_MAIL, dwPDBID, (LPCSTR)&refer, sizeof(DBREQ_PARAM_READMAIL), static_cast<IDBRetSink *>(this)) == false)
    {
        ErrorLn(__FUNCTION__": GAMEDB_REQUEST_READ_MAIL to DB failed");
        return;
    }

    // 设置领取附件时间
    setLastRecvPlusTime((DWORD)time(NULL));
}

// 收到客户端快速领取
void MailPart::onMessageRequestAsfRecv(void * data, size_t len)
{
    if (isPublicGameWorld())
        return;

    // 判断参数
    if (nullptr == data || len != sizeof(SMsgMail_ReadMail) || nullptr == m_pMaster)
    {
        return;
    }

    PDBID dwPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
    SMsgMail_FastRecv *pRecvData = (SMsgMail_FastRecv*)data;

    if ((DWORD)time(NULL) < getLastRecvPlusTime() + MAIL_RECV_PLUS_CD_LEN)
    {
        g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_MAIL_RECV_PLUS_ING);
        return;
    }

    if (isAreadyRecvMail(pRecvData->llMailID))
    {
        return;
    }

    dealPlayerMailDetailData(pRecvData->llMailID, true);
}

////////////////////////////////////////////////////////////////////////////////////////////
// 获取邮件详细数据
bool MailPart::dealPlayerMailDetailData(LONGLONG llMailID, bool isAutoRecvPlus)
{
	if (m_pMaster == NULL)
		return false;

	TMap_MailListIt it = m_MailList.find(llMailID);
	if (it == m_MailList.end())
		return false;

	PDBID dwPDBID = m_pMaster->getIntProperty(PROPERTY_ID);

	SMailDetailData *pDetailData = &((*it).second);
	
	if (isAutoRecvPlus)
	{
		if (dealRecvMailPlus(pDetailData))
		{
			m_MailList.erase(it);
		}
	}
	else
	{
		g_EHelper.send2Client(dwPDBID, MSG_MODULEID_MAIL, MSG_MAIL_RESPONSEREADMAIL, (LPCSTR)pDetailData, sizeof(SMailDetailData));
	}

	return true;
}

// 设置领取附件时间
void MailPart::setLastRecvPlusTime(DWORD dwTime)
{
	m_dwLastRecvPlusTime = dwTime;
}

// 添加邮件详细数据
void MailPart::addPlayerMailDetailData(SMailDetailData sDetailData, bool isAutoRecvPlus)
{
	if (m_pMaster == NULL)
		return;
	
	bool bRecvPlusRet = false;

	// 自动收取附件
	if (isAutoRecvPlus)
	{
		bRecvPlusRet = dealRecvMailPlus(&sDetailData);
	}

	if (!bRecvPlusRet)
	{
		m_MailList[sDetailData.llMailID] = sDetailData;
	}

	m_dwLastRecvPlusTime = 0;
}

// 处理邮件发送附件数据
bool MailPart::dealSendMailPlusData(SMailSendData sendData)
{
	// todo 处理邮件发送附件数据

	return true;
}

// 获取领取附件时间
DWORD MailPart::getLastRecvPlusTime()
{
	return m_dwLastRecvPlusTime;
}

// 邮件是否已领取
bool MailPart::isAreadyRecvMail(LONGLONG llMailID)
{
	for (size_t i = 0; i < m_RecvMailList.size(); ++i)
	{
		if (m_RecvMailList[i] == llMailID)
			return true;
	}

	return false;
}

// 是否在快速取件
bool MailPart::isFastRecvMailPlus()
{
	if (m_bIsFastRecv && (m_FastRecvCount == 0 || (DWORD)time(NULL) > m_FastRecvOverTime))
	{
		m_bIsFastRecv = false;
	}

	return m_bIsFastRecv;
}

// 设置快速取件标识
void MailPart::setFastRecvMailPlus(bool bFlag, int nRecvCount)
{
	if (m_bIsFastRecv == bFlag)
		return;

	m_bIsFastRecv = bFlag;
	m_FastRecvCount = nRecvCount;
	m_FastRecvOverTime = (DWORD)time(NULL) + nRecvCount * MAIL_RECV_PLUS_CD_LEN;
}

void MailPart::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    switch (wEventID)
    {
        case EVENT_ACTOR_TOPUP_SUCCESS:
        {
            if (pszContext == NULL || nLen != sizeof(event_player_topup_success))
                return;

            event_player_topup_success* pData = (event_player_topup_success*)pszContext;

            SMsgMail_RefreshMailList msgData;
            msgData.dwPDBID = pData->dwPDBID;

            g_EHelper.sendMailSocial(MSG_MAIL_REFRESHMAILLIST, (const char*)&msgData, sizeof(msgData));
        }
        break;
    default:
        break;
    }
}

// 收取邮件附件
bool MailPart::dealRecvMailPlus(SMailDetailData *pDetailData)
{
    if (pDetailData == NULL || isAreadyRecvMail(pDetailData->llMailID)) {
        return false;
    }
    __IBankPart * pBankPart = static_cast<__IBankPart*>(m_pMaster->getEntityPart(PART_BANK));
    if (nullptr == pBankPart) {
        return false;
    }


    PDBID dwPDBID = m_pMaster->getIntProperty(PROPERTY_ID);

	// 处理收件预判
	if (pDetailData->nPlusMoney > 0 && !pBankPart->getMoneyMgr()->canAddMoney(0, pDetailData->nPlusMoney))
	{
		g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_MAIL_RECV_PLUS_MONEY_FAIL);
		return false;
	}

	if (pDetailData->nPlusGoodsNum > 0)
	{
		for (int i = 0; i < pDetailData->nPlusGoodsNum; ++i)
		{
			SMailGoodsInfo *pGoodsInfo = &(pDetailData->sGoodsList[i]);

			if (pGoodsInfo->dwGoodsID == 0)
				continue;

			if (!pBankPart->canPrizeActor(pGoodsInfo->dwGoodsID, pGoodsInfo->nCount))
			{
				g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_MAIL_RECV_PLUS_PRIZE_FAIL);
				return false;
			}
		}
	}

	// 删除邮件
	if (!deleteMail(pDetailData->llMailID))
	{
		return false;
	}

	// 处理收件
	char szText[256] = {0};
	ssprintf_s(szText, sizeof(szText), "邮件[mailID=%lld]附件", pDetailData->llMailID);
	LPCSTR pszRemark = a2utf8(szText);

	if (pDetailData->nPlusMoney > 0)
	{
        pBankPart->getMoneyMgr()->addMoney(0, pDetailData->nPlusMoney, OSSRES_ADD_MAILPLUS, pszRemark, 0, 0, false);	// 邮件附件金币
	}

	if (pDetailData->nPlusExp > 0)
	{
        __IPlayerRole* pHero = dynamic_cast<__IPlayerRole*>(m_pMaster);
        if (pHero)
        {
            pHero->addExp(pDetailData->nPlusExp, OSSRES_ADD_MAILPLUS, 0, pszRemark);	// 邮件附件经验
        }
        pBankPart->setActorBaseInfo( pDetailData->nPlusExp, ENPLAYERINFOTYPE_EXP, ENCHANGEDATAMODE_ADD, OSS_NUMERIC_EXP, OSS_RESOURCE_OPT_ADD, OSSRES_ADD_MAILPLUS, dwPDBID, pszRemark, 0);
	}

	// 邮件附件物品
	if (pDetailData->nPlusGoodsNum > 0)
	{
		for (int i = 0; i < pDetailData->nPlusGoodsNum; ++i)
		{
			SMailGoodsInfo *pGoodsInfo = &(pDetailData->sGoodsList[i]);
			if (pGoodsInfo->dwGoodsID > 0)
			{
                pBankPart->prizeActor(pGoodsInfo->dwGoodsID, pGoodsInfo->nCount, OSSRES_ADD_MAILPLUS, 0, pszRemark);
			}
		}
	}

	m_RecvMailList.push_back(pDetailData->llMailID);
	m_dwLastRecvPlusTime = 0;
	NotifyMailRecvSuccess(dwPDBID, pDetailData->llMailID);

	return true;
}

// 通知收件成功
void MailPart::NotifyMailRecvSuccess(DWORD dwPDBID, LONGLONG llMailID)
{
	SMsgMail_RecvSuccess sendData;
	sendData.dwPDBID = dwPDBID;
	sendData.llMailID = llMailID;
	g_EHelper.send2Client(dwPDBID, MSG_MODULEID_MAIL, MSG_MAIL_MAILRECVSUCCESS, (LPCSTR)&sendData, sizeof(SMsgMail_RecvSuccess));
}

// 通知快速收件结果
void MailPart::NotifyFastRecvRet(DWORD dwPDBID, LONGLONG llMailID, bool bRecvRet, bool bStopFlag)
{
	SMsgMail_FastRecv sendData;
	sendData.llMailID = llMailID;
	g_EHelper.send2Client(dwPDBID, MSG_MODULEID_MAIL, MSG_MAIL_REQUESTFASTRECV, (LPCSTR)&sendData, sizeof(SMsgMail_FastRecv));
}


// 删除邮件
bool MailPart::deleteMail(LONGLONG llMailID)
{
    if (nullptr == m_pMaster)
    {
        return false;
    }
    IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
        return false;
    }

    PDBID dwPDBID = m_pMaster->getIntProperty(PROPERTY_ID);

    DBREQ_PARAM_DELMAIL refer;
    refer.dwPDBID = dwPDBID;
    refer.llMailID = llMailID;
    if (pDBEngine->executeSP(GAMEDB_REQUEST_DEL_MAIL, dwPDBID, (LPCSTR)&refer, sizeof(DBREQ_PARAM_DELMAIL), static_cast<IDBRetSink *>(this)) == false)
    {
        ErrorLn(__FUNCTION__": GAMEDB_REQUEST_DEL_MAIL to DB failed");
        return false;
    }

    SMsgMail_DelMail_SO sendData;
    sendData.dwPDBID = dwPDBID;
    sendData.llMailID = llMailID;

    
    g_EHelper.sendMailSocial(MSG_MAIL_DELMAIL_SO, reinterpret_cast<LPCSTR>(&sendData), sizeof(sendData));

    return true;
}

////////////////////////////////////////////////////////////////////////////////////

void MailPart::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
    switch (nCmdID)
    {
    case GAMEDB_REQUEST_READ_MAIL:
        {
            onReturnReadMailData(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
        }
        break;

    case GAMEDB_REQUEST_DEL_MAIL:
        {
            onReturnDelMail(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
        }
        break;

    default: break;
    }
}


// 读取邮件详细数据
void MailPart::onReturnReadMailData(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
    if (nDBRetCode < DBRET_CODE_SUCCEED)
    {
        ErrorLn(__FUNCTION__": data of db is invalid! dwHolder = " << nQueueIndex << ", retCode=" << nDBRetCode << ", desc=" << pszDBRetDesc);
        return;
    }
    if (nOutLen != sizeof(DBREQ_RESULT_READMAIL))
    {
        ErrorLn("[onReturnReadMailData] error length outlength = " << nOutLen << " sizeof(DBREQ_RESULT_READMAIL) = " << sizeof(DBREQ_RESULT_READMAIL));
        return;
    }

    DBREQ_RESULT_READMAIL *pRetData = (DBREQ_RESULT_READMAIL*)pOutData;

    SMailDetailData detailData;
    detailData.llMailID         = pRetData->llMailID;
    detailData.nPlusExp         = pRetData->nPlusExp;
    detailData.nPlusMoney       = pRetData->nPlusMoney;
    detailData.nPlusGoodsNum    = pRetData->nPlusGoodsNum;
    sstrcpyn(detailData.szContext, pRetData->szContext, sizeof(detailData.szContext));

    for (int i = 0; i < pRetData->nPlusGoodsNum; ++i)
    {
        detailData.sGoodsList[i].dwGoodsID = pRetData->sGoodsList[i].dwGoodsID;
        detailData.sGoodsList[i].nCount = pRetData->sGoodsList[i].nCount;
    }

    // 将读取到的邮件发送到客户端
    g_EHelper.send2Client(pRetData->dwPDBID, MSG_MODULEID_MAIL, MSG_MAIL_RESPONSEREADMAIL, (LPCSTR)&detailData, sizeof(SMailDetailData));

    // 内部对邮件进行处理
    if (pRetData->nType == emMailType_Notify)
    {
        deleteMail(pRetData->llMailID);
        setLastRecvPlusTime(0);
    }
    else
    {
        if (pRetData->nPlusExp > 0 || pRetData->nPlusMoney > 0 || pRetData->nPlusGoodsNum > 0)
        {
            addPlayerMailDetailData(detailData, false);
            
            // 将邮件已读标识同步到社会服
            SMsgMail_SetMailRead_SO sendData;
            sendData.dwPDBID    = pRetData->dwPDBID;
            sendData.llMailID   = pRetData->llMailID;
            g_EHelper.sendMailSocial(MSG_MAIL_SETMAILREAD_SO, (LPCSTR)&sendData, sizeof(sendData));
        }
        else
        {
            deleteMail(pRetData->llMailID);
            setLastRecvPlusTime(0);
        }
    }
}

// 删除邮件
void MailPart::onReturnDelMail(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
    // 验证数据库返回结果
    if (nDBRetCode < DBRET_CODE_SUCCEED)
    {
        ErrorLn("[onReturnDelMail] Del Mail error！MailID = " << nQueueIndex << " pszDBRetDesc = " << pszDBRetDesc);
        return;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) __IMailPart * CreateMailPart()
{
	return new MailPart;
}