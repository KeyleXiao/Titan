/*******************************************************************
** 文件名:	ExternalHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/3/29
** 版  本:	1.0
** 描  述:	辅助功能类
********************************************************************/
#include "stdafx.h"
#include "ExternalHelper.h"
#include "ChatHelper.h"
#include "MobaGoodsSchemeDef.h"
#include "MatchSchemeDef.h"
#include "ITimerService.h"
#include "EntityDef.h"
#include "EntityHelper.h"
#include "WarMessageDef.h"

CExternalHelper	g_EHelper;

// 构造函数
CExternalHelper::CExternalHelper(void)
{

}

// 析构函数
CExternalHelper::~CExternalHelper(void)
{

}

bool CExternalHelper::SetTimer(DWORD dwTimerID, DWORD dwInterval, ITimerHandler *pHandler, DWORD dwCallTimes, const char *debugInfo)
{
	ITimerService *pTimerAxis = gServerGlobal->getTimerService();
	if (pTimerAxis == NULL)
	{
		return false;
	}

	return pTimerAxis->SetTimer(dwTimerID, dwInterval, pHandler, dwCallTimes, debugInfo);
}


/** 销毁定时器
*/
bool CExternalHelper::KillTimer(DWORD dwTimerID, ITimerHandler *pHandler)
{
	ITimerService* pTimerAxis = gServerGlobal->getTimerService();
	if (pTimerAxis == NULL)
	{
		return false;
	}

	return pTimerAxis->KillTimer(dwTimerID, pHandler);
}
// 在“战场频道”显示提示文字
void CExternalHelper::showSystemMessage(int nSceneID, EMChatTipID nTipID, LPCSTR szParam, WORD nReserveData /*= 0*/)
{
    ChatHelper chatHelper;
    IChatSceneService *pChatSceneService = chatHelper.m_ptr;
    if (pChatSceneService == NULL)
    {
        return;
    }

    pChatSceneService->broadcastSystemMessage(CHAT_BROADCASTTYPE_WARSCENE, nSceneID, nTipID, szParam, 0, nReserveData);
}

void CExternalHelper::showSystemMessageToClient(PDBID pdbid, EMChatTipID nTipID, LPCSTR szParam)
{
	ChatHelper chatHelper;
	IChatSceneService *pChatSceneService = chatHelper.m_ptr;
	if (pChatSceneService == NULL)
	{
		return;
	}

	pChatSceneService->broadcastSystemMessage(CHAT_BROADCASTTYPE_SINGLE, pdbid, nTipID, szParam);
}

void CExternalHelper::showSystemMessageToChatBox(int nSceneID, EMChatTipID nTipID, LPCSTR szParam, WORD nReserveData /*= 0*/)
{
	ChatHelper chatHelper;
	IChatSceneService *pChatSceneService = chatHelper.m_ptr;
	if (pChatSceneService == NULL)
	{
		return;
	}

	pChatSceneService->broadcastSystemMessageToChatBox(CHAT_BROADCASTTYPE_WARSCENE, nSceneID, nTipID, CHAT_CHANNEL_WARSCENE,szParam, 0, nReserveData);
}

bool CExternalHelper::isLowGoods(int nGoodID)
{
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return false;
	}
	ISchemeMobaGoods* pISchemeMobaGoods = gServerGlobal->getSchemeCenter()->getSchemeMobaGoods();
	if (pISchemeMobaGoods == NULL)
	{
		return false;
	}

	SSchemeMobaGoods* pGood = pISchemeMobaGoods->getSchemeMobaGoodsInfo(nGoodID);
	if (pGood == NULL)
	{
		return false;
	}

	return pGood->nPreciousDegree == MGPD_LOW;
}

bool CExternalHelper::isHighGoods(int nGoodID)
{
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return false;
	}
	ISchemeMobaGoods* pISchemeMobaGoods = gServerGlobal->getSchemeCenter()->getSchemeMobaGoods();
	if (pISchemeMobaGoods == NULL)
	{
		return false;
	}

	SSchemeMobaGoods* pGood = pISchemeMobaGoods->getSchemeMobaGoodsInfo(nGoodID);
	if (pGood == NULL)
	{
		return false;
	}

	return pGood->nPreciousDegree == MGPD_HIGH;
}

std::string CExternalHelper::getGoodsName(int nGoodID)
{
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return "";
	}
	ISchemeMobaGoods* pISchemeMobaGoods = gServerGlobal->getSchemeCenter()->getSchemeMobaGoods();
	if (pISchemeMobaGoods == NULL)
	{
		return "";
	}

	SSchemeMobaGoods* pGood = pISchemeMobaGoods->getSchemeMobaGoodsInfo(nGoodID);
	if (pGood == NULL)
	{
		return "";
	}

	return pGood->szName;
}

bool CExternalHelper::isCanUseHeroStarLv(int nMatchType)
{
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return false;
	ISchemeMatchRoom* pISchemeMatchRoom = pSchemeCenter->getSchemeMatchRoom();
	if (pISchemeMatchRoom == NULL)
		return false;
	SSchemeMatchRoom* pSchemeMatchRoom = pISchemeMatchRoom->getSchemeMatchRoomByMatchTypeID(nMatchType);
	if (pSchemeMatchRoom == NULL)
		return false;

	return pSchemeMatchRoom->m_bShowHeroStarInfo;
}

