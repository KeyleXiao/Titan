#include "stdafx.h"
#include "LegendCupSceneService.h"
#include "LegendCupDef.h"
#include "ICenterConnectorService.h"
#include "IServerGlobal.h"
#include "EntityHelper.h"
#include "LegendCupSceneHelper.h"
#include "OssLogDef.h"
#include "ExternalHelper.h"
#include "IDBEngineService.h"
#include "LegendCupSchemeDef.h"
#include "ITimeSyncService.h"
#include "ClanHelper.h"
#include "IOSSLogService.h"


LegendCupScenService::LegendCupScenService()
	: m_mutex(0)
{

}

void LegendCupScenService::release()
{
	TraceLn("LegendCupSceneService::release");

    IMessageDispatch* pMessageDispatch = gServerGlobal->getMessageDispatch();
    if (nullptr == pMessageDispatch)
    {
        ErrorLn(__FUNCTION__ ": failed! can not found the IMessageDispatch! ");
        return;
    }

    // �Լ���Ҫ������Ϣ
    pMessageDispatch->unregisterTransmitHandler(MSG_MODULEID_LEGENDCUP);

    // ȡ��ע����Ӧ�ͻ�����Ϣ
    pMessageDispatch->unregisterMessageHandler( MSG_MODULEID_LEGENDCUP );

    // ȡ��ע��DB�ص�
	gServerGlobal->getDBEngineService()->unregisterHandler(this);

	delete this;
}

bool LegendCupScenService::create()
{
	TraceLn("LegendCupSceneService::create...");

    IMessageDispatch* pMessageDispatch = gServerGlobal->getMessageDispatch();
    if (nullptr == pMessageDispatch)
    {
        ErrorLn(__FUNCTION__": failed! can not found the IMessageDispatch! ");
        return false;
    }
    // �Լ���Ҫ������Ϣ
    pMessageDispatch->registerTransmitHandler(MSG_MODULEID_LEGENDCUP, this);

    // ����ͻ��˷��͹�������Ϣ
    pMessageDispatch->registerMessageHandler(MSG_MODULEID_LEGENDCUP, this);

    // ע��DB�ص�
	gServerGlobal->getDBEngineService()->registerHandler(this, __FUNCTION__);

	m_u16ServerID = gServerGlobal->getServerID();


	return true;
}


void LegendCupScenService::onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len)
{
    LegendCupSceneHelper orderSceneHelper;
    ILegendCupSceneService *pLegendCupSceneService = orderSceneHelper.m_ptr;
    if (pLegendCupSceneService == NULL)
    {
        ErrorLn(__FUNCTION__": server ="<<server<<" uMsgID ="<<uMsgID);
        return;
    }

    pLegendCupSceneService->handleServerMsg( server, *head, data, len );
}

void LegendCupScenService::onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead* head, void* data, size_t len)
{
    //TraceLn(__FUNCTION__": clientID=" << clientID << " byKeyAction=" << pHead->byKeyAction << " stLen=" << stLen);

    LegendCupSceneHelper orderSceneHelper;
    ILegendCupSceneService *pLegendCupSceneService = orderSceneHelper.m_ptr;
    if (pLegendCupSceneService == NULL)
    {
        ErrorLn(__FUNCTION__": clientID ="<<clientID<<" uMsgID ="<<uMsgID);
        return;
    }

    pLegendCupSceneService->handleClientMsg( clientID, *head, data, len );
}


void LegendCupScenService::handleServerMsg(DWORD serverID, SNetMsgHead head, void* pData, size_t nLen)
{
    size_t len = nLen;
    void *data = pData;

    MATCH_TRACELN(__FUNCTION__": "<<" serverID ="<<serverID << " KeyAction=" << head.byKeyAction);

    switch( head.byKeyAction )
    {
    case  OS_CUP_CAN_CREATECUP:
        {
            onMsgCreateCupResult(data, len);
        }
        break;

    case  OS_CUP_REQUEST_JOIN_CUP:
        {
            onMsgReqJoinCup(data, len);
        }
        break;

    default:
        {
            ErrorLn(__FUNCTION__": unknown KeyAction=" << head.byKeyAction);
        }
    }
}

