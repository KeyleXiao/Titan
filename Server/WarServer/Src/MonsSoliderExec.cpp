#include "MonsSoliderExec.h"
#include "IWar.h"
#include "IWarDropMgr.h"
#include "ISoliderVigourMgr.h"
#include "IWarMonsterMgr.h"

// 创建战场
CMonsSoliderExec * CMonsSoliderExec::execCreateMonster(IWar *pIWar, EWarMonsterExec _ExexType)
{
    
	CMonsSoliderExec *pMonsSoliderExec = new CMonsSoliderExec();
	if(pMonsSoliderExec != NULL && pMonsSoliderExec->Init(pIWar, _ExexType))
	{
		return pMonsSoliderExec;
	}
	delete pMonsSoliderExec;
	pMonsSoliderExec = NULL;
	return NULL;
}

void CMonsSoliderExec::release()
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
bool CMonsSoliderExec::Init(IWar *pIWar, EWarMonsterExec _ExexType)
{
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
		// 怪物刷新时间。
		m_vecMonsterRefresh.push_back(tempMonsterInfo);
	}

    m_pMonsterEventMgr = new CWarMonsterEventMgr();
    if (m_pMonsterEventMgr == NULL || !m_pMonsterEventMgr->Init((IWar*)m_pWar,this))
    {
        ErrorLn(__FUNCTION__": failed !");
        return false;
    }
	return true;
}
// 时间处理
void CMonsSoliderExec::timeExec()
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

	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if(pSchemeCenter == NULL)
	{
		return ;
	}
	// 取得战场人物配置来获得气势信息
	const SchemeWarPersonConfigInfo* const pSchemeWarPersonConfigInfo = pSchemeCenter->getSchemeWarPersonConfigInfo()->GetSchemeWarPersonConfigInfo(m_pWar->getStaticID());
	if (pSchemeWarPersonConfigInfo == NULL)
	{
		ErrorLn("pSchemeWarPersonConfigInfo == NULL");
		return;
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
		
		//不能这样处理 刷超级兵可能会有问题而且碰到计时器延时问题这样刷兵会有问题
		// 如果时间过多，就将时间减少 		
		//if(refreshIt->nRefreshTime > 0)
		//{
		//	refreshIt->nRefreshTime = refreshIt->nRefreshTime - WarDefinRTime;
		//	continue;
		//}

		// 修改后的逻辑
        int nIndex = refreshIt->RefreshWarInfo.nIndex;
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
			npcContext.nAIID = pAIIDTable==NULL? 0: pAIIDTable->getNpcAISchemeID(m_pWar->getWarTypeId(), itPoint->nMonsterID);
            npcContext.nPathID = refreshIt->RefreshWarInfo.nPathID;
			npcContext.Angle = Vector3(itPoint->fAX, itPoint->fAY, itPoint->fAZ);
			if (warMiscMgr != NULL)
			{
				npcContext.nLevel = warMiscMgr->getCreateLevel(refreshIt->RefreshWarInfo.nBaseLevel, refreshIt->RefreshWarInfo.nStartTime,refreshIt->RefreshWarInfo.nUpgradeTime,refreshIt->RefreshWarInfo.nMaxLevel);
			}
			getSoliderVigourPropertyRatio(npcContext.VigourRatio,itPoint->nMonsterID,npcContext.nCamp,refreshIt->RefreshWarInfo.ePathType,pSchemeWarPersonConfigInfo);
			UID uID = pWarMonsterMgr->createNpc(itPoint->nMonsterID, loc, m_eExecType, &npcContext, itPoint->nIndex, itPoint->nLocalIndex, pNpcService);
			refreshIt->creatMonUIDList[uID] = itPoint->nLocalIndex;
			// 刷新之后累积计数0开始
			m_listRefreshCumulant[itPoint->nLocalIndex] = 0;

			// 同步累积刷新次数
			m_listRefreshCumulant[itPoint->nSynRefreshCumulantIndex] = 0;
		}

		// 刷出后，接着继续刷
		refreshIt->bIsFirstRefresh = false;
		refreshIt->bIsOnRefresh = true;
		refreshIt->nRefreshTime = refreshIt->RefreshWarInfo.nRefreshTime;
		refreshIt->nLatestRefreshTime = gServerGlobal->getTimeSyncService()->GetTick();;

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
                            refreshItFind->bIsFirstRefresh = false;
							refreshItFind->nLatestRefreshTime =  refreshIt->nLatestRefreshTime;
                            refreshItFind->nRefreshTime = refreshItFind->RefreshWarInfo.nRefreshTime;
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

