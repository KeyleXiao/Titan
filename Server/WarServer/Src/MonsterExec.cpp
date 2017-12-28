#include "MonsterExec.h"
#include "WarMessageDef.h"
#include "EntityHelper.h"
#include "ITimerService.h"
#include "IWar.h"
#include "ISoliderVigourMgr.h"
#include "IWarDropMgr.h"
#include "IRandRefreshMonsMgr.h"
#include "IWarMonsterMgr.h"

// 创建战场
CMonsterExec * CMonsterExec::execCreateMonster(IWar *pIWar, EWarMonsterExec _ExexType)
{
	CMonsterExec *pMonsterExec = new CMonsterExec();
	if(pMonsterExec != NULL && pMonsterExec->Init(pIWar, _ExexType))
	{
		return pMonsterExec;
	}
	delete pMonsterExec;
	pMonsterExec = NULL;
	return NULL;
}

void CMonsterExec::release()
{
    if (m_pMonsterEventMgr)
    {
        m_pMonsterEventMgr->Release();
        m_pMonsterEventMgr = NULL;
    }

	destroyAllRes();
    delete this;
}


// 初始化
// nWarID: 战场ID
// eType: 怪物处理类型
bool CMonsterExec::Init(IWar *pIWar, EWarMonsterExec _ExexType)
{
    m_WarDBKey = 0;
	m_pWar = pIWar;
	m_eExecType = _ExexType;

	// 获得战场ID
	int nWarStaticID = m_pWar->getStaticID();
	// 获得配置信息
	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if(pSchemeCenter == NULL)
	{
		return false;
	}
	ISchemeWarManager * pSchemeWarManager = pSchemeCenter->getSchemeWarManager();
	if(pSchemeWarManager == NULL)
	{
		return false;
	}
	
	SWarInfoSchemeVect* pWarInfoMonsterType = pSchemeWarManager->GetWarInfoSchemeMap(nWarStaticID, m_eExecType);
	SWarInfoSchemeVect::iterator itInfo = pWarInfoMonsterType->begin();
	for(; itInfo != pWarInfoMonsterType->end(); ++itInfo )
	{
		SMonsterRefreshInfo tempMonsterInfo;
		tempMonsterInfo.RefreshWarInfo = (*itInfo);
		ISchemeWarPoint * pSchemeWarPoint = pSchemeCenter->getSchemeWarPoint();
		if(pSchemeWarPoint == NULL)
		{
			break;
		}

        for (int i =0 ;i < itInfo->nMonsterIndexCount; ++i)
        {
            tempMonsterInfo.mapWarPointInfo[i] = pSchemeWarPoint->GetWarPointByIndex(itInfo->nMonsterIndexList[i]);
        }

		// 如果是直刷的才为true，否则直接去掉
		if(itInfo->bIsUse)
		{
			tempMonsterInfo.bIsOnRefresh = true;
			tempMonsterInfo.nRefreshTime = itInfo->nStartTime;
		}

		// 野怪生存情况已广播阵营初始化
		memset(tempMonsterInfo.bArrayBroadcastCamp, false, sizeof(tempMonsterInfo.bArrayBroadcastCamp));

		// 怪物刷新时间。
		m_vecMonsterRefresh.push_back(tempMonsterInfo);
	}


    // 启动定时器
    gServerGlobal->getTimerService()->SetTimer( ETimerEventID_SaveMonsterLog, WarBuyMonsterLog,this);

    gServerGlobal->getDBEngineService()->registerHandler(this, __FUNCTION__);

    m_WarDBKey = m_pWar->getWarDBKey();

    m_pMonsterEventMgr = new CWarMonsterEventMgr();
    if (m_pMonsterEventMgr == NULL || !m_pMonsterEventMgr->Init((IWar*)m_pWar,this))
    {
        ErrorLn(__FUNCTION__": failed !");
        return false;
    }
	return true;
}