void LegendCupScenService::handleClientMsg(DWORD client, SNetMsgHead head, void* data, size_t len)
{

    switch( head.byKeyAction )
    {
    case CS_REQUEST_CREATE_CUP_TYPE:         // ���󴴽���������
        {
            onMsgCreateCupType(client, data, len);
        }
        break;
    case CS_REQUEST_CREATE_CUP:         // ���󴴽�����
        {
            onMsgCreateCup(client, data, len);
        }
        break;
    case CS_REQ_JOIN_LEGENDCUP_ROOM:         // ������뱭������
        {
            onMsgReqJoinLegendcupRoom(client, data, len);
        }
        break;
        
    default:
        {
            ErrorLn(__FUNCTION__": unknown pHead->byKeyAction=" << head.byKeyAction);
        }
    }
}

void LegendCupScenService::onMsgCreateCup(ClientID clientID, void* pData, size_t stLen)
{
    if(pData==NULL|| stLen < sizeof(SMsgCreateCup))
    {
        return;
    }

    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService * pActorService = helper.m_ptr;
    if(pActorService == NULL)
    {
        ErrorLn(__FUNCTION__": pActorService == NULL clientID= "<<clientID);
        return;
    }

    DWORD dwActorID = pActorService->getProperty_Integer(PROPERTY_ID);
    DWORD dwClanID = pActorService->getProperty_Integer(PROPERTY_CLAN);
    SMsgCreateCup * pMsg = (SMsgCreateCup *)(pData);
    pMsg->dwCreataActorID = dwActorID;
    pMsg->dwClanID = dwClanID;

    //�����±���
    if(emCupType_Clan == pMsg->byCreateCupType)
    {
        pMsg->bClanLimit = true;
    }
    else
    {
        pMsg->bClanLimit = false;
    }

    // ���͵�����У���Ƿ��д����������˱���
    ICenterConnectorService * pConnector = gServerGlobal->getCenterConnectorService();
    if(pConnector == NULL)
    {
        ErrorLn( __FUNCTION__ ": CenterConnectorService==NULL");
        return;
    }
    obuf oData;
    oData.push_back(pData, stLen);
    pConnector->sendToSocial(MSG_MODULEID_LEGENDCUP, SO_CUP_CREATECUP_CHECK, oData.data(), oData.size());
}

