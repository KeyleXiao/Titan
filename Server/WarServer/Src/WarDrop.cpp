/*******************************************************************
** 文件名: WarDrop.cpp
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/10/31
** 版  本: 1.0
** 描  述: 战场掉落相关
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/

#include "StdAfx.h"
#include "WarDrop.h"
#include "SWPerson.h"
#include "WarLogRecord.h"
#include "IWar.h"
#include "ExternalHelper.h"
#include "AIExpCtrl.h"
#include "AIEcmCtrl.h"
#include "WarLogRecord.h"
#include "EconomyControl.h"
#include "LvExpControl.h"
#include "IWarMonsterMgr.h"

CWarDrop::CWarDrop(IWar* pWar)
	:m_pWar(pWar)
	,m_pAIEcmCtrl(NULL)
	,m_pAIExpCtrl(NULL)
	,m_pEcmCtrl(NULL)
	,m_pWarLogRecord(NULL)
	,m_pLvExpCtrl(NULL)
	,m_bFirstBlood(false)
	, m_pMiscMgr(NULL)
{
}

CWarDrop::~CWarDrop()
{
	m_pWar = NULL;
}

bool CWarDrop::Init(int nMatchType)
{
	if (m_pWar == NULL)
		return false;

	return true;
}

void CWarDrop::Release()
{
	m_pAIEcmCtrl = NULL;
	m_pAIExpCtrl = NULL;
	m_pEcmCtrl = NULL;
	m_pWarLogRecord = NULL;
	m_pLvExpCtrl = NULL;
	delete this;
}

void CWarDrop::onWarRealStart()
{
	m_pAIEcmCtrl = (IAIEcmCtrlMgr*)m_pWar->getWarMgr(Mgr_AIEcmCtrl);

	m_pAIExpCtrl = (IAIExpCtrlMgr*)m_pWar->getWarMgr(Mgr_AIExpCtrl);

	m_pWarLogRecord = (ILogRecordMgr*)m_pWar->getWarMgr(Mgr_LogRecord);

	m_pEcmCtrl = (IEcmCtrlMgr*)m_pWar->getWarMgr(Mgr_EcmCtrl);

	m_pLvExpCtrl = (ILvExpCtrlMgr*)m_pWar->getWarMgr(Mgr_LvExpCtrl);

	m_pMiscMgr = (IWarMiscManager*)m_pWar->getMiscManager();
}

void CWarDrop::onWarEnd()
{
}

void CWarDrop::addPersonTalentValue(UID realMurderID, UID uMurderID, int nMurderCamp, EWarTalentType TalentType, Vector3 vLoc, int nDistans, UID nMonsterID, int nEXP, int nAdd, float fDecayFactor, ETalentDropType talentDropType)
{
	if (nAdd == 0)
		return;

	// 只处理怪物添加天赋点
	if (uMurderID == INVALID_UID || nMonsterID == INVALID_UID)
	{
		return;
	}

	// 现在阵营是动态了，数目不固定
	if (/*nMurderCamp >= CAMP_MAX || */nMurderCamp < CAMP_NEUTRAL)
	{
		return;
	}

	//发送怪物死亡数据事件
	sendPersonKillMonsterEvent(uMurderID, nMonsterID, nEXP, nAdd);

	//添加天赋点
	switch (TalentType)
	{
	case EWTT_Alone:			// 单独
		{
			addPersonAloneTalentValue(uMurderID, nMurderCamp, nAdd, vLoc, nDistans, talentDropType);
		}
		break;
	case EWTT_Around:			// 周围
		{
			addPersonAroundTalentValue(realMurderID, uMurderID, nMurderCamp, nAdd, vLoc, nDistans, talentDropType);
		}
		break;
	case EWTT_CampAll:			// 己方全部
		{
			addPersonCampTalentValue(uMurderID, nMurderCamp, nAdd, talentDropType);
		}
		break;
	case EWTT_MurderAdd:			// 补刀掉落
		{
			addPersonMurderTalentValue(uMurderID, nMurderCamp, nAdd, talentDropType);
		}
		break;
	case EWTT_MurderAddEX:			// 补刀扩展掉落类型
		{
			addPersonMurderEXTalentValue(uMurderID, nMurderCamp, nAdd, vLoc, nDistans, talentDropType, fDecayFactor);
		}
		break;

	default:
		break;
	}
}

bool CWarDrop::addHeroEXP(EWarAddEXPType AddExpType, UID MurderID, int nMurderCamp, Vector3 vLoc, int nDis, int nAddValue, EExpDropType expDropType)
{
	if (nAddValue == 0)
		return true;

	// 这个函数只处理怪物相关添加经验
	// 现在阵营是动态了，数目不固定
	if (/*nMurderCamp >= CAMP_MAX || */nMurderCamp < CAMP_NEUTRAL)
	{
		return false;
	}

	

	// 标示拿全部的信息
	if (AddExpType == EWEXP_CampAll)
	{
		addPersonCampEXP(MurderID, nMurderCamp, nAddValue, vLoc, nDis, expDropType);
	}
	else if (AddExpType == EWEXP_Around)
	{
		addPersonAroundEXP(MurderID, nMurderCamp, nAddValue, vLoc, nDis, expDropType);
	}
	else if (AddExpType == EWEXP_Alone)
	{
		addPersonAloneEXP(MurderID, nMurderCamp, nAddValue, vLoc, nDis, expDropType);
	}

	return true;
}

bool CWarDrop::addHeroBuffValue(EWarBuffType AddBuffType, UID MurderID, int nMurderCamp, Vector3 vLoc, int nDis, int nBuffValue /*= 0*/, int nLevel /*=0*/)
{
	if (nBuffValue <= 0)
		return true;

	if (MurderID == INVALID_UID)
	{
		return false;
	}

	// 现在阵营是动态了，数目不固定
	if (/*nMurderCamp >= CAMP_MAX || */nMurderCamp < CAMP_NEUTRAL)
	{
		return false;
	}

	//添加天赋点
	switch (AddBuffType)
	{
	case EWBT_Alone:			// 单独
		{
			addPersonAloneBuffValue(MurderID, nMurderCamp, nBuffValue, vLoc, nDis, nLevel);
		}
		break;
	case EWBT_Around:			// 周围
		{
			addPersonAroundBuffValue(MurderID, nMurderCamp, nBuffValue, vLoc, nDis, nLevel);
		}
		break;
	case EWBT_CampAll:			// 己方全部
		{
			addPersonCampBuffValue(MurderID, nMurderCamp, nBuffValue, nLevel);
		}
		break;
	default:
		break;
	}
	return true;
}


void CWarDrop::dealWithAssistsInfo(SWPerson *EntiyDie, SWPerson *EntiyMurder, UID uMurder, vector<msg_war_entity_assist_info>& assistInfoList)
{
	if (EntiyDie == NULL)
	{
		return;
	}

	DWORD nowTick = getTickCountEx();

	int nEntiyDieCamp = EntiyDie->nCamp;

	vector<int> campList = m_pWar->getCampList();
	for (vector<int>::iterator iter = campList.begin(); iter != campList.end(); ++iter)
	{
		SWarCamp* pWarCamp = m_pWar->getWarCamp(*iter);
		if (pWarCamp == NULL)
			continue;

		if (pWarCamp->nCampID == nEntiyDieCamp)
			continue;

		WarPersonVector::iterator itPerson = pWarCamp->warPersVect.begin();
		for (; itPerson != pWarCamp->warPersVect.end(); ++itPerson)
		{
			// 击杀人物不计算助攻
			if (uMurder == itPerson->uID)
				continue;

			bool bAssist = false;
			// 根据增益buff来判断助攻
			if (EntiyMurder != NULL)
			{

				if (nowTick - EntiyMurder->AssistBuffVect[itPerson->uID] <= BENEFIT_BUFF_DURATION)
					bAssist = true;
			}

			// 根据减益buff来判断助攻
			if (EntiyDie != NULL)
			{
				if (nowTick - EntiyDie->AssistBuffVect[itPerson->uID] <= BENEFIT_BUFF_DURATION)
					bAssist = true;
			}

			if (bAssist || EntiyDie->GetDamageVect[itPerson->uID] > 0)
			{
				// 助攻者连死记录清空
				itPerson->nDeadCount = 0;
				
				//itPerson->deadContKillList.push_back(itPerson->nDeadCount);//压入

																		   //记录助攻数量
				itPerson->nBaseRecord[EDT_AssistCount]++;
				itPerson->nAliveContAssist++;
				// 将添加助攻的事件广播出去
				sendAssistEvent(itPerson->uID, EntiyDie->uID, uMurder, itPerson->nBaseRecord[EDT_AssistCount]);

				// 获取助攻者信息
				msg_war_entity_assist_info assistInfo;
				assistInfo.nEntityType = TYPE_PLAYER_ROLE;
				assistInfo.nHeroID = itPerson->nHeroID;
				assistInfo.uidAssist = itPerson->uID;
				sstrcpyn(assistInfo.szName, itPerson->szName, sizeof(assistInfo.szName));
				assistInfoList.push_back(assistInfo);
			}
		}
	}
}

