/*******************************************************************
** 文件名: EconomyControl.cpp
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/3/14
** 版  本: 1.0
** 描  述: 真人玩家战场经济控制
** 应  用: 
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "StdAfx.h"
#include "EconomyControl.h"
#include "IWar.h"
#include "WarDef_Internal.h"

CEconomyControl::CEconomyControl(IWar* pWar)
	:m_pWar(pWar)
	, m_bInitEcmCtrlLine(false)
{
	memset(m_EcmCtrlLine, 0, sizeof(m_EcmCtrlLine));
}

CEconomyControl::~CEconomyControl()
{
	m_pWar = NULL;
}

bool CEconomyControl::Init(int nMatchType)
{
	if (m_pWar == NULL)
		return false;

	return true;
}

void CEconomyControl::Release()
{
	delete this;
}

void CEconomyControl::onWarRealStart()
{
}

void CEconomyControl::onWarEnd()
{
}

// 计算经济控制系数
void CEconomyControl::calcEcmControlRatio()
{
	if (m_pWar == NULL)
	{
		return;
	}

	IWarMiscManager* warMiscMgr = m_pWar->getMiscManager();
	if (warMiscMgr == NULL)
	{
		ErrorLn(__FUNCTION__":warMiscMgr == NULL");
		return;
	}

	// 检查经济控制线是否已经初始化
	if (!m_bInitEcmCtrlLine)
	{
#ifdef __ECM_CTRL_DEBUG_PRINT
		TraceLn(__FUNCTION__ << _GT("经济控制线还没有初始化"));
#endif
		return;
	}

	// 战场有两个阵营
	const int nCampNum = 2;
	const CAMP_TYPE nCampListList[nCampNum] = { CAMP_BLUE, CAMP_RED };

	// 计算每个阵营的平均经济
	map<CAMP_TYPE, int> nAvgEmcList;
	for (int i = 0; i < nCampNum; ++i)
	{
		CAMP_TYPE nCamp = nCampListList[i];

		// 计算阵营平均经济
		nAvgEmcList[nCamp] = warMiscMgr->getAvgEmcByCamp(nCamp);
	}

	// 计算每个玩家的系数
	for (int i = 0; i < nCampNum; ++i)
	{
		CAMP_TYPE nCamp = nCampListList[i];

		SWarCamp* pWarCamp = m_pWar->getWarCamp(nCamp);
		if (pWarCamp == NULL)
			continue;

		const WarPersonVector& warPersVect = pWarCamp->warPersVect;

		// 计算对方阵营
		CAMP_TYPE nEnemyCamp = nCampListList[(i + 1) % nCampNum];

		WarPersonVector::const_iterator itPer = warPersVect.begin();
		for (; itPer != warPersVect.end(); ++itPer)
		{
			// 计算个人与敌方平均经济的经济差
			int nDiffEmc = nAvgEmcList[nEnemyCamp] - itPer->nBaseRecord[EDT_Money];

			// 根据与对手的平均经济差，计算经济落后的级别
			if (nDiffEmc <= m_EcmCtrlLine[EWEcmCtrlLine_1])
			{
				m_EcmCtrlList[itPer->uPDBID] = EWEcmDiff_NONE;
			}
			else if (nDiffEmc > m_EcmCtrlLine[EWEcmCtrlLine_1] && nDiffEmc <= m_EcmCtrlLine[EWEcmCtrlLine_2])
			{
				m_EcmCtrlList[itPer->uPDBID] = EWEcmDiff_SMALL;
			}
			else if (nDiffEmc > m_EcmCtrlLine[EWEcmCtrlLine_2] && nDiffEmc <= m_EcmCtrlLine[EWEcmCtrlLine_3])
			{
				m_EcmCtrlList[itPer->uPDBID] = EWEcmDiff_BIG;
			}
			else
			{
				m_EcmCtrlList[itPer->uPDBID] = EWEcmDiff_HUGE;
			}

#ifdef __ECM_CTRL_DEBUG_PRINT
			TraceLn(__FUNCTION__ << _GT(" PDBID=") << itPer->uPDBID << _GT(" 玩家=") << itPer->szName << _GT(" 经济差级别= ") << m_EcmCtrlList[itPer->uPDBID]);
#endif
		}
	}
}

// 计算经济控制线
void CEconomyControl::calcEcmControlLine()
{
	if (m_pWar == NULL)
	{
		return;
	}

	// 战场持续时间钟
	int nPlayTime = m_pWar->getPlayTimeMin();

	//计算此时的经济差状态区间边界线，
	//公式：C1 = a1*T + b1
	//公式：C2 = a2*T + b2
	//公式：Cmax = a3*T + b3
	ISchemeEconomyControl* pEcmCtrl = gServerGlobal->getSchemeCenter()->getSchemeEconomyControl();
	int nConfigWarID = m_pWar->getStaticID();
	for (int i = EWEcmCtrlLine_1; i < EWEcmCtrlLine_MAX; ++i)
	{
		SEconomyControlConfig_S* pEcmCtrlConfig = pEcmCtrl->getEconomyCtrl(nConfigWarID, i);
		if (pEcmCtrlConfig != NULL)
		{
			m_EcmCtrlLine[i] = pEcmCtrlConfig->aRatio * nPlayTime + pEcmCtrlConfig->bRatio;

#ifdef __ECM_CTRL_DEBUG_PRINT
			TraceLn(__FUNCTION__ << _GT(" 时间=") << nPlayTime << _GT(" 经济差线条") << i + 1 << "=" << m_EcmCtrlLine[i]);
#endif
		}
	}

	m_bInitEcmCtrlLine = true;
}

// 获取某个玩家的经济控制系数
float CEconomyControl::getEmcCtrlRatio(PDBID pdbID, EWEcmExpCtrlRatioType nRatioType)
{
	float fRatio = 0.f;
	do
	{
		if (m_pWar == NULL)
		{
			break;
		}

		if (nRatioType <= EWEcmExpCtrlRatioType_None || nRatioType >= EWEcmCtrlRatioType_Max)
		{
			ErrorLn(__FUNCTION__ " Invalid EcmCtrlRatioType, nRatioType=" << nRatioType);
			break;
		}
		std::map<PDBID, int>::iterator iter = m_EcmCtrlList.find(pdbID);
		if (iter == m_EcmCtrlList.end())
		{
			break;
		}
		if (iter->second == EWEcmDiff_NONE)
		{
			break;
		}

		ISchemeEconomyControl* pEcmCtrl = gServerGlobal->getSchemeCenter()->getSchemeEconomyControl();
		if (pEcmCtrl == NULL)
		{
			break;
		}
		// 计算经济控制系数				
		int nConfigWarID = m_pWar->getStaticID();
		SEconomyControlConfig_S* pEcmCtrlConfig = pEcmCtrl->getEconomyCtrl(nConfigWarID, iter->second - 1);
		if (pEcmCtrlConfig != NULL)
		{
			fRatio = pEcmCtrlConfig->nRatioType[nRatioType];
		}
	} while (false);

	return fRatio;
}

// 计算经济控制后获得的天赋点
int CEconomyControl::controlTelentPoint(PDBID pdbID, int nAddValue, ETalentDropType nDropType)
{
	EWEcmExpCtrlRatioType nRatioType = EWEcmExpCtrlRatioType_None;
	switch(nDropType)
	{
	case ETALENTDROPTYPE_PLAYER:	// 玩家增加金钱
		{
			nRatioType = EWEcmExpCtrlRatioType_Hero;
		}
		break;
	case ETALENTDROPTYPE_SOLIDER:	// 小兵增加金钱
		{
			nRatioType = EWEcmExpCtrlRatioType_Sodlier;
		}
		break;
	case ETALENTDROPTYPE_NORMAL:	// 自然增加金钱
		{
			nRatioType = EWEcmExpCtrlRatioType_Salary;
		}
		break;
	case ETALENTDROPTYPE_MONSTER:	// 野怪增加金钱
		{
			nRatioType = EWEcmExpCtrlRatioType_Monster;
		}
		break;
	default:
		{
			return nAddValue;
		}
	}

	// 计算经济控制后的新增天赋点，当玩家处于劣势时，才有意义
	// 公式：新的天赋点 = (1 + 金币控制系数) * 旧的天赋点

	float fRatio = getEmcCtrlRatio(pdbID, nRatioType);

	int nNewAddValue = (1 + fRatio) * nAddValue;

#ifdef __ECM_CTRL_DEBUG_PRINT
	TraceLn(__FUNCTION__ << _GT(" 旧的天赋值=") << nAddValue << _GT(" 新的天赋值=") << nNewAddValue << " fRatio=" << fRatio << " PDBID=" << pdbID << _GT(" 获得天赋值类型=") << nRatioType);
#endif

	return nNewAddValue;
}

// 控制连杀经济
int CEconomyControl::controlContKillTelentPoint( IActorService* pDead, IActorService* pAttacker, int nAddValue, float fDropGainTalentRate, float fContKillEmcCtrlRatio, int nMaxMultiKill, int nContKillCount)
{
	if (m_pWar == NULL)
		return nAddValue;

	IWarMiscManager* warMiscMgr = m_pWar->getMiscManager();
	if (warMiscMgr == NULL)
	{
		ErrorLn(__FUNCTION__":warMiscMgr == NULL");
		return nAddValue;
	}

	SWPerson* pPerson = m_pWar->getPlayerInfo(pDead->getUID());
	if (pPerson == NULL)
		return nAddValue;

	// 经济差D = 被击杀的敌方英雄经济M - 击杀方阵营英雄的平均经济L
	// 新的天赋值 = （1 + 经济控制差系数F）* (标准天赋值 * (1 + 连杀系数 + 连杀次数) + 经济控制差系数G * 经济差D)

	int nDeadTalentPoint = pPerson->nBaseRecord[EDT_Money];
	// 攻击方阵营平均经济
	int nAttackerCamp = pAttacker->getProperty_Integer(PROPERTY_CAMP);
	int nAttackerCampAvgEcm = warMiscMgr->getAvgEmcByCamp(nAttackerCamp);
	// 计算经济差D,当D小于0时,取0
	int nEmcDiff = nDeadTalentPoint - nAttackerCampAvgEcm;
	nEmcDiff = max(0, nEmcDiff);

	nContKillCount = min(nMaxMultiKill, nContKillCount);
	
	PDBID attackerPDBID = pAttacker->getProperty_Integer(PROPERTY_ID);
	float fEmcRatio = getEmcCtrlRatio(attackerPDBID, EWEcmExpCtrlRatioType_Hero);

	int nNewTalentPoint = int((1 + fEmcRatio) * (nAddValue * (1 + fDropGainTalentRate * nContKillCount) + fContKillEmcCtrlRatio * nEmcDiff));

#ifdef __WAR__HERO_GET_EXP__TALENT__DEBUG__
	TraceLn(__FUNCTION__ << _GT(" 新的天赋值=") << nNewTalentPoint << _GT(" 经济控制差F=") << fEmcRatio << _GT(" 标准天赋值=") << nAddValue
		<< _GT(" 连杀系数=") << fDropGainTalentRate << _GT(" 连杀次数=") << nContKillCount << _GT(" 经济控制差系数G=") << fContKillEmcCtrlRatio
		<< _GT(" 经济差D=") << nEmcDiff << _GT(" 被杀者经济=") << nDeadTalentPoint << _GT(" 攻击方平均经济=") << nAttackerCampAvgEcm
		<< _GT(" 被击杀者名字=") << pDead->getName() << _GT(" 攻击者名字=") << pAttacker->getName());
#endif
	
	return nNewTalentPoint;
}
