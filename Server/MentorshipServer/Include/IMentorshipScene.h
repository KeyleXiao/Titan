/*******************************************************************
** 文件名:	IMentorshipScene.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	11/29/2016
** 版  本:	1.0
** 描  述:	师徒系统场景接口
********************************************************************/

#pragma once
#include "MentorshipDef.h"
#include "IServiceContainer.h"
#include "GatewayServiceDef.h"
using Gateway::ClientID;

// 帮会场景接口
struct IMentorshipScene
{
	// 加载 
	virtual bool			load() = 0;
    // 执行释放Service
    virtual void            onStop() = 0;
	// 释放 
	virtual void			release() = 0;

	// 获取帮会服务
	virtual SERVICE_PTR		getMentorshipSceneService() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(MENTORSHIPSCENESERVER_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(MentorshipSceneServer))
extern "C" IMentorshipScene * CreateMentorshipScene();
#	define	CreateMentorshipSceneProc	CreateMentorshipScene
#else													/// 动态库版本
typedef IMentorshipScene * (RKT_CDECL * procCreateMentorshipScene)();
#	define	CreateMentorshipSceneProc	DllApi<procCreateMentorshipScene>::load(MAKE_DLL_NAME(MentorshipServer), "CreateMentorshipScene")
#endif


