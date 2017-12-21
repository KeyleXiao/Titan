/*******************************************************************
** 文件名: WarAddBuff.cpp
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/6/7
** 版  本: 1.0
** 描  述: 战场自动添加Buff
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/

#include "StdAfx.h"
#include "WarAddBuff.h"
#include "IWar.h"
#include "ExternalHelper.h"
#include "ITimerService.h"
#include "WarDef_Internal.h"

#define PRINT_WARADDBUFF_DEBUG

CWarAddBuff::CWarAddBuff(IWar* pWar)
	: m_pWar(pWar)
	, m_nMatchTypeID(0)
{
}

CWarAddBuff::~CWarAddBuff()
{
	m_pWar = NULL;
}

void CWarAddBuff::OnTimer(unsigned long dwTimerID)
{
	if (m_pWar == NULL || m_pWar->isEnd())
		return;

	IWarMiscManager* warMiscMgr = m_pWar->getMiscManager();
	if (warMiscMgr == NULL)
	{
		ErrorLn(__FUNCTION__":warMiscMgr == NULL");
		return;
	}

	map<int, SWarAddBuff>::iterator iter = m_BuffInfoList.find(dwTimerID);
	if (iter == m_BuffInfoList.end())
	{
		return;
	}

	bool bAddToAI = iter->second.bAddToAI;
	bool bAddToAIEx = iter->second.bAddToAIEx;
	int nLimitLv = iter->second.nLimitLv;

	// 添加buf
	UID uidList[128] = { 0 };
	int nCount = m_pWar->getAllPerson(uidList, sizeof(uidList));
	for (int i = 0; i < nCount; ++i)
	{
		UID uid = uidList[i];
		SWPerson* pPerson = m_pWar->getPlayerInfo(uid);
		if (pPerson == NULL)
			continue;

		// 判断是否给机器人加
		if (pPerson->bIsComputer)
		{
			if (bAddToAI == false)
				continue;

			// 是否给只有玩家阵营的机器人添加Buff
			bool isHaveRealPlayer = warMiscMgr->isHaveRealPlayer(pPerson->nCamp);
			if (bAddToAIEx && isHaveRealPlayer == false)
				continue;
		}

		if (pPerson->nLevel > nLimitLv)
			continue;

		AddBuff(pPerson->uID, iter->second.nBuffID, iter->second.nBuffLv, pPerson->uID);
	}


}

bool CWarAddBuff::Init(int nMatchType)
{
	if (m_pWar == NULL)
	{
		return false;
	}

	m_nMatchTypeID = nMatchType;

	return true;
}

void CWarAddBuff::Release()
{
	// 删除定时器
	ITimerService * pTimerService = gServerGlobal->getTimerService();
	if (pTimerService != NULL)
	{
		map<int, SWarAddBuff>::iterator iter = m_BuffInfoList.begin();
		for (; iter != m_BuffInfoList.end(); ++iter)
		{
			pTimerService->KillTimer(iter->first, this);
		}
	}

	m_pWar = NULL;

	delete this;
}

void CWarAddBuff::onWarRealStart()
{
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return ;

	ISchemeWarAddBuff* pSchemeWarAddBuff = pSchemeCenter->getSchemeWarAddBuff();
	if (pSchemeWarAddBuff == NULL)
		return ;

	ITimerService * pTimerService = gServerGlobal->getTimerService();
	if (pTimerService == NULL)
		return ;

	vector<SWarAddBuff>* pWarAddBuff = pSchemeWarAddBuff->getAddBuffInfo(m_nMatchTypeID);
	if (pWarAddBuff != NULL)
	{
		// 设置定时器
		int nTimerID = ETimerEventID_AutoAddBuff;
		vector<SWarAddBuff>::iterator iter = pWarAddBuff->begin();
		for (; iter != pWarAddBuff->end(); ++iter)
		{
			int nTime = iter->nTime;
			if (nTime < 0)
				continue;

			m_BuffInfoList[nTimerID] = *iter;

			pTimerService->SetTimer(nTimerID, nTime, this, 1);

			nTimerID++;
		}

	}
}

void CWarAddBuff::onWarEnd()
{

}
