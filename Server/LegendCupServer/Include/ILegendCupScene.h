/*******************************************************************
** 文件名:	E:\speed\Server\LegendCupServer\Include\ILegendCupScene.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	10/19/2015
** 版  本:	1.0
** 描  述:	排行榜系统场景服			
  
********************************************************************/

#pragma once
#include "LegendCupDef.h"

struct ILegendCupScene
{
	// 加载 
	virtual bool			load() = 0;
    // 执行释放Service
    virtual void            onStop() = 0;
	// 释放 
	virtual void			release() = 0;

	// 获取比赛大厅场景服服务
	virtual SERVICE_PTR		getLegendCupSceneService() = 0;

};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(LegendCupSceneServer_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(LegendCupSceneServer))
extern "C" ILegendCupScene * CreateLegendCupScene();
#	define	CreateLegendCupSceneProc	CreateLegendCupScene
#else													/// 动态库版本
typedef ILegendCupScene * (RKT_CDECL * procCreateLegendCupScene)();
#	define	CreateLegendCupSceneProc	DllApi<procCreateLegendCupScene>::load(MAKE_DLL_NAME(LegendCupServer), "CreateLegendCupScene")
#endif


