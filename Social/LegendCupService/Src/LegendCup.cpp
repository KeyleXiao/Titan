//==========================================================================
/**
* @file	  : CLegendCup.cpp
* @author : 秦其勇
* created : 2010-4-29
* purpose : 杯赛相关
*/
//==========================================================================
#include "stdafx.h"
#include "LegendCup.h"
#include "SharePropertyDef.h"
#include "IShareServer.h"
#include "IGatewayAcceptorService.h"
#include "KnockoutCompetitionRound.h"
#include "GroupCompetitionRound.h"
#include "ExternalFacade.h"
#include "KinHelper.h"
#include "ClanHelper.h"
#include <time.h>
#include "OssLogDef.h"
#include "IOSSLogService.h"
#include "Mail_ManagedDef.h"
#include "ChatHelper.h"

using namespace Mail_ManagerDef;

CLegendCup::CLegendCup()
    : m_pLegendCupService(NULL)
{	
    memset(&m_LegendCupBaseInfo, 0,sizeof(m_LegendCupBaseInfo));
    m_mapRegisterKin.clear();                   // 报名列表       详细信息 从战队读取
    m_mapRequstBlack.clear();                   // 该比赛黑名单   详细信息 从战队读取

    m_KinInfoListDetail.clear();                // 正式参赛战队详细信息
    m_mapCompetitionRound.clear();

    IDBEngineService* pBEngineService = gSocialGlobal->getDBEngineService();
    if (pBEngineService != NULL)
    {
        pBEngineService->registerHandler(this);
    }
}

CLegendCup::~CLegendCup()
{
}

bool CLegendCup::Create(ILegendCupService* pService, SLegendCupBaseInfo* createInfo)
{
    if (pService == NULL)
    {
        ErrorLn(__FUNCTION__": pService == NULL");
        return false;
    }
    // 初始化数据
    m_pLegendCupService = pService;

    m_eDBReadFlag = emLegendRead_AllData;
    
    // 创建杯赛的基础信息
    memcpy(&m_LegendCupBaseInfo, createInfo,sizeof(m_LegendCupBaseInfo));

    //根据配置创建比赛流程类
    if (!createCompetitionRound())
    {
        return false;
    }

    // 切换状态保存数据库
    if (!initalFromCreate())
    {
        return false;
    }
	return true;
}

void CLegendCup::Release()
{
    IDBEngineService* pBEngineService = gSocialGlobal->getDBEngineService();
    if (pBEngineService != NULL)
    {
        pBEngineService->unregisterHandler(this);
    }

    IDateTriggerService * pDateTriggerService = gSocialGlobal->getDateTriggerService();
    if (pDateTriggerService != NULL)
    {
        pDateTriggerService->UnRegisterDateTrigger(ETIMER_END_REGISTER_TRIGGER, this);
        pDateTriggerService->UnRegisterDateTrigger(ETIMER_CONFIRM_REGISTER_TRIGGER, this);
        pDateTriggerService->UnRegisterDateTrigger(ETIMER_CURRENT_ROUND_BEGIN_TRIGGER, this);
        pDateTriggerService->UnRegisterDateTrigger(ETIMER_CURRENT_ROUND_END_TRIGGER, this);
    }

    map<int,ICompetitionRoundBase *>::iterator iterRound =  m_mapCompetitionRound.begin();
    for (;iterRound != m_mapCompetitionRound.end(); ++iterRound)
    {
        iterRound->second->Release();
        iterRound->second = NULL;
    }
    m_mapRegisterKin.clear();                   // 报名列表       详细信息 从战队读取
    m_mapRequstBlack.clear();                   // 该比赛黑名单   详细信息 从战队读取
    m_KinInfoListDetail.clear();                // 正式参赛战队详细信息
    m_mapCompetitionRound.clear();

	delete this;
}

bool CLegendCup::writeCupBaseDataToDB()
{
    IDBEngineService* pDBEngine = (IDBEngineService*)gSocialGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
        return false;
    }
    DBREQ_REQUEST_UPDATE_LEGEND_CUP_BASEINFO DBInfo;
    DBInfo.dwWorldID    = getThisGameWorldID();
    DBInfo.llLegendCupID = m_LegendCupBaseInfo.llLegendCupID;              // 杯赛ID
    DBInfo.bHaveRewards = m_LegendCupBaseInfo.bHaveRewards;                 // 杯赛奖励标示
    DBInfo.eCupState = m_LegendCupBaseInfo.eCupState;                      // 状态 ECupState
    DBInfo.nBeginRound = m_LegendCupBaseInfo.nBeginRound;                   // 第几轮开始
    DBInfo.nCurrentRound = m_LegendCupBaseInfo.nCurrentRound;               // 当前打到第几轮
    DBInfo.bIsEnd = m_LegendCupBaseInfo.bIsEnd;                             // 是否结束
    DBInfo.dwEndTime = m_LegendCupBaseInfo.dwEndTime;                       // 结束日期
    DBInfo.dwTotalRegisterTicket = m_LegendCupBaseInfo.dwTotalRegisterTicket; //报名累积的点卷

    if (pDBEngine->executeSP(GAMEDB_REQUEST_UPDATE_LEGEND_CUP_BASEINFO, 0, LPCSTR(&DBInfo), sizeof(DBInfo), static_cast<IDBRetSink *>(this)) == false)
    {
        ErrorLn(__FUNCTION__": pDBEngine->ExecuteSP()  cmdID = "<<GAMEDB_REQUEST_UPDATE_LEGEND_CUP_BASEINFO<<" failed");
        return false;
    }

	return true;
}


bool CLegendCup::competitionStart()
{

    // 定位到第几轮
    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeCenter");
        return false;
    }
    ISchemeLegendCup *pLegendCupScheme = pSchemeCenter->getSchemeLegendCup();
    SSchemeLegendCup* pSchemeConfig = pLegendCupScheme->getConfig(m_LegendCupBaseInfo.nCreateConfigID);
    if (pSchemeConfig == NULL)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeConfig nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID);
        return false;
        
    }
    int nRoundID = pLegendCupScheme->getRoundIDByRegisterCount(pSchemeConfig->m_nDetailID, m_mapRegisterKin.size());
    if (nRoundID == 0)
    {
        ErrorLn(__FUNCTION__": failed! can't find roundID nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID<<" m_dwDetailID ="<< pSchemeConfig->m_nDetailID<<" kinCount = "<<m_mapRegisterKin.size());
        return false;
    }

    // 初始轮数
    m_LegendCupBaseInfo.nBeginRound = nRoundID;
    //定位到第几轮
    m_LegendCupBaseInfo.nCurrentRound = nRoundID;

    // 计算每轮的开始结束时间
    if (m_LegendCupBaseInfo.bIsOnlineCup)
    {
        if (!calculateRoundTimeInfo(nRoundID))
        {
            return false;
        }
    }

    //初始化第一轮参赛列表
    map<int,ICompetitionRoundBase *>::iterator iter = m_mapCompetitionRound.find(m_LegendCupBaseInfo.nCurrentRound);
    if (iter == m_mapCompetitionRound.end())
    {
        ErrorLn(__FUNCTION__": failed! can't find CompetitionRoundBase nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID<<" m_dwDetailID ="<< pSchemeConfig->m_nDetailID<<" kinCount = "<<m_mapRegisterKin.size());
        return false;
    }

   
    vector<DWORD> veckinList;
    for (map<DWORD, SCupRegisterInfo>::iterator iterKin = m_mapRegisterKin.begin();iterKin != m_mapRegisterKin.end(); ++iterKin)
    {
        veckinList.push_back(iterKin->first);
    }
    
    list<DWORD> randomKinList;
    while (veckinList.size() > 0)
    {
        int nIndex = (mt_range_rand(1,time(NULL))) % veckinList.size();
        vector<DWORD>::iterator iterFind = veckinList.begin() + nIndex;
        DWORD dwFindKinID = *iterFind;
        if (dwFindKinID > 0)
        {
            randomKinList.push_back(dwFindKinID);
        }
        veckinList.erase(iterFind);
    }

    if (!iter->second->setKinListAndGroup(randomKinList,true, true))
    {
        return false;
    }

    // 根据分组创建比赛节点比赛创建房间
    if (!iter->second->startRound())
    {
        ErrorLn(__FUNCTION__": start error!! legendID ="<<m_LegendCupBaseInfo.llLegendCupID<<" roundID ="<<m_LegendCupBaseInfo.nCurrentRound);
        return false;
    }

    if (m_LegendCupBaseInfo.bIsOnlineCup)
    {
        // 线上启动本轮比赛结束开始定时器
        resetTriggerRoundBeginAndEnd();
    }

    // todo 广播给所有参赛成员 比赛已经发布 可以查看分组信息

    return true;
}


bool CLegendCup::calculateRoundTimeInfo(int nFirstRoundID)
{
    // 推算每轮比赛的时间
    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeCenter nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID);
        return false;
    }

    ISchemeLegendCup *pLegendCupScheme = pSchemeCenter->getSchemeLegendCup();
    SSchemeLegendCup* pSchemeConfig = pLegendCupScheme->getConfig(m_LegendCupBaseInfo.nCreateConfigID);
    if (pSchemeConfig == NULL)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeConfig nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID);
        return false;

    }

    // 推算出每一轮开始时间结束时间
    DWORD preEndTime = m_LegendCupBaseInfo.dwConfirmRegisterDeadline;

    for (int nRoundID = nFirstRoundID; nRoundID <= m_LegendCupBaseInfo.nNeedRoundCount; ++nRoundID)
    {
        SSchemeLegendCupDetail* pDetailScheme= pLegendCupScheme->getDetailInfo(pSchemeConfig->m_nDetailID, nRoundID);
        if (pDetailScheme == NULL)
        {
            ErrorLn(__FUNCTION__": failed! can't find roundID nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID<<" m_dwDetailID ="<< pSchemeConfig->m_nDetailID<<" kinCount = "<<m_mapRegisterKin.size());
            return false;
        }

        // 计算下轮比赛的开始结束时间
        // 当前轮结束时间
        SRoundTimeInfo sTimeInfo;
        sTimeInfo.nRoundID = nRoundID;

        time_t tStartTime = preEndTime + pDetailScheme->m_nStartDayInterval * 24 * 3600; 
        tm tmStart;
        localtime_s(&tmStart, &tStartTime);
        tmStart.tm_hour = pDetailScheme->m_nCreateRoomTime[0];
        tmStart.tm_min = pDetailScheme->m_nCreateRoomTime[1];
        tmStart.tm_sec = pDetailScheme->m_nCreateRoomTime[2];

        sTimeInfo.dwRoundCreateRoomTime = (DWORD)mktime(&tmStart);

        DWORD dwStartInterval =0;
        // 时
        dwStartInterval += (pDetailScheme->m_nStartTime[0] - pDetailScheme->m_nCreateRoomTime[0])*60*60;
        // 分
        dwStartInterval += (pDetailScheme->m_nStartTime[1] - pDetailScheme->m_nCreateRoomTime[1])*60;
        // 秒
        dwStartInterval += (pDetailScheme->m_nStartTime[2] - pDetailScheme->m_nCreateRoomTime[2]);
        sTimeInfo.dwRoomBeginInterval = dwStartInterval;

        // 当前轮结束时间
        time_t tEndTime = preEndTime + pDetailScheme->m_nEndDayInterval* 24 * 3600;  
        tm tmEnd;
        localtime_s(&tmEnd, &tEndTime);  
        tmEnd.tm_hour = pDetailScheme->m_nEndTime[0];
        tmEnd.tm_min = pDetailScheme->m_nEndTime[1];
        tmEnd.tm_sec = pDetailScheme->m_nEndTime[2];
        sTimeInfo.dwRoundEndTime =  (DWORD)mktime(&tmEnd);

        m_mapRoundBeginTime[nRoundID] = sTimeInfo;
        preEndTime = sTimeInfo.dwRoundEndTime;
    }
    
    // 保存到数据库
    IDBEngineService* pDBEngine = (IDBEngineService*)gSocialGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
        return false;
    }

    obuf ob;
    DBREQ_REQUEST_SAVE_CUP_ROUND_TIMEINFO saveInfo;
    saveInfo.dwWorldID = getThisGameWorldID();
    saveInfo.llLegendCupID = m_LegendCupBaseInfo.llLegendCupID;
    saveInfo.nCount = m_mapRoundBeginTime.size();
    int nBufSize = saveInfo.nCount * sizeof(DBREQ_PARAM_ROUND_TIMEINFO);
    ob<<saveInfo<<nBufSize;
    map<int, SRoundTimeInfo>::iterator iter = m_mapRoundBeginTime.begin();
    for (;iter != m_mapRoundBeginTime.end(); ++iter)
    {
        ob<< iter->second;
    }

    if (pDBEngine->executeSP(GAMEDB_REQUEST_SAVE_CUP_ROUND_TIMEINFO, 0, ob.data(), ob.size(), static_cast<IDBRetSink *>(this)) == false)
    {
        ErrorLn(__FUNCTION__": pDBEngine->ExecuteSP()  cmdID = "<<GAMEDB_REQUEST_SAVE_CUP_ROUND_TIMEINFO<<" failed");
        return false;
    }
    return true;
}

void CLegendCup::onTriggerEndRegister()
{
    changeLegendCupState(SATE_CONFIRMENTRIES);
}

void CLegendCup::onTriggerConfirmRegister()
{
    // 定时器到自动发布的比赛处理
    if (!bIsCanPublic(INVALID_PDBID))
    {
        // 取消杯赛流程
        cancelLegendCup();
        return;
    }

    // 发布比赛
    publishCompetition();
}


void CLegendCup::cancelLegendCup()
{
    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeCenter nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID);
        return;
    }

    ISchemeLegendCup *pLegendCupScheme = pSchemeCenter->getSchemeLegendCup();
    SSchemeLegendCup* pSchemeConfig = pLegendCupScheme->getConfig(m_LegendCupBaseInfo.nCreateConfigID);
    if (pSchemeConfig == NULL)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeConfig nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID);
        return;
    }

    ISchemeMailDataCenter *pSchemeMailDataCenter = pSchemeCenter->getSchemeMailDataCenter();
	if (NULL == pSchemeMailDataCenter)
	{
		ErrorLn(__FUNCTION__" nullptr == pSchemeMailDataCenter");
		return;
	}

    MailHelper mailHelper;
	IMailService *pMailService = mailHelper.m_ptr;
	if (NULL == pMailService)
	{
		ErrorLn(__FUNCTION__": failed! pMailService == NULL");
		return;
	}

    if (m_LegendCupBaseInfo.eCupState > SATE_CONFIRMENTRIES)
    {
        return;
    }

    EmphasisLn(__FUNCTION__": cancelLegendCup() CupName ="<<m_LegendCupBaseInfo.szCupName);
    // 清退出资 创建 报名费用
    if (pSchemeConfig->m_nNeedMoneyGold > 0)
    {
        // 创建费用
        SMailConfig* pMailConfig = pSchemeMailDataCenter->getConfig(emMailFill_LegendCupCancelCreateRet);
        if( NULL != pMailConfig)
        {
            SMailSendData mailInfo;
            mailInfo.nType = emMailType_System;							// 邮件类型
            mailInfo.nSourceType = emMailSourceType_System;				// 资源来源类型
            sstrcpyn(mailInfo.szSenderName, pMailConfig->szSenderName, sizeof(mailInfo.szSenderName));// 发件者姓名
            mailInfo.dwMasterPDBID = m_LegendCupBaseInfo.dwCreataActorID;
            sstrcpyn(mailInfo.szTitle, pMailConfig->szTitle, sizeof(mailInfo.szTitle));
            mailInfo.nPlusMoney = pSchemeConfig->m_nNeedMoneyGold;

            char  szParam[MAIL_CONTENT_MAXSIZE];					// 内容
            ssprintf_s(szParam, sizeof(szParam), a2utf8("%s;%d"),m_LegendCupBaseInfo.szCupName, mailInfo.nPlusMoney);

            if (mailInfo.nPlusMoney > 0)
            {
                pMailService->sendMail(mailInfo,emMailFill_LegendCupCancelCreateRet, szParam);
            }
        }
        
    }

    if (m_LegendCupBaseInfo.dwContriBonus > 0)
    {
        // 出资费用
        SMailConfig* pMailConfig = pSchemeMailDataCenter->getConfig(emMailFill_LegendCupCancelContriBonusRet);
        if( NULL != pMailConfig)
        {
            SMailSendData mailInfo;
            mailInfo.nType = emMailType_System;							// 邮件类型
            mailInfo.nSourceType = emMailSourceType_System;				// 资源来源类型
            sstrcpyn(mailInfo.szSenderName, pMailConfig->szSenderName, sizeof(mailInfo.szSenderName));// 发件者姓名
            mailInfo.dwMasterPDBID = m_LegendCupBaseInfo.dwCreataActorID;
            sstrcpyn(mailInfo.szTitle, pMailConfig->szTitle, sizeof(mailInfo.szTitle));
            mailInfo.nPlusMoney = m_LegendCupBaseInfo.dwContriBonus;

            char  szParam[MAIL_CONTENT_MAXSIZE];					// 内容
            ssprintf_s(szParam, sizeof(szParam), a2utf8("%s;%d"),m_LegendCupBaseInfo.szCupName, mailInfo.nPlusMoney);

            if (mailInfo.nPlusMoney > 0)
            {
                pMailService->sendMail(mailInfo, emMailFill_LegendCupCancelContriBonusRet, szParam);
            }
        }
    }

    if (m_LegendCupBaseInfo.dwRegisterNeedTicket > 0)
    {
        SMailConfig* pMailConfig = pSchemeMailDataCenter->getConfig(emMailFill_LegendCupCancelRegisterRet);
        if( NULL != pMailConfig)
        {
            for (map<DWORD, SCupRegisterInfo>::iterator iterRG = m_mapRegisterKin.begin();  iterRG!= m_mapRegisterKin.end(); ++ iterRG)
            {
                // 清退邮件
                if (iterRG->second.dwRegisterPDBID == NULL)
                {
                    continue;
                }
                SMailSendData mailInfo;
                mailInfo.nType = emMailType_System;							// 邮件类型
                mailInfo.nSourceType = emMailSourceType_System;				// 资源来源类型
                sstrcpyn(mailInfo.szSenderName, pMailConfig->szSenderName, sizeof(mailInfo.szSenderName));// 发件者姓名
                mailInfo.dwMasterPDBID = iterRG->second.dwRegisterPDBID;;
                sstrcpyn(mailInfo.szTitle, pMailConfig->szTitle, sizeof(mailInfo.szTitle));
                mailInfo.nPlusMoney = m_LegendCupBaseInfo.dwRegisterNeedTicket;

                char  szParam[MAIL_CONTENT_MAXSIZE];					// 内容
                ssprintf_s(szParam, sizeof(szParam), a2utf8("%s;%d"),m_LegendCupBaseInfo.szCupName, mailInfo.nPlusMoney);

                if (mailInfo.nPlusMoney > 0)
                {
                    pMailService->sendMail(mailInfo, emMailFill_LegendCupCancelRegisterRet, szParam);
                }
            }
        }
    }

    changeLegendCupState(SATE_SYSTEMCANCEL);

    // 移除杯赛记录
    if (m_pLegendCupService != NULL)
    {
        m_pLegendCupService->removeServiceCupInfo(m_LegendCupBaseInfo.llLegendCupID);
    }

	// 释放
	Release();
}


