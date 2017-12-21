/*******************************************************************
** 文件名:	WarPersonTaskTrigger.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	11/25/2016
** 版  本:	1.0
** 描  述:	战场人物任务数据触发器
********************************************************************/
#include "stdafx.h"
#include "WarPersonTaskTrigger.h"
#include "TriggerManager.h"
#include "ExternalHelper.h"
#include "ILuaService.h"
#include "EntityHelper.h"
#include "EventDef.h"
#include "DGlobalMessage.h"
#include "TaskHelper.h"
#include "TaskDef.h"

//////////////////////////////////////////////////////////////////////////
bool WarPersonTaskTrigger::Setup()
{
    SceneLogicEventHelper helper(m_nSceneID);
    IEventEngine *pEventEngine = helper.m_ptr;
    if (pEventEngine == NULL)
    {
        return false;
    }
    pEventEngine->Subscibe(this, EVENT_GAME_WAR_PERSON_TASK, TYPE_PLAYER_ROLE, 0, __FUNCTION__);
    
	return true;
}

void WarPersonTaskTrigger::Release()
{
    SceneLogicEventHelper helper(m_nSceneID);
    IEventEngine *pEventEngine = helper.m_ptr;
    if (pEventEngine != NULL)
    {
		pEventEngine->UnSubscibe(this, EVENT_GAME_WAR_PERSON_TASK, TYPE_PLAYER_ROLE, 0);
    }

	delete this;
}

void WarPersonTaskTrigger::SetID(int nID)
{
	m_nTriggerID = nID;
}

int WarPersonTaskTrigger::GetID() const
{
	return m_nTriggerID;
}

int WarPersonTaskTrigger::GetTriggerType(void)
{
	return ETriggerType_WarPersonTask;
}

std::string WarPersonTaskTrigger::GetTriggerCallBack(void)
{
	return m_szCallFunc;
}

int WarPersonTaskTrigger::GetSceneID()
{
    return m_nSceneID;
}

//////////////////////////////////////////////////////////////////////////
WarPersonTaskTrigger::WarPersonTaskTrigger(int nSceneID, const char *szCallFunc) : 
    m_nSceneID(nSceneID),    
    m_szCallFunc(szCallFunc)
{
}

WarPersonTaskTrigger::~WarPersonTaskTrigger()
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
void WarPersonTaskTrigger::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    if (EVENT_GAME_WAR_PERSON_TASK == wEventID)
    {
        if (pszContext == NULL || nLen !=sizeof(event_game_war_person_task))
        {
            return;
        }
        event_game_war_person_task *pData = (event_game_war_person_task *)pszContext;
        
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

        CLuaParam params[4];
		params[0] = (int)emTaskLuaEventType_FinishMatch;
        params[1] = pData->dwPDBID;
		params[2] = pData->szTeamMemberID;
		params[3] = pData->szWarResult;
        pLuaService->RunLuaFunction(m_szCallFunc.c_str(), params, _countof(params));
    }
}