#include "MonsTowerExec.h"
#include "ITimerService.h"
#include "IWar.h"
#include "ISoliderVigourMgr.h"
#include "IWarDropMgr.h"
#include "IWarMonsterMgr.h"

// 创建战场
CMonsTowerExec * CMonsTowerExec::execCreateMonster(IWar *pCWar, EWarMonsterExec _ExexType)
{
	CMonsTowerExec *pMonsTowerExec = new CMonsTowerExec();
	if(pMonsTowerExec != NULL && pMonsTowerExec->Init(pCWar, _ExexType))
	{
		return pMonsTowerExec;
	}
	delete pMonsTowerExec;
	pMonsTowerExec = NULL;
	return NULL;
}

void CMonsTowerExec::release()
{
    if(m_nReliveEarlyTimeID==0)
    {
        return;
    }
    for(int i=m_nReliveEarlyTimeID-1;i>=0;i--)
    {
        gServerGlobal->getTimerService()->KillTimer(i,this);
    }
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
bool CMonsTowerExec::Init(IWar *pCWar, EWarMonsterExec _ExexType)
{
    // 获得配置信息
    ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
    {
        return false;
    }
    ISchemeWarManager * pSchemeWarManager = pSchemeCenter->getSchemeWarManager();
    if (pSchemeWarManager == NULL)
    {
        return false;
    }
    ISchemeWarPoint * pSchemeWarPoint = pSchemeCenter->getSchemeWarPoint();
    if (pSchemeWarPoint == NULL)
    {
        return false;
    }

	m_pWar      = pCWar;
	m_eExecType = _ExexType;
    m_nReliveEarlyTimeID=0;
	// 获得战场ID
	int nWarStaticID = m_pWar->getStaticID();
	
	SWarInfoSchemeVect* pWarInfoMonsterType = pSchemeWarManager->GetWarInfoSchemeMap(nWarStaticID, m_eExecType);
	SWarInfoSchemeVect::iterator itInfo = pWarInfoMonsterType->begin();
	for(; itInfo != pWarInfoMonsterType->end(); ++itInfo )
	{
		SMonsterRefreshInfo tempMonsterInfo;
		tempMonsterInfo.RefreshWarInfo = (*itInfo);

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
void CMonsTowerExec::timeExec()
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
            npcContext.nLevel = warMiscMgr->getCreateLevel(refreshIt->RefreshWarInfo.nBaseLevel, refreshIt->RefreshWarInfo.nStartTime, refreshIt->RefreshWarInfo.nUpgradeTime, refreshIt->RefreshWarInfo.nMaxLevel);
            UID uID = pWarMonsterMgr->createNpc(itPoint->nMonsterID, loc, m_eExecType, &npcContext, itPoint->nIndex, itPoint->nLocalIndex, pNpcService);

            refreshIt->creatMonUIDList[uID] = itPoint->nLocalIndex;

			//复活广播
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
						warMiscMgr->sendToClientImportantMonsterRelive(MonsterReliveInfo);
                    }
                }	
            }

			if (itPoint->nMiniMapAttackProtectFlag == 1)
			{
				// 注册受到攻击事件
				m_mapUidMiniMapInfoRecord[uID] = gServerGlobal->getTimeSyncService()->GetTick();

				if (pEventEngine != NULL)
				{
					// 订阅 场景创建成功场景逻辑服务加载完事件(这个对象销毁的时候，记得取消注册啊!!! 问题代码暂时注释掉!!)
					// pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_DAMAGE, TYPE_MONSTER, uID, " CMonsTowerExec::timeExec");
				}
			}
		}

		// 塔，死亡后，不进行重刷，因此，进行处理
		// 兵营，死亡后，需要重新刷，对待不同，死亡事件中处理。
		// 刷出后，就停止
		refreshIt->bIsFirstRefresh = false;
		refreshIt->bIsOnRefresh = false;
		refreshIt->bEventHaveChanged = true;
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
void CMonsTowerExec::execMonsterDead(sEntityDieInfo entityDieInfo)
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
		}
		// 如果是本实体，进行处理
		// 本处理未塔和兵营，如果是塔，就进行处理
		if(bIsThis)
		{
			//可能有多个影响事件
            if (m_pMonsterEventMgr)
            {
                m_pMonsterEventMgr->handleMonsDieEvent(entityDieInfo, &(*refreshIt));
            }

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

					//if (itPoint->nMiniMapAttackProtectFlag == 1)
					//{
					//	// 取消注册事件
					//	SceneLogicEventHelper eventHelper(m_pWar->getSceneID());
					//	rkt::IEventEngine* pEventEngine = eventHelper.m_ptr;
					//	if (pEventEngine != NULL)
					//	{
					//		// 订阅 场景创建成功场景逻辑服务加载完事件(问题代码暂时注释掉!!!)		
					//		//pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_DAMAGE, TYPE_MONSTER, uID);

					//	}
					//}

					if (pWarDropMgr != NULL)
					{
						pWarDropMgr->addHeroEXP(itPoint->nAddEXPType, entityDieInfo.uMurderUID, entityDieInfo.nMurderCamp, vec, itPoint->nDistanceValue, itPoint->nExp, EEXPDROPTYPE_TOWER);
						pWarDropMgr->addPersonTalentValue(entityDieInfo.uMurderUID, entityDieInfo.uMurderUID, entityDieInfo.nMurderCamp, itPoint->nTalentType, vec, itPoint->nDistanceTalent, entityDieInfo.uDiedUID,
							itPoint->nExp, itPoint->nTalentValue, itPoint->fDecayFactor, ETALENTDROPTYPE_TOWER);
						pWarDropMgr->addHeroBuffValue(itPoint->nBuffType, entityDieInfo.uMurderUID, entityDieInfo.nMurderCamp, vec, itPoint->nDistanceValue, itPoint->nBuffValue, itPoint->nBuffLevel);
					}

					if (pSoliderVigourMgr != NULL)
					{
						pSoliderVigourMgr->addCampPathTypeVigour(entityDieInfo.nMurderCamp,itPoint->eVigourPathType,itPoint->nVigourAddValue);
					}

					//死亡图标广播
					for(int i = 0; i < MAX_WAR_BROADCAST_TYPE_COUNT; ++i)
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

                        if ((itPoint->BroadCastTypeList[i].eBroadCastType & EWBCT_OnTime) == EWBCT_OnTime)
                        {
                            // 设置定时广播之提早复活消息广播
                            int nRefreshTime = refreshIt->RefreshWarInfo.nRefreshTime;
                            setOnTimerBroadCastEarlyRelive(nRefreshTime,entityDieInfo.nDiedCamp,itPoint,i);
                        }
                    }

					break;
				}
			}

			break;
		}
	}
}

