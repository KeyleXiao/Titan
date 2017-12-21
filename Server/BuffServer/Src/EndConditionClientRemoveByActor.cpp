/*******************************************************************
** 文件名:	EndConditionClientRemoveByActor.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	12/28/2016
** 版  本:	1.0
** 描  述:	客户端移除取消buff
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "EndConditionClientRemoveByActor.h"
#include "ExternalFacade.h"
#include "BuffPart.h"
#include "IEntityEvent.h"
#include "EventDef.h"
#include "EntityHelper.h"

/** 
@param   
@param   
@return  
*/
CEndConditionClientRemoveByActor::CEndConditionClientRemoveByActor(void)
{
	// 主人
	m_pBuffPart = NULL;

	// BUFF
	m_pBuff = NULL;

	// 是否创建
	m_bCreate = false;
}

/** 
@param   
@param   
@return  
*/
CEndConditionClientRemoveByActor::~CEndConditionClientRemoveByActor(void)
{
}

/** 
@param   
@param   
@return  
*/
bool CEndConditionClientRemoveByActor::Create(CBuffPart * pBuffPart, CBuff * pBuff)
{
	if(pBuffPart == NULL || pBuff == NULL)
	{
		return false;
	}

	// 不要重复创建
	if(m_bCreate)
	{
		return true;
	}
	m_bCreate = true;

	// 主人
	__IEntity *pMaster = pBuffPart->getHost();
	if(pMaster == NULL)
	{
		return false;
	}

	// 主人部件
	m_pBuffPart = pBuffPart;

	// BUFF
	m_pBuff = pBuff;

	// 订阅客户端移除BUFF
	pMaster->getEntityEvent()->Subscibe(this, EVENT_ENTITY_CLIENT_REMOVE_BUFF, "Buff");

	return true;
}

/** 
@param   
@param   
@return  
*/
void CEndConditionClientRemoveByActor::Close(void)
{
	if(m_pBuffPart != NULL && m_pBuff != NULL)
	{
		__IEntity * pMaster = m_pBuffPart->getHost();
		if (pMaster != NULL)
		{
			// 注销客户端移除BUFF
			pMaster->getEntityEvent()->UnSubscibe(this, EVENT_ENTITY_CLIENT_REMOVE_BUFF);
		}
	}

	// 主人
	m_pBuffPart = NULL;

	// BUFF
	m_pBuff = NULL;

	// 是否创建
	m_bCreate = false;
}

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
void CEndConditionClientRemoveByActor::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	if(wEventID != EVENT_ENTITY_CLIENT_REMOVE_BUFF)
	{
		return;
	}
    if (m_pBuffPart == NULL)
    {
        return;
    }
    if (pszContext == NULL || nLen != sizeof(event_entity_client_remove_buff))
    {
        return;
    }
    event_entity_client_remove_buff *pRemoveBuff = (event_entity_client_remove_buff *)pszContext;
    if (!isActor(pRemoveBuff->uidOperator))
    {
        return;
    }

	m_pBuffPart->Remove(m_pBuff, INVALID_UID, true);
}