// 怪物死亡事件处理
void CMonsSoliderExec::execMonsterDead(sEntityDieInfo entityDieInfo )
{
	IWarDropMgr* pWarDropMgr = (IWarDropMgr*)m_pWar->getWarMgr(Mgr_Drop);
	if (pWarDropMgr == NULL)
	{
		WarningLn(__FUNCTION__":pWarDropMgr == NULL");
	}

	IWarMiscManager* warMiscMgr = m_pWar->getMiscManager();
	if (warMiscMgr == NULL)
	{
		WarningLn(__FUNCTION__":warMiscMgr == NULL");
	}

	int nLocalIndex = 0;
    Vector3 vec = Vector3(entityDieInfo.x, entityDieInfo.y, entityDieInfo.z);
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
		}
		

		if(bIsThis)
		{
			// 查找后，处理完成
            //可能有多个影响事件
            if (m_pMonsterEventMgr)
            {
                m_pMonsterEventMgr->handleMonsDieEvent(entityDieInfo, &(*refreshIt));
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
					if (pWarDropMgr != NULL)
					{
						pWarDropMgr->addHeroEXP(itPoint->nAddEXPType, entityDieInfo.uMurderUID,entityDieInfo.nMurderCamp, vec, itPoint->nDistanceValue, itPoint->nExp, EEXPDROPTYPE_SOLIDER);
					}

                    UID uMinDisHeroUid = 0;

                    if (!aroundIsHaveTower(entityDieInfo.nDiedCamp, vec, uMinDisHeroUid))
                    {
						if (UID_2_TYPE(entityDieInfo.uMurderUID) != TYPE_PLAYER_ROLE && UID_2_TYPE(uMinDisHeroUid) == TYPE_PLAYER_ROLE && getProperty_Integer(entityDieInfo.uMurderUID, PROPERTY_SEX) != MONSTER_TYPE_TOWER)
						{
							// 非人物非塔击杀
							int nCamp = getProperty_Integer(uMinDisHeroUid, PROPERTY_CAMP);
							if (pWarDropMgr != NULL)
							{
								pWarDropMgr->addPersonTalentValue(entityDieInfo.uMurderUID, uMinDisHeroUid, nCamp, itPoint->nTalentType, vec, itPoint->nDistanceTalent, entityDieInfo.uDiedUID,
									itPoint->nExp, itPoint->nTalentValue, itPoint->fDecayFactor, ETALENTDROPTYPE_SOLIDER);
							}
						}
						else if (UID_2_TYPE(entityDieInfo.uMurderUID) == TYPE_MONSTER && UID_2_TYPE(uMinDisHeroUid) == TYPE_PLAYER_ROLE && getProperty_Integer(entityDieInfo.uMurderUID, PROPERTY_SEX) == MONSTER_TYPE_TOWER)
						{
							// 塔击杀
							int nCamp = getProperty_Integer(uMinDisHeroUid, PROPERTY_CAMP);
							if (pWarDropMgr != NULL)
							{
								pWarDropMgr->addPersonTalentValue(entityDieInfo.uMurderUID, uMinDisHeroUid, nCamp, itPoint->nTalentType, vec, itPoint->nDistanceTalent, entityDieInfo.uDiedUID,
									itPoint->nExp, itPoint->nTalentValue, itPoint->fDecayFactor, ETALENTDROPTYPE_SOLIDER);
							}
						}
                        else if( UID_2_TYPE(entityDieInfo.uMurderUID) == TYPE_PLAYER_ROLE )
                        {
							// 人物击杀
							if (pWarDropMgr != NULL)
							{
								pWarDropMgr->addPersonTalentValue(entityDieInfo.uMurderUID, entityDieInfo.uMurderUID, entityDieInfo.nMurderCamp, itPoint->nTalentType, vec, itPoint->nDistanceTalent,
									entityDieInfo.uDiedUID, itPoint->nExp, itPoint->nTalentValue, itPoint->fDecayFactor, ETALENTDROPTYPE_SOLIDER);
							}
                        }

                        //obuf obData;
                        //obData << uMurderID;
                        //m_pWar->broadCastDataToAllPlayer(SC_MSG_WAR_MONSTERDIE_BROADCAST, obData.data(), obData.size(), nMurderCamp );
                    }
                    
					if (pWarDropMgr != NULL)
					{
						pWarDropMgr->addHeroBuffValue(itPoint->nBuffType, entityDieInfo.uMurderUID, entityDieInfo.nMurderCamp, vec, itPoint->nDistanceValue, itPoint->nBuffValue, itPoint->nBuffLevel);
					}

                    if (itPoint->bNeedBroadCast)
                    {
                        for(int i = 0; i < MAX_WAR_BROADCAST_TYPE_COUNT; ++i)
                        {
                            if ((itPoint->BroadCastTypeList[i].eBroadCastType & EWBCT_DieIcon) == EWBCT_DieIcon)
                            {
								if (warMiscMgr != NULL)
								{
									warMiscMgr->onEntityDieBroadCast(entityDieInfo, itPoint->BroadCastTypeList[i].nIndex);
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
								if (warMiscMgr != NULL)
								{
									warMiscMgr->sendToClientMonsterDieBroadText(MonsterDieTextInfo);
								}
                            }
                        }
                    }
					
					break;
				}
			}

			break;
		}
	}
}



void CMonsSoliderExec::getWarMonsterEvent( SWarEventDefine eventInfo, void *DataEx, int nSize, bool bBorn )
{
    if (m_pMonsterEventMgr)
    {
        m_pMonsterEventMgr->handleSlefMonsterEvent(eventInfo, DataEx, nSize, bBorn);
    }
}