void LegendCupScenService::onMsgCreateCupType(ClientID clientID, void* pData, size_t stLen)
{
    if(pData==NULL|| stLen < sizeof(SMsgCreateCupType))
    {
        return;
    }

    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService * pActorService = helper.m_ptr;
    if(pActorService == NULL)
    {
        ErrorLn(__FUNCTION__": pActorService == NULL clientID= "<<clientID);
        return;
    }
    // ��鴴��������
    SMsgCreateCupType * pMsg = (SMsgCreateCupType *)(pData);

    //�ж�����������
    bool isFreeCreate = false;
    DWORD dwActorID = pActorService->getProperty_Integer(PROPERTY_ID);
    switch(pMsg->byCreateType)
    {
    case emCupType_System :  // ϵͳ����
        {
            WarningLn(__FUNCTION__": can't create system legendcup!");
            return;
        }
        break;
    case emCupType_Personal :  // ���˱���
        {
            // �����˿��Դ���������ѵĴ���
            isFreeCreate = false;
        }
        break;
    case emCupType_Clan :  // ���˱���
        {
            // ������˿��Դ���������ѵĴ���
            ClanHelper clanHelper;
            if(! clanHelper.m_ptr)
            {
                WarningLn(__FUNCTION__": can't clanHelper == NULL!");
                return;
            }
            DWORD dwClanID = helper.m_ptr->getProperty_Integer(PROPERTY_CLAN);
            //����ӵ����
            bool bClanOwner = (clanHelper.m_ptr->getNumProp(dwClanID, emClanProp_ShaikhID) == dwActorID);
            if(!bClanOwner)
            {
                g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_PLAYER_ISNOT_CLAN_SHAIKH);
                return;
            }

            //���˻�Ծ��
            int nCupActivityLimit = 0;
            int nClanActivity = clanHelper.m_ptr->getNumProp(dwClanID, emClanProp_Activity);
            if(nClanActivity < nCupActivityLimit)
            {
                // �����շ����˱���
                char szBuf[10] = {0};
                sprintf_s(szBuf, sizeof(szBuf), "%d", nCupActivityLimit);
                g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_CLAN_ACTIVITY_LESS_CONFIG,szBuf);
            }

            //���˱�������
            int nFreeLegendCupCount = clanHelper.m_ptr->getNumProp(dwClanID, emClanProp_LegendCupCount);
            if(nFreeLegendCupCount > 0)
            {
                // �����շ����˱���
                isFreeCreate = true;
            }

        }
        break;
    default:
        {
            ErrorLn(__FUNCTION__": create error legendcup type="<<pMsg->byCreateType);
            return;
        }
        break;
    }

    // �·�����������������Ϣ���ͻ���
	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint= MSG_ENDPOINT_CLIENT;
	head.byKeyModule   = MSG_MODULEID_LEGENDCUP;
	head.byKeyAction   = SC_CUP_CREATE_CUP_TYPE; // ���������������      

	// �����·�����
    SMsgClientCreateCupType sMsgInfo;
	sMsgInfo.byCreateType = pMsg->byCreateType;
    sMsgInfo.bIsFree      = isFreeCreate;

	obuf obmsg;
	obmsg << head ;
	obmsg.push_back(&(sMsgInfo), sizeof(SMsgClientCreateCupType));
	helper.m_ptr->send2Client(PACKAGE_PTR(new string(obmsg.data(),obmsg.size())));
}


void LegendCupScenService::onMsgReqJoinCup(void* pData, size_t nLen)
{
    if(pData==NULL|| nLen != sizeof(SMsgJoinCup))
    {
        return;
    }

    SMsgJoinCup * pMsg = (SMsgJoinCup *)(pData);

    ActorHelper helper(pMsg->dwActorID, ACTOR_DBID);
    IActorService * pActorService = helper.m_ptr;
    if(pActorService == NULL)
    {
        ErrorLn(__FUNCTION__": pActorService == NULL dwActorID= "<<pMsg->dwActorID);
        return;
    }

    DWORD dwActorID = pMsg->dwActorID;
    int nRegisterNeedGold = pMsg->dwRegisterNeedTicket;

    if (nRegisterNeedGold < 0)
    {
        ISchemeLegendCup* pSchemeLegendCup = gServerGlobal->getSchemeCenter()->getSchemeLegendCup();
        if (pSchemeLegendCup == NULL)
        {
            ErrorLn(__FUNCTION__": pSchemeLegendCup == NULL");
            return;
        }
        SSchemeLegendCup* pConfig = pSchemeLegendCup->getConfig(pMsg->nCreateConfigID);
        if (pConfig == NULL)
        {
            ErrorLn(__FUNCTION__": pConfig == NULL nCreateConfigID= "<<pMsg->nCreateConfigID);
            return;
        }

        // �۳�
        if (pConfig->m_nRegisterNeedGold < 0)
        {
            ErrorLn(__FUNCTION__": config register gold is < 0!")
            return;
        }

        nRegisterNeedGold = pConfig->m_nRegisterNeedGold;
    }
    
    if (nRegisterNeedGold < 0)
    {
        ErrorLn(__FUNCTION__": nRegisterNeedGold < 0");
        return;
    }

    // �������У��
    bool bTickIsEnough = pActorService->canAddMoney(0, -(nRegisterNeedGold));
    if ( !bTickIsEnough)
    {
        // �������У��
        g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_NOT_ENOUGH_REGISTER_GOLD);
        return;
    }


    // �۳����뱭�����
    pActorService->addMoney(0, -nRegisterNeedGold, OSSRES_DEL_LEGENDCUP_REQ_JOINCUP, 0, 0, 0);// ������뱭��

    // ���͵��������뱭��
    ICenterConnectorService * pConnector = gServerGlobal->getCenterConnectorService();
    if(pConnector == NULL)
    {
        ErrorLn( __FUNCTION__ ": CenterConnectorService==NULL");
        return;
    }
    obuf oData;
    oData.push_back(pData, nLen);
    pConnector->sendToSocial(MSG_MODULEID_LEGENDCUP, SO_CUP_REQ_JOIN_CUP_SUCCESS, oData.data(), oData.size());
}

