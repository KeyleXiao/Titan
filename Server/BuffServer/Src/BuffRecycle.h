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
#pragma once

#include <list>
using namespace std;
#include "Buff.h"

class CBuffRecycle : public TimerHandler
{
	typedef list<CBuff *>	TLIST_RECYCLE;

	enum
	{
		ETimerEventID_Check = 0,	// 检查生命周期
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
	/** 压入场景对像
	@param   
	@param   
	@return  
	*/
	void					Push(CBuff * pBuff);

	/** 弹出场景对像
	@param   
	@param   
	@return  
	*/
	CBuff *					Pop(void);

	/** 关闭
	@param   
	@param   
	@return  
	*/
	void					Close(void);

	/** 取得buff回收池的信息
	@param   
	@param   
	@return  
	*/
	void					GetBuffRecycle(int &nRecycleSize, int &nReleaseSize, int &nRestoreSize);

	/** 
	@param   
	@param   
	@return  
	*/
	CBuffRecycle(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CBuffRecycle(void);

private:
	/** 
	@param   
	@param   
	@return  
	*/
	void					PushNoReleasePool(CBuff * pBuff);

private:
	// 回收列表
	TLIST_RECYCLE			m_listRecycle;

	// 等待释放
	TLIST_RECYCLE			m_listRelease;

	// 是否启动定时器
	bool					m_bSetCheckTimer;
};