void CLegendCup::onTriggerBeginRound()
{
    map<int,ICompetitionRoundBase *>::iterator iter = m_mapCompetitionRound.find(m_LegendCupBaseInfo.nCurrentRound);
    if (iter == m_mapCompetitionRound.end())
    {

        ErrorLn(__FUNCTION__": can't find the competitionround roundID ="<<m_LegendCupBaseInfo.nCurrentRound);
        return;
    }
    // todo 通知开始了第几轮
}

void CLegendCup::onTriggerEndRound()
{
    map<int,ICompetitionRoundBase *>::iterator iter = m_mapCompetitionRound.find(m_LegendCupBaseInfo.nCurrentRound);
    if (iter == m_mapCompetitionRound.end())
    {

        ErrorLn(__FUNCTION__": can't find the competitionround roundID ="<<m_LegendCupBaseInfo.nCurrentRound);
        return;
    }
    iter->second->endRound();
}


LegendCup_ManagedDef::ECupState CLegendCup::getLegendCupState()
{
    return (ECupState)m_LegendCupBaseInfo.eCupState;
}

bool CLegendCup::createFromDB(ILegendCupService *pService, LPCSTR pszContext, int nLen)
{
    if (pService == NULL)
    {
        ErrorLn(__FUNCTION__": pKinService == NULL");
        return false;
    }
    if (nLen != sizeof(DBREQ_RESULT_LEGEND_CUP_BASE_INFO))
    {
        return false;
    }

    m_pLegendCupService = pService;

    m_eDBReadFlag = emLegendRead_NoIni;
     //设置数据
    DBREQ_RESULT_LEGEND_CUP_BASE_INFO* pDBContext = (DBREQ_RESULT_LEGEND_CUP_BASE_INFO*)pszContext;
    // 赋值基础数据
    memcpy(&m_LegendCupBaseInfo, pDBContext, sizeof(m_LegendCupBaseInfo));

    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeCenter nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID);
        return false;
    }
    ISchemeLegendCup *pLegendCupScheme = pSchemeCenter->getSchemeLegendCup();
    SSchemeLegendCup* pSchemeConfig = pLegendCupScheme->getConfig(m_LegendCupBaseInfo.nCreateConfigID);
    if (pSchemeConfig == NULL)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeConfig nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID);
        return false;

    }

    for (int n = 1; n <= m_LegendCupBaseInfo.nNeedRoundCount; ++n)
    {
        map<int,ICompetitionRoundBase *>::iterator iterFind = m_mapCompetitionRound.find(n);
        if(iterFind !=  m_mapCompetitionRound.end())
        {
            ErrorLn(__FUNCTION__": have create round DetailID ="<<pSchemeConfig->m_nDetailID<<" nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID<<" nRound ="<<n);
            return false;
        }

        SSchemeLegendCupDetail* pConfigInfo = pLegendCupScheme->getDetailInfo(pSchemeConfig->m_nDetailID, n);
        if (pConfigInfo == NULL)
        {
            ErrorLn(__FUNCTION__": failed! pConfigInfo == NULL DetailID ="<<pSchemeConfig->m_nDetailID<<" nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID<<" nRound ="<<n);
            return false;
        }

        ICompetitionRoundBase *pCompetitionRound = NULL;
        switch(pConfigInfo->m_nCompetitionType)
        {
        case emType_GroupCompetition:    // 小组赛类型
            {
                pCompetitionRound = new CGroupCompetitionRound();
            }
            break;
        case emType_KnockoutCompetition: // 淘汰赛类型
            {
                pCompetitionRound = new CKnockoutCompetitionRound();
            }
            break;
        }

        if (pCompetitionRound == NULL)
        {
            ErrorLn(__FUNCTION__": new pCompetitionRound == NULL DetailID ="<<pSchemeConfig->m_nDetailID<<" nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID<<" nRound ="<<n);
            return false;
        }
        SCreateRoundBaseInfo sCreateInfo;
        sCreateInfo.llLegendCupID = m_LegendCupBaseInfo.llLegendCupID; 
        sCreateInfo.nRoundID = n; 
        sCreateInfo.nMaxGroup = pConfigInfo->m_nMaxGroup;
        sCreateInfo.nGroupMaxKin = pConfigInfo->m_nGroupMaxKin;
        sCreateInfo.nGroupPromotionCount = pConfigInfo->m_nPromotionKinCount;
        sCreateInfo.nDetailConfigID = pConfigInfo->m_nDetailID;
        sCreateInfo.byRoundCompetitionType = pConfigInfo->m_nCompetitionType;
        sCreateInfo.byBoType = pConfigInfo->m_nBoType;

        pCompetitionRound->CreateFromDB(this, sCreateInfo);
        m_mapCompetitionRound[n] = pCompetitionRound;
    }

    // 切换初始化状态
    if (SATE_NONE == m_LegendCupBaseInfo.eCupState)
    {
        changeLegendCupState(SATE_REGISTER);
    }

    // 已读标志
    setLegendCupDBReadFlag(emLegendRead_BaseData);
   
    return true;
}

bool CLegendCup::setCupDBMemberContext(LPCSTR pszContext, int nLen, obuf *pNodebuf, int& nMemberCount)
{
    if (nLen < sizeof(DBREQ_RESULT_READLEGENDCUP_MEMBERLIST))
    {
        return false;
    }

    DBREQ_RESULT_READLEGENDCUP_MEMBERLIST* pRetData = (DBREQ_RESULT_READLEGENDCUP_MEMBERLIST*)pszContext;
    int nTempLen = sizeof(DBREQ_RESULT_READLEGENDCUP_MEMBERLIST) + pRetData->nMemberCount * sizeof(DBREQ_PARAM_LEGENDCUP_MEMBER);
    if (nLen != nTempLen)
    {
        return false;
    }

    // 设置比赛成员数据
    int offset = sizeof(DBREQ_RESULT_READLEGENDCUP_MEMBERLIST);
    for (int i = 0; i < pRetData->nMemberCount; i ++)
    {
        DBREQ_PARAM_LEGENDCUP_MEMBER* pMember = (DBREQ_PARAM_LEGENDCUP_MEMBER*)(pszContext + offset);
        m_CupMemberInfo.insert(pair<DWORD, SLegendCupMemberInfo>( pMember->dwActorID, *pMember));

		if (pNodebuf != NULL)
		{
			DBREQ_PARAM_READ_PLAYER_CUP_INFO nodeInfo;
			nodeInfo.dwPDBID = pMember->dwActorID;
			pNodebuf->push_back(&nodeInfo, sizeof(DBREQ_PARAM_READ_PLAYER_CUP_INFO));
			++nMemberCount;
		}

        offset += sizeof(DBREQ_PARAM_LEGENDCUP_MEMBER);
    }

    // 已读标志
    setLegendCupDBReadFlag(emLegendRead_MemberData);

    return true;
}


bool CLegendCup::setCupDBKinDataContext(LPCSTR pszContext, int nLen)
{
    if (nLen < sizeof(DBREQ_RESULT_READLEGENDCUP_KINLIST))
    {
        return false;
    }

    DBREQ_RESULT_READLEGENDCUP_KINLIST* pRetData = (DBREQ_RESULT_READLEGENDCUP_KINLIST*)pszContext;
    int nTempLen = sizeof(DBREQ_RESULT_READLEGENDCUP_KINLIST) + pRetData->nKinCount * sizeof(DBREQ_REQUEST_UPDATE_CUP_KININFO);
    if (nLen != nTempLen)
    {
        return false;
    }

    // 战队信息数据
    int offset = sizeof(DBREQ_RESULT_READLEGENDCUP_KINLIST);
    for (int i = 0; i < pRetData->nKinCount; i ++)
    {
        DBREQ_REQUEST_UPDATE_CUP_KININFO* pMember = (DBREQ_REQUEST_UPDATE_CUP_KININFO*)(pszContext + offset);
        m_KinInfoListDetail.insert(pair<DWORD, SCupKinInfo>( pMember->dwKinID, *pMember));
        offset += sizeof(DBREQ_REQUEST_UPDATE_CUP_KININFO);
    }

    // 已读标志
    setLegendCupDBReadFlag(emLegendRead_KinData);
    return true;
}


bool CLegendCup::setCupDBCompetitionContext(LPCSTR pszContext, int nLen)
{
    // 从DB中设置比赛的数据
    if (nLen < sizeof(DBREQ_REQUEST_ALL_CUP_COMPETITION_NODE))
    {
        return false;
    }

    DBREQ_REQUEST_ALL_CUP_COMPETITION_NODE* pRetData = (DBREQ_REQUEST_ALL_CUP_COMPETITION_NODE*)pszContext;
    int nTempLen = sizeof(DBREQ_REQUEST_ALL_CUP_COMPETITION_NODE) + pRetData->nCount * sizeof(DBREQ_PARAM_COMPETITION_NODE);
    if (nLen != nTempLen)
    {
        return false;
    }

    // 设置比赛成员数据
    int offset = sizeof(DBREQ_REQUEST_ALL_CUP_COMPETITION_NODE);
    for (int i = 0; i < pRetData->nCount; i ++)
    {
        DBREQ_PARAM_COMPETITION_NODE* pMember = (DBREQ_PARAM_COMPETITION_NODE*)(pszContext + offset);
        // 根据roundID 设置节点ID
        map<int,ICompetitionRoundBase *>::iterator iterRound = m_mapCompetitionRound.find(pMember->nRoundID);
        if (iterRound == m_mapCompetitionRound.end())
        {
            offset += sizeof(DBREQ_PARAM_COMPETITION_NODE);
            ErrorLn(__FUNCTION__": error please check readDB info!!! nRoundID ="<< pMember->nRoundID<<" llLegendCupID ="<<m_LegendCupBaseInfo.llLegendCupID);
            continue;
        }
        iterRound->second->addCompetitionNodeFromDB(pMember);
        offset += sizeof(DBREQ_PARAM_COMPETITION_NODE);
    }

    // 已读标志
    setLegendCupDBReadFlag(emLegendRead_CompetitionData);
    return true;
}


bool CLegendCup::setCupDBRoundKinPos(LPCSTR pszContext, int nLen)
{
    if (nLen < sizeof(DBREQ_RESULT_ROUND_GROUP_KIN_POS))
    {
        return false;
    }

    DBREQ_RESULT_ROUND_GROUP_KIN_POS* pRetData = (DBREQ_RESULT_ROUND_GROUP_KIN_POS*)pszContext;
    int nTempLen = sizeof(DBREQ_RESULT_ROUND_GROUP_KIN_POS) + pRetData->nCount * sizeof(DBREQ_PARAM_ROUND_GROUP_KIN_POS);
    if (nLen != nTempLen)
    {
        return false;
    }

    // 设置比赛节点数据
    int offset = sizeof(DBREQ_REQUEST_ALL_CUP_COMPETITION_NODE);
    for (int i = 0; i < pRetData->nCount; i ++)
    {
        DBREQ_PARAM_ROUND_GROUP_KIN_POS* pMember = (DBREQ_PARAM_ROUND_GROUP_KIN_POS*)(pszContext + offset);
        // 根据roundID 设置节点ID
        map<int,ICompetitionRoundBase *>::iterator iterRound = m_mapCompetitionRound.find(pMember->nRoundID);
        if (iterRound == m_mapCompetitionRound.end())
        {
            ErrorLn(__FUNCTION__": error please check readDB info!!! nRoundID ="<< pMember->nRoundID<<" llLegendCupID ="<<m_LegendCupBaseInfo.llLegendCupID);
            continue;
        }
        iterRound->second->setKinListFromDB(pMember);

        offset += sizeof(DBREQ_PARAM_ROUND_GROUP_KIN_POS);
    }

    // 每轮战队列表
    setLegendCupDBReadFlag(emLegendRead_RoundInKinList);

    return true;
}

bool CLegendCup::setCupDBRoundTimeInfo(LPCSTR pszContext, int nLen)
{
    if (nLen < sizeof(DBREQ_RESULT_READ_CUP_ROUND_TIMEINFO))
    {
        return false;
    }

    DBREQ_RESULT_READ_CUP_ROUND_TIMEINFO* pRetData = (DBREQ_RESULT_READ_CUP_ROUND_TIMEINFO*)pszContext;
    int nTempLen = sizeof(DBREQ_RESULT_READ_CUP_ROUND_TIMEINFO) + pRetData->nCount * sizeof(DBREQ_PARAM_ROUND_TIMEINFO);
    if (nLen != nTempLen)
    {
        return false;
    }

    // 设置比赛节点数据
    int offset = sizeof(DBREQ_RESULT_READ_CUP_ROUND_TIMEINFO);
    for (int i = 0; i < pRetData->nCount; i ++)
    {
        DBREQ_PARAM_ROUND_TIMEINFO* pTimeInfo = (DBREQ_PARAM_ROUND_TIMEINFO*)(pszContext + offset);
        // 根据roundID 设置节点ID
        m_mapRoundBeginTime[pTimeInfo->nRoundID] = *pTimeInfo;    // 每轮比赛开始时间记录

        offset += sizeof(DBREQ_PARAM_ROUND_TIMEINFO);
    }

    // 每轮战队列表
    setLegendCupDBReadFlag(emLegendRead_RoundTimeInfo);

    return true;
}


DWORD CLegendCup::getRegisterLeftTime()
{
    if (m_LegendCupBaseInfo.eCupState != SATE_REGISTER)
    {
        return 0;
    }

    DWORD dwNowTime = (DWORD)time(NULL);
    if (m_LegendCupBaseInfo.dwRegisterDeadline < dwNowTime )
    {
        return 0;
    }
    DWORD dwLeftTime = m_LegendCupBaseInfo.dwRegisterDeadline - dwNowTime;
    return dwLeftTime;
}

DWORD CLegendCup::getConfirmRegisterLeftTime()
{
    if (m_LegendCupBaseInfo.eCupState != SATE_CONFIRMENTRIES)
    {
        return 0;
    }

    DWORD dwNowTime = (DWORD)time(NULL);
    if (m_LegendCupBaseInfo.dwConfirmRegisterDeadline < dwNowTime )
    {
        return 0;
    }
    DWORD dwLeftTime = m_LegendCupBaseInfo.dwConfirmRegisterDeadline - dwNowTime;
    return dwLeftTime;
}


void CLegendCup::setLegendCupDBReadFlag(ELegendDBReadFlag eReadFlag)
{
    m_eDBReadFlag |= eReadFlag;
    if (m_eDBReadFlag == emLegendRead_AllData && !m_LegendCupBaseInfo.bIsEnd)
    {
        // 继续比赛
        continueLegendCup();
    }
}

int CLegendCup::getLegendCupDBReadFlag()
{
    return m_eDBReadFlag;
}

bool CLegendCup::initalFromCreate()
{
    // 改变初始化状态
    changeLegendCupState(SATE_REGISTER);
    return true;
}

void CLegendCup::changeLegendCupState(ECupState eState)
{
    m_LegendCupBaseInfo.eCupState = eState;
    // 切换初始化状态
    switch(m_LegendCupBaseInfo.eCupState)
    {
    case SATE_SYSTEMCANCEL:
        {
            // 记录取消状态
            char szText[512] = {0};
            ssprintf_s(szText, sizeof(szText), a2utf8("---------------------------[%s] 杯赛被系统取消---------------------------"),m_LegendCupBaseInfo.szCupName);
            IOSSLogService * pOSSLogService = gSocialGlobal->getOSSLogService();
            if ( pOSSLogService != NULL )
            {
                pOSSLogService->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_LEGENDCUP, m_LegendCupBaseInfo.llLegendCupID, szText);
            }

        }
        break;
    case SATE_REGISTER:  //  报名
        {
            // 创建报名触发器
            time_t tRegisterDeadline = m_LegendCupBaseInfo.dwRegisterDeadline;
            tm *pRegisterDeadLine =  localtime(&tRegisterDeadline);

            SDateTriggerInfo sTriggerInfo;
            sTriggerInfo.year = pRegisterDeadLine->tm_year + 1900;
            sTriggerInfo.month = pRegisterDeadLine->tm_mon + 1;
            sTriggerInfo.day = pRegisterDeadLine->tm_mday;
            sTriggerInfo.hour = pRegisterDeadLine->tm_hour;
            sTriggerInfo.min = pRegisterDeadLine->tm_min;
            sTriggerInfo.wday = -1;
            if (getRegisterLeftTime() > 0)
            {
                IDateTriggerService * pDateTriggerService = gSocialGlobal->getDateTriggerService();
                if (pDateTriggerService == NULL)
                {
                    ErrorLn(__FUNCTION__": pDateTriggerService == NULL cupName=["<<m_LegendCupBaseInfo.szCupName<<"] legendCupID="<<m_LegendCupBaseInfo.llLegendCupID);
                    return;
                }
                pDateTriggerService->UnRegisterDateTrigger(ETIMER_END_REGISTER_TRIGGER, this);
                pDateTriggerService->RegisterDateTrigger(ETIMER_END_REGISTER_TRIGGER, this, sTriggerInfo);

            }
            else
            {
                // 直接触发结束报名
                onTriggerEndRegister();
            }
        }
        break;
    case SATE_CONFIRMENTRIES://  确定报名列表
        {
            // 创建确定最终报名列表触发器
            time_t tConfirmRegisterDeadline = m_LegendCupBaseInfo.dwConfirmRegisterDeadline;
            tm *pConfirmRegisterDeadline =  localtime(&tConfirmRegisterDeadline);

            SDateTriggerInfo sTriggerInfo;
            sTriggerInfo.year = pConfirmRegisterDeadline->tm_year + 1900;
            sTriggerInfo.month = pConfirmRegisterDeadline->tm_mon + 1;
            sTriggerInfo.day = pConfirmRegisterDeadline->tm_mday;
            sTriggerInfo.hour = pConfirmRegisterDeadline->tm_hour;
            sTriggerInfo.min = pConfirmRegisterDeadline->tm_min;
            sTriggerInfo.wday = -1;
            if (getConfirmRegisterLeftTime() > 0)
            {
                IDateTriggerService * pDateTriggerService = gSocialGlobal->getDateTriggerService();
                if (pDateTriggerService == NULL)
                {
                    ErrorLn(__FUNCTION__": pDateTriggerService == NULL cupName=["<<m_LegendCupBaseInfo.szCupName<<"] legendCupID="<<m_LegendCupBaseInfo.llLegendCupID);
                    return;
                }
                pDateTriggerService->UnRegisterDateTrigger(ETIMER_CONFIRM_REGISTER_TRIGGER, this);
                pDateTriggerService->RegisterDateTrigger(ETIMER_CONFIRM_REGISTER_TRIGGER, this, sTriggerInfo);

            }
            else
            {
                // 确定比赛成员信息
                onTriggerConfirmRegister();
            }
        }
        break;
    case SATE_GROUPING://  定位比赛进行流程
        {
            // 确定比赛参赛之后 确定分组
            IDateTriggerService * pDateTriggerService = gSocialGlobal->getDateTriggerService();
            if (pDateTriggerService != NULL)
            {
                pDateTriggerService->UnRegisterDateTrigger(ETIMER_CONFIRM_REGISTER_TRIGGER, this);
            }

            if (!competitionStart())
            {
                ErrorLn(__FUNCTION__": grouping error! plesase check!!");
                return;
            }
        }
        break;
    case SATE_32TO16://  32进16 总共分8组
    case SATE_16TO8://  16进8   总共分8组
    case SATE_8TO4://  8进4     总共分4组
    case SATE_4TO2://  4进2     总共分2组
    case SATE_FINAL://  决赛    总共分2组
        {
            
        }
        break;
    default:
        break;
    }

    // 保存到数据库
    writeCupBaseDataToDB();
}


SCupKinInfo* CLegendCup::getCupKinInfo(DWORD dwKinID)
{
    map<DWORD, SCupKinInfo>::iterator iter = m_KinInfoListDetail.find(dwKinID);    // 正式参赛战队详细信息
    if (iter == m_KinInfoListDetail.end())
    {
        return  NULL;
    }
    
    return &(iter->second);
}

void CLegendCup::toNextRound(int nEndRoundID, list<DWORD> dwPromotionList)
{
    // 进入下一轮比赛
    m_LegendCupBaseInfo.nCurrentRound = nEndRoundID + 1;
    map<int,ICompetitionRoundBase *>::iterator iter = m_mapCompetitionRound.find(m_LegendCupBaseInfo.nCurrentRound);
    if (iter == m_mapCompetitionRound.end())
    {
        ErrorLn(__FUNCTION__": can't find Round RoundID ="<<m_LegendCupBaseInfo.nCurrentRound<<" nConfigID="<<m_LegendCupBaseInfo.nCreateConfigID);
        return;
    }

    if (!iter->second->setKinListAndGroup(dwPromotionList))
    {
        ErrorLn(__FUNCTION__": setKinListAndGroup error !");
        return;
    }

    //根据分组创建比赛节点比赛创建房间
    if (!iter->second->startRound())
    {
        ErrorLn(__FUNCTION__": start error!! legendID ="<<m_LegendCupBaseInfo.llLegendCupID<<" roundID ="<<m_LegendCupBaseInfo.nCurrentRound);
        return;
    }

    if (m_LegendCupBaseInfo.bIsOnlineCup)
    {
        // 线上启动本轮开始结束定时器 定时触发
        resetTriggerRoundBeginAndEnd();
    }

    // 保存到数据库
    writeCupBaseDataToDB();

    // 下发给所有观察者本轮信息
    sendCurRoundInfo2Viewer();
    
}

bool CLegendCup::createCompetitionRound()
{
    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeCenter nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID);
        return false;
    }
    ISchemeLegendCup *pLegendCupScheme = pSchemeCenter->getSchemeLegendCup();
    SSchemeLegendCup* pSchemeConfig = pLegendCupScheme->getConfig(m_LegendCupBaseInfo.nCreateConfigID);
    if (pSchemeConfig == NULL)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeConfig nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID);
        return false;

    }

    for (int n = 1; n <= m_LegendCupBaseInfo.nNeedRoundCount; ++n)
    {
        map<int,ICompetitionRoundBase *>::iterator iterFind = m_mapCompetitionRound.find(n);
        if(iterFind !=  m_mapCompetitionRound.end())
        {
            ErrorLn(__FUNCTION__": have create round DetailID ="<<pSchemeConfig->m_nDetailID<<" nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID<<" nRound ="<<n);
            continue;
        }

        SSchemeLegendCupDetail* pConfigInfo = pLegendCupScheme->getDetailInfo(pSchemeConfig->m_nDetailID, n);
        if (pConfigInfo == NULL)
        {
            ErrorLn(__FUNCTION__": failed! pConfigInfo == NULL DetailID ="<<pSchemeConfig->m_nDetailID<<" nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID<<" nRound ="<<n);
            continue;
        }

        ICompetitionRoundBase *pCompetitionRound = NULL;
        switch(pConfigInfo->m_nCompetitionType)
        {
        case emType_GroupCompetition:    // 小组赛类型
            {
                pCompetitionRound = new CGroupCompetitionRound();
            }
            break;
        case emType_KnockoutCompetition: // 淘汰赛类型
            {
                pCompetitionRound = new CKnockoutCompetitionRound();
            }
            break;
        }

        if (pCompetitionRound == NULL)
        {
            ErrorLn(__FUNCTION__": new pCompetitionRound == NULL DetailID ="<<pSchemeConfig->m_nDetailID<<" nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID<<" nRound ="<<n);
            continue;
        }
        SCreateRoundBaseInfo sCreateInfo;
        sCreateInfo.llLegendCupID = m_LegendCupBaseInfo.llLegendCupID; 
        sCreateInfo.nRoundID = n; 
        sCreateInfo.nMaxGroup = pConfigInfo->m_nMaxGroup;
        sCreateInfo.nGroupMaxKin = pConfigInfo->m_nGroupMaxKin;
        sCreateInfo.nGroupPromotionCount = pConfigInfo->m_nPromotionKinCount;
        sCreateInfo.nDetailConfigID = pConfigInfo->m_nDetailID;
        sCreateInfo.byRoundCompetitionType = pConfigInfo->m_nCompetitionType;
        sCreateInfo.byBoType = pConfigInfo->m_nBoType;
        
        if (!pCompetitionRound->Create(this, sCreateInfo))
        {
            ErrorLn(__FUNCTION__": new pCompetitionRound == NULL DetailID ="<<pSchemeConfig->m_nDetailID<<" nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID<<" nRound ="<<n);
            continue;
        }
        m_mapCompetitionRound[n] = pCompetitionRound;
    }
    return true;
}

void CLegendCup::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
    // 验证数据库返回结果
    if (nDBRetCode < DBRET_CODE_SUCCEED)
    {
        ErrorLn(__FUNCTION__":-- nCmdID = "<<nCmdID<< " nQueueIndex ="<< nQueueIndex << " pszDBRetDesc：" << pszDBRetDesc);
        return;
    }

    return;
}

void CLegendCup::getRegisterKinList(map<DWORD,SCupRegisterInfo> &RegisterKinList)
{
    RegisterKinList = m_mapRegisterKin;
}

SLegendCupBaseInfo* CLegendCup::getLegendCupBaseInfo()
{
    return &m_LegendCupBaseInfo;
}

LONGLONG CLegendCup::getLegendCupID()
{
    return m_LegendCupBaseInfo.llLegendCupID;
}

bool CLegendCup::isEnd()
{
    return m_LegendCupBaseInfo.bIsEnd;
}


bool CLegendCup::registerCheck(SMsgJoinCup* pMsgInfo)
{
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver == NULL)
    {
        ErrorLn(__FUNCTION__": pShareReceiver == NULL");
        return false;
    }

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(pMsgInfo->dwActorID);
    if (shareProperty.dwPDBID == INVALID_PDBID)
    {
        ErrorLn(__FUNCTION__": can't find person!");
        return false;
    }

    // 是否帮会限定
    if ( m_LegendCupBaseInfo.bClanLimit)
    {
        if(m_LegendCupBaseInfo.dwClanID > 0)
        {
            if (m_LegendCupBaseInfo.dwClanID != shareProperty.dwClanID)
            {
                g_ExternalFacade.showSystemMessage(pMsgInfo->dwActorID, CHAT_TIP_LEGENDCUP_CANT_REGISTER_CLANLIMITE,"");
                return false;
            }
        }
    }

    KinHelper kinHelper;
    if (kinHelper.m_ptr == NULL)
    {
        ErrorLn(__FUNCTION__": kinHelper.m_ptr == NULL ")
            return false;
    }

    // 队长判定
    SKinInfo sKinInfo = kinHelper.m_ptr->getKinInfo(pMsgInfo->dwKinID);
    if (sKinInfo.nKinProp_ShaikhID != pMsgInfo->dwActorID)
    {
        g_ExternalFacade.showSystemMessage(pMsgInfo->dwActorID, CHAT_TIP_LEGENDCUP_NOT_SHAIKH_CANT_REQUEST,"");
        return false;
    }

    // 等级限定
    if (sKinInfo.nKinProp_Level < m_LegendCupBaseInfo.nLevelLimit)
    {
        g_ExternalFacade.showSystemMessage(pMsgInfo->dwActorID, CHAT_TIP_LEGENDCUP_KIN_LEVEL_MINI,"");
        return false;
    }

    // 是否满员
    if (m_mapRegisterKin.size() >= m_LegendCupBaseInfo.byMaxKinNumber)
    {
        g_ExternalFacade.showSystemMessage(pMsgInfo->dwActorID, CHAT_TIP_LEGENDCUP_REGISTER_FULL,"");
        return false;
    }

    //黑名单中
    map<DWORD, bool>::iterator iterBK = m_mapRequstBlack.find(pMsgInfo->dwKinID);
    if (iterBK != m_mapRequstBlack.end())
    {
        g_ExternalFacade.showSystemMessage(pMsgInfo->dwActorID, CHAT_TIP_LEGENDCUP_IN_BLACK_LIST,"");
        return false;
    }

    // 杯赛状态判定
    if (m_LegendCupBaseInfo.eCupState != SATE_REGISTER)
    {
        // 已经结束了申请，无法申请
        g_ExternalFacade.showSystemMessage(pMsgInfo->dwActorID, CHAT_TIP_LEGENDCUP_CANT_REQUEST_REGISTER,"");
        return false;
    }

    // 密码检测
    if (havePassword())
    {
        if(strcmpi(pMsgInfo->szPassword, m_LegendCupBaseInfo.szPassword) != 0)
        {
            g_ExternalFacade.showSystemMessage(pMsgInfo->dwActorID, CHAT_TIP_LEGENDCUP_PASSWORD_NOT_RIGHT,"");
            return false;
        }
    }

    map<DWORD, SCupRegisterInfo>::iterator iterRG = m_mapRegisterKin.find(pMsgInfo->dwKinID);
    if (iterRG != m_mapRegisterKin.end())
    {
        // 无法重复申请
        g_ExternalFacade.showSystemMessage(pMsgInfo->dwActorID, CHAT_TIP_LEGENDCUP_REPEAT_REQUEST_REGISTER,"");
        return false;
    }

    return true;
}


bool CLegendCup::addRegister(SMsgJoinCup* pMsgInfo)
{
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver == NULL)
    {
        ErrorLn(__FUNCTION__": pShareReceiver == NULL");
        return false;
    }

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(pMsgInfo->dwActorID);
    if (shareProperty.dwPDBID == INVALID_PDBID)
    {
        ErrorLn(__FUNCTION__": can't find person!");
        return false;
    }

    SCupRegisterInfo rgInfo;
    rgInfo.dwRegisterPDBID = pMsgInfo->dwActorID;
    rgInfo.nRankID = 0;
    // 更新申请列表到数据库
    m_mapRegisterKin.insert(pair<DWORD,SCupRegisterInfo>(pMsgInfo->dwKinID, rgInfo));
    saveRegisterToDB();

    // 更新报名累积奖金到数据库
    m_LegendCupBaseInfo.dwTotalRegisterTicket += m_LegendCupBaseInfo.dwRegisterNeedTicket;
    writeCupBaseDataToDB();

    // 通知客户端加入成功
    obuf32 ob;
    SNetMsgHead NetMsgHead;
    NetMsgHead.bySrcEndPoint	= MSG_ENDPOINT_SOCIAL;
    NetMsgHead.byDestEndPoint	= MSG_ENDPOINT_CLIENT;
    NetMsgHead.byKeyModule	    = MSG_MODULEID_LEGENDCUP;
    NetMsgHead.byKeyAction	    = OC_CUP_SEND_REGISTER_SUCCESS;
    ob<<NetMsgHead;
    send2Client(shareProperty.clientID, ob.data(), ob.size());

    // 添加申请成功 更新杯赛信息
    g_ExternalFacade.showSystemMessage(pMsgInfo->dwActorID, CHAT_TIP_LEGENDCUP_REGISTER_SUCCESS,"");

    // 发送单独比赛给观察者
    updateSingleCupBaseInfoAllView();

    sendRegisterList2AllViewer();

    // 记录日志
    IOSSLogService * pOSSLogServer = gSocialGlobal->getOSSLogService();
    if (pOSSLogServer != NULL )
    {
        KinHelper kinHelper;
        if (kinHelper.m_ptr == NULL)
        {
            ErrorLn(__FUNCTION__": kinHelper.m_ptr == NULL ")
            return false;
        }

        SKinInfo sKinInfo = kinHelper.m_ptr->getKinInfo(pMsgInfo->dwKinID);
        char szText[512] = {0};
        ssprintf_s(szText, sizeof(szText), a2utf8("报名：战队[%u] 战队名[%s] 队长id[%u] 队长名[%s] 报名参与 比赛（比赛id[%u] 比赛名称[%s]）"), 
            sKinInfo.nKinProp_ID, sKinInfo.szName, shareProperty.dwPDBID, shareProperty.szName,m_LegendCupBaseInfo.llLegendCupID, m_LegendCupBaseInfo.szCupName );

        pOSSLogServer->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_LEGENDCUP, m_LegendCupBaseInfo.llLegendCupID, szText);
    }

    return true;
}