// 时间处理
void CMonsterExec::timeExec()
{
	IWarMiscManager* warMiscMgr = m_pWar->getMiscManager();
	if (warMiscMgr == NULL)
	{
		WarningLn(__FUNCTION__":warMiscMgr == NULL");
	}

	IWarMonsterMgr* pWarMonsterMgr = (IWarMonsterMgr*)m_pWar->getWarMgr(Mgr_Monster);
	if (pWarMonsterMgr == NULL)
	{
		ErrorLn(__FUNCTION__":pWarMonsterMgr == NULL");
		return;
	}

	int nSceneID = m_pWar->getWarSceneID();
	SceneLogicEventHelper eventHelper(nSceneID);
	rkt::IEventEngine* pEventEngine = eventHelper.m_ptr;
	if(pEventEngine == NULL)
	{
		return ;
	}

	SceneLogicHelper helper(nSceneID);
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

	MonsterRefreshVect::iterator refreshIt = m_vecMonsterRefresh.begin();
	for(; refreshIt != m_vecMonsterRefresh.end(); ++refreshIt )
	{
		// 判断是否需要刷新
		if(!refreshIt->RefreshWarInfo.bIsUse)
		{
			continue;
		}
		// 判断是否刷新
		if(!refreshIt->bIsOnRefresh)
		{
			continue;
		}
		
		// 战绩怪物刷新数据相关下发
		if (true == refreshIt->bIsFirstTableRefresh && refreshIt->RefreshWarInfo.nTimeIndex > 0)
		{
			obuf oAllMonsterTime;
			msg_war_monster_time tempWarMonster;
			tempWarMonster.nIconIndex = refreshIt->RefreshWarInfo.nIconIndex;
			tempWarMonster.nType = refreshIt->RefreshWarInfo.nTimeIndex;
			tempWarMonster.nTableIndex = refreshIt->RefreshWarInfo.nTableIndex;
			tempWarMonster.nTime = refreshIt->RefreshWarInfo.nStartTime;
			oAllMonsterTime<<tempWarMonster;
			m_pWar->broadCastDataToAllPlayer(SC_MSG_WAR_MONSTER_TIME, oAllMonsterTime.data(), oAllMonsterTime.size());
		}
		// 战绩怪物刷新数据首次下发
		refreshIt->bIsFirstTableRefresh = false;

		// 如果时间过多，就将时间减少
		//if(refreshIt->nRefreshTime > 0)
		//{
		//	refreshIt->nRefreshTime = refreshIt->nRefreshTime - WarDefinRTime;
		//	continue;
		//}

		// 修改后的逻辑
		if (refreshIt->nLatestRefreshTime == 0)
		{
			refreshIt->nLatestRefreshTime = gServerGlobal->getTimeSyncService()->GetTick();
			continue;
		}
		else if (refreshIt->nLatestRefreshTime + refreshIt->nRefreshTime > gServerGlobal->getTimeSyncService()->GetTick())
		{
			continue;
		}
		
        // 判定刷怪类型
        WarPointVect* pWarPointvect = NULL;
        int nRefreshIndex = 0;
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
                // 随机数 - 每个怪物索引权重 < 0 则就 取这个怪物索引
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
			// 累积第几波刷怪处理
			m_listRefreshCumulant[itPoint->nLocalIndex]++;
			if (itPoint->nRefreshCumulant > 0 && itPoint->nRefreshCumulant > m_listRefreshCumulant[itPoint->nLocalIndex])
			{

				// 同步累积刷新次数
				if (itPoint->nSynRefreshCumulantIndex > 0)
				{
					m_listRefreshCumulant[itPoint->nSynRefreshCumulantIndex] = m_listRefreshCumulant[itPoint->nLocalIndex];
				}

				continue;
			}
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
			npcContext.nAIID = pAIIDTable == NULL? 0 : pAIIDTable->getNpcAISchemeID(m_pWar->getWarTypeId(), itPoint->nMonsterID);
            npcContext.nPathID = refreshIt->RefreshWarInfo.nPathID;
			npcContext.Angle = Vector3(itPoint->fAX, itPoint->fAY, itPoint->fAZ);
			if (warMiscMgr != NULL)
			{
				npcContext.nLevel = warMiscMgr->getCreateLevel(refreshIt->RefreshWarInfo.nBaseLevel, refreshIt->RefreshWarInfo.nStartTime,refreshIt->RefreshWarInfo.nUpgradeTime, refreshIt->RefreshWarInfo.nMaxLevel);
			}
			UID uID = INVALID_UID;
			if (itPoint->bIsInvisibleMonster)
			{
                // 该怪物为隐形广播怪而且是第一次创建,作用是用于监视当前刷怪点的野怪生存状况，是死是活,隐形广播怪仅在战场初始化时进行第一次创建，之后不再对其进行操作
                if (refreshIt->bIsFirstRefresh)
                {
                    uID = pWarMonsterMgr->createNpc(itPoint->nMonsterID, loc, m_eExecType, &npcContext, itPoint->nIndex, itPoint->nLocalIndex, pNpcService);
                    refreshIt->uBroadcastMonsterUid = uID;
                }
                else
                {
                    // 该怪物为隐形广播怪而且不是第一次创建,直接跳过
                    continue;
                }
				
			}
			else
			{
                // 该怪物为普通野怪
				uID = pWarMonsterMgr->createNpc(itPoint->nMonsterID, loc, m_eExecType, &npcContext, itPoint->nIndex, itPoint->nLocalIndex, pNpcService);
				refreshIt->creatMonUIDList[uID] = itPoint->nLocalIndex;
			} 

			//复活广播和出生广播
            if (itPoint->bNeedBroadCast)
            {
                for(int i = 0; i < MAX_WAR_BROADCAST_TYPE_COUNT; ++i)
                {
                    if ((itPoint->BroadCastTypeList[i].eBroadCastType & EWBCT_Relive) == EWBCT_Relive && refreshIt->bIsFirstRefresh == false)
                    {
                        msg_war_important_monster_reliveinfo MonsterReliveInfo;
                        MonsterReliveInfo.MasterEntityType = TYPE_MONSTER;
                        MonsterReliveInfo.MasterHeroID = itPoint->nMonsterID;
                        MonsterReliveInfo.nCamp = refreshIt->RefreshWarInfo.nCamp;
                        MonsterReliveInfo.nBroadCastIndex = itPoint->BroadCastTypeList[i].nIndex;

						if (warMiscMgr != NULL)
						{
							warMiscMgr->sendToClientImportantMonsterRelive(MonsterReliveInfo);
						}
                    }

					if ((itPoint->BroadCastTypeList[i].eBroadCastType & EWBCT_Both) == EWBCT_Both && refreshIt->bIsFirstRefresh == true)
					{
						msg_war_important_monster_reliveinfo MonsterBothInfo;
						MonsterBothInfo.MasterEntityType = TYPE_MONSTER;
						MonsterBothInfo.MasterHeroID = itPoint->nMonsterID;
						MonsterBothInfo.nCamp = refreshIt->RefreshWarInfo.nCamp;
						MonsterBothInfo.nBroadCastIndex = itPoint->BroadCastTypeList[i].nIndex;

						if (warMiscMgr != NULL)
						{
							warMiscMgr->sendToClientImportantMonsterRelive(MonsterBothInfo);
						}
					}
                }
            }

			// 刷新之后累积计数0开始
			m_listRefreshCumulant[itPoint->nLocalIndex] = 0;

			// 同步累积刷新次数
			m_listRefreshCumulant[itPoint->nSynRefreshCumulantIndex] = 0;

		}
		
		obuf oAllMiniMap;
		msg_war_entity_minimap_icon tempWarIconInfo;
		tempWarIconInfo.nIconIndex = refreshIt->RefreshWarInfo.nIconIndex;
		tempWarIconInfo.nMapID = m_pWar->getMapID();
		tempWarIconInfo.bIsShow = true;


		oAllMiniMap.clear();
		oAllMiniMap<<tempWarIconInfo;

		//小地图怪物图标下发
		if (true == refreshIt->bIsFirstRefresh)
		{
			m_pWar->broadCastDataToAllPlayer(SC_MSG_WAR_MINI_MAP_ICON, oAllMiniMap.data(), oAllMiniMap.size());
			// 清空野怪生存情况已广播阵营状态
			memset(refreshIt->bArrayBroadcastCamp, false, sizeof(refreshIt->bArrayBroadcastCamp));
		}
		else
		{
			// 阵营广播
			m_pWar->broadCastDataToAllPlayer(SC_MSG_WAR_MINI_MAP_ICON, oAllMiniMap.data(), oAllMiniMap.size(), refreshIt->nMurderCamp);
			refreshIt->bArrayBroadcastCamp[refreshIt->nMurderCamp] = false;
            int nCampCount = m_pWar->getMaxCamp();
            for(int n = 1; n <= nCampCount; ++n)
            {
                if (refreshIt->nMurderCamp != n)
                {
                    m_pWar->broadCastDataToAllPlayer(SC_MSG_WAR_MINI_MAP_ICON, oAllMiniMap.data(), oAllMiniMap.size(),n);
                    refreshIt->bArrayBroadcastCamp[n] = false;
                }
            }
		}

		// 刷出后，就停止
		refreshIt->bIsFirstRefresh = false;
		refreshIt->bIsOnRefresh = false;
		refreshIt->nLatestRefreshTime = gServerGlobal->getTimeSyncService()->GetTick();

		//同步到刷新时间列
		if (refreshIt->RefreshWarInfo.bIsNeedRefreshTime == true)
		{
			// 查找到当前同步的行，然后进行处理
			for (int i = 0; i < MAX_WAR_SYN_DEFINE; ++i)
			{
				if(refreshIt->RefreshWarInfo.SynRefreshTimeIndexList[i] < 0)
				{
					continue;
				}

				MonsterRefreshVect::iterator refreshItFind = m_vecMonsterRefresh.begin();
				for(; refreshItFind != m_vecMonsterRefresh.end(); ++refreshItFind )
				{
				
					if(refreshItFind->RefreshWarInfo.nIndex == refreshIt->RefreshWarInfo.SynRefreshTimeIndexList[i])
					{
						if (refreshIt->nLatestRefreshTime != 0)
						{
							refreshItFind->nLatestRefreshTime =  refreshIt->nLatestRefreshTime;
							refreshItFind->nRefreshTime = refreshIt->RefreshWarInfo.nRefreshTime;
						}
						break;
					}
				
				}
			}
		}
	}

    // 事件逻辑处理得放在最后 原来的方式有问题（影响列的实体没创建 直接执行事件处理没用）
    if (m_pMonsterEventMgr)
    {
        m_pMonsterEventMgr->handleMonsReliveEvent();
    }
}

