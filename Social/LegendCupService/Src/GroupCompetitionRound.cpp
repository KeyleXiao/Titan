#include "stdafx.h"
#include "GroupCompetitionRound.h"
#include "SharePropertyDef.h"
#include "IShareServer.h"
#include "IGatewayAcceptorService.h"

CGroupCompetitionRound::CGroupCompetitionRound()  
{	
    IDBEngineService* pBEngineService = gSocialGlobal->getDBEngineService();
    if (pBEngineService != NULL)
    {
        pBEngineService->registerHandler(this);
    }
}

CGroupCompetitionRound::~CGroupCompetitionRound()
{
    
}

void CGroupCompetitionRound::Release()
{
    IDBEngineService* pBEngineService = gSocialGlobal->getDBEngineService();
    if (pBEngineService != NULL)
    {
        pBEngineService->unregisterHandler(this);
    }
	delete this;
}

bool CGroupCompetitionRound::endRound()
{
    map<DWORD, DWORD> KinTotalKDA;
    // 强制结束没有打完的比赛节点
    for (map<int, SCompetitionNode>::iterator iter = m_CompetitionNodeList.begin();iter != m_CompetitionNodeList.end(); ++iter)
    {
        SCompetitionNode *pNode = &iter->second;
        if (pNode->nKin1Score > pNode->nKin2Score)
        {
            pNode->dwWinnerKinID = pNode->dwKin1;
        }

        if (pNode->nKin1Score < pNode->nKin2Score)
        {
            pNode->dwWinnerKinID = pNode->dwKin2;
        }

        if (pNode->nKin1Score == pNode->nKin2Score)
        {
            if (pNode->dwKin1TotalKda > pNode->dwKin2TotalKda)
            {
                pNode->dwWinnerKinID = pNode->dwKin1;
            }
            else
            {
                pNode->dwWinnerKinID = pNode->dwKin2;
            }
        }

        // 修改小组赛得分
        if (pNode->eNodeState != emNodeState_End)
        {
            if (pNode->dwWinnerKinID > 0)
            {
                // 打完节点比赛 修改战队得分
                SCupKinInfo* pKinInfo = m_pLegendCup->getCupKinInfo(pNode->dwWinnerKinID);
                if (pKinInfo != NULL)
                {
                    pKinInfo->nGroupScore++;

                    //打完节点比赛 更新战队分数
                    IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
                    if (pDBEngine != NULL)
                    {
                        DBREQ_REQUEST_UPDATE_CUP_KININFO dbParam;
                        dbParam.dwWorldID = getThisGameWorldID();
                        dbParam.llLegendCupID = m_llLegendCupID;                      // 杯赛ID
                        dbParam.dwKinID = pKinInfo->dwKinID;                          // 战队ID
                        dbParam.nGroupScore = pKinInfo->nGroupScore;                  // 小组赛积分情况 
                        dbParam.nCurKnockOutScore = pKinInfo->nCurKnockOutScore;      // 当前淘汰赛积分情况 bo1 bo3 bo5

                        if (pDBEngine->executeSP(GAMEDB_REQUEST_UPDATE_CUP_KININFO,0,(LPCSTR)&dbParam,sizeof(dbParam),static_cast<IDBRetSink *>(this)) == false)
                        {
                            ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_UPDATE_CUP_KININFO failed!");
                        }
                    }
                }
                else
                {
                    KinHelper helper;
                    IKinService *pKinService = helper.m_ptr;
                    SKinInfo sKin1Info = pKinService->getKinInfo(pNode->dwKin1);
                    SKinInfo sKin2Info = pKinService->getKinInfo(pNode->dwKin2);
                    WarningLn(__FUNCTION__": 【"<<sKin1Info.szName<< "】 VS 【 "<<sKin2Info.szName<<"】");
                    WarningLn(__FUNCTION__": winnerID = "<<pNode->dwWinnerKinID<< " serchID ="<<pNode->nSerchID);
                    

                }
            }

        }
        //记录每队本轮比赛的总KDA
        KinTotalKDA[pNode->dwKin1] += pNode->dwKin1TotalKda;
        KinTotalKDA[pNode->dwKin2] += pNode->dwKin2TotalKda;

        pNode->eNodeState = emNodeState_End;
        pNode->dwEndTime = (DWORD)time(NULL);

        // 保存节点信息到DB
        saveCompetitionNodeToDB(pNode);
    }

    
    // 构造每组算分信息
    map<int, list<SCompetitionScoreInfo>> mapGroupsScoreInfo;
    // 分组记录分数nGroupScore * 10000 + KDA 每组排序
    map<int, map<int,DWORD>>::iterator iterGroup = map_RoundGroup.begin(); 
    for (;iterGroup != map_RoundGroup.end(); ++iterGroup)
    {
        map<int, DWORD>::iterator iterKin = iterGroup->second.begin();
        for(;iterKin != iterGroup->second.end(); ++iterKin)
        {
            if (iterKin->second == 0)
            {
                continue;
            }
            // 每组分配的位置信息遍历
            SCompetitionScoreInfo nodeScoreInfo;
            nodeScoreInfo.dwKinID = iterKin->second;                            // 战队ID
            SCupKinInfo* pKinInfo = m_pLegendCup->getCupKinInfo(iterKin->second);
            if (pKinInfo == NULL)
            {
               nodeScoreInfo.dwNodeScore = 0;                                   // 比赛胜利得分
            }
            else
            {
                nodeScoreInfo.dwNodeScore = pKinInfo->nGroupScore;              // 比赛胜利得分
            }
            nodeScoreInfo.dwKDA = KinTotalKDA[iterKin->second];                 // KDA
            mapGroupsScoreInfo[iterGroup->first].push_back(nodeScoreInfo);

        }
    }
    //对小组赛每组的得分进行排序
    list<DWORD> promotionList;  // 晋级的战队
    list<DWORD> weedOutList;    // 淘汰的战队
    for(map<int, list<SCompetitionScoreInfo>>::iterator iter = mapGroupsScoreInfo.begin();iter != mapGroupsScoreInfo.end(); ++iter)
    {
        // 对战队得分排序
        iter->second.sort();
        
        // 倒序确定晋级列表
        int nPromotionCount = m_nGroupPromotionCount;
        list<SCompetitionScoreInfo>::reverse_iterator iterPromotion = iter->second.rbegin();
        for (; iterPromotion!=iter->second.rend(); ++iterPromotion)
        { 
            // 节点得分为0 说明没有参赛 不让晋级
            if (iterPromotion->dwNodeScore == 0)
            {
                continue;
            }

            if (nPromotionCount > 0)
            {
                promotionList.push_back(iterPromotion->dwKinID);
            }
            else
            {
                weedOutList.push_back(iterPromotion->dwKinID);
            }
            nPromotionCount--;
        }
    }

    LEDENDCUP_TRACELN("---------------------------["<<m_pLegendCup->getLegendCupBaseInfo()->szCupName<<"]GroupEndRound["<<m_nRoundID<<"]------------------------");
    KinHelper helper;
    IKinService *pKinService = helper.m_ptr;
    if (pKinService != NULL)
    {
        if (promotionList.size() == 0)
        {
            WarningLn(__FUNCTION__":  llLegendCupID = "<< m_llLegendCupID<<" nRoundID ="<<m_nRoundID<<" promotionList count == 0");
        }

        for (list<DWORD>::iterator iter = promotionList.begin(); iter != promotionList.end(); ++iter)
        {
            // 晋级列表
            LEDENDCUP_TRACELN(__FUNCTION__": llLegendCupID = "<< m_llLegendCupID<<" nRoundID ="<<m_nRoundID<<" promotionList")
                SKinInfo sKinInfo = pKinService->getKinInfo(*iter);
            if (sKinInfo.nKinProp_ID > 0)
            {
                LEDENDCUP_TRACELN(__FUNCTION__": KinID = "<<sKinInfo.nKinProp_ID<<" name ="<<sKinInfo.szName);
            }
            else
            {
                LEDENDCUP_TRACELN(__FUNCTION__": KinID = "<<sKinInfo.nKinProp_ID<<" name = (NULL)");
            }
        }

        
    }
    LEDENDCUP_TRACELN("----------------------------------------------------------------------------------------------------------------------");

    // 每组晋级的数量 小组赛按组判定晋级
    CCompetitionRoundBase::endRound(promotionList,weedOutList);
    return true;
}

