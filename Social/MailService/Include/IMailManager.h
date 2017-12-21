/*******************************************************************
** 文件名:	speed\Server\MailServer\Include\IMailMananger.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	2/6/2017
** 版  本:	1.0
** 描  述:	邮件管理接口
********************************************************************/

#pragma once
#include "IServiceContainer.h"

struct IMailManager
{
	// 获得邮件管理器
	virtual SERVICE_PTR GetMailService() = 0;

	// 加载 
	virtual bool	Load() = 0;
	// 释放 
	virtual void	Release() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(MAILSERVICE_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(MailService))
extern "C" IMailManager * CreateMailManager();
#	define	CreateMailManagerProc	CreateMailManager
#else													/// 动态库版本
typedef IMailManager * (RKT_CDECL * procCreateMailManager)();
#	define	CreateMailManagerProc	DllApi<procCreateMailManager>::load(MAKE_DLL_NAME(MailService), "CreateMailManager")
#endif