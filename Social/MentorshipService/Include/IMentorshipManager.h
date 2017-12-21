/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\MentorshipServer\Include\IMentorshipMananger.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	11/24/2016	15:45
** 版  本:	1.0
** 描  述:	师徒管理接口
********************************************************************/

#pragma once
#include "IServiceContainer.h"

struct IMentorshipManager
{
	// 获得师徒管理器
	virtual SERVICE_PTR GetMentorshipService() = 0;

	// 加载 
	virtual bool	Load() = 0;
	// 释放 
	virtual void	Release() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(MENTORSHIPSERVICE_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(MentorshipService))
extern "C" IMentorshipManager * CreateMentorshipManager();
#	define	CreateMentorshipManagerProc	CreateMentorshipManager
#else													/// 动态库版本
typedef IMentorshipManager * (RKT_CDECL * procCreateMentorshipManager)();
#	define	CreateMentorshipManagerProc	DllApi<procCreateMentorshipManager>::load(MAKE_DLL_NAME(MentorshipService), "CreateMentorshipManager")
#endif