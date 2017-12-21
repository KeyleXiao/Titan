/*******************************************************************
** 文件名: LvExpControl.cpp
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/3/24
** 版  本: 1.0
** 描  述: 等级经验控制
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/

#include "StdAfx.h"
#include "LvExpControl.h"
#include "IWar.h"
#include "WarDef_Internal.h"

CLvExpControl::CLvExpControl(IWar* pWar)
	:m_pWar(pWar)
{

}

CLvExpControl::~CLvExpControl()
{
	m_pWar = NULL;
}


bool CLvExpControl::Init(int nMatchType)
{
	if (m_pWar == NULL)
		return false;

	return true;
}

void CLvExpControl::Release()
{

	delete this;
}

void CLvExpControl::onWarRealStart()
{
}

void CLvExpControl::onWarEnd()
{
}

// 1.野区野怪/2.线上小兵 获得经验 = 死亡者标准经验 * 分享者系数 * 等级差控制系数
// 3.击杀英雄 获得经验 = 死亡者标准经验 * 分享者系数 * 等级差控制系数 * （1 - 连续死亡衰减）
void CLvExpControl::controlLvExp(EExpDropType expDropType, int nPlayerLv, int nCamp, Vector3 vLoc, int nDistance, int& nAdd, bool bIsUseShareCoeff/* = false */)
{
	if (m_pWar == NULL)
		return;

#ifdef PRINTF_PLANLOG
	WarningLn(__FUNCTION__"Planning expDropType = " << expDropType << ", nPlayerLv = " << nPlayerLv);
	WarningLn(__FUNCTION__"Planning former nAdd = " << nAdd);
#endif
	// 获得配置信息
	ISchemeWarPersonConfigInfo * pSchemeConfig = gServerGlobal->getSchemeCenter()->getSchemeWarPersonConfigInfo();
	if (pSchemeConfig == NULL)
	{
		return;
	}
	const SchemeWarPersonConfigInfo* pSchemeWarPersonConfigInfo = pSchemeConfig->GetSchemeWarPersonConfigInfo(m_pWar->getStaticID());
	if (pSchemeWarPersonConfigInfo == NULL)
	{
		return;
	}
	float fShareCoeff = bIsUseShareCoeff ? getShareCoeff(nCamp, vLoc, nDistance) : 1;

	switch (expDropType)
	{
	case EEXPDROPTYPE_MONSTER:
	{
		int nLevelGrap = getAllHeroMeanLv() - nPlayerLv;
		int nIncreaseCoeff = pSchemeWarPersonConfigInfo->nIncreaseMonster;
		int nDecreaseCoeff = pSchemeWarPersonConfigInfo->nDecreaseMonster;
		int nIncreaseMax = pSchemeWarPersonConfigInfo->nIncreaseMonsterMax;
		int nDescreaseMin = pSchemeWarPersonConfigInfo->nDecreaseMonsterMin;
		float fLevelGapControlCoeff = getHeroOrMonsterLevelGapControlCoeff(nLevelGrap, nIncreaseCoeff, nDecreaseCoeff);
		fLevelGapControlCoeff = fLevelGapControlCoeff > ((float)nIncreaseMax) / 100.f ? ((float)nIncreaseMax) / 100.f : fLevelGapControlCoeff;
		fLevelGapControlCoeff = fLevelGapControlCoeff < ((float)nDescreaseMin) / 100.f ? ((float)nDescreaseMin) / 100.f : fLevelGapControlCoeff;

#ifdef PRINTF_PLANLOG
		WarningLn(__FUNCTION__"Planning nLevelGrap = " << nLevelGrap << ", nIncreaseCoeff = " << nIncreaseCoeff << ", nDecreaseCoeff" << nDecreaseCoeff << ", nIncreaseMax" << nIncreaseMax << ", nDescreaseMin" << nDescreaseMin);
		WarningLn(__FUNCTION__"Planning fLevelGapControlCoeff = " << fLevelGapControlCoeff << ", fShareCoeff = " << fShareCoeff);
#endif
		// 1.野区野怪/2.线上小兵 获得经验 = 死亡者标准经验 * 分享者系数 * 等级差控制系数
		nAdd = (float)nAdd * fShareCoeff * fLevelGapControlCoeff;
#ifdef PRINTF_PLANLOG
		WarningLn(__FUNCTION__"Planning nAdd = " << nAdd);
#endif
	}
	break;
	case EEXPDROPTYPE_SOLIDER:
	{
		int nEnemyCamp = nCamp == CAMP_BLUE ? CAMP_RED : CAMP_BLUE;
		int nLevelGrap = getCampHeroMeanLv(nEnemyCamp) - nPlayerLv;

		int nIncreaseCoeff = pSchemeWarPersonConfigInfo->nIncreaseSolider;
		int nControlLevel = pSchemeWarPersonConfigInfo->nControlLv;
		int nIncreaseMax = pSchemeWarPersonConfigInfo->nIncreaseSoliderMax;
		float fLevelGapControlCoeff = getSolderLevelGapControlCoeff(nLevelGrap, nIncreaseCoeff, nControlLevel);
		fLevelGapControlCoeff = fLevelGapControlCoeff > ((float)nIncreaseMax) / 100.f ? ((float)nIncreaseMax) / 100.f : fLevelGapControlCoeff;
#ifdef PRINTF_PLANLOG
		WarningLn(__FUNCTION__"Planning nLevelGrap = " << nLevelGrap << ", nIncreaseCoeff = " << nIncreaseCoeff << ", nIncreaseMax" << nIncreaseMax << ", nControlLevel" << nControlLevel);
		WarningLn(__FUNCTION__"Planning fLevelGapControlCoeff = " << fLevelGapControlCoeff << ", fShareCoeff = " << fShareCoeff);
#endif
		// 1.野区野怪/2.线上小兵 获得经验 = 死亡者标准经验 * 分享者系数 * 等级差控制系数
		nAdd = (float)nAdd * fShareCoeff * fLevelGapControlCoeff;
#ifdef PRINTF_PLANLOG
		WarningLn(__FUNCTION__"Planning nAdd = " << nAdd);
#endif
	}
	break;

	default: break;
	}
}

