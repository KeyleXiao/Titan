/*******************************************************************
** 文件名:	NoHasCheck.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/14  16:19
** 版  本:	1.0
** 描  述:	非BUFF条件（此处只是"不停地检查"的检查方式）
** 应  用:  	
当目标不处于相应ID的BUFF时BUFF才会加载；可填多个，代表与的关系	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "NoHasCheck.h"
#include "Buff.h"
#include "BuffPart.h"
#include "ExternalFacade.h"

/** 
@param   
@param   
@return  
*/
CNoHasCheck::CNoHasCheck(void)
{
	// BUFF
	m_pBuff = NULL;

	// BUFF 部件
	m_pBuffPart = NULL;

	// 非BUFF条件 BUFF
	m_listNoBuff.clear();

	// 是否创建了
	m_bCreate = false;
}

/** 
@param   
@param   
@return  
*/
CNoHasCheck::~CNoHasCheck(void)
{

}

/** 
@param   
@param   
@return  
*/
bool CNoHasCheck::Create(CBuff * pBuff, CBuffPart * pBuffPart, list<DWORD> &listNoBuff)
{
	if(pBuff == NULL || pBuffPart == NULL|| listNoBuff.empty())
	{
		return false;
	}

	// BUFF
	m_pBuff = pBuff;

	// BUFF 部件
	m_pBuffPart = pBuffPart;

	// BUFF条件 BUFF
	m_listNoBuff = listNoBuff;

	// 是否创建了
	m_bCreate = true;

	// 启一个一秒的定时器
	g_ExternalFacade.SetTimer(ETimerEventID_Check, 1000, this, "CNoHasCheck::Create");

	return true;
}

/** 
@param   
@param   
@return  
*/
void CNoHasCheck::Close(void)
{
	// BUFF
	m_pBuff = NULL;

	// BUFF 部件
	m_pBuffPart = NULL;

	// 非BUFF条件 BUFF
	m_listNoBuff.clear();

	// 是否创建了
	m_bCreate = false;

	// kill定时器
	g_ExternalFacade.KillTimer(ETimerEventID_Check, this);
}

/**
@purpose          : 定时器触发后回调,你可以在这里编写处理代码
@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
@return		      : empty
*/
void CNoHasCheck::OnTimer(unsigned long dwTimerID)
{
	if(m_pBuff == NULL || m_pBuffPart == NULL)
	{
		return;
	}

	list<DWORD>::iterator it = m_listNoBuff.begin();
	for( ; it != m_listNoBuff.end(); ++it)
	{
		CBuff * outbuff[256]; int nOutQty = 256;
		m_pBuffPart->GetBuff((*it), outbuff, nOutQty);
		if(nOutQty > 0)
		{
			return;
		}
	}

	// 启动BUFF
	m_pBuff->StartEffect();

	// 关闭
	Close();
}