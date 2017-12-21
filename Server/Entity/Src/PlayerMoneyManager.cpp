/*******************************************************************
** �ļ���:	PlayerMoneyManager.cpp 
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	��绪
** ��  ��:	7/4/2017
** ��  ��:	1.0
** ��  ��:  ��ҽ�Ǯ������

********************************************************************/

#include "stdafx.h"
#include <functional>

#include "OssLogDef.h"
#include "IGameDatabaseDef.h"
#include "PlayerBankPart.h"
#include "IAntiAddictServer.h"
#include "IOSSLogService.h"
#include "ExternalHelper.h"
#include "PlayerMoneyManager.h"

//////////////////////////////////////////////////////////////////////////

std::function<bool(int,int)> check_money = [](int nCurValue, int nOptValue) -> bool
{
    if ( nOptValue == 0)
        return true;

    // �����ˣ����ܲ���
    if (nCurValue + nOptValue < 0)
    {
        return false;
    }
    // ���ݿ����������ƣ��������жϣ���ֹ���ݿ�ų���
    if (nCurValue + nOptValue > 100000000)
    {
        return false;
    }
    return true;
};

//////////////////////////////////////////////////////////////////////////

// ���캯��
CPlayerMoneyManager::CPlayerMoneyManager()
    : m_pBankPart(nullptr)
    , m_isOperatingToDB(false)    /*  �Ƿ����ڲ�����Ǯ����ȯ����ң���DB��DB���غ��Զ�����false */
{

}

// ��������
CPlayerMoneyManager::~CPlayerMoneyManager()
{
	m_pBankPart = nullptr;
}

// ��ʼ��
bool CPlayerMoneyManager::init(CPlayerBankPart *pBankPart)
{
	m_pBankPart = pBankPart;

	return true;
}

// ȡ�����Ƕ���
inline __IEntity * CPlayerMoneyManager::getMaster() const
{
    return m_pBankPart ? m_pBankPart->getHost() : nullptr;
}

/** ȡ�õ�ȯ�һ���ҵĻ���
@return : 
*/
double CPlayerMoneyManager::getExchangeRate(void)
{
    return 1.5;
}

/** �Ƿ�����ӻ�۳�����
@param nMoney:      ������ȯ/���������ע������Ϊ�۳�������Ϊ����
@param nMoneyType:  �������� GAME_MONEY_TYPE,���Ϊ
@return : 
*/
bool CPlayerMoneyManager::canAddMoneyEx(int nMoney, BYTE nMoneyType)
{
    // �����ִ�����ݿ�����������ν����ܽ��п۳�����
    if ( m_isOperatingToDB && (nMoney < 0) )
    {
        return false;
    }
    __IEntity * pMaster = getMaster();
    if(nullptr == pMaster)
    {
        return false;
    }
    
    int nCurrentTicket  = pMaster->getIntProperty(PROPERTY_TICKET);
    int nCurrentGlod    = pMaster->getIntProperty(PROPERTY_GOLD);

    // ��ȯ
    switch(nMoneyType)
    {
    case MONEY_TICKET:  // ֻ���ĵ�ȯ
        {
            return check_money(nCurrentTicket, nMoney);
        }
        break;

    case MONEY_GOLD:    // ֻ���Ľ��
        {
             return check_money(nCurrentGlod, nMoney);
        }
        break;

    case MONEY_ALL:     // TODO ��ȯ����ת�����
        {
            // TODO��У����
            // �����ȯת��

        }
        break;

    default: break;
    }
    
    return false;
}


/** �Ƿ�����ӻ�۳�����
@param nTicket:  ������ȯ������ע������Ϊ�۳�������Ϊ����
@param nGlod  :  �������������ע������Ϊ�۳�������Ϊ����
@return : 
*/
bool CPlayerMoneyManager::canAddMoney(int nTicket, int nGlod)
{
    // У���ȯ
    if(nTicket != 0)
    {
        if( !canAddMoneyEx(nTicket, MONEY_TICKET) )
        {
            return false;
        }
    }
    // У����
    if(nGlod != 0)
    {
        if( !canAddMoneyEx(nGlod, MONEY_GOLD) )
        {
            return false;
        }
    }

    return true;
}