// 查找配置索引是否存在
bool CMonsSoliderExec::findIndex(int nIndex)
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

void CMonsSoliderExec::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	switch (wEventID)
	{

	default:
		break;
	}
}

void CMonsSoliderExec::destroyAllRes()
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

	m_vecMonsterRefresh.clear();
}

bool CMonsSoliderExec::aroundIsHaveTower( int nCamp ,Vector3 loc, UID& uID )
{
    int nSceneID = m_pWar->getWarSceneID();
    SceneServiceHelper sceneHelper(nSceneID);
    IGameSceneService * pSceneService = sceneHelper.m_ptr;
    if ( pSceneService == 0 )
    {
        return false;
    }

    SWarPersonConfig* pInfo = gServerGlobal->getSchemeCenter()->getSchemeWarPersonConfigInfo()->GetWarPersonConfig();
    //int nDistance = pInfo ? pInfo->nTowerDistance : 32;
    int nDistance = pInfo ? pInfo->nMonsDistance : 32;
    
	AOI_PROXY_PTR pProxyArray[MAX_INTEREST_OBJECT];
    int num = pSceneService->k_nearest( loc, nDistance, pProxyArray, MAX_INTEREST_OBJECT, CAMP_EXCEPT_MASK(nCamp), true);

    UID uid = INVALID_UID;
    int nSubType = 0;
    float fDistance = 0.0f;
    for (int i=0; i<num; ++i)
    {
        if(pProxyArray[i] == 0 )
            continue;

		uid = pProxyArray[i]->m_uid;
        if ( UID_2_TYPE(uid) == TYPE_MONSTER )
        {
            //nSubType = getProperty_Integer(uid, PROPERTY_SEX);
            //if (MONSTER_TYPE_TOWER == nSubType)
            //{
            //    if ( loc.checkDistance(getLocation(uid),nDistance) == false)
            //        continue;
            //    return true;
            //}
        }else if ( UID_2_TYPE(uid) == TYPE_PLAYER_ROLE)
        {
            float fEntityDistance = getLocation(uid).getDistance(loc);
            if ( fDistance == 0 || fEntityDistance < fDistance)
            {
                fDistance = fEntityDistance;
                uID = uid;
            }
        }
    }

    return false;


}

bool CMonsSoliderExec::getSoliderVigourPropertyRatio(int* pVigourRatio,UID uidMonster,int nCamp,EWarSoliderPathType ePathType,const SchemeWarPersonConfigInfo* const pSchemeWarPersonConfigInfo)
{
	ISoliderVigourMgr* pSoliderVigourMgr = (ISoliderVigourMgr*)m_pWar->getWarMgr(Mgr_SoliderVigour);
	if (pSoliderVigourMgr == NULL)
	{
		ErrorLn(__FUNCTION__":pSoliderVigourMgr == NULL");
		return false;
	}

	// 先取得本小兵本阵营本线路的相对气势值

	int nRelativeVigour = pSoliderVigourMgr->getCampPathTypeRelativeVigour(nCamp,ePathType);
	if (nRelativeVigour <= 0)
	{
		return false;
	}

	// 再获各个属性的加成系数
	int VigourLevel[MaxVigourRatioCount] = {1,2,3,4};
	for(int nTypeIndex = VPT_ADD_HP;nTypeIndex < VPT_ADD_MAX;++nTypeIndex)
	{
		for(int nLevelIndex = 0;nLevelIndex < MaxVigourRatioCount;++nLevelIndex)
		{
			if(nRelativeVigour < VigourLevel[nLevelIndex])
			{
				continue;
			}

			if(nLevelIndex == MaxVigourRatioCount-1)
			{
				pVigourRatio[nTypeIndex] = pSchemeWarPersonConfigInfo->VigourLevelRatio[nTypeIndex][nLevelIndex];
				break;
			}

			if(nRelativeVigour <  VigourLevel[nLevelIndex+1])
			{
				pVigourRatio[nTypeIndex] = pSchemeWarPersonConfigInfo->VigourLevelRatio[nTypeIndex][nLevelIndex];
				break;
			}
		}
	}

	// 为了安全再校正一遍值
	for(int nTypeIndex = VPT_ADD_HP;nTypeIndex<VPT_ADD_MAX;nTypeIndex++)
	{
		makesure_minmax(pVigourRatio[nTypeIndex],0,WAR_SOLIDER_VIGOUR_MAX);
	}

	return true;
}




void CMonsSoliderExec::serchCampUIDList(BYTE bySerchCamp,BYTE byGetCampType, CreateMonUIDList& retMap)
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


void CMonsSoliderExec::sendMiniMapInfo(ClientID clientID)
{

}

MonsterRefreshVect* CMonsSoliderExec::getMonsterRefresh()
{
    return &m_vecMonsterRefresh;
}

War::EWarMonsterExec CMonsSoliderExec::getMonsterType()
{
    return m_eExecType;
}

void CMonsSoliderExec::stopAI()
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
