/*******************************************************************
** 文件名:	IAntiAddictClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	周坤
** 日  期:	11/15/2016
** 版  本:	1.0
** 描  述:	

********************************************************************/

#pragma once

// 防沉迷服务
struct IAntiAddictClient
{
	/**
	@name   : 创建防沉迷服务
	*/
	virtual bool Create() = 0;

	/**
	@name   : 释放防沉迷服务
	*/
	virtual bool Release() = 0;
};


////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"

#if defined(_LIB) || defined(EntityClient_STATIC_LIB)       ///静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(EntityClient))
extern "C" IAntiAddictClient *CreateAntiAddictClient(void);
#	define CreateAntiAddictClientProc CreateAntiAddictClient
#else															///动态库版本
typedef IAntiAddictClient *(RKT_CDECL *procCreateAntiAddictClient)(void);
#	define CreateAntiAddictClientProc DllApi<procCreateAntiAddictClient>::load(MAKE_DLL_NAME(EntityClient), "CreateAntiAddictClient")
#endif     


