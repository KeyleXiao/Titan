//==========================================================================
/**
* @file	  : CCompetitionRoundBase.cpp
* @author : 秦其勇
* created : 2010-4-29
* purpose : 杯赛小组赛相关
*/
//==========================================================================
#include "stdafx.h"
#include "CompetitionRoundBase.h"
#include "SharePropertyDef.h"
#include "IShareServer.h"
#include "IGatewayAcceptorService.h"
#include "MatchHelper.h"
#include "IMatchService.h"
#include "KinHelper.h"
#include "IKinService.h"
#include "OssLogDef.h"
#include "IOSSLogService.h"
#include "ChatHelper.h"

CCompetitionRoundBase::CCompetitionRoundBase()
    : m_llLegendCupID(0)
    , m_nRoundID(0)
    , m_nMaxGroup(0)
    , m_nGroupMaxKin(0)
    , m_nGroupPromotionCount(0)

{	
    m_CompetitionNodeWarInfo.clear();
    m_CompetitionNodeList.clear();
    map_RoundGroup.clear();
}

CCompetitionRoundBase::~CCompetitionRoundBase()
{

}

bool CCompetitionRoundBase::Create(CLegendCup* pLegendCup, SCreateRoundBaseInfo sCreateInfo)
{
    // 初始化基本信息
    if (pLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__": pLegendCup == NULL llLegendCupID ="<<sCreateInfo.llLegendCupID<<" nRoundID ="<<sCreateInfo.nRoundID);
        return false;
    }

    m_pLegendCup = pLegendCup;
    m_llLegendCupID = sCreateInfo.llLegendCupID;
    m_nRoundID = sCreateInfo.nRoundID;
    m_nMaxGroup = sCreateInfo.nMaxGroup;
    m_nGroupMaxKin = sCreateInfo.nGroupMaxKin;
    m_nGroupPromotionCount = sCreateInfo.nGroupPromotionCount;
    m_nDetailConfigID = sCreateInfo.nDetailConfigID;
    m_eCompetitionType = (ERoundCompetitionType)sCreateInfo.byRoundCompetitionType;
    m_byBoType = sCreateInfo.byBoType;
    

    IDBEngineService* pBEngineService = gSocialGlobal->getDBEngineService();
    if (pBEngineService != NULL)
    {
        pBEngineService->registerHandler(this);
    }

	return true;
}

bool CCompetitionRoundBase::CreateFromDB(CLegendCup* pLegendCup, SCreateRoundBaseInfo sCreateInfo)
{
    if (pLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__": pLegendCup == NULL llLegendCupID ="<<sCreateInfo.llLegendCupID<<" nRoundID ="<<sCreateInfo.nRoundID);
        return false;
    }

    m_pLegendCup = pLegendCup;
    m_llLegendCupID = sCreateInfo.llLegendCupID;
    m_nRoundID = sCreateInfo.nRoundID;
    m_nMaxGroup = sCreateInfo.nMaxGroup;
    m_nGroupMaxKin = sCreateInfo.nGroupMaxKin;
    m_nGroupPromotionCount = sCreateInfo.nGroupPromotionCount;
    m_nDetailConfigID = sCreateInfo.nDetailConfigID;
    m_eCompetitionType = (ERoundCompetitionType)sCreateInfo.byRoundCompetitionType;
    m_byBoType = sCreateInfo.byBoType;
    return true;
}

void CCompetitionRoundBase::Release()
{
    IDBEngineService* pBEngineService = gSocialGlobal->getDBEngineService();
    if (pBEngineService != NULL)
    {
        pBEngineService->unregisterHandler(this);
    }

	delete this;
}

int CCompetitionRoundBase::buildSerchID(int nRound, int nGroup, int nDay, int pos)
{
    return nRound* 10000 + nGroup*1000 + nDay *100 + pos;
}

bool CCompetitionRoundBase::buildAllCompetitionNode()
{
    IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        ErrorLn(__FUNCTION__": pDBEngine == NULL");
        return false; 
    }
    
    if (map_RoundGroup.size() <= 0)
    {
        ErrorLn(__FUNCTION__": map_RoundGroup.size() <= 0");
        return false;
    }

    m_CompetitionNodeList.clear();
    map<int, map<int, DWORD>>::iterator iterCompetition = map_RoundGroup.begin();
    for (; iterCompetition != map_RoundGroup.end(); ++iterCompetition)
    {
        map<int, DWORD> mapGroupKin = iterCompetition->second;
        // 每组出线判定 如果出线数量 大于本次每组战队数量 则直接晋级下一轮
        if (mapGroupKin.size() > m_nGroupPromotionCount)
        {
            // 生成比赛节点
            buildGroupCompetitionNode(iterCompetition->first, mapGroupKin);
        }
    }

    // 比赛节点信息保存到数据库
    if (m_CompetitionNodeList.size() <= 0)
    {
        ErrorLn(__FUNCTION__": m_CompetitionNodeList.size() <= 0 " );
        return true;
    }

    obuf ob;
    DBREQ_REQUEST_SAVE_CUP_COMPETITION_NODE dbPara;
    dbPara.dwWorldID = getThisGameWorldID();
    dbPara.llLegendCupID = m_llLegendCupID;
    dbPara.nCount = m_CompetitionNodeList.size();
    ob<<dbPara;
    // 变长 增加4个字节的buff长度
    int nBufSize = dbPara.nCount * sizeof(SCompetitionNode);
    ob<<nBufSize;
    
    for (map<int,SCompetitionNode>::iterator iterDB = m_CompetitionNodeList.begin();iterDB != m_CompetitionNodeList.end(); ++iterDB)
    {
        ob<<iterDB->second;
    }

    if (pDBEngine)
    {
        if (pDBEngine->executeSP(GAMEDB_REQUEST_SAVE_CUP_COMPETITION_NODE, 0, ob.data(),ob.size(),static_cast<IDBRetSink *>(this)) == false)
        {
            ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_SAVE_CUP_COMPETITION_NODE failed!");
            return false;
        }
    }
    return true;
}


bool CCompetitionRoundBase::buildGroupCompetitionNode(int GroupID, map<int, DWORD> mapAllKins)
{

    // 生成某一组的节点信息 并保存到数据库
    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeCenter");
        return false;
    }
    ISchemeLegendCup* pLegendCupScheme = pSchemeCenter->getSchemeLegendCup();
    SSchemeLegendCupDetail* pSchemeDetailConfig = pLegendCupScheme->getDetailInfo(m_nDetailConfigID, m_nRoundID);
    if (pSchemeDetailConfig == NULL)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeConfig nDetailConfigID = "<<m_nDetailConfigID);
        return false;

    }

    // 生成所有比赛节点 
    int nKinCount = mapAllKins.size();

    //不满两组的
    if (mapAllKins.size() % 2 != 0)
    {
        // 添加一个轮空补位的
        mapAllKins[nKinCount +1] = 0;
    }
    /////////////////////////////////轮转分组算法/////////////////////////////////////////
    map<int, map<int, DWORD>> mapDayList;
    for (int i = 1; i <= mapAllKins.size() -1; ++i)
    {
        map<int, DWORD> temp;
        for (int j = 1; j <= mapAllKins.size() / 2; j++)  
        { 
            temp.insert(pair<int, DWORD>( 2*j -1, mapAllKins[j]));
            temp.insert(pair<int, DWORD>( 2*j, mapAllKins[mapAllKins.size() - j + 1])); 
        }

        mapDayList.insert(pair<int,map<int, DWORD>>(i,temp));

        // 淘汰赛不用打循环赛 分配了之后直接跳出
        if (m_eCompetitionType == emType_KnockoutCompetition)
        {
            break;
        }

        // 保证第一个位置不动
        for (int i = mapAllKins.size(); i >  2; --i)
        {
            // 把最后一个值排到2号位置
            swap(mapAllKins[i], mapAllKins[i - 1]);
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////

    //要多少天才能打完
    int nDay = 1;
    for (map<int, map<int, DWORD>>::iterator iter =mapDayList.begin();iter != mapDayList.end(); ++iter)
    {
        // 取两个队伍出来生成节点 如果有0的则轮空
        map<int, DWORD> mapKinList = iter->second;
        for (int i = 1; i <= mapKinList.size() / 2; ++i)
        {
            DWORD dwKin1 = mapKinList[2*i-1];
            DWORD dwKin2 = mapKinList[2*i];
            // 轮空
            bool bBye = false;
            if (dwKin1 ==0 || dwKin2 == 0)
            {
                bBye = true;
                // 小组赛轮空不生成比赛节点
                if (m_eCompetitionType == emType_GroupCompetition)
                {
                    continue;
                }
            }

            // 创建比赛节点相关
            SCompetitionNode Node;
            Node.nSerchID               = buildSerchID(m_nRoundID, GroupID, nDay,i); // 生成的快速定位ID
            Node.byCompetitionType      = m_eCompetitionType;  // 1小组赛 2淘汰赛 ERoundCompetitionType
            Node.nGroupID               = GroupID;                  // 分配到第几组
            Node.nRoundID               = m_nRoundID;               // 第小组赛默认打一轮
            Node.nDay                   = nDay;                     // 第几天
            Node.nPos                   = i;                        // 每天的比赛的位置索引 从1开始
            Node.dwKin1                 = dwKin1;                  // 比赛战队1ID
            Node.nKin1Score             = 0;                        // 战队1本次比赛分数
            Node.dwKin2                 = dwKin2;                  // 比赛战队2ID
            Node.nKin2Score             = 0;                        // 战队2本次比赛分数
            Node.nCurrentBo             = 0;                        // 第几场 bo1 bo3 bo5
            Node.byBoType               = m_byBoType;               // ELegendCupBO

            if (bBye)
            {
                Node.eNodeState = emNodeState_End;          // 是否结束 有轮空则结束
                Node.dwEndTime = (DWORD)time(NULL);         // 结束时间
                if (dwKin1 == 0)
                {
                    Node.dwWinnerKinID = dwKin2;            // 轮空胜利战队ID
                }
                if( dwKin2 == 0)
                {
                    Node.dwWinnerKinID = dwKin1;            // 轮空胜利战队ID
                }
            }
            else
            {
                Node.eNodeState = emNodeState_WaitStart;    // 直接创建房间等待开始
                Node.dwWinnerKinID = 0;                     // 胜利战队ID
            }

            // 计算比赛节点开始时间 分线上线下
            // 线上则需要计算进入房间的时间
            if (m_pLegendCup->getLegendCupBaseInfo()->bIsOnlineCup)
            {
                // 得到个节点开始房间的时间 然后AllCompetitionNodeSetTimer统一创建相关定时器
               SRoundTimeInfo* pRoundTimeInfo = m_pLegendCup->getRoundTimeInfo(m_nRoundID);
               if (pRoundTimeInfo == NULL)
               {
                   Node.dwBeginTime = 0;
               }
               else
               {
                   Node.dwBeginTime = pRoundTimeInfo->dwRoundCreateRoomTime + (nDay - 1)*24*60*60;
               }
            }
            else
            {
                Node.dwBeginTime = 0;
            }

            // 添加到本地节点信息列表
            m_CompetitionNodeList.insert(pair<int,SCompetitionNode>(Node.nSerchID, Node));
        }
        nDay++;
    }

    return true;
}


bool CCompetitionRoundBase::AllCompetitionNodeCreateRoom()
{
    MatchHelper matchHelper;
    IMatchService *pMatchService = matchHelper.m_ptr;
    if (pMatchService == NULL)
    {
        ErrorLn(__FUNCTION__":can't find pMatchService == NULL");
        return false;
    }

    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeCenter");
        return false;
    }
    ISchemeLegendCup* pLegendCupScheme = pSchemeCenter->getSchemeLegendCup();
    SSchemeLegendCupDetail* pSchemeDetailConfig = pLegendCupScheme->getDetailInfo(m_nDetailConfigID, m_nRoundID);
    if (pSchemeDetailConfig == NULL)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeConfig nDetailConfigID = "<<m_nDetailConfigID);
        return false;
    }
    
    DWORD dwNowTime = (DWORD)time(NULL);
    TraceLn(__FUNCTION__": Create legendcup Room begin! llLegendCupID="<<m_pLegendCup->getLegendCupID());
    map<int, SCompetitionNode>::iterator iter = m_CompetitionNodeList.begin();
    for (; iter != m_CompetitionNodeList.end(); ++iter)
    {
        SCompetitionNode* pNode = &(iter->second);
        // 等待裁决或者是结束了则不创建房间了
        if (pNode->eNodeState == emNodeState_End || pNode->eNodeState == emNodeState_WaitJudge)
        {
            continue;
        }
        
        // 线上没有打完则创建房间
        createCompetitionNodeRoom(pNode);
    }
    TraceLn(__FUNCTION__": Create legendcup Room end!"<<m_pLegendCup->getLegendCupID());
    return true;
}

bool CCompetitionRoundBase::recordCupWarResult(SMsgRecordNodeWarResult* pMsgInfo)
{
    if (m_pLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__" m_pLegendCup == NULL");
        return false;
    }

    // 有效比赛奖励
    if (pMsgInfo->llWarID > 0)
    {
        list<DWORD> dwKinList;
        dwKinList.push_back(pMsgInfo->dwKin1ID);
        dwKinList.push_back(pMsgInfo->dwKin2ID);

        m_pLegendCup->distributionEveryValidWarPrize(dwKinList);
    }

    // 记录结果
    if (m_pLegendCup->getLegendCupBaseInfo()->bIsOnlineCup)
    {
        // 线上记录比赛结果
        return onlineRecordWarResult(pMsgInfo);
    }
    else
    {
        // 线下记录比赛
        return offlineRecordWarResult(pMsgInfo);
    }
    

    return true;
}

bool CCompetitionRoundBase::setLegendCupNodeState(SMsgSetCompetitionNodeState* sNodeState)
{
    if (m_pLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__" m_pLegendCup == NULL");
        return false;
    }

    map<int, SCompetitionNode>::iterator iter = m_CompetitionNodeList.find(sNodeState->nSerchID);
    if(m_CompetitionNodeList.find(sNodeState->nSerchID) == m_CompetitionNodeList.end())
    {
        ErrorLn(__FUNCTION__":can't find nSerchID ="<<sNodeState->nSerchID);
        return false;
    }
    updateNodeState(&(iter->second), sNodeState->eState);
    
    return true;
}


