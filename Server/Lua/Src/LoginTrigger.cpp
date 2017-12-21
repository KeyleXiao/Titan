/*******************************************************************
** 文件名:	LoginTrigger.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	11/25/2016
** 版  本:	1.0
** 描  述:	登入触发器
********************************************************************/
#include "stdafx.h"
#include "LoginTrigger.h"
#include "TriggerManager.h"
#include "ExternalHelper.h"
#include "ILuaService.h"
#include "EntityHelper.h"
#include "EventDef.h"
#include "DGlobalMessage.h"

//////////////////////////////////////////////////////////////////////////
bool LoginTrigger::Setup()
{
    SceneLogicEventHelper helper(m_nSceneID);
    IEventEngine *pEventEngine = helper.m_ptr;
    if (pEventEngine == NULL)
    {
        return false;
    }
    pEventEngine->Subscibe(this, EVENT_GAME_ACTOR_LOGIN, TYPE_PLAYER_ROLE, 0, __FUNCTION__);
    pEventEngine->Subscibe(this, EVENT_GAME_ACTOR_READTASKDATA, TYPE_PLAYER_ROLE, 0, __FUNCTION__);
    
	return true;
}

void LoginTrigger::Release()
{
    SceneLogicEventHelper helper(m_nSceneID);
    IEventEngine *pEventEngine = helper.m_ptr;
    if (pEventEngine != NULL)
    {
		pEventEngine->UnSubscibe(this, EVENT_GAME_ACTOR_LOGIN, TYPE_PLAYER_ROLE, 0);
		pEventEngine->UnSubscibe(this, EVENT_GAME_ACTOR_READTASKDATA, TYPE_PLAYER_ROLE, 0);
    }

	delete this;
}

void LoginTrigger::SetID(int nID)
{
	m_nTriggerID = nID;
}

int LoginTrigger::GetID() const
{
	return m_nTriggerID;
}

int LoginTrigger::GetTriggerType(void)
{
	return ETriggerType_Login;
}

std::string LoginTrigger::GetTriggerCallBack(void)
{
	return m_szCallFunc;
}

int LoginTrigger::GetSceneID()
{
    return m_nSceneID;
}

//////////////////////////////////////////////////////////////////////////
LoginTrigger::LoginTrigger(int nSceneID, const char *szCallFunc) : 
    m_nSceneID(nSceneID),    
    m_szCallFunc(szCallFunc)
{
    m_LoginMethodMap.clear();
    m_ReadTaskDataMap.clear();
}

LoginTrigger::~LoginTrigger()
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
void LoginTrigger::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    // 收到登陆事件,记录玩家是真正登陆还是跨场景
    if (EVENT_GAME_ACTOR_LOGIN == wEventID)
    {
        if (pszContext == NULL || nLen !=sizeof(event_actor_login))
        {
            return;
        }
        event_actor_login *pData = (event_actor_login *)pszContext;
        // 登陆表
        m_LoginMethodMap[pData->dwDBID] = pData->eMode;
        // 设置LUA登陆事件
        SetLuaLoginEvent(pData->dwDBID);
    }

    // 读取任务数据成功,lua脚本中才能真正处理上层玩法逻辑
    if (EVENT_GAME_ACTOR_READTASKDATA == wEventID)
    {
        if (pszContext == NULL || nLen != sizeof(event_actor_read_taskdata_s))
        {
            return;
        }
        event_actor_read_taskdata_s *pData = (event_actor_read_taskdata_s *)pszContext;

        // 读取任务数据表
        m_ReadTaskDataMap[pData->dwHolder] = true;
        // 设置LUA登陆事件
        SetLuaLoginEvent(pData->dwHolder);
    }
}

// 设置LUA登陆事件
void LoginTrigger::SetLuaLoginEvent(DWORD dwPDBID)
{
    if (dwPDBID <= 0)
    {
        return;
    }

    std::map<DWORD,bool>::iterator IterTaskData = m_ReadTaskDataMap.find(dwPDBID);
    if (IterTaskData == m_ReadTaskDataMap.end())
    {
        return;
    }

    std::map<DWORD,int>::iterator Iter = m_LoginMethodMap.find(dwPDBID);
    if (Iter == m_LoginMethodMap.end())
    {
        return;
    }
    int nLoginMethod = Iter->second;

    // 登陆表中移除
    m_LoginMethodMap.erase(Iter);
    // 读取任务表中移除
    m_ReadTaskDataMap.erase(IterTaskData);

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
    ActorHelper actorHelper(dwPDBID, ACTOR_DBID);
    IActorService *pActorService = actorHelper.m_ptr;
    if (pActorService == NULL)
    {
        return;
    }

    CLuaParam params[3];
    params[0] = dwPDBID;
    params[1] = nLoginMethod;
    params[2] = pActorService->getName().c_str();
    pLuaService->RunLuaFunction(m_szCallFunc.c_str(), params, _countof(params));
}