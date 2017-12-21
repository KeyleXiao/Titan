/*****************************************************************************
** 文件名:	IPathFindManager.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	彭璐
** 日  期:	2015/07/14
** 版  本:	1.0
** 描  述:	寻路管理接口
** 应  用:
**************************** 修改记录 ****************************************
** 修改人:  
** 日  期:  
** 描  述:  
******************************************************************************/

#pragma once

struct IPathFindService;

struct IPathFindManager
{
    // 下面是系统调用的，已经写死位置，使用者不要调用
    //////////////////////////////////////////////////////////////////////////
    // 加载所有寻路数据
    virtual bool                load() = 0;

    // 释放寻路管理器
    virtual void                release() = 0;

    virtual IPathFindService*   createServiceInterface( int nMapID,int nSceneID ) = 0;

    virtual void                releaseServiceInterface( IPathFindService *& pInterface ) = 0;

     virtual IPathFindService*  getPathFindService() = 0;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(MapPathFindClient_STATIC_LIB)      /// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(MapPathFindClient))
extern "C" IPathFindManager * CreatePathFindManager();
#	define	CreatePathFindManagerProc	CreatePathFindManager
#else													        /// 动态库版本
typedef IPathFindManager * (__cdecl * procCreatePathFindManager)();
#	define	CreatePathFindManagerProc	DllApi<procCreatePathFindManager>::load(MAKE_DLL_NAME(MapPathFindClient), "CreatePathFindManager")
#endif