void CExternalHelper::sendEntityMessage(UID uid, BYTE byKeyAction, LPCSTR pszData, size_t nDataLen)
{
	if (isInvalidUID(uid))
		return;

	// 通知客户端
	obuf256 obufData;

	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.byKeyModule = MSG_MODULEID_ENTITY;
	head.byKeyAction = byKeyAction;

	// 实体类统一用子头来用于转向部件
	SMsgEntityHead entityHead;
	entityHead.uidMaster = uid;				// 实体UID
	entityHead.byPartID = PART_WAR;			// 部件ID，ENTITY_PART_MAX时将由实体来处理

	obufData << head << entityHead;
	obufData.push_back(pszData, nDataLen);

	// 发送消息
	send2Client(uid, PACKAGE_PTR(new string(obufData.data(), obufData.size())));
}

void CExternalHelper::sendMessageToClientWar(UID uid, BYTE byKeyAction, LPCSTR pszData, size_t nDataLen, bool bToView)
{
	if (isInvalidUID(uid))
		return;

	// 通知客户端
	obuf256 obufData;

	if (bToView)
	{
		fillDataToClientView(obufData, byKeyAction, (LPCSTR)pszData, nDataLen);
	}
	else
	{
		SNetMsgHead head;
		head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
		head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
		head.byKeyModule = MSG_MODULEID_WAR;
		head.byKeyAction = byKeyAction;

		obufData << head;
		obufData.push_back(pszData, nDataLen);
	}

	// 发送消息
	send2Client(uid, PACKAGE_PTR(new string(obufData.data(), obufData.size())));
}

void CExternalHelper::fillDataToClientView(obuf & ob, BYTE byKeyAction, LPCSTR pszData, size_t nDataLen)
{
	msg_war_server_toview_msg msg;
	msg.nKeyAction = byKeyAction;

	SNetMsgHead head;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byKeyModule = MSG_MODULEID_WAR;
	head.byKeyAction = SC_MSG_WAR_SERVER_TOVIEW_MSG;

	ob << head;
	ob << msg;
	ob.push_back(pszData, nDataLen);
}

void CExternalHelper::Subscribe(int nSceneID, IEventExecuteSink* pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPSTR pszDesc)
{
	SceneLogicEventHelper eventHelper(nSceneID);
	rkt::IEventEngine* pEventEngine = eventHelper.m_ptr;
	if (pEventEngine != NULL)
	{
		pEventEngine->Subscibe(pSink, wEventID, bSrcType, dwSrcID, pszDesc);
	}
}

void CExternalHelper::UnSubscribe(int nSceneID, IEventExecuteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID)
{
	SceneLogicEventHelper eventHelper(nSceneID);
	rkt::IEventEngine* pEventEngine = eventHelper.m_ptr;
	if (pEventEngine != NULL)
	{
		pEventEngine->UnSubscibe(pSink, wEventID, bSrcType, dwSrcID);
	}
}

void CExternalHelper::FireExecute(int nSceneID, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	SceneLogicEventHelper eventHelper(nSceneID);
	rkt::IEventEngine* pEventEngine = eventHelper.m_ptr;
	if (pEventEngine != NULL)
	{
		pEventEngine->FireExecute(wEventID, bSrcType, dwSrcID, pszContext, nLen);
	}
}

SWarSafetyZone * CExternalHelper::getWarSafetyZoneInfo(int nRound)
{
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return NULL;
	ISchemeWarSafetyZone* pSchemeWarSafetyZone = pSchemeCenter->getSchemeWarSafetyZone();
	if (pSchemeWarSafetyZone == NULL)
		return NULL;

	return pSchemeWarSafetyZone->getWarSafetyZone(nRound);
}

SEntityXml_Server *CExternalHelper::getEntityXml(void)
{
	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return NULL;
	}

	ISchemeEntity *pSchemeEntity = pSchemeCenter->getSchemeEntity();
	if (pSchemeEntity == NULL)
	{
		return NULL;
	}

	return pSchemeEntity->getEntityXml();
}

ETalentDropType CExternalHelper::talentChangeType2TalentDropType(ENUM_TALENT_CHANGE_TYPE changeType)
{
	switch (changeType)
	{
	case ENUM_TALENT_EFFECT: return ETALENTDROPTYPE_EFFECT;
	case ENUM_TALENT_SELL: return ETALENTDROPTYPE_SELLEQUIP;
	case ENUM_TALENT_KILLMONSTER: return ETALENTDROPTYPE_MONSTER;
	case ENUM_TALENT_KILLSOLIDER: return ETALENTDROPTYPE_SOLIDER;
	case ENUM_TALENT_KILLPLAYER: return ETALENTDROPTYPE_PLAYER;
	case ENUM_TALENT_KILLTOWER: return ETALENTDROPTYPE_TOWER;
	case ENUM_TALENT_KILLBOSS: return ETALENTDROPTYPE_BOSS;
	case ENUM_TALENT_WARTIMER: return ETALENTDROPTYPE_NORMAL;
	default:return ETALENTDROPTYPE_MAX;
	}
}

ENUM_TALENT_CHANGE_TYPE CExternalHelper::talentDropType2TalentChangeType(ETalentDropType dropType)
{
	switch (dropType)
	{
	case  ETALENTDROPTYPE_EFFECT: return ENUM_TALENT_EFFECT;
	case  ETALENTDROPTYPE_SELLEQUIP: return ENUM_TALENT_SELL;
	case  ETALENTDROPTYPE_MONSTER: return ENUM_TALENT_KILLMONSTER;
	case  ETALENTDROPTYPE_SOLIDER: return ENUM_TALENT_KILLSOLIDER;
	case  ETALENTDROPTYPE_PLAYER: return ENUM_TALENT_KILLPLAYER;
	case  ETALENTDROPTYPE_TOWER: return ENUM_TALENT_KILLTOWER;
	case  ETALENTDROPTYPE_BOSS: return ENUM_TALENT_KILLBOSS;
	case  ETALENTDROPTYPE_NORMAL: return ENUM_TALENT_WARTIMER;
	default:return ENUM_TALENT_MAX;
	}
}
