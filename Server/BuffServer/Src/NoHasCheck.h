﻿/*******************************************************************
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
#pragma once
#include <list>
using namespace std;

// 申明
class CBuff;
class CBuffPart;

// BUFF条件
class CNoHasCheck : public TimerHandler
{
	enum
	{
		ETimerEventID_Check = 0,	// buff检查周期
	};
public:
	//////////////////////////ITimeAxis///////////////////////////////
	/**
	@purpose          : 定时器触发后回调,你可以在这里编写处理代码
	@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
	@return		      : empty
	*/
	virtual void			OnTimer(unsigned long dwTimerID);

	///////////////////////////////////////////////////////////////////
	/** 
	@param   
	@param   
	@return  
	*/
	bool					Create(CBuff * pBuff, CBuffPart * pBuffPart, list<DWORD> &listNoBuff);

	/** 
	@param   
	@param   
	@return  
	*/
	void					Close(void);

	/** 
	@param   
	@param   
	@return  
	*/
	CNoHasCheck(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CNoHasCheck(void);	

private:
	// BUFF
	CBuff *					m_pBuff;

	// BUFF 部件
	CBuffPart *				m_pBuffPart;

	// 非BUFF条件 BUFF
	list<DWORD>				m_listNoBuff;	

	// 是否创建了
	bool					m_bCreate;
};