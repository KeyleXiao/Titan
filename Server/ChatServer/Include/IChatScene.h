/*******************************************************************
** 文件名:	E:\speed\Server\ChatServer\Include\IChatScene.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	10/16/2015	14:41
** 版  本:	1.0
** 描  述:	聊天系统场景服			
  
********************************************************************/

#pragma once
#include "ChatDef.h"

struct IChatScene
{
	// 加载 
	virtual bool			load() = 0;
    // 执行释放Service
    virtual void            onStop() = 0;
	// 释放 
	virtual void			release() = 0;

	// 获取聊天场景服服务
	virtual SERVICE_PTR		getChatSceneService() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(ChatSceneServer_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(ChatSceneServer))
extern "C" IChatScene * CreateChatScene();
#	define	CreateChatSceneProc	CreateChatScene
#else													/// 动态库版本
typedef IChatScene * (RKT_CDECL * procCreateChatScene)();
#	define	CreateChatSceneProc	DllApi<procCreateChatScene>::load(MAKE_DLL_NAME(ChatSceneServer), "CreateChatScene")
#endif


