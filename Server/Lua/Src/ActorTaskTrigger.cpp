/*******************************************************************
** 文件名:	ActorTaskTrigger.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	3/16/2017
** 版  本:	1.0
** 描  述:	角色任务数据触发器
********************************************************************/
#include "stdafx.h"
#include "ActorTaskTrigger.h"
#include "TriggerManager.h"
#include "ExternalHelper.h"
#include "ILuaService.h"
#include "EntityHelper.h"
#include "EventDef.h"
#include "DGlobalMessage.h"
#include "TaskDef.h"

//////////////////////////////////////////////////////////////////////////
bool ActorTaskTrigger::Setup()
{
    SceneLogicEventHelper helper(m_nSceneID);
    IEventEngine *pEventEngine = helper.m_ptr;
    if (pEventEngine == NULL)
    {
        return false;
    }

    pEventEngine->Subscibe(this, EVENT_GAME_ACTOR_DATA_TASK, TYPE_PLAYER_ROLE, 0, __FUNCTION__);
	
	return true;
}

void ActorTaskTrigger::Release()
{
    SceneLogicEventHelper helper(m_nSceneID);
    IEventEngine *pEventEngine = helper.m_ptr;
    if (pEventEngine != NULL)
    {
		pEventEngine->UnSubscibe(this, EVENT_GAME_ACTOR_DATA_TASK, TYPE_PLAYER_ROLE, 0);
    }

	delete this;
}

void ActorTaskTrigger::SetID(int nID)
{
	m_nTriggerID = nID;
}

int ActorTaskTrigger::GetID() const
{
	return m_nTriggerID;
}

int ActorTaskTrigger::GetTriggerType(void)
{
	return ETriggerType_ActorTask;
}

std::string ActorTaskTrigger::GetTriggerCallBack(void)
{
	return m_szCallFunc;
}

int ActorTaskTrigger::GetSceneID()
{
    return m_nSceneID;
}

//////////////////////////////////////////////////////////////////////////
ActorTaskTrigger::ActorTaskTrigger(int nSceneID, const char *szCallFunc) : 
    m_nSceneID(nSceneID),    
    m_szCallFunc(szCallFunc)
{
}

ActorTaskTrigger::~ActorTaskTrigger()
{
}

//////////////////////////////////////////////////////////////////////////
/** 
@param   wEventID ：事件ID
@param   bSrcType ：发送源类型
@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
@param   pszContext : 上下文
@param   nLen : 上下文长度
@return  
@note     
@warning 
@retval buffer 
*/
void ActorTaskTrigger::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	ILuaService *pLuaService = NULL;
	SceneLogicHelper helper(m_nSceneID);
	if (helper.m_ptr)
	{
		pLuaService = helper.m_ptr->getLuaService();
	}

	if (pLuaService == NULL)
	{
		return;
	}

	switch (wEventID)
	{
	case EVENT_GAME_ACTOR_DATA_TASK:
		{
			if(pszContext == NULL || nLen < sizeof(event_game_actor_data_task))
			{
				ErrorLn(__FUNCTION__ << __LINE__);
				return;
			}

			event_game_actor_data_task* pData = (event_game_actor_data_task*)pszContext;

			CLuaParam params[5];
			params[0] = pData->nTaskLuaEvent;
			params[1] = pData->dwPDBID;
			params[2] = pData->nParam1;
			params[3] = pData->nParam2;
			params[4] = pData->nParam3;
			pLuaService->RunLuaFunction(m_szCallFunc.c_str(), params, _countof(params));
		}
		break;
	default:
		break;
	}	
}