void CMonsTowerExec::getWarMonsterEvent( SWarEventDefine eventInfo,void *DataEx, int nSize , bool bBorn)
{
    if (m_pMonsterEventMgr)
    {
        m_pMonsterEventMgr->handleSlefMonsterEvent(eventInfo, DataEx, nSize, bBorn);
    }
}

void CMonsTowerExec::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	switch (wEventID)
	{
	case EVENT_GAME_ENTITY_DAMAGE:
		{
			if (nLen == sizeof(event_entity_damage))
			{
				event_entity_damage *pData = (event_entity_damage *)pszContext;
				onGetDamage(*pData);
			}
		}
		break;

	default:
		break;
	}
}

void CMonsTowerExec::onGetDamage(event_entity_damage damage)
{
	IWarMiscManager* warMiscMgr = m_pWar->getMiscManager();
	if (warMiscMgr == NULL)
	{
		ErrorLn(__FUNCTION__":warMiscMgr == NULL");
		return;
	}

	//被攻击了判定发战斗小地图信息
	int nCamp = getProperty_Integer(damage.uidTarget,PROPERTY_CAMP);
	map<UID,DWORD>::iterator iter = m_mapUidMiniMapInfoRecord.find(damage.uidTarget);
	DWORD curTime = gServerGlobal->getTimeSyncService()->GetTick();
	if(iter == m_mapUidMiniMapInfoRecord.end() || iter->second + warMiscMgr->getPersonConfigInfo()->nCrystalDamageIntervalTime*1000 > curTime)
	{
		return;
	}

	m_mapUidMiniMapInfoRecord[damage.uidTarget] = curTime;

	// 进攻
	msg_entity_fight_mini_map_info msgMyCampFightInfo;
	msgMyCampFightInfo.dwSrcID = damage.uidTarget;
	msgMyCampFightInfo.nInfoType = 2;
	obuf oFightInfo;
	oFightInfo<<msgMyCampFightInfo;
	m_pWar->broadCastDataToAllOtherCamp(SC_MSG_WAR_ENTITY_FIGHT_MINI_MAP_INFO, oFightInfo.data(), oFightInfo.size(), nCamp);

	// 防守
	msg_entity_fight_mini_map_info msgOtherCampFightInfo;
	msgOtherCampFightInfo.dwSrcID = damage.uidTarget;
	msgMyCampFightInfo.nInfoType = 3;
	oFightInfo.clear();
	oFightInfo<<msgOtherCampFightInfo;
	m_pWar->broadCastDataToAllPlayer(SC_MSG_WAR_ENTITY_FIGHT_MINI_MAP_INFO, oFightInfo.data(), oFightInfo.size(), nCamp);
}

