/*******************************************************************
** 文件名:	IMailScene.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	2/7/2017
** 版  本:	1.0
** 描  述:	邮件系统场景接口
********************************************************************/

#pragma once
#include "MailDef.h"
#include "IServiceContainer.h"
#include "GatewayServiceDef.h"
using Gateway::ClientID;

// 帮会场景接口
struct IMailScene
{
	// 加载 
	virtual bool			load() = 0;
    // 执行释放Service
    virtual void            onStop() = 0;
	// 释放 
	virtual void			release() = 0;

	// 获取帮会服务
	virtual SERVICE_PTR		getMailSceneService() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(MAILSCENESERVER_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(MailSceneServer))
extern "C" IMailScene * CreateMailScene();
#	define	CreateMailSceneProc	CreateMailScene
#else													/// 动态库版本
typedef IMailScene * (RKT_CDECL * procCreateMailScene)();
#	define	CreateMailSceneProc	DllApi<procCreateMailScene>::load(MAKE_DLL_NAME(MailServer), "CreateMailScene")
#endif


