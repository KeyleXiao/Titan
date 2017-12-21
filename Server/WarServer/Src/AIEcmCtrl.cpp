/*******************************************************************
** 文件名: AIEcmCtrl.cpp
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
#include "AIEcmCtrl.h"
#include "IWar.h"
#include "WarDef_Internal.h"

//#define __AI_ECM_CTRL_DEBUG_PRINT

CAIEcmCtrl::CAIEcmCtrl(IWar* pWar)
	: m_pWar(pWar)
	, m_bInitAIEcmCtrlLine(false)
	, m_bInitAvgMatchScore(false)
{
}

CAIEcmCtrl::~CAIEcmCtrl()
{
	m_pWar = NULL;
}

bool CAIEcmCtrl::Init(int nMatchType)
{
	if (m_pWar == NULL)
		return false;

	m_nMatchType = nMatchType;

	return true;
}

void CAIEcmCtrl::Release()
{
	delete this;
}

void CAIEcmCtrl::onWarRealStart()
{
}

void CAIEcmCtrl::onWarEnd()
{
}

void CAIEcmCtrl::initAvgMatchScore()
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
		TraceLn(__FUNCTION__ << _GT("AI经济控制, 匹配分：") << m_mapMatchScore[iter->first] << _GT(" 阵营=") << iter->first);
#endif
	}
	m_bInitAvgMatchScore = true;
}

// 计算经济控制系数
void CAIEcmCtrl::calcAIEcmCtrlRatio()
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
	if (!m_bInitAIEcmCtrlLine)
	{
#ifdef __AI_ECM_CTRL_DEBUG_PRINT
		TraceLn(__FUNCTION__ << _GT("AI经济控制,AI经验控制线还没有初始化"));
#endif
		return;
	}

	// 检查平均分是否已经初始化
	if (!m_bInitAvgMatchScore)
	{
#ifdef __AI_ECM_CTRL_DEBUG_PRINT
		TraceLn(__FUNCTION__ << _GT("AI经济控制,平均分还没初始化"));
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

			int nCamp = itPer->nCamp;

			// 根据与对手的平均经济差，计算经济落后的级别
			if (nDiffEmc <= m_AIEcmCtrlLine[nCamp][EWAIEcmCtrlLine_1])
			{
				m_AIEcmCtrlList[itPer->uPDBID] = EWAIEcmDiff_NONE;
			}
			else if (nDiffEmc > m_AIEcmCtrlLine[nCamp][EWAIEcmCtrlLine_1] && nDiffEmc <= m_AIEcmCtrlLine[nCamp][EWAIEcmCtrlLine_2])
			{
				m_AIEcmCtrlList[itPer->uPDBID] = EWAIEcmDiff_SMALL;
			}
			else if (nDiffEmc > m_AIEcmCtrlLine[nCamp][EWAIEcmCtrlLine_2] && nDiffEmc <= m_AIEcmCtrlLine[nCamp][EWAIEcmCtrlLine_3])
			{
				m_AIEcmCtrlList[itPer->uPDBID] = EWAIEcmDiff_BIG;
			}
			else
			{
				m_AIEcmCtrlList[itPer->uPDBID] = EWAIEcmDiff_HUGE;
			}

#ifdef __AI_ECM_CTRL_DEBUG_PRINT
			TraceLn(__FUNCTION__ << _GT("AI经济控制, PDBID=") << itPer->uPDBID << _GT(" 玩家=") << itPer->szName << _GT(" 经济差级别= ") << m_AIEcmCtrlList[itPer->uPDBID]);
#endif
		}
	}
}

// 计算经济控制线
void CAIEcmCtrl::calcAIEcmCtrlLine()
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
		for (int i = EWAIEcmCtrlLine_1; i < EWAIEcmCtrlLine_MAX; ++i)
		{
			SAIEcmCtrlConfig_S* pEcmCtrlConfig = pEcmExpCtrl->getAIEcmCtrl(nConfigWarID, m_mapMatchScore[nCamp], i);
			if (pEcmCtrlConfig != NULL)
			{
				m_AIEcmCtrlLine[nCamp][i] = pEcmCtrlConfig->aRatio * nPlayTime + pEcmCtrlConfig->bRatio;

#ifdef __AI_ECM_CTRL_DEBUG_PRINT
				TraceLn(__FUNCTION__ << _GT("AI经济控制, 时间=") << nPlayTime << _GT(" 经济差线条") << i + 1 << "=" << m_AIEcmCtrlLine[nCamp][i] << _GT(" 阵营=") << nCamp);
#endif
			}
		}
	}
	m_bInitAIEcmCtrlLine = true;
}

// 获取某个玩家的经济控制系数
float CAIEcmCtrl::getAIEcmCtrlRatio(PDBID pdbID, EWEcmExpCtrlRatioType nRatioType)
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
		std::map<PDBID, int>::iterator iter = m_AIEcmCtrlList.find(pdbID);
		if (iter == m_AIEcmCtrlList.end())
		{
			break;
		}
		if (iter->second == EWAIEcmDiff_NONE)
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
		SAIEcmCtrlConfig_S* pEcmCtrlConfig = pEcmExpCtrl->getAIEcmCtrl(nConfigWarID, nAvgMatchScore, iter->second - 1);
		if (pEcmCtrlConfig != NULL)
		{
			fRatio = pEcmCtrlConfig->nRatioType[nRatioType];
		}

	} while (false);

	return fRatio;
}

int CAIEcmCtrl::getMatchScoreForCalc(PDBID pdbid)
{
	int nAvgMatchScore = 0;
	SWPerson* pPerson = m_pWar->getPlayerInfoByPDBIU(pdbid);
	if (pPerson != NULL)
	{
		nAvgMatchScore = m_mapMatchScore[pPerson->nCamp];
	}

	return nAvgMatchScore;
}

// 计算经济控制后获得的天赋点
int CAIEcmCtrl::controlTelentPoint(PDBID pdbID, int nAddValue, ETalentDropType nDropType)
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

	float fRatio = getAIEcmCtrlRatio(pdbID, nRatioType);

	int nNewAddValue = (1 + fRatio) * nAddValue;

#ifdef __AI_ECM_CTRL_DEBUG_PRINT
	TraceLn(__FUNCTION__ << _GT("AI经济控制, 旧的天赋值=") << nAddValue << _GT(" 新的天赋值=") << nNewAddValue << " fRatio=" << fRatio << " PDBID=" << pdbID << _GT(" 获得天赋值类型=") << nRatioType);
#endif

	return nNewAddValue;
}

// 控制连杀经济
int CAIEcmCtrl::controlContKillTelentPoint( IActorService* pDead, IActorService* pAttacker, int nAddValue, float fDropGainTalentRate, float fContKillEmcCtrlRatio, int nMaxMultiKill, int nContKillCount)
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

	int nDeadTalentPoint = pPerson->nBaseRecord[EDT_Money];
	// 攻击方阵营平均经济
	int nAttackerCamp = pAttacker->getProperty_Integer(PROPERTY_CAMP);
	int nAttackerCampAvgEcm = warMiscMgr->getAvgEmcByCamp(nAttackerCamp);
	// 计算经济差D,当D小于0时,取0
	int nEmcDiff = nDeadTalentPoint - nAttackerCampAvgEcm;
	nEmcDiff = max(0, nEmcDiff);

	nContKillCount = min(nMaxMultiKill, nContKillCount);
	
	PDBID attackerPDBID = pAttacker->getProperty_Integer(PROPERTY_ID);
	float fEmcRatio = getAIEcmCtrlRatio(attackerPDBID, EWEcmExpCtrlRatioType_Hero);

	int nNewTalentPoint = int((1 + fEmcRatio) * (nAddValue * (1 + fDropGainTalentRate * nContKillCount) + fContKillEmcCtrlRatio * nEmcDiff));

#ifdef __AI_ECM_CTRL_DEBUG_PRINT
	char log[256];
	ssprintf_s(log, sizeof(log), "%s:AI经济控制,新的天赋值=%d,经济控制差F=%f,标准天赋值=%d,连杀系数=%f,连杀次数=%d,经济控制差系数G=%f,经济差D=%d,被杀者经济=%d,攻击方平均经济=%d,被击杀者名字=%s,攻击者名字=%s",
		__FUNCTION__,nNewTalentPoint, fEmcRatio, nAddValue, fDropGainTalentRate, nContKillCount, fContKillEmcCtrlRatio, nEmcDiff, nDeadTalentPoint, nAttackerCampAvgEcm,
		pDead->getName(), pAttacker->getName());
	TraceLn(log);
#endif
	
	return nNewTalentPoint;
}