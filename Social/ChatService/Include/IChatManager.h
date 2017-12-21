/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\MatchServer\Include\IMatch.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	10/15/2015	15:45
** 版  本:	1.0
** 描  述:	
			聊天管理
********************************************************************/

#pragma once
#include "IServiceContainer.h"
#include "ChatDef.h"

struct IChatManager
{
	// 获得聊天管理器
	virtual SERVICE_PTR getChatService() = 0;
	// 加载 
	virtual bool	load() = 0;
	// 释放 
	virtual void	release() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(CHATSERVICE_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(ChatService))
extern "C" IChatManager * CreateChatManager();
#	define	CreateChatManagerProc	CreateChatManager
#else													/// 动态库版本
typedef IChatManager * (RKT_CDECL * procCreateChatManager)();
#	define	CreateChatManagerProc	DllApi<procCreateChatManager>::load(MAKE_DLL_NAME(ChatService), "CreateChatManager")
#endif