/** ���ӻ�۳�����
@param nTicket: ��ӵĵ�ȯ��������ʾ�۳����
@param nGlod  : ��ӵĽ�ң�������ʾ�۳����
@param  : nReason ��ӵ�ԭ�򣨲���OSS_RESOURCE_ADD_RANGE/���ĵ�;��OSS_RESOURCE_OPT_DEL/ת��OSS_RESOURCE_OPT_TRANSIN/ת��OSS_RESOURCE_OPT_TRANSOUT��
@param  : pszRemark ��ӵĵ�ȯ��־�ı�ע������־�ã�
@param  : nSubclassID, ����ID��������Ʒ������ƷID��û������Ĭ��0
@param  : nNum ��������������ƷID������Ʒ����
@param  : bAddictLimit���Ƿ��ǳ�������
@ note  : ����ӵ�ȯ�����ύ���ݿ⣨��������ģ���ύ�ģ���û�з���ǰ����ʱ�����ύ��ȯ�ǻ�ʧ�ܵ�
*/
bool CPlayerMoneyManager::addMoney(int nTicket, int nGlod, int nReason, const char * pszRemark/*=0*/, int nSubclassID/*=0*/, short nNum/*=0*/, bool bAddictLimit/*=true*/)
{
    __IPlayerRole * pMaster = CAST_TYPE(__IPlayerRole*, getMaster());
    if(pMaster == 0)
    {
        return false;
    }
    UID uid = pMaster->getUID();
    // ������˥��
    IAntiAddictServer * pAntiAddict = gServerGlobal->getAntiAddictServer();
    if ( pAntiAddict && nGlod > 0 && bAddictLimit)
    {
        int reduce = pAntiAddict->getReduceRate(uid, pMaster->getPlayerLevel());
        if ( reduce < 100 )
        {
            nGlod = ( nGlod * reduce ) / 100;
            char szParam[256];
            ssprintf_s(szParam, sizeof(szParam), a2utf8("%d;������״̬,����˥����%d��,ʵ�ʻ�ý��%d"), CHAT_CHANNEL_SYSTEM, reduce, nGlod);
            g_EHelper.showSystemMessage(uid, CHAT_TIP_ANTIADDICT_INCOME_REDUCE, szParam);
        }
    }
    if (nTicket == 0 && nGlod==0)
    {
        return true;
    }
    // ��������Ϸ�������ӵ�ȯ
    if(nTicket > 0)
    {
        return false;
    }

    // ��һ�ֻ��ڴ����о��������β���
    if (m_isOperatingToDB && (nTicket < 0 || nGlod < 0) )
    {
        return false;
    }

    int nCurrentTicket = pMaster->getIntProperty(PROPERTY_TICKET);
    int nCurrentGlod   = pMaster->getIntProperty(PROPERTY_GOLD);

    // ֻ���ĵ�ȯ����
    if(nTicket==0 || nGlod==0)
    {
        bool checked = true;
        EMChatTipID nMsgID = CHAT_TIP_ENTITY_BEGIN;
        if (nTicket != 0)
        {
            if (!check_money(nCurrentTicket, nTicket))
            {
                nMsgID = CHAT_TIP_ENTITY_TICKET_OPERATE_ERROR;
                checked = false;
            }
        }
        
        if (nGlod != 0)
        {
            if (!check_money(nCurrentGlod, nGlod))
            {
                nMsgID = CHAT_TIP_ENTITY_GOLD_OPERATE_ERROR;
                checked = false;
            }
        }

        // ���Ϸ�
        if(!checked)
        {
            if (nMsgID != CHAT_TIP_ENTITY_BEGIN)
            {
                g_EHelper.showSystemMessage(uid, nMsgID, "");
            }
            return false;
        }

        executeMoneyDataBase(nTicket,nReason,nGlod,nReason,pszRemark,nSubclassID,nNum,0);
    }
    else    // ͬʱҪ���ĵ�ȯ�ͽ�ң�ͬʱʹ�����ֻ��ң��Խ��Ϊ��λ���㣩
    {
        // ���Ϸ�У��
        if (!check_money(nCurrentTicket, nTicket))
        {
            g_EHelper.showSystemMessage(uid, CHAT_TIP_ENTITY_TICKET_OPERATE_ERROR, "");
            return false;
        }
        
        if (!check_money(nCurrentGlod, nGlod))
        {
            g_EHelper.showSystemMessage(uid, CHAT_TIP_ENTITY_GOLD_OPERATE_ERROR, "");
            return false;
        }

        // ���ӽ�Ҳ���
        if(nGlod > 0)
        {
            executeMoneyDataBase(nTicket,nReason,nGlod,nReason,pszRemark,nSubclassID,nNum,0);
        }
        else    // ���Ľ�Ҳ���
        {
            // 1���������ѵ�ȯת���ɽ��(дһ����ȯ���ļ�¼(��Ϊ�Ƕһ���¼����һ��������Ӽ�¼)����Ϸ�⣩DBREQ_PARAM_GAMETICKET_ADDMINE
            int nExhGlod = abs(nTicket) * getExchangeRate(); // ������ĵ�ȯ��������Ľ����
            int nAddGlod = nExhGlod;                         // ��ȯ��������Ľ����

            // TODO ���涼���첽�������쳣ʱ�������鷳��
            executeMoneyDataBase(nTicket,OSSRES_DEL_TICKET_TO_GLOD,nAddGlod,OSSRES_ADD_GLOD_FROM_TICKET,pszRemark,nSubclassID,nNum,0);

            // ������Ľ�� + ��ڲ���Ҫ�۳��Ľ��
            int nCostGlod = -1*nAddGlod + nGlod;

            // 2�� ���Ľ�Ҽ�¼����Ϸ�⣩DBREQ_PARAM_GAMETICKET_ADDMINE
            executeMoneyDataBase(0,0,nCostGlod,OSSRES_DEL_GLOD_EXCHAGE,pszRemark,nSubclassID,nNum,0);

            // �̳ǹ����߼��Լ�Ҫִ�������3�� �̳ǹ�����Ʒ��Ҽ�¼����־�⣩DBREQ_OSS_MONITOR_ACTORTICKET_ITEM��
        }
    }

    // ��������״̬��Ϊtrueʱ�򣬲��ܽ����̳ǹ������
    m_isOperatingToDB = true;

    // TODO ���û���Ϣ�����ͻ���
    return true;
}

