/*******************************************************************
** 文件名:	EndConditionChangeLoc.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/15  10:37
** 版  本:	1.0
** 描  述:	改变位置后取消，包括移动，瞬移，切换地图
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "EndConditionChangeLoc.h"
#include "ExternalFacade.h"
#include "BuffPart.h"
#include "IEntityEvent.h"
#include "EventDef.h"

/** 
@param   
@param   
@return  
*/
CEndConditionChangeLoc::CEndConditionChangeLoc(void):m_vLoc(0,0,0)
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
CEndConditionChangeLoc::~CEndConditionChangeLoc(void)
{
}

/** 
@param   
@param   
@return  
*/
bool CEndConditionChangeLoc::Create(CBuffPart * pBuffPart, CBuff * pBuff)
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

    m_vLoc = pMaster->getLocation();

	// 订阅改变位置事件
    pMaster->getEntityEvent()->Subscibe(this, (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_LOCATION), "Buff");

	return true;
}

/** 
@param   
@param   
@return  
*/
void CEndConditionChangeLoc::Close(void)
{
	if(m_pBuffPart != NULL && m_pBuff != NULL)
	{
		__IEntity * pMaster = m_pBuffPart->getHost();
		if (pMaster != NULL)
		{
			// 注销改变位置事件
			pMaster->getEntityEvent()->UnSubscibe(this, (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_LOCATION));
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
void CEndConditionChangeLoc::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	if(wEventID != (EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_LOCATION) || pszContext == NULL || nLen != sizeof(event_entity_property_change))
	{
		return;
	}

    event_entity_property_change* pEvent = (event_entity_property_change*)pszContext;
    if(pEvent->nPropertyID != PROPERTY_LOCATION)
    {
        return;
    }

    if(m_pBuffPart == NULL)
    {
        return;
    }

    // 主人
    __IEntity *pMaster = m_pBuffPart->getHost();
    if(pMaster == NULL)
    {
        return;
    }

    Vector3 vCurrentLoc = pMaster->getLocation();
    // 在范围内不打断
    if (m_vLoc.checkDistance(vCurrentLoc, 0.3f))
    {
        return;
    }

	m_pBuffPart->Remove(m_pBuff, INVALID_UID, false);
}