void CMonsterExec::fillMonsterDieLog(UID uMurderID, int nCamp, int nMonstId)
{
    ActorHelper actorHelper(uMurderID);
    IActorService* pMurderService = actorHelper.m_ptr;
    if ( pMurderService == nullptr )
        return;

    DBREQ_PARAM_WAR_MONSTERDIE warMonsterDie;
    warMonsterDie.dwActorID = pMurderService->getProperty_Integer(PROPERTY_ID);
    warMonsterDie.byCamp = nCamp;
    warMonsterDie.dwMonsterID = nMonstId;
    warMonsterDie.dwOperateTime = time(NULL);
    m_vecMonsterDieLog.push_back(warMonsterDie);
}

// 怪物死亡事件处理
void CMonsterExec::execMonsterDead(sEntityDieInfo entityDieInfo)
{
	ISoliderVigourMgr* pSoliderVigourMgr = (ISoliderVigourMgr*)m_pWar->getWarMgr(Mgr_SoliderVigour);
	if (pSoliderVigourMgr == NULL)
	{
		WarningLn(__FUNCTION__":pSoliderVigourMgr == NULL");
	}

	IWarDropMgr* pWarDropMgr = (IWarDropMgr*)m_pWar->getWarMgr(Mgr_Drop);
	if (pWarDropMgr == NULL)
	{
		WarningLn(__FUNCTION__":pWarDropMgr == NULL");
	}

	IWarMiscManager* pWarMiscMgr = m_pWar->getMiscManager();
	if (pWarMiscMgr == NULL)
	{
		WarningLn(__FUNCTION__":pWarMiscMgr == NULL");
	}

	SceneLogicEventHelper eventHelper(m_pWar->getWarSceneID());
	rkt::IEventEngine* pEventEngine = eventHelper.m_ptr;
    Vector3 vec = Vector3(entityDieInfo.x, entityDieInfo.y, entityDieInfo.z);
	int nLocalIndex = 0;
	MonsterRefreshVect::iterator refreshIt = m_vecMonsterRefresh.begin();
	for(; refreshIt != m_vecMonsterRefresh.end(); ++refreshIt )
	{
		bool bIsThis = false;
		CreateMonUIDList::iterator itMonster = refreshIt->creatMonUIDList.find(entityDieInfo.uDiedUID);
		if(itMonster != refreshIt->creatMonUIDList.end())
		{
			bIsThis = true;
			nLocalIndex = itMonster->second;
			// 查找，发现是否是这个
			refreshIt->creatMonUIDList.erase(itMonster);

            int nMonsterID = entityDieInfo.uDiedPropertyID;

            // 凶手为人物时记录怪物死亡日记
            if( isActor(entityDieInfo.uMurderUID) )
            {
                fillMonsterDieLog(entityDieInfo.uMurderUID, entityDieInfo.nDiedCamp, nMonsterID);
            }
		}
		
		if(bIsThis)
		{

			// 处理死亡触发创建怪物
			if (entityDieInfo.nMurderCamp >= CAMP_NEUTRAL &&  entityDieInfo.nMurderCamp < CAMP_MAX)
			{

                if (m_pMonsterEventMgr)
                {
                    m_pMonsterEventMgr->handleMonsDieEvent(entityDieInfo, &(*refreshIt));
                }
			}

			// 目前先对玩家进行处理，后续再进行距离处理
			//增加天赋点
            map<int,WarPointVect>::iterator iterWarPoint = refreshIt->mapWarPointInfo.find(refreshIt->nRefreshIndex);
            if (iterWarPoint == refreshIt->mapWarPointInfo.end())
            {
                ErrorLn(__FUNCTION__": can't find warinfo  WarInfoIndex"<<refreshIt->RefreshWarInfo.nIndex<<" nRefreshIndex ="<<refreshIt->nRefreshIndex);
                return;
            }

            WarPointVect::iterator itPoint = iterWarPoint->second.begin();
            for(; itPoint != iterWarPoint->second.end(); ++itPoint)
			{
				//WarPoint.csv nLocalIndex 唯一
				if(itPoint->nLocalIndex == nLocalIndex)
				{
                    // 
                    EExpDropType expDropType = EEXPDROPTYPE_MONSTER;
                    ETalentDropType talentDropType = ETALENTDROPTYPE_MONSTER;
                    if (refreshIt->RefreshWarInfo.eMonsterType == EWME_BOSS )
                    {
                        expDropType = EEXPDROPTYPE_BOSS;
                        talentDropType = ETALENTDROPTYPE_BOSS;
						
						if (pSoliderVigourMgr != NULL)
						{
							pSoliderVigourMgr->addCampPathTypeVigour(entityDieInfo.nMurderCamp, itPoint->eVigourPathType, itPoint->nVigourAddValue);
						}
                    }
					
					if (pWarDropMgr != NULL)
					{
						pWarDropMgr->addHeroEXP(itPoint->nAddEXPType, entityDieInfo.uMurderUID, entityDieInfo.nMurderCamp, vec, itPoint->nDistanceValue, itPoint->nExp, expDropType);

						pWarDropMgr->addPersonTalentValue(entityDieInfo.uMurderUID, entityDieInfo.uMurderUID, entityDieInfo.nMurderCamp, itPoint->nTalentType, vec, itPoint->nDistanceTalent, entityDieInfo.uDiedUID,
							itPoint->nExp, itPoint->nTalentValue, itPoint->fDecayFactor, talentDropType);

						pWarDropMgr->addHeroBuffValue(itPoint->nBuffType, entityDieInfo.uMurderUID, entityDieInfo.nMurderCamp, vec, itPoint->nDistanceValue, itPoint->nBuffValue, itPoint->nBuffLevel);
					}

					//死亡图标广播
					for(int i = 0; i < MAX_WAR_BROADCAST_TYPE_COUNT; ++i)
					{
						if(entityDieInfo.uDiedUID == entityDieInfo.uMurderUID)
						{
							//自杀广播
							if (((itPoint->BroadCastTypeList[i].eBroadCastType & EWBCT_KillSelf) == EWBCT_KillSelf))
							{
								msg_war_important_monster_dietxtinfo MonsterDieTextInfo;
								MonsterDieTextInfo.MasterEntityType = TYPE_MONSTER;
								MonsterDieTextInfo.MasterHeroID = itPoint->nMonsterID;
								MonsterDieTextInfo.nCamp = entityDieInfo.nDiedCamp;
								MonsterDieTextInfo.nBroadCastIndex = itPoint->BroadCastTypeList[i].nIndex;
								if (pWarMiscMgr != NULL)
								{
									pWarMiscMgr->sendToClientMonsterDieBroadText(MonsterDieTextInfo);
								}
							}
						}
						else
						{
							if ((itPoint->BroadCastTypeList[i].eBroadCastType & EWBCT_DieIcon) == EWBCT_DieIcon)
							{
								if (pWarMiscMgr != NULL)
								{
									pWarMiscMgr->onEntityDieBroadCast(entityDieInfo, itPoint->BroadCastTypeList[i].nIndex);
								}
							}

							//死亡文字广播
							if ((itPoint->BroadCastTypeList[i].eBroadCastType & EWBCT_DieText) == EWBCT_DieText)
							{
								msg_war_important_monster_dietxtinfo MonsterDieTextInfo;
								MonsterDieTextInfo.MasterEntityType = TYPE_MONSTER;
								MonsterDieTextInfo.MasterHeroID = itPoint->nMonsterID;
								MonsterDieTextInfo.nCamp = entityDieInfo.nDiedCamp;
								MonsterDieTextInfo.nBroadCastIndex = itPoint->BroadCastTypeList[i].nIndex;
								if (pWarMiscMgr != NULL)
								{
									pWarMiscMgr->sendToClientMonsterDieBroadText(MonsterDieTextInfo);
								}
							}
						}						
					}
					
					break;
				}
			}


			// 如果当前怪物全死亡，就刷新
			if(refreshIt->creatMonUIDList.empty())
			{
				//*
				//ErrorLn("****************** Monster is dead!!!!");
				// 为监视野怪生死隐形实体设置刷新位
				refreshIt->bIsOnRefresh = true;
				refreshIt->nRefreshTime = refreshIt->RefreshWarInfo.nRefreshTime;
				// 全部野怪死亡后开始重新刷新计时
				refreshIt->nLatestRefreshTime = gServerGlobal->getTimeSyncService()->GetTick();

				obuf oAllPerson;

				msg_war_entity_minimap_icon tempWarIconInfo;
				tempWarIconInfo.nIconIndex = refreshIt->RefreshWarInfo.nIconIndex;
				tempWarIconInfo.nMapID = m_pWar->getMapID();
				tempWarIconInfo.bIsShow = false;

				oAllPerson.clear();
				oAllPerson<<tempWarIconInfo;
				//m_pWar->broadCastDataToHaveSightPlayer(vec,SC_MSG_WAR_MINI_MAP_ICON, oAllPerson.data(), oAllPerson.size());

				if (refreshIt->RefreshWarInfo.eMonsterType == EWME_BOSS)
				{
					m_pWar->broadCastDataToAllPlayer(SC_MSG_WAR_MINI_MAP_ICON, oAllPerson.data(), oAllPerson.size());
				}
				//*
				else
				{
					m_pWar->broadCastDataToHaveSightPlayer(vec, SC_MSG_WAR_MINI_MAP_ICON, oAllPerson.data(), oAllPerson.size());
					refreshIt->bArrayBroadcastCamp[entityDieInfo.nMurderCamp] = true;
				}
				//*/

				//*
				if (refreshIt->RefreshWarInfo.nTimeIndex > 0)
				{
					msg_war_monster_time tempWarMonster;
					tempWarMonster.nIconIndex = refreshIt->RefreshWarInfo.nIconIndex;
					tempWarMonster.nType = refreshIt->RefreshWarInfo.nTimeIndex;
					tempWarMonster.nTime = refreshIt->RefreshWarInfo.nRefreshTime;
					tempWarMonster.nTableIndex = refreshIt->RefreshWarInfo.nTableIndex;
					//ErrorLn("nRefreshTime : " << refreshIt->RefreshWarInfo.nRefreshTime);
					oAllPerson.clear();
					oAllPerson<<tempWarMonster;
					//m_pWar->broadCastDataToHaveSightPlayer(vec,SC_MSG_WAR_MONSTER_TIME, oAllPerson.data(), oAllPerson.size());
					
					if (refreshIt->RefreshWarInfo.eMonsterType == EWME_BOSS)
					{
						m_pWar->broadCastDataToAllPlayer(SC_MSG_WAR_MONSTER_TIME, oAllPerson.data(), oAllPerson.size());
					}
					//*
					else
					{
						m_pWar->broadCastDataToHaveSightPlayer(vec, SC_MSG_WAR_MONSTER_TIME, oAllPerson.data(), oAllPerson.size());
						refreshIt->bArrayBroadcastCamp[entityDieInfo.nMurderCamp] = true;
					}
					//*/
				}

				refreshIt->nMurderCamp = entityDieInfo.nMurderCamp;
				//*/
				//大小龙复活，小地图敌营同步刷新
				if (entityDieInfo.nDiedVocation == MONSTER_SUB_TYPE_SMALL_DRAGON
					|| entityDieInfo.nDiedVocation == MONSTER_SUB_TYPE_HUGE_DRAGON)
				{
                    int nCampCount = m_pWar->getMaxCamp();
                    for(int n = 1; n <= nCampCount; ++n)
                    {
                        if (refreshIt->nMurderCamp != n)
                        {
                            
                            refreshIt->bArrayBroadcastCamp[n] = true;
                        }
                    }
				}
			}
			
			// 查找后，处理完成
			break;
		}
	}
}