// ִ�����ݿ����
inline bool CPlayerMoneyManager::executeMoneyDataBase(int nTicket, int nTickReasonID, int nGold, int nGlodReasonID, const char * pszRemark, int nSubclassID, short nNum, int nReserverID)
{
    __IPlayerRole * pMaster = CAST_TYPE(__IPlayerRole*, getMaster());
    if(pMaster == 0)
    {
        return false;
    }
    IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        WarningLn(__FUNCTION__": failed pDBEngine == NULL, name="<< pMaster->getName());
        return false;
    }
    IOSSLogService * pOSSLogServer = gServerGlobal->getOSSLogService();
    if (pOSSLogServer == NULL)
    {
        WarningLn(__FUNCTION__": failed pOSSLogServer== NULL, name="<< pMaster->getName());
        return false;
    }

    int nPDBID          = pMaster->getIntProperty(PROPERTY_ID);
    int nCurrentTicket  = pMaster->getIntProperty(PROPERTY_TICKET);
    int nCurrentGlod    = pMaster->getIntProperty(PROPERTY_GOLD);
    DWORD dwWorldID     = m_pBankPart->getFromGameWorldID();
    DWORD dwUserID      = m_pBankPart->getUserID();

    // ���Ľ�Ҽ�¼
    DBREQ_PARAM_GAMETICKET_ADDMINE dbParam;
    dbParam.dwWorldID       = dwWorldID;        // ��Ϸ����ID	
    dbParam.dwUserID		= dwUserID;         // �ʺ�ID	
    dbParam.dwActorID		= nPDBID;           // ��ɫID
    dbParam.nAddTick		= nTicket;		    // ��ȯ�� ����Ϊ�ӣ�����Ϊ��
    dbParam.nReasonID		= nTickReasonID;    // �Ӽ�ԭ��
    dbParam.nAddBindTick    = nGold;			// ����� ����Ϊ�ӣ�����Ϊ��
    dbParam.nBindReasonID   = nGlodReasonID;    // �Ӽ�ԭ��
    dbParam.nSubclassID	    = nSubclassID;		// ����ID��������Ʒ������ƷID��û������Ĭ��0
    dbParam.nNum			= nNum;	            // ��������������ƷID������Ʒ����

    if(!pDBEngine->executeSP(GAMEDB_REQUEST_GAMETICKET_ADDMINE, dbParam.dwActorID, (LPCSTR)&dbParam, sizeof(dbParam), m_pBankPart, dwWorldID))
    {
        WarningLn(__FUNCTION__": GAMEDB_REQUEST_GAMETICKET_ADDMINE execute failed,dwUserID ="<< dbParam.dwUserID <<",name="<< pMaster->getName()<<",PDBID="<<nPDBID);
        return false;
    }
    // ��ֵ�ռ�
    if(nTickReasonID > 0 && nTicket != 0) pOSSLogServer->addTicketLog(dwWorldID, nPDBID, nPDBID, nTicket, nTickReasonID, 0, pszRemark);
    if(nGlodReasonID > 0 && nGold != 0) pOSSLogServer->addMoneyLog(dwWorldID, nPDBID, nPDBID, nGold, nGlodReasonID, 0, pszRemark);

    int nNewTicket      = nCurrentTicket + nTicket;
    int nNewBindTicket  = nCurrentGlod + nGold; 

    // ���޸����ڴ����ݣ����滹�������ݿⷵ��Ϊ׼
    pMaster->setIntProperty(PROPERTY_TICKET, nNewTicket);
    pMaster->setIntProperty(PROPERTY_GOLD, nNewBindTicket);
    // �޸�PLAYER�ڴ�����
    SPlayerInfo *  pPlayerInfo = m_pBankPart->getPlayerInfo();
    if(pPlayerInfo)
    {
        pPlayerInfo->dwTicket       = nNewTicket;
        pPlayerInfo->dwBindTicket   = nNewBindTicket;
    }
    return true;
}