void LegendCupScenService::onMsgCreateCupResult(void* pData, size_t nLen)
{
    if(pData==NULL|| nLen != sizeof(SMsgCreateCup))
    {
        return;
    }
    
    ClanHelper clanHelper;
    if(! clanHelper.m_ptr)
    {
        WarningLn(__FUNCTION__": can't clanHelper == NULL!");
        return;
    }

    SMsgCreateCup * pMsg = (SMsgCreateCup *)(pData);

    ActorHelper helper(pMsg->dwCreataActorID, ACTOR_DBID);
    IActorService * pActorService = helper.m_ptr;
    if(pActorService == NULL)
    {
        ErrorLn(__FUNCTION__": pActorService == NULL dwCreataActorID= "<<pMsg->dwCreataActorID);
        return;
    }

    DWORD dwActorID = pMsg->dwCreataActorID;
    IDBEngineService* pDBEngine = gServerGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        return;
    }

    ISchemeLegendCup* pSchemeLegendCup = gServerGlobal->getSchemeCenter()->getSchemeLegendCup();
    if (pSchemeLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__": pSchemeLegendCup == NULL");
        return;
    }
    SSchemeLegendCup* pConfig = pSchemeLegendCup->getConfig(pMsg->nCreateConfigID);
    if (pConfig == NULL)
    {
        ErrorLn(__FUNCTION__": pConfig == NULL nCreateConfigID= "<<pMsg->nCreateConfigID);
        return;
    }
    if(pMsg->nContriBonus < 0)
    {
        // ���ʽ���С��0
        g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_CONTRIBONUS_MONEY_LESS_ZERO);
        return;
    }

    if (pMsg->nRegisterNeedTicket < 0)
    {
        g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_REGISTER_MONEY_LESS_ZERO);
        return;
    }

    if (pMsg->nRegisterNeedTicket > pConfig->m_nHighestRegisterGold)
    {
        char szParam[32]={0};
        ssprintf_s(szParam, sizeof(szParam), "%d", pConfig->m_nHighestRegisterGold);
        g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_OVER_HIGHEST_REGISTER_MONEY, szParam);
        return;
    }
    

    // �����  ����������ң���ѵ�Ϊ0�� + ���ʽ��
    int nNeedMoney = pConfig->m_nNeedMoneyGold + pMsg->nContriBonus;
    if (nNeedMoney < 0)
    {
        ErrorLn(__FUNCTION__":create legendCup money is not right!!  check the config LegendCup.csv!");
        return;
    }

    if (nNeedMoney > 0)
    {
        // ���У��
        bool bGoldIsEnough = pActorService->canAddMoney(0, -nNeedMoney);
        if ( !bGoldIsEnough)
        {
            // ��ʾ��Ҳ���
            g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_NOT_ENOUGH_MONEY);
            return;
        }
    }

    // todo ����߻�Ҫ���һ��ǵ�� Ŀǰû�ж�
    //if (pMsg->nContriBonus < 0)
    //{
    //    g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_CANT_LESS_THAN_ZERO);
    //    return;
    //}

    //if (pMsg->nContriBonus > 0)
    //{
    //    // ���ʵ��У��
    //    bool bTickIsEnough = pActorService->canAddMoney(0, -(pMsg->nContriBonus));
    //    if ( !bTickIsEnough)
    //    {
    //        // ���ʽ��У��
    //        g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_NOT_ENOUGH_GOLD);
    //        return;
    //    }
    //}

    if(emCupType_Clan == pMsg->byCreateCupType)
    {
        pMsg->bClanLimit = true;
    }
    else
    {
        pMsg->bClanLimit = false;
    }
    DWORD dwCreateClanID = pActorService->getProperty_Integer(PROPERTY_CLAN);
    bool bIsFreeCreate = false;
    if (pMsg->bClanLimit)
    {
        if (dwCreateClanID <= 0)
        {
            g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_HAVE_NOT_CLAN_CANT_CREATE_CLANLIMITE);
            return;
        }

        if (strcmp( pMsg->szPassword,"") != 0)
        {
            g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_CLAN_CUP_MUST_NO_PASSWORD);
            return;
        }

        if (!clanHelper.m_ptr->checkCreateClanLegendCup(dwCreateClanID,pMsg->nCreateConfigID))
        {
            SSchemeLegendCupLimit* pLimitConfig = pSchemeLegendCup->getLimitConfig(pMsg->nCreateConfigID);
            if (pLimitConfig != NULL)
            {
                g_EHelper.showSystemMessage(dwActorID, (EMChatTipID)pLimitConfig->m_nFailedDesID);
            }
            return;
        }

        //ս������
        bool bMemberCount = clanHelper.m_ptr->getNumProp(dwCreateClanID, emClanProp_MemberCount);
        if(bMemberCount <= 0)
        {
            g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_PLAYER_ISNOT_CLAN_SHAIKH);
            return;
        }

        //���˱�������
        int nFreeLegendCupCount = clanHelper.m_ptr->getNumProp(dwCreateClanID, emClanProp_LegendCupCount);
        if (nFreeLegendCupCount > 0)
        {
            bIsFreeCreate = true;
        }
    }

    if (bIsFreeCreate)
    {
        // ����Ѵ���
         int nFreeLegendCupCount = clanHelper.m_ptr->getNumProp(dwCreateClanID, emClanProp_LegendCupCount);
         --nFreeLegendCupCount;
        if (nFreeLegendCupCount <=0 )
        {
            nFreeLegendCupCount = 0;
        }
        clanHelper.m_ptr->setNumProp(dwCreateClanID, emClanProp_LegendCupCount, nFreeLegendCupCount);
    }
    
    // ��Ǯ
    char szLogInfo[256] = { 0 };
	ssprintf_s(szLogInfo, sizeof(szLogInfo), a2utf8("cup:(%s),zz:(%d),ID:(%d)"),
                                                    pMsg->szCupName , pMsg->nContriBonus, pMsg->nCreateConfigID);
    
    if (!pActorService->addMoney(0, -nNeedMoney, OSSRES_DEL_LEGENDCUP_CREATE, szLogInfo, 0, 0))
    {
        return;
    }

    // �ٴ���
    DBREQ_PARAM_CREATE_LEGEND_CUP DBData;
    DBData.dwWorldID        = getThisGameWorldID();
    DBData.llLegendCupID    = createDBLegendCupId(getThisGameWorldID(), gServerGlobal->getServerID());                      // ����ID
    DBData.nCreateConfigID  = pMsg->nCreateConfigID;     // ����������ID
    DBData.byCreateCupType  = pMsg->byCreateCupType;     // �������� ELegendCupType
    DBData.bIsOnlineCup     = pMsg->bIsOnlineCup;        // ���ϱ��� ���±���
    DBData.dwCreataActorID  = dwActorID;                 // ������� ϵͳ��Ϊ 0
    DBData.byMaxKinNumber   = pConfig->m_nMaxKinCount;   // ����������
    DBData.bClanLimit       = pMsg->bClanLimit;          // ֻ���������Ʋ��ܲμ�
    DBData.dwClanID         = dwCreateClanID;
    DBData.nNeedRoundCount  = pConfig->m_byRoundCount;   // ������Ҫ����
    DBData.nLevelLimit      = pMsg->nLevelLimit;         // �ȼ�����
    DBData.dwContriBonus    = pMsg->nContriBonus;        // ���ʽ���
    DBData.dwRegisterNeedTicket = DBData.byCreateCupType == emCupType_Personal ? pMsg->nRegisterNeedTicket : pConfig->m_nRegisterNeedGold; // ���������Ҫ�ĵ��
    DBData.byTicketAllocationMode = ETAM_SystemSet;      // DBData.byCreateCupType == emCupType_Personal ? ETAM_CustomerSet : ETAM_SystemSet;              //  �����������䷽ʽ ETicketAllocateMode
    if (DBData.byTicketAllocationMode == ETAM_CustomerSet)
    {
        // ���Ƹ������õķ������
        memcpy(&DBData.byRankTicketRatio, pMsg->byRankTicketRatio, sizeof(DBData.byRankTicketRatio));
    }

    sstrcpyn( DBData.szCreaterName,pActorService->getName().data(),sizeof(DBData.szCreaterName));
    sstrcpyn( DBData.szCupName,pMsg->szCupName,sizeof(DBData.szCupName));
    sstrcpyn( DBData.szPassword, pMsg->szPassword,sizeof(DBData.szPassword));

    //���㱨����ֹʱ��
    // ��ǰ�ֽ���ʱ��
    time_t dwNowTime = time(NULL);
    time_t dwComfirmRegisterDeadline = dwNowTime + pMsg->nDayInterval*24 * 3600;
    time_t dwRegisterDeadline = dwNowTime + pMsg->nDayInterval*24 * 3600;

    // �Զ����㵽 23��59��0
    tm *pConfirmRegisterDeadline = localtime(&dwComfirmRegisterDeadline);
    pConfirmRegisterDeadline->tm_hour = 23;
    pConfirmRegisterDeadline->tm_min  = 59;
    pConfirmRegisterDeadline->tm_sec  = 0;
    time_t ConfirmRegisterDeadline = mktime(pConfirmRegisterDeadline);

    // �Զ� ����պ���ǰ����Сʱ�������뱨��
    tm *pRegisterDeadLine =  localtime(&dwRegisterDeadline);
    pRegisterDeadLine->tm_hour = 24 - pConfig->m_nRegisterDeadLine;
    pRegisterDeadLine->tm_min  = 0;
    pRegisterDeadLine->tm_sec  = 0;
    time_t RegisterDeadLine = mktime(pRegisterDeadLine);

    DBData.dwRegisterDeadline = (DWORD)RegisterDeadLine ;                          // ������ֹʱ��
    DBData.dwConfirmRegisterDeadline = (DWORD)ConfirmRegisterDeadline;             // ȷ�����ձ�����ֹʱ��

    if (pDBEngine->executeSP(GAMEDB_REQUEST_CREATE_LEGEND_CUP, dwActorID, (LPCSTR)&DBData, sizeof(DBData), static_cast<IDBRetSink*>(this)) == false)
    {
        ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_CREATE_LEGEND_CUP failed! cmd="<<GAMEDB_REQUEST_CREATE_LEGEND_CUP);
    }


    IOSSLogService *pOSSLogService = gServerGlobal->getOSSLogService();
    if (NULL != pOSSLogService )
    {
        char szText[512] = {0};
        sprintf_s(szText, sizeof(szText), a2utf8("���:[%d] [%s] ������������ID = [%d] �Ƿ���� = [%d] "), dwActorID, pActorService->getName(), pMsg->byCreateCupType, bIsFreeCreate);
        pOSSLogService->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_LEGENDCUP, DBData.llLegendCupID, szText);
    }
}


