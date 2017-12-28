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

    // 自己需要处理消息
    pMessageDispatch->unregisterTransmitHandler(MSG_MODULEID_LEGENDCUP);

    // 取消注册响应客户端消息
    pMessageDispatch->unregisterMessageHandler( MSG_MODULEID_LEGENDCUP );

    // 取消注册DB回调
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
    // 自己需要处理消息
    pMessageDispatch->registerTransmitHandler(MSG_MODULEID_LEGENDCUP, this);

    // 处理客户端发送过来的消息
    pMessageDispatch->registerMessageHandler(MSG_MODULEID_LEGENDCUP, this);

    // 注册DB回调
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
    case CS_REQUEST_CREATE_CUP_TYPE:         // 请求创建杯赛类型
        {
            onMsgCreateCupType(client, data, len);
        }
        break;
    case CS_REQUEST_CREATE_CUP:         // 请求创建杯赛
        {
            onMsgCreateCup(client, data, len);
        }
        break;
    case CS_REQ_JOIN_LEGENDCUP_ROOM:         // 请求进入杯赛房间
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

    //处理下杯赛
    if(emCupType_Clan == pMsg->byCreateCupType)
    {
        pMsg->bClanLimit = true;
    }
    else
    {
        pMsg->bClanLimit = false;
    }

    // 发送到社会服校验是否有创建其他个人比赛
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
    // 检查创建的类型
    SMsgCreateCupType * pMsg = (SMsgCreateCupType *)(pData);

    //判定创建的类型
    bool isFreeCreate = false;
    DWORD dwActorID = pActorService->getProperty_Integer(PROPERTY_ID);
    switch(pMsg->byCreateType)
    {
    case emCupType_System :  // 系统比赛
        {
            WarningLn(__FUNCTION__": can't create system legendcup!");
            return;
        }
        break;
    case emCupType_Personal :  // 个人比赛
        {
            // 检查个人可以创建杯赛免费的次数
            isFreeCreate = false;
        }
        break;
    case emCupType_Clan :  // 联盟比赛
        {
            // 检查联盟可以创建杯赛免费的次数
            ClanHelper clanHelper;
            if(! clanHelper.m_ptr)
            {
                WarningLn(__FUNCTION__": can't clanHelper == NULL!");
                return;
            }
            DWORD dwClanID = helper.m_ptr->getProperty_Integer(PROPERTY_CLAN);
            //联盟拥有者
            bool bClanOwner = (clanHelper.m_ptr->getNumProp(dwClanID, emClanProp_ShaikhID) == dwActorID);
            if(!bClanOwner)
            {
                g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_PLAYER_ISNOT_CLAN_SHAIKH);
                return;
            }

            //联盟活跃度
            int nCupActivityLimit = 0;
            int nClanActivity = clanHelper.m_ptr->getNumProp(dwClanID, emClanProp_Activity);
            if(nClanActivity < nCupActivityLimit)
            {
                // 创建收费联盟比赛
                char szBuf[10] = {0};
                sprintf_s(szBuf, sizeof(szBuf), "%d", nCupActivityLimit);
                g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_CLAN_ACTIVITY_LESS_CONFIG,szBuf);
            }

            //联盟比赛次数
            int nFreeLegendCupCount = clanHelper.m_ptr->getNumProp(dwClanID, emClanProp_LegendCupCount);
            if(nFreeLegendCupCount > 0)
            {
                // 创建收费联盟比赛
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

    // 下发到创建杯赛类型信息到客户端
	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint= MSG_ENDPOINT_CLIENT;
	head.byKeyModule   = MSG_MODULEID_LEGENDCUP;
	head.byKeyAction   = SC_CUP_CREATE_CUP_TYPE; // 创建杯赛类型相关      

	// 设置下发数据
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

        // 扣除
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

    // 报名金币校验
    bool bTickIsEnough = pActorService->canAddMoney(0, -(nRegisterNeedGold));
    if ( !bTickIsEnough)
    {
        // 报名金币校验
        g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_NOT_ENOUGH_REGISTER_GOLD);
        return;
    }


    // 扣除申请杯赛金币
    pActorService->addMoney(0, -nRegisterNeedGold, OSSRES_DEL_LEGENDCUP_REQ_JOINCUP, 0, 0, 0);// 申请加入杯赛

    // 发送到社会服加入杯赛
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
        // 出资奖金小于0
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
    

    // 检查金币  创建杯赛金币（免费的为0） + 出资金币
    int nNeedMoney = pConfig->m_nNeedMoneyGold + pMsg->nContriBonus;
    if (nNeedMoney < 0)
    {
        ErrorLn(__FUNCTION__":create legendCup money is not right!!  check the config LegendCup.csv!");
        return;
    }

    if (nNeedMoney > 0)
    {
        // 金币校验
        bool bGoldIsEnough = pActorService->canAddMoney(0, -nNeedMoney);
        if ( !bGoldIsEnough)
        {
            // 提示金币不够
            g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_NOT_ENOUGH_MONEY);
            return;
        }
    }

    // todo 这里策划要求金币还是点卷 目前没有定
    //if (pMsg->nContriBonus < 0)
    //{
    //    g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_CANT_LESS_THAN_ZERO);
    //    return;
    //}

    //if (pMsg->nContriBonus > 0)
    //{
    //    // 出资点卷校验
    //    bool bTickIsEnough = pActorService->canAddMoney(0, -(pMsg->nContriBonus));
    //    if ( !bTickIsEnough)
    //    {
    //        // 出资金币校验
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

        //战队数量
        bool bMemberCount = clanHelper.m_ptr->getNumProp(dwCreateClanID, emClanProp_MemberCount);
        if(bMemberCount <= 0)
        {
            g_EHelper.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_PLAYER_ISNOT_CLAN_SHAIKH);
            return;
        }

        //联盟比赛次数
        int nFreeLegendCupCount = clanHelper.m_ptr->getNumProp(dwCreateClanID, emClanProp_LegendCupCount);
        if (nFreeLegendCupCount > 0)
        {
            bIsFreeCreate = true;
        }
    }

    if (bIsFreeCreate)
    {
        // 扣免费次数
         int nFreeLegendCupCount = clanHelper.m_ptr->getNumProp(dwCreateClanID, emClanProp_LegendCupCount);
         --nFreeLegendCupCount;
        if (nFreeLegendCupCount <=0 )
        {
            nFreeLegendCupCount = 0;
        }
        clanHelper.m_ptr->setNumProp(dwCreateClanID, emClanProp_LegendCupCount, nFreeLegendCupCount);
    }
    
    // 扣钱
    char szLogInfo[256] = { 0 };
	ssprintf_s(szLogInfo, sizeof(szLogInfo), a2utf8("cup:(%s),zz:(%d),ID:(%d)"),
                                                    pMsg->szCupName , pMsg->nContriBonus, pMsg->nCreateConfigID);
    
    if (!pActorService->addMoney(0, -nNeedMoney, OSSRES_DEL_LEGENDCUP_CREATE, szLogInfo, 0, 0))
    {
        return;
    }

    // 再创建
    DBREQ_PARAM_CREATE_LEGEND_CUP DBData;
    DBData.dwWorldID        = getThisGameWorldID();
    DBData.llLegendCupID    = createDBLegendCupId(getThisGameWorldID(), gServerGlobal->getServerID());                      // 杯赛ID
    DBData.nCreateConfigID  = pMsg->nCreateConfigID;     // 创建的配置ID
    DBData.byCreateCupType  = pMsg->byCreateCupType;     // 杯赛类型 ELegendCupType
    DBData.bIsOnlineCup     = pMsg->bIsOnlineCup;        // 线上比赛 线下比赛
    DBData.dwCreataActorID  = dwActorID;                 // 创建玩家 系统的为 0
    DBData.byMaxKinNumber   = pConfig->m_nMaxKinCount;   // 最大参赛队伍
    DBData.bClanLimit       = pMsg->bClanLimit;          // 只有联盟限制才能参加
    DBData.dwClanID         = dwCreateClanID;
    DBData.nNeedRoundCount  = pConfig->m_byRoundCount;   // 杯赛需要打几轮
    DBData.nLevelLimit      = pMsg->nLevelLimit;         // 等级限制
    DBData.dwContriBonus    = pMsg->nContriBonus;        // 出资奖励
    DBData.dwRegisterNeedTicket = DBData.byCreateCupType == emCupType_Personal ? pMsg->nRegisterNeedTicket : pConfig->m_nRegisterNeedGold; // 申请参赛需要的点卷
    DBData.byTicketAllocationMode = ETAM_SystemSet;      // DBData.byCreateCupType == emCupType_Personal ? ETAM_CustomerSet : ETAM_SystemSet;              //  杯赛奖励分配方式 ETicketAllocateMode
    if (DBData.byTicketAllocationMode == ETAM_CustomerSet)
    {
        // 复制个人设置的分配比例
        memcpy(&DBData.byRankTicketRatio, pMsg->byRankTicketRatio, sizeof(DBData.byRankTicketRatio));
    }

    sstrcpyn( DBData.szCreaterName,pActorService->getName().data(),sizeof(DBData.szCreaterName));
    sstrcpyn( DBData.szCupName,pMsg->szCupName,sizeof(DBData.szCupName));
    sstrcpyn( DBData.szPassword, pMsg->szPassword,sizeof(DBData.szPassword));

    //计算报名截止时间
    // 当前轮结束时间
    time_t dwNowTime = time(NULL);
    time_t dwComfirmRegisterDeadline = dwNowTime + pMsg->nDayInterval*24 * 3600;
    time_t dwRegisterDeadline = dwNowTime + pMsg->nDayInterval*24 * 3600;

    // 自动补足到 23：59：0
    tm *pConfirmRegisterDeadline = localtime(&dwComfirmRegisterDeadline);
    pConfirmRegisterDeadline->tm_hour = 23;
    pConfirmRegisterDeadline->tm_min  = 59;
    pConfirmRegisterDeadline->tm_sec  = 0;
    time_t ConfirmRegisterDeadline = mktime(pConfirmRegisterDeadline);

    // 自动 计算刚好提前多少小时不能申请报名
    tm *pRegisterDeadLine =  localtime(&dwRegisterDeadline);
    pRegisterDeadLine->tm_hour = 24 - pConfig->m_nRegisterDeadLine;
    pRegisterDeadLine->tm_min  = 0;
    pRegisterDeadLine->tm_sec  = 0;
    time_t RegisterDeadLine = mktime(pRegisterDeadLine);

    DBData.dwRegisterDeadline = (DWORD)RegisterDeadLine ;                          // 报名截止时间
    DBData.dwConfirmRegisterDeadline = (DWORD)ConfirmRegisterDeadline;             // 确定最终报名截止时间

    if (pDBEngine->executeSP(GAMEDB_REQUEST_CREATE_LEGEND_CUP, dwActorID, (LPCSTR)&DBData, sizeof(DBData), static_cast<IDBRetSink*>(this)) == false)
    {
        ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_CREATE_LEGEND_CUP failed! cmd="<<GAMEDB_REQUEST_CREATE_LEGEND_CUP);
    }


    IOSSLogService *pOSSLogService = gServerGlobal->getOSSLogService();
    if (NULL != pOSSLogService )
    {
        char szText[512] = {0};
        sprintf_s(szText, sizeof(szText), a2utf8("玩家:[%d] [%s] 创建杯赛类型ID = [%d] 是否免费 = [%d] "), dwActorID, pActorService->getName(), pMsg->byCreateCupType, bIsFreeCreate);
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
    //房间所需附加数据
	Simple_Atom_Lock lock(&m_mutex);
    pActor->getPlayerRoomExData(&(pMsg->sExData));
    pMsg->sExData.clientID = clientID;

    sstrcpyn(pMsg->szName, pActor->getProperty_String(PROPERTY_NAME).c_str(), sizeof(pMsg->szName));

    // 召唤师技能
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
        // 往中心服提交创建战队操作	-- 数据库返回处理
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
    // 毫秒级的
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

    // 验证数据库返回结果
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
        ErrorLn("[onReturnCreateLegendCup] error outlen：" << nOutLen << " length：" << nStructLen);
        return;
    }
    DBREQ_RESULT_CREATE_LEGEND_CUP* pRetData = (DBREQ_RESULT_CREATE_LEGEND_CUP*)pOutData;

    // 通知社会服创建杯赛
    obuf oData;
    oData.push_back(pRetData,sizeof(DBREQ_RESULT_CREATE_LEGEND_CUP));

    pConnector->sendToSocial(MSG_MODULEID_LEGENDCUP, SO_CUP_CREATE_CUP_RESLUT, oData.data(), oData.size());
}