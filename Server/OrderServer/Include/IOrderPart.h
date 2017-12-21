/*******************************************************************
** 文件名:	__IOrderPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	10/19/2015
** 版  本:	1.0
** 描  述:	排行榜	

********************************************************************/

#pragma once
#include "IEntity.h"

struct __IOrderPart:public __IEntityPart
{

};

//////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(ENTITY))
extern "C" __IOrderPart * CreateOrderPart();
#	define	CreateOrderPartProc	CreateOrderPart
#else													/// 动态库版本
typedef __IOrderPart * (RKT_CDECL * procCreateOrderPart)();
#	define	CreateOrderPartProc	DllApi<procCreateOrderPart>::load(MAKE_DLL_NAME(OrderSceneServer), "CreateOrderPart")
#endif
