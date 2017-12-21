/*******************************************************************
** 文件名:	IWingPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/21/2016
** 版  本:	1.0
** 描  述:	翅膀部件接口
********************************************************************/

#pragma once

#include "IEntity.h"

struct __IWingPart : public __IEntityPart
{
    // 设置翅膀ID
    // @param nWingID : 翅膀ID
    virtual bool SetWingID(int nWingID) = 0;

    // 取得翅膀ID
    virtual int GetWingID(void) = 0;

    // 设置翅膀状态
    // @param nWingID : 翅膀状态
    virtual void SetWingState(int nWingState) = 0;

    // 取得翅膀状态
    virtual int GetWingState(void) = 0;

    // 设置翅膀飞行时间
    // @param nTime : 飞行时间
    virtual void SetFlyTime(int nTime) = 0;

    // 增加翅膀飞行时间
    // @param nTime : 增加飞行时间
    virtual void AddFlyTime(int nTime) = 0;

    // 取得翅膀飞行时间
    virtual int GetFlyTime(void) = 0;

    // 检测改变翅膀状态
    virtual bool CheckChangeWingState(int nStateType) = 0;

    // 设置翅膀属性
    // @param nType  : 翅膀属性
    // @param fValue : 翅膀数值
    virtual void SetWingProperty(int nType, float fValue) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"

#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(ENTITY))
extern "C" __IWingPart * CreateWingPart();
#	define	CreateWingPartProc		CreateWingPart
#else													/// 动态库版本
typedef __IWingPart * (RKT_CDECL * procCreateWingPart)();
#	define	CreateWingPartProc	DllApi<procCreateWingPart>::load(MAKE_DLL_NAME(ENTITY), "CreateWingPart")
#endif
