/*******************************************************************
** 文件名:	IWarPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林飞
** 日  期:	03/06/2015
** 版  本:	1.0
** 描  述:	

********************************************************************/

#pragma once

#include "IEntity.h"

// 实体状态部件接口
struct __IWarPart : public __IEntityPart
{
    virtual int getWarID() = 0;
    virtual int getIndex() = 0;  // 几楼
    virtual int getSmSkillId(int nSlot) = 0;

    virtual int getMatchTypeID() = 0;

    virtual int getWarTypeID() = 0;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"

#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(ENTITY))
extern "C" __IWarPart * CreateWarPart();
#	define	CreateWarPartProc	CreateWarPart
#else													/// 动态库版本
typedef __IWarPart * (RKT_CDECL * procCreateWarPart)();
#	define	CreateWarPartProc	DllApi<procCreateWarPart>::load(MAKE_DLL_NAME(WarServer), "CreateWarPart")
#endif