void CMonsterExec::getWarMonsterEvent( SWarEventDefine eventInfo, void *DataEx, int nSize ,bool bBorn)
{
    if (m_pMonsterEventMgr)
    {
        m_pMonsterEventMgr->handleSlefMonsterEvent(eventInfo, DataEx, nSize, bBorn);
    }
}

// 查找配置索引是否存在
bool CMonsterExec::findIndex(int nIndex)
{
	MonsterRefreshVect::iterator refreshIt = m_vecMonsterRefresh.begin();
	for(; refreshIt != m_vecMonsterRefresh.end(); ++refreshIt )
	{
		if (refreshIt->RefreshWarInfo.nIndex == nIndex)
		{
			return true;
		}
	}

	return false;
}

// 由广播怪通知指定阵营当前进入当前视野野怪重生点的野怪生死状况
void CMonsterExec::msgToSpecCampWildInfo(UID uidMonster, int uCamp)
{
	MonsterRefreshVect::iterator refreshIt = m_vecMonsterRefresh.begin();

	for (; refreshIt != m_vecMonsterRefresh.end(); ++refreshIt)
	{
		if (refreshIt->uBroadcastMonsterUid == uidMonster)
		{
            map<int,WarPointVect>::iterator iterWarPoint = refreshIt->mapWarPointInfo.find(refreshIt->nRefreshIndex);
            if (iterWarPoint == refreshIt->mapWarPointInfo.end())
            {
                ErrorLn(__FUNCTION__": can't find warinfo  WarInfoIndex"<<refreshIt->RefreshWarInfo.nIndex<<" nRefreshIndex ="<<refreshIt->nRefreshIndex);
                return;
            }

			// 当前野怪点没有在刷新，而且没有通知到这个阵营，所以不用通知客户端刷新小地图图标
			if (!refreshIt->bIsOnRefresh && !refreshIt->bArrayBroadcastCamp[uCamp])
			{
				return;
			}
			// 当前野怪点在刷新，但是却已经通知到这个阵营，所以不用再次通知客户端刷新小地图图标
			else if (refreshIt->bIsOnRefresh && refreshIt->bArrayBroadcastCamp[uCamp])
			{
				return;
			}

			refreshIt->bArrayBroadcastCamp[uCamp] = true;

			obuf oAllPerson;
			msg_war_entity_minimap_icon tempWarIconInfo;
			tempWarIconInfo.nIconIndex = refreshIt->RefreshWarInfo.nIconIndex;
			tempWarIconInfo.nMapID = m_pWar->getMapID();
			Vector3 vec3(iterWarPoint->second.front().fX[refreshIt->nPosIndex],
					iterWarPoint->second.front().fY[refreshIt->nPosIndex],
					iterWarPoint->second.front().fZ[refreshIt->nPosIndex]);
			
			if (gServerGlobal->getTimeSyncService()->GetTick() > refreshIt->nLatestRefreshTime + refreshIt->nRefreshTime)
			{
				tempWarIconInfo.bIsShow = true;
				oAllPerson<<tempWarIconInfo;
				m_pWar->broadCastDataToHaveSightPlayer(vec3,SC_MSG_WAR_MINI_MAP_ICON, oAllPerson.data(), oAllPerson.size());
				refreshIt->bArrayBroadcastCamp[uCamp] = false;
				return;
			}

			tempWarIconInfo.bIsShow = false;
			oAllPerson<<tempWarIconInfo;
			m_pWar->broadCastDataToHaveSightPlayer(vec3,SC_MSG_WAR_MINI_MAP_ICON, oAllPerson.data(), oAllPerson.size());
			if (refreshIt->RefreshWarInfo.nTimeIndex > 0)
			{
				msg_war_monster_time tempWarMonster;
				tempWarMonster.nIconIndex = refreshIt->RefreshWarInfo.nIconIndex;
				tempWarMonster.nType = refreshIt->RefreshWarInfo.nTimeIndex;
				tempWarMonster.nTime = refreshIt->nLatestRefreshTime + refreshIt->nRefreshTime - gServerGlobal->getTimeSyncService()->GetTick();
				tempWarMonster.nTableIndex = refreshIt->RefreshWarInfo.nTableIndex;
				oAllPerson.clear();
				oAllPerson<<tempWarMonster;
				m_pWar->broadCastDataToHaveSightPlayer(vec3,SC_MSG_WAR_MONSTER_TIME, oAllPerson.data(), oAllPerson.size());
			}

			return;
		}
	}
}

