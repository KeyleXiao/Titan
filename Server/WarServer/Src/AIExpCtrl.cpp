/*******************************************************************
** 文件名: AIEcmExpCtrl.cpp
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/10/28
** 版  本: 1.0
** 描  述: AI玩家战场经济控制
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/

#include "StdAfx.h"
#include "AIExpCtrl.h"
#include "IWar.h"
#include "WarDef_Internal.h"

//#define __AI_ECM_CTRL_DEBUG_PRINT

CAIExpCtrl::CAIExpCtrl(IWar* pWar)
	: m_pWar(pWar)
	, m_bInitAIExpCtrlLine(false)
	, m_bInitAvgMatchScore(false)
{
}

CAIExpCtrl::~CAIExpCtrl()
{
	m_pWar = NULL;
}

bool CAIExpCtrl::Init(int nMatchType)
{
	if (m_pWar == NULL)
		return false;

	m_nMatchType = nMatchType;

	return true;
}

void CAIExpCtrl::Release()
{
	delete this;
}

void CAIExpCtrl::onWarRealStart()
{
}

void CAIExpCtrl::onWarEnd()
{
}

void CAIExpCtrl::initAvgMatchScore()
{
	IWarMiscManager* warMiscMgr = m_pWar->getMiscManager();
	if (warMiscMgr == NULL)
	{
		ErrorLn(__FUNCTION__":warMiscMgr == NULL");
		return;
	}

	vector<int> campLsit = m_pWar->getCampList();
	for (int i = 0; i < campLsit.size(); ++i)
	{
		int nCamp = campLsit[i];

		int nEnemyCamp = warMiscMgr->getEnemyCamp(nCamp);
		SWarCamp* pEnemyCamp = m_pWar->getWarCamp(nEnemyCamp);
		if (pEnemyCamp == NULL)
		{
			ErrorLn(__FUNCTION__":getWarCamp == NULL, nEnemyCamp=" << nEnemyCamp);
			continue;
		}

		// 敌对阵营真人玩家为0，则取自己阵营的真人数据
		if (pEnemyCamp->nRealPlayerCount == 0)
		{
			m_mapMatchScore[nCamp] = warMiscMgr->getCampMatchTypeAverageScore(nCamp, true);
		}
		else
		{
			m_mapMatchScore[nCamp] = warMiscMgr->getCampMatchTypeAverageScore(nEnemyCamp, true);
		}

#ifdef __AI_ECM_CTRL_DEBUG_PRINT
		TraceLn(__FUNCTION__ << _GT("AI经验控制, 匹配分：") << m_mapMatchScore[iter->first] << _GT(" 阵营=") << iter->first);
#endif
	}
	m_bInitAvgMatchScore = true;
}

// 计算经济控制线
void CAIExpCtrl::calcAIExpCtrlLine()
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
	ISchemeAIEcmExpCtrl* pEcmExpCtrl = gServerGlobal->getSchemeCenter()->getSchemeAIEcmExpCtrl();
	int nConfigWarID = m_pWar->getStaticID();
	vector<int> campLsit = m_pWar->getCampList();
	for (int i = 0; i < campLsit.size(); ++i)
	{
		int nCamp = campLsit[i];
		for (int i = EWAIExpCtrlLine_1; i < EWAIExpCtrlLine_MAX; ++i)
		{
			SAIExpCtrlConfig_S* pExpCtrlConfig = pEcmExpCtrl->getAIExpCtrl(nConfigWarID, m_mapMatchScore[nCamp], i);
			if (pExpCtrlConfig != NULL)
			{
				m_AIExpCtrlLine[nCamp][i] = pExpCtrlConfig->aRatio * nPlayTime + pExpCtrlConfig->bRatio;

#ifdef __AI_ECM_CTRL_DEBUG_PRINT
				TraceLn(__FUNCTION__ << _GT("AI经验控制,AI经验： 时间=") << nPlayTime << _GT(" 经验差线条") << i + 1 << "=" << m_AIExpCtrlLine[nCamp][i] << _GT(" 阵营=") << nCamp);
#endif
			}
		}
	}
	m_bInitAIExpCtrlLine = true;
}


int CAIExpCtrl::getMatchScoreForCalc(PDBID pdbid)
{
	// 检查平均分是否已经初始化
	if (!m_bInitAvgMatchScore)
	{
#ifdef __AI_ECM_CTRL_DEBUG_PRINT
		TraceLn(__FUNCTION__ << _GT("AI经济控制,平均分还没初始化"));
#endif
		return 0;
	}

	int nAvgMatchScore = 0;
	SWPerson* pPerson = m_pWar->getPlayerInfoByPDBIU(pdbid);
	if (pPerson != NULL)
	{
		nAvgMatchScore = m_mapMatchScore[pPerson->nCamp];
	}

	return nAvgMatchScore;
}

void CAIExpCtrl::calcAIExpCtrlRatio()
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

	// 检查经验控制线是否已经初始化
	if (!m_bInitAIExpCtrlLine)
	{
#ifdef __AI_ECM_CTRL_DEBUG_PRINT
		TraceLn(__FUNCTION__ << _GT("AI经验控制,AI经验：AI经验控制线还没有初始化"));
#endif
		return;
	}

	// 战场有两个阵营
	const int nCampNum = 2;
	const CAMP_TYPE nCampListList[nCampNum] = { CAMP_BLUE, CAMP_RED };

	// 计算每个阵营的平均经验
	map<CAMP_TYPE, int> nAvgExpList;
	for (int i = 0; i < nCampNum; ++i)
	{
		CAMP_TYPE nCamp = nCampListList[i];

		// 计算阵营平均经济
		nAvgExpList[nCamp] = warMiscMgr->getAvgExpByCamp(nCamp);
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
			int nDiffExp = nAvgExpList[nEnemyCamp] - itPer->nExperien;

			int nCamp = itPer->nCamp;

			// 根据与对手的平均经济差，计算经济落后的级别
			if (nDiffExp <= m_AIExpCtrlLine[nCamp][EWAIExpCtrlLine_1])
			{
				m_AIExpCtrlList[itPer->uPDBID] = EWAIExpDiff_NONE;
			}
			else if (nDiffExp > m_AIExpCtrlLine[nCamp][EWAIExpCtrlLine_1] && nDiffExp <= m_AIExpCtrlLine[nCamp][EWAIExpCtrlLine_2])
			{
				m_AIExpCtrlList[itPer->uPDBID] = EWAIExpDiff_SMALL;
			}
			else if (nDiffExp > m_AIExpCtrlLine[nCamp][EWAIExpCtrlLine_2] && nDiffExp <= m_AIExpCtrlLine[nCamp][EWAIExpCtrlLine_3])
			{
				m_AIExpCtrlList[itPer->uPDBID] = EWAIExpDiff_BIG;
			}
			else
			{
				m_AIExpCtrlList[itPer->uPDBID] = EWAIExpDiff_HUGE;
			}

#ifdef __AI_ECM_CTRL_DEBUG_PRINT
			TraceLn(__FUNCTION__ << _GT("AI经验控制,AI经验： PDBID=") << itPer->uPDBID << _GT(" 玩家=") << itPer->szName << _GT(" 经验差级别= ") << m_AIExpCtrlList[itPer->uPDBID]);
#endif
		}
	}
}


int CAIExpCtrl::controlExp(PDBID pdbID, int nAddValue, EExpDropType nDropType)
{
	EWEcmExpCtrlRatioType nRatioType = EWEcmExpCtrlRatioType_None;
	switch (nDropType)
	{
	case EEXPDROPTYPE_PLAYER:	// 玩家增加金钱
		{
			nRatioType = EWEcmExpCtrlRatioType_Hero;
		}
		break;
	case EEXPDROPTYPE_SOLIDER:	// 小兵增加金钱
		{
			nRatioType = EWEcmExpCtrlRatioType_Sodlier;
		}
		break;
	case EEXPDROPTYPE_NORMAL:	// 自然增加金钱
		{
			nRatioType = EWEcmExpCtrlRatioType_Salary;
		}
		break;
	case EEXPDROPTYPE_MONSTER:	// 野怪增加金钱
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

	float fRatio = getAIExpCtrlRatio(pdbID, nRatioType);

	int nNewExp = (1 + fRatio) * nAddValue;

#ifdef __AI_ECM_CTRL_DEBUG_PRINT
	TraceLn(__FUNCTION__ << _GT("AI经验控制,AI经验： 旧的经验=") << nAddValue << _GT(" 新的经验=") << nNewExp << " fRatio=" << fRatio << " PDBID=" << pdbID << _GT(" 获得天经验类型=") << nRatioType);
#endif

	return nNewExp;
}

// 获取某个玩家的经济控制系数
float CAIExpCtrl::getAIExpCtrlRatio(PDBID pdbID, EWEcmExpCtrlRatioType nRatioType)
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
		std::map<PDBID, int>::iterator iter = m_AIExpCtrlList.find(pdbID);
		if (iter == m_AIExpCtrlList.end())
		{
			break;
		}
		if (iter->second == EWAIExpDiff_NONE)
		{
			break;
		}

		ISchemeAIEcmExpCtrl* pEcmExpCtrl = gServerGlobal->getSchemeCenter()->getSchemeAIEcmExpCtrl();
		if (pEcmExpCtrl == NULL)
		{
			break;
		}

		// 获取本玩家的阵营
		int nAvgMatchScore = getMatchScoreForCalc(pdbID);

		// 计算经济控制系数				
		int nConfigWarID = m_pWar->getStaticID();
		SAIExpCtrlConfig_S* pExpCtrlConfig = pEcmExpCtrl->getAIExpCtrl(nConfigWarID, nAvgMatchScore, iter->second - 1);
		if (pExpCtrlConfig != NULL)
		{
			fRatio = pExpCtrlConfig->nRatioType[nRatioType];
		}

	} while (false);

	return fRatio;
}

int CAIExpCtrl::controlContKillExp(IActorService * pDead, IActorService * pAttacker, int nAddValue, float fDropGainTalentRate, float fContKillEmcCtrlRatio, int nMaxMultiKill, int nContKillCount)
{
	if (m_pWar == NULL)
		return nAddValue;

	IWarMiscManager* warMiscMgr = m_pWar->getMiscManager();
	if (warMiscMgr == NULL)
	{
		ErrorLn(__FUNCTION__":warMiscMgr == NULL");
		return 0;
	}

	SWPerson* pPerson = m_pWar->getPlayerInfo(pDead->getUID());
	if (pPerson == NULL)
		return nAddValue;

	// 经济差D = 被击杀的敌方英雄经济M - 击杀方阵营英雄的平均经济L
	// 新的天赋值 = （1 + 经济控制差系数F）* (标准天赋值 * (1 + 连杀系数 + 连杀次数) + 经济控制差系数G * 经济差D)

	int nDeadExp = pPerson->nExperien;
	// 攻击方阵营平均经济
	int nAttackerCamp = pAttacker->getProperty_Integer(PROPERTY_CAMP);
	int nAttackerCampAvgExp = warMiscMgr->getAvgExpByCamp(nAttackerCamp);
	// 计算经济差D,当D小于0时,取0
	int nExpDiff = nDeadExp - nAttackerCampAvgExp;
	nExpDiff = max(0, nExpDiff);

	nContKillCount = min(nMaxMultiKill, nContKillCount);

	PDBID attackerPDBID = pAttacker->getProperty_Integer(PROPERTY_ID);
	float fExpRatio = getAIExpCtrlRatio(attackerPDBID, EWEcmExpCtrlRatioType_Hero);

	int nNewExp = int((1 + fExpRatio) * (nAddValue * (1 + fDropGainTalentRate * nContKillCount) + fContKillEmcCtrlRatio * nExpDiff));

#ifdef __AI_ECM_CTRL_DEBUG_PRINT
	char log[256];
	ssprintf_s(log, sizeof(log), "%s:AI经验控制,新的经验值=%d,经验控制差F=%f,标准经验值=%d,连杀系数=%f,连杀次数=%d,经验控制差系数G=%f,经验差D=%d,被杀者经验=%d,攻击方平均经验=%d,被击杀者名字=%s,攻击者名字=%s",
		__FUNCTION__, nNewExp, fExpRatio, nAddValue, fDropGainTalentRate, nContKillCount, fContKillEmcCtrlRatio, nExpDiff, nDeadExp, nAttackerCampAvgExp,
		pDead->getName(), pAttacker->getName());
	TraceLn(log);
#endif

	return nNewExp;
}
