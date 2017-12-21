/*******************************************************************
** 文件名:	ISNSManager.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	12/6/2016
** 版  本:	1.0
** 描  述:	SNS管理
********************************************************************/

#pragma once
#include "IServiceContainer.h"

struct ISNSManager
{
	// 获得SNS管理器
	virtual SERVICE_PTR getSNSService() = 0;
	// 加载 
	virtual bool	load() = 0;
	// 释放 
	virtual void	release() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(SNSSERVICE_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(SNSService))
extern "C" ISNSManager * CreateSNSManager();
#	define	CreateSNSManagerProc	CreateSNSManager
#else													/// 动态库版本
typedef ISNSManager * (RKT_CDECL * procCreateSNSManager)();
#	define	CreateSNSManagerProc	DllApi<procCreateSNSManager>::load(MAKE_DLL_NAME(SNSService), "CreateSNSManager")
#endif