bool CLegendCup::addToBlackList(DWORD dwActorID, DWORD dwKinID)
{
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver == NULL)
    {
        ErrorLn(__FUNCTION__": pShareReceiver == NULL");
        return false;
    }

    // 如果不在申请和确定申请状态下无法移除黑名单
    if(m_mapRequstBlack.size() >= DBDEF_LEGENDCUP_BALCKLIST_LEN) 
    {
        // 黑名单满了
        g_ExternalFacade.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_BLACKLIST_ISFULL,"");
        return false;
    }

    if(! (m_LegendCupBaseInfo.eCupState == SATE_REGISTER || m_LegendCupBaseInfo.eCupState == SATE_CONFIRMENTRIES)) 
    {
        // 无法移动到黑名单
        g_ExternalFacade.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_CANT_REMOVE_TO_BLACKLIST,"");
        return false;
    }
    map<DWORD, SCupRegisterInfo>::iterator iterRegInfo = m_mapRegisterKin.find(dwKinID);
    if (iterRegInfo == m_mapRegisterKin.end())
    {
        return false;   
    }

    DWORD dwRegisterPDBID = iterRegInfo->second.dwRegisterPDBID;
    m_mapRegisterKin.erase(iterRegInfo);
    saveRegisterToDB();

    m_mapRequstBlack.insert(pair<DWORD,bool>(dwKinID, true));
    saveBlackListToDB();

    // 把队伍移动到黑名单需要回退报名资金 减少累积的报名资金
    sendBackRegisterNeedTicket(dwKinID,dwRegisterPDBID);

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwActorID);

	// 发送更新基础信息
	updateSingleCupBaseInfoAllView();

    // 发送更新申请列表
    sendRegisterList2AllViewer();

    // 发送更新黑名单列表
    sendBlackList2Client(shareProperty.clientID);

    IOSSLogService * pOSSLogServer = gSocialGlobal->getOSSLogService();
    if (pOSSLogServer != NULL )
    {
        // 获取战队信息
        KinHelper helper;
        IKinService *pKinService = helper.m_ptr;
        if (pKinService != NULL)
        {
            SKinInfo sKinInfo = pKinService->getKinInfo(dwKinID);
            char szText[512] = {0};
            ssprintf_s(szText, sizeof(szText), a2utf8("黑名单日志：举办者[%s]在比赛（比赛id[%u] 比赛名称[%s]）把战队ID[%u][%s]加入黑名单"), 
                m_LegendCupBaseInfo.szCreaterName,m_LegendCupBaseInfo.llLegendCupID, m_LegendCupBaseInfo.szCupName,dwKinID,sKinInfo.szName);

            pOSSLogServer->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_LEGENDCUP, m_LegendCupBaseInfo.llLegendCupID, szText);
        }
    }

    return true;
}


