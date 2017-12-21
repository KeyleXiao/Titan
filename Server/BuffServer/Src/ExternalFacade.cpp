/*******************************************************************
** 文件名:	ExternalFacade.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/13  11:36
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "stdafx.h"
#include "ExternalFacade.h"
#include "ITimerService.h"
#include "IFramework.h"
#include "IServiceMgr.h"
#include "IGatewayAcceptorService.h"
#include "IServerGlobal.h"
#include "IEffect.h"
#include "IPlayerRole.h"
#include "ISceneManager.h"
#include "IGameSceneService.h"
#include "AOI_Define.h"
#include "IActorMgr.h"
#include "IEntityAOI.h"
#include "ISpell.h"
#include "IActorService.h"
#include "EntityHelper.h"

CExternalFacade	g_ExternalFacade;

/** 
*/
CExternalFacade::CExternalFacade(void)
{

}

/** 
*/
CExternalFacade::~CExternalFacade(void)
{

}

/** 启用定时器
*/ 
bool CExternalFacade::SetTimer(DWORD timerID, DWORD interval, ITimerHandler * handler, LPCSTR debugInfo, DWORD callTimes)
{
	// 获取时间轴接口
	ITimerService * pTimerService = gServerGlobal->getTimerService();
	if (pTimerService == NULL)
	{
		return false;
	}

	return pTimerService->SetTimer(timerID, interval, handler, callTimes, debugInfo);
}

/** 销毁定时器
*/
bool CExternalFacade::KillTimer(DWORD timerID, ITimerHandler * handler)
{
	// 获取时间轴接口
	ITimerService * pTimerService = gServerGlobal->getTimerService();
	if (pTimerService == NULL)
	{
		return false;
	}

	return pTimerService->KillTimer(timerID, handler);
}

/** 创建现场
*/
IEffect *CExternalFacade::CreateEffect(DWORD dwEffectID)
{
	IEffectFactory *pEffectFactory = gServerGlobal->getEffectFactory();
	if(pEffectFactory == NULL)
	{
		return NULL;
	}

	return pEffectFactory->BuildEffect(dwEffectID);
}

/** 
*/
bool CExternalFacade::IsExistEffectID(int nEffectID)
{
	IEffectFactory * pEffectFactory = gServerGlobal->getEffectFactory();
	if(pEffectFactory == NULL)
	{
		return false;
	}

	return pEffectFactory->IsExistEffectID(nEffectID);
}


/** 发送数据
*/
void CExternalFacade::playerSend2Client(__IEntity *pEntity, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData, int nDataLen)
{
    if (pEntity == NULL || pszData == NULL || nDataLen == 0)
    {
        return;
    }

    // 非人物技能，不下发
    if (pEntity->getType() != TYPE_PLAYER_ROLE)
    {
        return;
    }

	//是否电脑玩家
	if (IsComputerPDBID(pEntity->getIntProperty(PROPERTY_ID)))
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


/** 发送数据
*/
bool CExternalFacade::broadCastInAOI(__IEntity *pEntity, Vector3 vLoc, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData, int nDataLen)
{
	if (pEntity == NULL || pszData == NULL || nDataLen == 0)
	{
		return false;
	}

    // 取得技能部件
    __ISpellPart *pSpellPart = (__ISpellPart *)pEntity->getEntityPart(PART_SPELL);
    if (pSpellPart == NULL)
    {
        return false;
    }
    // 未创建实体，下发客户端消息会接不到，不下发
    if (!pSpellPart->getCreateEntity())
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

    __IEntityAOI* pEntityAOIPart = (__IEntityAOI*)pEntity->getEntityPart(PART_AOI);
    if (pEntityAOIPart != NULL)
    {
        pEntityAOIPart->broadcast(vLoc, 0, BROADCAST_BUFF, std::string(o.data(),o.size()), __FUNCTION__, __LINE__);
    }

	return true;
}


// 广播数据给所有人
bool CExternalFacade::broadcastDataToAllPlayer(__IEntity *pMaster, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData, int nDataLen)
{
	if (pMaster == NULL || pszData == NULL || nDataLen == 0)
	{
		return false;
	}

	int nSceneID = pMaster->getSceneID();

	ISceneManager *pSceneManager = gServerGlobal->getSceneManager();
	if (pSceneManager == NULL)
	{
		return false;
	}
	SERVICE_PTR pContainer = pSceneManager->getScene(nSceneID);
	if (pContainer == NULL)
	{
		return false;
	}
	IGameSceneService * pSceneService = (IGameSceneService*)pContainer->query_interface();
	if (pSceneService == NULL)
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
	entityHead.uidMaster = pMaster->getUID();	// 实体UID
	entityHead.byPartID = byPartID;			    // 部件ID，ENTITY_PART_MAX时将由实体来处理

	o << head << entityHead;
	o.push_back(pszData,nDataLen);

	pSceneService->broadcast_all(BROADCAST_BUFF,std::string(o.data(),o.size()));

	return true;
}

void CExternalFacade::playerSend2OtherClient(UID curUID, UID otherUID, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData, int nDataLen)
{
	if (curUID == INVALID_UID || otherUID == INVALID_UID || pszData == NULL || nDataLen == 0)
	{
		return;
	}

	// 通知客户端
	obuf256 obufData;

	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.byKeyModule = MSG_MODULEID_ENTITY;
	head.byKeyAction = byKeyAction;

	// 实体类统一用子头来用于转向部件
	SMsgEntityHead entityHead;
	entityHead.uidMaster = curUID;				// 实体UID
	entityHead.byPartID = byPartID;			    // 部件ID，ENTITY_PART_MAX时将由实体来处理

	obufData << head << entityHead;
	obufData.push_back(pszData, nDataLen);

	send2Client(otherUID, PACKAGE_PTR(new string(obufData.data(), obufData.size())));
}