bool CCompetitionRoundBase::onlineRecordWarResult(SMsgRecordNodeWarResult* pMsgInfo)
{
    MatchHelper matchHelper;
    IMatchService *pMatchService = matchHelper.m_ptr;
    if (pMatchService == NULL)
    {
        ErrorLn(__FUNCTION__":can't find pMatchService == NULL");
        return false;
    }
    // 线上比赛结果记录
    map<int, SCompetitionNode>::iterator iter = m_CompetitionNodeList.find(pMsgInfo->nSerchID);
    if(m_CompetitionNodeList.find(pMsgInfo->nSerchID) == m_CompetitionNodeList.end())
    {
        ErrorLn(__FUNCTION__":can't find nSerchID ="<<pMsgInfo->nSerchID);
        return false;
    }

    // 移除房间
    if(m_mapSerchID2RoomID.find(pMsgInfo->nSerchID) != m_mapSerchID2RoomID.end())
    {
        TraceLn(__FUNCTION__":removeRoom nSerchID ="<<pMsgInfo->nSerchID<<" llLegendCupID="<<m_llLegendCupID);
        pMatchService->removeLegendCupRoom(m_mapSerchID2RoomID[pMsgInfo->nSerchID]);
        m_mapSerchID2RoomID.erase(pMsgInfo->nSerchID);
    }

    SCompetitionNode* pNodeInfo = &(iter->second);
    SNodeWarRecord sNodeWarRecord;
    sNodeWarRecord.byWinFlag = pMsgInfo->byJudgeFlag;       // 胜利标志 EJudgeNodeWar
    sNodeWarRecord.llWarID = pMsgInfo->llWarID;             // 战场ID
    sNodeWarRecord.dwWinnerKin = pMsgInfo->dwWinnerKinID;   // 战场胜利战队
    sNodeWarRecord.nBoCount = iter->second.nCurrentBo = pMsgInfo->nBoCount;

    map<int,SNodeWarRecord>::iterator iterWarRec = m_CompetitionNodeWarInfo[pMsgInfo->nSerchID].find(sNodeWarRecord.nBoCount);
    if (iterWarRec == m_CompetitionNodeWarInfo[pMsgInfo->nSerchID].end())
    {
        ErrorLn(__FUNCTION__": can't find warRec!! LegendCupID"<< m_llLegendCupID<<" nSerchID="<< pMsgInfo->nSerchID<<" nBoCount ="<<sNodeWarRecord.nBoCount);
        return false;
    }

    // 更新记录信息
    iterWarRec->second = sNodeWarRecord;

    LEDENDCUP_TRACELN("----------------------------------------["<<m_pLegendCup->getLegendCupBaseInfo()->szCupName<<"]onlineRecordWarResult----------------------------");
    LEDENDCUP_TRACELN(__FUNCTION__": legendCupID = "<<m_llLegendCupID<<" serchID ="<<pNodeInfo->nSerchID<<" BoCount="<<sNodeWarRecord.nBoCount);
    LEDENDCUP_TRACELN("byWinFlag = "<<sNodeWarRecord.byWinFlag);       // 胜利标志 EJudgeNodeWar
    LEDENDCUP_TRACELN("llWarID = "<<pMsgInfo->llWarID);                // 战场ID
    if (pMsgInfo->dwWinnerKinID > 0)
    {
        LEDENDCUP_TRACELN("dwWinnerKin = "<<pMsgInfo->dwWinnerKinID);      // 战场胜利战队
    }
    else
    {
        LEDENDCUP_WARNINGLN("dwWinnerKin = "<<pMsgInfo->dwWinnerKinID);// 战场胜利战队
    }
    LEDENDCUP_TRACELN("nBoCount = "<<sNodeWarRecord.nBoCount);       // 第几场
    LEDENDCUP_TRACELN("nDay = "<<pNodeInfo->nDay);                // 第几天
    LEDENDCUP_TRACELN("nRoundID = "<<pNodeInfo->nRoundID);        // 第几轮比赛
    LEDENDCUP_TRACELN("nGroupID = "<<pNodeInfo->nGroupID);        // 第几组
    LEDENDCUP_TRACELN("nPos = "<<pNodeInfo->nPos);                // 位置索引 从1开始
    LEDENDCUP_TRACELN("-----------------------------------------------------------------------------------------------------------------------");

    if (pMsgInfo->dwWinnerKinID > 0)
    {
        // 记录本次KDA
        pNodeInfo->dwKin1TotalKda += pMsgInfo->dwKin1KDA;
        pNodeInfo->dwKin2TotalKda += pMsgInfo->dwKin2KDA;

        if (pNodeInfo->dwKin1 == pMsgInfo->dwWinnerKinID)
        {
            pNodeInfo->nKin1Score++;

            if (pNodeInfo->nKin1Score >= getWinScoreByType(pNodeInfo->byBoType))
            {
                // 该节点比赛已经可以结束了
                updateNodeState(pNodeInfo,emNodeState_End);   // 已经结束          
            }
        }

        if (pNodeInfo->dwKin2 == pMsgInfo->dwWinnerKinID)
        {
            pNodeInfo->nKin2Score++;
            if (pNodeInfo->nKin2Score >= getWinScoreByType(pNodeInfo->byBoType))
            {
                // 该节点比赛已经可以结束了
                updateNodeState(pNodeInfo,emNodeState_End);   // 已经结束
            }
        }
    }
    else
    {
        // 两个队都没有参加 直接结束比赛节点
        updateNodeState(pNodeInfo,emNodeState_End);            // 已经结束             
    }

    if (pNodeInfo->eNodeState == emNodeState_End)
    {
        if (pMsgInfo->dwWinnerKinID > 0)
        {
            pNodeInfo->dwWinnerKinID = pMsgInfo->dwWinnerKinID;
            // 修改队伍得分
            SCupKinInfo* pKinInfo = m_pLegendCup->getCupKinInfo(pNodeInfo->dwWinnerKinID);
            if (pKinInfo == NULL)
            {
                // 队伍都找不到了 这个问题很严重
                ErrorLn(__FUNCTION__": can't find winnerID = "<<pMsgInfo->dwWinnerKinID);
                return false;
            }
            // 打完节点比赛 杯赛战队得分
            if (m_eCompetitionType == emType_GroupCompetition)
            {
                pKinInfo->nGroupScore++;
            }

            if (m_eCompetitionType == emType_KnockoutCompetition)
            {
                pKinInfo->nCurKnockOutScore++;
            }

            // 打完节点比赛 更新杯赛战队分数
            IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
            if (pDBEngine == NULL)
            {
                ErrorLn(__FUNCTION__": pDBEngine == NULL");
                return false; 
            }

            DBREQ_REQUEST_UPDATE_CUP_KININFO dbParam;
            dbParam.dwWorldID   = getThisGameWorldID();
            dbParam.llLegendCupID = m_llLegendCupID;                      // 杯赛ID
            dbParam.dwKinID = pKinInfo->dwKinID;                          // 战队ID
            dbParam.nGroupScore = pKinInfo->nGroupScore;                  // 小组赛积分情况 
            dbParam.nCurKnockOutScore = pKinInfo->nCurKnockOutScore;      // 当前淘汰赛积分情况 bo1 bo3 bo5

            if (pDBEngine->executeSP(GAMEDB_REQUEST_UPDATE_CUP_KININFO,0,(LPCSTR)&dbParam,sizeof(dbParam),static_cast<IDBRetSink *>(this)) == false)
            {
                ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_UPDATE_CUP_KININFO failed!");
                return false;
            }
        }
    }
    else
    {
        // 线上没有打完则创建房间
        createCompetitionNodeRoom(pNodeInfo);
    }

    return saveCompetitionNodeToDB(pNodeInfo);
    
}

bool CCompetitionRoundBase::offlineRecordWarResult(SMsgRecordNodeWarResult* pMsgInfo)
{
    // 线下比赛记录
    map<int, SCompetitionNode>::iterator iter = m_CompetitionNodeList.find(pMsgInfo->nSerchID);
    if(m_CompetitionNodeList.find(pMsgInfo->nSerchID) == m_CompetitionNodeList.end())
    {
        ErrorLn(__FUNCTION__":can't find nSerchID ="<<pMsgInfo->nSerchID);
        return false;
    }

    // 线下打完之后
    // 移除房间
    MatchHelper matchHelper;
    IMatchService *pMatchService = matchHelper.m_ptr;
    if (pMatchService == NULL)
    {
        ErrorLn(__FUNCTION__":can't find pMatchService == NULL");
        return false;
    }

    if(m_mapSerchID2RoomID.find(pMsgInfo->nSerchID) != m_mapSerchID2RoomID.end())
    {
        TraceLn(__FUNCTION__":removeRoom nSerchID ="<<pMsgInfo->nSerchID<<" llLegendCupID="<<m_llLegendCupID);
        pMatchService->removeLegendCupRoom(m_mapSerchID2RoomID[pMsgInfo->nSerchID]);
        m_mapSerchID2RoomID.erase(pMsgInfo->nSerchID);
    }

    // 1、更新m_CompetitionNodeWarInfo 里面记录 2、更新节点信息到等待裁决状态 3、 更新数据库节点信息
    SCompetitionNode* pNodeInfo = &(iter->second);
    SNodeWarRecord sNodeWarRecord;
    sNodeWarRecord.byWinFlag    = JUDGE_NONE;                   // 胜利标志 EJudgeNodeWar
    sNodeWarRecord.llWarID      = pMsgInfo->llWarID;           // 战场ID
    sNodeWarRecord.dwWinnerKin  = pMsgInfo->dwWinnerKinID;      // 战场胜利战队
    sNodeWarRecord.nBoCount    = iter->second.nCurrentBo = pMsgInfo->nBoCount;
    map<int,SNodeWarRecord>::iterator iterWarRec = m_CompetitionNodeWarInfo[pMsgInfo->nSerchID].find(sNodeWarRecord.nBoCount);
    if (iterWarRec == m_CompetitionNodeWarInfo[pMsgInfo->nSerchID].end())
    {
        ErrorLn(__FUNCTION__": can't find warRec!! LegendCupID"<< m_llLegendCupID<<" nSerchID="<< pMsgInfo->nSerchID<<" nBoCount ="<<sNodeWarRecord.nBoCount);
        return false;
    }
    // 更新记录信息
    iterWarRec->second = sNodeWarRecord;

    LEDENDCUP_TRACELN("----------------------------------------------["<<m_pLegendCup->getLegendCupBaseInfo()->szCupName<<"]offlineRecordWarResult-------------------------------------");
    LEDENDCUP_TRACELN(__FUNCTION__": legendCupID = "<<m_llLegendCupID<<" serchID ="<<pNodeInfo->nSerchID<<" BoCount="<<sNodeWarRecord.nBoCount);
    LEDENDCUP_TRACELN("byWinFlag = "<<sNodeWarRecord.byWinFlag);       // 胜利标志 EJudgeNodeWar
    LEDENDCUP_TRACELN("llWarID = "<<pMsgInfo->llWarID);                // 战场ID
    if (pMsgInfo->dwWinnerKinID > 0)
    {
        LEDENDCUP_TRACELN("dwWinnerKin = "<<pMsgInfo->dwWinnerKinID);      // 战场胜利战队
    }
    else
    {
        LEDENDCUP_WARNINGLN("dwWinnerKin = "<<pMsgInfo->dwWinnerKinID);// 战场胜利战队
    }
    LEDENDCUP_TRACELN("nBoCount = "<<sNodeWarRecord.nBoCount);       // 第几场
    LEDENDCUP_TRACELN("nDay = "<<pNodeInfo->nDay);                // 第几天
    LEDENDCUP_TRACELN("nRoundID = "<<pNodeInfo->nRoundID);        // 第几轮比赛
    LEDENDCUP_TRACELN("nGroupID = "<<pNodeInfo->nGroupID);        // 分配到第几组
    LEDENDCUP_TRACELN("nPos = "<<pNodeInfo->nPos);                // 位置索引 从1开始
    LEDENDCUP_TRACELN("-----------------------------------------------------------------------------------------------------------------------");

    // 节点信息设置为裁决状态
    updateNodeState(pNodeInfo,emNodeState_WaitJudge);

    // 通知创办者有比赛已经打完
    notifyCreaterNeedJudge(pNodeInfo->dwKin1, pNodeInfo->dwKin2);
    return saveCompetitionNodeToDB(pNodeInfo);
    
}

void CCompetitionRoundBase::notifyCreaterNeedJudge(DWORD dwKin1, DWORD dwKin2)
{
    DWORD dwCreateActorID = m_pLegendCup->getLegendCupBaseInfo()->dwCreataActorID;
    if (dwCreateActorID == INVALID_PDBID)
    {
        return;
    }
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
	if(pShareReceiver != NULL)
	{
		return;
	}
    SSharePersonProperty shareProperty= pShareReceiver->GetSharePerson(dwCreateActorID);
    // 在战场里面的玩家不下发
    if( shareProperty.dwPDBID == INVALID_PDBID || shareProperty.dwSceneID > MAP_MAXCOUNT)
    {
        return;
    }

    KinHelper helper;
    IKinService *pKinService = helper.m_ptr;
    if (pKinService != NULL)
    {
        SKinInfo sKin1Info = pKinService->getKinInfo(dwKin1);
        SKinInfo sKin2Info = pKinService->getKinInfo(dwKin2);
        char szParam[128] = { 0 };
        ssprintf_s(szParam, sizeof(szParam), "%s;%s", sKin1Info.szName, sKin2Info.szName);
        g_ExternalFacade.showSystemMessage(dwCreateActorID, CHAT_TIP_LEGENDCUP_NEED_JUDGED_MSGBOX_INFO,szParam );
    }
}

bool CCompetitionRoundBase::judgeNodeWarResult(SMsgJudgeWarResult* pJudgeInfo)
{
    // 裁定比赛  
    if (m_pLegendCup->getLegendCupBaseInfo()->bIsOnlineCup)
    {
        //线上裁定比赛
        return onlinejudgeNodeWarResult(pJudgeInfo);
    }
    else
    {
        //线下裁定比赛
        return offlinejudgeNodeWarResult(pJudgeInfo);
    }
}


// 线上裁定比赛
bool CCompetitionRoundBase::onlinejudgeNodeWarResult(SMsgJudgeWarResult* pJudgeInfo)
{
    MatchHelper matchHelper;
    IMatchService *pMatchService = matchHelper.m_ptr;
    if (pMatchService == NULL)
    {
        ErrorLn(__FUNCTION__":can't find pMatchService == NULL");
        return false;
    }

    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if(pShareReceiver == NULL)
    {
        ErrorLn(__FUNCTION__": pShareReceiver == NULL");
        return false;
    }
    SSharePersonProperty shareProperty = pShareReceiver->GetSharePersonByClientID(pJudgeInfo->clientID);
    if (shareProperty.dwPDBID == INVALID_PDBID)
    {
        ErrorLn(__FUNCTION__": shareProperty.dwPDBID == INVALID_PDBID");
        return false;
    }

    // 如果是系统创建 自动裁决
    map<int, SCompetitionNode>::iterator iter = m_CompetitionNodeList.find(pJudgeInfo->nSerchID);
    if(m_CompetitionNodeList.find(pJudgeInfo->nSerchID) == m_CompetitionNodeList.end())
    {
        ErrorLn(__FUNCTION__":can't find nSerchID ="<<pJudgeInfo->nSerchID);
        return false;
    }

    SCompetitionNode *pNodeInfo = &(iter->second);

    string JudgLog;
    bool bJudgedSuccess = false;
    switch(pJudgeInfo->byJudgeFlag)
    {
    case JUDGE_Reset :
        {
            // 撤销比赛记录处理
            bJudgedSuccess = onlineResetWarRecord(pJudgeInfo, pNodeInfo);
            JudgLog += a2utf8("重置");
        }
        break;

    case JUDGE_SelectWinner :
        {
            // 选择胜出队伍处理
            bJudgedSuccess = onlineJudgeSelectWinner(pJudgeInfo, pNodeInfo);
            JudgLog += a2utf8("选择胜出队伍");
        }
        break;

    case JUDGE_AllKnockOut :
        {
            // 判定双方弃权处理
            bJudgedSuccess = onlineJudgeAllOut(pJudgeInfo, pNodeInfo);
            JudgLog += a2utf8("双方弃权");
        }
        break;

    default:
        {
            WarningLn(__FUNCTION__"can't judge war byJudgeFlag ="<<pJudgeInfo->byJudgeFlag);
            return false;
        }
        break;
    }

    if (bJudgedSuccess)
    {
        // 保存杯赛节点信息相关到数据库
        saveCompetitionNodeToDB(pNodeInfo);

        // 通知客户端更新节点信息
        g_ExternalFacade.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_LEGENDCUP_JUDGED_SUCCESS,"");

        // 更新节点信息到客户端
        updateSingleCupNodeInfo2AllView( pNodeInfo);
        // 刷新战场结果到客户端
        updateCupNodeWarInfo2AllView( pNodeInfo);

        IOSSLogService * pOSSLogServer = gSocialGlobal->getOSSLogService();
        if (pOSSLogServer != NULL )
        {
            KinHelper helper;
            IKinService *pKinService = helper.m_ptr;
            if (pKinService != NULL)
            {
                char szText[512] = {0};
                SKinInfo sKin1Info = pKinService->getKinInfo(pNodeInfo->dwKin1);
                SKinInfo sKin2Info = pKinService->getKinInfo(pNodeInfo->dwKin2);
                string winnerLog = "-";
                string endLog = pNodeInfo->eNodeState == emNodeState_End? a2utf8("已结束"): a2utf8("未结束");
                if(pJudgeInfo->dwWinnerKinID > 0)
                {
                    SKinInfo sKinWinner = pKinService->getKinInfo(pNodeInfo->dwKin2);
                    winnerLog = sKinWinner.szName;
                }

                ssprintf_s(szText, sizeof(szText), a2utf8("裁定结果：[%s] 杯赛 [%s] 裁定[%s]对[%s] 第[%d]场比赛结果为: [%s]  比分: [%d : %d] 胜利队伍: [%s] 结束状态：[%s]")
                    , m_pLegendCup->getLegendCupBaseInfo()->szCupName, m_pLegendCup->getLegendCupBaseInfo()->szCreaterName,sKin1Info.szName,sKin2Info.szName
                    , pJudgeInfo->nBoCount, JudgLog.c_str(), pNodeInfo->nKin1Score, pNodeInfo->nKin2Score,winnerLog.data(), endLog.data());

                pOSSLogServer->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_LEGENDCUP, m_llLegendCupID, szText);

            }
        }
    }

    

    return true;
}