bool CMonsterExec::batchSaveMonsterDieLog()
{
    IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        ErrorLn("BatchSavePirateLog() save monster log failed pDBEngine == NULL");
        return false;
    }

    obuf128 dbbuf;
    obuf dbdata;
    DBREQ_PARAM_WAR_MONSTERINFO_SAVE log;
    log.nWarID = m_WarDBKey;

    log.nCount = 0;

    size_t node_len = sizeof( DBREQ_PARAM_WAR_MONSTERDIE );

    vector<DBREQ_PARAM_WAR_MONSTERDIE>::iterator itEnd = m_vecMonsterDieLog.end();

    int nWarId = m_pWar->getWarID();
    for (vector<DBREQ_PARAM_WAR_MONSTERDIE>::iterator iter = m_vecMonsterDieLog.begin(); iter != itEnd; iter++)
    {
        if ((log.nCount + 1) * node_len >  7000)
        {
            dbbuf.push_back( &log, sizeof(log));
            DWORD dwBytes = log.nCount * node_len;
            dbbuf.push_back(&dwBytes, sizeof(dwBytes));
            dbbuf.push_back(dbdata.data(), dbdata.size());

            if (pDBEngine->executeSP(GAMEDB_REQUEST_WAR_SAVE_MONSTERINFO, nWarId, dbbuf.data(), dbbuf.size(), static_cast<IDBRetSink *>(this)) == false)
            {
                ErrorLn("CMonsterExec::batchSaveMonsterDieLog pDBEngine->ExecuteSP failed");
            }
            log.nCount= 0; 
            dbdata.clear();
            dbbuf.clear();
        }

        DBREQ_PARAM_WAR_MONSTERDIE detail;
        detail = *iter;
        log.nCount++;
        dbdata.push_back(&detail, sizeof(DBREQ_PARAM_WAR_MONSTERDIE));
    }

    if(log.nCount > 0)
    {
        dbbuf.push_back(&log, sizeof(log));
        DWORD dwBytes = log.nCount* node_len;
        dbbuf.push_back(&dwBytes, sizeof(dwBytes));
        dbbuf.push_back(dbdata.data(), dbdata.size());
        if (pDBEngine->executeSP(GAMEDB_REQUEST_WAR_SAVE_MONSTERINFO, nWarId, dbbuf.data(), dbbuf.size(), static_cast<IDBRetSink *>(this)) == false)
        {
            ErrorLn("CMonsterExec::batchSaveMonsterDieLog pDBEngine->ExecuteSP failed");
            return false;
        }
    }
    m_vecMonsterDieLog.clear();
    return true;
}

