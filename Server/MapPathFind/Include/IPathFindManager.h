/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\MapPathFind\Include\IPathFindManager.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	3/12/2015	16:24
** 版  本:	1.0
** 描  述:	
            寻路管理器
			加载所有的寻路服务,运行期间一直缓存着

			调用方式
			gServerGlobal->getPathFindManager()->***();
********************************************************************/

#pragma once

#include "IServiceContainer.h"

struct IPathFindService;
struct IFramework;

struct IPathFindManager
{
    // 下面是系统调用的，已经写死位置，使用者不要调用
    //////////////////////////////////////////////////////////////////////////
    // 加载所有寻路数据
    virtual bool                Load() = 0;

    // 释放寻路管理器
    virtual void                Release() = 0;

    virtual IPathFindService*   createServiceInterface( int nMapID,int nSceneID,BYTE mode ) = 0;
    virtual void                releaseServiceInterface( IPathFindService *& pInterface ) = 0;

	// 辅助函数 方便调用 
	//////////////////////////////////////////////////////////////////////////
    // 结合地图ID，场景ID，寻路起点和终点，来获取寻路路径
	virtual bool                GetPathFromMapID(int nMapID, int nSceneID,
                                                 Vector3 const& v3Start, Vector3 const& v3End, vector<Vector3>& vecRet,
                                                 int nFlag = 0x0, int nCamp = 0x0,
                                                 float fWidthMin = 0) = 0;
    virtual bool                checkObstacle(int nMapID, int nSceneID, Vector3 const& v3Pos, const float *verts, const int nVerts,
                                              int nFlag = 0x0, int nType = 0x0) = 0;
	virtual bool                getHeight(int nMapID, int nSceneID, Vector3* v3Pos, float& fHeight) = 0;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(MapPathFind_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(PathFind))
extern "C" IPathFindManager * CreatePathFindManager();
#	define	CreatePathFindManagerProc	CreatePathFindManager
#else													/// 动态库版本
typedef IPathFindManager * (RKT_CDECL * procCreatePathFindManager)();
#	define	CreatePathFindManagerProc	DllApi<procCreatePathFindManager>::load(MAKE_DLL_NAME(MapPathFind), "CreatePathFindManager")
#endif