void CWarDrop::dealWithAssistsEXPAndTalentPoint(SWPerson *EntiyDie, UID uMurder, int nAssistPersonCount, EWarDropType WarDropType, int KillOrDeadCount, bool isFirstKill)
{
	if (EntiyDie == NULL)
	{
		return;
	}

	MapGetDamageVect::iterator iter = EntiyDie->GetDamageVect.begin();
	int nAroundCount = EntiyDie->GetDamageVect.size();
	for (; iter != EntiyDie->GetDamageVect.end(); ++iter)
	{
		const UID & uid = iter->first;
		if (uid == uMurder)
		{
			continue;
		}
		else if (UID_2_TYPE(uid) == TYPE_PLAYER_ROLE)
		{
			//伤害大于0
			if (iter->second > 0)
			{
				calculatePersonAddExpAndTalentPoint(EWGP_Assist, EntiyDie->uID, iter->first, nAssistPersonCount, WarDropType, KillOrDeadCount, isFirstKill, nAroundCount);
			}
		}
	}
}

void CWarDrop::dealWithAroundEXPAndTalentPoint(SWPerson *EntiyDie, UID uMurder, int nAroundPersonCount, EWarDropType WarDropType, int KillOrDeadCount, bool isFirstKill)
{
	if (EntiyDie == NULL)
	{
		return;
	}

	int nAroundCount = EntiyDie->DeadPersonAroundEnemy.size();
	for (MapDeadPersonAroundEnemy::iterator iter = EntiyDie->DeadPersonAroundEnemy.begin(); iter != EntiyDie->DeadPersonAroundEnemy.end(); ++iter)
	{
		calculatePersonAddExpAndTalentPoint(EWGP_Around, EntiyDie->uID, iter->first, nAroundPersonCount, WarDropType, KillOrDeadCount, isFirstKill, nAroundCount);
	}
}

bool CWarDrop::checkPersonCanAddTalentInPropertyMask(int nPropertyMask, int nDropType)
{
	// 过滤掉落类型
	if ((nDropType&TalentDropTypeFilter) != nDropType)
	{
		return true;
	}

	// 过滤加钱属性标志位
	return ((nPropertyMask&MASKFLAG_KILL_MONSTER_NOT_ADD_GOLD) != MASKFLAG_KILL_MONSTER_NOT_ADD_GOLD);
}

void CWarDrop::sendPersonKillMonsterEvent(UID uMurderId, UID uDeadId, int nEXP, int nTalentValue)
{
	if (UID_2_TYPE(uMurderId) != TYPE_PLAYER_ROLE)
	{
		return;
	}
	// 获得事件中心接口
	SceneLogicEventHelper eventHelper(m_pWar->getWarSceneID());
	IEventEngine *pEventEngine = eventHelper.m_ptr;
	if (pEventEngine == NULL)
	{
		return;
	}
	// 实体助攻事件
	event_kill_monster EventInfo;
	EventInfo.uMurderUid = uMurderId;
	EventInfo.uDeadId = uDeadId;
	EventInfo.nEXP = nEXP;
	EventInfo.nTalentValue = nTalentValue;

	// 发送角色击杀怪物事件
	pEventEngine->FireExecute(EVENT_GAME_ACTOR_KILL_MONSTER, UID_2_TYPE(uMurderId), uMurderId, (LPCSTR)&EventInfo, sizeof(EventInfo));
}


void CWarDrop::addPersonAloneTalentValue(UID uMurderID, int nMurderCamp, int nAdd, Vector3 vLoc, int nDistans, ETalentDropType talentDropType)
{
	// 不是人物击杀则找周围的人
	if (UID_2_TYPE(uMurderID) != TYPE_PLAYER_ROLE)
	{
		int nPlayerCount = 0;
		int num = 0;
		float fNearest = 0.0f;
		SWarCamp* pWarCamp = m_pWar->getWarCamp(nMurderCamp);
		if(pWarCamp == NULL)
		{
			return;
		}

		// 在指定的阵营中，找到玩家的UID，列表
		WarPersonVector::iterator itPerson = pWarCamp->warPersVect.begin();
		for (; itPerson != pWarCamp->warPersVect.end(); ++itPerson)
		{
			ActorHelper actorHelper(itPerson->uPDBID, ACTOR_DBID);
			IActorService *pActorService = actorHelper.m_ptr;
			if (pActorService == NULL)
			{
				continue;
			}
			// 过滤不能加金币玩家
			int nMask = pActorService->getProperty_Integer(PROPERTY_MASK);
			if (!checkPersonCanAddTalentInPropertyMask(nMask, talentDropType))
			{
				continue;
			}

			//判定距离
			float fActorWithDeathDistance = vLoc.getDistance(pActorService->getLocation());
			if (fActorWithDeathDistance <  nDistans)
			{
				if (fNearest == 0.0f)
				{
					fNearest = vLoc.getDistance(pActorService->getLocation());
					uMurderID = itPerson->uID;
				}
				else
				{
					if (fActorWithDeathDistance <= fNearest)
					{
						// 判定离死亡实体最近的人是凶手
						fNearest = fActorWithDeathDistance;
						uMurderID = itPerson->uID;
					}
				}
			}
		}
	}

	if (UID_2_TYPE(uMurderID) != TYPE_PLAYER_ROLE)
	{
		return;
	}
	// 单独添加天赋点
	ActorHelper actorHelper(uMurderID);
	IActorService *pMurderService = actorHelper.m_ptr;
	if (pMurderService == NULL)
	{
		return;
	}
	SWarCamp* pWarCamp = m_pWar->getWarCamp(nMurderCamp);
	if (pWarCamp == NULL)
	{
		return;
	}

	int nMask = pMurderService->getProperty_Integer(PROPERTY_MASK);
	if (!checkPersonCanAddTalentInPropertyMask(nMask, talentDropType))
	{
		return;
	}

	PDBID pdbIDMurder = pMurderService->getProperty_Integer(PROPERTY_ID);
	if (IsComputerPDBID(pdbIDMurder))
	{
		// 经济差控制
		if (m_pAIEcmCtrl != NULL)
			nAdd = m_pAIEcmCtrl->controlTelentPoint(pdbIDMurder, nAdd, talentDropType);
	}
	else
	{
		// 经济差控制
		if (m_pEcmCtrl != NULL)
			nAdd = m_pEcmCtrl->controlTelentPoint(pdbIDMurder, nAdd, talentDropType);
	}

	if (m_pMiscMgr != NULL)
	{
		m_pMiscMgr->addPersonTalentPoint(uMurderID, nAdd, talentDropType);
	}


	// 获得事件中心接口 击杀人物发送添加天赋点事件
	SceneLogicEventHelper eventHelper(m_pWar->getWarSceneID());
	IEventEngine *pEventEngine = eventHelper.m_ptr;
	if (pEventEngine != NULL)
	{
		// 发出实体创建事件
		event_entity_kill_add_talent_point context;
		context.uidOperator = uMurderID;
		context.uidTarget = 0;
		context.nValue = nAdd; // 增加值

							   // 触发角色创建事件
		pEventEngine->FireExecute(EVENT_GAME_ACTOR_KILL_ADD_TALENT_POINT, UID_2_TYPE(uMurderID), uMurderID, (LPCSTR)&context, sizeof(context));
	}
}

