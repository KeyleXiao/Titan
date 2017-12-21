/*******************************************************************
** 文件名:	IMailClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	12/2/2017
** 版  本:	1.0
** 描  述:	邮件客户端接口
********************************************************************/

#ifndef __IMAILCLIENT_H__
#define __IMAILCLIENT_H__

struct IMailClient
{
	// 释放
	virtual void release(void) = 0;
};


#if defined(_LIB) || defined(MAILCLIENT_STATIC_LIB)	// 静态链接
#	pragma comment(lib, MAKE_LIB_NAME(MailClient))
extern "C" IMailClient * CreateMailClient(void);
#	define	CreateMailClientProc	CreateMailClient
#	define	CloseMailClientProc()
#elif !defined(MAILCLIENT_EXPORTS) && defined(DYN_LIB) // 动态链接
#	pragma comment(lib, MAKE_DLL_LIB_NAME(MailClient))
API_IMPORT IMailClient * CreateMailClient(void);
#	define	CreateMailClientProc	CreateMailClient
#	define	CloseMailClientProc()
#else													// 动态加载
typedef IMailClient * (RKT_CDECL *procCreateMailClient)(void);
#	define	CreateMailClientProc	DllApi<procCreateMailClient>::load(MAKE_DLL_NAME(MailClient), "CreateMailClient")
#	define	CloseMailClientProc	DllApi<procCreateMailClient>::freelib
#endif

#endif // __IMAILCLIENT_H__