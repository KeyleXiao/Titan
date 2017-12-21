#include "MonsBaseExec.h"
#include "IWar.h"
#include "IWarDropMgr.h"
#include "IWarMonsterMgr.h"

// 创建战场
CMonsBaseExec * CMonsBaseExec::execCreateMonster(IWar *pIWar, EWarMonsterExec _ExexType)
{
	CMonsBaseExec *pMonsBaseExec = new CMonsBaseExec();
	if(pMonsBaseExec != NULL && pMonsBaseExec->Init(pIWar, _ExexType))
	{
		return pMonsBaseExec;
	}
	delete pMonsBaseExec;
	pMonsBaseExec = NULL;
	return NULL;
}

void CMonsBaseExec::release()
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
bool CMonsBaseExec::Init(IWar *pIWar, EWarMonsterExec _ExexType)
{
	m_pWar = pIWar;

	m_eExecType = _ExexType;
	// 获得战场ID
	int nWarStaticID = ((IWar*)pIWar)->getStaticID();
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
void CMonsBaseExec::timeExec()
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
			npcContext.nAIID = pAIIDTable== NULL? 0: pAIIDTable->getNpcAISchemeID(m_pWar->getWarTypeId(), itPoint->nMonsterID);
            npcContext.nPathID = refreshIt->RefreshWarInfo.nPathID;
			npcContext.Angle = Vector3(itPoint->fAX, itPoint->fAY, itPoint->fAZ);
			npcContext.nLevel = warMiscMgr->getCreateLevel(refreshIt->RefreshWarInfo.nBaseLevel, refreshIt->RefreshWarInfo.nStartTime,refreshIt->RefreshWarInfo.nUpgradeTime, refreshIt->RefreshWarInfo.nMaxLevel);
			UID uID = pWarMonsterMgr->createNpc(itPoint->nMonsterID, loc, m_eExecType, &npcContext, itPoint->nIndex, itPoint->nLocalIndex, pNpcService);
			refreshIt->creatMonUIDList[uID] = itPoint->nLocalIndex;

			warMiscMgr->setCampBaseUID(refreshIt->RefreshWarInfo.nCamp, uID);

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
				m_mapUidMiniMapInfoRecord[uID] = gServerGlobal->getTimeSyncService()->GetTick();;

				if (pEventEngine != NULL)
				{
					// 订阅 场景创建成功场景逻辑服务加载完事件
					pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_DAMAGE, TYPE_MONSTER, uID, " CMonsBaseExec::timeExec");
				}
			}
		}

		// 刷出后，停止
		refreshIt->bIsFirstRefresh = false;
		refreshIt->bIsOnRefresh = false;
		refreshIt->nRefreshTime = refreshIt->RefreshWarInfo.nRefreshTime;
		refreshIt->nLatestRefreshTime = ::GetTickCount();

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
void CMonsBaseExec::execMonsterDead(sEntityDieInfo entityDieInfo)
{
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
		

		if(bIsThis)
		{
			// 查找后，处理完成
            //可能有多个影响事件
            if (m_pMonsterEventMgr)
            {
                m_pMonsterEventMgr->handleMonsDieEvent(entityDieInfo, &(*refreshIt));
            }

			// 清理当前死亡的怪物
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

					if (itPoint->nMiniMapAttackProtectFlag == 1)
					{
						// 取消注册事件
						if (pEventEngine != NULL)
						{
							// 订阅 场景创建成功场景逻辑服务加载完事件		
							pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_DAMAGE, TYPE_MONSTER, entityDieInfo.uDiedUID);

						}
					}

					if (pWarDropMgr != NULL)
					{
						pWarDropMgr->addHeroEXP(itPoint->nAddEXPType, entityDieInfo.uMurderUID, entityDieInfo.nMurderCamp, vec, itPoint->nDistanceValue, itPoint->nExp, EEXPDROPTYPE_SOLIDER);
						pWarDropMgr->addPersonTalentValue(entityDieInfo.uMurderUID, entityDieInfo.uMurderUID, entityDieInfo.nMurderCamp, itPoint->nTalentType, vec, itPoint->nDistanceTalent ,entityDieInfo.uDiedUID,itPoint->nExp,itPoint->nTalentValue,itPoint->fDecayFactor,ETALENTDROPTYPE_SOLIDER);
						pWarDropMgr->addHeroBuffValue(itPoint->nBuffType, entityDieInfo.uMurderUID,entityDieInfo.nMurderCamp, vec, itPoint->nDistanceValue, itPoint->nBuffValue, itPoint->nBuffLevel);
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
					}
					break;
				}
			}

			// 失败方让游戏结束
			if (pWarMiscMgr != NULL)
			{
				pWarMiscMgr->setFailedCampBasePos(Vector3(entityDieInfo.x, entityDieInfo.y, entityDieInfo.z));

				int nWinCamp = pWarMiscMgr->getEnemyCamp(refreshIt->RefreshWarInfo.nCamp);
				m_pWar->setWinCamp(nWinCamp);
				m_pWar->End();
			}

			break;
		}
	}
}


void CMonsBaseExec::getWarMonsterEvent( SWarEventDefine eventInfo, void *DataEx, int nSize,bool bBorn )
{
    if (m_pMonsterEventMgr)
    {
        m_pMonsterEventMgr->handleSlefMonsterEvent(eventInfo, DataEx, nSize, bBorn);
    }
}


void CMonsBaseExec::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
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

void CMonsBaseExec::onGetDamage(event_entity_damage damage)
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
	DWORD curTime = gServerGlobal->getTimeSyncService()->GetTick();;
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
bool CMonsBaseExec::findIndex(int nIndex)
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

void CMonsBaseExec::destroyAllRes()
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

	rkt::IEventEngine* pEventEngine = pSceneLogicService->getEventService();
	if(pEventEngine == NULL)
	{
		ErrorLn(__FUNCTION__" -- pEventEngine == NULL");
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
            map<int,WarPointVect>::iterator iterWarPoint = refreshIt->mapWarPointInfo.find(refreshIt->nRefreshIndex);
            if (iterWarPoint == refreshIt->mapWarPointInfo.end())
            {
                ErrorLn(__FUNCTION__": can't find warinfo WarInfoIndex"<<refreshIt->RefreshWarInfo.nIndex<<" nRefreshIndex ="<<refreshIt->nRefreshIndex);
                return;
            }

            WarPointVect::iterator itPoint = iterWarPoint->second.begin();
			for(; itPoint != iterWarPoint->second.end(); ++itPoint)
			{
				//WarPoint.csv nLocalIndex 唯一
				if(itPoint->nLocalIndex == nLocalIndex)
				{
					if (itPoint->nMiniMapAttackProtectFlag == 1)
					{
						// 取消注册事件
						if (pEventEngine != NULL)
						{
							// 订阅 场景创建成功场景逻辑服务加载完事件		
							pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_DAMAGE, TYPE_MONSTER, uID);
						}
					}
                    break;
				}
			}

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

void CMonsBaseExec::sendMiniMapInfo(ClientID clientID)
{

}

void CMonsBaseExec::serchCampUIDList(BYTE bySerchCamp,BYTE byGetCampType, CreateMonUIDList& retMap)
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

MonsterRefreshVect* CMonsBaseExec::getMonsterRefresh()
{
    return &m_vecMonsterRefresh;
}

War::EWarMonsterExec CMonsBaseExec::getMonsterType()
{
    return m_eExecType;
}

void CMonsBaseExec::stopAI()
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
