/*******************************************************************
** 文件名:	IChatClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	马传飞
** 日  期:	2015/10/19  17:21
** 版  本:	1.0
** 描  述:	聊天系统客户端
** 应  用:  	
	
*******************************************************************/

#pragma once

#include "ChatDef.h"
#include "IChatPart.h"

struct IChatClient
{
	// 释放
	virtual void release(void) = 0;

	// 给实体创建聊天部件，重新new的部件才调此方法。如果是回收的，则不需要
	virtual IChatPart* CreatePart(void) = 0;

	// 显示系统提示(参数用分号分隔)
	// @param tipID: EMChatTipID
	virtual void showSystemMessage(int tipID, LPCSTR szParam = NULL, WORD nReserveData = 0) const = 0;

	// 获取系统提示
	virtual std::string getSystemTipText(LPCSTR szParam, int nTipID) const = 0;
};


#if defined(_LIB) || defined(FREEZECLIENT_STATIC_LIB)	// 静态链接
#	pragma comment(lib, MAKE_LIB_NAME(ChatClient))
extern "C" IChatClient * CreateChatClient(void);
#	define	CreateChatClientProc	CreateChatClient
#	define	CloseChatClientProc()
#elif !defined(FREEZECLIENT_EXPORTS) && defined(DYN_LIB) // 动态链接
#	pragma comment(lib, MAKE_DLL_LIB_NAME(ChatClient))
API_IMPORT IChatClient * CreateChatClient(void);
#	define	CreateChatClientProc	CreateChatClient
#	define	CloseChatClientProc()
#else													// 动态加载
typedef IChatClient * (RKT_CDECL *procCreateChatClient)(void);
#	define	CreateChatClientProc	DllApi<procCreateChatClient>::load(MAKE_DLL_NAME(ChatClient), "CreateChatClient")
#	define	CloseChatClientProc	DllApi<procCreateChatClient>::freelib
#endif