bool CLegendCup::removeBlackList(DWORD dwActorID, DWORD dwKinID)
{
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver == NULL)
    {
        ErrorLn(__FUNCTION__": pShareReceiver == NULL");
        return false;
    }

    // 如果不在申请和确定申请状态下无法移除黑名单
    if(! (m_LegendCupBaseInfo.eCupState == SATE_REGISTER || m_LegendCupBaseInfo.eCupState == SATE_CONFIRMENTRIES)) 
    {
        g_ExternalFacade.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_CANT_REMOVE_TO_BLACKLIST,"");
        return false;
    }

    map<DWORD, bool>::iterator iter = m_mapRequstBlack.find(dwKinID);
    if (iter == m_mapRequstBlack.end())
    {
        return false;   
    }

    m_mapRequstBlack.erase(iter);
    saveBlackListToDB();

    // 发送更新黑名单列表
    SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(dwActorID);
    sendBlackList2Client(shareProperty.clientID);

    return true;
}

bool CLegendCup::exitRegister(DWORD dwActorID, DWORD dwKinID)
{
    // 不在申请状态则无法主动退出
    if(m_LegendCupBaseInfo.eCupState != SATE_REGISTER ) 
    {
        g_ExternalFacade.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_CANT_EXIT_REGISTER,"");
        return false;
    }

    if (m_mapRegisterKin.find(dwKinID) == m_mapRegisterKin.end())
    {
        g_ExternalFacade.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_NOT_REGISTER,"");
        return false;
    }

    m_mapRegisterKin.erase(dwKinID);

    g_ExternalFacade.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_EXIT_SUCCESS,"");
    saveRegisterToDB();

    // 发送更新基础信息
	updateSingleCupBaseInfoAllView();

	// 发送更新申请列表
    sendRegisterList2AllViewer();
    
    return true;
}
void CLegendCup::saveRegisterToDB()
{
    IDBEngineService* pDBEngine = (IDBEngineService*)gSocialGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
        return;
    }

    DBREQ_REQUST_UPDATE_RIGESTERLIST updateInfo;
    updateInfo.dwWorldID    = getThisGameWorldID();
    updateInfo.llLegendCupID = m_LegendCupBaseInfo.llLegendCupID;
    int index = 0;
    for (map<DWORD, SCupRegisterInfo>::iterator iterRe = m_mapRegisterKin.begin();iterRe != m_mapRegisterKin.end(); ++iterRe)
    {
        
        updateInfo.dwRegisterList[index] = iterRe->first;
        updateInfo.dwRegisterPDBID[index] = iterRe->second.dwRegisterPDBID;
        index++;
    }
    if (pDBEngine->executeSP(GAMEDB_REQUEST_UPDATE_RIGESTERLIST, 0, (LPCSTR)(&updateInfo), sizeof(updateInfo), static_cast<IDBRetSink *>(this)) == false)
    {
        ErrorLn(__FUNCTION__": pDBEngine->ExecuteSP()  cmdID = "<<GAMEDB_REQUEST_UPDATE_RIGESTERLIST<<" failed");
        return;
    }
}


void CLegendCup::updatRankToKinRecord(DWORD dwKinID, int nRankID)
{
    map<DWORD, SCupRegisterInfo>::iterator iterFind = m_mapRegisterKin.find(dwKinID);
    if (iterFind != m_mapRegisterKin.end())
    {
        iterFind->second.nRankID = nRankID;
    }
}


void CLegendCup::saveRankListToDB()
{
    IDBEngineService* pDBEngine = (IDBEngineService*)gSocialGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
        return;
    }

    DBREQ_REQUST_UPDATE_RANKLIST updateInfo;
    updateInfo.dwWorldID = getThisGameWorldID();
    updateInfo.llLegendCupID = m_LegendCupBaseInfo.llLegendCupID;
    int index = 0;
    for (map<DWORD, SCupRegisterInfo>::iterator iterRe = m_mapRegisterKin.begin();iterRe != m_mapRegisterKin.end(); ++iterRe)
    {
        updateInfo.nRegisterRankList[index++] = iterRe->second.nRankID;
    }

    if (pDBEngine->executeSP(GAMEDB_REQUEST_UPDATE_RANKLIST, 0, (LPCSTR)(&updateInfo), sizeof(updateInfo), static_cast<IDBRetSink *>(this)) == false)
    {
        ErrorLn(__FUNCTION__": pDBEngine->ExecuteSP()  cmdID = "<<GAMEDB_REQUEST_UPDATE_RANKLIST<<" failed");
        return;
    }
}


void CLegendCup::saveBlackListToDB()
{
    IDBEngineService* pDBEngine = (IDBEngineService*)gSocialGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
        return;
    }

    DBREQ_REQUST_UPDATE_BLACKLIST updateInfo;
    updateInfo.dwWorldID = getThisGameWorldID();
    updateInfo.llLegendCupID = m_LegendCupBaseInfo.llLegendCupID;
    int index = 0;
    for (map<DWORD, bool>::iterator iterB = m_mapRequstBlack.begin();iterB != m_mapRequstBlack.end(); ++iterB)
    {
        updateInfo.dwBlackList[index++] = iterB->first;

    }
    if (pDBEngine->executeSP(GAMEDB_REQUEST_UPDATE_BLACKLIST, 0, (LPCSTR)(&updateInfo), sizeof(updateInfo), static_cast<IDBRetSink *>(this)) == false)
    {
        ErrorLn(__FUNCTION__": pDBEngine->ExecuteSP()  cmdID = "<<GAMEDB_REQUEST_UPDATE_BLACKLIST<<" failed");
        return;
    }
}

bool CLegendCup::saveCupKinData()
{
    // 保存当前参赛战队和战队成员信息
    IDBEngineService* pDBEngine = (IDBEngineService*)gSocialGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
        return false;
    }

    KinHelper helper;
    IKinService *pKinService = helper.m_ptr;
    if (pKinService == NULL)
    {
        ErrorLn(__FUNCTION__": failed pKinService == NULL");
        return false;
    }

    obuf obKinData;
    DBREQ_REQUST_SAVE_ALL_KININFO allKinInfo;
    allKinInfo.nCount = m_mapRegisterKin.size();
    int nBufSize = m_mapRegisterKin.size() * sizeof(SCupKinInfo);
    obKinData<<allKinInfo<<nBufSize;
    for (map<DWORD, SCupRegisterInfo>::iterator iterKin = m_mapRegisterKin.begin();iterKin != m_mapRegisterKin.end(); ++iterKin)
    {
        //初始化战队相关信息设置
        SCupKinInfo data;
        data.dwKinID        = iterKin->first;
        data.llLegendCupID  = m_LegendCupBaseInfo.llLegendCupID;
        data.dwWorldID   = getThisGameWorldID();
        data.nCurKnockOutScore = 0;
        data.nGroupScore = 0;

        // 初始化本地信息
        m_KinInfoListDetail[data.dwKinID] = data;
        obKinData<< data;
    }

    if (pDBEngine->executeSP(GAMEDB_REQUEST_SAVE_ALL_KININFO, 0, obKinData.data(), obKinData.size(), static_cast<IDBRetSink *>(this)) == false)
    {
        ErrorLn(__FUNCTION__": pDBEngine->ExecuteSP()  cmdID = "<<GAMEDB_REQUEST_SAVE_ALL_KININFO<<" failed");
        return false;
    }

    // 成员信息保存到数据库
    obuf obMemberData;
    obuf obMember;
    DBREQ_REQUST_UPDATE_CUP_MEMBER_HEAD dbHead;
    dbHead.dwWorldID = getThisGameWorldID();
    dbHead.llLegendCupID = m_LegendCupBaseInfo.llLegendCupID;
    for (map<DWORD, SCupRegisterInfo>::iterator iterKinMember = m_mapRegisterKin.begin();iterKinMember != m_mapRegisterKin.end(); ++iterKinMember)
    {
        // 获取每个战队成员信息
        string memberLog;
        PDBID pReturnArray[256] = {0};
        DWORD dwCount = pKinService->getKinMemberList(iterKinMember->first, pReturnArray, sizeof(pReturnArray));
        dbHead.nCount += dwCount;
        for (DWORD i = 0; i < dwCount; ++i)
        {
            // 获取成员信息

            SKinMember memberInfo = pKinService->getKinMemberInfo(pReturnArray[i]);

            // 日志信息
            memberLog += memberInfo.szName;
            memberLog+= ",";

            // DB保存赋值
            DBREQ_REQUST_UPDATE_CUP_MEMBERLIST DBMember;
            DBMember.dwActorID  = memberInfo.dwPDBID;
            DBMember.dwKinID    = memberInfo.dwKinID; 
            obMember<<DBMember;

            // 本地成员数据赋值
            SLegendCupMemberInfo data;
            data.dwUserID       = memberInfo.dwUDBID;
            data.dwKinID        = memberInfo.dwKinID; 
            data.dwActorID      = memberInfo.dwPDBID;
            data.nMatchType     = memberInfo.nMatchType;
            data.nRankScore     = memberInfo.nRankScore;
            data.nGrade         = memberInfo.nGrade;
            data.nLevel         = memberInfo.nLevel;
            data.bSex           = memberInfo.bySex;
            sstrcpyn(data.szActorName, memberInfo.szName, sizeof(data.szActorName));
            m_CupMemberInfo[memberInfo.dwPDBID] = data;
        }

        // 输出确定杯赛日志
        IOSSLogService * pOSSLogServer = gSocialGlobal->getOSSLogService();
        if (pOSSLogServer != NULL )
        {
            // 获取战队信息
            SKinInfo sKinInfo = pKinService->getKinInfo(iterKinMember->first);
            char szText[1024] = {0};
            ssprintf_s(szText, sizeof(szText), a2utf8("杯赛确认参赛战队：[%s]比赛确认名单[%s]战队 成员:(%s)"), 
                m_LegendCupBaseInfo.szCupName, sKinInfo.szName, memberLog.c_str());

            pOSSLogServer->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_LEGENDCUP, m_LegendCupBaseInfo.llLegendCupID, szText);
        }
    }
    //压入数据
    obMemberData<<dbHead;
    int nMemberBufSize = obMember.size();
    //变长大小
    obMemberData<<nMemberBufSize;
    obMemberData.push_back(obMember.data(), obMember.size());

    if (pDBEngine->executeSP(GAMEDB_REQUEST_UPDATE_CUP_MEMBERLIST, 0, obMemberData.data(), obMemberData.size(), static_cast<IDBRetSink *>(this)) == false)
    {
        ErrorLn(__FUNCTION__": pDBEngine->ExecuteSP()  cmdID = "<<GAMEDB_REQUEST_UPDATE_CUP_MEMBERLIST<<" failed");
        return false;
    }
    return true;
}

void CLegendCup::continueLegendCup()
{

    // 没有结束则切换状态
    changeLegendCupState((ECupState)m_LegendCupBaseInfo.eCupState);
    // 比赛轮数判定
    if (m_LegendCupBaseInfo.nCurrentRound > 0)
    {
        map<int,ICompetitionRoundBase *>::iterator iter = m_mapCompetitionRound.find(m_LegendCupBaseInfo.nCurrentRound);
        if (iter == m_mapCompetitionRound.end())
        {
            
            ErrorLn(__FUNCTION__": can't find the competitionround roundID ="<<m_LegendCupBaseInfo.nCurrentRound);
            return;
        }
        iter->second->continueRound();

        // 设置本轮开始结束定时器
        if (m_LegendCupBaseInfo.bIsOnlineCup)
        {
            // 线上启动本轮比赛结束开始定时器
            resetTriggerRoundBeginAndEnd();
        }
    }
}


bool CLegendCup::eidtCurRoundEndTime(DWORD dwActorID,DWORD dwChangeTime)
{
    // todo 权限检查是否有权限修改比赛时间权限
    IDBEngineService* pDBEngine = (IDBEngineService*)gSocialGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
        return false;
    }

    if (m_LegendCupBaseInfo.bIsEnd)
    {
        // 比赛已经结束了无法调整时间
        g_ExternalFacade.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_END_CANT_CHANGE_TIEM,"");
        return false;
    }
    
    DWORD dwNowTime = (DWORD)time(NULL);
    if (dwNowTime < dwChangeTime)
    {
        
        // 结束时间小于当前时间无法调整
        g_ExternalFacade.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_TOO_SMALL_TIME,"");
        return false;
    }

    DBREQ_UPDATE_SIGNAL_ROUND_TIMEINFO DBInfo;
    DBInfo.dwWorldID = getThisGameWorldID();
    DBInfo.llLegendCupID;					    // 杯赛ID
    DBInfo.nRoundID;                           // 第几轮
    DBInfo.dwRoundCreateRoomTime;              // 本轮第一次创建房间的时间
    DBInfo.dwRoomBeginInterval;                // 房间创建后开始间隔
    DBInfo.dwRoundEndTime;                     // 本轮结束时间

    if (pDBEngine->executeSP(GAMEDB_REQUEST_UPDATE_CUP_ROUND_TIMEINFO, 0, LPCSTR(&DBInfo), sizeof(DBInfo), static_cast<IDBRetSink *>(this)) == false)
    {
        ErrorLn(__FUNCTION__": pDBEngine->ExecuteSP()  cmdID = "<<GAMEDB_REQUEST_UPDATE_CUP_ROUND_TIMEINFO<<" failed");
        return false;
    }

    // 移除原来的比赛timer 创建新的timer

    //gSocialGlobal->getTimerService()->KillTimer(ETIMER_CURRENT_ROUND_END_TIMER,  this);

    //gSocialGlobal->getTimerService()->SetTimer(ETIMER_CURRENT_ROUND_END_TIMER, getEndRoundLeftTime(), this, 1, __FUNCTION__);

    writeCupBaseDataToDB();
    return true;
}

bool CLegendCup::eidtCompetitionNodeTime(DWORD dwChangeTime)
{
    return true;
}

void CLegendCup::endLegendCup(list<DWORD> finalList)
{
    TraceLn(__FUNCTION__": LegendCup is End ! llLegendCupID ="<<m_LegendCupBaseInfo.llLegendCupID);
    // 本次比赛结束
    m_LegendCupBaseInfo.bIsEnd = true;
    m_LegendCupBaseInfo.dwEndTime = (DWORD)time(NULL);

    recordKinFinalRank(finalList);

    // 发放奖励相关
    if (distributionOfRewards())
    {
        m_LegendCupBaseInfo.bHaveRewards = true;
    }

    writeCupBaseDataToDB();

    // 移除杯赛创建者对应的记录 并没有释放杯赛实体
    if (m_pLegendCupService != NULL)
    {
        m_pLegendCupService->endCupRemoveCreaterRecInfo(m_LegendCupBaseInfo.llLegendCupID);
    }

    updateSingleCupBaseInfoAllView();

    //updateSingleCupNodeInfo2AllView();
}

void CLegendCup::recordKinFinalRank(list<DWORD> finalList)
{
    if (finalList.size() == 0)
    {
        return;
    }
    // 名次相关
    KinHelper kinHelper;
    IKinService *pKinService = kinHelper.m_ptr;
    string sRankLog;
    BYTE byRank = 1;
    for (list<DWORD>::iterator iter = finalList.begin(); iter != finalList.end();++iter )
    {
        if (pKinService != NULL)
        {
            SKinCupAwardNode sAwardNode;
            sAwardNode.dwKinID          = *iter;											// 战队ID
            sAwardNode.llLegengCupID    = m_LegendCupBaseInfo.llLegendCupID;				// 杯赛ID
            sAwardNode.byCupRank        = byRank++;											// 杯赛名次
			sAwardNode.nCupConfigID		= m_LegendCupBaseInfo.nCreateConfigID;				// 杯赛配置
			sAwardNode.dwCupEndTime		= m_LegendCupBaseInfo.dwEndTime;					// 杯赛结束时间
			sstrcpyn(sAwardNode.szCupName, m_LegendCupBaseInfo.szCupName, sizeof(sAwardNode.szCupName));		// 杯赛名称
            pKinService->addKinLegendAward(sAwardNode);

            SKinInfo sKininfo = pKinService->getKinInfo(*iter);
            char szText[64] = {0};
            ssprintf_s(szText, sizeof(szText), a2utf8("第[%d]名 [%s] ") ,byRank, sKininfo.szName);
            sRankLog += szText;
        }
    }

    IOSSLogService * pOSSLogServer = gSocialGlobal->getOSSLogService();
    if (pOSSLogServer != NULL )
    {
        char szText[512] = {0};
        ssprintf_s(szText, sizeof(szText), a2utf8("赛后名次输出：[%s] 最终排名: %s"), m_LegendCupBaseInfo.szCupName ,sRankLog.data());
        pOSSLogServer->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_LEGENDCUP, m_LegendCupBaseInfo.llLegendCupID, szText);
    }
}