int CWarDrop::calculatePersonAddExpAndTalentPoint(EWarGetPersonDieEXPType nType, UID DeadPerson, UID AttacktPerson, int PersonCount, EWarDropType WarDropType, int KillOrDeadCount, bool isFirstKill, int nAroundCount/* = 1*/)
{
	if (PersonCount <= 0)
	{
		ErrorLn(__FUNCTION__" -- pDeadPerson == NULL PersonCount =" << PersonCount);
		return 0;
	}
	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeCenter == NULL");
		return 0;
	}
	ISchemeWarPersonConfigInfo *  pWarPersonConfigInfo = pSchemeCenter->getSchemeWarPersonConfigInfo();
	if (pWarPersonConfigInfo == NULL)
	{
		ErrorLn(__FUNCTION__": pWarPersonConfigInfo == NULL");
		return 0;
	}
	const SchemeWarPersonConfigInfo* pSchemeWarPersonConfigInfo = pWarPersonConfigInfo->GetSchemeWarPersonConfigInfo(m_pWar->getStaticID());
	if (pSchemeWarPersonConfigInfo == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeWarPersonConfigInfo == NULL");
		return 0;
	}
	ISchemeLevelUpgrade* pSchemeLevelUpgrade = pSchemeCenter->getLevelUpgradeSheme();
	if (pSchemeLevelUpgrade == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeLevelUpgrade == NULL");
		return 0;
	}

	ActorHelper deadActorHelper(DeadPerson);
	IActorService *pDeadPerson = deadActorHelper.m_ptr;
	if (pDeadPerson == NULL)
	{
		ErrorLn(__FUNCTION__": -- pDeadPerson == NULL DeadPerson =" << DeadPerson << " nType =" << nType);
		return 0;
	}
	ActorHelper attackActorHelper(AttacktPerson);
	IActorService *pAttacktPerson = attackActorHelper.m_ptr;
	if (pAttacktPerson == NULL)
	{
		ErrorLn(__FUNCTION__" -- pAttacktPerson == NULL AttacktPerson =" << AttacktPerson << " nType =" << nType);
		return 0;
	}

	PDBID attackerPdbID = pAttacktPerson->getProperty_Integer(PROPERTY_ID);

	int nMaxLevel = pSchemeLevelUpgrade->GetSchemeLevelUpgradeMaxLevel();
	int nDeadPersonLevel = min(pDeadPerson->getProperty_Integer(PROPERTY_LEVEL), nMaxLevel);
	int nCurLevelNeedExp = pSchemeLevelUpgrade->GetNextLevelExp(nDeadPersonLevel);
	float fDieExpRate = floatZeroSet(pSchemeWarPersonConfigInfo->fDieExpRate, 1.0f);
	float fAssistTalentRate = floatZeroSet(pSchemeWarPersonConfigInfo->fAssistTalentRate, 1.0f);
	int nDieTalentPoint = pSchemeWarPersonConfigInfo->nDieTalentPoint;
	int AddEXPValue = (int)((nCurLevelNeedExp * fDieExpRate) / PersonCount);
	int AddTalentPoint = (int)(nDieTalentPoint * fAssistTalentRate);

	// 是否需要经济差控制
	bool bNeedEcmCtrl = true;

	AddEXPValue = AddEXPValue > 0 ? AddEXPValue : 1;
	if (nType == EWGP_Kill)
	{
		AddTalentPoint = isFirstKill ? pSchemeWarPersonConfigInfo->nFirstKillGetMoney : nDieTalentPoint;
	}
	else if (nType == EWGP_Assist)
	{
		//AddTalentPoint = AddTalentPoint > 0 ? AddTalentPoint : 1;
		AddTalentPoint = isFirstKill ? pSchemeWarPersonConfigInfo->nFirstKillGetMoney : nDieTalentPoint;
		AddTalentPoint *= fAssistTalentRate;

#ifdef __WAR__HERO_GET_EXP__TALENT__DEBUG__
		WarningLn(__FUNCTION__": type=" << nType << ",uid=" << AttacktPerson << ",AddTalentPoint=" << AddTalentPoint << ", isFirstKill=" << isFirstKill);
#endif
	}
	else if (nType == EWGP_Around)
	{
		AddTalentPoint = 0;
	}

	// 连杀 连死两次以上才会衰减
	//掉落会衰减，衰减公式：实际掉落＝ 基础值*（1-max(0.8,(被杀次数 -1)*配置系数)）
	if (WarDropType == EWDT_Damping)
	{
		// 衰减掉落
		if (!isFloatEqualZero(pSchemeWarPersonConfigInfo->fDropDumpingEXPRate))
		{
			AddEXPValue = (int)(AddEXPValue *(1 - min(0.8, (KillOrDeadCount - 1)* pSchemeWarPersonConfigInfo->fDropDumpingEXPRate)));
		}

		if (!isFloatEqualZero(pSchemeWarPersonConfigInfo->fDropDumpingTalentRate))
		{
			AddTalentPoint = (int)(AddTalentPoint *(1 - min(0.8, (KillOrDeadCount - 1)* pSchemeWarPersonConfigInfo->fDropDumpingTalentRate)));
		}
	}
	//掉落会递增，加成公式：实际掉落＝基础值*（1+(连杀次数 -1)*配置系数）
	else if (WarDropType == EWDT_Gain)
	{
		// 增益掉落
		if (!isFloatEqualZero(pSchemeWarPersonConfigInfo->fDropGainTalentRate))
		{
			AddEXPValue = (int)(AddEXPValue *(1 + (KillOrDeadCount - 1)* pSchemeWarPersonConfigInfo->fDropGainTalentRate));
		}

		// 连杀天赋点数优化方案一
		//AddTalentPoint =(int)(nNewAdd *(1 + (KillOrDeadCount-1)* pSchemeWarPersonConfigInfo->fDropGainTalentRate));

		// 连杀机制优化,方案二
		float fEconomyControlRate = pSchemeWarPersonConfigInfo->fEconomyControlRate;
		float fDropGainTalentRate = pSchemeWarPersonConfigInfo->fDropGainTalentRate;
		int nMaxMultiKill = pSchemeWarPersonConfigInfo->nMaxMultiKill;

		if (IsComputerPDBID(attackerPdbID))
		{
			if (m_pAIEcmCtrl != NULL)
			{
				AddTalentPoint = m_pAIEcmCtrl->controlContKillTelentPoint(pDeadPerson, pAttacktPerson, AddTalentPoint, fDropGainTalentRate, fEconomyControlRate, nMaxMultiKill, KillOrDeadCount);
			}
		}
		else
		{
			if (m_pEcmCtrl != NULL)
			{
				AddTalentPoint = m_pEcmCtrl->controlContKillTelentPoint(pDeadPerson, pAttacktPerson, AddTalentPoint, fDropGainTalentRate, fEconomyControlRate, nMaxMultiKill, KillOrDeadCount);
			}
		}

		bNeedEcmCtrl = false;
	}


	// 增加经验
	if (AddEXPValue > 0 && DeadPerson != AttacktPerson)
	{
		//加成系数: 掉落经验*（1+(分享人数 - 1)*加成系数）
		if (PersonCount > 0)
		{
			float fKillPersonDropEXPRate = floatZeroSet(pSchemeWarPersonConfigInfo->fKillPersonDropEXPRate, 1.0f);
			AddEXPValue = AddEXPValue*(1 + (PersonCount - 1)* fKillPersonDropEXPRate);
		}

		int nPlayerLv = pAttacktPerson->getProperty_Integer(PROPERTY_LEVEL);

		if (m_pLvExpCtrl != NULL)
			m_pLvExpCtrl->controlLvExp(nPlayerLv, nDeadPersonLevel, nAroundCount, AddEXPValue);

		if (IsComputerPDBID(attackerPdbID))
		{
			if (m_pAIExpCtrl != NULL)
			{
				AddEXPValue = m_pAIExpCtrl->controlExp(attackerPdbID, AddEXPValue, EEXPDROPTYPE_PLAYER);
			}
		}

		if (m_pMiscMgr != NULL)
		{
			m_pMiscMgr->addPersonExp(AttacktPerson, AddEXPValue, EEXPDROPTYPE_PLAYER);
		}
	}

	// 增加天赋
	if (AddTalentPoint > 0 && DeadPerson != AttacktPerson)
	{
		int nMask = pAttacktPerson->getProperty_Integer(PROPERTY_MASK);
		if ((nMask&MASKFLAG_KILL_ROLE_NOT_ADD_GOLD) != MASKFLAG_KILL_ROLE_NOT_ADD_GOLD)
		{
			// 经济差控制
			int nNewAdd = AddTalentPoint;
			if (bNeedEcmCtrl)
			{
				if (IsComputerPDBID(attackerPdbID))
				{
					if (m_pAIEcmCtrl != NULL)
					{
						nNewAdd = m_pAIEcmCtrl->controlTelentPoint(attackerPdbID, AddTalentPoint, ETALENTDROPTYPE_PLAYER);
					}
				}
				else
				{
					if (m_pEcmCtrl != NULL)
					{
						nNewAdd = m_pEcmCtrl->controlTelentPoint(attackerPdbID, AddTalentPoint, ETALENTDROPTYPE_PLAYER);
					}
				}

			}

			if (m_pMiscMgr != NULL)
			{
				m_pMiscMgr->addPersonTalentPoint(AttacktPerson, nNewAdd, ETALENTDROPTYPE_PLAYER);
			}
		}
	}

	//if (getWarShowAddEXPAndMoney() == true)
	//{
		// 打印信息
		//TraceLn(__FUNCTION__" -- EWarGetPersonDieEXPType =" << nType << "[" << pAttacktPerson->getName().c_str() << "]" << " killed " << "[" << pDeadPerson->getName().c_str() << "]"
			//<< ": AddEXPValue = " << AddEXPValue << " AddTalentPoint = " << AddTalentPoint);
	//}

#ifdef __WAR__HERO_GET_EXP__TALENT__DEBUG__
	ErrorLn("CMobaWar::calculatePersonAddExpAndTalentPoint() -- PersonCount" << PersonCount << " nType =" << nType << " Name" << pAttacktPerson->getName().c_str() << ": AddEXPValue = " << AddEXPValue << " AddTalentPoint = " << AddTalentPoint);
#endif // _DEBUG

	// 获得事件中心接口 击杀人物发送添加天赋点事件
	SceneLogicEventHelper eventHelper(m_pWar->getWarSceneID());
	IEventEngine *pEventEngine = eventHelper.m_ptr;
	if (pEventEngine)
	{
		// 发出实体创建事件
		event_entity_kill_add_talent_point context;
		context.uidOperator = AttacktPerson;
		context.uidTarget = DeadPerson;
		context.nValue = AddTalentPoint; // 增加值

										 // 触发角色创建事件
		pEventEngine->FireExecute(EVENT_GAME_ACTOR_KILL_ADD_TALENT_POINT, UID_2_TYPE(AttacktPerson), AttacktPerson, (LPCSTR)&context, sizeof(context));
	}

	//WarningLn(__FUNCTION__":type=" << nType << ",uid=" << AttacktPerson << ",AddTalentPoint=" << AddTalentPoint);
	return AddTalentPoint;
}