float CLvExpControl::getAllHeroMeanLv()
{
	if (m_pWar == NULL)
		return 0.f;

	int nSumLv = 0;
	int nPlayerCount = 0;

	vector<int> campList = m_pWar->getCampList();
	for (int i = 0; i < campList.size(); ++i)
	{
		const SWarCamp* pWarCamp = m_pWar->getWarCamp(campList[i]);
		if (pWarCamp == NULL)
			continue;

		nPlayerCount += pWarCamp->warPersVect.size();
		WarPersonVector::const_iterator itBeginPerson = pWarCamp->warPersVect.begin();
		for (; itBeginPerson != pWarCamp->warPersVect.end(); ++itBeginPerson)
		{
			nSumLv += itBeginPerson->nLevel;
		}
	}

	nPlayerCount = (nPlayerCount == 0 ? 1 : nPlayerCount);
	return (float)nSumLv / (float)nPlayerCount;
}

float CLvExpControl::getCampHeroMeanLv(int nCamp)
{
	if (m_pWar == NULL)
		return 0.f;

	int nSumLv = 0;
	int nPlayerCount = 0;

	vector<int> campList = m_pWar->getCampList();
	for (int i = 0; i < campList.size(); ++i)
	{
		const SWarCamp* pWarCamp = m_pWar->getWarCamp(campList[i]);
		if (pWarCamp == NULL)
			continue;

		if (pWarCamp->nCampID != nCamp)
			continue;

		nPlayerCount += pWarCamp->warPersVect.size();
		WarPersonVector::const_iterator itBeginPerson = pWarCamp->warPersVect.begin();
		for (; itBeginPerson != pWarCamp->warPersVect.end(); ++itBeginPerson)
		{
			nSumLv += itBeginPerson->nLevel;
		}
	}
	nPlayerCount = (nPlayerCount == 0 ? 1 : nPlayerCount);
	return (float)nSumLv / (float)nPlayerCount;
}

float CLvExpControl::getHeroOrMonsterLevelGapControlCoeff(int nLevelGap, int nIncreaseCoeff, int nDecreaseCoeff)
{
	float fCoeff = 0.0f;
	if (nLevelGap >= 0)
	{
		//当D大于等于0时  等级差控制系数 = 100% + Increase_Monster * D
		fCoeff = 1.0f + (float)nIncreaseCoeff * (float)nLevelGap / 100.0f;

	}
	else
	{
		//当D小于0时 等级差控制系数 = 100% + Decrease_Monster * (D + 1)
		fCoeff = 1.0f + (float)nDecreaseCoeff * ((float)nLevelGap + 1.0f) / 100.0f;
	}
	return fCoeff;
}