void CLegendCup::sendRegisterList2Client(ClientID clientID)
{
    if (clientID == INVALID_CLIENT)
    {
        return;
    }
    // 请求查看参赛战队信息
    KinHelper helper;
    IKinService *pKinService = helper.m_ptr;
    if (pKinService == NULL)
    {
        ErrorLn(__FUNCTION__": failed pKinService == NULL");
        return;
    }

    obuf ob;

    SNetMsgHead NetMsgHead;
    NetMsgHead.bySrcEndPoint	= MSG_ENDPOINT_SOCIAL;
    NetMsgHead.byDestEndPoint	= MSG_ENDPOINT_CLIENT;
    NetMsgHead.byKeyModule		= MSG_MODULEID_LEGENDCUP;
    NetMsgHead.byKeyAction		= OC_CUP_SEND_ALL_KINLIST;
    ob<<NetMsgHead;

    SMsgRegisterHead rgHead;
    rgHead.nCount = m_mapRegisterKin.size();
    rgHead.nMaxCount = m_LegendCupBaseInfo.byMaxKinNumber;
    rgHead.dwRegisterLeftTime = getRegisterLeftTime();
    rgHead.dwConfirmKinLeftTime = getConfirmRegisterLeftTime();
    rgHead.llLegendCupID = m_LegendCupBaseInfo.llLegendCupID;

    ob<<rgHead;
    map<DWORD, SCupRegisterInfo>::iterator iter = m_mapRegisterKin.begin();
    for (;iter != m_mapRegisterKin.end(); ++iter)
    {
        // 获取战队信息
        SKinInfo sKinInfo = pKinService->getKinInfo(iter->first);

        // 组织下发信息
        SMsgRegisterKinInfo  RegisterKin;
        RegisterKin.nKinProp_ID = sKinInfo.nKinProp_ID;
        RegisterKin.nKinProp_Level = sKinInfo.nKinProp_Level;

        int nKinWinNum = pKinService->getKinLegendGlory(iter->first, emLegendGlory_WinCount);
        int nLoseNum = pKinService->getKinLegendGlory(iter->first, emLegendGlory_LoseCount);
        //RegisterKin.nKinWinNum = nKinWinNum + nLoseNum;
		RegisterKin.nKinMaxNum = nKinWinNum + nLoseNum;
		RegisterKin.nKinWinNum = nKinWinNum;
        
        sstrcpyn(RegisterKin.szName, sKinInfo.szName,sizeof(RegisterKin.szName));
        ob<<RegisterKin;
    }

    send2Client(clientID, ob.data(), ob.size());

}

void CLegendCup::sendRegisterList2AllViewer()
{
    
    // 请求查看参赛战队信息
    KinHelper helper;
    IKinService *pKinService = helper.m_ptr;
    if (pKinService == NULL)
    {
        ErrorLn(__FUNCTION__": failed pKinService == NULL");
        return;
    }

    obuf ob;

    SNetMsgHead NetMsgHead;
    NetMsgHead.bySrcEndPoint	= MSG_ENDPOINT_SOCIAL;
    NetMsgHead.byDestEndPoint	= MSG_ENDPOINT_CLIENT;
    NetMsgHead.byKeyModule		= MSG_MODULEID_LEGENDCUP;
    NetMsgHead.byKeyAction		= OC_CUP_SEND_ALL_KINLIST;
    ob<<NetMsgHead;

    SMsgRegisterHead rgHead;
    rgHead.nCount = m_mapRegisterKin.size();
    rgHead.nMaxCount = m_LegendCupBaseInfo.byMaxKinNumber;
    rgHead.dwRegisterLeftTime = getRegisterLeftTime();
    rgHead.dwConfirmKinLeftTime = getConfirmRegisterLeftTime();
    rgHead.llLegendCupID = m_LegendCupBaseInfo.llLegendCupID;

    ob<<rgHead;
    map<DWORD, SCupRegisterInfo>::iterator iter = m_mapRegisterKin.begin();
    for (;iter != m_mapRegisterKin.end(); ++iter)
    {
        // 获取战队信息
        SKinInfo sKinInfo = pKinService->getKinInfo(iter->first);

        // 组织下发信息
        SMsgRegisterKinInfo  RegisterKin;
        RegisterKin.nKinProp_ID = sKinInfo.nKinProp_ID;
        RegisterKin.nKinProp_Level = sKinInfo.nKinProp_Level;

        int nKinWinNum = pKinService->getKinLegendGlory(iter->first, emLegendGlory_WinCount);
        int nLoseNum = pKinService->getKinLegendGlory(iter->first, emLegendGlory_LoseCount);
        RegisterKin.nKinWinNum = nKinWinNum + nLoseNum;
        
        sstrcpyn(RegisterKin.szName, sKinInfo.szName,sizeof(RegisterKin.szName));
        ob<<RegisterKin;
    }

    sendDataToAllViewer(ob.data(), ob.size());

}

void CLegendCup::sendBlackList2Client(ClientID clientID)
{
    if (clientID == INVALID_CLIENT)
    {
        return;
    }
    // 请求查看黑名单信息
    KinHelper helper;
    IKinService *pKinService = helper.m_ptr;
    if (pKinService == NULL)
    {
        ErrorLn(__FUNCTION__": failed pKinService == NULL");
        return;
    }

    obuf ob;

    SNetMsgHead NetMsgHead;
    NetMsgHead.bySrcEndPoint	= MSG_ENDPOINT_SOCIAL;
    NetMsgHead.byDestEndPoint	= MSG_ENDPOINT_CLIENT;
    NetMsgHead.byKeyModule		= MSG_MODULEID_LEGENDCUP;
    NetMsgHead.byKeyAction		= OC_CUP_SEND_ALL_BALCKLIST;
    ob<<NetMsgHead;

    SMsgBalckListHead bkHead;
    bkHead.nCount = m_mapRequstBlack.size();
    bkHead.llLegendCupID = m_LegendCupBaseInfo.llLegendCupID;
    ob<<bkHead;
    map<DWORD, bool>::iterator iter = m_mapRequstBlack.begin();
    for (;iter != m_mapRequstBlack.end(); ++iter)
    {
        // 获取战队信息
        SKinInfo sKinInfo = pKinService->getKinInfo(iter->first);

        // 组织下发信息
        SMsgBlackKinInfo  BlackKin;
        BlackKin.nKinProp_ID = sKinInfo.nKinProp_ID;
        BlackKin.nKinProp_Level = sKinInfo.nKinProp_Level;

        int nKinWinNum = pKinService->getKinLegendGlory(iter->first, emLegendGlory_WinCount);
        int nLoseNum = pKinService->getKinLegendGlory(iter->first, emLegendGlory_LoseCount);
        BlackKin.nKinWinNum = nKinWinNum + nLoseNum;

        sstrcpyn(BlackKin.szName, sKinInfo.szName,sizeof(BlackKin.szName));
        ob<<BlackKin;
    }

    send2Client(clientID, ob.data(), ob.size());
}


void CLegendCup::sendKinMember2Client(ClientID clientID, DWORD dwKinID)
{
    // 请求查看战队成员信息
    KinHelper helper;
    IKinService *pKinService = helper.m_ptr;
    if (pKinService == NULL)
    {
        ErrorLn(__FUNCTION__": failed pKinService == NULL");
        return;
    }

    obuf ob;

    SNetMsgHead NetMsgHead;
    NetMsgHead.bySrcEndPoint	= MSG_ENDPOINT_SOCIAL;
    NetMsgHead.byDestEndPoint	= MSG_ENDPOINT_CLIENT;
    NetMsgHead.byKeyModule		= MSG_MODULEID_LEGENDCUP;
    NetMsgHead.byKeyAction		= OC_CUP_SEND_KIN_MEMBER;
    ob<<NetMsgHead;

    // 如果还没到开始比赛 则取所有当前战队玩家
    if (m_LegendCupBaseInfo.nCurrentRound <= 0)
    {
        PDBID pReturnArray[256] = {0};
        DWORD dwCount = pKinService->getKinMemberList(dwKinID, pReturnArray, 256);
        // 压入数量
        //ob<<dwCount;
		ob<<dwKinID<<dwCount;
        for (DWORD i = 0; i < dwCount; ++i)
        {
            DWORD dwPDBID = pReturnArray[i];

            SKinMember sKinMember = pKinService->getKinMemberInfo(dwPDBID);
            if (sKinMember.dwKinID == NULL)
                continue;
            SLegendCupMemberInfo KinMemberInfo;
            KinMemberInfo.dwUserID = sKinMember.dwUDBID;
            KinMemberInfo.dwKinID = sKinMember.dwKinID;
            KinMemberInfo.dwActorID = sKinMember.dwPDBID; 
            KinMemberInfo.nMatchType = sKinMember.nMatchType;
            KinMemberInfo.nRankScore = sKinMember.nRankScore;
            KinMemberInfo.nGrade = sKinMember.nGrade;
            KinMemberInfo.nLevel = sKinMember.nLevel;
            sstrcpyn(KinMemberInfo.szActorName, sKinMember.szName,sizeof(KinMemberInfo.szActorName));
            ob<<KinMemberInfo;

        }
    }
    else
    {
        // 只取记录的参赛玩家
        DWORD dwCount = m_CupMemberInfo.size();
        // 压入数量
        //ob<<dwCount;
		ob<<dwKinID<<dwCount;
        map<DWORD, SLegendCupMemberInfo>::iterator iter = m_CupMemberInfo.begin();
        for (; iter != m_CupMemberInfo.end(); ++iter)
        {
            if(iter->second.dwKinID == dwKinID)
            {
                ob<<iter->second;
            }
        }
    }

    send2Client(clientID, ob.data(), ob.size());
}


void CLegendCup::send2Client( ClientID clientID, char *pData, int nSize )
{
    IGatewayAcceptorService* pGatewayAcceptorService = gSocialGlobal->getGatewayAcceptorService();
    if (nullptr == pGatewayAcceptorService)
    {
        ErrorLn(__FUNCTION__": failed! IGatewayAcceptorService clientID=" << clientID);
        return;
    }

    
    if(clientID == INVALID_CLIENT)
    {
        ErrorLn(__FUNCTION__": failed! clientID == INVALID_CLIENT clientID=" << clientID);
        return;
    }

    pGatewayAcceptorService->sendData( clientID, PACKAGE_PTR(new std::string((char const *)pData, nSize)));
}

bool CLegendCup::havePassword()
{
    char szDefaultPassWord[DBDEF_PASSWORD_MAXSIZE] = {0};
    // 有没有密码
    if (strcmpi(szDefaultPassWord, m_LegendCupBaseInfo.szPassword) == 0)
    {
        return false;
    }

    return true;
    
}

SRoundTimeInfo* CLegendCup::getRoundTimeInfo(int nRoundID)
{
    map<int, SRoundTimeInfo>::iterator iter = m_mapRoundBeginTime.find(nRoundID);
    if (iter == m_mapRoundBeginTime.end())
    {
        return NULL;
    }

    return &(iter->second);
}

DWORD CLegendCup::getStartRoundLeftTime()
{
    DWORD dwNowTime = (DWORD)time(NULL);

    SRoundTimeInfo* pRoundTimeInfo = getRoundTimeInfo(m_LegendCupBaseInfo.nCurrentRound);
    if (pRoundTimeInfo == NULL)
    {
        ErrorLn(__FUNCTION__"can't find roundTimeinfo! legendCupID="<<m_LegendCupBaseInfo.llLegendCupID<<" nCurrentRound="<< m_LegendCupBaseInfo.nCurrentRound);
        return 0;
    }

    // 本轮开始进入房间的时间
    DWORD dwRoomBeginInterval = pRoundTimeInfo->dwRoundCreateRoomTime;

    if ( dwRoomBeginInterval < dwNowTime)
    {
        return 0;
    }

    return dwRoomBeginInterval - dwNowTime;
}

DWORD CLegendCup::getEndRoundLeftTime()
{
    DWORD dwNowTime = (DWORD)time(NULL);

    SRoundTimeInfo* pRoundTimeInfo = getRoundTimeInfo(m_LegendCupBaseInfo.nCurrentRound);
    if (pRoundTimeInfo == NULL)
    {
        WarningLn(__FUNCTION__"can't find roundTimeinfo! legendCupID="<<m_LegendCupBaseInfo.llLegendCupID<<" nCurrentRound="<< m_LegendCupBaseInfo.nCurrentRound);
        return 0;
    }

    DWORD dwRoundEndTime = pRoundTimeInfo->dwRoundEndTime;

    if ( dwRoundEndTime < dwNowTime)
    {
        return 0;
    }

    return dwRoundEndTime - dwNowTime;
}

bool CLegendCup::recordCupWarResult(SMsgRecordNodeWarResult* pMsgInfo)
{
    map<int,ICompetitionRoundBase *>::iterator iterRound =  m_mapCompetitionRound.find(pMsgInfo->nRoundID);
    if (iterRound == m_mapCompetitionRound.end())
    {
        ErrorLn(__FUNCTION__" can't find cup round legendCupID="<<m_LegendCupBaseInfo.llLegendCupID<<" roundID ="<<pMsgInfo->nRoundID);
        return false;
    }
    // 记录比赛结果
    return iterRound->second->recordCupWarResult(pMsgInfo);
}

bool CLegendCup::setLegendCupNodeState(SMsgSetCompetitionNodeState* sNodeState)
{
    map<int,ICompetitionRoundBase *>::iterator iterRound =  m_mapCompetitionRound.find(sNodeState->nRoundID);
    if (iterRound == m_mapCompetitionRound.end())
    {
        ErrorLn(__FUNCTION__" can't find cup round legendCupID="<<m_LegendCupBaseInfo.llLegendCupID<<" roundID ="<<sNodeState->nRoundID);
        return false;
    }
    // 记录比赛结果
    return iterRound->second->setLegendCupNodeState(sNodeState);

}

bool CLegendCup::judgeNodeWarResult(SMsgJudgeWarResult* pJudgeInfo)
{
    map<int,ICompetitionRoundBase *>::iterator iterRound =  m_mapCompetitionRound.find(pJudgeInfo->nRoundID);
    if (iterRound == m_mapCompetitionRound.end())
    {
        ErrorLn(__FUNCTION__" can't find cup round legendCupID="<<m_LegendCupBaseInfo.llLegendCupID<<" roundID ="<<pJudgeInfo->nRoundID);
        return false;
    }
    // 记录比赛结果
    return iterRound->second->judgeNodeWarResult(pJudgeInfo);
}


bool CLegendCup::joinLegendCupRoom(SMsgJoinLegendCupRoom* pJoinInfo)
{
    map<int,ICompetitionRoundBase *>::iterator iterRound =  m_mapCompetitionRound.find(pJoinInfo->nRoundID);
    if (iterRound == m_mapCompetitionRound.end())
    {
        ErrorLn(__FUNCTION__" can't find cup round legendCupID="<<m_LegendCupBaseInfo.llLegendCupID<<" roundID ="<<pJoinInfo->nRoundID);
        return false;
    }
    // 进入杯赛房间
    return iterRound->second->joinLegendCupRoom(pJoinInfo);
}


bool CLegendCup::lookUpCupNodeWarInfo(SMsgLookUpCupNodeWarInfo* pMsgInfo)
{
    map<int,ICompetitionRoundBase *>::iterator iterRound =  m_mapCompetitionRound.find(pMsgInfo->nRoundID);
    if (iterRound == m_mapCompetitionRound.end())
    {
        ErrorLn(__FUNCTION__" can't find cup round legendCupID="<<m_LegendCupBaseInfo.llLegendCupID<<" roundID ="<<pMsgInfo->nRoundID);
        return false;
    }
    // 查看比赛节点
    return iterRound->second->lookUpCupNodeWarInfo(pMsgInfo);
}


