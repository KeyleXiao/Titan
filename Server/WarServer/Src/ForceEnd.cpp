/*******************************************************************
** 文件名: SoliderVigour.cpp
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/3/28
** 版  本: 1.0
** 描  述: 士兵气势
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/

#include "StdAfx.h"
#include "ForceEnd.h"
#include "IWar.h"
#include "ExternalHelper.h"
#include "Chat_ManagedDef.h"
#include "ITimerService.h"
#include "ExternalHelper.h"
#include "WarDef_Internal.h"

#define PRINT_FORCEEND_DEBUG

CForceEnd::CForceEnd(IWar* pWar)
	:m_pWar(pWar)
	, m_nCurRemain5Min(5)
	, m_nCurRemain10Sec(10)
{

}

CForceEnd::~CForceEnd()
{
	m_pWar = NULL;
}

void CForceEnd::OnTimer(unsigned long dwTimerID)
{
	if (m_pWar == NULL || m_pWar->isEnd())
		return;

	char szParam[64] = { 0 };
	switch (dwTimerID)
	{
	case ETimerEventID_ForceEnd:
		{
#ifdef PRINT_FORCEEND_DEBUG
			EmphasisLn(__FUNCTION__ " ETimerEventID_ForceEnd,playTime=" << m_pWar->getWarPlayTime() / 1000);
#endif
			// 强制结算处理
			onForceEnd();
		}
		break;
	case ETimerEventID_ForceEnd5Min_Start:
		{
#ifdef PRINT_FORCEEND_DEBUG
			EmphasisLn(__FUNCTION__ " ETimerEventID_ForceEnd5Min_Start,playTime=" << m_pWar->getWarPlayTime() / 1000);
#endif
			// 开始5分钟倒计时,每隔一分钟发一次(5,4,3,2,1)
			ITimerService * pTimerService = gServerGlobal->getTimerService();
			if (pTimerService != NULL)
			{
				pTimerService->SetTimer(ETimerEventID_ForceEnd5Min, ForceEnd5MinTime, this, 4);
			}

			// 发送5分钟倒计时
			ssprintf_s(szParam, sizeof(szParam), "%d;%c;%d", CHAT_CHANNEL_WARSCENE, CHAT_MSG_ITEM_FLAG, m_nCurRemain5Min);
			g_EHelper.showSystemMessage(m_pWar->getWarSceneID(), CHAT_TIP_WAR_FORCE_END_COUNTDOWN_MIN, szParam);

			m_nCurRemain5Min--;
		}
		break;
	case ETimerEventID_ForceEnd10Sec_Start:
		{
#ifdef PRINT_FORCEEND_DEBUG
			EmphasisLn(__FUNCTION__ " ETimerEventID_ForceEnd10Sec_Start,playTime=" << m_pWar->getWarPlayTime() / 1000);
#endif
			// 开始10秒倒计时
			ITimerService * pTimerService = gServerGlobal->getTimerService();
			if (pTimerService != NULL)
			{
				pTimerService->SetTimer(ETimerEventID_ForceEnd10Sec, ForceEnd10SecTime, this, 10);
			}

			// 发送“10倒计时开始”到客户端
			ssprintf_s(szParam, sizeof(szParam), "%d;%c;%d", CHAT_CHANNEL_WARSCENE, CHAT_MSG_ITEM_FLAG, m_nCurRemain10Sec);
			g_EHelper.showSystemMessage(m_pWar->getWarSceneID(), CHAT_TIP_WAR_FORCE_END_COUNTDOWN_SEC_START, szParam);
		}
		break;
	case ETimerEventID_ForceEnd5Min:
		{
#ifdef PRINT_FORCEEND_DEBUG
			EmphasisLn(__FUNCTION__ " ETimerEventID_ForceEnd5Min,playTime=" << m_pWar->getWarPlayTime() / 1000);
#endif
			// 发送5分钟倒计时
			ssprintf_s(szParam, sizeof(szParam), "%d;%c;%d", CHAT_CHANNEL_WARSCENE, CHAT_MSG_ITEM_FLAG, m_nCurRemain5Min);
			g_EHelper.showSystemMessage(m_pWar->getWarSceneID(), CHAT_TIP_WAR_FORCE_END_COUNTDOWN_MIN, szParam);

			m_nCurRemain5Min--;
		}
		break;
	case ETimerEventID_ForceEnd10Sec:
		{
#ifdef PRINT_FORCEEND_DEBUG
			EmphasisLn(__FUNCTION__ " ETimerEventID_ForceEnd10Sec,playTime=" << m_pWar->getWarPlayTime() / 1000);
#endif
			// 发送10s倒计时
			ssprintf_s(szParam, sizeof(szParam), "%d;%c;%d", CHAT_CHANNEL_WARSCENE, CHAT_MSG_ITEM_FLAG, m_nCurRemain10Sec);
			g_EHelper.showSystemMessage(m_pWar->getWarSceneID(), CHAT_TIP_WAR_FORCE_END_COUNTDOWN_SEC, szParam);

			m_nCurRemain10Sec--;
		}
		break;
	default:
		break;
	}
}

bool CForceEnd::Init(int nMatchType)
{
	if (m_pWar == NULL)
	{
		return false;
	}

	// 判断强制结算是否开启
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return false;

	ISchemeWarForceEnd* pSchemeWarForceEnd = pSchemeCenter->getSchemeWarForceEnd();
	if (pSchemeWarForceEnd == NULL)
		return false;

	if (!pSchemeWarForceEnd->isEnableForceEnd(nMatchType))
		return false;

	// 获取强制结算信息
	m_pForceEndInfo = pSchemeWarForceEnd->getWarForceEndInfo(nMatchType);
	if (m_pForceEndInfo == NULL)
	{
		ErrorLn(__FUNCTION__ " Read ForceEnd scheme failed.nForceEndType=" << nMatchType);
		return false;
	}

	return true;
}

void CForceEnd::Release()
{
	// 清除定时器
	g_EHelper.KillTimer(ETimerEventID_ForceEnd, this);
	g_EHelper.KillTimer(ETimerEventID_ForceEnd5Min_Start, this);
	g_EHelper.KillTimer(ETimerEventID_ForceEnd10Sec_Start, this);
	g_EHelper.KillTimer(ETimerEventID_ForceEnd5Min, this);
	g_EHelper.KillTimer(ETimerEventID_ForceEnd10Sec, this);

	delete this;
}

void CForceEnd::onWarRealStart()
{
	// 强制结束时间定时器(毫秒)
	DWORD dwForceEndTime = 0;
	DWORD dwCurWarPlayTime = m_pWar->getWarPlayTime();
	DWORD dwTriggerTime = m_pForceEndInfo->dwTriggerTime * 1000;  // 转为毫秒
	if (dwTriggerTime > dwCurWarPlayTime)
	{
		dwForceEndTime = dwTriggerTime - dwCurWarPlayTime;
	}

	// 强制结束5分钟倒计时开始
	DWORD dwForceEnd5Min = 0;
	const DWORD dw5MinMs = 5 * 60 * 1000;
	if (dwForceEndTime > dw5MinMs)
	{
		dwForceEnd5Min = dwForceEndTime - dw5MinMs;
	}

	// 强制结束10秒倒计时开始
	DWORD dwForceEnd10Sec = 0;
	if (dwForceEndTime > (10 * 1000))
	{
		dwForceEnd10Sec = dwForceEndTime - (10 * 1000);
	}

#ifdef PRINT_FORCEEND_DEBUG
	EmphasisLn(__FUNCTION__ " settimer," << dwForceEndTime << "," << dwForceEnd5Min << "," << dwForceEnd10Sec);
#endif

	// 设置定时器
	g_EHelper.SetTimer(ETimerEventID_ForceEnd, dwForceEndTime, this, 1);
	g_EHelper.SetTimer(ETimerEventID_ForceEnd5Min_Start, dwForceEnd5Min, this, 1);
	g_EHelper.SetTimer(ETimerEventID_ForceEnd10Sec_Start, dwForceEnd10Sec, this, 1);
}

void CForceEnd::onWarEnd()
{
	g_EHelper.KillTimer(ETimerEventID_ForceEnd, this);
	g_EHelper.KillTimer(ETimerEventID_ForceEnd5Min_Start, this);
	g_EHelper.KillTimer(ETimerEventID_ForceEnd10Sec_Start, this);
	g_EHelper.KillTimer(ETimerEventID_ForceEnd5Min, this);
	g_EHelper.KillTimer(ETimerEventID_ForceEnd10Sec, this);
}

int CForceEnd::getScore(int nCamp)
{
	if (m_pWar == NULL)
	{
		return 0;
	}

	IWarMiscManager* warMiscMgr = m_pWar->getMiscManager();
	if (warMiscMgr == NULL)
	{
		ErrorLn(__FUNCTION__":warMiscMgr == NULL");
		return 0;
	}

	// 获取人头差
	int nKillDiff = getCampKillNum(nCamp) - getCampKillNum(getEnemyCamp(nCamp));

	// 获取经济差
	int nEcmDiff = warMiscMgr->getAvgEmcByCamp(nCamp) - warMiscMgr->getAvgEmcByCamp(getEnemyCamp(nCamp));

	// 获取推塔数差
	int nTowerDiff = getCampDestroyTowerNum(nCamp) - getCampDestroyTowerNum(getEnemyCamp(nCamp));

	// 分数 = 人头差×人头差权重 + 经济差×经济差权重 + 推塔差×推塔差权重
	return (nKillDiff*m_pForceEndInfo->nKillDiffWeight + nEcmDiff*m_pForceEndInfo->nEcmDiffWeight + nTowerDiff*m_pForceEndInfo->nTowerDiffWeight);
}

int CForceEnd::getCampKillNum(int nCamp)
{
	if (m_pWar == NULL)
	{
		return 0;
	}

	IWarMiscManager* warMiscMgr = m_pWar->getMiscManager();
	if (warMiscMgr == NULL)
	{
		ErrorLn(__FUNCTION__":warMiscMgr == NULL");
		return 0;
	}

	SCampTeamRecordInfo* teamRecordInfo = warMiscMgr->getCampRecordInfo(nCamp);
	if (teamRecordInfo == NULL)
	{
		return 0;
	}

	return teamRecordInfo->nCampKillNum;
}

int CForceEnd::getCampDestroyTowerNum(int nCamp)
{
	int nTowerNum = 0;

	SWarCamp* pWarCamp = m_pWar->getWarCamp(nCamp);
	if (pWarCamp == NULL)
		return 0;

	WarPersonVector& warPesVect = pWarCamp->warPersVect;
	WarPersonVector::iterator itPerson = warPesVect.begin();
	for (; itPerson != warPesVect.end(); ++itPerson)
	{
		nTowerNum += itPerson->nBaseRecord[EDT_DestoryTower];
	}

	return nTowerNum;
}

int CForceEnd::getEnemyCamp(int nCamp)
{
	if (nCamp == CAMP_BLUE)
	{
		return CAMP_RED;
	}
	else if (nCamp == CAMP_RED)
	{
		return CAMP_BLUE;
	}
	else
	{
		return CAMP_NEUTRAL;
	}
}

void CForceEnd::onForceEnd()
{
	if (m_pWar == NULL || m_pWar->isEnd())
		return;

	// 强制结算
	int nFailedCamp = 0;

	// 判断输赢
	int nBlueScore = getScore(CAMP_BLUE);
	int nRedScore = getScore(CAMP_RED);
	if (nBlueScore > nRedScore)
	{
		nFailedCamp = CAMP_RED;
	}
	else if (nRedScore > nBlueScore)
	{
		nFailedCamp = CAMP_BLUE;
	}
	else
	{
		// 分数相等,则随机一个阵营为失败方
		int nRandNum = mt_range_rand(1, 100);
		if (nRandNum > 50)
		{
			nFailedCamp = CAMP_RED;
		}
	}

	// 强制结束战场
	m_pWar->forceEndWar(nFailedCamp, false);
}
