#include "stdafx.h"
#include "KnockoutCompetitionRound.h"
#include "SharePropertyDef.h"
#include "IShareServer.h"
#include "IGatewayAcceptorService.h"

CKnockoutCompetitionRound::CKnockoutCompetitionRound() 
{	
    IDBEngineService* pBEngineService = gSocialGlobal->getDBEngineService();
    if (pBEngineService != NULL)
    {
        pBEngineService->registerHandler(this);
    }
}

CKnockoutCompetitionRound::~CKnockoutCompetitionRound()
{
    
}

void CKnockoutCompetitionRound::Release()
{
    IDBEngineService* pBEngineService = gSocialGlobal->getDBEngineService();
    if (pBEngineService != NULL)
    {
        pBEngineService->unregisterHandler(this);
    }
	delete this;
}

bool CKnockoutCompetitionRound::WriteDataToDB()
{
	return true;
}


bool CKnockoutCompetitionRound::bNodeKinIsBye(SCompetitionNode *pNode)
{
    if (pNode == NULL)
    {
        return false;
    }

    if (pNode->dwKin1 == 0 || pNode->dwKin2 == 0)
    {
        return true;
    }

    return false;
}


bool CKnockoutCompetitionRound::endRound()
{
    IDBEngineService* pDBEngine = gSocialGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        ErrorLn(__FUNCTION__": pDBEngine == NULL");
        return false; 
    }
    // 强制结束没有打完的比赛节点
    map<int, SCompetitionNode>::iterator iter = m_CompetitionNodeList.begin();
    for (;iter != m_CompetitionNodeList.end(); ++iter)
    {
        SCompetitionNode *pNode = &iter->second;
        // 轮空判定 轮空的淘汰赛节点已经在创建的时候设置了胜利队伍了
        if (!bNodeKinIsBye(pNode))
        {
            // 两个队伍都没有参加比赛
            if ( pNode->nKin1Score == 0 && pNode->nKin2Score == 0)
            {
                //淘汰赛一局都没有打的
                pNode->dwWinnerKinID = 0;
            }
            else
            {
                if (pNode->nKin1Score > pNode->nKin2Score)
                {
                    pNode->dwWinnerKinID = pNode->dwKin1;
                }

                if (pNode->nKin1Score < pNode->nKin2Score)
                {
                    pNode->dwWinnerKinID = pNode->dwKin2;
                }
                // 修改小组赛得分
                if (pNode->eNodeState != emNodeState_End)
                {

                    // 修改杯赛记录的队伍得分
                    SCupKinInfo* pKinInfo = m_pLegendCup->getCupKinInfo(pNode->dwWinnerKinID);
                    if (pKinInfo == NULL)
                    {
                        // 队伍都找不到了 这个问题很严重
                        ErrorLn(__FUNCTION__": can't find winnerID = "<<pNode->dwWinnerKinID);
                        return false;
                    }
                    //打完节点比赛 战队加分
                    pKinInfo->nCurKnockOutScore++;

                    //打完节点比赛  更新战队分数到DB
                    DBREQ_REQUEST_UPDATE_CUP_KININFO dbParam;
                    dbParam.dwWorldID = getThisGameWorldID();
                    dbParam.llLegendCupID = m_llLegendCupID;                      // 杯赛ID
                    dbParam.dwKinID = pKinInfo->dwKinID;                            // 战队ID
                    dbParam.nGroupScore = pKinInfo->nGroupScore;                        // 小组赛积分情况 
                    dbParam.nCurKnockOutScore = pKinInfo->nCurKnockOutScore;        // 当前淘汰赛积分情况 bo1 bo3 bo5

                    if (pDBEngine->executeSP(GAMEDB_REQUEST_UPDATE_CUP_KININFO,0,(LPCSTR)&dbParam,sizeof(dbParam),static_cast<IDBRetSink *>(this) ) == false)
                    {
                        ErrorLn(__FUNCTION__": pDBEngine->executeSP()  GAMEDB_REQUEST_UPDATE_CUP_KININFO failed!");
                        return false;
                    }
                }
            }
        }

        pNode->eNodeState = emNodeState_End;
        pNode->dwEndTime = (DWORD)time(NULL);

        // 保存节点信息到DB
        saveCompetitionNodeToDB(pNode);
    }

    // 淘汰赛 按顺序生成下一轮晋级列表 按比赛节点的获胜队伍来获得晋级队伍 若双方都弃权的 则压入战队ID为 0 占位
    // 决赛则是排名顺序
    list<DWORD> promotionList;  // 胜利者
    list<DWORD> weedOutList;    // 淘汰的战队
    if (m_nMaxGroup * m_nGroupMaxKin > 4)
    {
        // 四强赛前晋级直接判定胜利者
        map<int, SCompetitionNode>::iterator iterPromotion = m_CompetitionNodeList.begin();
        for (;iterPromotion != m_CompetitionNodeList.end(); ++iterPromotion)
        {
            // 晋级战队
            promotionList.push_back(iterPromotion->second.dwWinnerKinID);

            // 淘汰战队
            if (iterPromotion->second.dwWinnerKinID == iterPromotion->second.dwKin1)
            {
                weedOutList.push_back(iterPromotion->second.dwKin2);
            }
            else if (iterPromotion->second.dwWinnerKinID == iterPromotion->second.dwKin2)
            {
                weedOutList.push_back(iterPromotion->second.dwKin1);
            }
            else
            {
                weedOutList.push_back(0);
            }
        }
    }
    else if (m_nMaxGroup * m_nGroupMaxKin == 4)
    {
        // 四强得打季军赛 把争夺冠军两个队伍和季军两个队伍分到同一组
        map<int, SCompetitionNode>::iterator iterPromotion = m_CompetitionNodeList.begin();
        for (;iterPromotion != m_CompetitionNodeList.end(); ++iterPromotion)
        {
            promotionList.push_back(iterPromotion->second.dwWinnerKinID);

            if (iterPromotion->second.dwWinnerKinID == iterPromotion->second.dwKin1)
            {
                weedOutList.push_back(iterPromotion->second.dwKin2);
            }
            else if (iterPromotion->second.dwWinnerKinID == iterPromotion->second.dwKin2)
            {
                weedOutList.push_back(iterPromotion->second.dwKin1);
            }
            else
            {
                weedOutList.push_back(0);
            }
        }

        // 再插入败者组 
        promotionList.insert(promotionList.end(),weedOutList.begin(),weedOutList.end());
    }
    else if (m_nMaxGroup * m_nGroupMaxKin < 4)
    {
        // 决赛排名处理
        map<int, SCompetitionNode>::iterator iterPromotion = m_CompetitionNodeList.begin();
        for (;iterPromotion != m_CompetitionNodeList.end(); ++iterPromotion)
        {
            // 最终排名列表 
            // 1、线上 不参加比赛的不记录名次
            if (iterPromotion->second.dwWinnerKinID > 0)
            {
                promotionList.push_back(iterPromotion->second.dwWinnerKinID);
                if (iterPromotion->second.dwWinnerKinID == iterPromotion->second.dwKin1)
                {
                    promotionList.push_back(iterPromotion->second.dwKin2);
                }

                if (iterPromotion->second.dwWinnerKinID == iterPromotion->second.dwKin2)
                {
                    promotionList.push_back(iterPromotion->second.dwKin1);
                }
            }
        }
    }

    LEDENDCUP_TRACELN("--------------------------------["<<m_pLegendCup->getLegendCupBaseInfo()->szCupName<<"]KnockOutendRound["<<m_nRoundID<<"]-----------------------------");
    KinHelper helper;
    IKinService *pKinService = helper.m_ptr;
    if (pKinService != NULL)
    {
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

    // 通知杯赛进入下一轮比赛
    CCompetitionRoundBase::endRound(promotionList,weedOutList);

    return true;
}

