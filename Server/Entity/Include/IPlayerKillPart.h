/*******************************************************************
** 文件名:	IPlayerKillPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	8/04/2016
** 版  本:	1.0
** 描  述:	PK部件接口
********************************************************************/

#pragma once

#include "IEntity.h"

struct AOI_Proxy;

struct __IPlayerKillPart : public __IEntityPart
{
    // 选择目标
    virtual bool chooseTarget(int nTargetFilter, UID uidTarget) = 0;
    // 设置PK类型
    virtual void setPKType(int nType) = 0;
    // 获取PK类型
    virtual int getPKType(void) = 0;
    // 设置切磋目标
    virtual void setFightTarget(UID uidTarget) = 0;
    // 检测切磋
    virtual bool checkPK(UID uidTarget) = 0;
    // 获取PK状态
    virtual int getPKState(void) = 0;

    virtual void destroy() = 0;

    virtual void  restore() = 0;

    virtual void  forceEndPK() = 0;

	// 设置PK状态
	virtual void setPKState(int nPKState) = 0;

	// 获取自己的阵营
	virtual int getPKCamp() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"

#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(ENTITY))
extern "C" __IPlayerKillPart * CreatePlayerKillPart();
#	define	CreatePlayerKillPartProc		CreatePlayerKillPart
#else													/// 动态库版本
typedef __IPlayerKillPart * (RKT_CDECL * procCreatePlayerKillPart)();
#	define	CreatePlayerKillPartProc	DllApi<procCreatePlayerKillPart>::load(MAKE_DLL_NAME(ENTITY), "CreatePlayerKillPart")
#endif
