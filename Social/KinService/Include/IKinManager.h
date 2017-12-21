/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\MatchServer\Include\IKinMananger.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	10/15/2015	15:45
** 版  本:	1.0
** 描  述:	战队管理接口
********************************************************************/

#pragma once
#include "IServiceContainer.h"
#include "KinDef.h"

struct IKinManager
{
	// 获得聊天管理器
	virtual SERVICE_PTR GetKinService() = 0;

	// 加载 
	virtual bool	Load() = 0;
	// 释放 
	virtual void	Release() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(KINSERVICE_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(KinService))
extern "C" IKinManager * CreateKinManager();
#	define	CreateKinManagerProc	CreateKinManager
#else													/// 动态库版本
typedef IKinManager * (RKT_CDECL * procCreateKinManager)();
#	define	CreateKinManagerProc	DllApi<procCreateKinManager>::load(MAKE_DLL_NAME(KinService), "CreateKinManager")
#endif