bool CKnockoutCompetitionRound::setKinListAndGroup(list<DWORD> dwKinList,bool bBreakSort /*= false*/,bool bStartRound /*= false*/)
{
    //分组
    if (!groupKinList(m_nRoundID, m_nMaxGroup, m_nGroupMaxKin, dwKinList, bStartRound))
    {
        ErrorLn(__FUNCTION__": group kin list error! ");
        return false;
    }
    return true;
}

bool CKnockoutCompetitionRound::groupKinList(int nRoundID, int nMaxGroup, int nGroupMaxKin,list<DWORD> KinList, bool bStartRound /*= false*/)
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
        ErrorLn(__FUNCTION__": KinIDList size too small");
        return false;
    }

    if (KinList.size() == 0)
    {
        return false;
    }

    //决赛因为要打季军赛特殊处理下分组
    int nFinalMaxGroup = m_nMaxGroup;
    if (isFinalRound())
    {
        nFinalMaxGroup  = m_nMaxGroup + 1;
    }

    if (bStartRound)
    {
        // 开始就定位到本轮淘汰赛
        list<DWORD>::iterator iter = KinList.begin();
        // 淘汰赛和小组赛分组方式不同
        for (int t = 1; t <= m_nGroupMaxKin; ++t)
        {
            for (int i = 1; i <= nFinalMaxGroup; ++i)
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
    }
    else
    {
        list<DWORD>::iterator iter = KinList.begin();
        // 淘汰赛和小组赛分组方式不同
        for (int t = 1; t <= nFinalMaxGroup; ++t)
        {
            for (int i = 1; i <= m_nGroupMaxKin; ++i)
            {
                if(iter == KinList.end())
                {
                    map_RoundGroup[t][i] = 0;
                }
                else
                {
                    map_RoundGroup[t][i] = *iter;
                    ++iter;
                }
            }
        }
    }

    // DB位置保存相关
    int DBPos = 0;
    for (int t = 1; t <= nFinalMaxGroup; ++t)
    {
        for (int i = 1; i <= m_nGroupMaxKin; ++i)
        {
            dbInfo.KinIDList[DBPos++] = map_RoundGroup[t][i];
        }   
    }

    LEDENDCUP_TRACELN("-----------------------["<<m_pLegendCup->getLegendCupBaseInfo()->szCupName<<"]CKnockoutCompetitionRound::groupKinList---------------------");
    KinHelper helper;
    IKinService *pKinService = helper.m_ptr;
    if (pKinService != NULL)
    {
        map<int, map<int, DWORD>>::iterator iterGroup = map_RoundGroup.begin();
        for (; iterGroup != map_RoundGroup.end(); ++iterGroup)
        {
            // 晋级列表
            LEDENDCUP_TRACELN(__FUNCTION__": llLegendCupID = "<< m_llLegendCupID<<" nRoundID ="<<m_nRoundID<<" groupID ="<<iterGroup->first);
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

        for (int pos = 0;pos < nFinalMaxGroup * m_nGroupMaxKin; ++pos)
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

bool CKnockoutCompetitionRound::judgeNodeWarResult(SMsgJudgeWarResult* pJudgeInfo)
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