void CWarDrop::addPersonMurderEXTalentValue(UID uMurderID, int nMurderCamp, int nAdd, Vector3 vLoc, int nDistans, ETalentDropType talentDropType, float fDecayFactor /*= 0.0f*/)
{
	UID uidArray[MAX_WAR_NEAREST_COUNT] = { 0 };
	int num = 0;

	SWarCamp* pWarCamp = m_pWar->getWarCamp(nMurderCamp);
	if (pWarCamp == NULL)
	{
		return;
	}

	// 在指定的阵营中，找到玩家的UID，列表
	WarPersonVector::iterator itPerson = pWarCamp->warPersVect.begin();
	for (; itPerson != pWarCamp->warPersVect.end(); ++itPerson)
	{
		ActorHelper actorHelper(itPerson->uPDBID, ACTOR_DBID);
		IActorService *pActorService = actorHelper.m_ptr;
		if (pActorService == NULL)
		{
			continue;
		}

		int nMask = pActorService->getProperty_Integer(PROPERTY_MASK);
		if (!checkPersonCanAddTalentInPropertyMask(nMask, talentDropType))
		{
			continue;
		}

		if (MAX_WAR_NEAREST_COUNT <= num || pActorService->getState() == ENTITY_STATE_DIE)
		{
			continue;
		}

		// 按照之前的传飞逻辑击杀者和附近的队友获得金币是互斥的
		if (uMurderID == itPerson->uID)
		{
			num = 0;
			memset(uidArray, 0, sizeof(uidArray));
			uidArray[num++] = uMurderID;
			break;
		}
		//判定距离
		if (vLoc.getDistance(pActorService->getLocation()) < nDistans)
		{
			uidArray[num++] = itPerson->uID;
		}
	}

	if (num <= 0)
	{
		return;
	}

	// 获得事件中心接口 击杀人物发送添加天赋点事件
	SceneLogicEventHelper eventHelper(m_pWar->getWarSceneID());
	IEventEngine *pEventEngine = eventHelper.m_ptr;


	if (fDecayFactor > 0.0f)
	{
		nAdd = (int)((float)nAdd*fDecayFactor);
	}

	//没有击杀者击杀 则周围任务平分
	int nAverageAddValue = (int)(nAdd / num);
	nAverageAddValue = nAverageAddValue>0 ? nAverageAddValue : 1;

	for (int i = 0; i < num; ++i)
	{
		if (uidArray[i] == INVALID_UID || UID_2_TYPE(uidArray[i]) != TYPE_PLAYER_ROLE)
		{
			continue;
		}

		UID uid = uidArray[i];
		ActorHelper actorHelper(uidArray[i]);
		IActorService *pActorService = actorHelper.m_ptr;
		if (pActorService == NULL)
		{
			continue;
		}
		PDBID pdbIDMurder = pActorService->getProperty_Integer(PROPERTY_ID);
		// 经济差控制
		int nNewAdd = nAverageAddValue;
		if (IsComputerPDBID(pdbIDMurder))
		{
			if (m_pAIEcmCtrl != NULL)
				nNewAdd = m_pAIEcmCtrl->controlTelentPoint(pdbIDMurder, nAverageAddValue, talentDropType);
		}
		else
		{
			if (m_pEcmCtrl != NULL)
				nNewAdd = m_pEcmCtrl->controlTelentPoint(pdbIDMurder, nAverageAddValue, talentDropType);
		}

		if (m_pMiscMgr != NULL)
		{
			m_pMiscMgr->addPersonTalentPoint(uid, nNewAdd, talentDropType);
		}

		if (pEventEngine)
		{
			// 发出实体创建事件
			event_entity_kill_add_talent_point context;
			context.uidOperator = uMurderID;
			context.uidTarget = 0;
			context.nValue = nNewAdd; // 增加值

									  // 触发角色创建事件
			pEventEngine->FireExecute(EVENT_GAME_ACTOR_KILL_ADD_TALENT_POINT, UID_2_TYPE(uMurderID), uMurderID, (LPCSTR)&context, sizeof(context));
		}
	}
}

void CWarDrop::addPersonMurderTalentValue(UID uMurderID, int nMurderCamp, int nAdd, ETalentDropType talentDropType)
{
	if (nAdd == 0)
	{
		return;
	}
	if (UID_2_TYPE(uMurderID) != TYPE_PLAYER_ROLE)
	{
		return;
	}
	SWarCamp* pWarCamp = m_pWar->getWarCamp(nMurderCamp);
	if (pWarCamp == NULL)
	{
		return;
	}

	int nPDBID = INVALID_PDBID;
	WarPersonVector::iterator itPerson = pWarCamp->warPersVect.begin();
	for (; itPerson != pWarCamp->warPersVect.end(); ++itPerson)
	{
		if (itPerson->uID == uMurderID)
		{
			nPDBID = itPerson->uPDBID;
			break;
		}
	}
	if (nPDBID == INVALID_PDBID)
	{
		return;
	}

	// 补兵添加天赋点
	ActorHelper actorHelper(uMurderID);
	IActorService *pMurderService = actorHelper.m_ptr;
	if (pMurderService == NULL)
	{
		return;
	}

	int nMask = pMurderService->getProperty_Integer(PROPERTY_MASK);
	if (!checkPersonCanAddTalentInPropertyMask(nMask, talentDropType))
	{
		return;
	}

	// 更新天赋点
	PDBID pdbIDMurder = pMurderService->getProperty_Integer(PROPERTY_ID);
	// 经济差控制
	int nNewAdd = nAdd;

	if (IsComputerPDBID(pdbIDMurder))
	{
		if (m_pAIEcmCtrl != NULL)
			nNewAdd = m_pAIEcmCtrl->controlTelentPoint(pdbIDMurder, nAdd, talentDropType);
	}
	else
	{
		if (m_pEcmCtrl != NULL)
			nNewAdd = m_pEcmCtrl->controlTelentPoint(pdbIDMurder, nAdd, talentDropType);
	}

	if (m_pMiscMgr != NULL)
	{
		m_pMiscMgr->addPersonTalentPoint(uMurderID, nAdd, talentDropType);
	}

	// 获得事件中心接口 击杀人物发送添加天赋点事件
	SceneLogicEventHelper eventHelper(m_pWar->getWarSceneID());
	IEventEngine *pEventEngine = eventHelper.m_ptr;
	if (pEventEngine)
	{
		event_entity_kill_add_talent_point context;
		context.uidOperator = uMurderID;
		context.uidTarget = 0;
		context.nValue = nNewAdd; // 增加值

								  // 触发角色创建事件
		pEventEngine->FireExecute(EVENT_GAME_ACTOR_KILL_ADD_TALENT_POINT, UID_2_TYPE(uMurderID), uMurderID, (LPCSTR)&context, sizeof(context));
	}
}


void CWarDrop::addPersonCampTalentValue(UID uMurderID, int nMurderCamp, int nAdd, ETalentDropType talentDropType)
{
	// 给己方阵营添加天赋点
	SWarCamp* pWarCamp = m_pWar->getWarCamp(nMurderCamp);
	if (pWarCamp == NULL)
	{
		return;
	}

	WarPersonVector::iterator itPerson = pWarCamp->warPersVect.begin();
	for (; itPerson != pWarCamp->warPersVect.end(); ++itPerson)
	{
		ActorHelper actorHelper(itPerson->uPDBID, ACTOR_DBID);
		IActorService *pActorService = actorHelper.m_ptr;
		if (pActorService == NULL)
		{
			if (!itPerson->bIsComputer)
			{
				ErrorLn(__FUNCTION__": pActorService == NULL PDBID = " << itPerson->uPDBID << " UID = " << itPerson->uID);
			}
			continue;
		}
		// 这块的判断待定  
		int nMask = pActorService->getProperty_Integer(PROPERTY_MASK);
		if (!checkPersonCanAddTalentInPropertyMask(nMask, talentDropType))
		{
			continue;
		}

		PDBID pdbIDMurder = pActorService->getProperty_Integer(PROPERTY_ID);
		// 经济差控制
		int nNewAdd = nAdd;
		if (IsComputerPDBID(pdbIDMurder))
		{
			if (m_pAIEcmCtrl != NULL)
				nNewAdd = m_pAIEcmCtrl->controlTelentPoint(pdbIDMurder, nAdd, talentDropType);
		}
		else
		{
			if (m_pEcmCtrl != NULL)
				nNewAdd = m_pEcmCtrl->controlTelentPoint(pdbIDMurder, nAdd, talentDropType);
		}

		if (m_pMiscMgr != NULL)
		{
			m_pMiscMgr->addPersonTalentPoint(itPerson->uID, nNewAdd, talentDropType);
		}
	}
}

