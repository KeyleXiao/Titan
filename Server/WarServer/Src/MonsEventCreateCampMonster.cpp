#include "MonsEventCreateCampMonster.h"
#include "IServerGlobal.h"
#include "ITimerService.h"
#include "IWarMonsterMgr.h"

CMonsEventCreateCampMonster::CMonsEventCreateCampMonster(void)
{
}


CMonsEventCreateCampMonster::~CMonsEventCreateCampMonster(void)
{
}

bool CMonsEventCreateCampMonster::Init(IWar*pWar, IMonsterExec *pMonsterExec)
{
    if(!pWar )
    {
        ErrorLn(__FUNCTION__": pWar == NULL");
        return false;
    }

    if (!pMonsterExec)
    {
        ErrorLn(__FUNCTION__": pMonsterExec == NULL");
        return false;
    }

    m_pWar = pWar;
    m_pMonsterExec = pMonsterExec;
    return true;
}


bool CMonsEventCreateCampMonster::release()
{
    list<DWORD>::iterator iter =  m_CreateMonsterTimer.begin();
    for(;iter != m_CreateMonsterTimer.end(); ++iter)
    {
        gServerGlobal->getTimerService()->KillTimer(*iter, this);
    }

    m_pWar = NULL;
    m_pMonsterExec = NULL;
    return true;
}

bool CMonsEventCreateCampMonster::handleSlefMonsterEvent(SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn /*= true*/)
{
    if (DataEx == NULL || nSize != sizeof(int))
    {
        ErrorLn(__FUNCTION__": DataEx == NULL || nSize != sizeof(int) EWIE_CreateCampMonster");
        return false;
    }

    MonsterRefreshVect* pMonsterRefresh = m_pMonsterExec->getMonsterRefresh();
    if (pMonsterRefresh == NULL)
    {
        ErrorLn(__FUNCTION__": pMonsterRefresh == NULL")
        return false;
    }

    int *pCamp = (int *)DataEx;
    MonsterRefreshVect::iterator refreshIt = pMonsterRefresh->begin();
    for(; refreshIt != pMonsterRefresh->end(); ++refreshIt )
    {
        if (refreshIt->RefreshWarInfo.nIndex == eventInfo.nContIndex && refreshIt->RefreshWarInfo.nCamp == *pCamp)
        {
            if (eventInfo.nParm > 0)
            {
                if(gServerGlobal->getTimerService()->SetTimer(eventInfo.nContIndex, eventInfo.nParm, this,1,__FUNCTION__))
                {
                    //记录起来
                    m_CreateMonsterTimer.push_back(eventInfo.nContIndex);
                }
            }
            else
            {
                WarEventCreateNPC(eventInfo.nContIndex);
            }
        }
    }
    return true;
}

void CMonsEventCreateCampMonster::OnTimer(unsigned long dwTimerID)
{
    WarEventCreateNPC(dwTimerID);
}