// 线下裁定比赛
bool CCompetitionRoundBase::offlinejudgeNodeWarResult(SMsgJudgeWarResult* pJudgeInfo)
{
    MatchHelper matchHelper;
    IMatchService *pMatchService = matchHelper.m_ptr;
    if (pMatchService == NULL)
    {
        ErrorLn(__FUNCTION__":can't find pMatchService == NULL");
        return false;
    }

    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if(pShareReceiver == NULL)
    {
        ErrorLn(__FUNCTION__": pShareReceiver == NULL");
        return false;
    }
    SSharePersonProperty shareProperty = pShareReceiver->GetSharePersonByClientID(pJudgeInfo->clientID);
    if (shareProperty.dwPDBID == INVALID_PDBID)
    {
        ErrorLn(__FUNCTION__": shareProperty.dwPDBID == INVALID_PDBID");
        return false;
    }

    // 如果是系统创建 自动裁决
    map<int, SCompetitionNode>::iterator iter = m_CompetitionNodeList.find(pJudgeInfo->nSerchID);
    if(m_CompetitionNodeList.find(pJudgeInfo->nSerchID) == m_CompetitionNodeList.end())
    {
        ErrorLn(__FUNCTION__":can't find nSerchID ="<<pJudgeInfo->nSerchID);
        return false;
    }

    SCompetitionNode *pNodeInfo = &(iter->second);
    if (pNodeInfo->eNodeState == emNodeState_Competition ||  pNodeInfo->eNodeState == emNodeState_End)
    { 
        g_ExternalFacade.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_LEGENDCUP_STATE_CANT_JUDGED_WAR,"");
        return false;
    }

    //判定输赢
    map<int, map<int, SNodeWarRecord>>::iterator iterMapWarRec = m_CompetitionNodeWarInfo.find(pJudgeInfo->nSerchID);
    if ( iterMapWarRec == m_CompetitionNodeWarInfo.end())
    {
        ErrorLn(__FUNCTION__":can't find llLegendCupID ="<<m_llLegendCupID<<" nSerchID ="<<pJudgeInfo->nSerchID);
        return false;
    }

    map<int, SNodeWarRecord>::iterator iterWarRec = iterMapWarRec->second.find(pJudgeInfo->nBoCount);
    if (iterWarRec == iterMapWarRec->second.end())
    {
        ErrorLn(__FUNCTION__":can't find llLegendCupID ="<<m_llLegendCupID<<" nSerchID ="<<pJudgeInfo->nSerchID<<" nBoCount"<<pJudgeInfo->nBoCount);
        return false;
    }

    if (iterWarRec->second.byWinFlag > JUDGE_NONE)
    {
        // 已经裁定比赛无法再次修改
        g_ExternalFacade.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_LEGENDCUP_CANT_REPEATE_JUDGED,"");
        return false;
    }

    // 删除对应的房间记录
    if(m_mapSerchID2RoomID.find(pJudgeInfo->nSerchID) != m_mapSerchID2RoomID.end())
    {
        TraceLn(__FUNCTION__":removeRoom nSerchID ="<<pJudgeInfo->nSerchID<<" llLegendCupID="<<m_llLegendCupID);
        pMatchService->removeLegendCupRoom(m_mapSerchID2RoomID[pJudgeInfo->nSerchID]);
        m_mapSerchID2RoomID.erase(pJudgeInfo->nSerchID);
    }

    string JudgLog;
    bool bJudgedSuccess = false;
    switch(pJudgeInfo->byJudgeFlag)
    {
    case JUDGE_Cancle :
        {
            // 撤销比赛记录处理
            bJudgedSuccess = offlineJudgeCancel(pJudgeInfo, pNodeInfo);
            JudgLog += a2utf8("撤销比赛");
        }
        break;

    case JUDGE_AllKnockOut :
        {
            // 判定双方弃权处理
            bJudgedSuccess = offlineJudgeAllOut(pJudgeInfo, pNodeInfo);
            JudgLog += a2utf8("双方弃权");
        }
        break;

    case JUDGE_ConfirmNode :
        {
            // 维持原来的记录信息处理
            bJudgedSuccess = offlineJudgeConfirmNode(pJudgeInfo, pNodeInfo);
            JudgLog += a2utf8("维持原判");
        }
        break;

    case JUDGE_SelectWinner :
        {
            // 选择胜出队伍处理
            bJudgedSuccess = offlineJudgeSelectWinner(pJudgeInfo, pNodeInfo);
            JudgLog += a2utf8("选择胜出队伍");
        }
        break;

    default:
        {
            WarningLn(__FUNCTION__"can't judge war byJudgeFlag ="<<pJudgeInfo->byJudgeFlag);
            return false;
        }
        break;
    }
    
    if (!bJudgedSuccess)
    {
        // 通知客户端更新节点信息
        g_ExternalFacade.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_LEGENDCUP_JUDGED_FAIL,"");
    }
    else
    {
        // 保存杯赛节点信息相关到数据库
        saveCompetitionNodeToDB(pNodeInfo);

        // 更新节点信息到客户端
        updateSingleCupNodeInfo2AllView( pNodeInfo);
        // 刷新战场结果到客户端
        updateCupNodeWarInfo2AllView( pNodeInfo);

        // 通知客户端更新节点信息
        g_ExternalFacade.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_LEGENDCUP_JUDGED_SUCCESS,"");

        // 通知比赛节点双方人员已经裁定完成
        notifyNodeMemberJudgeWar(pNodeInfo);
        IOSSLogService * pOSSLogServer = gSocialGlobal->getOSSLogService();
        if (pOSSLogServer != NULL )
        {
            KinHelper helper;
            IKinService *pKinService = helper.m_ptr;
            if (pKinService != NULL)
            {
                char szText[512] = {0};
                SKinInfo sKin1Info = pKinService->getKinInfo(pNodeInfo->dwKin1);
                SKinInfo sKin2Info = pKinService->getKinInfo(pNodeInfo->dwKin2);
                string winnerLog = "-";
                string endLog = pNodeInfo->eNodeState == emNodeState_End? a2utf8("已结束"): a2utf8("未结束");
                if(pJudgeInfo->dwWinnerKinID > 0)
                {
                    SKinInfo sKinWinner = pKinService->getKinInfo(pNodeInfo->dwKin2);
                    winnerLog = sKinWinner.szName;
                }

                ssprintf_s(szText, sizeof(szText), a2utf8("裁定结果：[%s] 杯赛 [%s] 裁定[%s]对[%s] 第[%d]场比赛结果为: [%s]  比分: [%d : %d] 胜利队伍: [%s] 结束状态：[%s]")
                    , m_pLegendCup->getLegendCupBaseInfo()->szCupName, m_pLegendCup->getLegendCupBaseInfo()->szCreaterName,sKin1Info.szName,sKin2Info.szName
                    , pJudgeInfo->nBoCount, JudgLog.c_str(), pNodeInfo->nKin1Score, pNodeInfo->nKin2Score,winnerLog.data(), endLog.data());

                pOSSLogServer->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_LEGENDCUP, m_llLegendCupID, szText);
            }
        }
    }

    // 线下比赛 进入下一轮判定
    return true;
}

void CCompetitionRoundBase::notifyNodeMemberJudgeWar(SCompetitionNode* pNodeInfo)
{
    // 广播给所有节点参赛者
    ChatHelper chatHelper;
    IChatService *pChatService = chatHelper.m_ptr;
    if (NULL == pChatService)
    {
        ErrorLn(__FUNCTION__ << "failed! nullptr == pChatService" );
        return;
    }

    list<DWORD> KinMember;
    if (pNodeInfo->dwKin1 > 0)
    {
        // 获取参赛队员
        m_pLegendCup->getCupKinMemberList(pNodeInfo->dwKin1, KinMember);
    }

    if (pNodeInfo->dwKin2 > 0)
    {
        // 获取参赛队员
        m_pLegendCup->getCupKinMemberList(pNodeInfo->dwKin2, KinMember);
    }

    if (KinMember.size() <= 0)
    {
        return;
    }

    // 先把基础数据, 当前轮所在节点信息下发下去
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver == NULL)
    {
        ErrorLn(__FUNCTION__": pShareReceiver == NULL");
        return;
    }

    for (list<DWORD>::iterator iter = KinMember.begin(); iter != KinMember.end(); ++iter)
    {
        DWORD dwActorID = *iter;
        SSharePersonProperty shareProperty= pShareReceiver->GetSharePerson(dwActorID);
        // 在战场里面的玩家不下发
        if( shareProperty.dwPDBID == INVALID_PDBID || shareProperty.dwSceneID > MAP_MAXCOUNT)
        {
            continue;
        }
        m_pLegendCup->updateSingleCupBaseInfo2Client(shareProperty.clientID, shareProperty.dwKinID);

        updateSingleCupNodeInfo2Client(shareProperty.clientID, shareProperty.dwKinID);

        // 通知客户端更新节点信息
        g_ExternalFacade.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_LEGENDCUP_JUDGED_NODE_MSGBOX_INFO,"");
    }
}


bool CCompetitionRoundBase::offlineJudgeSelectWinner(SMsgJudgeWarResult* pJudgeInfo, SCompetitionNode* pNodeInfo)
{
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if(pShareReceiver == NULL)
    {
        ErrorLn(__FUNCTION__": pShareReceiver == NULL");
        return false;
    }

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePersonByClientID(pJudgeInfo->clientID);
    if (shareProperty.dwPDBID == INVALID_PDBID)
    {
        ErrorLn(__FUNCTION__": shareProperty.dwPDBID == INVALID_PDBID");
        return false;
    }
    //判定输赢
    map<int, map<int, SNodeWarRecord>>::iterator iterMapWarRec = m_CompetitionNodeWarInfo.find(pJudgeInfo->nSerchID);
    if ( iterMapWarRec == m_CompetitionNodeWarInfo.end())
    {
        ErrorLn(__FUNCTION__":can't find llLegendCupID ="<<m_llLegendCupID<<" nSerchID ="<<pJudgeInfo->nSerchID);
        return false;
    }

    map<int, SNodeWarRecord>::iterator iterWarRec = iterMapWarRec->second.find(pJudgeInfo->nBoCount);

    if (iterWarRec == iterMapWarRec->second.end())
    {
        ErrorLn(__FUNCTION__":can't find llLegendCupID ="<<m_llLegendCupID<<" nSerchID ="<<pJudgeInfo->nSerchID<<" nBoCount"<<pJudgeInfo->nBoCount);
        return false;
    }

    if(pJudgeInfo->dwWinnerKinID <= 0)
    {
        g_ExternalFacade.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_LEGENDCUP_SELECT_RIGHT_KINID,"");
        return false;
    }

    if (pNodeInfo->dwKin1 == pJudgeInfo->dwWinnerKinID)
    {
        pNodeInfo->nKin1Score++;
        // 已经能决出胜负了
        if (pNodeInfo->nKin1Score >= getWinScoreByType(pNodeInfo->byBoType))
        {
            updateNodeState(pNodeInfo,emNodeState_End);   // 修改节点状态
        }
    }

    if (pNodeInfo->dwKin2 == pJudgeInfo->dwWinnerKinID)
    {
        pNodeInfo->nKin2Score++;
        // 已经能决出胜负了
        if (pNodeInfo->nKin2Score >= getWinScoreByType(pNodeInfo->byBoType))
        {
            updateNodeState(pNodeInfo,emNodeState_End);   // 修改节点状态     
        }
    }

    // 更新战场判定状态
    iterWarRec->second.byWinFlag = pJudgeInfo->byJudgeFlag;
    iterWarRec->second.dwWinnerKin = pJudgeInfo->dwWinnerKinID;
    // 修改进行到当前第几轮比赛
    pNodeInfo->nCurrentBo = pJudgeInfo->nBoCount;

    if (pNodeInfo->eNodeState == emNodeState_End)
    {
        pNodeInfo->dwWinnerKinID = pJudgeInfo->dwWinnerKinID;

        // 修改队伍得分
        SCupKinInfo* pKinInfo = m_pLegendCup->getCupKinInfo(pNodeInfo->dwWinnerKinID);
        if (pKinInfo == NULL)
        {
            // 队伍都找不到了 这个问题很严重
            ErrorLn(__FUNCTION__": can't find winnerID = "<<pJudgeInfo->dwWinnerKinID);
            return false;
        }
        // 打完节点比赛 战队加分
        if (m_eCompetitionType == emType_GroupCompetition)
        {
            pKinInfo->nGroupScore++;
        }

        if (m_eCompetitionType == emType_KnockoutCompetition)
        {
            pKinInfo->nCurKnockOutScore++;
        }

        // 打完节点比赛 更新战队分数
        IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
        if (pDBEngine == NULL)
        {
            ErrorLn(__FUNCTION__": pDBEngine == NULL");
            return false; 
        }

        DBREQ_REQUEST_UPDATE_CUP_KININFO dbParam;
        dbParam.dwWorldID = getThisGameWorldID();
        dbParam.llLegendCupID = m_llLegendCupID;                      // 杯赛ID
        dbParam.dwKinID = pKinInfo->dwKinID;                            // 战队ID
        dbParam.nGroupScore = pKinInfo->nGroupScore;                        // 小组赛积分情况 
        dbParam.nCurKnockOutScore = pKinInfo->nCurKnockOutScore;                  // 当前淘汰赛积分情况 bo1 bo3 bo5

        if (pDBEngine->executeSP(GAMEDB_REQUEST_UPDATE_CUP_KININFO,0,(LPCSTR)&dbParam,sizeof(dbParam),static_cast<IDBRetSink *>(this)) == false)
        {
            ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_UPDATE_CUP_KININFO failed!");
            return false;
        }
    }
    else
    {
        // 线上没有打完则创建房间
        TraceLn(__FUNCTION__": Create legendcup Room!");
        createCompetitionNodeRoom(pNodeInfo);
    }
    return true;
}



bool CCompetitionRoundBase::offlineJudgeCancel(SMsgJudgeWarResult* pJudgeInfo, SCompetitionNode* pNodeInfo)
{
    // 取消比赛
    MatchHelper matchHelper;
    IMatchService *pMatchService = matchHelper.m_ptr;
    if (pMatchService == NULL)
    {
        ErrorLn(__FUNCTION__":can't find pMatchService == NULL");
        return false;
    }
    // 取出记录过的信息
    map<int, map<int,SNodeWarRecord>>::iterator iter = m_CompetitionNodeWarInfo.find(pNodeInfo->nSerchID);
    if ( iter == m_CompetitionNodeWarInfo.end())
    {
        ErrorLn(__FUNCTION__":can't find llLegendCupID ="<<m_llLegendCupID<<" nSerchID ="<<pNodeInfo->nSerchID);
        return false;
    }

    map<int,SNodeWarRecord>::iterator iterWarRec = iter->second.find(pJudgeInfo->nBoCount);
    if (iterWarRec == iter->second.end())
    {
        ErrorLn(__FUNCTION__":can't find llLegendCupID ="<<m_llLegendCupID<<" nSerchID ="<<pNodeInfo->nSerchID<<" nBoCount"<<pJudgeInfo->nBoCount);
        return false;
    }

    // 撤销比赛记录 
    // 1、移除记录m_CompetitionNodeWarInfo信息 2、更新比赛节点信息  4、线下新创建房间
    // 移除WarRec记录
    iter->second.erase(iterWarRec);
    // 更新节点信息
    pNodeInfo->nCurrentBo--;
    updateNodeState(pNodeInfo,emNodeState_CanEnter);   // 修改节点状态

    //取消一场 肯定要重新创建一个新房间
    TraceLn(__FUNCTION__": Create legendcup Room!");
    createCompetitionNodeRoom(pNodeInfo);
    return true;

}