void CLegendCup::resetTriggerRoundBeginAndEnd()
{
    // 线下不创建比赛开始结束定时器 由裁判判定之后检查怎么结束这一轮
    if (!m_LegendCupBaseInfo.bIsOnlineCup)
    {
        return;
    }
    IDateTriggerService * pDateTriggerService = gSocialGlobal->getDateTriggerService();
    if (pDateTriggerService == NULL)
    {
        return;
    }
    //线上这一轮的开始与结束定时器 由系统触发器来结束这一轮比赛
    SRoundTimeInfo* pRoundTimeInfo = getRoundTimeInfo(m_LegendCupBaseInfo.nCurrentRound);
    if (pRoundTimeInfo == NULL)
    {
        ErrorLn(__FUNCTION__"can't find roundTimeinfo! legendCupID="<<m_LegendCupBaseInfo.llLegendCupID<<" nCurrentRound="<< m_LegendCupBaseInfo.nCurrentRound);
        return;
    }
    time_t tConfirmRegisterDeadline = pRoundTimeInfo->dwRoundCreateRoomTime;
    tm *pConfirmRegisterDeadline =  localtime(&tConfirmRegisterDeadline);

    SDateTriggerInfo sTriggerInfo;
    sTriggerInfo.year = pConfirmRegisterDeadline->tm_year + 1900;
    sTriggerInfo.month = pConfirmRegisterDeadline->tm_mon + 1;
    sTriggerInfo.day = pConfirmRegisterDeadline->tm_mday;
    sTriggerInfo.hour = pConfirmRegisterDeadline->tm_hour;
    sTriggerInfo.min = pConfirmRegisterDeadline->tm_min;
    sTriggerInfo.wday = -1;
    if (getStartRoundLeftTime() > 0)
    {
        pDateTriggerService->UnRegisterDateTrigger(ETIMER_CURRENT_ROUND_BEGIN_TRIGGER, this);
        if (pDateTriggerService != NULL)
        {
            pDateTriggerService->RegisterDateTrigger(ETIMER_CURRENT_ROUND_BEGIN_TRIGGER, this, sTriggerInfo);
        }
    }
    else
    {
        // 直接触发
        onTriggerBeginRound();
    }

    // 开启定时器
    time_t tRoundEndTime = pRoundTimeInfo->dwRoundEndTime;
    tm *pRoundEndTime =  localtime(&tRoundEndTime);

    SDateTriggerInfo sTriggerRoundEndTimeInfo;
    sTriggerRoundEndTimeInfo.year   = pRoundEndTime->tm_year + 1900;
    sTriggerRoundEndTimeInfo.month  = pRoundEndTime->tm_mon + 1;
    sTriggerRoundEndTimeInfo.day    = pRoundEndTime->tm_mday;
    sTriggerRoundEndTimeInfo.hour   = pRoundEndTime->tm_hour;
    sTriggerRoundEndTimeInfo.min    = pRoundEndTime->tm_min;
    sTriggerRoundEndTimeInfo.wday = -1;
    if (getEndRoundLeftTime() > 0)
    {
        pDateTriggerService->UnRegisterDateTrigger(ETIMER_CURRENT_ROUND_END_TRIGGER, this);
        if (pDateTriggerService != NULL)
        {
            pDateTriggerService->RegisterDateTrigger(ETIMER_CURRENT_ROUND_END_TRIGGER, this, sTriggerRoundEndTimeInfo);
        }
    }
    else
    {
        // 直接触发
        onTriggerEndRound();
    }
}

bool CLegendCup::bKinInRegister(DWORD dwKinID)
{
   map<DWORD, SCupRegisterInfo>::iterator iter = m_mapRegisterKin.find(dwKinID);
   if (iter == m_mapRegisterKin.end())
   {
       return false;
   }
   return true;
}

void CLegendCup::changeNewRoundCupState()
{
    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeCenter nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID);
        return ;
    }
    ISchemeLegendCup *pLegendCupScheme = pSchemeCenter->getSchemeLegendCup();

    SSchemeLegendCup* pSchemeConfig = pLegendCupScheme->getConfig(m_LegendCupBaseInfo.nCreateConfigID);
    if (pSchemeConfig == NULL)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeConfig nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID);
        return;
    }

    SSchemeLegendCupDetail* pDetailScheme= pLegendCupScheme->getDetailInfo(pSchemeConfig->m_nDetailID, m_LegendCupBaseInfo.nCurrentRound);
    if (pDetailScheme == NULL)
    {
        ErrorLn(__FUNCTION__": failed! can't find roundID nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID<<" m_dwDetailID ="<< pSchemeConfig->m_nDetailID<<" kinCount = "<<m_mapRegisterKin.size());
        return;
    }

    // 切换状态
    if (pDetailScheme->m_nMaxGroup * pDetailScheme->m_nGroupMaxKin == 32)
    {
        m_LegendCupBaseInfo.eCupState = SATE_32TO16;
    }
    else if (pDetailScheme->m_nMaxGroup * pDetailScheme->m_nGroupMaxKin == 16)
    {
        m_LegendCupBaseInfo.eCupState = SATE_16TO8;
    }
    else if (pDetailScheme->m_nMaxGroup * pDetailScheme->m_nGroupMaxKin == 8)
    {
        m_LegendCupBaseInfo.eCupState = SATE_8TO4;
    }
    else if (pDetailScheme->m_nMaxGroup * pDetailScheme->m_nGroupMaxKin == 4)
    {
        m_LegendCupBaseInfo.eCupState = SATE_4TO2;
    }
    else if (pDetailScheme->m_nMaxGroup * pDetailScheme->m_nGroupMaxKin == 2)
    {
        m_LegendCupBaseInfo.eCupState = SATE_FINAL;
    }

    changeLegendCupState((ECupState)m_LegendCupBaseInfo.eCupState);
}

// 发布比赛
bool CLegendCup::publishCompetition()
{
    if (!saveCupKinData())
    {
        ErrorLn(__FUNCTION__": saveCupKinData error! can't publish Competition!");
        return false;
    }
    changeLegendCupState(SATE_GROUPING);

    // todo 通知杯赛参赛者发布成功
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver == NULL)
    {
        return false;
    }

    SSharePersonProperty shareCreaterProperty = pShareReceiver->GetSharePerson(m_LegendCupBaseInfo.dwCreataActorID);
    if (shareCreaterProperty.dwPDBID != INVALID_PDBID)
    {
        obuf32 ob;
        SNetMsgHead NetMsgHead;
        NetMsgHead.bySrcEndPoint	= MSG_ENDPOINT_SOCIAL;
        NetMsgHead.byDestEndPoint	= MSG_ENDPOINT_CLIENT;
        NetMsgHead.byKeyModule	    = MSG_MODULEID_LEGENDCUP;
        NetMsgHead.byKeyAction	    = OC_CUP_SEND_CREATER_PUBLIC_SUCCESS;
        ob<<NetMsgHead;
        send2Client(shareCreaterProperty.clientID, ob.data(), ob.size());
    }

    // 通知所有队员杯赛发布了
    for (map<DWORD, SLegendCupMemberInfo>::iterator iter = m_CupMemberInfo.begin(); iter != m_CupMemberInfo.end();++iter)
    {
        SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(iter->second.dwActorID);
        if (shareProperty.dwPDBID == INVALID_PDBID)
        {
            continue;
        }

        obuf32 ob;
        SNetMsgHead NetMsgHead;
        NetMsgHead.bySrcEndPoint	= MSG_ENDPOINT_SOCIAL;
        NetMsgHead.byDestEndPoint	= MSG_ENDPOINT_CLIENT;
        NetMsgHead.byKeyModule	    = MSG_MODULEID_LEGENDCUP;
        NetMsgHead.byKeyAction	    = OC_CUP_SEND_PUBLIC_SUCCESS;
        ob<<NetMsgHead;
        send2Client(shareProperty.clientID, ob.data(), ob.size());
    }

    return true;
}


bool CLegendCup::haveRightJudge(ClientID clientID)
{
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver == NULL)
    {
        ErrorLn(__FUNCTION__": pShareReceiver == NULL");
        return false;
    }
    SSharePersonProperty shareProperty = pShareReceiver->GetSharePersonByClientID(clientID);

    // 检查是否有权限裁定
    if(m_LegendCupBaseInfo.bIsOnlineCup)
    {
        return false;
    }

    if (m_LegendCupBaseInfo.dwCreataActorID != shareProperty.dwPDBID)
    {
        return false;
    }

    return true;
}


bool CLegendCup::haveRightJoinCupRoom(SMsgJoinLegendCupRoom *pJoinedRoom)
{
	// GM账号直接返回true
	if (IsGM(pJoinedRoom->idActor))
	{
		return true;
	}

	// 普通账号 检查是否有权限加入
    if(pJoinedRoom->dwKinID == 0)
    {
        ErrorLn(__FUNCTION__": pJoinedRoom->dwKinID == 0");
        return false;
    }
    
    if (m_mapRegisterKin.find(pJoinedRoom->dwKinID)  == m_mapRegisterKin.end())
    {
        g_ExternalFacade.showSystemMessage(pJoinedRoom->idActor, CHAT_TIP_LEGENDCUP_KIN_CANT_JOIN,"");
        return false;
    }

    if (m_CupMemberInfo.find(pJoinedRoom->idActor) == m_CupMemberInfo.end())
    {
        g_ExternalFacade.showSystemMessage(pJoinedRoom->idActor, CHAT_TIP_LEGENDCUP_NOT_CUPMEMBER_CANT_JOIN,"");
        return false;
    }
    return true;
}

void CLegendCup::updateSingleCupBaseInfo2Client(ClientID clientID, DWORD dwClientKinID, bool bNewCreate)
{
    DWORD dwNowTime = (DWORD)time(NULL);
    obuf ob;
    SNetMsgHead NetMsgHead;
    NetMsgHead.bySrcEndPoint	= MSG_ENDPOINT_SOCIAL;
    NetMsgHead.byDestEndPoint	= MSG_ENDPOINT_CLIENT;
    NetMsgHead.byKeyModule		= MSG_MODULEID_LEGENDCUP;
    NetMsgHead.byKeyAction		= OC_CUP_SEND_UPDATE_SINGEL_CUP_INFO;
    ob<<NetMsgHead;

    SMsgLegendCupInfo CupInfo;
    CupInfo.bNewCreate          = bNewCreate;
    CupInfo.llLegendCupID       = m_LegendCupBaseInfo.llLegendCupID;                            // 杯赛ID
    CupInfo.nCreateConfigID     = m_LegendCupBaseInfo.nCreateConfigID;                          // 创建的配置ID
    CupInfo.byCreateCupType     = m_LegendCupBaseInfo.byCreateCupType;                          // 杯赛类型 ELegendCupType
    CupInfo.bIsOnlineCup        = m_LegendCupBaseInfo.bIsOnlineCup;                             // 线上比赛 线下比赛
    CupInfo.dwCreataActorID     = m_LegendCupBaseInfo.dwCreataActorID;                          // 创建玩家 系统的为 0
    CupInfo.byMaxKinNumber      = m_LegendCupBaseInfo.byMaxKinNumber;                           // 最大参赛队伍
    CupInfo.nCurrentKinNum      = getRegisterCount();                                           // 杯赛参赛队伍数量
    CupInfo.dwRegisterLeftTime  = m_LegendCupBaseInfo.dwRegisterDeadline - dwNowTime >0? m_LegendCupBaseInfo.dwRegisterDeadline - dwNowTime: 0;
    CupInfo.bClanLimit          = m_LegendCupBaseInfo.bClanLimit;                               // 只有联盟限制才能参加
    CupInfo.nLevelLimit         = m_LegendCupBaseInfo.nLevelLimit;                             // 等级限制
    CupInfo.eCupState           = m_LegendCupBaseInfo.eCupState;                               // 状态 ECupState
    CupInfo.bIsEnd              = m_LegendCupBaseInfo.bIsEnd;                                  // 是否结束
    CupInfo.dwEndTime           = m_LegendCupBaseInfo.dwEndTime;                                // 结束日期
    CupInfo.bHavePassword       = havePassword() ;                                              // 是否有密码
    CupInfo.bIsJoinedCup        = bKinInRegister(dwClientKinID);                               // 是否加入了的比赛
    CupInfo.nCurrentKinNum      = m_mapRegisterKin.size();                                      // 当前报名数量
	CupInfo.nNeedRoundCount		= m_LegendCupBaseInfo.nNeedRoundCount;							// 杯赛需要打几轮
	CupInfo.nBeginRound			= m_LegendCupBaseInfo.nBeginRound;								// 本场比赛由第几轮开始
	CupInfo.nCurrentRound		= m_LegendCupBaseInfo.nCurrentRound;							// 当前打到第几轮
    CupInfo.dwTotalCompetitionBonus = m_LegendCupBaseInfo.dwTotalRegisterTicket + m_LegendCupBaseInfo.dwContriBonus;
    CupInfo.dwRegistGold        = m_LegendCupBaseInfo.dwRegisterNeedTicket;
    
    sstrcpyn(CupInfo.szCupName, m_LegendCupBaseInfo.szCupName,sizeof(CupInfo.szCupName));       // 杯赛名称
    sstrcpyn(CupInfo.szCreaterName, m_LegendCupBaseInfo.szCreaterName,sizeof(CupInfo.szCreaterName));     // 创建者

    ob<<CupInfo;
    send2Client(clientID, ob.data(), ob.size());
    
    sendAllKinMemberInfo2Client(clientID);
}


void CLegendCup::updateSingleCupBaseInfoAllView(bool bNewCreate /*= false */)
{
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver == NULL)
    {
        ErrorLn(__FUNCTION__": pShareReceiver == NULL");
        return;
    }

    DWORD dwNowTime = (DWORD)time(NULL);

    SNetMsgHead NetMsgHead;
    NetMsgHead.bySrcEndPoint	= MSG_ENDPOINT_SOCIAL;
    NetMsgHead.byDestEndPoint	= MSG_ENDPOINT_CLIENT;
    NetMsgHead.byKeyModule		= MSG_MODULEID_LEGENDCUP;
    NetMsgHead.byKeyAction		= OC_CUP_SEND_UPDATE_SINGEL_CUP_INFO;


    SMsgLegendCupInfo CupInfo;
    CupInfo.bNewCreate          = bNewCreate;
    CupInfo.llLegendCupID       = m_LegendCupBaseInfo.llLegendCupID;                            // 杯赛ID
    CupInfo.nCreateConfigID     = m_LegendCupBaseInfo.nCreateConfigID;                          // 创建的配置ID
    CupInfo.byCreateCupType     = m_LegendCupBaseInfo.byCreateCupType;                          // 杯赛类型 ELegendCupType
    CupInfo.bIsOnlineCup        = m_LegendCupBaseInfo.bIsOnlineCup;                             // 线上比赛 线下比赛
    CupInfo.dwCreataActorID     = m_LegendCupBaseInfo.dwCreataActorID;                          // 创建玩家 系统的为 0
    CupInfo.byMaxKinNumber      = m_LegendCupBaseInfo.byMaxKinNumber;                           // 最大参赛队伍
    CupInfo.nCurrentKinNum      = getRegisterCount();                                           // 杯赛参赛队伍数量
    CupInfo.dwRegisterLeftTime  = m_LegendCupBaseInfo.dwRegisterDeadline - dwNowTime >0? m_LegendCupBaseInfo.dwRegisterDeadline - dwNowTime: 0;
    CupInfo.bClanLimit          = m_LegendCupBaseInfo.bClanLimit;                               // 只有联盟限制才能参加
    CupInfo.nLevelLimit         = m_LegendCupBaseInfo.nLevelLimit;                             // 等级限制
    CupInfo.eCupState           = m_LegendCupBaseInfo.eCupState;                               // 状态 ECupState
    CupInfo.bIsEnd              = m_LegendCupBaseInfo.bIsEnd;                                  // 是否结束
    CupInfo.dwEndTime           = m_LegendCupBaseInfo.dwEndTime;                                // 结束日期
    CupInfo.bHavePassword       = havePassword() ;                                              // 是否有密码
    
    CupInfo.nCurrentKinNum      = m_mapRegisterKin.size();                                      // 当前报名数量
    CupInfo.dwTotalCompetitionBonus = m_LegendCupBaseInfo.dwTotalRegisterTicket + m_LegendCupBaseInfo.dwContriBonus;
    CupInfo.dwRegistGold        = m_LegendCupBaseInfo.dwRegisterNeedTicket;
    
    sstrcpyn(CupInfo.szCupName, m_LegendCupBaseInfo.szCupName,sizeof(CupInfo.szCupName));       // 杯赛名称
    sstrcpyn(CupInfo.szCreaterName, m_LegendCupBaseInfo.szCreaterName,sizeof(CupInfo.szCreaterName));     // 创建者

    for (map<PDBID,bool>::iterator iter = m_mapCupViewActor.begin(); iter != m_mapCupViewActor.end(); ++iter)
    {
        // 组织下发数据
        obuf sendBuf;
        SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(iter->first);
        if (shareProperty.dwPDBID == INVALID_PDBID)
        {
            continue;
        }
        CupInfo.bIsJoinedCup        = bKinInRegister(shareProperty.dwKinID);                               // 是否加入了的比赛
        sendBuf<<NetMsgHead<<CupInfo;
        send2Client(shareProperty.clientID, sendBuf.data(), sendBuf.size());

        sendAllKinMemberInfo2Client(shareProperty.clientID);
    }

   
}

void CLegendCup::deleteKinMember(DWORD dwKinMemberID)
{
    //移除内存数据
    m_CupMemberInfo.erase(dwKinMemberID);

    //// todo移除数据库记录
    //IDBEngineService* pDBEngine = (IDBEngineService*)gSocialGlobal->getDBEngineService();
    //if (pDBEngine == NULL)
    //{
    //    ErrorLn(__FUNCTION__": failed pDBEngine == NULL");
    //    return;
    //}
    //DBREQ_PARAM_CUP_DELETE_KINMEMBER DBInfo;
    //DBInfo.llLegendCupID    = m_LegendCupBaseInfo.llLegendCupID;              // 杯赛ID
    //DBInfo.dwActorID        = dwKinMemberID;

    //if (pDBEngine->executeSP(GAMEDB_REQUEST_LEGEND_CUP_CUP_DELETE_KINMEMBER, 0, LPCSTR(&DBInfo), sizeof(DBInfo), static_cast<IDBRetSink *>(this)) == false)
    //{
    //    ErrorLn(__FUNCTION__": pDBEngine->ExecuteSP()  cmdID = "<<GAMEDB_REQUEST_LEGEND_CUP_CUP_DELETE_KINMEMBER<<" failed");
    //    return;
    //}
}

