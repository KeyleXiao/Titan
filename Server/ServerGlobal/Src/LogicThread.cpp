/*******************************************************************
** 文件名:	LogicThread.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/1/31  11:16
** 版  本:	1.0
** 描  述:	逻辑线程，游戏逻辑主线程
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "LogicThread.h"
#include "net/AsynIoFrame.h"


/** 
@param   
@param   
@return  
*/
CLogicThread::CLogicThread(void)
{
	// 是否停止线程
	m_bStopThread = false;

	// 获得当前cpu的时钟频率
	LARGE_INTEGER litmp;::QueryPerformanceFrequency(&litmp);
	m_fCPUFreq = (double)litmp.QuadPart;
}

/** 
@param   
@param   
@return  
*/
CLogicThread::~CLogicThread(void)
{
	
}

/** 
@param   
@param   
@return  
*/
void CLogicThread::run()
{
	while(true)
	{		
		if(m_bStopThread)
		{
			break;
		}
		
		if(m_Task.pTask != NULL)
		{
			m_Task.pTask->OnDo();
		}

        ::Sleep(16);
	}

}

/** 
@param   
@param   
@return  
*/
void CLogicThread::release()
{
	// 是否停止线程
	m_bStopThread = false;
}

/** 在逻辑线程上挂一个任务
@param   
@param   
@return  
*/
bool CLogicThread::AttachTask(ILogicThreadTask * pTask, LPCSTR pszDebug)
{
	if( pszDebug == NULL)
	{
		return false;
	}

	m_Task.pTask = pTask;
	sstrcpyn(m_Task.szDebug, pszDebug, sizeof(m_Task.szDebug));

	EmphasisLn( "CLogicThread::AttachTask [" << pszDebug << "]" );

	return true;
}

/** 在逻辑线程上取消一个任务
@param   
@param   
@return  
*/
void CLogicThread::DetachTask(ILogicThreadTask * pTask)
{
	if(m_Task.pTask == pTask)
	{
		memset(&m_Task, 0, sizeof(STaskInfo));		
	}
}

/** 停止线程
@param   
@param   
@return  
*/
void CLogicThread::StopThread(void)
{
	m_bStopThread = true;
}