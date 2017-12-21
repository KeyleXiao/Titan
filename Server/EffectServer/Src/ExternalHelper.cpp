/*******************************************************************
** 文件名:	ExternalHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/10/2015
** 版  本:	1.0
** 描  述:	辅助功能类
********************************************************************/

#include "StdAfx.h"
#include "ExternalHelper.h"
#include "IServerGlobal.h"
#include "IServiceContainer.h"
#include "IGatewayAcceptorService.h"
#include "IFramework.h"
#include "IServiceMgr.h"
#include "IWarPart.h"
#include "IWarManager.h"
#include "EntityHelper.h"
#include "IEntityAOI.h"
#include "ChatHelper.h"
#include "IChatSceneService.h"
#include "IPlayerKillPart.h"
#include "IMovePart.h"
#include "PathFindHelper.h"
#include "IPathFindService.h"
#include "ISpell.h"

CExternalHelper				g_EHelper;

CExternalHelper::CExternalHelper()
{
}


CExternalHelper::~CExternalHelper()
{
}


/** 启用定时器
*/
bool CExternalHelper::SetTimer(DWORD dwTimerID, DWORD dwInterval, ITimerHandler *pHandler, DWORD dwCallTimes, const char *debugInfo, bool bCheck)
{
	ITimerService *pTimerAxis = gServerGlobal->getTimerService();
	if (pTimerAxis == NULL)
	{
		return false;
	}

    if (bCheck && dwInterval < 500 && dwCallTimes == INFINITY_CALL)
    {
        WarningLn("debugInfo="<<debugInfo<<",time short dwInterval="<<dwInterval);
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


/** 订阅投票事件
@param   pSink : 回调sink
@param   wEventID : 事件ID
@param	 pDesc : 订阅者描述，用来调试程序
@return  成功返回true,失败返回false
*/
bool CExternalHelper::Subscibe(__IEntity *pEntity, IEventVoteSink *pSink, EventKey eventID, LPCSTR pszDesc)
{
	if (pEntity == NULL)
	{
		return false;
	}

	__IEntityEvent *pEntityEvent = pEntity->getEntityEvent();
	if (pEntityEvent == NULL)
	{
		return false;
	}

	return pEntityEvent->Subscibe(pSink, eventID, pszDesc);
}


/** 订阅执行事件
@param   pSink : 回调sink
@param   wEventID : 事件ID
@param	 pDesc : 订阅者描述，用来调试程序
@return  成功返回true,失败返回false
*/
bool CExternalHelper::Subscibe(__IEntity *pEntity, IEventExecuteSink *pSink, EventKey eventID, LPCSTR pszDesc)
{
	if (pEntity == NULL)
	{
		return false;
	}

	__IEntityEvent *pEntityEvent = pEntity->getEntityEvent();
	if (pEntityEvent == NULL)
	{
		return false;
	}

	return pEntityEvent->Subscibe(pSink, eventID, pszDesc);
}


/** 取消订阅投票事件
@param   pSink : 回调sink
@param   wEventID : 事件ID
@return  成功返回true,失败返回false
*/
bool CExternalHelper::UnSubscibe(__IEntity *pEntity, IEventVoteSink *pSink, EventKey eventID)
{
	if (pEntity == NULL)
	{
		return false;
	}

	__IEntityEvent *pEntityEvent = pEntity->getEntityEvent();
	if (pEntityEvent == NULL)
	{
		return false;
	}

	return pEntityEvent->UnSubscibe(pSink, eventID);
}


/** 取消订阅执行事件
@param   pSink : 回调sink
@param   wEventID : 事件ID
@return  成功返回true,失败返回false
*/
bool CExternalHelper::UnSubscibe(__IEntity *pEntity, IEventExecuteSink *pSink, EventKey eventID)
{
	if (pEntity == NULL)
	{
		return false;
	}

	__IEntityEvent *pEntityEvent = pEntity->getEntityEvent();
	if (pEntityEvent == NULL)
	{
		return false;
	}

	return pEntityEvent->UnSubscibe(pSink, eventID);
}


/** 发送实体数据
*/
void CExternalHelper::sendEntityMessage(UID uid, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData, size_t nDataLen)
{
	if (isInvalidUID(uid))
	{
		return;
	}

	// 通知客户端
	obuf256 obufData;

	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
	head.byDestEndPoint= MSG_ENDPOINT_SCENE;	
	head.byKeyModule   = MSG_MODULEID_ENTITY;	
	head.byKeyAction   = byKeyAction;

	// 实体类统一用子头来用于转向部件
	SMsgEntityHead entityHead;
	entityHead.uidMaster = uid;				// 实体UID
	entityHead.byPartID = byPartID;			// 部件ID，ENTITY_PART_MAX时将由实体来处理

	obufData << head << entityHead;
	obufData.push_back(pszData,nDataLen);

	// 发送消息
	handleMessage(uid, obufData.data(), (int)obufData.size());
}


/** 发送数据
*/
bool CExternalHelper::broadCastInAOI(__IEntity *pEntity, Vector3 ptTargetTile, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData, size_t nDataLen)
{
	if (pEntity == NULL || pszData == NULL || nDataLen == 0)
	{
		return false;
	}

	obuf o;
	SNetMsgHead head;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.bySrcEndPoint  = MSG_ENDPOINT_SCENE;
	head.byKeyModule    = MSG_MODULEID_ENTITY;
	head.byKeyAction    = byKeyAction;

	// 实体类统一用子头来用于转向部件
	SMsgEntityHead entityHead;
	entityHead.uidMaster = pEntity->getUID();	// 实体UID
	entityHead.byPartID = byPartID;			// 部件ID，ENTITY_PART_MAX时将由实体来处理

	o << head << entityHead;
	o.push_back(pszData,nDataLen);

	Vector3 vLoc = pEntity->getLocation();
    __IEntityAOI* pEntityAOIPart = (__IEntityAOI*)pEntity->getEntityPart(PART_AOI);
    if (pEntityAOIPart != NULL)
    {
        // 填0是默认范围
        pEntityAOIPart->broadcast(vLoc, 0, BROADCAST_DO_ACTION,std::string(o.data(),o.size()), __FUNCTION__, __LINE__);
    }

	return true;
}


/** 发送数据
*/
bool CExternalHelper::broadCastDataToAllPlayer(__IEntity *pEntity, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData, size_t nDataLen)
{
	if (pEntity == NULL || pszData == NULL || nDataLen == 0)
	{
		return false;
	}

    SceneHelper sceneHelper(pEntity->getSceneID());
    IGameSceneService * pScene = sceneHelper.m_ptr;
    if (pScene == NULL)
    {
        return false;
    }

	obuf o;
	SNetMsgHead head;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.bySrcEndPoint  = MSG_ENDPOINT_SCENE;
	head.byKeyModule    = MSG_MODULEID_ENTITY;
	head.byKeyAction    = byKeyAction;

	// 实体类统一用子头来用于转向部件
	SMsgEntityHead entityHead;
	entityHead.uidMaster = pEntity->getUID();	// 实体UID
	entityHead.byPartID = byPartID;			// 部件ID，ENTITY_PART_MAX时将由实体来处理

	o << head << entityHead;
	o.push_back(pszData,nDataLen);

	pScene->broadcast_all(BROADCAST_DO_ACTION,std::string(o.data(),o.size()));

	return true;
}

/** 显示提示文字
*/
void CExternalHelper::showSystemMessage(__IEntity *pEntity, EMChatTipID nTipID, LPCSTR szParam)
{
    if (pEntity == NULL)
    {
        return;
    }

    if (pEntity->getType() != TYPE_PLAYER_ROLE)
    {
        return;
    }

    ChatHelper chatHelper;
    IChatSceneService *pChatSceneService = chatHelper.m_ptr;
    if (pChatSceneService == NULL)
    {
        return;
    }

	pChatSceneService->broadcastSystemMessage(CHAT_BROADCASTTYPE_SINGLE, pEntity->getIntProperty(PROPERTY_ID), nTipID, szParam);
}


/** 选择目标
*/
bool CExternalHelper::chooseTarget(__IEntity *pEntity, int nTargetFilter, UID uidTarget)
{
    if (pEntity == NULL)
    {
        return false;
    }

    __IPlayerKillPart *pPlayerKillPart = (__IPlayerKillPart *)pEntity->getEntityPart(PART_PK);
    if (pPlayerKillPart == NULL)
    {
        return false;
    }

    // 检测目标
    return pPlayerKillPart->chooseTarget(nTargetFilter, uidTarget);
}

// 给群体怪物增加BUFF
void CExternalHelper::BatchAddBuff(UID* uidArray, DWORD dwArrayCount, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd, DWORD dwTime, void *pBuffContext, int nContextLen)
{
    if (dwArrayCount <= 0)
    {
        return;
    }

    UID uid = uidArray[0];
    MonsterHelper helper(uid);
    INPCService *pNpcService = helper.m_ptr;		
    if (pNpcService == NULL)
    {
        return;
    }
    pNpcService->BatchAddBuff(uidArray,dwArrayCount,dwBuffID,dwBuffLevel,uidAdd,dwTime,pBuffContext,nContextLen); 
}

// 给群体怪物移除BUFF
void CExternalHelper::BatchRemoveBuff(UID* uidArray, DWORD dwArrayCount, DWORD dwBuffID, UID uidAdd, UID uidRemove)
{
    if (dwArrayCount <= 0)
    {
        return;
    }

    UID uid = uidArray[0];
    MonsterHelper helper(uid);
    INPCService *pNpcService = helper.m_ptr;		
    if (pNpcService == NULL)
    {
        return;
    }
    pNpcService->BatchRemoveBuff(uidArray,dwArrayCount,dwBuffID,uidAdd,uidRemove); 
}

// 给群体怪物带检测指定BUFFID和BUFF类型来增加BUFF
void CExternalHelper::BatchCheckAddBuff(UID* uidArray, DWORD dwArrayCount, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd, DWORD dwCheckBuff, DWORD dwCheckLevel, DWORD dwCheckBuffType, DWORD dwTime, void *pBuffContext, int nContextLen)
{
    if (dwArrayCount <= 0)
    {
        return;
    }

    UID uid = uidArray[0];
    MonsterHelper helper(uid);
    INPCService *pNpcService = helper.m_ptr;		
    if (pNpcService == NULL)
    {
        return;
    }
    pNpcService->BatchCheckAddBuff(uidArray,dwArrayCount,dwBuffID,dwBuffLevel,uidAdd,dwCheckBuff,dwCheckLevel,dwCheckBuffType,dwTime,pBuffContext,nContextLen, true); 
}

void CExternalHelper::Transport(__IEntity *pEntity, Vector3 newLoc, bool bTransport, int nSceneID, int nAngle, int nReason)
{
    if (pEntity == NULL)
    {
        return;
    }

    __IMovePart * pMovePart = (__IMovePart *)pEntity->getEntityPart(PART_MOVE);
    if(pMovePart == NULL)
    {
        return;               
    }

    Move3 move = pMovePart->getMoveState();
    move.vPosition = newLoc;
    if(nAngle != INT_MAX)
    {
        move.vRotation.x = 0;
        move.vRotation.y = (float)nAngle;
        move.vRotation.z = 0;
    }

    if (bTransport)
    {
        pMovePart->stopAt(move, nSceneID, nReason);
    }
    else
    {
        pMovePart->changeMoveState(move);
    }
}

/** 发送数据
*/
void CExternalHelper::playerSend2Client(__IEntity *pEntity, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData, size_t nDataLen)
{
    if (pEntity == NULL || pszData == NULL || nDataLen == 0)
    {
        return;
    }

    // 取得技能部件
    __ISpellPart *pSpellPart = (__ISpellPart *)pEntity->getEntityPart(PART_SPELL);
    if (pSpellPart == NULL)
    {
        return;
    }
    // 未创建实体，下发客户端消息会接不到，不下发
    if (!pSpellPart->getCreateEntity())
    {
        return;
    }

    // 非人物技能，不下发
    if (pEntity->getType() != TYPE_PLAYER_ROLE)
    {
        return;
    }

    // 通知客户端
    obuf256 obufData;

    SNetMsgHead head;
    head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
    head.byDestEndPoint= MSG_ENDPOINT_CLIENT;	
    head.byKeyModule   = MSG_MODULEID_ENTITY;	
    head.byKeyAction   = byKeyAction;

    // 实体类统一用子头来用于转向部件
    SMsgEntityHead entityHead;
    entityHead.uidMaster = pEntity->getUID();	// 实体UID
    entityHead.byPartID = byPartID;			    // 部件ID，ENTITY_PART_MAX时将由实体来处理

    obufData << head << entityHead;
    obufData.push_back(pszData,nDataLen);

    ((__IPlayerRole*)pEntity)->send2Client(obufData.data(), obufData.size());
}