void LegendCupScenService::onMsgReqJoinLegendcupRoom(ClientID clientID, void* pData, size_t stLen)
{
    if(pData==NULL|| stLen < sizeof(SMsgJoinLegendCupRoom))
    {
        return;
    }
    obuf oData;
    SMsgJoinLegendCupRoom* pMsg = (SMsgJoinLegendCupRoom*)pData;
    ActorHelper helper(clientID, ACTOR_CLIENT);
    IActorService *pActor = helper.m_ptr;
    if (pActor == NULL)
    {
        ErrorLn(__FUNCTION__": pActor==NULL, clientID="<<clientID);
        return;
    }
    pMsg->dwKinID       = pActor->getProperty_Integer(PROPERTY_KIN);
    pMsg->idActor       = pActor->getProperty_Integer(PROPERTY_ID);
    pMsg->dwSrcScene    = pActor->getSceneID();
    pMsg->dwSrcWorldID  = pActor->getFromGameWorldID();
    pMsg->bySex         = pActor->getProperty_Integer(PROPERTY_SEX);
    //�������踽������
	Simple_Atom_Lock lock(&m_mutex);
    pActor->getPlayerRoomExData(&(pMsg->sExData));
    pMsg->sExData.clientID = clientID;

    sstrcpyn(pMsg->szName, pActor->getProperty_String(PROPERTY_NAME).c_str(), sizeof(pMsg->szName));

    // �ٻ�ʦ����
    for (int i = 0; i < SUMMONER_SKILL_COUNT; ++i )
    {
        pMsg->nSummonerSkill[i] = pActor->getSummonerSkillInfo(i);
    }

    ICenterConnectorService * pConnector = gServerGlobal->getCenterConnectorService();
    if(pConnector == NULL)
    {
        ErrorLn( __FUNCTION__ ": CenterConnectorService==NULL");
        return;
    }

    oData.push_back(pMsg,sizeof(SMsgJoinLegendCupRoom));
    pConnector->sendToSocial(MSG_MODULEID_LEGENDCUP, SO_CUP_REQ_JOIN_LEGENDCUP_ROOM, oData.data(), oData.size());
}


