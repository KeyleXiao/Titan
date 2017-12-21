/*******************************************************************
** 文件名:	EndConditionDie.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/14  17:03
** 版  本:	1.0
** 描  述:	死亡后取消
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "EndConditionDie.h"
#include "ExternalFacade.h"
#include "BuffPart.h"
#include "IEntityEvent.h"
#include "EventDef.h"

/** 
@param   
@param   
@return  
*/
CEndConditionDie::CEndConditionDie(void)
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
CEndConditionDie::~CEndConditionDie(void)
{
}

/** 
@param   
@param   
@return  
*/
bool CEndConditionDie::Create(CBuffPart * pBuffPart, CBuff * pBuff)
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

	// 订阅死亡事件
	pMaster->getEntityEvent()->Subscibe(this, EVENT_ENTITY_DIE, "Buff");

	return true;
}

/** 
@param   
@param   
@return  
*/
void CEndConditionDie::Close(void)
{
	if(m_pBuffPart != NULL && m_pBuff != NULL)
	{
		__IEntity * pMaster = m_pBuffPart->getHost();
		if (pMaster != NULL)
		{
			// 注销死亡事件
			pMaster->getEntityEvent()->UnSubscibe(this, EVENT_ENTITY_DIE);
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
@param   dwSrcID : 发送源标识（UID中"序列号"部份）
@param   pszContext : 上下文
@param   nLen : 上下文长度
@return  如果返回false : 则中断执行，否则继续向下执行
@note     
@warning 
@retval buffer 
*/
bool CEndConditionDie::OnVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	if(wEventID != EVENT_ENTITY_DIE)
	{
		return true;
	}

	if(m_pBuffPart != NULL)
	{
		m_pBuffPart->Remove(m_pBuff, INVALID_UID, false);
	}

	return true;
}
