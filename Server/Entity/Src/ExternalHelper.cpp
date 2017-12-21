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
#include "EntityHelper.h"
#include "ISpell.h"
#include "IPlayerRole.h"
#include "IEntityAOI.h"
#include "ChatHelper.h"
#include "IChatSceneService.h"

CExternalHelper				g_EHelper;

CExternalHelper::CExternalHelper()
{
}


CExternalHelper::~CExternalHelper()
{
}


/** 启用定时器
*/
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
void CExternalHelper::sendEntityMessage(UID uid, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData, int nDataLen)
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
	handleMessage(uid, obufData.data(),obufData.size());
}

/** 发送数据
*/
void CExternalHelper::playerSend2Client(__IEntity *pEntity, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData, int nDataLen)
{
    if (pEntity == NULL)
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

/** 英雄发送数据到客户端
*/
void CExternalHelper::playerSend2Client(UID uid, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData, int nDataLen)
{
    if (isInvalidUID(uid))
    {
        return;
    }

    int nType = UID_2_TYPE(uid);
    // 非人物技能，不下发
    if (nType != TYPE_PLAYER_ROLE)
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
    entityHead.uidMaster = uid;	                // 实体UID
    entityHead.byPartID = byPartID;			    // 部件ID，ENTITY_PART_MAX时将由实体来处理

    obufData << head << entityHead;
    obufData.push_back(pszData,nDataLen);

    send2Client(uid, PACKAGE_PTR(new std::string(obufData.data(), obufData.size())));
}


/** 发送数据
*/
bool CExternalHelper::broadCastInAOI(__IEntity *pEntity, Vector3 ptTargetTile, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData, int nDataLen)
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
    entityHead.byPartID = byPartID;			    // 部件ID，ENTITY_PART_MAX时将由实体来处理

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
bool CExternalHelper::broadCastDataToAllPlayer(__IEntity *pEntity, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData, int nDataLen)
{
    if (pEntity == NULL || pszData == NULL || nDataLen == 0)
    {
        return false;
    }

    int nSceneID = pEntity->getSceneID();
    SERVICE_PTR pContainer = gServerGlobal->getSceneManager()->getScene(nSceneID);
    if (pContainer == NULL)
    {
        return false;
    }

    IGameSceneService *pScene = (IGameSceneService*)pContainer->query_interface();
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
    entityHead.byPartID = byPartID;			    // 部件ID，ENTITY_PART_MAX时将由实体来处理

    o << head << entityHead;
    o.push_back(pszData,nDataLen);

    pScene->broadcast_all(BROADCAST_DO_ACTION,std::string(o.data(),o.size()));

    return true;
}


/** 显示提示文字
*/
void CExternalHelper::showSystemMessage(__IEntity *pEntity, EMChatTipID nTipID, LPCSTR szParam, EMChatBroadcastType nBroadcastType, int nDelayTime)
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

    int nValue = 0;
    switch (nBroadcastType)
    {
    case CHAT_BROADCASTTYPE_SINGLE:
        {
            nValue = pEntity->getIntProperty(PROPERTY_ID);
        }
        break;
    case CHAT_BROADCASTTYPE_WARSCENE:
        {
            nValue = pEntity->getSceneID();
        }
        break;
	case  CHAT_BROADCASTTYPE_CLAN:
		{
			nValue = pEntity->getIntProperty(PROPERTY_CLAN);
		}
		break;
	case CHAT_BROADCASTTYPE_KIN:
		{
			nValue = pEntity->getIntProperty(PROPERTY_KIN);
		}
		break;
	case CHAT_BROADCASTTYPE_WORLD:
		{
			nValue = pEntity->getIntProperty(PROPERTY_ID);
		}
		break;
	case CHAT_BROADCASTTYPE_LOUDSPEAKER:
		{
			nValue = 0;
		}
		break;
	case CHAT_BROADCASTTYPE_ZONE:
		{
			nValue = pEntity->getSceneID();
		}
		break;
    default:
        {
            ErrorLn("showSystemMessage error nBroadcastType="<<nBroadcastType);
            return;
        }
        break;
    }

    pChatSceneService->broadcastSystemMessage(nBroadcastType, nValue, nTipID, szParam, nDelayTime);
}


/** 显示提示文字
*/
void CExternalHelper::showSystemMessage(UID uid, EMChatTipID nTipID, LPCSTR szParam, EMChatBroadcastType nBroadcastType, int nDelayTime)
{
    if (isInvalidUID(uid))
    {
        return;
    }

    int nType = UID_2_TYPE(uid);
    if (nType != TYPE_PLAYER_ROLE)
    {
        return;
    }

    ChatHelper chatHelper;
    IChatSceneService *pChatSceneService = chatHelper.m_ptr;
    if (pChatSceneService == NULL)
    {
        return;
    }

    int nValue = 0;
    switch (nBroadcastType)
    {
    case CHAT_BROADCASTTYPE_SINGLE:
        {
            nValue = getProperty_Integer(uid, PROPERTY_ID);
        }
        break;
    case CHAT_BROADCASTTYPE_WARSCENE:
        {
            nValue = UID_2_SCENE(uid);
        }
        break;
    default:
        {
            ErrorLn("showSystemMessage error nBroadcastType="<<nBroadcastType);
            return;
        }
        break;
    }

    pChatSceneService->broadcastSystemMessage(nBroadcastType, nValue, nTipID, szParam, nDelayTime);
}