#include "MonsCrystalExec.h"

// 创建战场
CMonsCrystalExec * CMonsCrystalExec::CreateMonsterExec(CWar *pCWar)
{
	CMonsCrystalExec *pMonsCrystalExec = new CMonsCrystalExec();
	if(pMonsCrystalExec != NULL && pMonsCrystalExec->Init(pCWar))
	{
		return pMonsCrystalExec;
	}
	delete pMonsCrystalExec;
	pMonsCrystalExec = NULL;
	return NULL;
}

// 初始化
// nWarID: 战场ID
// eType: 怪物处理类型
bool CMonsCrystalExec::Init(CWar *pCWar)
{
	m_CWar = pCWar;
	m_ExexType = EWME_Crystal;
	// 获得战场ID
	int nWarStaticID = pCWar->GetStaticID();
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
	
	
	SWarInfoSchemeVect tempSchemeVect = pSchemeWarManager->GetWarInfoSchemeMap(nWarStaticID, m_ExexType);
	SWarInfoSchemeVect::iterator itInfo = tempSchemeVect.begin();
	for(; itInfo != tempSchemeVect.end(); ++itInfo )
	{
		SMonsterRefreshInfo tempMonsterInfo;
		tempMonsterInfo.refreshWarInfo = *itInfo;
		ISchemeWarPoint * pSchemeWarPoint = pSchemeCenter->getSchemeWarPoint();
		if(pSchemeWarPoint == NULL)
		{
			break;
		}
		tempMonsterInfo.warPointVect = pSchemeWarPoint->GetWarPointByIndex(itInfo->nMonsterIndex);
		// 如果是直刷的才为true，否则直接去掉
		if(itInfo->bIsUse)
		{
			tempMonsterInfo.bIsOnRefresh = true;
			tempMonsterInfo.nRefreshTime = itInfo->nStartTime;
		}
		// 怪物刷新时间。
		m_MonsterRefreshVect.push_back(tempMonsterInfo);

	}
	return true;
}
// 时间处理
void CMonsCrystalExec::timeExec()
{
	
	MonsterRefreshVect::iterator refreshIt = m_MonsterRefreshVect.begin();
	for(; refreshIt != m_MonsterRefreshVect.end(); ++refreshIt )
	{
		// 判断是否需要刷新
		if(!refreshIt->refreshWarInfo.bIsUse)
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
			refreshIt->nLatestRefreshTime = ::GetTickCount();
			continue;
		}
		else if (refreshIt->nLatestRefreshTime + refreshIt->nRefreshTime > ::GetTickCount())
		{
			continue;
		}


		// 刷新时间到，对怪物进行刷新, 并订阅死亡事件
		WarPointVect::iterator itPoint = refreshIt->warPointVect.begin();
		for(; itPoint != refreshIt->warPointVect.end(); ++itPoint)
		{
			Vector3 loc = Vector3(itPoint->fX, itPoint->fY, itPoint->fZ);
			CreateNpcContext npcContext;
			npcContext.nCamp = refreshIt->refreshWarInfo.nCamp;
			npcContext.nPathID = refreshIt->refreshWarInfo.nPathID;
			npcContext.Angle = Vector3(itPoint->fAX, itPoint->fAY, itPoint->fAZ);
			npcContext.nLevel =  m_CWar->GetCreateLevel(refreshIt->refreshWarInfo.nBaseLevel, refreshIt->refreshWarInfo.nUpgradeTime, refreshIt->refreshWarInfo.nMaxLevel);
			UID uID = m_CWar->CreateNpc(itPoint->nMonsterID, loc, m_ExexType, npcContext);

			refreshIt->creatMonUIDList[uID] = itPoint->nLocalIndex;

			//复活广播
			if ((itPoint->nBroadCastType & EWBCT_Relive) == EWBCT_Relive && refreshIt->bIsFirstRefresh == false)
			{
				msg_war_important_monster_reliveinfo MonsterReliveInfo;
				MonsterReliveInfo.MasterEntityType = TYPE_MONSTER;
				MonsterReliveInfo.MasterHeroID = itPoint->nMonsterID;
				MonsterReliveInfo.nCamp = refreshIt->refreshWarInfo.nCamp;
				m_CWar->SendToClientImportantMonsterRelive(MonsterReliveInfo);
			}

			if (itPoint->nMiniMapAttackProtectFlag == 1)
			{
				// 注册受到攻击事件
				m_UIDMiniMapInfoRecord[uID] = ::GetTickCount();

				rkt::IEventEngine* pEventEngine = SceneLogicEventHelper(m_CWar->GetSceneID()).m_ptr;
				if (pEventEngine != NULL)
				{
					// 订阅 场景创建成功场景逻辑服务加载完事件
					pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_DAMAGE, TYPE_MONSTER, uID, " CMonsCrystalExec::timeExec");
				}
			}
			
		}

		// 塔，死亡后，不进行重刷，因此，进行处理
		// 兵营，死亡后，需要重新刷，对待不同，死亡事件中处理。
		// 刷出后，就停止
		refreshIt->bIsFirstRefresh = false;
		refreshIt->bIsOnRefresh = false;
		refreshIt->bEventHaveChanged = true;
		refreshIt->nLatestRefreshTime = ::GetTickCount();

		//同步到刷新时间列
		if(refreshIt->refreshWarInfo.nSynRefreshTimeIndex > 0)
		{
			MonsterRefreshVect::iterator refreshItFind = m_MonsterRefreshVect.begin();
			for(; refreshItFind != m_MonsterRefreshVect.end(); ++refreshItFind )
			{
				// 查找到当前同步的行，然后进行处理
				if(refreshItFind->refreshWarInfo.nIndex == refreshIt->refreshWarInfo.nSynRefreshTimeIndex)
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

	
	// 事件逻辑处理得放在最后 原来的方式有问题（影响列的实体没创建 直接执行事件处理没用）
	MonsterRefreshVect::iterator refreshEventIt = m_MonsterRefreshVect.begin();
	for(; refreshEventIt != m_MonsterRefreshVect.end(); ++refreshEventIt )
	{
		if (refreshEventIt->bEventHaveChanged == true)
		{
			for (int i = 0; i < MAX_WAR_EVENT_DEFINE; ++i)
			{
				if(refreshEventIt->refreshWarInfo.WarEventDefine[i].eWarEvent == EWIE_Protect)
				{
					// 保护塔事件
					SetTowerProtect(refreshEventIt->refreshWarInfo.WarEventDefine[i].nContIndex, refreshEventIt->refreshWarInfo.WarEventDefine[i].nParm,true);
				}
				else if(refreshEventIt->refreshWarInfo.WarEventDefine[i].eWarEvent == EWIE_OpenRefresh)
				{
					// 如果是兵营，就进行处理 最后一个参数是状态 复活
					SetSoliderRefresh(refreshEventIt->refreshWarInfo.WarEventDefine[i].nContIndex, refreshEventIt->refreshWarInfo.WarEventDefine[i].nParm, true);
				}
				else if (refreshEventIt->refreshWarInfo.WarEventDefine[i].eWarEvent == EWIE_ProtectBase)
				{
					// 保护基地事件
					SetBaseProtect(refreshEventIt->refreshWarInfo.WarEventDefine[i].nContIndex, refreshEventIt->refreshWarInfo.WarEventDefine[i].nParm, true);
				}
				else if (refreshEventIt->refreshWarInfo.WarEventDefine[i].eWarEvent == EWIE_MutexProtectBase)
				{
					SProtectInfo tmp_ProtectInfo;
					tmp_ProtectInfo.bHaveExist = true;
					tmp_ProtectInfo.nEffectIndex = refreshEventIt->refreshWarInfo.WarEventDefine[i].nContIndex;
					tmp_ProtectInfo.nParam = refreshEventIt->refreshWarInfo.WarEventDefine[i].nParm;

					map<int,MAP_RecBaseProtect>::iterator iter = m_CampProtectBaseCount.find(refreshEventIt->refreshWarInfo.nCamp);
					if(iter != m_CampProtectBaseCount.end())
					{
						iter->second[refreshEventIt->refreshWarInfo.nIndex] = tmp_ProtectInfo;
					}
					else
					{

						MAP_RecBaseProtect map_RecBaseProtect;
						map_RecBaseProtect[refreshEventIt->refreshWarInfo.nIndex] = tmp_ProtectInfo;
						m_CampProtectBaseCount[refreshEventIt->refreshWarInfo.nCamp] = map_RecBaseProtect;
					}
				}
				
			}

			//处理完事件则设置改变事件标识
			refreshEventIt->bEventHaveChanged = false;
		}
	}

	//处理互斥事件
	HandleMutexBaseProtect();
}

// 怪物死亡事件处理
void CMonsCrystalExec::MonsterDeadExec(UID uID, UID uMurderID, int nMonsterID,int nCamp, int nMurderCamp, Vector3 vec)
{
	int nLocalIndex = 0;
	MonsterRefreshVect::iterator refreshIt = m_MonsterRefreshVect.begin();
	for(; refreshIt != m_MonsterRefreshVect.end(); ++refreshIt )
	{
		bool bIsThis = false;
		CreateMonUIDList::iterator itMonster = refreshIt->creatMonUIDList.find(uID);
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
			for (int i = 0; i < MAX_WAR_EVENT_DEFINE; ++i)
			{
				// 保护的事件, 保护的塔显露出来
				if(refreshIt->refreshWarInfo.WarEventDefine[i].eWarEvent == EWIE_Protect)
				{
					SetTowerProtect(refreshIt->refreshWarInfo.WarEventDefine[i].nContIndex, refreshIt->refreshWarInfo.WarEventDefine[i].nParm,false);
				}
				// 添加事件，给敌对添加超级兵
				else if(refreshIt->refreshWarInfo.WarEventDefine[i].eWarEvent == EWIE_OpenRefresh)
				{
					SetSoliderRefresh(refreshIt->refreshWarInfo.WarEventDefine[i].nContIndex, refreshIt->refreshWarInfo.WarEventDefine[i].nParm, false);
					refreshIt->bIsOnRefresh = true;
					refreshIt->nRefreshTime = refreshIt->refreshWarInfo.nRefreshTime;
					refreshIt->nLatestRefreshTime = ::GetTickCount();
				}
				// 保护基地事件
				else if (refreshIt->refreshWarInfo.WarEventDefine[i].eWarEvent == EWIE_ProtectBase)
				{
					// 如果当前怪物全死亡，就取消基地保护
					if(refreshIt->creatMonUIDList.empty())
					{
						SetBaseProtect(refreshIt->refreshWarInfo.WarEventDefine[i].nContIndex, refreshIt->refreshWarInfo.WarEventDefine[i].nParm, false);
					}
				}
				else if (refreshIt->refreshWarInfo.WarEventDefine[i].eWarEvent == EWIE_MutexProtectBase)
				{
					// 互斥保护有死亡
					SProtectInfo tmp_ProtectInfo;
					tmp_ProtectInfo.bHaveExist = false;
					tmp_ProtectInfo.nEffectIndex = refreshIt->refreshWarInfo.WarEventDefine[i].nContIndex;
					tmp_ProtectInfo.nParam = refreshIt->refreshWarInfo.WarEventDefine[i].nParm;

					map<int,MAP_RecBaseProtect>::iterator iter = m_CampProtectBaseCount.find(refreshIt->refreshWarInfo.nCamp);
					if(iter != m_CampProtectBaseCount.end())
					{
						iter->second[refreshIt->refreshWarInfo.nIndex] = tmp_ProtectInfo;
					}
					else
					{

						MAP_RecBaseProtect map_RecBaseProtect;
						map_RecBaseProtect[refreshIt->refreshWarInfo.nIndex] = tmp_ProtectInfo;
						m_CampProtectBaseCount[refreshIt->refreshWarInfo.nCamp] = map_RecBaseProtect;
					}
					HandleMutexBaseProtect();
				}
			}

			//增加天赋点
			WarPointVect::iterator itPoint = refreshIt->warPointVect.begin();
			for(; itPoint != refreshIt->warPointVect.end(); ++itPoint)
			{
				//WarPoint.csv nLocalIndex 唯一
				if(itPoint->nLocalIndex == nLocalIndex)
				{
					if (itPoint->nMiniMapAttackProtectFlag == 1)
					{
						// 取消注册事件
						rkt::IEventEngine* pEventEngine = SceneLogicEventHelper(m_CWar->GetSceneID()).m_ptr;
						if (pEventEngine != NULL)
						{
							// 订阅 场景创建成功场景逻辑服务加载完事件		
							pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_DAMAGE, TYPE_MONSTER, nMonsterID);

						}
					}
					
					m_CWar->AddHeroEXP((EWarAddEXPType)itPoint->nAddEXPType, uMurderID, nMurderCamp,vec, itPoint->nDistanceValue, itPoint->nExp);
					m_CWar->AddPersonTalentValue(uMurderID, nMurderCamp,(EWarTalentType)itPoint->nTalentType, vec, itPoint->nDistanceValue ,itPoint->nTalentValue);
					m_CWar->AddHeroBuffValue((EWarBuffType)itPoint->nBuffType, uMurderID,nMurderCamp, vec, itPoint->nDistanceValue, itPoint->nBuffValue, itPoint->nBuffLevel);

					//死亡图标广播
					if ((itPoint->nBroadCastType & EWBCT_DieIcon) == EWBCT_DieIcon)
					{
						m_CWar->onEntityDieBroadCast(uID, uMurderID, nCamp, nMurderCamp);
					}

					//死亡文字广播
					if ((itPoint->nBroadCastType & EWBCT_DieText) == EWBCT_DieText)
					{
						msg_war_important_monster_dietxtinfo MonsterDieTextInfo;
						MonsterDieTextInfo.MasterEntityType = TYPE_MONSTER;
						MonsterDieTextInfo.MasterHeroID = itPoint->nMonsterID;
						MonsterDieTextInfo.nCamp = nCamp;
						m_CWar->SendToClientMonsterDieBroadText(MonsterDieTextInfo);
					}
					break;
				}
			}

			break;
		}
	}
}