// 查找配置索引是否存在
bool CMonsTowerExec::findIndex(int nIndex)
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

void CMonsTowerExec::setUltimateSoldiersIsOpen(  int nCamp )
{
	
}

void CMonsTowerExec::destroyAllRes()
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

			// 查找后，处理完成
			// 清理当前死亡的怪物
			// 目前先对玩家进行处理，后续再进行距离处理
			//WarPointVect::iterator itPoint = refreshIt->warPointVect.begin();
			//for(; itPoint != refreshIt->warPointVect.end(); ++itPoint)
			//{
			//	//WarPoint.csv nLocalIndex 唯一
			//	if(itPoint->nLocalIndex == nLocalIndex)
			//	{

			//		if (itPoint->nMiniMapAttackProtectFlag == 1)
			//		{
			//			// 取消注册事件
			//			if (pEventEngine != NULL)
			//			{
			//				// 订阅 场景创建成功场景逻辑服务加载完事件		
			//				pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_DAMAGE, TYPE_MONSTER, uID);

			//			}
			//		}

			//	}
			//}

			// 移除创建的怪物
			if (uID != INVALID_UID)
			{
				pNpcService->destroyNPC(uID);
			}
		}	
		refreshIt->creatMonUIDList.clear();
	}

	m_vecMonsterRefresh.clear();
    m_monsterEarlyReliveInfoMap.clear();

}

void CMonsTowerExec::OnTimer(unsigned long dwTimerID)
{
	IWarMiscManager* warMiscMgr = m_pWar->getMiscManager();
	if (warMiscMgr == NULL)
	{
		ErrorLn(__FUNCTION__":warMiscMgr == NULL");
		return;
	}

    // 根据timerID找到对应记录的参数
    // 目前还没其他功能定时器，有的话在此取一个范围或者标识来区分
	warMiscMgr->SendToClientImportantMonsterEarlyRelive(m_monsterEarlyReliveInfoMap[dwTimerID]);
}

void CMonsTowerExec::setOnTimerBroadCastEarlyRelive(int& nRefreshTime,int& nCamp,WarPointVect::iterator &nItPoint,int& nBroadCastIndex)
{
    // 记录数据
    msg_war_important_monster_reliveinfo nMonsterEarlyReliveInfo;
    nMonsterEarlyReliveInfo.MasterEntityType=TYPE_MONSTER;
    nMonsterEarlyReliveInfo.MasterHeroID = nItPoint->nMonsterID;
    nMonsterEarlyReliveInfo.nCamp = nCamp;
    nMonsterEarlyReliveInfo.nBroadCastIndex =nItPoint->BroadCastTypeList[nBroadCastIndex].nIndex;;

    m_monsterEarlyReliveInfoMap[m_nReliveEarlyTimeID]=nMonsterEarlyReliveInfo;
    // 得到提前提醒的时间
    int nEarlyTime=nItPoint->BroadCastParameterList[nBroadCastIndex].nBroadCastParameter;
    // 得到实际定时时间
    int nRealTime =nRefreshTime-nEarlyTime*1000;
    // 启动定时器 并记录定时器相关对应的信息
    gServerGlobal->getTimerService()->SetTimer(m_nReliveEarlyTimeID++,nRealTime , this, 1, "MonsTowerExec");
}

void CMonsTowerExec::monsterAddBuff(SWarEffectAddBuff sData)
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

void CMonsTowerExec::serchCampUIDList(BYTE bySerchCamp,BYTE byGetCampType, CreateMonUIDList& retMap)
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

void CMonsTowerExec::sendMiniMapInfo(ClientID clientID)
{
    
}

MonsterRefreshVect* CMonsTowerExec::getMonsterRefresh()
{
    return &m_vecMonsterRefresh;
}

War::EWarMonsterExec CMonsTowerExec::getMonsterType()
{
    return m_eExecType;
}

void CMonsTowerExec::stopAI()
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