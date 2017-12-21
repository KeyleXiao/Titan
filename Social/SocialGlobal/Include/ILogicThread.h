﻿/*******************************************************************
** 文件名:	ILogicThread.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-08-26
** 版  本:	1.0
** 描  述:	DB引擎独用，逻辑线程任务管理器			
********************************************************************/

#pragma once


/// 逻辑线程任务接口
struct ILogicThreadTask
{
	/** 逻辑线程信号到时，会调此接口
	@param   
	@param   
	@return  返回true:表示操作已处理完，返回false:表示操作未处理完
	*/
	virtual bool			OnDo(void) = NULL;
};


/// 逻辑线程任务管理器
struct ILogicThread
{
	/** 在逻辑线程上挂一个任务
	@param   
	@param   
	@return  
	*/
	virtual bool			AttachTask(ILogicThreadTask * pTask, LPCSTR pszDebug) = NULL;

	/** 在逻辑线程上取消一个任务
	@param   
	@param   
	@return  
	*/
	virtual void			DetachTask(ILogicThreadTask * pTask) = NULL;
};