bool CCompetitionRoundBase::offlineJudgeConfirmNode(SMsgJudgeWarResult* pJudgeInfo, SCompetitionNode* pNodeInfo)
{
    //维持比赛原来判定
    map<int, map<int, SNodeWarRecord>>::iterator iterWarList = m_CompetitionNodeWarInfo.find(pJudgeInfo->nSerchID);
    if ( iterWarList == m_CompetitionNodeWarInfo.end())
    {
        ErrorLn(__FUNCTION__":can't find llLegendCupID ="<<m_llLegendCupID<<" nSerchID ="<<pJudgeInfo->nSerchID);
        return false;
    }

    map<int,SNodeWarRecord>::iterator iterWarRec = iterWarList->second.find(pJudgeInfo->nBoCount);

    if (iterWarRec == iterWarList->second.end())
    {
        ErrorLn(__FUNCTION__":can't find llLegendCupID ="<<m_llLegendCupID<<" nSerchID ="<<pJudgeInfo->nSerchID<<" nBoCount"<<pJudgeInfo->nBoCount);
        return false;
    }

    // 维持比赛原来判定
    // 修改节点信息
    // 添加战队本节点得分
    if (pNodeInfo->dwKin1 == iterWarRec->second.dwWinnerKin)
    {
        pNodeInfo->nKin1Score++;
        // 已经能决出胜负了
        if (pNodeInfo->nKin1Score >= getWinScoreByType(pNodeInfo->byBoType))
        {
            updateNodeState(pNodeInfo,emNodeState_End);   // 修改节点状态   
        }
    }

    if (pNodeInfo->dwKin2 == iterWarRec->second.dwWinnerKin)
    {
        pNodeInfo->nKin2Score++;
        // 已经能决出胜负了
        if (pNodeInfo->nKin2Score >= getWinScoreByType(pNodeInfo->byBoType))
        {
            updateNodeState(pNodeInfo,emNodeState_End);   // 修改节点状态   
        }
    }

    // 更新战场判定状态
    iterWarRec->second.byWinFlag = pJudgeInfo->byJudgeFlag;
    iterWarRec->second.dwWinnerKin = pJudgeInfo->dwWinnerKinID;

    // 修改进行到当前第几轮比赛
    pNodeInfo->nCurrentBo = pJudgeInfo->nBoCount;

    //节点判定
    if (pNodeInfo->eNodeState == emNodeState_End)
    {
        pNodeInfo->dwWinnerKinID = pJudgeInfo->dwWinnerKinID;

        // 修改队伍得分
        SCupKinInfo* pKinInfo = m_pLegendCup->getCupKinInfo(pNodeInfo->dwWinnerKinID);
        if (pKinInfo == NULL)
        {
            // 队伍都找不到了 这个问题很严重
            ErrorLn(__FUNCTION__": can't find winnerID = "<<pJudgeInfo->dwWinnerKinID);
            return false;
        }
        //打完节点比赛 战队加分
        if (m_eCompetitionType == emType_GroupCompetition)
        {
            pKinInfo->nGroupScore++;
        }

        if (m_eCompetitionType == emType_KnockoutCompetition)
        {
            pKinInfo->nCurKnockOutScore++;
        }

        //打完节点比赛  更新战队分数
        IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
        if (pDBEngine == NULL)
        {
            ErrorLn(__FUNCTION__": pDBEngine == NULL");
            return false; 
        }

        DBREQ_REQUEST_UPDATE_CUP_KININFO dbParam;
        dbParam.dwWorldID = getThisGameWorldID();
        dbParam.llLegendCupID = m_llLegendCupID;                        // 杯赛ID
        dbParam.dwKinID = pKinInfo->dwKinID;                            // 战队ID
        dbParam.nGroupScore = pKinInfo->nGroupScore;                    // 小组赛积分情况 
        dbParam.nCurKnockOutScore = pKinInfo->nCurKnockOutScore;        // 当前淘汰赛积分情况 bo1 bo3 bo5

        if (pDBEngine->executeSP(GAMEDB_REQUEST_UPDATE_CUP_KININFO,0,(LPCSTR)&dbParam,sizeof(dbParam),static_cast<IDBRetSink *>(this) ) == false)
        {
            ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_UPDATE_CUP_KININFO failed!");
            return false;
        }
    }
    else
    {
        // 没有打完则创建房间
        TraceLn(__FUNCTION__": Create legendcup Room!");
        createCompetitionNodeRoom(pNodeInfo);
    }

    return true;
}

bool CCompetitionRoundBase::offlineJudgeAllOut(SMsgJudgeWarResult* pJudgeInfo, SCompetitionNode* pNodeInfo)
{
    // 全部淘汰
    map<int, map<int,SNodeWarRecord>>::iterator iterWarList = m_CompetitionNodeWarInfo.find(pJudgeInfo->nSerchID);
    if ( iterWarList == m_CompetitionNodeWarInfo.end())
    {
        ErrorLn(__FUNCTION__":can't find llLegendCupID ="<<m_llLegendCupID<<" nSerchID ="<<pJudgeInfo->nSerchID);
        return false;
    }

    map<int, SNodeWarRecord>::iterator iterWarRec = iterWarList->second.find(pJudgeInfo->nBoCount);
    if (iterWarRec == iterWarList->second.end())
    {
        ErrorLn(__FUNCTION__":can't find llLegendCupID ="<<m_llLegendCupID<<" nSerchID ="<<pJudgeInfo->nSerchID<<" nBoCount"<<pJudgeInfo->nBoCount);
        return false;
    }

    //全部淘汰
    // 1、更新m_CompetitionNodeWarInfo 里面信息 2、修改设置胜利者ID 为 0 3、更新节点状态
    iterWarRec->second.byWinFlag = JUDGE_AllKnockOut;

    pNodeInfo->dwWinnerKinID = 0;
    updateNodeState(pNodeInfo,emNodeState_End);   // 修改节点状态 

    return true;
}

bool CCompetitionRoundBase::createCompetitionNodeRoom(SCompetitionNode* pNodeInfo, bool bReset)
{
    // 创建比赛
    MatchHelper matchHelper;
    IMatchService *pMatchService = matchHelper.m_ptr;
    if (pMatchService == NULL)
    {
        ErrorLn(__FUNCTION__":can't find pMatchService == NULL");
        return false;
    }

    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeCenter");
        return false;
    }
    ISchemeLegendCup* pLegendCupScheme = pSchemeCenter->getSchemeLegendCup();
    if (nullptr == pLegendCupScheme)
    {
        ErrorLn(__FUNCTION__": failed! pLegendCupScheme == NULL");
        return false;
    }
    SSchemeLegendCup* pConfig = pLegendCupScheme->getConfig(m_pLegendCup->getLegendCupBaseInfo()->nCreateConfigID);
    if (nullptr == pConfig)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pConfig nCreateConfigID = "<<m_pLegendCup->getLegendCupBaseInfo()->nCreateConfigID);
        return false;

    }

    SSchemeLegendCupDetail* pSchemeDetailConfig = pLegendCupScheme->getDetailInfo(m_nDetailConfigID, m_nRoundID);
    if (pSchemeDetailConfig == NULL)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeConfig nDetailConfigID = "<<m_nDetailConfigID);
        return false;
    }

    // 所有节点立即创建比赛房间
    SCreateLegendCupRoomInfo sCreateCupRoomInfo;
    sCreateCupRoomInfo.bIsOnlineCup     = m_pLegendCup->getLegendCupBaseInfo()->bIsOnlineCup == 1? true: false;
    sCreateCupRoomInfo.dwKin1           = pNodeInfo->dwKin1;
    sCreateCupRoomInfo.dwKin2           = pNodeInfo->dwKin2;

    sCreateCupRoomInfo.dwWorldID        = getThisGameWorldID();                                                             // 来自于哪个世界的ID
    sCreateCupRoomInfo.dwLedendCupID    = m_llLegendCupID;                                                                  // 杯赛ID
    sCreateCupRoomInfo.nSerchID         = pNodeInfo->nSerchID;                                                              // 比赛搜索ID
	sCreateCupRoomInfo.nRoundID			= pNodeInfo->nRoundID;					                                            // 比赛第几轮
    sCreateCupRoomInfo.nBoCount         = pNodeInfo->nCurrentBo + 1;                                                        // 下一场比赛
    sCreateCupRoomInfo.nMaxCancelCount  = 3;

    sCreateCupRoomInfo.bIsOnlineCup     = m_pLegendCup->getLegendCupBaseInfo()->bIsOnlineCup;
    sCreateCupRoomInfo.nMaxRound        = m_pLegendCup->getLegendCupBaseInfo()->nNeedRoundCount;

    sCreateCupRoomInfo.eRoundType       = m_eCompetitionType;
    sCreateCupRoomInfo.byBoType         = pNodeInfo->byBoType;                                                              // bo类型
    sCreateCupRoomInfo.nNodeKin1Score   = pNodeInfo->nKin1Score;                                                            // Kin1得分
    sCreateCupRoomInfo.nNodeKin2Score   = pNodeInfo->nKin2Score;                                                            // Kin2得分
    sCreateCupRoomInfo.dwWarInterval    = pSchemeDetailConfig->m_nBoInterval;                                               // 节点未结束下一场比赛时间间隔
    
    SNextWarTimeInfo kin1NextTime = getNextWarTime(pNodeInfo->nSerchID, pNodeInfo->dwKin1);
    sCreateCupRoomInfo.bKin1RoundLastNode = kin1NextTime.bRoundLastNode;                                                    // kin1本轮最后的节点   
    sCreateCupRoomInfo.dwKin1NodeEndNextWarTime = kin1NextTime.dwNextWarTime;                                               // Kin1不同战队下场比赛时间

    SNextWarTimeInfo kin2NextTime = getNextWarTime(pNodeInfo->nSerchID, pNodeInfo->dwKin2);
    sCreateCupRoomInfo.bKin2RoundLastNode = kin2NextTime.bRoundLastNode;                                                    // kin1本轮最后的节点   
    sCreateCupRoomInfo.dwKin2NodeEndNextWarTime = kin2NextTime.dwNextWarTime;                                               // Kin2不同战队下场比赛时间

    // 调整节点可进入房间的时间
    if (m_pLegendCup->getLegendCupBaseInfo()->bIsOnlineCup)
    {
        // 第一场比赛取记录的时间 取当前的时间为进入时间
        if(bReset)
        {
            sCreateCupRoomInfo.dwCanEnterTime       = pNodeInfo->dwBeginTime;
        }
        else
        {
            sCreateCupRoomInfo.dwCanEnterTime       = sCreateCupRoomInfo.nBoCount == 1 ? pNodeInfo->dwBeginTime : time(NULL);
        }

        //线上第一场间隔是取通用记录 dwRoomBeginInterval 如果第一场打完之后取配置 BoInterval
        sCreateCupRoomInfo.dwBeginTimeInterval  = sCreateCupRoomInfo.nBoCount == 1 ? m_pLegendCup->getRoundTimeInfo(m_nRoundID)->dwRoomBeginInterval : pSchemeDetailConfig->m_nBoInterval*60; 
        sCreateCupRoomInfo.dwMatchType          = pConfig->m_nOnlineMatchType;

    }
    else
    {
        // 线下进入房间的时间处理
        sCreateCupRoomInfo.dwCanEnterTime       = time(NULL);
        sCreateCupRoomInfo.dwBeginTimeInterval  = 0;                                        // 如果为0 则第一个进入房间的玩家可以手动开始
        sCreateCupRoomInfo.dwMatchType          = pConfig->m_nOfflineMatchType;
        updateNodeState(pNodeInfo, emNodeState_CanEnter);
    }

    // 修改节点开始时间
    pNodeInfo->dwBeginTime              = sCreateCupRoomInfo.dwCanEnterTime;

    DWORD RoomID = pMatchService->createLegendCupRoom(sCreateCupRoomInfo);
    if (0 == RoomID)
    {
        ErrorLn(__FUNCTION__": can't create room!!!! legendCupID = "<<m_llLegendCupID<<" serchID ="<<pNodeInfo->nSerchID);
        return false;
    }

    m_mapSerchID2RoomID.insert(pair<int,DWORD>(pNodeInfo->nSerchID, RoomID));
    //生成当前创建的房间节点记录 用于可能在房间状态下就判定比赛胜利相关
    SNodeWarRecord sNodeWarRecord;
    sNodeWarRecord.byWinFlag    = JUDGE_NONE;
    sNodeWarRecord.llWarID      = 0;           
    sNodeWarRecord.dwWinnerKin  = 0;            
    sNodeWarRecord.nBoCount    = sCreateCupRoomInfo.nBoCount;

    map<int, map<int,SNodeWarRecord>>::iterator iterNodeWarInfo = m_CompetitionNodeWarInfo.find(pNodeInfo->nSerchID);
    if (iterNodeWarInfo == m_CompetitionNodeWarInfo.end())
    {
        m_CompetitionNodeWarInfo[pNodeInfo->nSerchID].insert(pair<int, SNodeWarRecord>(sNodeWarRecord.nBoCount, sNodeWarRecord));
    }
    else
    {
        map<int,SNodeWarRecord>::iterator iterWarRec = iterNodeWarInfo->second.find(sCreateCupRoomInfo.nBoCount);
        if (iterWarRec == iterNodeWarInfo->second.end())
        {
            m_CompetitionNodeWarInfo[pNodeInfo->nSerchID].insert(pair<int, SNodeWarRecord>(sNodeWarRecord.nBoCount, sNodeWarRecord));}
        else
        {
            WarningLn(__FUNCTION__":repeat createRoom boCount!!legendCupID = "<<m_llLegendCupID<<" serchID ="<<pNodeInfo->nSerchID);
        }
        
    }


    LEDENDCUP_TRACELN("--------------------------["<<m_pLegendCup->getLegendCupBaseInfo()->szCupName<<"]CompetitionNodeRoom---------------------------");
    LEDENDCUP_TRACELN(__FUNCTION__": create room llLegendCupID= "<<m_llLegendCupID<<" serchID ="<<pNodeInfo->nSerchID<<" BoCount="<<sNodeWarRecord.nBoCount);
    LEDENDCUP_TRACELN("RoomID = "<<RoomID);                       // 房间ID
    LEDENDCUP_TRACELN("nBoCount = "<<sNodeWarRecord.nBoCount);  // 第几场
    LEDENDCUP_TRACELN("nDay = "<<pNodeInfo->nDay);                // 第几天
    LEDENDCUP_TRACELN("nRoundID = "<<pNodeInfo->nRoundID);        // 第几轮比赛
    LEDENDCUP_TRACELN("nGroupID = "<<pNodeInfo->nGroupID);        // 分配到第几组
    LEDENDCUP_TRACELN("nPos = "<<pNodeInfo->nPos);                // 位置索引 从1开始

    time_t dwEnterRoomTime = sCreateCupRoomInfo.dwCanEnterTime;
    tm *pConfirmRegisterDeadline = localtime(&dwEnterRoomTime);
    char tBeginTime[56] = {0};
    sprintf_s(tBeginTime, sizeof(tBeginTime), "%d-%.2d-%.2d %.2d:%.2d:%.2d", 
                                1900+pConfirmRegisterDeadline->tm_year,
                                pConfirmRegisterDeadline->tm_mon+1,
                                pConfirmRegisterDeadline->tm_mday,
                                pConfirmRegisterDeadline->tm_hour,
                                pConfirmRegisterDeadline->tm_min,
                                pConfirmRegisterDeadline->tm_sec);
    LEDENDCUP_TRACELN("enterRoomTime= "<<tBeginTime);                // 开始时间
    LEDENDCUP_TRACELN("-----------------------------------------------------------------------------------------------------------------------");

    return true;
}

