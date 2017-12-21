/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\MatchServer\Include\IMatch.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	10/15/2015	15:45
** 版  本:	1.0
** 描  述:	
			排行榜管理
********************************************************************/

#pragma once
#include "IServiceContainer.h"
#include "OrderDef.h"

struct IOrderManager
{
	// 获得聊天管理器
	virtual SERVICE_PTR getOrderService() = 0;

	// 加载 
	virtual bool	load() = 0;
	// 释放 
	virtual void	Release() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(ORDERSERVICE_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(OrderService))
extern "C" IOrderManager * CreateOrderManager();
#	define	CreateOrderManagerProc	CreateOrderManager
#else													/// 动态库版本
typedef IOrderManager * (RKT_CDECL * procCreateOrderManager)();
#	define	CreateOrderManagerProc	DllApi<procCreateOrderManager>::load(MAKE_DLL_NAME(OrderService), "CreateOrderManager")
#endif