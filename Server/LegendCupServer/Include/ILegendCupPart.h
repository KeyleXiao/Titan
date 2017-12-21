/*******************************************************************
** 文件名:	__ILegendCupPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	10/19/2015
** 版  本:	1.0
** 描  述:	排行榜	

********************************************************************/

#pragma once
#include "IEntity.h"

struct __ILegendCupPart:public __IEntityPart
{

};

//////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(ENTITY))
extern "C" __ILegendCupPart * CreateLegendCupPart();
#	define	CreateLegendCupPartProc	CreateLegendCupPart
#else													/// 动态库版本
typedef __ILegendCupPart * (RKT_CDECL * procCreateLegendCupPart)();
#	define	CreateLegendCupPartProc	DllApi<procCreateLegendCupPart>::load(MAKE_DLL_NAME(LegendCupSceneServer), "CreateLegendCupPart")
#endif