bool CCompetitionRoundBase::setKinListAndGroup(list<DWORD> dwKinList,bool bBreakSort /*= false*/,bool bStartRound /*= false*/)
{
    return true;
}

bool CCompetitionRoundBase::continueRound()
{
    // 继续本轮比赛
    if (m_pLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__":  m_pLegendCup == NULL!!");
        return false;
    }

    if (!AllCompetitionNodeCreateRoom())
    {
        ErrorLn(__FUNCTION__": AllCompetitionNodeCreateRoom error!!");
        return false;
    }
    return true;
}


bool CCompetitionRoundBase::endRound(list<DWORD> promotionList, list<DWORD> weedOutList)
{
    if (m_pLegendCup == NULL)
    {
        ErrorLn(__FUNCTION__": m_pLegendCup == NULL")
    }
    // 杯赛记录名次

    // 除了最终轮其他都只记录淘汰战队的rank 
    if (!isFinalRound())
    {
        // 更新淘汰队伍排名
        updateWeedOutListRank(weedOutList);
    }
    else
    {
        updateFinalListRank(promotionList);
    }

    // 没有晋级队伍则强制结束杯赛
    if (promotionList.size() == 0)
    {
        m_pLegendCup->endLegendCup(promotionList);
    }
    else
    {
        // 判定比赛是否结束
        if (m_nRoundID >= m_pLegendCup->getLegendCupBaseInfo()->nNeedRoundCount)
        {
            m_pLegendCup->endLegendCup(promotionList);
        }
        else
        {
            m_pLegendCup->toNextRound(m_nRoundID, promotionList);
        }
    }
    return true;
}


void CCompetitionRoundBase::updateWeedOutListRank(list<DWORD> weedOutList)
{
    // 获取淘汰名次区间
    int nRankBeginID = m_nMaxGroup *m_nGroupPromotionCount + 1;
    int nRankEndID = m_nMaxGroup * m_nGroupMaxKin;

    int nRank = nRankBeginID;
    list<DWORD>::iterator iter = weedOutList.begin();
    for (;iter != weedOutList.end(); ++iter)
    {
        DWORD dwKinID = *iter;
        m_pLegendCup->updatRankToKinRecord(dwKinID, nRank);
        nRank++;
        if (nRank >= nRankEndID)
        {
            nRank = nRankEndID;
        }
    }
    // 名次保存到数据库
    m_pLegendCup->saveRankListToDB();
}

void CCompetitionRoundBase::updateFinalListRank(list<DWORD> promotionList)
{
    int nRank = 1;
    for (list<DWORD>::iterator iter = promotionList.begin(); iter != promotionList.end();++iter )
    {
        DWORD dwKinID = *iter;
        m_pLegendCup->updatRankToKinRecord(dwKinID, nRank);
        nRank++;
    }
    // 名次保存到数据库
    m_pLegendCup->saveRankListToDB();
}



bool CCompetitionRoundBase::startRound()
{
    // 开始本轮比赛 
    if (!buildAllCompetitionNode())
    {
        ErrorLn(__FUNCTION__": startRound error!");
        return false;
    }

    // 创建所有节点房间
    if (!AllCompetitionNodeCreateRoom())
    {
        ErrorLn(__FUNCTION__": AllCompetitionNodeCreateRoom error!");
        return false;
    }

    m_pLegendCup->changeNewRoundCupState();
    return true;
}

void CCompetitionRoundBase::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
    return;
}

bool CCompetitionRoundBase::setKinListFromDB(SGroupKinPos* posInfo)
{
    // 淘汰赛和小组赛分组方式不同
    map_RoundGroup.clear();
    // 淘汰赛和小组赛分组方式不同
    int nFinalMaxGroup = m_nMaxGroup;
    if (isFinalRound())
    {
        nFinalMaxGroup  = m_nMaxGroup + 1;
    }

    int DBPos = 0;
    for (int t = 1; t <= nFinalMaxGroup; ++t)
    {
        for (int i = 1; i <= m_nGroupMaxKin; ++i)
        {
            if(DBPos < DBDEF_LEGENDCUP_KINSCOUNT_LEN)
            {
                map_RoundGroup[t][i] = posInfo->KinIDList[DBPos++];
            }
        }
    }

    return true;
}

bool CCompetitionRoundBase::addCompetitionNodeFromDB(SCompetitionNode* pNodeInfo)
{
    //从数据库添加比赛节点信息
    m_CompetitionNodeList.insert(pair<int, SCompetitionNode>(pNodeInfo->nSerchID, *pNodeInfo));

    LEDENDCUP_TRACELN("----------------------------------------["<<m_pLegendCup->getLegendCupBaseInfo()->szCupName<<"]addCompetitionNodeFromDB-----------------------------");
    LEDENDCUP_TRACELN(" legendCupID = "<<m_llLegendCupID<<" serchID ="<<pNodeInfo->nSerchID<<" BoCount="<<pNodeInfo->nCurrentBo);
    LEDENDCUP_TRACELN("nDay = "<<pNodeInfo->nDay);                // 第几天
    LEDENDCUP_TRACELN("nRoundID = "<<pNodeInfo->nRoundID);        // 第几轮比赛
    LEDENDCUP_TRACELN("nGroupID = "<<pNodeInfo->nGroupID);        // 第几组
    LEDENDCUP_TRACELN("nPos = "<<pNodeInfo->nPos);                // 位置索引 从1开始

    time_t dwEnterRoomTime = pNodeInfo->dwBeginTime;
    tm *pConfirmRegisterDeadline = localtime(&dwEnterRoomTime);
    char tBeginTime[56] = {0};
    sprintf_s(tBeginTime, sizeof(tBeginTime), "%d-%.2d-%.2d %.2d:%.2d:%.2d", 
        1900+pConfirmRegisterDeadline->tm_year,
        pConfirmRegisterDeadline->tm_mon+1,
        pConfirmRegisterDeadline->tm_mday,
        pConfirmRegisterDeadline->tm_hour,
        pConfirmRegisterDeadline->tm_min,
        pConfirmRegisterDeadline->tm_sec);
    LEDENDCUP_TRACELN("enterRoomTime= "<<tBeginTime);                // 开始时间
    LEDENDCUP_TRACELN("-----------------------------------------------------------------------------------------------------------------------");

    for (int n = 0; n < pNodeInfo->nCurrentBo; ++n)
    {
        SNodeWarRecord sNodeWarRec;
        sNodeWarRec.byWinFlag = pNodeInfo->WinFlag[n];
        sNodeWarRec.llWarID = pNodeInfo->WarIDList[n];       // 战场ID
        sNodeWarRec.dwWinnerKin = pNodeInfo->WinKinList[n];   // 战场胜利列表
        sNodeWarRec.bDBHaveRecord = true;
        sNodeWarRec.nBoCount = n + 1;
        // 记录每场的比赛相关信息到内存索引1开始
        m_CompetitionNodeWarInfo[pNodeInfo->nSerchID].insert(pair<int, SNodeWarRecord>(sNodeWarRec.nBoCount, sNodeWarRec));
    }

    return true;
}

void CCompetitionRoundBase::OnTimer(unsigned long dwTimerID)
{
    return;
}

int CCompetitionRoundBase::competitionNode2Client(obuf &outBuf)
{
    // 头信息 本轮基础信息和分组信息
    // 决赛特殊处理下
    int nSendMaxGroup = m_nMaxGroup;
    if (isFinalRound())
    {
        nSendMaxGroup  = m_nMaxGroup + 1;
    }

    SMsgCompetitionRoundHead* pHead = new SMsgCompetitionRoundHead();
    pHead->nRoundID = m_nRoundID;
    pHead->nMaxGoup = nSendMaxGroup;                // 分组数量  (特别注意决赛是2组 2个队 胜者组 败者组)
    pHead->nGroupMaxKin = m_nGroupMaxKin;           // 每组最大战队数量
    pHead->eCompetitionType = m_eCompetitionType;   // ERoundCompetitionType
    pHead->bIsFinal = isFinalRound();

    // 推算本轮比赛开始时间
    SRoundTimeInfo* pRoundTimeInfo = m_pLegendCup->getRoundTimeInfo(m_nRoundID);
    if (pRoundTimeInfo != NULL)
    {
        pHead->dwRoundBeginTime = pRoundTimeInfo->dwRoundCreateRoomTime + pRoundTimeInfo->dwRoomBeginInterval;
    }

    KinHelper helper;
    IKinService *pKinService = helper.m_ptr;
    int DBPos = 0;
    for (int i = 1; i <= nSendMaxGroup; ++i)
    {
        for (int t = 1; t <= m_nGroupMaxKin; ++t)
        {
            pHead->KinIDList[DBPos].dwKinID = map_RoundGroup[i][t];
            if (pHead->KinIDList[DBPos].dwKinID != 0)
            {
                if (pKinService != NULL)
                {
                    SKinInfo sKinInfo = pKinService->getKinInfo(pHead->KinIDList[DBPos].dwKinID);
                    sstrcpyn(pHead->KinIDList[DBPos].szKinName, sKinInfo.szName, sizeof(pHead->KinIDList[DBPos].szKinName));
                }
            }

            DBPos++;
        }
    }

    pHead->nCount  = m_CompetitionNodeList.size();
    outBuf.push_back((void *)pHead, sizeof(SMsgCompetitionRoundHead));

    // 每个节点信息
    map<int, SCompetitionNode>::iterator iter = m_CompetitionNodeList.begin();
    for (;iter != m_CompetitionNodeList.end(); ++iter)
    {
        SCompetitionNode nodeInfo = iter->second;
        DWORD dwEnterInterval = 0;
        if (m_pLegendCup->getLegendCupBaseInfo()->bIsOnlineCup)
        {
           dwEnterInterval = nodeInfo.nCurrentBo < 1 ? pRoundTimeInfo->dwRoomBeginInterval : getBoInterval();
        }

        SMsgCompetitionNodeInfo node;
        node.byCompetitionType  = nodeInfo.byCompetitionType;   // 1小组赛 2淘汰赛
        node.nSerchID           = nodeInfo.nSerchID;           // 生成的快速定位ID
        node.nRoundID           = nodeInfo.nRoundID;           // 第几轮比赛
        node.nGroupID           = nodeInfo.nGroupID;           // 分配到第几组
        node.nDay               = nodeInfo.nDay;               // 第几天
        node.nPos               = nodeInfo.nPos;               // 位置索引 从1开始
        node.dwKin1             = nodeInfo.dwKin1;             // 比赛战队1
        SKinInfo sKinInfo1      = pKinService->getKinInfo(node.dwKin1);
        sstrcpyn(node.szKin1Name, sKinInfo1.szName, sizeof(node.szKin1Name));
        node.nKin1Score         = nodeInfo.nKin1Score;         // 战队1本次比赛分数
        node.dwKin2             = nodeInfo.dwKin2;             // 比赛战队2
        SKinInfo sKinInfo2      = pKinService->getKinInfo(node.dwKin2);
        node.nKin2Score         = nodeInfo.nKin2Score;         // 战队2本次比赛分数
        sstrcpyn(node.szKin2Name, sKinInfo2.szName, sizeof(node.szKin2Name));
        node.eNodeState         = nodeInfo.eNodeState;         // 节点状态ECompetitionNodeState
        node.dwWinnerKinID      = nodeInfo.dwWinnerKinID;      // 胜利战队ID
        node.byBoType           = nodeInfo.byBoType;           // BO类型 ELegendCupBO
        node.nCurrentBo         = nodeInfo.nCurrentBo;         // 已经打了多少场
        node.dwBeginTime        = nodeInfo.dwBeginTime + dwEnterInterval;// 比赛开始时间
        node.dwEnterTime        = nodeInfo.dwBeginTime;		 // 节点进入时间
        node.dwServerTime       = (DWORD)time(NULL);		// 服务器当前时间
        outBuf<<node;
    }

    return pHead->nCount;
}

// 获取本轮比赛开始时间
DWORD CCompetitionRoundBase::getRoundBeginTime()
{
    return 0;
}

bool CCompetitionRoundBase::isFinalRound()
{
    return m_nGroupMaxKin * m_nMaxGroup == 2;
}

bool CCompetitionRoundBase::saveCompetitionNodeToDB(SCompetitionNode* pNodeInfo)
{
    if (pNodeInfo == NULL)
    {
        ErrorLn(__FUNCTION__": pNodeInfo == NULL");
        return false;
    }

    IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        ErrorLn(__FUNCTION__": pDBEngine == NULL");
        return false; 
    }

    obuf ob;
    DBREQ_REQUEST_UPDATE_CUP_COMPETITION_NODE dbPara;
    dbPara.dwWorldID            = getThisGameWorldID();
    dbPara.llLegendCupID        = m_llLegendCupID;	    // 比赛ID
    dbPara.nSerchID             = pNodeInfo->nSerchID;               // 生成的快速定位ID
    dbPara.byCompetitionType    = pNodeInfo->byCompetitionType;      // 1小组赛 2淘汰赛
    dbPara.nRoundID             = pNodeInfo->nRoundID;               // 第几轮比赛
    dbPara.nGroupID             = pNodeInfo->nGroupID;               // 分配到第几组
    dbPara.nDay                 = pNodeInfo->nDay;                   // 第几天
    dbPara.nPos                 = pNodeInfo->nPos;                   // 位置索引
    dbPara.nKin                 = pNodeInfo->dwKin1;                   // 比赛战队1
    dbPara.nKin1Score           = pNodeInfo->nKin1Score;             // 战队1本次比赛分数
    dbPara.dwKin1TotalKda       = pNodeInfo->dwKin1TotalKda;         // 战队1总KDA
    dbPara.nKin2                = pNodeInfo->dwKin2;                  // 比赛战队2
    dbPara.nKin2Score           = pNodeInfo->nKin2Score;             // 战队2本次比赛分数
    dbPara.dwKin2TotalKda       = pNodeInfo->dwKin2TotalKda;         // 战队2总KDA
    dbPara.eNodeState           = pNodeInfo->eNodeState;             // 节点状态ECompetitionNodeState
    dbPara.dwWinnerKinID        = pNodeInfo->dwWinnerKinID;          // 胜利战队ID
    dbPara.byBoType             = pNodeInfo->byBoType;               // BO类型 ELegendCupBO
    dbPara.nCurrentBo           = pNodeInfo->nCurrentBo;             // 已经打了多少场 bo1 bo3 bo5
    dbPara.dwBeginTime          = pNodeInfo->dwBeginTime;            // 比赛节点开始时间
    dbPara.dwEndTime            = pNodeInfo->dwEndTime;              // 比赛节点结束时间

    LEDENDCUP_TRACELN("---------------------["<<m_pLegendCup->getLegendCupBaseInfo()->szCupName<<"]saveCompetitionNodeToDB-----------------------");
    LEDENDCUP_TRACELN(__FUNCTION__": legendCupID = "<<m_llLegendCupID<<" serchID ="<<pNodeInfo->nSerchID<<" BoCount="<<pNodeInfo->nCurrentBo);
    LEDENDCUP_TRACELN("nDay = "<<pNodeInfo->nDay);                // 第几天
    LEDENDCUP_TRACELN("nRoundID = "<<pNodeInfo->nRoundID);        // 第几轮比赛
    LEDENDCUP_TRACELN("nGroupID = "<<pNodeInfo->nGroupID);        // 第几组
    LEDENDCUP_TRACELN("nPos = "<<pNodeInfo->nPos);                // 位置索引 从1开始

    time_t dwEnterRoomTime = pNodeInfo->dwBeginTime;
    tm *pConfirmRegisterDeadline = localtime(&dwEnterRoomTime);
    char tBeginTime[56] = {0};
    sprintf_s(tBeginTime, sizeof(tBeginTime), "%d-%.2d-%.2d %.2d:%.2d:%.2d", 
        1900+pConfirmRegisterDeadline->tm_year,
        pConfirmRegisterDeadline->tm_mon+1,
        pConfirmRegisterDeadline->tm_mday,
        pConfirmRegisterDeadline->tm_hour,
        pConfirmRegisterDeadline->tm_min,
        pConfirmRegisterDeadline->tm_sec);
    LEDENDCUP_TRACELN("enterRoomTime= "<<tBeginTime);                // 开始时间

    LEDENDCUP_TRACELN("-----------------------------------------------------------------------------------------------------------------------");
    
    // 保存到
    map<int, map<int,SNodeWarRecord>>::iterator iter = m_CompetitionNodeWarInfo.find(pNodeInfo->nSerchID);
    if(iter != m_CompetitionNodeWarInfo.end())
    {
        int nIndex = 0;
        for (map<int,SNodeWarRecord>::iterator iterRec = iter->second.begin(); iterRec != iter->second.end(); ++iterRec)
        {

            dbPara.WarIDList[nIndex]    = iterRec->second.llWarID;
            dbPara.WinKinList[nIndex]   = iterRec->second.dwWinnerKin;
            dbPara.WinFlag[nIndex]      = iterRec->second.byWinFlag;
            nIndex++;
        }
    }
    ob<<dbPara;
    if (pDBEngine->executeSP(GAMEDB_REQUEST_UPDATE_CUP_COMPETITION_NODE, 0, ob.data(),ob.size(),static_cast<IDBRetSink *>(this)) == false)
    {
        ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_UPDATE_CUP_COMPETITION_NODE failed!");
        return false;
    }
    return true;
}

