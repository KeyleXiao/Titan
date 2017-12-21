/*******************************************************************
** 文件名:	EndConditionAttackReady.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/6/27 17:33
** 版  本:	1.0
** 描  述:	有蓄气攻击，蓄气时取消
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "EndConditionAttackReady.h"
#include "BuffPart.h"
#include "IEntityEvent.h"
#include "EventDef.h"

/** 
@param   
@param   
@return  
*/
CEndConditionAttackReady::CEndConditionAttackReady(void)
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
CEndConditionAttackReady::~CEndConditionAttackReady(void)
{
}

/** 
@param   
@param   
@return  
*/
bool CEndConditionAttackReady::Create(CBuffPart * pBuffPart, CBuff * pBuff)
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

	// 订阅开始蓄气
	//pMaster->getEntityEvent()->Subscibe(this, EVENT_CREATURE_ATTACKREADY, "Buff");

	return true;
}

/** 
@param   
@param   
@return  
*/
void CEndConditionAttackReady::Close(void)
{
	if(m_pBuffPart != NULL && m_pBuff != NULL)
	{
		__IEntity * pMaster = m_pBuffPart->getHost();
		if (pMaster != NULL)
		{
			// 注销开始蓄气
			//pMaster->getEntityEvent()->UnSubscibe(this, EVENT_CREATURE_ATTACKREADY);
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
void CEndConditionAttackReady::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	/*if(wEventID != EVENT_CREATURE_ATTACKREADY)
	{
	return;
	}

	if(m_pBuffPart != NULL)
	{
	m_pBuffPart->Remove(m_pBuff, INVALID_UID, false);
	}*/	
}
