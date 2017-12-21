/*******************************************************************
** 文件名:	e:\Rocket\Server\TaskServer\Include\ITaskServer.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/2  16:25
** 版  本:	1.0
** 描  述:	任务服务器
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

/*! \addtogroup TaskServer
*  任务服务器
*  @{
*/

// dwHolder大于此值为公共任务数据
#define COMMON_TASKHOLDER_ID	(0xFFFFFFFF - 1024)

/// 任务服务器
struct ITaskServer
{
	/////////////////////////基本处理//////////////////////////////////
	/** 执行Service的on_stop
	*/
    virtual void                            onStop() = NULL;

	/** 释放
	*/
	virtual void                            release(void) = NULL;
};
/** @} */

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(TASKSERVER_STATIC_LIB)	/// 静态库版本
	#	pragma comment(lib, MAKE_LIB_NAME(TaskServer))
	extern "C" ITaskServer * CreateTaskServer(void);
	#	define	CreateTaskServerProc	CreateTaskServer
#else													/// 动态库版本
	typedef ITaskServer * (RKT_CDECL *procCreateTaskServer)(void);
	#	define	CreateTaskServerProc	DllApi<procCreateTaskServer>::load(MAKE_DLL_NAME(TaskServer), "CreateTaskServer")
#endif