void CWarDrop::addPersonAroundTalentValue(UID realMurderID, UID uMurderID, int nMurderCamp, int nAdd, Vector3 vLoc, int nDistans, ETalentDropType talentDropType)
{
	// 给周围玩家添加天赋点
	UID uidArray[MAX_WAR_NEAREST_COUNT] = { 0 };
	int num = 0;

	SWarCamp* pWarCamp = m_pWar->getWarCamp(nMurderCamp);
	if (pWarCamp == NULL)
	{
		return;
	}

	// 在指定的阵营中，找到玩家的UID，列表
	WarPersonVector::iterator itPerson = pWarCamp->warPersVect.begin();
	for (; itPerson != pWarCamp->warPersVect.end(); ++itPerson)
	{
		ActorHelper actorHelper(itPerson->uPDBID, ACTOR_DBID);
		IActorService *pActorService = actorHelper.m_ptr;
		if (pActorService == NULL)
		{
			continue;
		}

		// 拥有這些标誌的不给予金币分配
		int nMask = pActorService->getProperty_Integer(PROPERTY_MASK);
		if (!checkPersonCanAddTalentInPropertyMask(nMask, talentDropType))
		{
			continue;
		}

		if (MAX_WAR_NEAREST_COUNT <= num || pActorService->getState() == ENTITY_STATE_DIE)
		{
			continue;
		}

		//判定距离 把击杀者也加进来
		if (vLoc.getDistance(pActorService->getLocation()) < nDistans || uMurderID == itPerson->uID)
		{
			uidArray[num++] = itPerson->uID;
		}
	}

	if (num <= 0)
	{
		return;
	}

	// 获得事件中心接口 击杀人物发送添加天赋点事件
	SceneLogicEventHelper eventHelper(m_pWar->getWarSceneID());
	IEventEngine *pEventEngine = eventHelper.m_ptr;


	IWarMiscManager* warMiscMgr = m_pWar->getMiscManager();
	if (warMiscMgr == NULL)
	{
		ErrorLn(__FUNCTION__":warMiscMgr == NULL");
		return;
	}


	int nAverageAddValue = 0;
	const SchemeWarPersonConfigInfo *pPersonConfigInfo = warMiscMgr->getPersonConfigInfo();
	if (pPersonConfigInfo == NULL)
	{
		return;
	}


	// 分享者系数
	float ratio1 = 2.0f - 2.0f / (float)num;
	float ratio2 = pow(pPersonConfigInfo->fGoldShareBase, ratio1);
	float ratio = ratio2 / (float)num;
	if (isMonster(realMurderID) && getProperty_Integer(realMurderID, PROPERTY_SEX) == MONSTER_TYPE_TOWER)// 塔击杀
	{
		nAverageAddValue = (nAdd * ratio * pPersonConfigInfo->fTowerKillDecrease);
		//WarningLn(a2utf8("addPersonAroundTalentValue,塔击杀, nAverageAddValue=") << nAverageAddValue << ",nAdd=" << nAdd << ",num=" << num << ",分享者系数=" << ratio);
	}
	else
	{
		nAverageAddValue = (nAdd * ratio);
		//WarningLn(a2utf8("addPersonAroundTalentValue,小兵金币分享, nAverageAddValue=") << nAverageAddValue << ",nAdd=" << nAdd << ",num=" << num << ",分享者系数=" << ratio);
	}

	nAverageAddValue = nAverageAddValue>0 ? nAverageAddValue : 1;

	for (int i = 0; i < num; ++i)
	{
		if (uidArray[i] == INVALID_UID || UID_2_TYPE(uidArray[i]) != TYPE_PLAYER_ROLE)
		{
			continue;
		}
		UID uid = uidArray[i];
		ActorHelper actorHelper(uid);
		IActorService *pActorService = actorHelper.m_ptr;
		if (pActorService == NULL)
		{
			continue;
		}

		PDBID pdbIDMurder = pActorService->getProperty_Integer(PROPERTY_ID);
		// 经济差控制
		int nNewAdd = nAverageAddValue;
		if (IsComputerPDBID(pdbIDMurder))
		{
			if (m_pAIEcmCtrl != NULL)
				nNewAdd = m_pAIEcmCtrl->controlTelentPoint(pdbIDMurder, nAverageAddValue, talentDropType);
		}
		else
		{
			if (m_pEcmCtrl != NULL)
				nNewAdd = m_pEcmCtrl->controlTelentPoint(pdbIDMurder, nAverageAddValue, talentDropType);
		}

		if (m_pMiscMgr != NULL)
		{
			m_pMiscMgr->addPersonTalentPoint(uid, nNewAdd, talentDropType);
		}

		if (pEventEngine)
		{
			// 发出实体创建事件
			event_entity_kill_add_talent_point context;
			context.uidOperator = uMurderID;
			context.uidTarget = 0;
			context.nValue = nNewAdd; // 增加值

									  // 触发角色创建事件
			pEventEngine->FireExecute(EVENT_GAME_ACTOR_KILL_ADD_TALENT_POINT, UID_2_TYPE(uMurderID), uMurderID, (LPCSTR)&context, sizeof(context));
		}

	}
}

void CWarDrop::addPersonAloneEXP(UID uMurderID, int nMurderCamp, int nAdd, Vector3 vLoc, int nDistans, EExpDropType expDropType)
{
	if (nAdd == 0)
		return;

	SWarCamp* pWarCamp = m_pWar->getWarCamp(nMurderCamp);
	if (pWarCamp == NULL)
	{
		return;
	}

	//如果是怪物击杀
	if (UID_2_TYPE(uMurderID) == TYPE_MONSTER)
	{
		//给同阵营周围玩家随机加一个
		float fNearest = 0.0f;

		// 在指定的阵营中，找到玩家的UID，列表
		WarPersonVector & playerList = pWarCamp->warPersVect;
		for (WarPersonVector::iterator itPerson = playerList.begin(); itPerson != playerList.end(); ++itPerson)
		{
			ActorHelper actorHelper(itPerson->uPDBID, ACTOR_DBID);
			IActorService *pActorService = actorHelper.m_ptr;
			if (pActorService == NULL)
			{
				continue;
			}

			//判定距离
			float fActorWithDeathDistance = vLoc.getDistance(pActorService->getLocation());
			if (fActorWithDeathDistance <  nDistans)
			{
				if (fNearest == 0.0f)
				{
					fNearest = vLoc.getDistance(pActorService->getLocation());
					uMurderID = itPerson->uID;
				}
				else
				{
					if (fActorWithDeathDistance <= fNearest)
					{
						// 判定离死亡实体最近的人是凶手
						fNearest = fActorWithDeathDistance;
						uMurderID = itPerson->uID;
					}
				}
			}
		}
	}

	if (UID_2_TYPE(uMurderID) != TYPE_PLAYER_ROLE)
	{
		return;
	}

	int nPDBID = INVALID_PDBID;
	WarPersonVector::iterator itPerson = pWarCamp->warPersVect.begin();
	for (; itPerson != pWarCamp->warPersVect.end(); ++itPerson)
	{
		if (itPerson->uID == uMurderID)
		{
			nPDBID = itPerson->uPDBID;
			break;
		}
	}
	if (nPDBID == INVALID_PDBID)
		return;

	ActorHelper actorHelper(uMurderID);
	IActorService *pMurderService = actorHelper.m_ptr;
	// 不在线的角色不加数值
	if (pMurderService == NULL)
	{
		return;
	}

	int nLevel = pMurderService->getProperty_Integer(PROPERTY_LEVEL);

	if (m_pLvExpCtrl != NULL)
		m_pLvExpCtrl->controlLvExp(expDropType, nLevel, nMurderCamp, vLoc, nDistans, nAdd);

	if (nAdd != 0)
	{
		if (IsComputerPDBID(nPDBID))
		{
			if (m_pAIExpCtrl != NULL)
			{
				nAdd = m_pAIExpCtrl->controlExp(nPDBID, nAdd, expDropType);
			}
		}

		if (m_pMiscMgr != NULL)
		{
			m_pMiscMgr->addPersonExp(uMurderID, nAdd, expDropType);
		}
	}
}

