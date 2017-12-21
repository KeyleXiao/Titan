/*******************************************************************
** 文件名:	ISceneManager.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/30/2014
** 版  本:	1.0
** 描  述:	

			场景管理器
********************************************************************/

#pragma once


#include "IGameSceneService.h"

#define IS_DYNAMIC_SCENE(sceneID) ((sceneID) >= MAP_MAXCOUNT + 1)

/**
	负责场景的创建，销毁，场景对象的查询，甚至动态负载等
*/
struct ISceneManager
{
	virtual void onStop() = 0;

	virtual void release() = 0;

	// 加载场景
	virtual bool load() = 0;

	// 创建场景
	// @param mapID : 传入地图ID，同一张地图可以创建多个副本 是否是副本
	virtual int createScene(SCreateSceneContext context, void* pUserData = NULL, int len = 0) = 0;

	// 关闭场景
	virtual bool closeScene( int sceneID ) = 0;

	// 获取场景对象 传入场景ID
	virtual SERVICE_PTR getScene( int sceneID ) = 0;

	// 获取服务器中当前场景总数
	virtual int getSceneCount() = 0;

	// 获取所有场景列表
	virtual int getAllScenes( SERVICE_PTR* pReturnArray,int nArraySize ) = 0;

    // 获取所有场景ID
    virtual int getAllSceneID( int* pReturnArray, int nArraySize ) = 0;

	virtual void clearAllScenes() = 0;

    // 场景service真正退出 回调
    virtual void onSceneClosed( int sceneID ) = 0;

    virtual int sceneIDToMapID( int sceneID ) = 0;

    virtual int sceneIDToSerialNo( int sceneID ) = 0;

    // 获取本场景服开启的主城镜像地图区间值
    virtual void getMirrorMapID(BYTE& nMin, BYTE &nMax) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(AOI_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(AOI))
extern "C" ISceneManager * CreateSceneManager();
#	define	CreateSceneManagerProc	CreateSceneManager
#else													/// 动态库版本
typedef ISceneManager * (RKT_CDECL * procCreateSceneManager)();
#	define	CreateSceneManagerProc	DllApi<procCreateSceneManager>::load(MAKE_DLL_NAME(AOI), "CreateSceneManager")
#endif