bool CLegendCup::bIsCanPublic(DWORD dwActorID)
{
    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (NULL ==  pSchemeCenter)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeCenter");
        return false;
    }
    ISchemeLegendCup *pLegendCupScheme = pSchemeCenter->getSchemeLegendCup();
    if (NULL ==  pLegendCupScheme)
    {
        ErrorLn(__FUNCTION__": pLegendCupScheme == NULL");
        return false;
    }
    SSchemeLegendCup* pSchemeConfig = pLegendCupScheme->getConfig(m_LegendCupBaseInfo.nCreateConfigID);
    if ( NULL == pSchemeConfig)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeConfig nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID);
        return false;
    }

    if (m_mapRegisterKin.size() < pSchemeConfig->m_nMiniStartKinCount)
    {
        if (dwActorID != INVALID_PDBID)
        {
            g_ExternalFacade.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_PUBLIC_COMPETITION_KINCOUNT_SATISFY,"");
        }
        else
        {
            WarningLn(__FUNCTION__": kin count not satisfy! cupName=["<<m_LegendCupBaseInfo.szCupName<<"] lengendCupID="<<m_LegendCupBaseInfo.llLegendCupID);
        }
        return false;
    }

    if (m_LegendCupBaseInfo.eCupState != SATE_CONFIRMENTRIES)
    {
        if (dwActorID != INVALID_PDBID)
        {
            g_ExternalFacade.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_NOT_IN_COFIRMREGISTER,"");
        }
        return false;
    }
    

    return true;
}

int CLegendCup::getRegisterCount()
{
    return m_mapRegisterKin.size();
}

void CLegendCup::exitStartCompetitionNode(int nSerchID, DWORD dwNewStartTime)
{
    // 线下不处理
    if (!m_LegendCupBaseInfo.bIsOnlineCup)
    {
        return;
    }
    
    int nRound = getRoundIDBySerchID(nSerchID);
    map<int,ICompetitionRoundBase *>::iterator iter = m_mapCompetitionRound.find(nRound);
    if (iter == m_mapCompetitionRound.end())
    {
        ErrorLn(__FUNCTION__": can't find Round = "<<nRound<<" lengendCupID="<<m_LegendCupBaseInfo.llLegendCupID);
        return;
    }

    iter->second->updateNodeNewStartTime(nSerchID, dwNewStartTime);
}

void CLegendCup::sendAbstentionKinMail(DWORD dwFailedKinID)
{
	ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
	if (NULL == pSchemeCenter)
	{
		ErrorLn(__FUNCTION__": failed! nullptr == pSchemeCenter");
		return;
	}

	ISchemeMailDataCenter *pSchemeMailDataCenter = pSchemeCenter->getSchemeMailDataCenter();
	if (NULL == pSchemeMailDataCenter)
	{
		ErrorLn(__FUNCTION__" nullptr == pSchemeMailDataCenter");
		return;
	}

	SMailConfig* pMailConfig = pSchemeMailDataCenter->getConfig(emMailFill_LegendCupAbstentionMail);
	if (NULL == pMailConfig)
	{
		ErrorLn(__FUNCTION__" nullptr == pMailConfig");
		return;
	}

	KinHelper kinHelper;
	IKinService *pKinService = kinHelper.m_ptr;
	if (pKinService == NULL)
	{
		ErrorLn(__FUNCTION__": failed! pKinService == NULL");
		return;
	}

	MailHelper mailHelper;
	IMailService *pMailService = mailHelper.m_ptr;
	if (NULL == pMailService)
	{
		ErrorLn(__FUNCTION__": failed! pMailService == NULL");
		return;
	}

	tm localNowTime;
	time_t currentTime = time(NULL);
	localtime_s(&localNowTime, &currentTime);

	PDBID memberList[256] = { 0 };
	int memberCount = pKinService->getKinMemberList(dwFailedKinID, memberList, 256);
	for (int i = 0; i < memberCount; ++i)
	{
		// 先给所有玩家发
		SMailSendData mailInfo;
		mailInfo.nType = emMailType_System;							// 邮件类型
		mailInfo.nSourceType = emMailSourceType_System;				// 资源来源类型
		sstrcpyn(mailInfo.szSenderName, pMailConfig->szSenderName, sizeof(mailInfo.szSenderName));// 发件者姓名
		mailInfo.dwMasterPDBID = memberList[i];
		sstrcpyn(mailInfo.szTitle, pMailConfig->szTitle, sizeof(mailInfo.szTitle));
		mailInfo.nPlusMoney = 0;

		char  szParam[MAIL_CONTENT_MAXSIZE];					// 内容
		ssprintf_s(szParam, sizeof(szParam), a2utf8("%s;%s;%d;%d"), pMailConfig->szContext, m_LegendCupBaseInfo.szCupName, localNowTime.tm_mon + 1, localNowTime.tm_mday);

		pMailService->sendMail(mailInfo, emMailFill_LegendCupAbstentionMail, szParam);
	}

}

int CLegendCup::getRoundIDBySerchID(int nSerchID)
{
    return nSerchID / 10000;
}

void CLegendCup::getCupKinMemberList(DWORD dwKinID, list<DWORD> &KinMember)
{
    map<DWORD, SLegendCupMemberInfo>::iterator iter = m_CupMemberInfo.begin();
    for (;iter != m_CupMemberInfo.end(); ++iter)
    {
        if (iter->second.dwKinID == dwKinID)
        {
            KinMember.push_back(iter->first);
        }
    }
}

bool CLegendCup::distributionOfRewards()
{
    // 分配总奖金
    if (!distributionTotalBonus())
    {
        ErrorLn(__FUNCTION__" distributionTotalBonus failed! LegendCupID ="<<m_LegendCupBaseInfo.llLegendCupID);
        return false;
    }

    // 分配物品奖励列表发放
    if (!distributionPrizeList())
    {
        ErrorLn(__FUNCTION__" distributionPrizeList failed! LegendCupID ="<<m_LegendCupBaseInfo.llLegendCupID);
        return false;
    }

    return true;

}

bool CLegendCup::distributionTotalBonus()
{
    MailHelper mailHelper;
    IMailService *pMailService = mailHelper.m_ptr;
    if (NULL == pMailService )
    {
        ErrorLn(__FUNCTION__": failed! pMailService == NULL");
        return false;
    }

    // 分配奖金
    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (NULL ==  pSchemeCenter)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeCenter");
        return false;
    }
    ISchemeLegendCup *pLegendCupScheme = pSchemeCenter->getSchemeLegendCup();
    if (NULL ==  pLegendCupScheme)
    {
        ErrorLn(__FUNCTION__": pLegendCupScheme == NULL");
        return false;
    }
    SSchemeLegendCup* pSchemeConfig = pLegendCupScheme->getConfig(m_LegendCupBaseInfo.nCreateConfigID);
    if ( NULL == pSchemeConfig)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeConfig nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID);
        return false;
    }

	ISchemeMailDataCenter *pSchemeMailDataCenter = pSchemeCenter->getSchemeMailDataCenter();
	if (NULL == pSchemeMailDataCenter)
	{
		ErrorLn(__FUNCTION__" nullptr == pSchemeMailDataCenter");
		return NULL;
	}

    SMailConfig* pMailConfig = pSchemeMailDataCenter->getConfig(emMailFill_LegendCupBonus);
    if( NULL == pMailConfig)
    {
        ErrorLn(__FUNCTION__" nullptr == pMailConfig");
		return NULL;
    }

    BYTE byRankTicketRatio[DBDEF_LEGENDCUP_KINSCOUNT_LEN] = {0}; // 分配奖励比例
    if (m_LegendCupBaseInfo.byTicketAllocationMode == ETAM_CustomerSet)
    {
        // 玩家自定义设置
        memcpy(byRankTicketRatio, m_LegendCupBaseInfo.byRankTicketRatio, sizeof(byRankTicketRatio));// 分配奖励比例
    }
    else
    {
        // 获取系统配置的奖励比例
        int nPrizeID = pSchemeConfig->m_nPrizeID;
        for (int i = 0; i < DBDEF_LEGENDCUP_KINSCOUNT_LEN ; ++i)
        {
            int nRankID = i+1;
            SSchemeLegendCupPrize* pPrizeInfo = pLegendCupScheme->getPrizeInfo(nPrizeID, nRankID);
            if (pPrizeInfo == NULL)
            {
                WarningLn(__FUNCTION__": legendcup pPrizeInfo == NULL nPrizeID="<<nPrizeID<<" nRankID="<<nRankID);
                continue;
            }
            byRankTicketRatio[i] = pPrizeInfo->m_byPrizeRatio;
        }
    }

    DWORD dwTotalRewards = m_LegendCupBaseInfo.dwContriBonus + m_LegendCupBaseInfo.dwTotalRegisterTicket;
    DWORD dwTotalRatio = 0;
    for (int i = 0; i < DBDEF_LEGENDCUP_KINSCOUNT_LEN ; ++i)
    {
        dwTotalRatio += byRankTicketRatio[i];
    }

    //todo 杯赛奖励
    for (int i = 0; i < DBDEF_LEGENDCUP_KINSCOUNT_LEN ; ++i)
    {
        // 权重
        DWORD dwRatio = byRankTicketRatio[i];
        if (dwRatio == 0)
        {
            continue;
        }

        list<DWORD> dwRewardKinList;
        int nRankID = i + 1;
        int nRewardsCount = 0;
        for (map<DWORD, SCupRegisterInfo>::iterator iter = m_mapRegisterKin.begin(); iter != m_mapRegisterKin.end(); ++iter)
        {
            if (nRankID == iter->second.nRankID)
            {
                nRewardsCount++;
                dwRewardKinList.push_back(iter->first);
            }
        }
        // 平均分配给参赛队员
        if (dwRewardKinList.size() <= 0)
        {
            continue;
        }

        // 平均每个参赛队能得到多少
        DWORD dwKinRewardMoney = (dwTotalRewards * dwRatio) / (dwTotalRatio * nRewardsCount);
        for (list<DWORD>::iterator iterKin = dwRewardKinList.begin(); iterKin != dwRewardKinList.end(); ++iterKin)
        {
            // 分配给参赛队员
            list<DWORD> dwKinMemberList;
            for (map<DWORD, SLegendCupMemberInfo>::iterator iterFindMember = m_CupMemberInfo.begin(); iterFindMember != m_CupMemberInfo.end(); ++iterFindMember)
            {
                if (iterFindMember->second.dwKinID == *iterKin)
                {
                    dwKinMemberList.push_back(iterFindMember->second.dwActorID);
                }
            }
            if (dwKinMemberList.size() <= 0)
            {
                continue;
            }

            for (list<DWORD>::iterator iterMember = dwKinMemberList.begin(); iterMember != dwKinMemberList.end(); ++iterMember)
            {
                // todo 发放奖励
                DWORD dwMemberRewardMoney = dwKinRewardMoney / dwKinMemberList.size();
                SMailSendData mailInfo;
                mailInfo.nType = emMailType_System;							// 邮件类型
                mailInfo.nSourceType = emMailSourceType_System;				// 资源来源类型
                sstrcpyn(mailInfo.szSenderName, pMailConfig->szSenderName, sizeof(mailInfo.szSenderName));// 发件者姓名
                mailInfo.dwMasterPDBID = *iterMember;
                sstrcpyn(mailInfo.szTitle, pMailConfig->szTitle, sizeof(mailInfo.szTitle));
                mailInfo.nPlusMoney = dwMemberRewardMoney;

                char  szParam[MAIL_CONTENT_MAXSIZE];					// 内容
                ssprintf_s(szParam, sizeof(szParam), a2utf8("%s;%d;%d"),m_LegendCupBaseInfo.szCupName, nRankID, dwMemberRewardMoney);

                if (dwMemberRewardMoney > 0)
                {
                    pMailService->sendMail(mailInfo,emMailFill_LegendCupBonus, szParam);
                }
                
            }
        }
    }

    return true;
}

bool CLegendCup::distributionPrizeList()
{
    MailHelper mailHelper;
    IMailService *pMailService = mailHelper.m_ptr;
    if (pMailService == NULL)
    {
       ErrorLn(__FUNCTION__": failed! pMailService == NULL");
       return false;
    }

    // 奖励物品列表分配
    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeCenter");
        return false;
    }
    ISchemeLegendCup *pLegendCupScheme = pSchemeCenter->getSchemeLegendCup();
    if (pLegendCupScheme == NULL)
    {
        ErrorLn(__FUNCTION__": pLegendCupScheme == NULL");
        return false;
    }
    SSchemeLegendCup* pSchemeConfig = pLegendCupScheme->getConfig(m_LegendCupBaseInfo.nCreateConfigID);
    if (pSchemeConfig == NULL)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeConfig nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID);
        return false;
    }

    ISchemeMailDataCenter *pSchemeMailDataCenter = pSchemeCenter->getSchemeMailDataCenter();
	if (NULL == pSchemeMailDataCenter)
	{
		ErrorLn(__FUNCTION__" nullptr == pSchemeMailDataCenter");
		return NULL;
	}

    SMailConfig* pMailConfig = pSchemeMailDataCenter->getConfig(emMailFill_LegendCupPrize);
    if( NULL == pMailConfig)
    {
        ErrorLn(__FUNCTION__" nullptr == pMailConfig");
		return NULL;
    }

    for (map<DWORD, SCupRegisterInfo>::iterator iterKin = m_mapRegisterKin.begin(); iterKin != m_mapRegisterKin.end(); ++iterKin)
    {
        int nRankID = iterKin->second.nRankID;
        SSchemeLegendCupPrize* pPrizeInfo = pLegendCupScheme->getPrizeInfo(pSchemeConfig->m_nPrizeID, nRankID);
        if (pPrizeInfo == NULL)
        {
            continue;
        }

        // 分配给参赛队员
        list<DWORD> dwKinMemberList;
        for (map<DWORD, SLegendCupMemberInfo>::iterator iterMember = m_CupMemberInfo.begin(); iterMember != m_CupMemberInfo.end(); ++iterMember)
        {
            if (iterMember->second.dwKinID == iterKin->first)
            {

                SMailSendData mailInfo;
                mailInfo.nType = emMailType_System;							// 邮件类型
                mailInfo.nSourceType = emMailSourceType_System;				// 资源来源类型
                sstrcpyn(mailInfo.szSenderName, pMailConfig->szSenderName ,sizeof(mailInfo.szSenderName));// 发件者姓名

                mailInfo.dwMasterPDBID = iterMember->second.dwActorID;
                sstrcpyn(mailInfo.szTitle, pMailConfig->szTitle, sizeof(mailInfo.szTitle));

                int nIndex = 0;
                for(int n =0; n < CUP_PRIZEID_COUNT_SIZE; ++n)
                {
                    int nPrizeID = pPrizeInfo->m_nPrizeList[n];
                    if(nPrizeID > 0)
                    {
                        mailInfo.nPlusGoodsNum++;
                        DBREQ_PARAM_MAILGOODS GoodInfo;
                        GoodInfo.dwGoodsID = nPrizeID;
                        GoodInfo.nCount = 1;
                        mailInfo.sGoodsList[nIndex++] = GoodInfo;
                    }
                }
                
                char  szParam[MAIL_CONTENT_MAXSIZE];					// 内容
                ssprintf_s(szParam, sizeof(szParam), a2utf8("%s;%d"),m_LegendCupBaseInfo.szCupName, nRankID);
                if (mailInfo.nPlusGoodsNum> 0)
                {
                    pMailService->sendMail(mailInfo,emMailFill_LegendCupPrize, szParam);
                }
                
            }
        }
    }

    return true;
}

void CLegendCup::DateCome(unsigned long nTriggerID)
{
    switch (nTriggerID)
    {
    case ETIMER_END_REGISTER_TRIGGER:
        {
            onTriggerEndRegister();
        }
        break;
    case ETIMER_CONFIRM_REGISTER_TRIGGER:
        {
            onTriggerConfirmRegister();
        }
        break;
    case ETIMER_CURRENT_ROUND_BEGIN_TRIGGER:
        {
            onTriggerBeginRound();
        }
        break;
    case ETIMER_CURRENT_ROUND_END_TRIGGER:
        {
            onTriggerEndRound();
        }
        break;
    default:
        break;
    }
}

void CLegendCup::sendBackRegisterNeedTicket(DWORD dwKinID, DWORD dwRegisterPDBID)
{
    if (m_LegendCupBaseInfo.dwRegisterNeedTicket < 0)
    {
        return;
    }

    if (dwKinID == 0)
    {
        return;
    }
    
    if (dwRegisterPDBID == INVALID_PDBID)
    {
        return;
    }

    // 退回申请比赛点卷
    if (m_LegendCupBaseInfo.dwTotalRegisterTicket > m_LegendCupBaseInfo.dwRegisterNeedTicket)
    {
        m_LegendCupBaseInfo.dwTotalRegisterTicket -= m_LegendCupBaseInfo.dwRegisterNeedTicket;
    }
    else
    {
        m_LegendCupBaseInfo.dwTotalRegisterTicket = 0;
    }

    // 先减少分配资金再回退金币
    writeCupBaseDataToDB();

    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeCenter");
        return;
    }
    ISchemeLegendCup *pLegendCupScheme = pSchemeCenter->getSchemeLegendCup();
    if (pLegendCupScheme == NULL)
    {
        ErrorLn(__FUNCTION__": pLegendCupScheme == NULL");
        return;
    }
    SSchemeLegendCup* pSchemeConfig = pLegendCupScheme->getConfig(m_LegendCupBaseInfo.nCreateConfigID);
    if (pSchemeConfig == NULL)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeConfig nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID);
        return;
    }

    ISchemeMailDataCenter *pSchemeMailDataCenter = pSchemeCenter->getSchemeMailDataCenter();
    if (NULL == pSchemeMailDataCenter)
    {
        ErrorLn(__FUNCTION__" nullptr == pSchemeMailDataCenter");
        return;
    }

    SMailConfig* pMailConfig = pSchemeMailDataCenter->getConfig(emMailFill_LegendCupRetRegisterCup);
    if( NULL == pMailConfig)
    {
        ErrorLn(__FUNCTION__" nullptr == pMailConfig");
        return;
    }

    MailHelper mailHelper;
    IMailService *pMailService = mailHelper.m_ptr;
    if (pMailService == NULL)
    {
        ErrorLn(__FUNCTION__": failed! pMailService == NULL");
        return;
    }

    SMailSendData mailInfo;
    mailInfo.nType = emMailType_System;							// 邮件类型
    mailInfo.nSourceType = emMailSourceType_System;				// 资源来源类型
    sstrcpyn(mailInfo.szSenderName, pMailConfig->szSenderName, sizeof(mailInfo.szSenderName));// 发件者姓名
    mailInfo.dwMasterPDBID = dwRegisterPDBID;
    sstrcpyn(mailInfo.szTitle, pMailConfig->szTitle, sizeof(mailInfo.szTitle));
    mailInfo.nPlusMoney = m_LegendCupBaseInfo.dwRegisterNeedTicket;
    pMailService->sendMail(mailInfo, emMailFill_LegendCupRetRegisterCup, "");
}