void CWarDrop::addPersonAroundEXP(UID uMurderID, int nMurderCamp, int nAdd, Vector3 vLoc, int nDistans, EExpDropType expDropType)
{
	if (nAdd == 0)
		return;

	//平均经验值
	int nAverageAddValue = 0;
	UID uidArray[MAX_WAR_NEAREST_COUNT] = { 0 };
	int num = 0;

	SWarCamp* pWarCamp = m_pWar->getWarCamp(nMurderCamp);
	if (pWarCamp == NULL)
	{
		return;
	}

	// 在指定的阵营中，找到玩家的UID，列表
	WarPersonVector::iterator itPerson = pWarCamp->warPersVect.begin();
	for (; itPerson != pWarCamp->warPersVect.end(); ++itPerson)
	{
		ActorHelper actorHelper(itPerson->uPDBID, ACTOR_DBID);
		IActorService *pActorService = actorHelper.m_ptr;
		if (pActorService == NULL)
		{
			continue;
		}

		//判定距离
		if (vLoc.getDistance(pActorService->getLocation()) < nDistans)
		{
			if (MAX_WAR_NEAREST_COUNT > num && pActorService->getState() != ENTITY_STATE_DIE)
			{
				uidArray[num++] = itPerson->uID;
			}
		}
	}


	if (num <= 0)
	{
		if (UID_2_TYPE(uMurderID) == TYPE_PLAYER_ROLE)
		{
			num = 0;
			ActorHelper actorHelper(uMurderID);
			IActorService *pActorService = actorHelper.m_ptr;
			if (pActorService != NULL)
			{
				if (pActorService->getState() != ENTITY_STATE_DIE)
				{
					uidArray[num++] = uMurderID;
				}
			}
		}
	}
	else
	{
		int bHaveFound = false;
		//检查是否杀手在uid列表中 不在则添加进列表
		for (int i = 0; i < num; ++i)
		{
			if (uidArray[i] == INVALID_UID || UID_2_TYPE(uidArray[i]) != TYPE_PLAYER_ROLE)
			{
				continue;
			}

			if (uMurderID == uidArray[i])
			{
				bHaveFound = true;
				break;
			}
		}

		if (!bHaveFound && UID_2_TYPE(uMurderID) == TYPE_PLAYER_ROLE && MAX_WAR_NEAREST_COUNT > num)
		{
			ActorHelper actorHelper(uMurderID);
			IActorService *pActorService = actorHelper.m_ptr;
			if (pActorService != NULL)
			{
				if (pActorService->getState() != ENTITY_STATE_DIE)
				{
					uidArray[num++] = uMurderID;
				}
			}
		}
	}

	if (num <= 0)
	{
		return;
	}
	nAverageAddValue = (int)(nAdd / num);
	nAverageAddValue = nAverageAddValue>0 ? nAverageAddValue : 1;

	for (int i = 0; i < num; ++i)
	{
		int nAddValue = nAverageAddValue;
		if (uidArray[i] == INVALID_UID || UID_2_TYPE(uidArray[i]) != TYPE_PLAYER_ROLE)
		{
			continue;
		}

		UID uid = uidArray[i];
		ActorHelper actorHelper(uid);
		IActorService *pActorService = actorHelper.m_ptr;
		if (pActorService == NULL)
		{
			continue;
		}

		int nPLayerLv = pActorService->getProperty_Integer(PROPERTY_LEVEL);
		int nCamp = pActorService->getProperty_Integer(PROPERTY_CAMP);

		if (m_pLvExpCtrl != NULL)
			m_pLvExpCtrl->controlLvExp(expDropType, nPLayerLv, nCamp, vLoc, nDistans, nAddValue, true);

		PDBID pDBID = pActorService->getProperty_Integer(PROPERTY_ID);


		if (IsComputerPDBID(pDBID))
		{
			if (m_pAIExpCtrl != NULL)
			{
				nAddValue = m_pAIExpCtrl->controlExp(pDBID, nAddValue, expDropType);
			}
		}

		if (m_pMiscMgr != NULL)
		{
			m_pMiscMgr->addPersonExp(uid, nAddValue, expDropType);
		}

		// 查找人物并添加经验
		SWarCamp* pWarCamp = m_pWar->getWarCamp(nMurderCamp);
		if (pWarCamp == NULL)
		{
			return;
		}
	
		WarPersonVector::iterator itFindPerson = pWarCamp->warPersVect.begin();
		for (; itFindPerson != pWarCamp->warPersVect.end(); ++itFindPerson)
		{
			if (uidArray[i] == itFindPerson->uID)
			{
				itFindPerson->nExperien = itFindPerson->nExperien + nAddValue;
			}
		}
	}
}

void CWarDrop::addPersonCampEXP(UID uMurderID, int nMurderCamp, int nAdd, Vector3 vLoc, int nDistans, EExpDropType expDropType)
{
	if (nAdd == 0)
		return;

	SWarCamp* pWarCamp = m_pWar->getWarCamp(nMurderCamp);
	if (pWarCamp == NULL)
	{
		return;
	}

	WarPersonVector::iterator itPerson = pWarCamp->warPersVect.begin();
	for (; itPerson != pWarCamp->warPersVect.end(); ++itPerson)
	{
		int AddValue = nAdd;
		ActorHelper actorHelper(itPerson->uPDBID, ACTOR_DBID);
		IActorService *pActorService = actorHelper.m_ptr;
		if (pActorService == NULL)
		{
			continue;
		}

		int nPLayerLv = pActorService->getProperty_Integer(PROPERTY_LEVEL);
		int nCamp = pActorService->getProperty_Integer(PROPERTY_CAMP);

		if (m_pLvExpCtrl != NULL)
			m_pLvExpCtrl->controlLvExp(expDropType, nPLayerLv, nCamp, vLoc, nDistans, AddValue);

		if (itPerson->bIsComputer)
		{
			if (m_pAIExpCtrl != NULL)
			{
				AddValue = m_pAIExpCtrl->controlExp(itPerson->uPDBID, AddValue, expDropType);
			}
		}

		if (m_pMiscMgr != NULL)
		{
			m_pMiscMgr->addPersonExp(itPerson->uID, AddValue, expDropType);
		}
	}
}


void CWarDrop::addPersonAloneBuffValue(UID uMurderID, int nMurderCamp, int nAdd, Vector3 vLoc, int nDistans, int nLevel)
{
	// 不是人物击杀则找周围的人
	if (UID_2_TYPE(uMurderID) != TYPE_PLAYER_ROLE)
	{
		float fNearest = 0.0f;
		SWarCamp* pWarCamp = m_pWar->getWarCamp(nMurderCamp);
		if (pWarCamp == NULL)
		{
			return;
		}

		// 在指定的阵营中，找到玩家的UID，列表
		WarPersonVector::iterator itPerson = pWarCamp->warPersVect.begin();
		for (; itPerson != pWarCamp->warPersVect.end(); ++itPerson)
		{
			ActorHelper actorHelper(itPerson->uPDBID, ACTOR_DBID);
			IActorService *pActorService = actorHelper.m_ptr;
			if (pActorService == NULL)
			{
				continue;
			}

			//判定距离
			float fActorWithDeathDistance = vLoc.getDistance(pActorService->getLocation());
			if (fActorWithDeathDistance <  nDistans)
			{
				if (fNearest == 0.0f)
				{
					fNearest = vLoc.getDistance(pActorService->getLocation());
					uMurderID = itPerson->uID;
				}
				else
				{
					if (fActorWithDeathDistance <= fNearest)
					{
						// 判定离死亡实体最近的人是凶手
						fNearest = fActorWithDeathDistance;
						uMurderID = itPerson->uID;
					}
				}
			}
		}
	}

	if (UID_2_TYPE(uMurderID) != TYPE_PLAYER_ROLE)
	{
		return;
	}
	// 单独添加buff
	//添加buff
	if (nAdd > 0)
	{
		ActorHelper actorHelper(uMurderID);
		IActorService *pMurderService = actorHelper.m_ptr;
		if (pMurderService == NULL)
		{
			return;
		}
		SWarCamp* pWarCamp = m_pWar->getWarCamp(nMurderCamp);
		if (pWarCamp == NULL)
		{
			return;
		}

		WarPersonVector::iterator itPerson = pWarCamp->warPersVect.begin();
		for (; itPerson != pWarCamp->warPersVect.end(); ++itPerson)
		{
			if (itPerson->uID == uMurderID)
			{
				AddBuff(itPerson->uID, nAdd, nLevel, itPerson->uID);
				break;
			}
		}
	}
}

void CWarDrop::addPersonAroundBuffValue(UID uMurderID, int nMurderCamp, int nAdd, Vector3 vLoc, int nDistans, int nLevel)
{
	// 给周围玩家添加buff
	UID uidArray[MAX_WAR_NEAREST_COUNT] = { 0 };
	int num = 0;

	SWarCamp* pWarCamp = m_pWar->getWarCamp(nMurderCamp);
	if (pWarCamp == NULL)
	{
		return;
	}

	// 在指定的阵营中，找到玩家的UID，列表
	WarPersonVector::iterator itPerson = pWarCamp->warPersVect.begin();
	for (; itPerson != pWarCamp->warPersVect.end(); ++itPerson)
	{
		ActorHelper actorHelper(itPerson->uPDBID, ACTOR_DBID);
		IActorService *pActorService = actorHelper.m_ptr;
		if (pActorService == NULL)
		{
			continue;
		}

		//判定距离
		if (vLoc.getDistance(pActorService->getLocation()) < nDistans)
		{
			if (MAX_WAR_NEAREST_COUNT > num)
			{
				uidArray[num++] = itPerson->uID;
			}
		}
	}


	if (num <= 0)
	{
		if (UID_2_TYPE(uMurderID) == TYPE_PLAYER_ROLE)
		{
			num = 0;
			uidArray[num++] = uMurderID;
		}
	}
	else
	{
		int bHaveFound = false;
		//检查是否杀手在uid列表中 不在则添加进列表
		for (int i = 0; i < num; ++i)
		{
			if (uidArray[i] == INVALID_UID || UID_2_TYPE(uidArray[i]) != TYPE_PLAYER_ROLE)
			{
				continue;
			}

			if (uMurderID == uidArray[i])
			{
				bHaveFound = true;
				break;
			}
		}

		if (!bHaveFound)
		{
			if (UID_2_TYPE(uMurderID) == TYPE_PLAYER_ROLE)
			{
				if (MAX_WAR_NEAREST_COUNT > num)
				{
					uidArray[num++] = uMurderID;
				}
			}
		}
	}

	if (num <= 0)
	{
		return;
	}

	int nAverageAddValue = (int)(nAdd / num);
	nAverageAddValue = nAverageAddValue>0 ? nAverageAddValue : 1;

	for (int i = 0; i < num; ++i)
	{
		if (uidArray[i] == INVALID_UID || UID_2_TYPE(uidArray[i]) != TYPE_PLAYER_ROLE)
		{
			continue;
		}

		ActorHelper actorHelper(uidArray[i]);
		IActorService *pActorService = actorHelper.m_ptr;
		if (pActorService == NULL)
		{
			continue;
		}
		// 添加buff
		if (nAdd > 0)
		{
			AddBuff(uidArray[i], nAdd, nLevel, uidArray[i]);
		}
	}
}