bool CCompetitionRoundBase::bOfflineCupCanEndRound()
{
    bool bCanEndRound = true;

    if (m_pLegendCup->getLegendCupBaseInfo()->bIsOnlineCup)
    {
        bCanEndRound = false;
    }
    else
    {
        map<int, SCompetitionNode>::iterator iter = m_CompetitionNodeList.begin();
        for(;iter != m_CompetitionNodeList.end(); ++iter)
        {
            if (iter->second.eNodeState != emNodeState_End)
            {
                bCanEndRound = false;
                break;
            }
        }
    }
 
    return bCanEndRound;
}

bool CCompetitionRoundBase::joinLegendCupRoom(SMsgJoinLegendCupRoom* pMsgInfo)
{
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if(pShareReceiver == NULL)
    {
        ErrorLn(__FUNCTION__": pShareReceiver == NULL");
        return false;
    }
    // 进入房间判定
    map<int, SCompetitionNode>::iterator iter = m_CompetitionNodeList.find(pMsgInfo->nSerchID);
    if (iter == m_CompetitionNodeList.end())
    {
        ErrorLn(__FUNCTION__": can't find competitionNode legendCupID ="<<m_llLegendCupID<<" nSerchID ="<<pMsgInfo->nSerchID);
        return false;
    }

    if (iter->second.eNodeState == emNodeState_End)
    {
        WarningLn(__FUNCTION__": can't find competitionNode eNodeState == emNodeState_End");
        return false;
    }

    if (iter->second.eNodeState != emNodeState_CanEnter)
    {
        g_ExternalFacade.showSystemMessage(pMsgInfo->idActor, CHAT_TIP_LEGENDCUP_ROOM_CANT_JOIN,"");
        return false;
    }

    map<int,DWORD>::iterator iterRoom = m_mapSerchID2RoomID.find(pMsgInfo->nSerchID);
    if (iterRoom == m_mapSerchID2RoomID.end())
    {
        ErrorLn(__FUNCTION__": can't find legendCupID="<<m_llLegendCupID<<"roomID nSerchID="<<pMsgInfo->nSerchID)
        return false;
    }
    pMsgInfo->dwRoomID = iterRoom->second;

    return true;
}

bool CCompetitionRoundBase::lookUpCupNodeWarInfo(SMsgLookUpCupNodeWarInfo* pMsgInfo)
{
    KinHelper helper;
    IKinService *pKinService = helper.m_ptr;
    if (pKinService == NULL)
    {
        ErrorLn(__FUNCTION__": failed pKinService == NULL");
        return false;
    }

    obuf ob;
    SNetMsgHead NetMsgHead;
    NetMsgHead.bySrcEndPoint	= MSG_ENDPOINT_SOCIAL;
    NetMsgHead.byDestEndPoint	= MSG_ENDPOINT_CLIENT;
    NetMsgHead.byKeyModule		= MSG_MODULEID_LEGENDCUP;
    NetMsgHead.byKeyAction		= OC_CUP_SEND_NODE_LOOKUP_WARINFO;
    ob<<NetMsgHead;

    map<int, SCompetitionNode>::iterator iter = m_CompetitionNodeList.find(pMsgInfo->nSerchID);
    if (iter == m_CompetitionNodeList.end())
    {
        return false;
    }
    int nWarRecCount = 0;
    obuf obWarRec;
    map<int, map<int,SNodeWarRecord>>::iterator iterWarRec = m_CompetitionNodeWarInfo.find(pMsgInfo->nSerchID);
    if (iterWarRec != m_CompetitionNodeWarInfo.end())
    {
        for (map<int,SNodeWarRecord>::iterator iterWar = iterWarRec->second.begin(); iterWar != iterWarRec->second.end(); ++iterWar)
        {
            obWarRec<<iterWar->second;
            nWarRecCount++;
        }
    }

    // 准备下发数据
    SMsgLookUpClientNodeHead headInfo;
    headInfo.bHaveRightJudge = m_pLegendCup->haveRightJudge(pMsgInfo->clientID);
    headInfo.dwKin1 = iter->second.dwKin1;             // 比赛战队1
    headInfo.nKin1Score = iter->second.nKin1Score;
    sstrcpyn(headInfo.szKin1Name, pKinService->getKinInfo(iter->second.dwKin1).szName, sizeof(headInfo.szKin1Name));
    headInfo.dwKin2 = iter->second.dwKin2;             // 比赛战队2
    headInfo.nKin2Score = iter->second.nKin2Score;
    sstrcpyn(headInfo.szKin2Name, pKinService->getKinInfo(iter->second.dwKin2).szName, sizeof(headInfo.szKin2Name));
    headInfo.dwResetEndTime = m_pLegendCup->getRoundEndResetTime(m_nRoundID);

    headInfo.nCount = nWarRecCount;
    // 压入头信息
    ob<<headInfo;
    ob.push_back(obWarRec.data(), obWarRec.size());

    send2Client(pMsgInfo->clientID, PACKAGE_PTR(new std::string((char const *)ob.data(), ob.size())));
    return true;
}

void CCompetitionRoundBase::updateSingleCupNodeInfo2AllView( SCompetitionNode* pNodeInfo)
{

    DWORD dwEnterInterval = 0;
    if (m_pLegendCup->getLegendCupBaseInfo()->bIsOnlineCup)
    {
        dwEnterInterval = pNodeInfo->nCurrentBo < 1 ? m_pLegendCup->getRoundTimeInfo(m_nRoundID)->dwRoomBeginInterval : getBoInterval();
    }
    
    KinHelper helper;
    IKinService *pKinService = helper.m_ptr;
    if (pKinService == NULL)
    {
        ErrorLn(__FUNCTION__": failed pKinService == NULL");
        return;
    }
    DWORD dwNowTime = (DWORD)time(NULL);
    obuf ob;
    SNetMsgHead NetMsgHead;
    NetMsgHead.bySrcEndPoint	= MSG_ENDPOINT_SOCIAL;
    NetMsgHead.byDestEndPoint	= MSG_ENDPOINT_CLIENT;
    NetMsgHead.byKeyModule		= MSG_MODULEID_LEGENDCUP;
    NetMsgHead.byKeyAction		= OC_CUP_SEND_UPDATE_SINGEL_CUP_NODE_INFO;
	ob.push_back(&NetMsgHead, sizeof(NetMsgHead));

	ob.push_back(&m_llLegendCupID, sizeof(m_llLegendCupID));
    
    SMsgCompetitionNodeInfo node;
    node.byCompetitionType  = pNodeInfo->byCompetitionType;   // 1小组赛 2淘汰赛
    node.nSerchID           = pNodeInfo->nSerchID;           // 生成的快速定位ID
    node.nRoundID           = pNodeInfo->nRoundID;           // 第几轮比赛
    node.nGroupID           = pNodeInfo->nGroupID;           // 分配到第几组
    node.nDay               = pNodeInfo->nDay;               // 第几天
    node.nPos               = pNodeInfo->nPos;               // 位置索引 从1开始
    node.dwKin1             = pNodeInfo->dwKin1;             // 比赛战队1
    SKinInfo sKinInfo1      = pKinService->getKinInfo(node.dwKin1);
    sstrcpyn(node.szKin1Name, sKinInfo1.szName, sizeof(node.szKin1Name));
    node.nKin1Score         = pNodeInfo->nKin1Score;         // 战队1本次比赛分数
    node.dwKin2             = pNodeInfo->dwKin2;             // 比赛战队2
    SKinInfo sKinInfo2      = pKinService->getKinInfo(node.dwKin2);
    node.nKin2Score         = pNodeInfo->nKin2Score;         // 战队2本次比赛分数
    sstrcpyn(node.szKin2Name, sKinInfo2.szName, sizeof(node.szKin2Name));
    node.eNodeState         = pNodeInfo->eNodeState;         // 节点状态ECompetitionNodeState
    node.dwWinnerKinID      = pNodeInfo->dwWinnerKinID;      // 胜利战队ID
    node.byBoType           = pNodeInfo->byBoType;           // BO类型 ELegendCupBO
    node.nCurrentBo         = pNodeInfo->nCurrentBo;         // 已经打了多少场
    node.dwBeginTime        = pNodeInfo->dwBeginTime + dwEnterInterval;		// 节点比赛开始时间
    node.dwEnterTime        = pNodeInfo->dwBeginTime;       // 节点进场开始时间
    node.dwServerTime       = (DWORD)time(NULL);		    // 服务器当前时间
    ob.push_back(&node, sizeof(node));

    m_pLegendCup->sendDataToAllViewer(ob.data(), ob.size());
}


void CCompetitionRoundBase::updateCupNodeWarInfo2AllView( SCompetitionNode* pNodeInfo)
{
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if(pShareReceiver == NULL)
    {
        ErrorLn(__FUNCTION__": pShareReceiver == NULL");
        return;
    }

    KinHelper helper;
    IKinService *pKinService = helper.m_ptr;
    if (pKinService == NULL)
    {
        ErrorLn(__FUNCTION__": failed pKinService == NULL");
        return ;
    }

    map<DWORD,bool> mapViewer;
    m_pLegendCup->getAllViewerList(mapViewer);
    if (mapViewer.size() <= 0)
    {
        return;
    }

    SNetMsgHead NetMsgHead;
    NetMsgHead.bySrcEndPoint	= MSG_ENDPOINT_SOCIAL;
    NetMsgHead.byDestEndPoint	= MSG_ENDPOINT_CLIENT;
    NetMsgHead.byKeyModule		= MSG_MODULEID_LEGENDCUP;
    NetMsgHead.byKeyAction		= OC_CUP_SEND_NODE_LOOKUP_WARINFO;

    obuf warRecBuf;
    int nWarRecCount =0;
    map<int, map<int,SNodeWarRecord>>::iterator iterWarRec = m_CompetitionNodeWarInfo.find(pNodeInfo->nSerchID);
    if (iterWarRec != m_CompetitionNodeWarInfo.end())
    {
        for (map<int,SNodeWarRecord>::iterator iterWar = iterWarRec->second.begin(); iterWar != iterWarRec->second.end(); ++iterWar)
        {
            warRecBuf<<iterWar->second;
            nWarRecCount++;
        }
    }

    for(map<DWORD,bool>::iterator iter = mapViewer.begin();iter != mapViewer.end();++iter)
    {
        SSharePersonProperty shareProperty = pShareReceiver->GetSharePerson(iter->first);
        if (shareProperty.dwPDBID == INVALID_PDBID)
        {
            continue;
        }

        obuf sendBuf;
        // 消息头信息
        sendBuf<<NetMsgHead;

        // 战斗记录信息头
        SMsgLookUpClientNodeHead headInfo;
        headInfo.bHaveRightJudge = m_pLegendCup->haveRightJudge(shareProperty.clientID);
        headInfo.dwKin1 = pNodeInfo->dwKin1;             // 比赛战队1
        headInfo.nKin1Score = pNodeInfo->nKin1Score;
        sstrcpyn(headInfo.szKin1Name, pKinService->getKinInfo(pNodeInfo->dwKin1).szName, sizeof(headInfo.szKin1Name));
        headInfo.dwKin2 = pNodeInfo->dwKin2;             // 比赛战队2
        headInfo.nKin2Score = pNodeInfo->nKin2Score;
        sstrcpyn(headInfo.szKin2Name, pKinService->getKinInfo(pNodeInfo->dwKin2).szName, sizeof(headInfo.szKin2Name));
        headInfo.nCount = nWarRecCount;
        headInfo.dwResetEndTime = m_pLegendCup->getRoundEndResetTime(m_nRoundID);
        sendBuf<<headInfo;

        // 战斗记录节点信息
        sendBuf.push_back(warRecBuf.data(), warRecBuf.size());

        send2Client(shareProperty.clientID, PACKAGE_PTR(new std::string((char const *)sendBuf.data(), sendBuf.size())));
    }
}


void CCompetitionRoundBase::send2Client(ClientID clientID, PACKAGE_PTR Package)
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

    pGatewayAcceptorService->sendData( clientID, Package );
}

bool CCompetitionRoundBase::updateNodeNewStartTime(int nSerchID, DWORD dwNewStartTime)
{
    map<int, SCompetitionNode>::iterator iter = m_CompetitionNodeList.find(nSerchID);
    if (iter == m_CompetitionNodeList.end())
    {
        ErrorLn(__FUNCTION__": can't find legendCupID="<<m_llLegendCupID<<" serchID ="<<nSerchID);
        return false;
    }

    //更新节点
    SCompetitionNode* pNodeInfo = &(iter->second);
    updateNodeState(pNodeInfo, emNodeState_CanEnter);
    // 修改开始时间
    pNodeInfo->dwBeginTime = dwNewStartTime;
    return true;
}

DWORD CCompetitionRoundBase::getBoInterval()
{
    ISchemeCenter* pSchemeCenter = gSocialGlobal->getSchemeCenter();
    if (nullptr == pSchemeCenter)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeCenter");
        return 0;
    }
    ISchemeLegendCup* pLegendCupScheme = pSchemeCenter->getSchemeLegendCup();
    SSchemeLegendCupDetail* pSchemeDetailConfig = pLegendCupScheme->getDetailInfo(m_nDetailConfigID, m_nRoundID);
    if (pSchemeDetailConfig == NULL)
    {
        ErrorLn(__FUNCTION__": failed! nullptr == pSchemeConfig nDetailConfigID = "<<m_nDetailConfigID);
        return 0;
    }

    return pSchemeDetailConfig->m_nBoInterval*60;
}

void CCompetitionRoundBase::updateNodeState(SCompetitionNode* pNodeInfo, ECompetitionNodeState eState)
{
    if (pNodeInfo == NULL)
    {
        return;
    }
    pNodeInfo->eNodeState = eState;

    // 通知所有参赛成员状态更新
    broadCastKinMemberCompetitionInfo(pNodeInfo);

    // 广播给参赛节点成员状态改变信息
    updateSingleCupNodeInfo2AllView(pNodeInfo);
}