void LegendCupScenService::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
    switch (nCmdID)
    {
        // �����ķ��ύ����ս�Ӳ���	-- ���ݿⷵ�ش���
    case GAMEDB_REQUEST_CREATE_LEGEND_CUP:
        {
            onReturnCreateLegendCup(nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
        }
        break;
    default:
        break;
    }
}

void LegendCupScenService::onServerListUpdated()
{

}

void LegendCupScenService::onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData)
{

}

LONGLONG LegendCupScenService::createDBLegendCupId(int nWorld, int nServerID )
{
    // ���뼶��
	ulong dwSynTick = getTickCountEx();
	LONGLONG nHeadId = (nWorld*1000 + nServerID);
	nHeadId<<=32;
	return ( nHeadId | dwSynTick);
}

void LegendCupScenService::onReturnCreateLegendCup(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen)
{
    DWORD	dwActorID	= (DWORD)nQueueIndex;

    ICenterConnectorService * pConnector = gServerGlobal->getCenterConnectorService();
    if(pConnector == NULL)
    {
        ErrorLn( __FUNCTION__ ": CenterConnectorService==NULL");
        return;
    }

    // ��֤���ݿⷵ�ؽ��
    if (nDBRetCode < DBRET_CODE_SUCCEED)
    {
        if (nDBRetCode == -1 && pszDBRetDesc != NULL && pszDBRetDesc[0] != '\0')
        {
            ErrorLn(pszDBRetDesc);
        }
        return;
    }

    int nStructLen = sizeof(DBREQ_RESULT_CREATE_LEGEND_CUP);
    if (nOutLen != nStructLen)
    {
        ErrorLn("[onReturnCreateLegendCup] error outlen��" << nOutLen << " length��" << nStructLen);
        return;
    }
    DBREQ_RESULT_CREATE_LEGEND_CUP* pRetData = (DBREQ_RESULT_CREATE_LEGEND_CUP*)pOutData;

    // ֪ͨ������������
    obuf oData;
    oData.push_back(pRetData,sizeof(DBREQ_RESULT_CREATE_LEGEND_CUP));

    pConnector->sendToSocial(MSG_MODULEID_LEGENDCUP, SO_CUP_CREATE_CUP_RESLUT, oData.data(), oData.size());
}