void CWarDrop::addPersonCampBuffValue(UID uMurderID, int nMurderCamp, int nAdd, int nLevel)
{
	if (nAdd <= 0)
		return;

	// 给己方阵营添加buff
	SWarCamp* pWarCamp = m_pWar->getWarCamp(nMurderCamp);
	if (pWarCamp == NULL)
	{
		return;
	}

	WarPersonVector::iterator itPerson = pWarCamp->warPersVect.begin();
	for (; itPerson != pWarCamp->warPersVect.end(); ++itPerson)
	{
		ActorHelper actorHelper(itPerson->uPDBID, ACTOR_DBID);
		IActorService *pActorService = actorHelper.m_ptr;
		if (pActorService == NULL)
		{
			if (!itPerson->bIsComputer)
			{
				ErrorLn(__FUNCTION__": pActorService == NULL PDBID = " << itPerson->uPDBID << " UID = " << itPerson->uID);
			}
			continue;
		}

		// 添加buff
		AddBuff(itPerson->uID, nAdd, nLevel, itPerson->uID);
	}
}

void CWarDrop::dealPersonDie(sEntityDieInfo entityDieInfo,bool bBcastTips)
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

	const SchemeWarPersonConfigInfo *pPersonConfigInfo = warMiscMgr->getPersonConfigInfo();
	if (pPersonConfigInfo == NULL)
	{
		ErrorLn(__FUNCTION__": pPersonConfigInfo == NULL");
		return;
	}

	Vector3 vec = Vector3(entityDieInfo.x, entityDieInfo.y, entityDieInfo.z);
	int nMurderCamp = 0;
	int nKillOrDeadCount = 0;
	EWarDropType enmDropType = EWDT_None;
	msg_war_important_entiy_dieinfo msgEntiyDieInfo;
	msgEntiyDieInfo.MasterEntityType = TYPE_PLAYER_ROLE;
	msgEntiyDieInfo.uidMaster = entityDieInfo.uDiedUID;
	msgEntiyDieInfo.uidMurderer = entityDieInfo.uMurderUID;
	msg_war_entity_kill_countinfo EntityKillCountInfo;
	EntityKillCountInfo.MasterEntityType = TYPE_PLAYER_ROLE;
	EntityKillCountInfo.uidMaster = entityDieInfo.uDiedUID;
	EntityKillCountInfo.uidMurderer = entityDieInfo.uMurderUID;
	int nAssistsCount = 0;
	msgEntiyDieInfo.bIsOver = false;
	int nPlayerCamp = entityDieInfo.nDiedCamp;
	vector<msg_war_entity_assist_info> assistList;	// 助攻者列表

													// 死亡者
	SWPerson* pDiedPerson = m_pWar->getPlayerInfo(entityDieInfo.uDiedUID);
	if (pDiedPerson == NULL)
	{
		ErrorLn(__FUNCTION__": pDiedPerson == NULL uDiedUID =" << entityDieInfo.uDiedUID);
		return;
	}
	// 判断被怪物击杀
	if (UID_2_TYPE(entityDieInfo.uMurderUID) == TYPE_MONSTER)
	{
		SWarCamp* pWarCamp = m_pWar->getWarCamp(entityDieInfo.nDiedCamp);
		if (pWarCamp == NULL)
		{
			ErrorLn(__FUNCTION__": camp not found entityDieInfo.nDiedCamp =" << entityDieInfo.nDiedCamp);
			return;
		}
		// 死亡者信息是否有英雄归属
		if (isValidUID(pDiedPerson->uLastAttackUID))
		{
			ActorHelper actorHelper(pDiedPerson->uLastAttackUID);
			IActorService * pActor = actorHelper.m_ptr;
			if (pActor != NULL)
			{
				// 更改为击杀者为最后攻击自己的英雄
				entityDieInfo.uMurderUID = pDiedPerson->uLastAttackUID;

				//其他相关信息也得更新
				entityDieInfo.uMurderPropertyID = pActor->getProperty_Integer(PROPERTY_ID);                     // 凶手的怪物ID或者人物PDBID
				entityDieInfo.nMurderCamp = pActor->getProperty_Integer(PROPERTY_CAMP);                   // 死亡玩家的阵营
				entityDieInfo.nMurderVocation = pActor->getProperty_Integer(PROPERTY_VOCATION);               // 击杀者职业
				sstrcpyn(entityDieInfo.szMurderName, pActor->getName().data(), sizeof(entityDieInfo.szMurderName));
				msgEntiyDieInfo.uidMurderer = entityDieInfo.uMurderUID;
				EntityKillCountInfo.uidMurderer = entityDieInfo.uMurderUID;
			}
		}
		else
		{

			pDiedPerson->lifeState = ELS_Dead;
			pDiedPerson->nDeadNum = pDiedPerson->nDeadNum + 1;

			// 处理助攻记录
			dealWithAssistsInfo(pDiedPerson, NULL, entityDieInfo.uMurderUID, assistList);
			nAssistsCount = assistList.size();
			EntityKillCountInfo.nAssistCount = nAssistsCount;
			msgEntiyDieInfo.nAssistCount = nAssistsCount;

			msgEntiyDieInfo.MurdererEntityType = TYPE_MONSTER;
			msgEntiyDieInfo.nCamp = entityDieInfo.nMurderCamp;
			msgEntiyDieInfo.isMonsterKill = true;
			msgEntiyDieInfo.MurdererHeroID = entityDieInfo.uMurderPropertyID;
			msgEntiyDieInfo.MasterHeroID = entityDieInfo.nDiedVocation;
			sstrcpyn(msgEntiyDieInfo.MurdererName, entityDieInfo.szMurderName, sizeof(msgEntiyDieInfo.MurdererName));
			sstrcpyn(msgEntiyDieInfo.MasterName, entityDieInfo.szDiedName, sizeof(msgEntiyDieInfo.MasterName));

			EntityKillCountInfo.MurdererEntityType = TYPE_MONSTER;
			EntityKillCountInfo.isMonsterKill = true;
			EntityKillCountInfo.MurdererCamp = entityDieInfo.nMurderCamp;
			EntityKillCountInfo.MasterHeroID = entityDieInfo.nDiedVocation;
			EntityKillCountInfo.MurdererHeroID = entityDieInfo.uMurderPropertyID;
			sstrcpyn(EntityKillCountInfo.MasterName, entityDieInfo.szDiedName, sizeof(EntityKillCountInfo.MasterName));
		}

	}

	// 判断被玩家击杀
	if (UID_2_TYPE(entityDieInfo.uMurderUID) == TYPE_PLAYER_ROLE)
	{
		pDiedPerson->lifeState = ELS_Dead;
		pDiedPerson->nDeadNum++;		// 死亡数量
		pDiedPerson->nDeadCount++;		// 连死数量

		if (pDiedPerson->nDeadCount >= 2)
		{
			//掉落衰减
			enmDropType = EWDT_Damping;
			nKillOrDeadCount = pDiedPerson->nDeadCount;
		}
		else if (pDiedPerson->nDeadContKill >= 2)
		{
			//掉落增益
			enmDropType = EWDT_Gain;
			nKillOrDeadCount = pDiedPerson->nDeadContKill;
			EntityKillCountInfo.MasterDieKillCount = pDiedPerson->nDeadContKill;
			msgEntiyDieInfo.MasterDieKillCount = pDiedPerson->nDeadContKill;
		}

		// 设置整场战斗的连杀最大值
		setKillMaxInfo(&(*pDiedPerson));

		pDiedPerson->deadContKillList.push_back(pDiedPerson->nDeadContKill);  // 记录连杀
		if (pDiedPerson->nDeadContKill >= 3)
		{
			msgEntiyDieInfo.bIsOver = true;

		}
		pDiedPerson->nDeadContKill = 0;	                                // 死亡连杀数量清0
		pDiedPerson->timeContKillList.push_back(pDiedPerson->nTimeContKill);  // 记录连杀
		pDiedPerson->nTimeContKill = 0;	                                // 战斗连杀数量清0
		pDiedPerson->nAliveContAssist = 0;									// 不死连助攻清零

																			// 击杀者
		SWPerson* pMurderPerson = m_pWar->getPlayerInfo(entityDieInfo.uMurderUID);
		if (pMurderPerson == NULL)
		{
			ErrorLn(__FUNCTION__": pMurderPerson == NULL uMurderUID =" << entityDieInfo.uMurderUID);
			return;
		}


		pMurderPerson->nBaseRecord[EDT_KillCount]++;

		pMurderPerson->nDeadCount = 0;	// 连死清空
		//pMurderPerson->deadContKillList.push_back(pMurderPerson->nDeadCount);//压入

		pMurderPerson->nDeadContKill++;	// 连杀数量				
		pMurderPerson->nTimeContKill++;	// 战斗连杀

										// 战斗中连杀定时器处理		双杀以上才有
		pMurderPerson->dwTimeContKilltick = gServerGlobal->getTimeSyncService()->GetTick();
		if (pMurderPerson->nTimeContKill >= 2)
		{
			EntityKillCountInfo.nTimeKillCount = pMurderPerson->nTimeContKill;
		}

		if (pMurderPerson->nTimeContKill >= 1)
		{
			sendToClientShowKillIcon(entityDieInfo.uMurderUID, pMurderPerson->nTimeContKill);
		}

		if (pMurderPerson->nDeadContKill > 2)
		{
			// 死亡连杀：三杀以上才有
			msgEntiyDieInfo.nKillCount = pMurderPerson->nDeadContKill;
		}

		sstrcpyn(EntityKillCountInfo.MurdererName, entityDieInfo.szMurderName, sizeof(EntityKillCountInfo.MurdererName));

		//总体击杀数量
		SWarCamp* pWarCamp = m_pWar->getWarCamp(nPlayerCamp);
		if (pWarCamp != NULL)
		{
			pWarCamp->nTotalKillNum++;
		}

		msgEntiyDieInfo.MurdererEntityType = TYPE_PLAYER_ROLE;
		msgEntiyDieInfo.nCamp = entityDieInfo.nMurderCamp;
		msgEntiyDieInfo.MurdererHeroID = entityDieInfo.nMurderVocation;
		msgEntiyDieInfo.MasterHeroID = entityDieInfo.nDiedVocation;
		sstrcpyn(msgEntiyDieInfo.MurdererName, entityDieInfo.szMurderName, sizeof(msgEntiyDieInfo.MurdererName));
		sstrcpyn(msgEntiyDieInfo.MasterName, entityDieInfo.szDiedName, sizeof(msgEntiyDieInfo.MasterName));

		EntityKillCountInfo.MurdererEntityType = TYPE_PLAYER_ROLE;
		EntityKillCountInfo.MurdererCamp = entityDieInfo.nMurderCamp;
		EntityKillCountInfo.MasterHeroID = entityDieInfo.nDiedVocation;
		EntityKillCountInfo.MurdererHeroID = entityDieInfo.nMurderVocation;
		sstrcpyn(EntityKillCountInfo.MasterName, entityDieInfo.szDiedName, sizeof(EntityKillCountInfo.MasterName));

		// 记录一血信息
		if (m_bFirstBlood == false)
		{
			m_bFirstBlood = true;
			// 广播首杀
			msgEntiyDieInfo.bIsFirstBlood = true;

			pMurderPerson->bFirstBlood = true;
		}
		else
		{
			msgEntiyDieInfo.bIsFirstBlood = false;
		}

		// 处理助攻信息
		dealWithAssistsInfo(pDiedPerson, pMurderPerson, entityDieInfo.uMurderUID, assistList);
		nAssistsCount = assistList.size();
		EntityKillCountInfo.nAssistCount = nAssistsCount;
		msgEntiyDieInfo.nAssistCount = nAssistsCount;
	}

	sendKillBelongsEvent(entityDieInfo.uMurderUID, entityDieInfo.uDiedUID);

	// 死亡者助攻增益buff列表清空
	pDiedPerson->AssistBuffVect.clear();

	//死亡周围玩家判定（除去助攻玩家和击杀玩家 还有怪物）
	int nAroundCount = 0;
	SWarCamp* pWarCamp = m_pWar->getWarCamp(nMurderCamp);
	if (pWarCamp != NULL)
	{
		WarPersonVector::iterator itPerson = pWarCamp->warPersVect.begin();
		for (; itPerson != pWarCamp->warPersVect.end(); ++itPerson)
		{
			// 击杀者不记录
			if (itPerson->uID == entityDieInfo.uMurderUID)
			{
				continue;
			}

			// 有助攻不记录
			if (pDiedPerson->GetDamageVect.find(itPerson->uID) != pDiedPerson->GetDamageVect.end())
			{
				if (pDiedPerson->GetDamageVect.find(itPerson->uID)->second > 0)
				{
					continue;
				}
			}

			ActorHelper actorHelper(itPerson->uID);
			IActorService *pActorService = actorHelper.m_ptr;
			if (pActorService == NULL)
			{
				continue;
			}

			// 死亡者周围的敌方玩家
			float fActorWithDeathDistance = vec.getDistance(pActorService->getLocation());
			if (fActorWithDeathDistance <  pPersonConfigInfo->nAddAroundEXPDistance)
			{
				pDiedPerson->DeadPersonAroundEnemy[itPerson->uID] = true;
				nAroundCount++;
			}
		}
	}

	int nShareEXPCount = nAssistsCount;
	if (UID_2_TYPE(entityDieInfo.uMurderUID) == TYPE_PLAYER_ROLE)
	{
		nShareEXPCount++;
	}

	nShareEXPCount += nAroundCount;
	//击杀者经验处理
	if (UID_2_TYPE(entityDieInfo.uMurderUID) == TYPE_PLAYER_ROLE)
	{
		int nAddTalentPoint = calculatePersonAddExpAndTalentPoint(EWGP_Kill, entityDieInfo.uDiedUID, entityDieInfo.uMurderUID, nShareEXPCount, enmDropType, nKillOrDeadCount, msgEntiyDieInfo.bIsFirstBlood);

		int	nType = pWarMonsterMgr->getWarMonsterTypeByUID(entityDieInfo.uDiedUID);

		if (msgEntiyDieInfo.bIsFirstBlood || msgEntiyDieInfo.bIsOver || nType == EWME_BOSS)
			msgEntiyDieInfo.nAddTalentPoint = nAddTalentPoint;
	}

	// 处理助攻经验天赋点
	dealWithAssistsEXPAndTalentPoint(pDiedPerson, entityDieInfo.uMurderUID, nShareEXPCount, enmDropType, nKillOrDeadCount, msgEntiyDieInfo.bIsFirstBlood);

	// 处理未造成伤害周围添加天赋点和经验值
	dealWithAroundEXPAndTalentPoint(pDiedPerson, entityDieInfo.uMurderUID, nShareEXPCount, enmDropType, nKillOrDeadCount, msgEntiyDieInfo.bIsFirstBlood);

	// 将是否团灭加入进去
	if(m_pMiscMgr != NULL)
	{
		msgEntiyDieInfo.isAce = m_pMiscMgr->judgeCampIsAce(nPlayerCamp);
		EntityKillCountInfo.bIsAce = msgEntiyDieInfo.isAce;
	}

	if (bBcastTips)
	{
		if (EntityKillCountInfo.nTimeKillCount > 0)
		{
			// 战斗中连杀信息
			warMiscMgr->sendToClientKillCountInfo(EntityKillCountInfo, assistList);
		}
		else
		{
			// 击杀、死亡连杀信息
			warMiscMgr->sendToClientImportantEntityDie(msgEntiyDieInfo, assistList);
		}
	}
}


