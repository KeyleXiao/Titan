/*******************************************************************
** 文件名:	IDragonBallPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林飞
** 日  期:	03/06/2015
** 版  本:	1.0
** 描  述:	

********************************************************************/

#pragma once

#include "IEntity.h"

// 实体状态部件接口
struct __IDragonBallPart : public __IEntityPart
{
	
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"

#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(ENTITY))
extern "C" __IDragonBallPart * CreateDragonBallPart();
#	define	CreateDragonBallPartProc	CreateDragonBallPart
#else													/// 动态库版本
typedef __IDragonBallPart * (RKT_CDECL * procCreateDragonBallPart)();
#	define	CreateDragonBallPartProc	DllApi<procCreateDragonBallPart>::load(MAKE_DLL_NAME(ENTITY), "CreateDragonBallPart")
#endif