bool CCompetitionRoundBase::onlineJudgeSelectWinner(SMsgJudgeWarResult* pJudgeInfo, SCompetitionNode* pNodeInfo)
{
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if(pShareReceiver == NULL)
    {
        ErrorLn(__FUNCTION__": pShareReceiver == NULL");
        return false;
    }

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePersonByClientID(pJudgeInfo->clientID);
    if (shareProperty.dwPDBID == INVALID_PDBID)
    {
        ErrorLn(__FUNCTION__": shareProperty.dwPDBID == INVALID_PDBID");
        return false;
    }

    if (!(pNodeInfo->eNodeState == emNodeState_WaitStart || pNodeInfo->eNodeState == emNodeState_CanEnter))
    {
        // 线上的节点没有开始比赛才能裁定 （如果是进行中裁定 会有各种各样的问题）
        g_ExternalFacade.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_LEGENDCUP_RUNNING_CANT_SELECT_WINNER,"");
        return false;
    }

    // 移除房间
    MatchHelper matchHelper;
    IMatchService *pMatchService = matchHelper.m_ptr;
    if (pMatchService == NULL)
    {
        ErrorLn(__FUNCTION__":can't find pMatchService == NULL");
        return false;
    }

    if(m_mapSerchID2RoomID.find(pJudgeInfo->nSerchID) != m_mapSerchID2RoomID.end())
    {
        TraceLn(__FUNCTION__":removeRoom nSerchID ="<<pJudgeInfo->nSerchID<<" llLegendCupID="<<m_llLegendCupID);
        pMatchService->removeLegendCupRoom(m_mapSerchID2RoomID[pJudgeInfo->nSerchID]);
        m_mapSerchID2RoomID.erase(pJudgeInfo->nSerchID);
    }

    //线上判定输赢
    map<int, map<int, SNodeWarRecord>>::iterator iterMapWarRec = m_CompetitionNodeWarInfo.find(pJudgeInfo->nSerchID);
    if ( iterMapWarRec == m_CompetitionNodeWarInfo.end())
    {
        ErrorLn(__FUNCTION__":can't find llLegendCupID ="<<m_llLegendCupID<<" nSerchID ="<<pJudgeInfo->nSerchID);
        return false;
    }

    map<int, SNodeWarRecord>::iterator iterWarRec = iterMapWarRec->second.find(pJudgeInfo->nBoCount);

    if (iterWarRec == iterMapWarRec->second.end())
    {
        ErrorLn(__FUNCTION__":can't find llLegendCupID ="<<m_llLegendCupID<<" nSerchID ="<<pJudgeInfo->nSerchID<<" nBoCount"<<pJudgeInfo->nBoCount);
        return false;
    }

    if(pJudgeInfo->dwWinnerKinID <= 0)
    {
        g_ExternalFacade.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_LEGENDCUP_SELECT_RIGHT_KINID,"");
        return false;
    }

    if(pJudgeInfo->dwWinnerKinID == iterWarRec->second.dwWinnerKin)
    {
        g_ExternalFacade.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_LEGENDCUP_REPEAT_JUDGED,"");
        return false;
    }

    // 已经能决出胜负了
    if (pNodeInfo->dwKin1 == pJudgeInfo->dwWinnerKinID)
    {
        pNodeInfo->nKin1Score++;
        if (pNodeInfo->nKin1Score >= getWinScoreByType(pNodeInfo->byBoType))
        {
            updateNodeState(pNodeInfo,emNodeState_End);   // 修改节点状态
        }
    }

    if (pNodeInfo->dwKin2 == pJudgeInfo->dwWinnerKinID)
    {
        pNodeInfo->nKin2Score++;
        if (pNodeInfo->nKin2Score >= getWinScoreByType(pNodeInfo->byBoType))
        {
            updateNodeState(pNodeInfo,emNodeState_End);   // 修改节点状态     
        }
    }

    // 更新战场判定状态
    iterWarRec->second.byWinFlag = pJudgeInfo->byJudgeFlag;
    iterWarRec->second.dwWinnerKin = pJudgeInfo->dwWinnerKinID;
    // 修改进行到当前第几轮比赛
    pNodeInfo->nCurrentBo = pJudgeInfo->nBoCount;

    if (pNodeInfo->eNodeState == emNodeState_End)
    {
        pNodeInfo->dwWinnerKinID = pJudgeInfo->dwWinnerKinID;

        // 修改队伍得分
        SCupKinInfo* pKinInfo = m_pLegendCup->getCupKinInfo(pNodeInfo->dwWinnerKinID);
        if (pKinInfo == NULL)
        {
            // 队伍都找不到了 这个问题很严重
            ErrorLn(__FUNCTION__": can't find winnerID = "<<pJudgeInfo->dwWinnerKinID);
            return false;
        }
        // 打完节点比赛 战队加分
        if (m_eCompetitionType == emType_GroupCompetition)
        {
            pKinInfo->nGroupScore++;
        }

        if (m_eCompetitionType == emType_KnockoutCompetition)
        {
            pKinInfo->nCurKnockOutScore++;
        }

        // 打完节点比赛 更新战队分数
        IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
        if (pDBEngine == NULL)
        {
            ErrorLn(__FUNCTION__": pDBEngine == NULL");
            return false; 
        }

        DBREQ_REQUEST_UPDATE_CUP_KININFO dbParam;
        dbParam.dwWorldID = getThisGameWorldID();
        dbParam.llLegendCupID = m_llLegendCupID;                      // 杯赛ID
        dbParam.dwKinID = pKinInfo->dwKinID;                          // 战队ID
        dbParam.nGroupScore = pKinInfo->nGroupScore;                  // 小组赛积分情况 
        dbParam.nCurKnockOutScore = pKinInfo->nCurKnockOutScore;      // 当前淘汰赛积分情况 bo1 bo3 bo5

        if (pDBEngine->executeSP(GAMEDB_REQUEST_UPDATE_CUP_KININFO,0,(LPCSTR)&dbParam,sizeof(dbParam),static_cast<IDBRetSink *>(this)) == false)
        {
            ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_UPDATE_CUP_KININFO failed!");
            return false;
        }
    }
    else
    {
        createCompetitionNodeRoom(pNodeInfo);
    }

    return true;
}

bool CCompetitionRoundBase::onlineCanResetWarTime(SMsgJudgeWarResult* pJudgeInfo)
{
    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if(pShareReceiver == NULL)
    {
        ErrorLn(__FUNCTION__": pShareReceiver == NULL");
        return false;
    }

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePersonByClientID(pJudgeInfo->clientID);
    if (shareProperty.dwPDBID == INVALID_PDBID)
    {
        ErrorLn(__FUNCTION__": shareProperty.dwPDBID == INVALID_PDBID");
        return false;
    }

    // 不是当前轮 不让重置
    if(m_pLegendCup->getCurrentRoundID() != m_nRoundID)
    {
        g_ExternalFacade.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_LEGENDCUP_RESET_TIME_NOT_CURRENT_ROUND,"");
        return false;
    }
    if (pJudgeInfo->dwResetNewTime == 0)
    {
        // 如果没有设置 则自动调整到最终重置时间点
        pJudgeInfo->dwResetNewTime = m_pLegendCup->getRoundEndResetTime(m_nRoundID);
    }

    // 小于当前时间 不让重置
    DWORD dwNowTime = (DWORD)time(NULL);
    if (pJudgeInfo->dwResetNewTime <= dwNowTime)
    { 
        g_ExternalFacade.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_LEGENDCUP_RESET_TIME_LESS_THAN_NOW,"");
        return false;
    }

    // 大于最终重置时间点 不让重置
    DWORD dwEndResetTime = m_pLegendCup->getRoundEndResetTime(m_nRoundID);
    if (pJudgeInfo->dwResetNewTime > dwEndResetTime)
    { 
        g_ExternalFacade.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_LEGENDCUP_RESET_TIME_LARGE_THAN_END_RESETTIME,"");
        return false;
    }
    return true;
}

bool CCompetitionRoundBase::onlineCanResetScoreCheck(SMsgJudgeWarResult* pJudgeInfo, SCompetitionNode* pNodeInfo)
{
    // 获取bo类型最大决出胜负的比分
    if (pNodeInfo == NULL)
    {
        return false;
    }

    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if(pShareReceiver == NULL)
    {
        ErrorLn(__FUNCTION__": pShareReceiver == NULL");
        return false;
    }

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePersonByClientID(pJudgeInfo->clientID);
    if (shareProperty.dwPDBID == INVALID_PDBID)
    {
        ErrorLn(__FUNCTION__": shareProperty.dwPDBID == INVALID_PDBID");
        return false;
    }


    int nWinnerMaxScore = getWinScoreByType(pNodeInfo->byBoType);
    int nWinMaxScore  = getWinMaxScoreByType(pNodeInfo->byBoType);

    if (pJudgeInfo->nKin1Score + pJudgeInfo->nKin2Score > nWinMaxScore)
    {
        char szBuf[10] = {0};
        sprintf_s(szBuf, sizeof(szBuf), "%d", nWinMaxScore);

        g_ExternalFacade.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_LEGENDCUP_RESET_TOTAL_SCORE_ERROR, szBuf);
        return false;
    }

    if (pJudgeInfo->nKin1Score > nWinnerMaxScore ||  pJudgeInfo->nKin2Score > nWinnerMaxScore)
    {
        char szBuf[10] = {0};
        sprintf_s(szBuf, sizeof(szBuf), "%d", nWinnerMaxScore);

        g_ExternalFacade.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_LEGENDCUP_RESET_SINGLE_SCORE_ERROR, szBuf);
        return false;
    }

    return true;
    
}

bool CCompetitionRoundBase::onlineResetWarRecord(SMsgJudgeWarResult* pJudgeInfo, SCompetitionNode* pNodeInfo)
{
    IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        ErrorLn(__FUNCTION__": pDBEngine == NULL");
        return false; 
    }

    IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
    if(pShareReceiver == NULL)
    {
        ErrorLn(__FUNCTION__": pShareReceiver == NULL");
        return false;
    }

    SSharePersonProperty shareProperty = pShareReceiver->GetSharePersonByClientID(pJudgeInfo->clientID);
    if (shareProperty.dwPDBID == INVALID_PDBID)
    {
        ErrorLn(__FUNCTION__": shareProperty.dwPDBID == INVALID_PDBID");
        return false;
    }

    if (!onlineCanResetWarTime(pJudgeInfo))
    {
        return false;
    }

    if (!onlineCanResetScoreCheck(pJudgeInfo, pNodeInfo))
    {
        return false;
    }


    if (!(pNodeInfo->eNodeState == emNodeState_WaitStart || pNodeInfo->eNodeState == emNodeState_End))
    {
        // 重置比赛必须要等待开始或者结束节点才能重置比赛
        g_ExternalFacade.showSystemMessage(shareProperty.dwPDBID, CHAT_TIP_LEGENDCUP_ONLINE_STATE_CANT_JUDGED_WAR,"");
        return false;
    }

    if (pNodeInfo->eNodeState == emNodeState_End && pNodeInfo->dwWinnerKinID > 0)
    {
        // 回退比赛队伍得分
        SCupKinInfo* pKinInfo = m_pLegendCup->getCupKinInfo(pNodeInfo->dwWinnerKinID);
        if (pKinInfo == NULL)
        {
            // 队伍都找不到了 这个问题很严重
            ErrorLn(__FUNCTION__": can't find winnerID = "<<pJudgeInfo->dwWinnerKinID);
            return false;
        }
        // 打完节点比赛 战队加分
        if (m_eCompetitionType == emType_GroupCompetition)
        {
            pKinInfo->nGroupScore--;
        }

        if (m_eCompetitionType == emType_KnockoutCompetition)
        {
            pKinInfo->nCurKnockOutScore--;
        }

        // 回退节点比赛 更新战队分数
        DBREQ_REQUEST_UPDATE_CUP_KININFO dbParam;
        dbParam.dwWorldID = getThisGameWorldID();
        dbParam.llLegendCupID = m_llLegendCupID;                      // 杯赛ID
        dbParam.dwKinID = pKinInfo->dwKinID;                            // 战队ID
        dbParam.nGroupScore = pKinInfo->nGroupScore;                        // 小组赛积分情况 
        dbParam.nCurKnockOutScore = pKinInfo->nCurKnockOutScore;                  // 当前淘汰赛积分情况 bo1 bo3 bo5

        if (pDBEngine->executeSP(GAMEDB_REQUEST_UPDATE_CUP_KININFO,0,(LPCSTR)&dbParam,sizeof(dbParam),static_cast<IDBRetSink *>(this)) == false)
        {
            ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_UPDATE_CUP_KININFO failed!");
            return false;
        }
    }

    // 重置节点数据
    MatchHelper matchHelper;
    IMatchService *pMatchService = matchHelper.m_ptr;
    if (pMatchService == NULL)
    {
        ErrorLn(__FUNCTION__":can't find pMatchService == NULL");
        return false;
    }
    // 取出记录过的信息
    // 撤销比赛记录 
    // 1、移除节点房间 2、移除记录m_CompetitionNodeWarInfo信息 2、更新比赛节点信息  4、线下新创建房间
    if(m_mapSerchID2RoomID.find(pJudgeInfo->nSerchID) != m_mapSerchID2RoomID.end())
    {
        // 删除房间
        TraceLn(__FUNCTION__":removeRoom nSerchID ="<<pJudgeInfo->nSerchID<<" llLegendCupID="<<m_llLegendCupID);
        pMatchService->removeLegendCupRoom(m_mapSerchID2RoomID[pJudgeInfo->nSerchID]);
        m_mapSerchID2RoomID.erase(pJudgeInfo->nSerchID);
    }

    // 移除所有WarRec记录
    map<int, map<int,SNodeWarRecord>>::iterator iter = m_CompetitionNodeWarInfo.find(pNodeInfo->nSerchID);
    if ( iter != m_CompetitionNodeWarInfo.end())
    {
        // 移除战场记录相关
        m_CompetitionNodeWarInfo.erase(iter);
    }

    // 更新节点信息
    pNodeInfo->nCurrentBo = 0;
    pNodeInfo->nKin1Score = 0;             // 战队1本次比赛分数
    pNodeInfo->dwKin1TotalKda = 0;         // 战队1总KDA
    pNodeInfo->nKin2Score = 0;             // 战队2本次比赛分数
    pNodeInfo->dwKin2TotalKda = 0;         // 战队2总KDA
    pNodeInfo->eNodeState = emNodeState_WaitStart;             // 节点状态 ECompetitionNodeState
    pNodeInfo->dwWinnerKinID = 0;          // 胜利战队ID
    pNodeInfo->dwBeginTime = pJudgeInfo->dwResetNewTime;            // 比赛节点开始时间
    pNodeInfo->dwEndTime = 0;              // 比赛节点结束时间
    memset(pNodeInfo->WarIDList,0, sizeof(pNodeInfo->WarIDList));
    memset(pNodeInfo->WinKinList,0, sizeof(pNodeInfo->WinKinList));
    memset(pNodeInfo->WinFlag,0, sizeof(pNodeInfo->WinFlag));

    //重新设置比赛得分
    if (pJudgeInfo->nKin1Score > 0)
    {
        if (pJudgeInfo->dwKin1 == pNodeInfo->dwKin1)
        {
            pNodeInfo->nKin1Score = pJudgeInfo->nKin1Score;
        }

        if (pJudgeInfo->dwKin1 == pNodeInfo->dwKin2)
        {
            pNodeInfo->nKin2Score = pJudgeInfo->nKin1Score;
        }
    }

    if (pJudgeInfo->nKin2Score > 0)
    {
        if (pJudgeInfo->dwKin2 == pNodeInfo->dwKin1)
        {
            pNodeInfo->nKin1Score = pJudgeInfo->nKin2Score;
        }

        if (pJudgeInfo->dwKin2 == pNodeInfo->dwKin2)
        {
            pNodeInfo->nKin2Score = pJudgeInfo->nKin2Score;
        }
    }

    // 创建出设置的比分节点信息 并设置裁定为重置
    int nBoCount = 0;
    for(int i = 0; i < pNodeInfo->nKin1Score; i++)
    {
        nBoCount++;
        //生成当前创建的房间节点记录 用于可能在房间状态下就判定比赛胜利相关
        SNodeWarRecord sNodeWarRecord;
        sNodeWarRecord.byWinFlag    = JUDGE_Reset;
        sNodeWarRecord.llWarID      = 0;           
        sNodeWarRecord.dwWinnerKin  = pNodeInfo->dwKin1;            
        sNodeWarRecord.nBoCount    = nBoCount;

        map<int, map<int,SNodeWarRecord>>::iterator iterNodeWarInfo = m_CompetitionNodeWarInfo.find(pNodeInfo->nSerchID);
        if (iterNodeWarInfo == m_CompetitionNodeWarInfo.end())
        {
            m_CompetitionNodeWarInfo[pNodeInfo->nSerchID].insert(pair<int, SNodeWarRecord>(sNodeWarRecord.nBoCount, sNodeWarRecord));
        }
        else
        {
            map<int,SNodeWarRecord>::iterator iterWarRec = iterNodeWarInfo->second.find(nBoCount);
            if (iterWarRec == iterNodeWarInfo->second.end())
            {
                m_CompetitionNodeWarInfo[pNodeInfo->nSerchID].insert(pair<int, SNodeWarRecord>(sNodeWarRecord.nBoCount, sNodeWarRecord));}
            else
            {
                WarningLn(__FUNCTION__":repeat createRoom boCount!!legendCupID = "<<m_llLegendCupID<<" serchID ="<<pNodeInfo->nSerchID);
            }
        
        }
    }

    for(int i = 0; i < pNodeInfo->nKin2Score; i++)
    {
        nBoCount++;
        //生成当前创建的房间节点记录 用于可能在房间状态下就判定比赛胜利相关
        SNodeWarRecord sNodeWarRecord;
        sNodeWarRecord.byWinFlag    = JUDGE_Reset;
        sNodeWarRecord.llWarID      = 0;           
        sNodeWarRecord.dwWinnerKin  = pNodeInfo->dwKin2;            
        sNodeWarRecord.nBoCount    = nBoCount;

        map<int, map<int,SNodeWarRecord>>::iterator iterNodeWarInfo = m_CompetitionNodeWarInfo.find(pNodeInfo->nSerchID);
        if (iterNodeWarInfo == m_CompetitionNodeWarInfo.end())
        {
            m_CompetitionNodeWarInfo[pNodeInfo->nSerchID].insert(pair<int, SNodeWarRecord>(sNodeWarRecord.nBoCount, sNodeWarRecord));
        }
        else
        {
            map<int,SNodeWarRecord>::iterator iterWarRec = iterNodeWarInfo->second.find(nBoCount);
            if (iterWarRec == iterNodeWarInfo->second.end())
            {
                m_CompetitionNodeWarInfo[pNodeInfo->nSerchID].insert(pair<int, SNodeWarRecord>(sNodeWarRecord.nBoCount, sNodeWarRecord));}
            else
            {
                WarningLn(__FUNCTION__":repeat createRoom boCount!!legendCupID = "<<m_llLegendCupID<<" serchID ="<<pNodeInfo->nSerchID);
            }
        
        }
    }

    pNodeInfo->nCurrentBo = pNodeInfo->nKin2Score + pNodeInfo->nKin1Score;

    // 根据得分判定是否能决出胜负了
    if ( pNodeInfo->nKin1Score >= getWinScoreByType(pNodeInfo->byBoType))
    {
        pNodeInfo->dwWinnerKinID = pNodeInfo->dwKin1;
        updateNodeState(pNodeInfo,emNodeState_End);   // 修改节点状态   
    }

    if (pNodeInfo->nKin2Score >= getWinScoreByType(pNodeInfo->byBoType))
    {
        pNodeInfo->dwWinnerKinID = pNodeInfo->dwKin2;
        updateNodeState(pNodeInfo,emNodeState_End);   // 修改节点状态     
    }

    if (pNodeInfo->eNodeState == emNodeState_End)
    {
        // 节点结束了重算得分
        SCupKinInfo* pKinInfo = m_pLegendCup->getCupKinInfo(pNodeInfo->dwWinnerKinID);
        if (pKinInfo == NULL)
        {
            // 队伍都找不到了 这个问题很严重
            ErrorLn(__FUNCTION__": can't find winnerID = "<<pJudgeInfo->dwWinnerKinID);
            return false;
        }
        // 打完节点比赛 战队加分
        if (m_eCompetitionType == emType_GroupCompetition)
        {
            pKinInfo->nGroupScore++;
        }

        if (m_eCompetitionType == emType_KnockoutCompetition)
        {
            pKinInfo->nCurKnockOutScore++;
        }

        // 更新战队分数
        DBREQ_REQUEST_UPDATE_CUP_KININFO dbParam;
        dbParam.dwWorldID = getThisGameWorldID();
        dbParam.llLegendCupID = m_llLegendCupID;                      // 杯赛ID
        dbParam.dwKinID = pKinInfo->dwKinID;                          // 战队ID
        dbParam.nGroupScore = pKinInfo->nGroupScore;                  // 小组赛积分情况 
        dbParam.nCurKnockOutScore = pKinInfo->nCurKnockOutScore;      // 当前淘汰赛积分情况 bo1 bo3 bo5

        if (pDBEngine->executeSP(GAMEDB_REQUEST_UPDATE_CUP_KININFO,0,(LPCSTR)&dbParam,sizeof(dbParam),static_cast<IDBRetSink *>(this)) == false)
        {
            ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_UPDATE_CUP_KININFO failed!");
            return false;
        }
    }

    // 没结束则创建房间
    if (pNodeInfo->eNodeState != emNodeState_End)
    {
        // 重置比赛要设置下一场比赛的时间 不能像正常比赛那样按当前时间创建房间
        createCompetitionNodeRoom(pNodeInfo, true);
    }

    return true;
}