void CWarDrop::sendKillBelongsEvent(UID uMurderId, UID uDeadId)
{
	// 只有英雄能接受
	SceneLogicEventHelper eventHelper(m_pWar->getWarSceneID());
	rkt::IEventEngine* pEventEngine = eventHelper.m_ptr;
	if (pEventEngine == NULL)
	{
		return;
	}

	event_kill_belongs eventData;
	eventData.uMurderId = uMurderId;
	eventData.uDeadId = uDeadId;

	// 触发击杀归属事件
	pEventEngine->FireExecute(EVENT_GAME_ACTOR_KILL_BELONGS, UID_2_TYPE(uDeadId), uDeadId, (LPCSTR)&eventData, sizeof(eventData));
}

void CWarDrop::sendToClientShowKillIcon(UID uMurderId, int nKillCount)
{
	if (nKillCount == 0 || uMurderId == 0)
	{
		WarningLn(__FUNCTION__ ": uMurderId = " << uMurderId << ", nKillCount = " << nKillCount);
		return;
	}
	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.byKeyModule = MSG_MODULEID_ENTITY;
	head.byKeyAction = SC_MSG_WAR_COMBA_KILL;

	SMsgEntityHead entityHead;
	entityHead.uidMaster = uMurderId;
	entityHead.byPartID = PART_WAR;

	obuf obuffData;
	obuffData << head << entityHead;
	int nSkillNum = nKillCount;
	obuffData.push_back(&nSkillNum, sizeof(nSkillNum));
	send2Client(uMurderId, PACKAGE_PTR(new std::string(obuffData.data(), obuffData.size())));
}

void CWarDrop::sendAssistEvent(UID uId, UID uDeadId, UID uKillId, int nAssistCount)
{
	// 获得事件中心接口
	SceneLogicEventHelper eventHelper(m_pWar->getWarSceneID());
	IEventEngine *pEventEngine = eventHelper.m_ptr;
	if (pEventEngine == NULL)
	{
		return;
	}

	// 实体助攻事件
	event_assist_add assistEvent;
	assistEvent.uUid = uId;
	assistEvent.uDeadId = uDeadId;
	assistEvent.uKillId = uKillId;
	assistEvent.nAssistCount = nAssistCount;

	// 触发角色助攻数改变事件
	pEventEngine->FireExecute(EVENT_GAME_ACTOR_ASSIST_CHANGE, UID_2_TYPE(uId), uId, (LPCSTR)&assistEvent, sizeof(assistEvent));
}

void CWarDrop::setKillMaxInfo(SWPerson* pPerson)
{
	// 记录整场战斗的最高值
	int nKill = pPerson->nDeadKill;
	// 当前的击杀值
	int nContKill = pPerson->nDeadContKill;

	pPerson->nDeadKill = nKill < nContKill ? nContKill : nKill;

	nKill = pPerson->nTimeKill;
	nContKill = pPerson->nTimeContKill;
	pPerson->nTimeKill = nKill < nContKill ? nContKill : nKill;


	pPerson->nAliveContAssistMax = max(pPerson->nAliveContAssistMax, pPerson->nAliveContAssist);
}