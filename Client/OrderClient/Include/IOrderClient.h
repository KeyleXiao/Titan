/*******************************************************************
** 文件名:	IOrderClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2015/10/19  17:21
** 版  本:	1.0
** 描  述:	排行榜系统客户端
** 应  用:  		
	
*******************************************************************/

#pragma once
#include "OrderMsgDef.h"

struct IOrderClient
{
	/** 释放
	@param   
	@param   
	@param   
	@return  
	@note     释放所有资源，并且销毁此对像
	@warning 
	@retval buffer 
	*/
	virtual void			release(void) = NULL;

};


#if defined(_LIB) || defined(FREEZECLIENT_STATIC_LIB)	// 静态链接
#	pragma comment(lib, MAKE_LIB_NAME(OrderClient))
extern "C" IOrderClient * CreateOrderClient(void);
#	define	CreateOrderClientProc	CreateOrderClient
#	define	CloseOrderClientProc()
#elif !defined(FREEZECLIENT_EXPORTS) && defined(DYN_LIB) // 动态链接
#	pragma comment(lib, MAKE_DLL_LIB_NAME(OrderClient))
API_IMPORT IOrderClient * CreateOrderClient(void);
#	define	CreateOrderClientProc	CreateOrderClient
#	define	CloseOrderClientProc()
#else													// 动态加载
typedef IOrderClient * (RKT_CDECL *procCreateOrderClient)(void);
#	define	CreateOrderClientProc	DllApi<procCreateOrderClient>::load(MAKE_DLL_NAME(OrderClient), "CreateOrderClient")
#	define	CloseOrderClientProc	DllApi<procCreateOrderClient>::freelib
#endif