bool CLegendCup::setCupDBEXlistContext(LPCSTR pszContext, int nLen)
{
    if (nLen < sizeof(DBREQ_RESULT_EX_LIST_DATA_INFO))
    {
        return false;
    }

    DBREQ_RESULT_EX_LIST_DATA_INFO* pRetData = (DBREQ_RESULT_EX_LIST_DATA_INFO*)pszContext;

    // 黑名单战队
    m_mapRequstBlack.clear();
    for (int i = 0; i < DBDEF_LEGENDCUP_BALCKLIST_LEN; ++i )
    {
        if (pRetData->dwBlackList[i] != 0)
        {
            m_mapRequstBlack[pRetData->dwBlackList[i]] = true;
        }
    }

    // 申请参赛的战队
    m_mapRegisterKin.clear();
    for (int i = 0; i < DBDEF_LEGENDCUP_KINSCOUNT_LEN; ++i )
    {
        if (pRetData->dwRegisterList[i] != 0)
        {
            DWORD dwKinID = pRetData->dwRegisterList[i];
            SCupRegisterInfo info;
            info.nRankID = pRetData->nRegisterRankList[i];
            info.dwRegisterPDBID = pRetData->dwRegisterPDBID[i];

            m_mapRegisterKin[dwKinID] = info;
        }
    }

    // 已读标志
    setLegendCupDBReadFlag(emLegendRead_EXListData);
    return true;
}

void CLegendCup::onKinMemberLogin(DWORD dwActorID)
{
    // 正在参加比赛 前往参加
    ChatHelper chatHelper;
    IChatService *pChatService = chatHelper.m_ptr;
    if (NULL == pChatService)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pChatService" );
        return;
    }

    // 先把基础数据, 当前轮所在节点信息下发下去
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    SSharePersonProperty shareProperty= pShareReceiver->GetSharePerson(dwActorID);
    if(pShareReceiver != NULL)
    {
        updateSingleCupBaseInfo2Client(shareProperty.clientID, shareProperty.dwKinID);
        if (m_LegendCupBaseInfo.nCurrentRound == 0)
        {
            return;
        }

		map<int, ICompetitionRoundBase*>::iterator itRound = m_mapCompetitionRound.find(m_LegendCupBaseInfo.nCurrentRound);
		if (itRound == m_mapCompetitionRound.end())
		{
			return;
		}
		itRound->second->updateSingleCupNodeInfo2Client(shareProperty.clientID, shareProperty.dwKinID);
    }

    string strParam = "";
    strParam += CHAT_MSG_ITEM_FLAG;

    char szText[256] = {0};
    ssprintf_s(szText, sizeof(szText), "%d;%s;%s;%lld", CHAT_CHANNEL_KIN, m_LegendCupBaseInfo.szCupName, strParam.c_str(), m_LegendCupBaseInfo.llLegendCupID);

    pChatService->broadcastSystemMessage(CHAT_BROADCASTTYPE_SINGLE, dwActorID, CHAT_TIP_LEGENDCUP_KIN_LOGIN_CHAT_INFO, szText);
    
}

void CLegendCup::onKinMemberLogout(DWORD dwActorID)
{
    return;
}

void CLegendCup::addLegendCupViewer(DWORD dwActorID)
{
    m_mapCupViewActor.insert(pair<DWORD, bool>(dwActorID, true));
}

void CLegendCup::removeLegendCupViewer(DWORD dwActorID)
{
    m_mapCupViewActor.erase(dwActorID);
}

void CLegendCup::sendDataToAllViewer(char *pData, int nSize)
{
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver == NULL)
    {
        ErrorLn(__FUNCTION__": pShareReceiver == NULL");
        return;
    }

    if (m_mapCupViewActor.size() == 0)
    {
        return;
    }

    list<DWORD> listOffLineActor;

    for (map<PDBID,bool>::iterator iter = m_mapCupViewActor.begin(); iter != m_mapCupViewActor.end(); ++iter)
    {
        SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(iter->first);
        if (shareProperty.dwPDBID == INVALID_PDBID)
        {
            listOffLineActor.push_back(iter->first);
        }
        
        send2Client(shareProperty.clientID, pData, nSize);
    }

    if (listOffLineActor.size() <= 0)
    {
        return;
    }

    // 移除不在线的玩家
    for (list<DWORD>::iterator iterOffLine = listOffLineActor.begin();iterOffLine != listOffLineActor.end(); ++iterOffLine)
    {
        m_mapCupViewActor.erase(*iterOffLine);
    }
    
}

void CLegendCup::getAllViewerList(map<DWORD,bool> &mapViewer)
{
    mapViewer = m_mapCupViewActor;
}

void CLegendCup::distributionEveryValidWarPrize(list<DWORD> dwKinList)
{
    // 每场有效比赛奖励
    MailHelper mailHelper;
    IMailService *pMailService = mailHelper.m_ptr;
    if (pMailService == NULL)
    {
        ErrorLn(__FUNCTION__": failed! pMailService == NULL");
        return;
    }

    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeCenter");
        return;
    }
    ISchemeLegendCup *pLegendCupScheme = pSchemeCenter->getSchemeLegendCup();
    SSchemeLegendCup* pSchemeConfig = pLegendCupScheme->getConfig(m_LegendCupBaseInfo.nCreateConfigID);
    if (pSchemeConfig == NULL)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeConfig nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID);
        return;

    }

    SSchemeLegendCupDetail* pDetailScheme= pLegendCupScheme->getDetailInfo(pSchemeConfig->m_nDetailID, m_LegendCupBaseInfo.nCurrentRound);
    if (pDetailScheme == NULL)
    {
        ErrorLn(__FUNCTION__": failed! can't find roundID nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID<<" m_dwDetailID ="<< pSchemeConfig->m_nDetailID<<" kinCount = "<<m_mapRegisterKin.size());
        return;
    }

    ISchemeMailDataCenter *pSchemeMailDataCenter = pSchemeCenter->getSchemeMailDataCenter();
	if (NULL == pSchemeMailDataCenter)
	{
		ErrorLn(__FUNCTION__" nullptr == pSchemeMailDataCenter");
		return;
	}

    SMailConfig* pMailConfig = pSchemeMailDataCenter->getConfig(emMailFill_LegendCupEveryValidWarPrize);
    if( NULL == pMailConfig)
    {
        ErrorLn(__FUNCTION__" nullptr == pMailConfig");
		return;
    }
    
    for (list<DWORD>::iterator iter = dwKinList.begin(); iter != dwKinList.end(); ++iter)
    {
        // 找到参赛成员 发放奖励
        for (map<DWORD, SLegendCupMemberInfo>::iterator iterMember = m_CupMemberInfo.begin(); iterMember != m_CupMemberInfo.end(); ++iterMember)
        {
            if (iterMember->second.dwKinID == *iter)
            {
                // 发放奖励
                SMailSendData mailInfo;
                mailInfo.nType = emMailType_System;							// 邮件类型
                mailInfo.nSourceType = emMailSourceType_System;				// 资源来源类型
                sstrcpyn(mailInfo.szSenderName, pMailConfig->szSenderName, sizeof(mailInfo.szSenderName));// 发件者姓名
                mailInfo.dwMasterPDBID = iterMember->first;
                sstrcpyn(mailInfo.szTitle, pMailConfig->szTitle, sizeof(mailInfo.szTitle));

                int nIndex = 0;
                for(int n =0; n < EVERY_MATCH_PRIZE_ID_MAX_SIZE; ++n)
                {
                    int nPrizeID = pDetailScheme->m_nPrizeIDList[n];
                    if(nPrizeID > 0)
                    {
                        mailInfo.nPlusGoodsNum++;
                        DBREQ_PARAM_MAILGOODS GoodInfo;
                        GoodInfo.dwGoodsID = nPrizeID;
                        GoodInfo.nCount = 1;
                        mailInfo.sGoodsList[nIndex++] = GoodInfo;
                    }
                }

                if (mailInfo.nPlusGoodsNum> 0)
                {
                    pMailService->sendMail(mailInfo,emMailFill_LegendCupEveryValidWarPrize);
                }
            }
        }
    }
}

DWORD CLegendCup::getRoundEndResetTime(int nRoundID)
{
    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeCenter");
        return 0;
    }

    ISchemeLegendCup* pLegendCupScheme = pSchemeCenter->getSchemeLegendCup();
    SSchemeLegendCup* pSchemeConfig = pLegendCupScheme->getConfig(m_LegendCupBaseInfo.nCreateConfigID);
    if (pSchemeConfig == NULL)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeConfig nCreateConfigID = "<<m_LegendCupBaseInfo.nCreateConfigID);
        return 0;

    }

    SSchemeLegendCupDetail* pSchemeDetailConfig = pLegendCupScheme->getDetailInfo(pSchemeConfig->m_nDetailID, nRoundID);
    if (pSchemeDetailConfig == NULL)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeConfig nDetailConfigID = "<<pSchemeConfig->m_nDetailID<<" nRound ="<<nRoundID);
        return 0;
    }

    SRoundTimeInfo* pTimeInfo = getRoundTimeInfo(nRoundID);
    if (pTimeInfo == NULL)
    {
        WarningLn(__FUNCTION__": failed! nDetailConfigID = "<<pSchemeConfig->m_nDetailID<<" nRound ="<<nRoundID);
        return 0;
    }

    time_t tRoundEndTime = pTimeInfo->dwRoundEndTime;  
    tm tmEnd;
    localtime_s(&tmEnd, &tRoundEndTime);
    // 得到可重置最终时间点
    tmEnd.tm_hour = pSchemeDetailConfig->m_nResetEndTime[0];
    tmEnd.tm_min = pSchemeDetailConfig->m_nResetEndTime[1];
    tmEnd.tm_sec = pSchemeDetailConfig->m_nResetEndTime[2];
    DWORD dwEndResetTime = (DWORD)mktime(&tmEnd);

    return dwEndResetTime;
}

int CLegendCup::getCurrentRoundID()
{
    return m_LegendCupBaseInfo.nCurrentRound;
}

void CLegendCup::sendCurRoundInfo2Viewer()
{
    map<DWORD,bool> mapViewer;
    getAllViewerList(mapViewer);
    if (mapViewer.size() <= 0)
    {
        return;
    }
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver == NULL)
    {
        return;
    }

    if (m_LegendCupBaseInfo.nCurrentRound == 0)
    {
        return;
    }

    map<int, ICompetitionRoundBase*>::iterator itRound = m_mapCompetitionRound.find(m_LegendCupBaseInfo.nCurrentRound);
    if (itRound == m_mapCompetitionRound.end())
    {
        return;
    }

    for (map<DWORD,bool>::iterator iterViewer = mapViewer.begin(); iterViewer != mapViewer.end() ;++iterViewer)
    {
       
        SSharePersonProperty shareProperty= pShareReceiver->GetSharePerson(iterViewer->first);
        if(shareProperty.clientID == INVALID_CLIENT)
        {
           continue;
        }

        updateSingleCupBaseInfo2Client(shareProperty.clientID, shareProperty.dwKinID);
        itRound->second->updateSingleCupNodeInfo2Client(shareProperty.clientID, shareProperty.dwKinID);
    }
}

bool CLegendCup::preEndRegisterState(DWORD dwActorID)
{
    if (dwActorID != m_LegendCupBaseInfo.dwCreataActorID)
    {
        g_ExternalFacade.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_NOT_CREATER_CANT_END_REGISTER,"");
    }

    if (m_LegendCupBaseInfo.bIsOnlineCup)
    {
        g_ExternalFacade.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_ONLINE_NOT_PRE_END_REGISTER,"");
        return false;
    }

    if (m_LegendCupBaseInfo.eCupState != SATE_REGISTER)
    {
        g_ExternalFacade.showSystemMessage(dwActorID, CHAT_TIP_LEGENDCUP_OFFLINE_NOT_IN_REGISTER_STATE,"");
        return false;
    }

    EmphasisLn(__FUNCTION__": onTriggerEndRegister()! ActorID ="<<dwActorID<<" szCupName="<< m_LegendCupBaseInfo.szCupName);
    onTriggerEndRegister();
    sendRegisterList2AllViewer();
    return true;
}


void CLegendCup::sendAllRoundInfo2Client(ClientID clientID)
{

    // 先发送所有参赛成员数据
    sendAllKinMemberInfo2Client(clientID);

    // 所有比赛时间数据
    sendAllRoundTimeInfo2Client(clientID);

    // 根据配置创建的多少轮的比赛类
    sendAllCompetionNodeInfo2Client(clientID);
}


void CLegendCup::sendAllKinMemberInfo2Client(ClientID clientID)
{
    // 先发送所有参赛成员数据
    obuf obAllMember;
    SNetMsgHead MemberNetMsgHead;
    MemberNetMsgHead.bySrcEndPoint	= MSG_ENDPOINT_SOCIAL;
    MemberNetMsgHead.byDestEndPoint	= MSG_ENDPOINT_CLIENT;
    MemberNetMsgHead.byKeyModule	= MSG_MODULEID_LEGENDCUP;
    MemberNetMsgHead.byKeyAction	= OC_CUP_SEND_ALL_KIN_MEMBERINFO;
    obAllMember<<MemberNetMsgHead;

    SMsgAllMemebrHead memberHead;
    memberHead.llLegendCupID = m_LegendCupBaseInfo.llLegendCupID;
    memberHead.nMemberCount = m_CupMemberInfo.size();
    // 压入头信息
    obAllMember<<memberHead;

    // 比赛成员数据
    map<DWORD, SLegendCupMemberInfo>::iterator iterMember = m_CupMemberInfo.begin();
    for (;iterMember != m_CupMemberInfo.end(); ++iterMember)
    {
        obAllMember<<iterMember->second;
    }
    // 所有成员信息发送到客户端
    send2Client(clientID, obAllMember.data(), obAllMember.size());
}

void CLegendCup::sendAllRoundTimeInfo2Client(ClientID clientID)
{
    obuf obTime;
    SNetMsgHead sTimeNetMsgHead;
    sTimeNetMsgHead.bySrcEndPoint	= MSG_ENDPOINT_SOCIAL;
    sTimeNetMsgHead.byDestEndPoint	= MSG_ENDPOINT_CLIENT;
    sTimeNetMsgHead.byKeyModule	    = MSG_MODULEID_LEGENDCUP;
    sTimeNetMsgHead.byKeyAction	    = OC_CUP_SEND_ALL_ROUNDTIME;
    obTime<<sTimeNetMsgHead;

    SMsgAllTimeHead sTimeHead;
    sTimeHead.llLegendCupID = m_LegendCupBaseInfo.llLegendCupID;
    sTimeHead.nCount = m_mapRoundBeginTime.size();
    // 压入头信息
    obTime<<sTimeHead;

    // 比赛成员数据
    map<int, SRoundTimeInfo>::iterator iterTime = m_mapRoundBeginTime.begin();
    for (;iterTime != m_mapRoundBeginTime.end(); ++iterTime)
    {
        obTime<<iterTime->second;
    }
    // 所有成员信息发送到客户端
    send2Client(clientID, obTime.data(), obTime.size());
}

void CLegendCup::sendAllCompetionNodeInfo2Client(ClientID clientID)
{
    obuf ob;
    SNetMsgHead NetMsgHead;
    NetMsgHead.bySrcEndPoint	= MSG_ENDPOINT_SOCIAL;
    NetMsgHead.byDestEndPoint	= MSG_ENDPOINT_CLIENT;
    NetMsgHead.byKeyModule		= MSG_MODULEID_LEGENDCUP;
    NetMsgHead.byKeyAction		= OC_CUP_SEND_ALL_ROUNDINFO;
    ob<<NetMsgHead;

    // 打包每轮数据
    // 压入比赛总共有多少轮数据
    SMsgAllRoundHead sRoundHead;
    sRoundHead.llLegendCupID = m_LegendCupBaseInfo.llLegendCupID;
    sRoundHead.nRoundCount = m_LegendCupBaseInfo.nCurrentRound;

    // 每轮比赛数据
    obuf competitionNodeBuf;
    map<int,ICompetitionRoundBase*>::iterator iter =  m_mapCompetitionRound.begin(); // 比赛轮数相关
    for(;iter != m_mapCompetitionRound.end(); ++iter)
    {
        // 打包已经比赛的每轮比赛的信息给客户端
        if (iter->first <= m_LegendCupBaseInfo.nCurrentRound)
        {
            sRoundHead.nAllNodeCount += iter->second->competitionNode2Client(competitionNodeBuf);
        }
    }

    if (competitionNodeBuf.size() == 0)
    {
        return;
    }

    // 压入头信息
    ob<<sRoundHead;

    //压入节点信息
    ob.push_back(competitionNodeBuf.data(), competitionNodeBuf.size());
    send2Client(clientID, ob.data(), ob.size());
}