void CMonsterExec::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{

}

void CMonsterExec::OnTimer( unsigned long dwTimerID )
{
    switch(dwTimerID)
    {
    case ETimerEventID_SaveMonsterLog:
        {
            // 每几秒储存一次 并清空容器内的内容
            batchSaveMonsterDieLog();
        }
        break;

    }
}

void CMonsterExec::destroyAllRes()
{
	SceneLogicHelper helper(m_pWar->getWarSceneID());
	ISceneLogicService* pSceneLogicService = helper.m_ptr;
	if(pSceneLogicService == NULL)
	{
		ErrorLn(__FUNCTION__" -- pSceneLogicService == NULL");
		return ;
	}

	INPCService* pNpcService = pSceneLogicService->getNPCService();
	if(pNpcService == NULL)
	{
		ErrorLn(__FUNCTION__" -- pNpcService == NULL");
		return;
	}

	int nLocalIndex = 0;
	UID uID = INVALID_UID;

	MonsterRefreshVect::iterator refreshIt = m_vecMonsterRefresh.begin();
	for(; refreshIt != m_vecMonsterRefresh.end(); ++refreshIt )
	{
		CreateMonUIDList::iterator itMonster = refreshIt->creatMonUIDList.begin();
		for(;itMonster != refreshIt->creatMonUIDList.end(); ++itMonster)
		{
			nLocalIndex = itMonster->second;
			uID = itMonster->first;

			// 移除创建的怪物
			if (uID != INVALID_UID)
			{
				pNpcService->destroyNPC(uID);
			}
		}	
		refreshIt->creatMonUIDList.clear();
	}

    batchSaveMonsterDieLog();

	m_vecMonsterRefresh.clear();

    gServerGlobal->getTimerService()->KillTimer( ETimerEventID_SaveMonsterLog, this);

    gServerGlobal->getDBEngineService()->unregisterHandler(this);
}