float CLvExpControl::getSolderLevelGapControlCoeff(int nLevelGap, int nIncreaseCoeff, int nControlLevel)
{
	float fCoeff = 0.0f;
	if (nLevelGap <= nControlLevel)
	{
		// 当D小于等于ControlLevel时 等级差控制系数 = 100%
		fCoeff = 1.0f;
	}
	else
	{
		// 当D大于ControlLevel时 等级差控制系数 = 100% + （D - ControlLevel）* Increase_Minion
		fCoeff = 1.0f + ((float)nLevelGap - (float)nControlLevel) * (float)nIncreaseCoeff / 100.0f;
	}
	return fCoeff;
}

// 分享者系数 = 1.3的 （2-2/n）次方
float CLvExpControl::getShareCoeff(int nMurderCamp, Vector3 vLoc, int nDistance)
{
	if (m_pWar == NULL)
		return 0.f;

	SWarCamp* pWarCamp = m_pWar->getWarCamp(nMurderCamp);
	if (pWarCamp == NULL)
		return 0.f;

	// 在指定的阵营中，找到玩家的UID，列表
	WarPersonVector::iterator itPerson = pWarCamp->warPersVect.begin();
	int nPersonCount = 0;
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
		if (fActorWithDeathDistance <= nDistance)
		{
			nPersonCount++;
		}
	}
	return pow(1.3f, 2.0f - 2.0f / (float)nPersonCount);
}

void CLvExpControl::controlLvExp(int nPlayerLv, int nDiePlayerLv, int nAroundPlayerCount, int& nAdd)
{
	if (m_pWar == NULL)
		return;

#ifdef PRINTF_PLANLOG
	WarningLn(__FUNCTION__"Planning expDropType = " << EEXPDROPTYPE_PLAYER << ", nPlayerLv = " << nPlayerLv << ",nDiePlayerLv" << nDiePlayerLv);
	WarningLn(__FUNCTION__"Planning former nAdd = " << nAdd);
#endif
	// 获得配置信息
	ISchemeWarPersonConfigInfo * pSchemeConfig = gServerGlobal->getSchemeCenter()->getSchemeWarPersonConfigInfo();
	if (pSchemeConfig == NULL)
	{
		return;
	}
	const SchemeWarPersonConfigInfo* pSchemeWarPersonConfigInfo = pSchemeConfig->GetSchemeWarPersonConfigInfo(m_pWar->getStaticID());
	if (pSchemeWarPersonConfigInfo == NULL)
	{
		return;
	}
	nAroundPlayerCount = (nAroundPlayerCount == 0 ? 1 : nAroundPlayerCount);
	float fShareCoeff = pow(1.3f, 2.0f - 2.0f / (float)nAroundPlayerCount);

	int nLevelGrap = nDiePlayerLv - nPlayerLv;
	int nIncreaseCoeff = pSchemeWarPersonConfigInfo->nIncreaseHero;
	int nDecreaseCoeff = pSchemeWarPersonConfigInfo->nDecreaseHero;
	int nIncreaseMax = pSchemeWarPersonConfigInfo->nIncreaseHeroMax;
	int nDescreaseMin = pSchemeWarPersonConfigInfo->nDecreaseHeroMin;
	float fLevelGapControlCoeff = getHeroOrMonsterLevelGapControlCoeff(nLevelGrap, nIncreaseCoeff, nDecreaseCoeff);
	fLevelGapControlCoeff = fLevelGapControlCoeff > ((float)nIncreaseMax) / 100.f ? ((float)nIncreaseMax) / 100.f : fLevelGapControlCoeff;
	fLevelGapControlCoeff = fLevelGapControlCoeff < ((float)nDescreaseMin) / 100.f ? ((float)nDescreaseMin) / 100.f : fLevelGapControlCoeff;

#ifdef PRINTF_PLANLOG
	WarningLn(__FUNCTION__"Planning nLevelGrap = " << nLevelGrap << ", nIncreaseCoeff = " << nIncreaseCoeff << ", nDecreaseCoeff" << nDecreaseCoeff << ", nIncreaseMax" << nIncreaseMax << ", nDescreaseMin" << nDescreaseMin);
	WarningLn(__FUNCTION__"Planning fLevelGapControlCoeff = " << fLevelGapControlCoeff);
#endif

	// 3.击杀英雄 获得经验 = 死亡者标准经验 * 分享者系数 * 等级差控制系数 * （1 - 连续死亡衰减）
	nAdd = (float)nAdd * fShareCoeff * fLevelGapControlCoeff * (1 - pSchemeWarPersonConfigInfo->fDropDumpingEXPRate);
#ifdef PRINTF_PLANLOG
	WarningLn(__FUNCTION__"Planning nAdd = " << nAdd);
#endif
}