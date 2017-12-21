/*******************************************************************
** 文件名:	IServiceContainer.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	3/19/2015	15:45
** 版  本:	1.0
** 描  述:	比赛大厅场景服			
  
********************************************************************/

#pragma once

#include "IServiceContainer.h"
#include "GatewayServiceDef.h"
#include "GatewayServiceDef.h"
using Gateway::ClientID;

struct IMatchScene
{
	// 加载 
	virtual bool			load() = 0;
    /** 执行Service的on_stop
	@return   
	*/
    virtual void            onStop(void)=0;
	// 释放 
	virtual void			release() = 0;

	// 获取比赛大厅场景服服务
	virtual SERVICE_PTR		getMatchSceneService() = 0;

};

struct IWarMatch
{
	// 释放 
	virtual void			release() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(MatchSceneServer_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(MatchSceneServer))
extern "C" IMatchScene * CreateMatchScene();
#	define	CreateMatchSceneProc	CreateMatchScene
#else													/// 动态库版本
typedef IMatchScene * (RKT_CDECL * procCreateMatchScene)();
#	define	CreateMatchSceneProc	DllApi<procCreateMatchScene>::load(MAKE_DLL_NAME(MatchSceneServer), "CreateMatchScene")
#endif