void CMonsterExec::sendMiniMapInfo(ClientID clientID)
{
    ActorHelper actorHelper(clientID, ACTOR_CLIENT);
    if (actorHelper.m_ptr == NULL)
        return;
    MonsterRefreshVect::iterator refreshIt = m_vecMonsterRefresh.begin();

    for (; refreshIt != m_vecMonsterRefresh.end(); ++refreshIt)
    {
        // 没有怪物
        if (refreshIt->creatMonUIDList.empty())
        {
            if (refreshIt->RefreshWarInfo.nTimeIndex > 0)
            {
                obuf obufData;
                SNetMsgHead head;
                head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
                head.bySrcEndPoint  = MSG_ENDPOINT_SCENE;
                head.byKeyModule    = MSG_MODULEID_WAR;
                head.byKeyAction    = SC_MSG_WAR_MONSTER_TIME;
                obufData<<head;

                msg_war_monster_time tempWarMonster;
                tempWarMonster.nIconIndex = refreshIt->RefreshWarInfo.nIconIndex;
                tempWarMonster.nType = refreshIt->RefreshWarInfo.nTimeIndex;
                tempWarMonster.nTime = refreshIt->nLatestRefreshTime + refreshIt->nRefreshTime - gServerGlobal->getTimeSyncService()->GetTick();
                tempWarMonster.nTableIndex = refreshIt->RefreshWarInfo.nTableIndex;
                obufData<<tempWarMonster;

                actorHelper.m_ptr->send2Client(PACKAGE_PTR(new string((const char *)obufData.data(),obufData.size())));
            }
        }
        else
        {
            // 有怪物
            if (refreshIt->RefreshWarInfo.nIconIndex != 0)
            {
                map<int,WarPointVect>::iterator iterWarPoint = refreshIt->mapWarPointInfo.find(refreshIt->nRefreshIndex);
                if (iterWarPoint == refreshIt->mapWarPointInfo.end())
                {
                    ErrorLn(__FUNCTION__": can't find warinfo  WarInfoIndex"<<refreshIt->RefreshWarInfo.nIndex<<" nRefreshIndex ="<<refreshIt->nRefreshIndex);
                    return;
                }

                obuf obufData;
                SNetMsgHead head;
                head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
                head.bySrcEndPoint  = MSG_ENDPOINT_SCENE;
                head.byKeyModule    = MSG_MODULEID_WAR;
                head.byKeyAction    = SC_MSG_WAR_MINI_MAP_ICON;
                obufData << head;

                msg_war_entity_minimap_icon tempWarIconInfo;
                tempWarIconInfo.nIconIndex = refreshIt->RefreshWarInfo.nIconIndex;
                tempWarIconInfo.nMapID = m_pWar->getMapID();
                Vector3 vec3(iterWarPoint->second.front().fX[refreshIt->nPosIndex],
                    iterWarPoint->second.front().fY[refreshIt->nPosIndex],
                    iterWarPoint->second.front().fZ[refreshIt->nPosIndex]);

                // 要刷新的怪物图标
                tempWarIconInfo.bIsShow = true;
                obufData<<tempWarIconInfo;
                actorHelper.m_ptr->send2Client(PACKAGE_PTR(new string((const char *)obufData.data(),obufData.size())));

            }
        }
        
    }
}

