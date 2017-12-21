/*******************************************************************
** 文件名:	IClanScene.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/6/2016
** 版  本:	1.0
** 描  述:	帮会场景接口
********************************************************************/

#pragma once
#include "ClanDef.h"
#include "IServiceContainer.h"
#include "GatewayServiceDef.h"
using Gateway::ClientID;

// 帮会场景接口
struct IClanScene
{
	// 加载 
	virtual bool			load() = 0;
    // 执行释放Service
    virtual void            onStop() = 0;
	// 释放 
	virtual void			release() = 0;

	// 获取帮会服务
	virtual SERVICE_PTR		getClanSceneService() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(CLANSCENESERVER_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(ClanSceneServer))
extern "C" IClanScene * CreateClanScene();
#	define	CreateClanSceneProc	CreateClanScene
#else													/// 动态库版本
typedef IClanScene * (RKT_CDECL * procCreateClanScene)();
#	define	CreateClanSceneProc	DllApi<procCreateClanScene>::load(MAKE_DLL_NAME(ClanServer), "CreateClanScene")
#endif


