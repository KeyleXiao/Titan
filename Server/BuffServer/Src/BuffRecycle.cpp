/*******************************************************************
** 文件名:	BuffRecycle.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/15  12:42
** 版  本:	1.0
** 描  述:	BUFF回收器
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "BuffRecycle.h"
#include "ExternalFacade.h"

/** 
@param   
@param   
@return  
*/
CBuffRecycle::CBuffRecycle(void)
{
	// 回收列表
	m_listRecycle.clear();

	// 是否启动定时器
	m_bSetCheckTimer = false;
}

/** 
@param   
@param   
@return  
*/
CBuffRecycle::~CBuffRecycle(void)
{

}

/** 压入场景对像
@param   
@param   
@return  
*/
void CBuffRecycle::Push(CBuff * pBuff)
{
	if(pBuff == NULL)
	{
		return;
	}

	// 如果超过最大数量，直接释放
	if(m_listRecycle.size() >= 1000)
	{		
		PushNoReleasePool(pBuff);
		return;
	}

	// 放入列表中
	m_listRecycle.push_back(pBuff);
}

/** 弹出场景对像
@param   
@param   
@return  
*/
CBuff *	CBuffRecycle::Pop(void)
{
	if(m_listRecycle.empty())
	{
		return NULL;
	}

	CBuff * pBuff = m_listRecycle.front();
	m_listRecycle.pop_front();

	return pBuff;
}

/** 关闭
@param   
@param   
@return  
*/
void CBuffRecycle::Close(void)
{
	OnTimer(ETimerEventID_Check);

	TLIST_RECYCLE::iterator it = m_listRecycle.begin();
	for( ; it != m_listRecycle.end(); ++it)
	{
		SAFE_RELEASE((*it));
	}
	m_listRecycle.clear();	
}

/** 
@param   
@param   
@return  
*/
void CBuffRecycle::PushNoReleasePool(CBuff * pBuff)
{
	m_listRelease.push_back(pBuff);
	
	// 是否启动定时器
	if(!m_bSetCheckTimer)
	{
		g_ExternalFacade.SetTimer(ETimerEventID_Check, 16, this, "BuffRecycle::PushNoRecycle");

		m_bSetCheckTimer = true;
	}
}

/**
@purpose          : 定时器触发后回调,你可以在这里编写处理代码
@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
@return		      : empty
*/
void CBuffRecycle::OnTimer(unsigned long dwTimerID)
{	
	// 释放放不进回收列中的buff
	TLIST_RECYCLE::iterator it = m_listRelease.begin();
	for( ; it != m_listRelease.end(); ++it)
	{
		SAFE_RELEASE(*it);
	}
	m_listRelease.clear();

	if (m_bSetCheckTimer)
	{
		g_ExternalFacade.KillTimer(ETimerEventID_Check, this);
	}

	// 是否启动定时器
	m_bSetCheckTimer = false;
}

/** 取得buff回收池的信息
@param   
@param   
@return  
*/
void CBuffRecycle::GetBuffRecycle(int &nRecycleSize, int &nReleaseSize, int &nRestoreSize)
{
	nRecycleSize = m_listRecycle.size();
	nReleaseSize = m_listRelease.size();
	nRestoreSize = 0;
}