// 更新怪物配置，隐藏的
// nCamp: 阵营
// nMonsterIndex: 怪物配置表中的怪物
void CMonsCrystalExec::UpdateMonsterInfo(int nIndex, int nPam, bool bStatus)
{
}

// 设置保护塔
void CMonsCrystalExec::SetTowerProtect(int nIndex, int nPam, bool bAddOrRemove)
{
	MonsterRefreshVect::iterator refreshIt = m_MonsterRefreshVect.begin();
	for(; refreshIt != m_MonsterRefreshVect.end(); ++refreshIt )
	{
		// 查找到当前的信息，然后进行处理
		if(refreshIt->refreshWarInfo.nIndex == nIndex)
		{
			// 
			CreateMonUIDList::iterator itMonster = refreshIt->creatMonUIDList.begin();
			for(; itMonster != refreshIt->creatMonUIDList.end() ; ++itMonster )
			{
				// 处理
				if (bAddOrRemove)
				{
					if (nPam > 0)
					{
						// 添加保护buff
						AddBuff( itMonster->first, nPam, 1, itMonster->first);
					}
				}
				else
				{
					if (nPam > 0)
					{
						// 移除保护buff
						RemoveBuff(itMonster->first, nPam, itMonster->first, 0);
					}
					
				}
			}
			break;
		}
	}
}

