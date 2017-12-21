/*******************************************************************
** 文件名:	IAntiAddictServer.h  
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	11/8/2016
** 版  本:	1.0
** 描  述:	防沉迷-未成年

********************************************************************/

#pragma once


// 场景服防沉迷服务器接口
struct IAntiAddictServer
{
	/** 释放service服务
	*/
    virtual void onstop(void) = NULL;

	/** 释放对象
	*/
    virtual void release(void) = NULL;

    /** 取得玩家的衰减比例
    @param  : 角色uid
    @return : 返回衰减比例（x/100）,即返回，表示衰减一半
	*/
    virtual int getReduceRate(UID uid, int nPlayLevel=0) = NULL;

	/** 设置防沉迷等级
	*/
    virtual void setReduceLevel(int nReduceLevel) = NULL;

	/** 查看防沉迷等级
	*/
    virtual int getReduceLevel() = NULL;

    /** 玩家下线事件处理
    */
    virtual void onActorLogout(PDBID pdbid, UID uid, DWORD dwUserID, bool bAudlt, DWORD nIdCard) = 0;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"

#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(ENTITY))
extern "C" IAntiAddictServer * CreateAntiAddictServer();
#	define	CreateAntiAddictServerProc	CreateAntiAddictServer
#else													/// 动态库版本
typedef IAntiAddictServer * (RKT_CDECL * procCreateAntiAddictServer)();
#	define	CreateAntiAddictServerProc	DllApi<procCreateAntiAddictServer>::load(MAKE_DLL_NAME(ENTITY), "CreateAntiAddictServer")
#endif
