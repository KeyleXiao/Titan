/*******************************************************************
** 文件名:	ITasktClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/12/2016
** 版  本:	1.0
** 描  述:	任务接口
********************************************************************/

#pragma once

struct ITaskClient
{
	/**
	@name   : 创建
	*/
	virtual bool Create() = 0;

	/**
	@name   : 释放
	*/
	virtual bool Release() = 0;
};


////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"

#if defined(_LIB) || defined(EntityClient_STATIC_LIB)       ///静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(EntityClient))
extern "C" ITaskClient *CreateTaskClient(void);
#	define CreateTaskClientProc CreateTaskClient
#else															///动态库版本
typedef ITaskClient *(RKT_CDECL *procCreateTaskClient)(void);
#	define CreateTaskClientProc DllApi<procCreateTaskClient>::load(MAKE_DLL_NAME(EntityClient), "CreateTaskClient")
#endif     


