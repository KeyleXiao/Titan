/*******************************************************************
** 文件名:	ILuaManager.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	12/27/2016
** 版  本:	1.0
** 描  述:	Lua管理
********************************************************************/


#pragma once
#include "IServiceContainer.h"

struct ILuaManager
{
	// 获得Lus服务
	virtual SERVICE_PTR getLuaService() = 0;

    // 加载 
	virtual bool	load() = 0;

	// 释放 
	virtual void	release() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(LUASERVICE_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(LuaService))
extern "C" ILuaManager * CreateLuaManager();
#	define	CreateLuaManagerProc	CreateLuaManager
#else													/// 动态库版本
typedef ILuaManager * (RKT_CDECL * procCreateLuaManager)();
#	define	CreateLuaManagerProc	DllApi<procCreateLuaManager>::load(MAKE_DLL_NAME(LuaService), "CreateLuaManager")
#endif