// 添加超级兵
void CMonsCrystalExec::SetSoliderRefresh(int nIndex, int nPam, bool bStatus)
{
	bool bRefresh = nPam > 0 ? true: false;

	// 刷新开关

	if (bStatus == false)
	{
		bRefresh = !bRefresh;
	}

	IMonsterExec* pSoliderExec = m_CWar->GetWarMonsterExecByType(EWME_Soldier);
	if (pSoliderExec != NULL)
	{
		pSoliderExec->UpdateMonsterInfo(nIndex, nPam, bRefresh);
	}
}

void CMonsCrystalExec::SetBaseProtect( int nIndex, int nPam, bool bAddOrRemove)
{
	IMonsterExec* pBaseExec = m_CWar->GetWarMonsterExecByType(EWME_Base);
	if (pBaseExec != NULL)
	{
		pBaseExec->WarEventDeal(EWIE_ProtectBase, nIndex, nPam, bAddOrRemove);
	}
}

void CMonsCrystalExec::WarEventDeal( EWarInfoEvent WarInfoEvent,int nIndex, int nPam, bool bStatus )
{

}

void CMonsCrystalExec::HandleMutexBaseProtect()
{
	map<int,MAP_RecBaseProtect>::iterator iter = m_CampProtectBaseCount.begin();
	for (;iter != m_CampProtectBaseCount.end();++iter)
	{
		bool bAddParam = true;
		int nEffectIndex = 0;
		int nParam = 0;
		MAP_RecBaseProtect::iterator iterRecBaseProtect = iter->second.begin();
		for (;iterRecBaseProtect != iter->second.end(); ++iterRecBaseProtect)
		{
			bAddParam = iterRecBaseProtect->second.bHaveExist & bAddParam;
			nEffectIndex = iterRecBaseProtect->second.nEffectIndex;
			nParam = iterRecBaseProtect->second.nParam;
		}
		SetBaseProtect(nEffectIndex, nParam, bAddParam);
	}
}

