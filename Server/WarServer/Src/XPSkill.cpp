/*******************************************************************
** 文件名: XPSkill.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/3/20
** 版  本: 1.0
** 描  述: XP技能
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/

#include "StdAfx.h"
#include "XPSkill.h"
#include "ExternalHelper.h"
#include "IServerGlobal.h"
#include "ITimeSyncService.h"
#include "EntityHelper.h"
#include "SWPerson.h"
#include "WarDef_Internal.h"
#include "WarMessageDef.h"
#include "IWar.h"

CXPSkill::CXPSkill(IWar* pWar)
	:m_pWar(pWar)
	,m_nLastRefreshXpTick(0)
	,m_xpSkillAddState(ESKILLXP_ADDSTATE_RANDOMPLAYER)
	,m_nXPSkillBuffID(0)
{

}

CXPSkill::~CXPSkill()
{
	m_pWar = NULL;
}


bool CXPSkill::Init(int nMatchType)
{
	if (m_pWar == NULL)
		return false;

	SEntityXml_Server* pEntityXml = g_EHelper.getEntityXml();
	if (pEntityXml == NULL)
		return false;

	m_nXPSkillBuffID = pEntityXml->nXPSkillBuffID;

	return true;
}

void CXPSkill::Release()
{
	delete this;
}

void CXPSkill::onWarRealStart()
{
}

void CXPSkill::onWarEnd()
{
}


// 1、随机xp技能(冷却中)给某个玩家 2、xp技能冷却中的玩家检测冷却完毕的xp技能给添加buff给玩家使用
void CXPSkill::checkPlayerXpSkillChangeInTimes()
{
	if (m_pWar == NULL)
	{
		return;
	}


	SEntityXml_Server* pEntityXml = g_EHelper.getEntityXml();
	if (pEntityXml == NULL)
		return;
	
	int nXpCdTime = pEntityXml->nXPCdTime;
	if (nXpCdTime <= 0)
	{
		return;
	}

	DWORD cuTickCountEx = gServerGlobal->getTimeSyncService()->GetTick();
	if (m_nLastRefreshXpTick > 0 && m_nLastRefreshXpTick + nXpCdTime > cuTickCountEx)
	{
		return;
	}

	m_nLastRefreshXpTick = cuTickCountEx;
	switch (m_xpSkillAddState)
	{
	case ESKILLXP_ADDSTATE_RANDOMPLAYER:
	{
		randomPlayerXpSkillInCool();
	}
	break;
	case ESKILLXP_ADDSTATE_ADDXPBUFF:
	{

		setPlayerXpSkillCoolFinishedToAddXPBuff();
	}
	break;
	default:
	{
		break;
	}
	}
}

void CXPSkill::removeXPBuff(UID uid, PDBID pdbid)
{
	map<PDBID, int>::iterator it = m_mapCurrentSkill.find(pdbid);
	if (it != m_mapCurrentSkill.end())
	{

		RemoveBuff(uid, m_nXPSkillBuffID, uid, 0);
	}
}

// 随机2分钟一次
void CXPSkill::randomPlayerXpSkillInCool()
{
	if (m_pWar == NULL)
	{
		return;
	}

	// 随机时刻将所有buff移除
	for (auto itLastSkill = m_mapLastXPSkill.begin(); itLastSkill != m_mapLastXPSkill.end(); ++itLastSkill)
	{
		SWPerson *pPerson = m_pWar->getPlayerInfoByPDBIU(itLastSkill->first);
		if (pPerson == NULL)
		{
			continue;
		}

		UID uID = pPerson->uID;
		RemoveBuff(uID, m_nXPSkillBuffID, uID, 0);
	}


	m_mapCurrentSkill.clear();

	// 该XP技能随机算法只适合"同时XP使用者数目为1"的情况
	const int XPSKILL_HAVE_MAX = 1;

	vector<int> campList = m_pWar->getCampList();
	for(int i = 0; i < campList.size(); ++i)
	{
		int nCamp = campList[i];
		SWarCamp* pWarCamp = m_pWar->getWarCamp(nCamp);
		if (pWarCamp == NULL)
			continue;

		const int nPlayerCount = pWarCamp->nAIPlayerCount + pWarCamp->nRealPlayerCount;
		if (nPlayerCount == 0)
		{
			continue;
		}

		// 统计使用过XP的人数
		int nCampXpCount = 0;
		for (auto iterXp = m_mapPossessdSkill.begin(); iterXp != m_mapPossessdSkill.end(); ++iterXp)
		{
			if (iterXp->second == nCamp)
				nCampXpCount++;
		}

		// 当所有人都使用过XP,则全部删除,从新一轮开始随机（保证每人都有一次机会随机到xp）
		if (nCampXpCount >= nPlayerCount)
		{
			for (auto iterXp = m_mapPossessdSkill.begin(); iterXp != m_mapPossessdSkill.end();)
			{
				if (iterXp->second == nCamp)
					iterXp = m_mapPossessdSkill.erase(iterXp);
				else
					++iterXp;
			}
		}

		const int nPersonCount = pWarCamp->warPersVect.size();

		// 选出可以获取XP使用机会的玩家
		std::vector<PDBID> xpPlayerList;
		WarPersonVector::iterator  itPerson = pWarCamp->warPersVect.begin();
		for (;itPerson != pWarCamp->warPersVect.end(); ++itPerson)
		{
			if (nPersonCount == XPSKILL_HAVE_MAX)	// 总人数等于"XP同时拥有人数"时,则所有人都可以使用XP
			{
				xpPlayerList.push_back(itPerson->uPDBID);
			}
			else if (IsCanUseXpSkill(itPerson->uPDBID))
			{
				xpPlayerList.push_back(itPerson->uPDBID);
			}
		}
		
		// 随机可以使用XP的玩家
		for (int i = 0; i < XPSKILL_HAVE_MAX; ++i)
		{
			if (xpPlayerList.empty())
			{
				break;
			}

			int nRandNum = mt_range_rand(0, xpPlayerList.size() - 1);

			PDBID xpPDBID = xpPlayerList[nRandNum];

			SWPerson* pPerson = m_pWar->getPlayerInfoByPDBIU(xpPDBID);
			if (pPerson == NULL)
			{
				continue;
			}

			m_mapCurrentSkill[xpPDBID] = pPerson->nXPSkillID;
			m_mapPossessdSkill[xpPDBID] = nCamp;

			if (pPerson->nXPSkillID > 0)
			{
				sendXpSkillRandomPlayerToClient(xpPDBID);
			}
			
			// 删除本次随机到的玩家
			auto curXpPlayerIter = find(xpPlayerList.begin(), xpPlayerList.end(), xpPDBID);
			xpPlayerList.erase(curXpPlayerIter);
		}
	}

	m_mapLastXPSkill.clear();
	m_mapLastXPSkill = m_mapCurrentSkill;

	m_xpSkillAddState = ESKILLXP_ADDSTATE_ADDXPBUFF;
}

void CXPSkill::sendXpSkillRandomPlayerToClient(PDBID dwPDBID)
{
	SWPerson* pPerson = m_pWar->getPlayerInfoByPDBIU(dwPDBID);
	if (pPerson == NULL)
	{
		return;
	}

	UID uidTarget = pPerson->uID;

	SWar2ViewMsgXPBeginCD msg;
	msg.pdbid = dwPDBID;
	msg.nBeginCDTick = gServerGlobal->getTimeSyncService()->GetTick();
    msg.nSkillID = pPerson->nXPSkillID;
    msg.uID = pPerson->uID;
	SEntityXml_Server* pEntityXml = g_EHelper.getEntityXml();
	if (pEntityXml != NULL)
	{
		msg.nCDTime = pEntityXml->nXPCdTime;
	}

	m_pWar->sendDataToClient(uidTarget, WAR2VIEW_MSG_WAR_XP_BEGIN_CD, &msg, sizeof(msg), true);
}

void CXPSkill::setPlayerXpSkillCoolFinishedToAddXPBuff()
{
	addXPBuff();
	m_xpSkillAddState = ESKILLXP_ADDSTATE_RANDOMPLAYER;
}

void CXPSkill::addXPBuff()
{
	// 添加buff
	map<PDBID, int>::iterator itCur = m_mapCurrentSkill.begin();
	for (; itCur != m_mapCurrentSkill.end(); ++itCur)
	{
		SWPerson *pPerson = m_pWar->getPlayerInfoByPDBIU(itCur->first);
		if (pPerson == NULL)
		{
			continue;
		}
		
		// XPSKillID无效
		if (itCur->second <= 0)
		{
			continue;
		}

		UID uID = pPerson->uID;
		if (!AddBuff(uID, m_nXPSkillBuffID, 1, uID))
		{
			ErrorLn(__FUNCTION__":" << "Add XP buff faield for player .uID=" << uID);
		}
	}
}

int CXPSkill::getCanUseXPPlayerCount(int nCamp)
{
	auto pCamp = m_pWar->getWarCamp(nCamp);
	if (pCamp == NULL)
		return 0;

	int nCount = 0;
	for (auto iter = pCamp->warPersVect.begin(); iter != pCamp->warPersVect.end(); ++iter)
	{
		if (iter->nXPSkillID > 0)
		{
			nCount++;
		}
	}

	return nCount;
}

bool CXPSkill::judgeIsCanHaveXpSkill(PDBID pDBID, int nPlayerNum)
{
	if (nPlayerNum == 1)
	{
		return true;
	}
	else if (nPlayerNum <= 0)
	{
		return false;
	}

	// 判断上一次技能拥有者
	map<PDBID, int>::iterator it = m_mapPossessdSkill.find(pDBID);
	if (it != m_mapPossessdSkill.end())
	{
		return false;
	}

	return true;
}

bool CXPSkill::IsCanUseXpSkill(PDBID pDBID)
{
	// 判断上一次技能拥有者
	map<PDBID, int>::iterator it = m_mapPossessdSkill.find(pDBID);
	if (it != m_mapPossessdSkill.end())
	{
		return false;
	}

	return true;
}
