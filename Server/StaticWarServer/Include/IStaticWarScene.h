/*******************************************************************
** 文件名:	E:\speed\Server\OrderServer\Include\IStaticWarScene.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	10/19/2015
** 版  本:	1.0
** 描  述:	所有战场数据服务			
  
********************************************************************/

#pragma once

struct IStaticWarScene
{
	// 加载 
	virtual bool			load() = 0;
    // 执行释放Service
    virtual void            onStop() = 0;
	// 释放 
	virtual void			release() = 0;

	// 获取比赛大厅场景服服务
	virtual SERVICE_PTR		getStaticWarSceneService() = 0;

};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(StaticWarSceneServer_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(StaticWarSceneServer))
extern "C" IStaticWarScene * CreateStaticWarScene();
#	define	CreateStaticWarSceneProc	CreateStaticWarScene
#else													/// 动态库版本
typedef IStaticWarScene * (RKT_CDECL * procCreateStaticWarScene)();
#	define	CreateStaticWarSceneProc	DllApi<procCreateStaticWarScene>::load(MAKE_DLL_NAME(StaticWarServer), "CreateStaticWarScene")
#endif