void CMonsCrystalExec::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	switch (wEventID)
	{
	case EVENT_ENTITY_DAMAGE:
		{
			if (nLen == sizeof(event_entity_damage))
			{
				event_entity_damage *pData = (event_entity_damage *)pszContext;
				onGetDamage(*pData, dwSrcID);
			}
		}
		break;

	default:
		break;
	}
}

void CMonsCrystalExec::onGetDamage(event_entity_damage damage, DWORD dwSrcUID)
{
	//被攻击了判定发战斗小地图信息
	int nCamp = getProperty_Integer(dwSrcUID,PROPERTY_VOCATION);
	map<UID,DWORD>::iterator iter = m_UIDMiniMapInfoRecord.find(dwSrcUID);
	DWORD curTime = ::GetTickCount();
	if(iter == m_UIDMiniMapInfoRecord.end() && iter->second +  m_CWar->GetPersonConfigInfo()->nCrystalDamageIntervalTime < curTime)
	{
		return;
	}

	m_UIDMiniMapInfoRecord[dwSrcUID] = curTime;

	// 进攻
	msg_entity_fight_mini_map_icon msgMyCampFightInfo;
	msgMyCampFightInfo.dwSrcID = dwSrcUID;
	msgMyCampFightInfo.nInfoType = 2;
	obuf oFightInfo;
	oFightInfo<<msgMyCampFightInfo;
	m_CWar->BroadCastDataToAllOtherCamp(SC_MSG_WAR_ENTITY_FIGHT_MINI_MAP_INFO, oFightInfo.data(), oFightInfo.size(), nCamp);

	// 防守
	msg_entity_fight_mini_map_icon msgOtherCampFightInfo;
	msgOtherCampFightInfo.dwSrcID = dwSrcUID;
	msgMyCampFightInfo.nInfoType = 3;
	oFightInfo.clear();
	oFightInfo<<msgOtherCampFightInfo;
	m_CWar->BroadCastDataToAllPlayer(SC_MSG_WAR_ENTITY_FIGHT_MINI_MAP_INFO, oFightInfo.data(), oFightInfo.size(), nCamp);

}