void CMonsEventCreateCampMonster::WarEventCreateNPC(int nIndex)
{
	IWarMiscManager* warMiscMgr = m_pWar->getMiscManager();
	if (warMiscMgr == NULL)
	{
		ErrorLn(__FUNCTION__":warMiscMgr == NULL");
		return;
	}
    
	IWarMonsterMgr* pWarMonsterMgr = (IWarMonsterMgr*)m_pWar->getWarMgr(Mgr_Monster);
	if (pWarMonsterMgr == NULL)
	{
		ErrorLn(__FUNCTION__":pWarMonsterMgr == NULL");
		return;
	}

    // 创建怪物
    SceneLogicHelper helper(m_pWar->getWarSceneID());
    ISceneLogicService* pSceneLogicService = helper.m_ptr;
    if(pSceneLogicService == NULL)
    {
        return ;
    }

    INPCService* pNpcService = pSceneLogicService->getNPCService();
    if(pNpcService == NULL)
    {
        return ;
    }

    ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
    if(pSchemeCenter == NULL)
    {
        ErrorLn(__FUNCTION__":pSchemeCenter == NULL");
        return ;
    }
    // 取得战场人物配置来获得气势信息
    const SchemeWarPersonConfigInfo* const pSchemeWarPersonConfigInfo = pSchemeCenter->getSchemeWarPersonConfigInfo()->GetSchemeWarPersonConfigInfo(m_pWar->getStaticID());
    if (pSchemeWarPersonConfigInfo == NULL)
    {
        ErrorLn(__FUNCTION__":pSchemeWarPersonConfigInfo == NULL");
        return;
    }

    MonsterRefreshVect* pMonsterRefresh = m_pMonsterExec->getMonsterRefresh();
    if (pMonsterRefresh == NULL)
    {
        ErrorLn(__FUNCTION__": pMonsterRefresh == NULL")
        return;
    }

    MonsterRefreshVect::iterator refreshIt = pMonsterRefresh->begin();
    for(; refreshIt != pMonsterRefresh->end(); ++refreshIt )
    {
        if (refreshIt->RefreshWarInfo.nIndex == nIndex )
        {
            WarPointVect* pWarPointvect = NULL;
            int nRefreshIndex = -1;
            bool bPosRandom = false;
            switch(refreshIt->RefreshWarInfo.eMonsterRefreshType)
            {
            case EWMRT_Normal:
                {
                    // 正常刷怪
                    map<int,WarPointVect>::iterator iter = refreshIt->mapWarPointInfo.begin();
                    if (iter != refreshIt->mapWarPointInfo.end())
                    {
                        pWarPointvect =  &(iter->second);
                    }
                    nRefreshIndex = 0;
                }
                break;
            case EWMRT_Random:
                {
                    // 随机刷怪
                    if(refreshIt->mapWarPointInfo.size() != 0)
                    {
                        nRefreshIndex = (mt_range_rand(1,time(NULL)))%refreshIt->mapWarPointInfo.size();
                        if (refreshIt->mapWarPointInfo.find(nRefreshIndex) != refreshIt->mapWarPointInfo.end())
                        {
                            pWarPointvect = &(refreshIt->mapWarPointInfo[nRefreshIndex]);
                        }
                        else
                        {
                            nRefreshIndex = -1;
                        }
                    }

                }
                break;
            case EWMRT_Interval:
                {
                    // 找时间段索引
                    DWORD dwCotinueTime = m_pWar->getWarPlayTime() /1000;
                    for (int i = 0; i <refreshIt->RefreshWarInfo.nIntervalCount; ++i )
                    {
                        if ( dwCotinueTime >  (DWORD) (refreshIt->RefreshWarInfo.nInterval[i]))
                        {
                            // 配置里的 时段数量 + 1 = 怪物索引列表
                            // 大于当前的时段就要 刷新位置索引 + 1 
                            nRefreshIndex = i + 1;
                        }
                    }

                    if (refreshIt->mapWarPointInfo.find(nRefreshIndex) != refreshIt->mapWarPointInfo.end())
                    {
                        pWarPointvect = &(refreshIt->mapWarPointInfo[nRefreshIndex]);
                    }

                }
                break;
            case EWMRT_RandomInterval:
                {
                    // 找时间段索引
                    int nIntervalIndex = 0;
                    DWORD dwCotinueTime = m_pWar->getWarPlayTime() /1000;

                    for (int i = 0; i <refreshIt->RefreshWarInfo.nIntervalCount; ++i )
                    {
                        if ( dwCotinueTime >  (DWORD)(refreshIt->RefreshWarInfo.nInterval[i]))
                        {
                            nIntervalIndex = i + 1;
                        }
                    }

                    int nRatioBeginPos = nIntervalIndex * refreshIt->RefreshWarInfo.nMonsterIndexCount;
                    int nRatioEndPos = (nIntervalIndex + 1) * refreshIt->RefreshWarInfo.nMonsterIndexCount;

                    // 随机概率 权重
                    int nTotal = 0;
                    for (int i = nRatioBeginPos; i < nRatioEndPos; ++i)
                    {
                        nTotal += refreshIt->RefreshWarInfo.nRatioList[i];
                    }
                    // 随机数 - 每个怪物索引权重 < 0 则就 取这个位置的怪物索引
                    int nRandom = (mt_range_rand(1,nTotal));
                    for (int i = nRatioBeginPos; i < nRatioEndPos; ++i)
                    {
                        
                        // 是否刷这个怪物索引判定
                        nRandom -= refreshIt->RefreshWarInfo.nRatioList[i];
                        if (nRandom <= 0)
                        {
                            break;
                        }
                        nRefreshIndex++;
                    }

                    if (refreshIt->mapWarPointInfo.find(nRefreshIndex) != refreshIt->mapWarPointInfo.end())
                    {
                        pWarPointvect = &(refreshIt->mapWarPointInfo[nRefreshIndex]);
                    }
                }
                break;
            case EWMRT_PosAndMonsterRandom:
                {
                    // 位置随机刷怪
                    bPosRandom = true;
                    if(refreshIt->mapWarPointInfo.size() != 0)
                    {
                        nRefreshIndex = (mt_range_rand(1,time(NULL)))%refreshIt->mapWarPointInfo.size();
                        if (refreshIt->mapWarPointInfo.find(nRefreshIndex) != refreshIt->mapWarPointInfo.end())
                        {
                            pWarPointvect = &(refreshIt->mapWarPointInfo[nRefreshIndex]);
                        }
                    }

                }
                break;
            default:
                break;
            }

            if (pWarPointvect == NULL)
            {
                ErrorLn(__FUNCTION__": pWarPointvect == NULL eMonsterRefreshType ="<<refreshIt->RefreshWarInfo.eMonsterRefreshType
                    <<" WarInfoindex ="<<refreshIt->RefreshWarInfo.nIndex
                    <<" refreshIndex ="<<nRefreshIndex);
                return;
            }

            // 刷新时间到，对怪物进行刷新, 并订阅死亡事件
            refreshIt->nRefreshIndex = nRefreshIndex;
            ISchemeAIIDTable* pAIIDTable = gServerGlobal->getSchemeCenter()->getAISchemeIDTable();
            WarPointVect::iterator itPoint = pWarPointvect->begin();
            for(; itPoint != pWarPointvect->end(); ++itPoint)
            {
                int nPosIndex = 0;
                Vector3 loc = Vector3(itPoint->fX[nPosIndex], itPoint->fY[nPosIndex], itPoint->fZ[nPosIndex]);
                if(bPosRandom && itPoint->nPosCount > 0)
                {
                    nPosIndex = (mt_range_rand(1,time(NULL)))%itPoint->nPosCount;
                    loc = Vector3(itPoint->fX[nPosIndex], itPoint->fY[nPosIndex], itPoint->fZ[nPosIndex]);
                }
                refreshIt->nPosIndex = nPosIndex;
                CreateNpcContext npcContext;
                npcContext.nWarID = m_pWar->getWarID();
                npcContext.nCamp = refreshIt->RefreshWarInfo.nCamp;
                npcContext.nAIID = pAIIDTable == NULL? 0: pAIIDTable->getNpcAISchemeID(m_pWar->getWarTypeId(), itPoint->nMonsterID);
                npcContext.nPathID = refreshIt->RefreshWarInfo.nPathID;
                npcContext.Angle = Vector3(itPoint->fAX, itPoint->fAY, itPoint->fAZ);
                npcContext.nLevel = warMiscMgr->getCreateLevel(refreshIt->RefreshWarInfo.nBaseLevel, refreshIt->RefreshWarInfo.nStartTime,refreshIt->RefreshWarInfo.nUpgradeTime, refreshIt->RefreshWarInfo.nMaxLevel);
                
                UID uID = pWarMonsterMgr->createNpc(itPoint->nMonsterID, loc, m_pMonsterExec->getMonsterType(), &npcContext, itPoint->nIndex, itPoint->nLocalIndex, pNpcService);
                refreshIt->creatMonUIDList[uID] = itPoint->nLocalIndex;

                //创建广播
                if (itPoint->bNeedBroadCast)
                {
                    for(int i = 0; i < MAX_WAR_BROADCAST_TYPE_COUNT; ++i)
                    {
                        if ((itPoint->BroadCastTypeList[i].eBroadCastType & EWBCT_Relive) == EWBCT_Relive)
                        {
                            msg_war_important_monster_reliveinfo MonsterReliveInfo;
                            MonsterReliveInfo.MasterEntityType = TYPE_MONSTER;
                            MonsterReliveInfo.MasterHeroID = itPoint->nMonsterID;
                            MonsterReliveInfo.nCamp = refreshIt->RefreshWarInfo.nCamp;
                            MonsterReliveInfo.nBroadCastIndex = itPoint->BroadCastTypeList[i].nIndex;
                            warMiscMgr->sendToClientImportantMonsterRelive(MonsterReliveInfo);
                        }
                    }	
                }
            }
        }
    }
	
}

bool CMonsEventCreateCampMonster::sendMonsterEvent(SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn /*= true*/)
{
	IWarMiscManager* warMiscMgr = m_pWar->getMiscManager();
	if (warMiscMgr == NULL)
	{
		ErrorLn(__FUNCTION__":warMiscMgr == NULL");
		return false;
	}


	IWarMonsterMgr* pWarMonsterMgr = (IWarMonsterMgr*)m_pWar->getWarMgr(Mgr_Monster);
	if (pWarMonsterMgr == NULL)
	{
		ErrorLn(__FUNCTION__":pWarMonsterMgr == NULL");
		return false;
	}

    EWarMonsterExec EMEType = pWarMonsterMgr->getWarMonsterType(eventInfo.nContIndex);
    IMonsterExec* pMonsterExec = pWarMonsterMgr->getWarMonsterExecByType(EMEType);
    if (pMonsterExec != NULL)
    {
        pMonsterExec->getWarMonsterEvent(eventInfo, DataEx, nSize, bBorn);
    }
    return true;
}

bool CMonsEventCreateCampMonster::updateEventData(SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn /*= true*/)
{
    return true;
}
