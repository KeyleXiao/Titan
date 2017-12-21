/*******************************************************************
** 文件名:	IClanManager.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	3/31/2016	16:57
** 版  本:	1.0
** 描  述:	帮会管理
********************************************************************/


#pragma once
#include "IServiceContainer.h"

struct IClanManager
{
	// 获得聊天管理器
	virtual SERVICE_PTR getClanService() = 0;

    // 加载 
	virtual bool	load() = 0;

	// 释放 
	virtual void	release() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(CLANSERVICE_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(ClanService))
extern "C" IClanManager * CreateClanManager();
#	define	CreateClanManagerProc	CreateClanManager
#else													/// 动态库版本
typedef IClanManager * (RKT_CDECL * procCreateClanManager)();
#	define	CreateClanManagerProc	DllApi<procCreateClanManager>::load(MAKE_DLL_NAME(ClanService), "CreateClanManager")
#endif