void CMonsterExec::monsterAddBuff(SWarEffectAddBuff sData)
{
	UID uidArrayp[512] = { INVALID_UID };
	int nCount = 0;
	MonsterRefreshVect::iterator refreshIt = m_vecMonsterRefresh.begin();
	for (; refreshIt != m_vecMonsterRefresh.end(); ++refreshIt)
	{
		bool bNeedInsert = false;
		switch (sData.byGetCampType)
		{
		case EWCT_SELFCAMP:
		{
			// 己方
			if (refreshIt->RefreshWarInfo.nCamp == sData.bySelfCamp)
			{
				bNeedInsert = true;
			}
		}
		break;
		case EWCT_ENIMICAMP:
		{
			if (refreshIt->RefreshWarInfo.nCamp != sData.bySelfCamp)
			{
				bNeedInsert = true;
			}

		}
		break;
		case EWCT_ALL:
		{
			bNeedInsert = true;
		}
		break;
		}

		if (bNeedInsert)
		{
			for (CreateMonUIDList::iterator iter = refreshIt->creatMonUIDList.begin(); iter != refreshIt->creatMonUIDList.end(); ++iter)
			{
				uidArrayp[nCount++] = iter->first;
			}
		}
	}

	for (int n = 0; n < nCount; n++)
	{
		UID uid = uidArrayp[n];
		MonsterHelper helper(uid);
		INPCService *pNpcService = helper.m_ptr;
		if (pNpcService != NULL)
		{
			switch (sData.byAddType)
			{
			case BUFF_ADD:
			{
				// 给实体添加buff
				SBuffContext BuffContext;
				BuffContext.nID = sData.nID;
				pNpcService->BatchAddBuff(uidArrayp, nCount, sData.nBuffID, sData.nBuffLevel, sData.uidSrcEntity, 0, &BuffContext, sizeof(BuffContext));

			}
			break;
			case BUFF_REMOVE:
			{
				// 给实体移除buff
				pNpcService->BatchRemoveBuff(uidArrayp, nCount, sData.nBuffID, sData.uidSrcEntity, sData.uidSrcEntity);

			}
			break;
			//case BUFF_OVERLAY:
			//{
			//	// 叠加BUFF
			//	SetAccumulateBuff(iter->first, sData.nBuffID, sData.uidSrcEntity, true);
			//}
			//break;
			//case BUFF_DECREASE:
			//{
			//	// 递减BUFF
			//	SetAccumulateBuff(iter->first, sData.nBuffID, sData.uidSrcEntity, false);
			//}
			//break;
			default:
				break;
			}

			// 已经批量添加执行过一次了
			break;
		}

	}
}

void CMonsterExec::serchCampUIDList(BYTE bySerchCamp,BYTE byGetCampType, CreateMonUIDList& retMap)
{
    MonsterRefreshVect::iterator refreshIt = m_vecMonsterRefresh.begin();
    for(; refreshIt != m_vecMonsterRefresh.end(); ++refreshIt )
    {
        bool bNeedInsert = false;
        switch(byGetCampType)
        {
        case EWCT_SELFCAMP:
            {
                // 己方
                if(refreshIt->RefreshWarInfo.nCamp == bySerchCamp)
                {
                    bNeedInsert = true;
                }
            }
            break;
        case EWCT_ENIMICAMP:
            {
                if(refreshIt->RefreshWarInfo.nCamp != bySerchCamp)
                {
                    bNeedInsert = true;
                }
            }
            break;
        case EWCT_ALL:
            {
                bNeedInsert = true;
            }
            break;
        }

        if(bNeedInsert)
        {
            retMap.insert(refreshIt->creatMonUIDList.begin(),refreshIt->creatMonUIDList.end());
        }
    }
}

MonsterRefreshVect* CMonsterExec::getMonsterRefresh()
{
    return &m_vecMonsterRefresh;
}

War::EWarMonsterExec CMonsterExec::getMonsterType()
{
    return m_eExecType;
}

void CMonsterExec::stopAI()
{
    int nSceneID = m_pWar->getWarSceneID();
    AIServiceHelper aiHelper(nSceneID);
    if (aiHelper.m_ptr == NULL)
    {
        WarningLn(__FUNCTION__": aiHelper.m_ptr == NULL");
        return;
    }

    MonsterRefreshVect::iterator refreshIt = m_vecMonsterRefresh.begin();
    for(; refreshIt != m_vecMonsterRefresh.end(); ++refreshIt )
    {
        CreateMonUIDList::iterator itMonster = refreshIt->creatMonUIDList.begin();
        for(;itMonster != refreshIt->creatMonUIDList.end(); ++itMonster)
        {
            aiHelper.m_ptr->destroyAI(itMonster->first);
        }
    }
}
