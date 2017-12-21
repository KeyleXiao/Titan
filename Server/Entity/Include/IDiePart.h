/*******************************************************************
** 文件名:	IDiePart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	1/8/2015
** 版  本:	1.0
** 描  述:	实体死亡部件接口

********************************************************************/

#pragma once

#include "IEntity.h"

// 实体死亡部件接口
struct __IDiePart : public __IEntityPart
{
	/** 实体复活
	@return  
	*/
	virtual bool relive() = 0;

	/** 取得复活信息
	@return  
	*/
    virtual bool getReliveInfo(int &nSceneID, float &fLocX, float &fLocY, float &fLocZ, float &fAngleY) = 0;

	// 复活时设置属性(效果结束时设置属性有些不允许死亡状态下设置 要延迟到复活后。效果订阅复活事件不可行，因为复活时效果可能早就销毁了)
	virtual void setIntPropertyOnRelive(int nProperty, int nValue) = 0;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"

#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(ENTITY))
extern "C" __IDiePart * CreateDiePart();
#	define	CreateDiePartProc	CreateDiePart
#else													/// 动态库版本
typedef __IDiePart * (RKT_CDECL * procCreateDiePart)();
#	define	CreateDiePartProc	DllApi<procCreateDiePart>::load(MAKE_DLL_NAME(ENTITY), "CreateDiePart")
#endif
