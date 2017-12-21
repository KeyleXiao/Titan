/*******************************************************************
** 文件名:	EndConditionStartMove.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/15  12:19
** 版  本:	1.0
** 描  述:	开始移动后取消
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "EndConditionStartMove.h"
#include "ExternalFacade.h"
#include "BuffPart.h"
#include "IEntityEvent.h"
#include "EventDef.h"

/** 
@param   
@param   
@return  
*/
CEndConditionStartMove::CEndConditionStartMove(void)
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
CEndConditionStartMove::~CEndConditionStartMove(void)
{
}

/** 
@param   
@param   
@return  
*/
bool CEndConditionStartMove::Create(CBuffPart * pBuffPart, CBuff * pBuff)
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

	// 订阅开始移动
	//pMaster->getEntityEvent()->Subscibe(this, EVENT_CREATURE_STARTMOVE, "Buff");

	return true;
}

/** 
@param   
@param   
@return  
*/
void CEndConditionStartMove::Close(void)
{
	if(m_pBuffPart != NULL && m_pBuff != NULL)
	{
		__IEntity * pMaster = m_pBuffPart->getHost();
		if (pMaster != NULL)
		{
			// 注销开始移动
			//pMaster->getEntityEvent()->UnSubscibe(this, EVENT_CREATURE_STARTMOVE);
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
bool CEndConditionStartMove::OnVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	/*if(wEventID != EVENT_CREATURE_STARTMOVE)
	{
	return true;
	}

	if(m_pBuffPart != NULL)
	{
	m_pBuffPart->Remove(m_pBuff, INVALID_UID, false);
	}*/

	return true;
}