bool CCompetitionRoundBase::onlineJudgeAllOut(SMsgJudgeWarResult* pJudgeInfo, SCompetitionNode* pNodeInfo)
{
    //全部淘汰
    // 1、更新m_CompetitionNodeWarInfo 里面信息 2、修改设置胜利者ID 为 0 3、更新节点状态
    pNodeInfo->dwWinnerKinID = 0;
    updateNodeState(pNodeInfo,emNodeState_End);   // 修改节点状态 

    return true;
}

void CCompetitionRoundBase::updateSingleCupNodeInfo2Client(ClientID clientID, int nKinId)
{
	if (clientID <= 0 || nKinId <= 0)
	{
		ErrorLn(__FUNCTION__": clientID <= 0 || nKinId <= 0, clientID = " << clientID << ", nKinId = " << nKinId);
		return;
	}

    KinHelper helper;
    IKinService *pKinService = helper.m_ptr;
    if (pKinService == NULL)
    {
        ErrorLn(__FUNCTION__": failed pKinService == NULL");
        return;
    }
	
	SCompetitionNode * pNodeInfo = NULL;
	for (map<int, SCompetitionNode>::iterator itNode = m_CompetitionNodeList.begin(); itNode != m_CompetitionNodeList.end(); ++itNode)
	{
        // 小组赛可能有多个节点
		if (itNode->second.dwKin1 == nKinId || itNode->second.dwKin2 == nKinId)
		{
			pNodeInfo = &(itNode->second);
            if (pNodeInfo == NULL)
            {
                continue;
            }

            DWORD dwEnterInterval = 0;
            if (m_pLegendCup->getLegendCupBaseInfo()->bIsOnlineCup)
            {
                dwEnterInterval = pNodeInfo->nCurrentBo < 1 ? m_pLegendCup->getRoundTimeInfo(m_nRoundID)->dwRoomBeginInterval : getBoInterval();
            }

            DWORD dwNowTime = (DWORD)time(NULL);
            obuf ob;
            SNetMsgHead NetMsgHead;
            NetMsgHead.bySrcEndPoint	= MSG_ENDPOINT_SOCIAL;
            NetMsgHead.byDestEndPoint	= MSG_ENDPOINT_CLIENT;
            NetMsgHead.byKeyModule		= MSG_MODULEID_LEGENDCUP;
            NetMsgHead.byKeyAction		= OC_CUP_SEND_UPDATE_SINGEL_CUP_NODE_INFO;
            ob.push_back(&NetMsgHead, sizeof(NetMsgHead));

            ob.push_back(&m_llLegendCupID, sizeof(m_llLegendCupID));

            SMsgCompetitionNodeInfo node;
            node.byCompetitionType  = pNodeInfo->byCompetitionType;   // 1小组赛 2淘汰赛
            node.nSerchID           = pNodeInfo->nSerchID;           // 生成的快速定位ID
            node.nRoundID           = pNodeInfo->nRoundID;           // 第几轮比赛
            node.nGroupID           = pNodeInfo->nGroupID;           // 分配到第几组
            node.nDay               = pNodeInfo->nDay;               // 第几天
            node.nPos               = pNodeInfo->nPos;               // 位置索引 从1开始
            node.dwKin1             = pNodeInfo->dwKin1;             // 比赛战队1
            SKinInfo sKinInfo1      = pKinService->getKinInfo(node.dwKin1);
            sstrcpyn(node.szKin1Name, sKinInfo1.szName, sizeof(node.szKin1Name));
            node.nKin1Score         = pNodeInfo->nKin1Score;         // 战队1本次比赛分数
            node.dwKin2             = pNodeInfo->dwKin2;             // 比赛战队2
            SKinInfo sKinInfo2      = pKinService->getKinInfo(node.dwKin2);
            node.nKin2Score         = pNodeInfo->nKin2Score;         // 战队2本次比赛分数
            sstrcpyn(node.szKin2Name, sKinInfo2.szName, sizeof(node.szKin2Name));
            node.eNodeState         = pNodeInfo->eNodeState;         // 节点状态ECompetitionNodeState
            node.dwWinnerKinID      = pNodeInfo->dwWinnerKinID;      // 胜利战队ID
            node.byBoType           = pNodeInfo->byBoType;           // BO类型 ELegendCupBO
            node.nCurrentBo         = pNodeInfo->nCurrentBo;         // 已经打了多少场
            node.dwBeginTime        = pNodeInfo->dwBeginTime + dwEnterInterval;		// 节点比赛开始时间
            node.dwEnterTime        = pNodeInfo->dwBeginTime;       // 节点进场开始时间
            node.dwServerTime       = (DWORD)time(NULL);		    // 服务器当前时间
            ob.push_back(&node, sizeof(node));

            send2Client(clientID, PACKAGE_PTR(new std::string((char const *)ob.data(), ob.size())));
		}
	}
	
}

void CCompetitionRoundBase::broadCastKinMemberCompetitionInfo(SCompetitionNode* pNodeInfo)
{
    // 广播给所有节点参赛者
    ChatHelper chatHelper;
    IChatService *pChatService = chatHelper.m_ptr;
    if (NULL == pChatService)
    {
        ErrorLn(__FUNCTION__ << "failed! nullptr == pChatService" );
        return;
    }

    list<DWORD> KinMember;
    if (pNodeInfo->dwKin1 > 0)
    {
        // 获取参赛队员
        m_pLegendCup->getCupKinMemberList(pNodeInfo->dwKin1, KinMember);
    }

    if (pNodeInfo->dwKin2 > 0)
    {
        // 获取参赛队员
        m_pLegendCup->getCupKinMemberList(pNodeInfo->dwKin2, KinMember);
    }

    if (KinMember.size() <= 0)
    {
        return;
    }

    // 先把基础数据, 当前轮所在节点信息下发下去
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver == NULL)
    {
        ErrorLn(__FUNCTION__": pShareReceiver == NULL");
        return;
    }

    for (list<DWORD>::iterator iter = KinMember.begin(); iter != KinMember.end(); ++iter)
    {
        DWORD dwActorID = *iter;
        SSharePersonProperty shareProperty= pShareReceiver->GetSharePerson(dwActorID);
        // 在战场里面的玩家不下发
        if( shareProperty.dwPDBID == INVALID_PDBID || shareProperty.dwSceneID > MAP_MAXCOUNT)
        {
            continue;
        }
        m_pLegendCup->updateSingleCupBaseInfo2Client(shareProperty.clientID, shareProperty.dwKinID);

        updateSingleCupNodeInfo2Client(shareProperty.clientID, shareProperty.dwKinID);

        switch(pNodeInfo->eNodeState)
        {
        case emNodeState_CanEnter:
            {
                string strParam = "";
                strParam += CHAT_MSG_ITEM_FLAG;

                char szText[256] = {0};
                ssprintf_s(szText, sizeof(szText), "%d;%s;%s;%lld", CHAT_CHANNEL_KIN, m_pLegendCup->getLegendCupBaseInfo()->szCupName, strParam.c_str(), m_pLegendCup->getLegendCupBaseInfo()->llLegendCupID);
                pChatService->broadcastSystemMessage(CHAT_BROADCASTTYPE_SINGLE, dwActorID, CHAT_TIP_LEGENDCUP_CANENTER_CHAT_INFO, szText);
            }
            break;
        default:
            break;

        }
     }
    
}

SNextWarTimeInfo CCompetitionRoundBase::getNextWarTime(int nCurSerchID, DWORD dwKinID)
{
    SNextWarTimeInfo sNextTimeInfo;
    switch(m_eCompetitionType)
    {
    case emType_KnockoutCompetition:
        {
            // 淘汰赛则直接获取下一轮的比赛时间
            sNextTimeInfo = getKnockoutTypeNextWarTime();
        }
        break;

    case emType_GroupCompetition:
        {
            // 小组赛找下一个比赛节点时间
            sNextTimeInfo = getGroupTypeNextWarTime(nCurSerchID, dwKinID);
        }
        break;

    default:
        break;
    }
    
    return sNextTimeInfo;
}

SNextWarTimeInfo CCompetitionRoundBase::getKnockoutTypeNextWarTime()
{
    // 淘汰赛就获取下一轮比赛时间
    SNextWarTimeInfo sNextInfo;
    SRoundTimeInfo* pRoundTimeInfo = m_pLegendCup->getRoundTimeInfo(m_nRoundID + 1);
    if (pRoundTimeInfo == NULL)
    {
        return  sNextInfo;
    }
    sNextInfo.dwNextWarTime = pRoundTimeInfo->dwRoundCreateRoomTime;
    sNextInfo.bRoundLastNode = true;
    return  sNextInfo;
}

SNextWarTimeInfo CCompetitionRoundBase::getGroupTypeNextWarTime(int nCurSerchID, DWORD dwKinID)
{
    // 找战队所有比赛节点
    SNextWarTimeInfo sNextInfo;
    map<int, SCompetitionNode>::iterator iter = m_CompetitionNodeList.begin();
    for (; iter != m_CompetitionNodeList.end(); ++iter)
    {
        if (iter->second.dwKin1 == dwKinID || iter->second.dwKin2 == dwKinID)
        {
            // 和当前节点判定
            if (nCurSerchID < iter->first && iter->second.eNodeState != emNodeState_End)
            {
                sNextInfo.bRoundLastNode = false;
                sNextInfo.dwNextWarTime = iter->second.dwBeginTime;
                return sNextInfo;
            }
        }
    }

    // 没找到就取下一轮比赛的进入时间
    sNextInfo.bRoundLastNode = true;
    sNextInfo.dwNextWarTime = 0;
    SRoundTimeInfo* pRoundTimeInfo = m_pLegendCup->getRoundTimeInfo(m_nRoundID + 1);
    if (pRoundTimeInfo == NULL)
    {
        return  sNextInfo;
    }

    sNextInfo.dwNextWarTime = pRoundTimeInfo->dwRoundCreateRoomTime;
    return  sNextInfo;
}