// ����Ticket���ݿ����
void CPlayerMoneyManager::onDBReturnTicketAdd( int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
    m_isOperatingToDB = false;

    __IPlayerRole * pMaster = CAST_TYPE(__IPlayerRole*, getMaster());
    if(pMaster == 0)
    {
        return;
    }
    if(m_pBankPart == 0)
    {
        return;
    }

    if ( nDBRetCode != DBRET_CODE_SUCCEED )
    {
        // �����޸�ʧ���¼�
        event_actor_change_money event_data;
        event_data.dwUserID = m_pBankPart->getUserID();
        event_data.dwActorID = (DWORD)pMaster->getIntProperty(PROPERTY_ID);
        event_data.bResult = false;  
        sstrcpyn(event_data.szParam,pszDBRetDesc, sizeof(event_data.szParam));
        pMaster->getEntityEvent()->FireExecute(EVENT_ACTOR_CHANGE_MONEY, (LPCSTR)&event_data, sizeof(event_data));

        // д��Ҽ�����־
        IOSSLogService * pOSSLogServer = gServerGlobal->getOSSLogService();
        if (pOSSLogServer != NULL && pszDBRetDesc != NULL )
        {
            char szText[512] = {0};
            ssprintf_s(szText, sizeof(szText), "Ticket Operate Fail:%s", pszDBRetDesc);
            pOSSLogServer->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_PERSON, nQueueIndex, szText);
        }

        ErrorLn( __FUNCTION__<<": Operate Ticket Failed, Pdbid="<<nQueueIndex<<", nCmdID="<<nCmdID<<",nDBRetCode="<<nDBRetCode<<",Desc="<<pszDBRetDesc );
        return;
    }

    if (nOutLen != sizeof(DBREQ_RESULT_GAMETICKET_ADDMINE))
    {
        return;
    }

    DBREQ_RESULT_GAMETICKET_ADDMINE* pRetData = (DBREQ_RESULT_GAMETICKET_ADDMINE*)(pOutData);
    DWORD dwPDBID = pRetData->dwActorID;
    DWORD dwUserID = pRetData->dwUserID;
    int nTotalTicket = pRetData->nTotalTicket;          // ��ȯ
    int nTotalBindTicket = pRetData->nTotalBindTicket;  // ���

    // �����û�TICKET����
    pMaster->setIntProperty(PROPERTY_TICKET, nTotalTicket);
    // �����û��������
    pMaster->setIntProperty(PROPERTY_GOLD, nTotalBindTicket);

    // �޸�PLAYER�ڴ�����
    SPlayerInfo *  pPlayerInfo = m_pBankPart->getPlayerInfo();
    if(pPlayerInfo)
    {
        int nBeforeTotalTicket = pPlayerInfo->dwTicket;
        pPlayerInfo->dwTicket = nTotalTicket;
        pPlayerInfo->dwBindTicket = nTotalBindTicket;
        // �����޸ĳɹ��¼�
        event_actor_change_money event_data;
        event_data.dwUserID = dwUserID;
        event_data.dwActorID = dwPDBID;
        event_data.bResult = true;     
        event_data.nTotalTicket = pPlayerInfo->dwTicket;
        event_data.nTotalTicketBefore = nBeforeTotalTicket;
        pMaster->getEntityEvent()->FireExecute(EVENT_ACTOR_CHANGE_MONEY, (LPCSTR)&event_data, sizeof(event_data));
    }

    ENTITY_TRACELN(__FUNCTION__": PDBID="<< dwPDBID <<",PROPERTY_TICKET="<<nTotalTicket<<", PROPERTY_GOLD="<<nTotalBindTicket);
}
