/*******************************************************************
** 文件名: WarCheckBadBehavior.cpp
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/6/19
** 版  本: 1.0
** 描  述: 战场检测恶劣行为：挂机,消极游戏等
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/

#include "StdAfx.h"
#include "WarCheckBadBehavior.h"
#include "IWar.h"
#include "ICenterConnectorService.h"
#include "GatewayServiceDef.h"
#include "MatchSchemeDef.h"
#include "WarDef_Internal.h"

#define PRINT_CHECKBADBEHAVIOR_DEBUG

CWarCheckBadBehavior::CWarCheckBadBehavior(IWar* pWar)
	:m_pWar(pWar)
{
}

CWarCheckBadBehavior::~CWarCheckBadBehavior()
{
	m_pWar = NULL;
}

void CWarCheckBadBehavior::OnTimer(unsigned long dwTimerID)
{
	if (m_pWar == NULL || m_pWar->isEnd())
		return;
}

bool CWarCheckBadBehavior::Init(int nMatchType)
{
	if (m_pWar == NULL)
		return false;

	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return false;
	ISchemeMatchRankLimitInfo* pMatchRankLimit = pSchemeCenter->getSchemeMatchRankLimit();
	if (pMatchRankLimit == NULL)
		return false;
	SSchemePunish* pSchemePunish = pMatchRankLimit->getPunishInfo(emPunishmentType_War);
	if (pSchemePunish == NULL)
		return false;

	if (pSchemePunish->needPunish(nMatchType) == false)
		return false;

	return true;
}

void CWarCheckBadBehavior::Release()
{
	delete this;
}

void CWarCheckBadBehavior::onWarRealStart()
{
}

void CWarCheckBadBehavior::onWarEnd()
{
}


void CWarCheckBadBehavior::onMessage(BYTE byKeyAction, UID uid, void * data, size_t len)
{
	if (m_pWar == NULL)
		return;

	switch (byKeyAction)
	{
	case CS_MSG_WAR_BOTTING_CANCEL:
		{
			// 已经受到挂机惩罚
			if (isBottingPunish(uid))
				break;

			// 游戏已经结束
			if (m_pWar->isEnd())
				break;

			setBottingState(uid, false);
		}
		break;
	case CS_MSG_WAR_BOTTING_STATE:		// 客户端在挂机状态
		{
			// 已经受到挂机惩罚
			if (isBottingPunish(uid))
				break;

			// 游戏已经结束
			if (m_pWar->isEnd())
				break;

			increaseBottingCount(uid);
		}
		break;
	default:
		break;
	}
}

void CWarCheckBadBehavior::sendBottingTipToClient(UID uid, BYTE byType)
{
	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.byKeyModule = MSG_MODULEID_WAR;
	head.byKeyAction = SC_MSG_WAR_BOTTING_NOTIFY;

	msg_war_botting_notify msg;
	msg.byType = byType;

	obuf obuffData;

	obuffData << head << msg;
	send2Client(uid, PACKAGE_PTR(new std::string(obuffData.data(), obuffData.size())));
}

bool CWarCheckBadBehavior::isBottingPunish(UID uid)
{
	if (m_pWar == NULL)
		return false;

	SWPerson* pPerson = m_pWar->getPlayerInfo(uid);
	if (pPerson == NULL)
		return false;

	int nOldCount = pPerson->nBottingCount;

	return nOldCount >= BOTTING_COUNT_MAX;
}



// 挂机次数+1
void CWarCheckBadBehavior::increaseBottingCount(UID uid)
{
	if (m_pWar == NULL)
		return;

	SWPerson* pPerson = m_pWar->getPlayerInfo(uid);
	if (pPerson == NULL)
		return;

	pPerson->nBottingCount++;

	// 设置挂机状态
	setBottingState(uid, true);

	if (pPerson->nBottingCount >= BOTTING_COUNT_MAX)
	{
		//ChatHelper chatHelper;
		//if (chatHelper.m_ptr != NULL)
		//{
		//	chatHelper.m_ptr->broadcastSystemMessage(CHAT_BROADCASTTYPE_SINGLE, pPerson->uPDBID, CHAT_TIP_WAR_BADBEHAVIOR_BOTTING_KICKOUT);
		//}

		sendBottingTipToClient(uid, EWBNT_Punish);

		// 踢下线
		gServerGlobal->getGatewayAcceptorService()->kickOutClient(pPerson->ulClient, EKickoutClient_Reason_GMKickout);
	}
	else
	{
		sendBottingTipToClient(uid, EWBNT_Normal);
	}

}

void CWarCheckBadBehavior::setBottingState(UID uid, bool isBotting)
{
	m_IsBottingMap[uid] = isBotting;
}

void CWarCheckBadBehavior::warEndBottingRec()
{
	ICenterConnectorService * pCenterConnectorService = gServerGlobal->getCenterConnectorService();
	if (pCenterConnectorService == NULL)
	{
		return;
	}

	for (map<UID, bool>::iterator iter = m_IsBottingMap.begin(); iter != m_IsBottingMap.end(); ++iter)
	{
		if(!iter->second)
			continue;

		// 本局被认定为挂机,则发送惩罚信息到社会服
		ActorHelper actorHelper(iter->first,ACTOR_UID);
		IActorService *pActorService = actorHelper.m_ptr;
		if (pActorService == NULL)
		{
		   continue;
		}
		SMsgPunishActor_SO Msgdata;
		Msgdata.bAdd = true;
		Msgdata.byPunishType = emPunishmentType_War;
		Msgdata.dwActorID = pActorService->getProperty_Integer(PROPERTY_ID);
		Msgdata.dwFromWorldID = pActorService->getFromGameWorldID();
        Msgdata.nMatchTypeID = m_pWar->getMatchTypeID();

		// 子消息相关
		SMsgMatchDataSubMsg subMsg;
		subMsg.dwMsgCode = SO_MSG_MATCH_PUNISH_ACTOR;

		// 数据打包
		obuf256 obMsg;
		obMsg<<subMsg<<Msgdata;

		pCenterConnectorService->sendToSocial(MSG_MODULEID_MATCH, MSG_MATCH_SERVICE_MODULEMSG, (const char*)obMsg.data(), obMsg.size());
	}
}
