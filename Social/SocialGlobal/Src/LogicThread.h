/*******************************************************************
** 文件名:	LogicThread.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-08-26
** 版  本:	1.0
** 描  述:	DB引擎独用，逻辑线程	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include <list>
using namespace std;
#include "Thread.h"
using namespace rkt;
#include "ILogicThread.h"


// 逻辑线程
class CLogicThread : public ILogicThread, public IRunnable
{	
	struct STaskInfo
	{
		ILogicThreadTask *		pTask;

		// 调试信息
		char					szDebug[32];

		STaskInfo(void)
		{
			memset(this, 0, sizeof(STaskInfo));
		}
	};

public:
	////////////////////////IRunnable//////////////////////////////////
	/** 
	@param   
	@param   
	@return  
	*/
	virtual void				run();

	/** 
	@param   
	@param   
	@return  
	*/
	virtual void				release();

	////////////////////////ILogicThread///////////////////////////////
	/** 在逻辑线程上挂一个任务
	@param   
	@param   
	@return  
	*/
	virtual bool				AttachTask(ILogicThreadTask * pTask, LPCSTR pszDebug);

	/** 在逻辑线程上取消一个任务
	@param   
	@param   
	@return  
	*/
	virtual void				DetachTask(ILogicThreadTask * pTask);


	///////////////////////CLogicThread////////////////////////////////
	/** 停止线程
	@param   
	@param   
	@return  
	*/
	void						StopThread(void);

	/** 
	@param   
	@param   
	@return  
	*/
	CLogicThread(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CLogicThread(void);
private:
	// 当前正在用的任务
	STaskInfo					m_Task;

	// 是否停止线程
	bool						m_bStopThread;

	// 上次线程信号的时间
	LARGE_INTEGER				m_litmpThreadLast;

	// 获得当前cpu的时钟频率
	double						m_fCPUFreq;
};