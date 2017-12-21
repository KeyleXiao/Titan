/*******************************************************************
** 文件名:	ILogoutPart.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	
** 日  期:	
** 版  本:	1.0
** 描  述:	实体下线、存盘 部件

********************************************************************/

#pragma once

#include "IEntity.h"

// 实体数据仓库部件接口
struct __ILogoutPart : public __IEntityPart
{
    virtual int getLogoutReason() = 0;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"

#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(ENTITY))
extern "C" __ILogoutPart * CreateLogoutPart();
#	define	CreateLogoutPartProc	CreateLogoutPart
#else													/// 动态库版本
typedef __ILogoutPart * (RKT_CDECL * procCreateLogoutPart)();
#	define	CreateLogoutPartProc	DllApi<procCreateLogoutPart>::load(MAKE_DLL_NAME(ENTITY), "CreateLogoutPart")
#endif
