/*******************************************************************
** 文件名:	NPCFuncTrigger.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/2/14
** 版  本:	1.0
** 描  述:	NPC功能出发器
********************************************************************/
#include "stdafx.h"
#include "NPCFuncTrigger.h"
#include "TriggerManager.h"
#include "ExternalHelper.h"
#include "ILuaService.h"
#include "EntityHelper.h"
#include "EventDef.h"
#include "DGlobalMessage.h"

//////////////////////////////////////////////////////////////////////////
bool NPCFuncTrigger::Setup()
{
	SceneLogicEventHelper helper(m_nSceneID);
	IEventEngine *pEventEngine = helper.m_ptr;
	if (pEventEngine == NULL)
	{
		return false;
	}
	pEventEngine->Subscibe(this, EVENT_GAME_NPC_FUNC, TYPE_PLAYER_ROLE, 0, __FUNCTION__);
    
	return true;
}

void NPCFuncTrigger::Release()
{
	SceneLogicEventHelper helper(m_nSceneID);
	IEventEngine *pEventEngine = helper.m_ptr;
	if (pEventEngine != NULL)
	{
		pEventEngine->UnSubscibe(this, EVENT_GAME_NPC_FUNC, TYPE_PLAYER_ROLE, 0);
	}

	delete this;
}

void NPCFuncTrigger::SetID(int nID)
{
	m_nTriggerID = nID;
}

int NPCFuncTrigger::GetID() const
{
	return m_nTriggerID;
}

int NPCFuncTrigger::GetTriggerType(void)
{
	return ETriggerType_NPCFunc;
}

std::string NPCFuncTrigger::GetTriggerCallBack(void)
{
	return m_szCallFunc;
}

int NPCFuncTrigger::GetSceneID()
{
    return m_nSceneID;
}

//////////////////////////////////////////////////////////////////////////
NPCFuncTrigger::NPCFuncTrigger(int nSceneID, const char *szCallFunc) : 
    m_nSceneID(nSceneID),    
    m_szCallFunc(szCallFunc)
{
}

NPCFuncTrigger::~NPCFuncTrigger()
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
void NPCFuncTrigger::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	if (pszContext == NULL || nLen <sizeof(event_entity_npc_func))
	{
		ErrorLn(__FUNCTION__ ", pszContext == NULL || nLen <sizeof(event_entity_npc_func)");
		return;
	}

	event_entity_npc_func *pData = (event_entity_npc_func *)pszContext;

	char* szParam = (char*)pszContext + sizeof(event_entity_npc_func);
	DWORD dwParamLen = nLen - sizeof(event_entity_npc_func);
	if (dwParamLen != pData->dwParamLen)
	{
		ErrorLn(__FUNCTION__ ",dwParamLen != pData->dwParamLen, Invalid param length.");
		return;
	}

	SceneLogicHelper helper(m_nSceneID);
	ISceneLogicService* pSceneLogicService = helper.m_ptr;
	if(pSceneLogicService == NULL)
	{
		return ;
	}

	INPCService* pNPCService = pSceneLogicService->getNPCService();
	if(pNPCService == NULL)
	{
		return ;
	}

	NPC_PTR pNPC = pNPCService->findNPC(pData->uidNPC);
	if (pNPC == 0)
	{
		return;
	}
	int nNPCID = pNPC->getIntProperty(PROPERTY_ID);

	ILuaService *pLuaService = pSceneLogicService->getLuaService();
	if (pLuaService == NULL)
	{
		return;
	}

	CLuaParam params[4];
	params[0] = pData->uidNPC;
	params[1] = nNPCID;
	params[2] = pData->pdbid;
	params[3] = szParam;
	pLuaService->RunLuaFunction(m_szCallFunc.c_str(), params, _countof(params));
}