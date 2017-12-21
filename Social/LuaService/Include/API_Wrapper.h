/*******************************************************************
** 文件名:	API_Wrapper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	12/27/2016
** 版  本:	1.0
** 描  述:	社会服API封装
********************************************************************/

#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(LUA_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(LUA))
extern "C" bool BindLuaAPI(ILuaEngine * pLua);
#	define	BindLuaAPIProc	BindLuaAPI
#else													/// 动态库版本
typedef bool (RKT_CDECL * procBindLuaAPI)(ILuaEngine * pLua);
#	define	BindLuaAPIProc	DllApi<procBindLuaAPI>::load(MAKE_DLL_NAME(LuaService), "BindLuaAPI")
#endif