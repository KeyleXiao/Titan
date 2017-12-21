/*******************************************************************
** 文件名:	MeetNPCTrigger.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/1/18
** 版  本:	1.0
** 描  述:	NPC触发器
********************************************************************/

#include "stdafx.h"
#include "NPCTrigger.h"
#include "TriggerManager.h"
#include "ExternalHelper.h"
#include "ILuaService.h"
#include "EntityHelper.h"
#include "EventDef.h"
#include "DGlobalMessage.h"

//////////////////////////////////////////////////////////////////////////
bool NPCTrigger::Setup()
{
	SceneLogicEventHelper helper(m_nSceneID);
	IEventEngine *pEventEngine = helper.m_ptr;
	if (pEventEngine == NULL)
	{
		return false;
	}
	pEventEngine->Subscibe(this, EVENT_GAME_NPC_TRIGGER, TYPE_PLAYER_ROLE, 0, __FUNCTION__);

	return true;
}

void NPCTrigger::Release()
{
	SceneLogicEventHelper helper(m_nSceneID);
	IEventEngine *pEventEngine = helper.m_ptr;
	if (pEventEngine != NULL)
	{
		pEventEngine->UnSubscibe(this, EVENT_GAME_NPC_TRIGGER, TYPE_PLAYER_ROLE, 0);
	}

	delete this;
}

void NPCTrigger::SetID(int nID)
{
	m_nTriggerID = nID;
}

int NPCTrigger::GetID() const
{
	return m_nTriggerID;
}

int NPCTrigger::GetTriggerType(void)
{
	return ETriggerType_NPC;
}

std::string NPCTrigger::GetTriggerCallBack(void)
{
	return m_szCallFunc;
}

int NPCTrigger::GetSceneID()
{
    return m_nSceneID;
}

//////////////////////////////////////////////////////////////////////////
NPCTrigger::NPCTrigger(int nSceneID, const char *szCallFunc) : 
    m_nSceneID(nSceneID),    
    m_szCallFunc(szCallFunc)
{
}

NPCTrigger::~NPCTrigger()
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
void NPCTrigger::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	if (pszContext == NULL || nLen !=sizeof(event_entity_npc_trigger))
	{
		return;
	}
	event_entity_npc_trigger *pData = (event_entity_npc_trigger *)pszContext;

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

	CLuaParam params[3];
	params[0] = pData->uidNPC;
	params[1] = nNPCID;
	params[2] = pData->pdbid;
	pLuaService->RunLuaFunction(m_szCallFunc.c_str(), params, _countof(params));
}