bool CGroupCompetitionRound::setKinListAndGroup(list<DWORD> dwKinList,bool bBreakSort /*= false*/,bool bStartRound /*= false*/)
{
    //分组
    if (!groupKinList(m_nRoundID, m_nMaxGroup, m_nGroupMaxKin, dwKinList, bStartRound))
    {
        ErrorLn(__FUNCTION__": group kin list error! ");
        return false;
    }
    return true;
}

bool CGroupCompetitionRound::groupKinList(int nRoundID, int nMaxGroup, int nGroupMaxKin,list<DWORD> KinList, bool bStartRound /*= false*/)
{
    //分组位置信息保存到数据库
    obuf ob;
    DBREQ_REQUEST_SAVE_ROUND_GROUP_KIN_POS dbInfo;
    dbInfo.dwWorldID = getThisGameWorldID();
    dbInfo.llLegendCupID = m_llLegendCupID;
    dbInfo.nRoundID = nRoundID;
    memset(&(dbInfo.KinIDList), 0, sizeof(dbInfo.KinIDList));
    if (sizeof(dbInfo.KinIDList) < sizeof(DWORD) * nMaxGroup * nGroupMaxKin)
    {
        ErrorLn(__FUNCTION__"DBREQ_REQUEST_SAVE_CUP_ROUND_GROUP  KinIDList size too small");
        return false;
    }

    // 小组赛分组
    if (KinList.size() == 0)
    {
        return false;
    }

    list<DWORD>::iterator iter = KinList.begin();
    // 淘汰赛和小组赛分组方式不同
    for (int t = 1; t <= m_nGroupMaxKin; ++t)
    {
        for (int i = 1; i <= m_nMaxGroup; ++i)
        {
            // 单个组先分配
            if(iter == KinList.end())
            {
                map_RoundGroup[i][t] = 0;
            }
            else
            {
                map_RoundGroup[i][t] = *iter;
                ++iter;
            }
        }
    }

    int DBPos = 0;
    for (int t = 1; t <= m_nMaxGroup; ++t)
    {
        for (int i = 1; i <= m_nGroupMaxKin; ++i)
        {
            dbInfo.KinIDList[DBPos++] = map_RoundGroup[t][i];
        }   
    }


    LEDENDCUP_TRACELN("-------------------------["<<m_pLegendCup->getLegendCupBaseInfo()->szCupName<<"]CGroupCompetitionRound::groupKinList------------------------");
    KinHelper helper;
    IKinService *pKinService = helper.m_ptr;
    if (pKinService != NULL)
    {
        map<int, map<int, DWORD>>::iterator iterGroup = map_RoundGroup.begin();
        // 晋级列表
        LEDENDCUP_TRACELN(__FUNCTION__": llLegendCupID = "<< m_llLegendCupID<<" nRoundID ="<<m_nRoundID<<" groupID ="<<iterGroup->first);
        for (; iterGroup != map_RoundGroup.end(); ++iterGroup)
        {
            for (map<int, DWORD>::iterator iterList = iterGroup->second.begin();iterList != iterGroup->second.end(); ++iterList)
            {
                SKinInfo sKinInfo = pKinService->getKinInfo(iterList->second);
                if (sKinInfo.nKinProp_ID > 0)
                {
                    LEDENDCUP_TRACELN(__FUNCTION__": KinID = "<<sKinInfo.nKinProp_ID<<" name ="<<sKinInfo.szName);
                }
                else
                {
                    LEDENDCUP_TRACELN(__FUNCTION__": KinID = "<<sKinInfo.nKinProp_ID<<" name =(NULL)");
                }
            }
        }

        for (int pos = 0;pos < m_nGroupMaxKin*m_nGroupMaxKin; ++pos)
        {
            LEDENDCUP_TRACELN(__FUNCTION__": DBPos ="<<pos<<" KinID ="<<dbInfo.KinIDList[pos]);
        }
    }
    LEDENDCUP_TRACELN("----------------------------------------------------------------------------------------------------------------------");

    // 分组信息保存到数据库
    ob<<dbInfo;
    IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
    if (pDBEngine)
    {
        if (pDBEngine->executeSP(GAMEDB_REQUEST_SAVE_ROUND_GROUP_KIN_POS,0, ob.data(),ob.size(),static_cast<IDBRetSink *>(this)) == false)
        {
            ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_SAVE_ROUND_GROUP_KIN_POS failed!");
        }
    }

    return true;
}

bool CGroupCompetitionRound::judgeNodeWarResult(SMsgJudgeWarResult* pJudgeInfo)
{
    
    if (!CCompetitionRoundBase::judgeNodeWarResult(pJudgeInfo))
    {
        return false;
    }

    if (bOfflineCupCanEndRound())
    {
        endRound();
    }

    return true;
    
}
