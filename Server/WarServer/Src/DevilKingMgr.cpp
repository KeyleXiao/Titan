/*******************************************************************
** 文件名: DevilKingMgr.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/11/29
** 版  本: 1.0
** 描  述: 大逃杀-战场魔王玩法
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/

#include "StdAfx.h"
#include "DevilKingMgr.h"
#include "IWar.h"
#include "ExternalHelper.h"
#include "ITimerService.h"
#include "WarDef_Internal.h"

#define PRINT_DEVILKINGMGR_DEBUG

CDevilKingMgr::CDevilKingMgr(IWar* pWar)
	: m_pWar(pWar)
	, m_nMatchTypeID(0)
	, m_nCurDevilKing(INVALID_UID)
	, m_isClose(false)
	, m_isHasFirstDevilKing(false)
{
}

CDevilKingMgr::~CDevilKingMgr()
{
	m_pWar = NULL;
}

void CDevilKingMgr::OnTimer(unsigned long dwTimerID)
{
	if (m_pWar == NULL || m_pWar->isEnd())
		return;

	if (dwTimerID == ETimerEventID_BcastDevilKingPos)
	{
		bcastDevilKingPos();
	}
	else if (dwTimerID == ETimerEventID_CloseTime)
	{
		m_isClose = true;
	}
}

void CDevilKingMgr::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	switch (wEventID)
	{
	case EVENT_GAME_ACTOR_CONTINUE_PLAY:
		{
			if (m_isClose)
				return;

			if (nLen != sizeof(event_entity_continue_play))
			{
				ErrorLn(__FUNCTION__": nLen != sizeof(event_entity_continue_play) !" << ", nLen = " << nLen);
				return;
			}
			event_entity_continue_play* pEvent = (event_entity_continue_play*)pszContext;

			SWPerson* pPerson = m_pWar->getPlayerInfoByPDBIU(pEvent->dwDBID);
			if (pPerson == NULL)
				return;

			sendDevilKingToClient(pPerson->uID);
		}
		break;
	default:break;
	}
}

bool CDevilKingMgr::Init(int nMatchType)
{
	if (m_pWar == NULL)
	{
		return false;
	}

	m_nMatchTypeID = nMatchType;

	g_EHelper.Subscribe(m_pWar->getWarSceneID(), this, EVENT_GAME_ACTOR_CONTINUE_PLAY, TYPE_PLAYER_ROLE, 0, __FUNCTION__);

	return true;
}

void CDevilKingMgr::Release()
{
	g_EHelper.KillTimer(ETimerEventID_BcastDevilKingPos, this);
	g_EHelper.KillTimer(ETimerEventID_CloseTime, this);

	g_EHelper.UnSubscribe(m_pWar->getWarSceneID(), this, EVENT_GAME_ACTOR_CONTINUE_PLAY, TYPE_PLAYER_ROLE, 0);

	m_pWar = NULL;

	delete this;
}

void CDevilKingMgr::onWarRealStart()
{
	// 启动关闭魔王玩法的定时器
	g_EHelper.SetTimer(ETimerEventID_CloseTime, getCloseTime(), this);
}

void CDevilKingMgr::onWarEnd()
{
	g_EHelper.KillTimer(ETimerEventID_BcastDevilKingPos, this);
	g_EHelper.KillTimer(ETimerEventID_CloseTime, this);
}

void CDevilKingMgr::onPlayerDie()
{
	if (m_isClose)
		return;

	// 已经有魔王
	if (m_isHasFirstDevilKing)
		return;


	// 计算第一个魔王
	SWPerson* maxPerson = NULL;
	UID uidList[64] = { 0 };
	int nCount = m_pWar->getAllPerson(uidList, sizeof(uidList));
	for (int i = 0; i < nCount; ++i)
	{
		UID uid = uidList[i];
		SWPerson* pPerson = m_pWar->getPlayerInfo(uid);
		if (pPerson == NULL)
		{
			continue;
		}

		if (maxPerson == NULL)
		{
			maxPerson = pPerson;
		}
		else
		{
			if (maxPerson->nBaseRecord[EDT_KillCount] < pPerson->nBaseRecord[EDT_KillCount])
			{
				maxPerson = pPerson;
			}
		}
	}

	if (maxPerson != NULL  && m_nCurDevilKing != maxPerson->uID)
	{
		// 产生第一个魔王
		int nKillNum = maxPerson->nBaseRecord[EDT_KillCount];
		if (nKillNum >= getDevilKingKillNumCond())
		{
			m_isHasFirstDevilKing = true;

			// 新的魔王
			onNewDevilKing(maxPerson->uID);

			// 广播
			char szBuf[128] = { 0 };
			ssprintf_s(szBuf, sizeof(szBuf), a2utf8("%s玩家已经淘汰%d人，并且排名第一，前往击杀可获得随从"), maxPerson->szName, nKillNum);
			g_EHelper.showSystemMessageToChatBox(m_pWar->getWarSceneID(), CHAT_TIP_CHATBOX_CUSTOMER_TIP, szBuf, __LINE__);

#ifdef PRINT_DEVILKINGMGR_DEBUG
			WarningLn(__FUNCTION__":" << a2utf8("产生第一个魔王,") << maxPerson->szName << ",killNum=" << nKillNum);
#endif
		}
	}
}

void CDevilKingMgr::onNewDevilKing(UID uid)
{
	if (uid == INVALID_UID)
		return;

	SWPerson* pPerson = m_pWar->getPlayerInfo(uid);
	if (pPerson == NULL)
		return;

	m_nCurDevilKing = uid;

	// 启动定时广播魔王小地图位置定时器
	g_EHelper.KillTimer(ETimerEventID_BcastDevilKingPos, this);
	g_EHelper.SetTimer(ETimerEventID_BcastDevilKingPos, getBcastDevilKingPosInternal(), this);

	// 通知客户端新的魔王
	sendDevilKingToAll();

	// 广播小地图上魔王的位置
	bcastDevilKingPos();
}

void CDevilKingMgr::onPlayerKillPerson(UID uidMurder, UID uidDead)
{
	if (m_isClose)
		return;

	if (m_nCurDevilKing == INVALID_UID)
		return;

	if (uidMurder == uidDead)
	{
		// 魔王自杀死亡
		if (m_nCurDevilKing == uidMurder)
		{
			m_nCurDevilKing = INVALID_UID;

			// 通知客户端新的魔王
			sendDevilKingToAll();
		}

		return;
	}

	SWPerson* pMurderPerson = m_pWar->getPlayerInfo(uidMurder);
	if (pMurderPerson == NULL)
		return;

	// 死亡者是魔王，则召唤魔王随从NPC,且击杀者成为新的魔王
	if (uidDead == m_nCurDevilKing)
	{
		// 创建随从
		createDevilKingMonster(uidMurder);

		// 新的魔王
		onNewDevilKing(uidMurder);

		// 广播
		char szBuf[128] = { 0 };
		ssprintf_s(szBuf, sizeof(szBuf), a2utf8("%s玩家已经成为新的魔王，前往击杀可获得随从"), pMurderPerson->szName);
		g_EHelper.showSystemMessageToChatBox(m_pWar->getWarSceneID(), CHAT_TIP_CHATBOX_CUSTOMER_TIP, szBuf, __LINE__);
	}
}

void CDevilKingMgr::bcastDevilKingPos()
{
	if (m_isClose)
		return;

	if (m_nCurDevilKing == INVALID_UID)
		return;

	// 广播小地图上魔王的位置
	// 添加视野全局可见buff
	SWarDevilKing* pDevilKingScheme = getDevilKingScheme();
	if(pDevilKingScheme == NULL)
		return;

	AddBuff(m_nCurDevilKing, pDevilKingScheme->nDevilKingBuff, 1, m_nCurDevilKing);

    WarningLn(__FUNCTION__": add visible buff, uid="<< m_nCurDevilKing);
}

void CDevilKingMgr::createDevilKingMonster(UID uidMurder)
{
	if (m_isClose)
		return;

	SWPerson* pPerson = m_pWar->getPlayerInfo(uidMurder);
	if (pPerson == NULL)
		return;

	// 击杀者召唤出一个魔王NPC
	CreateNpcContext NpcContext;
	NpcContext.nCamp = pPerson->nCamp;
	NpcContext.nWarID = m_pWar->getWarID();
	NpcContext.uidCreator = uidMurder;
	NpcContext.nLevel = pPerson->nLevel;
	NpcContext.nAIID = -1;
	
	// 怪物ID
	int nMonsterID = getDevilKingMonsterID(pPerson->nHeroID);
	if (nMonsterID <= 0)
	{
		return;
	}

	// 怪物位置(人的位置）
	Vector3 loc = getLocation(uidMurder);

	// 创建NPC
	SceneLogicHelper helper(m_pWar->getWarSceneID());
	ISceneLogicService* pSceneLogicService = helper.m_ptr;
	if (pSceneLogicService == NULL)
	{
		return;
	}
	INPCService* pNpcService = pSceneLogicService->getNPCService();
	if (pNpcService == NULL)
	{
		return;
	}
	pNpcService->createNPC(nMonsterID, loc, NpcContext);

#ifdef PRINT_DEVILKINGMGR_DEBUG
	WarningLn(__FUNCTION__":" << a2utf8("创建魔王NPC,") << pPerson->szName << ",nMonsterID=" << nMonsterID);
#endif
}


int CDevilKingMgr::getDevilKingMonsterID(int nHeroID)
{
	// 怪物ID
	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeCenter == NULL");
		return -1;
	}
	ISchemeWarDevilKingNPC* ISchemeWarDevilKingNPC = pSchemeCenter->getSchemeWarDevilKingNPC();
	if (ISchemeWarDevilKingNPC == NULL)
	{
		return -1;
	}
	SWarDevilKingNPC* pWarDevilKingNPC = ISchemeWarDevilKingNPC->getWarDevilKingNPC(nHeroID);
	if (pWarDevilKingNPC == NULL)
	{
		return -1;
	}

	return pWarDevilKingNPC->nMonsterID;
}

SWarDevilKing* CDevilKingMgr::getDevilKingScheme()
{
	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeCenter == NULL");
		return NULL;
	}
	ISchemeWarDevilKing* ISchemeWarDevilKing = pSchemeCenter->getSchemeWarDevilKing();
	if (ISchemeWarDevilKing == NULL)
	{
		return NULL;
	}

	return ISchemeWarDevilKing->getWarDevilKing();
}

int CDevilKingMgr::getDevilKingKillNumCond()
{
	SWarDevilKing* pDevilKingScheme = getDevilKingScheme();
	if (pDevilKingScheme == NULL)
		return INT_MAX;

	return pDevilKingScheme->nKillNumCond;
}

int CDevilKingMgr::getBcastDevilKingPosInternal()
{
	SWarDevilKing* pDevilKingScheme = getDevilKingScheme();
	if (pDevilKingScheme == NULL)
		return INT_MAX;

	return pDevilKingScheme->nBcastTime;
}

int CDevilKingMgr::getCloseTime()
{
	SWarDevilKing* pDevilKingScheme = getDevilKingScheme();
	if (pDevilKingScheme == NULL)
		return INT_MAX;

	return pDevilKingScheme->nCloseTime;
}

void CDevilKingMgr::sendDevilKingToAll()
{
	SWar2ViewMsgDevilKingUpdate msg;
	msg.uidDevilKing = m_nCurDevilKing;
	msg.isReconnect = 0;

	m_pWar->broadCastDataToAllPlayer(WAR2VIEW_MSG_BATTLEWAR_DEVILKING_UPDATE, (LPCSTR)&msg, sizeof(msg), BCASTALLCAMP, true);
}

void CDevilKingMgr::sendDevilKingToClient(UID uid)
{
	SWar2ViewMsgDevilKingUpdate msg;
	msg.uidDevilKing = m_nCurDevilKing;
	msg.isReconnect = 1;

	g_EHelper.sendMessageToClientWar(uid, WAR2VIEW_MSG_BATTLEWAR_DEVILKING_UPDATE, (LPCSTR)&msg, sizeof(msg), true);
}
