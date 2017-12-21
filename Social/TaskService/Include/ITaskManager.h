/*******************************************************************
** 文件名:	ITaskManager.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	11/1/2017
** 版  本:	1.0
** 描  述:	任务数据操作管理对象

********************************************************************/

#ifndef __I_TASK_MANGER_H__
#define __I_TASK_MANGER_H__

struct ITaskStorageService;

// 任务数据操作管理对象接口
struct ITaskManager
{
    // 创建Service
    virtual bool                        create(void) = 0;

    // 停止Service
    virtual void                        onStop(void) = 0;

    // 释放资源
    virtual void                        release(void) = 0;

	// 获取服务指针
	virtual SERVICE_PTR					GetTaskService() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(TASKSERVICE_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(TaskService))
extern "C" ITaskManager * CreateTaskManager(void);
#	define	CreateTaskManagerProc	CreateTaskManager
#else													/// 动态库版本
typedef ITaskManager * (RKT_CDECL *procCreateTaskManager)(void);
#	define	CreateTaskManagerProc	DllApi<procCreateTaskManager>::load(MAKE_DLL_NAME(TaskService), "CreateTaskManager")
#endif

#endif  // __I_TASK_MANGER_H__
