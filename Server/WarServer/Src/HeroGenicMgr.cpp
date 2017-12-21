/*******************************************************************
** 文件名: HeroGenicMgr.cpp
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/11/30
** 版  本: 1.0
** 描  述: 英雄基因玩法
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/
#include "StdAfx.h"
#include "HeroGenicMgr.h"
#include "IWar.h"
#include "ExternalHelper.h"
#include "ITimerService.h"
#include "WarDef_Internal.h"

#define PRINT_DEVILKINGMGR_DEBUG

CHeroGenicMgr::CHeroGenicMgr(IWar* pWar)
	: m_pWar(pWar)
	, m_nMatchTypeID(0)
{
}

CHeroGenicMgr::~CHeroGenicMgr()
{
	m_pWar = NULL;
}

void CHeroGenicMgr::OnTimer(unsigned long dwTimerID)
{
	if (m_pWar == NULL || m_pWar->isEnd())
		return;

}

void CHeroGenicMgr::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	switch (wEventID)
	{
	case EVENT_GAME_ACTOR_CONTINUE_PLAY:
		{

			if (nLen != sizeof(event_entity_continue_play))
			{
				ErrorLn(__FUNCTION__": nLen != sizeof(event_entity_continue_play) !" << ", nLen = " << nLen);
				return;
			}
			event_entity_continue_play* pEvent = (event_entity_continue_play*)pszContext;

			SWPerson* pPerson = m_pWar->getPlayerInfoByPDBIU(pEvent->dwDBID);
			if (pPerson == NULL)
				return;

			// 发送收集的基因信息
			sendGenicInfoToClient(pPerson->uID);
		}
		break;
	default:break;
	}
}

bool CHeroGenicMgr::Init(int nMatchType)
{
	if (m_pWar == NULL)
	{
		return false;
	}

	m_nMatchTypeID = nMatchType;

	g_EHelper.Subscribe(m_pWar->getWarSceneID(), this, EVENT_GAME_ACTOR_CONTINUE_PLAY, TYPE_PLAYER_ROLE, 0, __FUNCTION__);

	return true;
}

void CHeroGenicMgr::Release()
{
	g_EHelper.UnSubscribe(m_pWar->getWarSceneID(), this, EVENT_GAME_ACTOR_CONTINUE_PLAY, TYPE_PLAYER_ROLE, 0);

	m_pWar = NULL;

	delete this;
}

void CHeroGenicMgr::onWarRealStart()
{
	// 启动关闭魔王玩法的定时器
}

void CHeroGenicMgr::onWarEnd()
{
}

void CHeroGenicMgr::onNewGenic(UID uid, int nAddHeroID, int nAddSkinID, const char * szDeadName)
{
	if (nAddHeroID <= 0)
	{
		ErrorLn(__FUNCTION__":Invalid HeroID, " << nAddHeroID);
		return;
	}

	SWPerson* pPerson = m_pWar->getPlayerInfo(uid);
	if (pPerson == NULL)
		return;

	// 是否与本体基因相同
	if (nAddHeroID == pPerson->nHeroID)
	{
		g_EHelper.showSystemMessageToClient(pPerson->uPDBID, CHAT_TIP_WAR_BATTLEWAR_GENIC_MUTEX);
		return;
	}

	// 是否重复收集
	for (int i = 0; i < GENIC_MAX_SIZE; ++i)
	{
		if (pPerson->GenicList[i].nHeroID == nAddHeroID)
		{
			g_EHelper.showSystemMessageToClient(pPerson->uPDBID, CHAT_TIP_WAR_BATTLEWAR_GENIC_ALREADYOWNED);
			return;
		}
	}

	// 获得基因提示
	char szBuf[64] = { 0 };
	ssprintf_s(szBuf, sizeof(szBuf), "%s;%s", szDeadName, szDeadName);
	g_EHelper.showSystemMessageToClient(pPerson->uPDBID, CHAT_TIP_WAR_BATTLEWAR_GENIC_OWNEDTIP, szBuf);

	// 更新新的基因
	int i = 0;
	for (i = 0; i < GENIC_MAX_SIZE; ++i)
	{
		if (pPerson->GenicList[i].nHeroID == 0)
		{
			pPerson->GenicList[i].nHeroID = nAddHeroID;
			pPerson->GenicList[i].nSkinID = nAddSkinID;
			break;
		}
	}
	if (i == GENIC_MAX_SIZE)
	{
		// 如果当前基因数组没有空余位置，则覆盖第一个基因
		pPerson->GenicList[0].nHeroID = nAddHeroID;
		pPerson->GenicList[0].nSkinID = nAddSkinID;
	}

	// 同步到客户端
	sendGenicInfoToClient(uid);
}

void CHeroGenicMgr::changeVocation(UID uid, int nSlotID)
{
	if (nSlotID < 0 || nSlotID >= GENIC_MAX_SIZE)
		return;

	SWPerson* pPerson = m_pWar->getPlayerInfo(uid);
	if (pPerson == NULL)
		return;

	// 只能存活状态才能进化改变职业
	if (pPerson->lifeState != ELS_Alive)
		return;

	SGenicInfo& genicInfo = pPerson->GenicList[nSlotID];
	if (genicInfo.nHeroID <= 0)
		return;

	ActorHelper actorHelper(uid);
	IActorService* pActorService = actorHelper.m_ptr;
	if (pActorService == NULL)
		return;

	if (pActorService->changeVoc(genicInfo.nHeroID, genicInfo.nSkinID) == false)
	{
		ErrorLn(__FUNCTION__":" << "Change vocation failed.uid=" << uid << ",nSlotID=" << nSlotID);
		return;
	}
	// 增加频死保护buff
	AddBuff(uid, 3069, 1, uid);

	// 更新英雄ID和皮肤ID
	pPerson->nHeroID = genicInfo.nHeroID;
	pPerson->nHeroSkinID = genicInfo.nSkinID;
	m_pWar->bcastPersonInfo(uid, EWPTUT_VOCATION);

	// 清空并同步到客户端
	genicInfo.nHeroID = 0;
	genicInfo.nSkinID = 0;
	sendGenicInfoToClient(uid);

	// 清空已经激活的天赋
	memset(pPerson->TalentList, 0, sizeof(pPerson->TalentList));
}

void CHeroGenicMgr::sendGenicInfoToClient(UID uid)
{
	SWPerson* pPerson = m_pWar->getPlayerInfo(uid);
	if (pPerson == NULL)
		return;

	SWar2ViewMsgGenicUpdate msg;
	for (int i = 0; i < GENIC_MAX_SIZE; ++i)
	{
		msg.nHeroIDList[i] = pPerson->GenicList[i].nHeroID;
		msg.nSkinIDList[i] = pPerson->GenicList[i].nSkinID;
	}

	m_pWar->sendDataToClient(uid, WAR2VIEW_MSG_BATTLEWAR_GENIC_UPDATE, &msg, sizeof(msg), true);
}