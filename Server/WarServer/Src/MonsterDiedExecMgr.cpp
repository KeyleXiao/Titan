#include "StdAfx.h"
#include "MonsterDiedExecMgr.h"
#include "SchemeWarDef.h"
#include "IWar.h"
#include "IWarDropMgr.h"
#include "IWarMonsterMgr.h"

CMonsterDiedExecMgr::CMonsterDiedExecMgr(IWar * pWar)
	:m_pWar(pWar)
	,m_nWarType(0)
{
}

CMonsterDiedExecMgr::~CMonsterDiedExecMgr()
{
}

bool CMonsterDiedExecMgr::Init(int nMatchType)
{
	if (m_pWar == NULL)
		return false;

	m_nWarType = m_pWar->getWarTypeId();

	return true;
}

void CMonsterDiedExecMgr::Release()
{
	m_pWar = NULL;

	delete this;
}

void CMonsterDiedExecMgr::onWarRealStart()
{
}

void CMonsterDiedExecMgr::onWarEnd()
{
}

void CMonsterDiedExecMgr::onMonsterDied(sEntityDieInfo * pEntityDieInfo)
{
	// 获得配置信息
	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return;
	}

	ISchemeWarPoint * pSchemeWarPoint = pSchemeCenter->getSchemeWarPoint();
	if (pSchemeWarPoint == NULL)
	{
		return;
	}

	IWarMiscManager* pWarMiscMgr = m_pWar->getMiscManager();
	if (pWarMiscMgr == NULL)
	{
		WarningLn(__FUNCTION__":pWarMiscMgr == NULL");
	}

	IWarMonsterMgr* pWarMonsterMgr = (IWarMonsterMgr*)m_pWar->getWarMgr(Mgr_Monster);
	if (pWarMonsterMgr == NULL)
	{
		ErrorLn(__FUNCTION__":pWarMonsterMgr == NULL");
		return;
	}

	CreateMonsterInfo* pMonsterInfo = pWarMonsterMgr->getWarMonsterInfoByUID(pEntityDieInfo->uDiedUID);
	if (pMonsterInfo == NULL)
	{
		ErrorLn(__FUNCTION__":pMonsterInfo == NULL");
		return;
	}

	IWarDropMgr* pWarDropMgr = (IWarDropMgr*)m_pWar->getWarMgr(Mgr_Drop);
	if (pWarDropMgr == NULL)
	{
		WarningLn(__FUNCTION__":pWarDropMgr == NULL");
	}

	//增加天赋点
	Vector3 vec = Vector3(pEntityDieInfo->x, pEntityDieInfo->y, pEntityDieInfo->z);

	WarPointVect warPointVector = pSchemeWarPoint->GetWarPointByIndex(pMonsterInfo->nRefreshIndex);
	for (WarPointVect::iterator itPoint = warPointVector.begin(); itPoint != warPointVector.end(); ++itPoint)
	{
		//WarPoint.csv nLocalIndex 唯一
		if (itPoint->nLocalIndex == pMonsterInfo->nLocalIndex)
		{
			EExpDropType expDropType = EEXPDROPTYPE_MONSTER;
			ETalentDropType talentDropType = ETALENTDROPTYPE_MONSTER;

			// 怪物掉落
			if (pWarDropMgr != NULL)
			{
				pWarDropMgr->addHeroEXP(itPoint->nAddEXPType, pEntityDieInfo->uMurderUID, pEntityDieInfo->nMurderCamp, vec, itPoint->nDistanceValue, itPoint->nExp, expDropType);

				pWarDropMgr->addPersonTalentValue(pEntityDieInfo->uMurderUID, pEntityDieInfo->uMurderUID, pEntityDieInfo->nMurderCamp, itPoint->nTalentType, vec, itPoint->nDistanceTalent, pEntityDieInfo->uDiedUID,
					itPoint->nExp, itPoint->nTalentValue, itPoint->fDecayFactor, talentDropType);

				pWarDropMgr->addHeroBuffValue(itPoint->nBuffType, pEntityDieInfo->uMurderUID, pEntityDieInfo->nMurderCamp, vec, itPoint->nDistanceValue, itPoint->nBuffValue, itPoint->nBuffLevel);
			}

			//死亡图标广播
			for (int i = 0; i < MAX_WAR_BROADCAST_TYPE_COUNT; ++i)
			{
				if (pEntityDieInfo->uDiedUID == pEntityDieInfo->uMurderUID)
				{
					//自杀广播
					if (((itPoint->BroadCastTypeList[i].eBroadCastType & EWBCT_KillSelf) == EWBCT_KillSelf))
					{
						msg_war_important_monster_dietxtinfo MonsterDieTextInfo;
						MonsterDieTextInfo.MasterEntityType = TYPE_MONSTER;
						MonsterDieTextInfo.MasterHeroID = itPoint->nMonsterID;
						MonsterDieTextInfo.nCamp = pEntityDieInfo->nDiedCamp;
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
							pWarMiscMgr->onEntityDieBroadCast(*pEntityDieInfo, itPoint->BroadCastTypeList[i].nIndex);
						}
					}

					//死亡文字广播
					if ((itPoint->BroadCastTypeList[i].eBroadCastType & EWBCT_DieText) == EWBCT_DieText)
					{
						msg_war_important_monster_dietxtinfo MonsterDieTextInfo;
						MonsterDieTextInfo.MasterEntityType = TYPE_MONSTER;
						MonsterDieTextInfo.MasterHeroID = itPoint->nMonsterID;
						MonsterDieTextInfo.nCamp = pEntityDieInfo->nDiedCamp;
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
}
