/*******************************************************************
** 文件名:	ISNSClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李有红
** 日  期:	12/7/2016
** 版  本:	1.0
** 描  述:	SNS系统客户端
** 应  用:  	
	
*******************************************************************/

#pragma once

#include "SNSDef.h"

struct ISNSClient
{
	// 释放
	virtual void release(void) = 0;
};


#if defined(_LIB) || defined(FREEZECLIENT_STATIC_LIB)	// 静态链接
#	pragma comment(lib, MAKE_LIB_NAME(SNSClient))
extern "C" ISNSClient * CreateSNSClient(void);
#	define	CreateSNSClientProc	CreateSNSClient
#	define	CloseSNSClientProc()
#elif !defined(FREEZECLIENT_EXPORTS) && defined(DYN_LIB) // 动态链接
#	pragma comment(lib, MAKE_DLL_LIB_NAME(SNSClient))
API_IMPORT ISNSClient * CreateSNSClient(void);
#	define	CreateSNSClientProc	CreateSNSClient
#	define	CloseSNSClientProc()
#else													// 动态加载
typedef ISNSClient * (RKT_CDECL *procCreateSNSClient)(void);
#	define	CreateSNSClientProc	DllApi<procCreateSNSClient>::load(MAKE_DLL_NAME(SNSClient), "CreateSNSClient")
#	define	CloseSNSClientProc	DllApi<procCreateSNSClient>::freelib
#endif