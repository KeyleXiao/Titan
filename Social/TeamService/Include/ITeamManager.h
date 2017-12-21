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
#include "TeamDef.h"

struct ITeamManager
{
	// 获得聊天管理器
	virtual SERVICE_PTR GetTeamService() = 0;

	// 加载 
	virtual bool	Load() = 0;
	// 释放 
	virtual void	release() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(TEAMSERVICE_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(TeamService))
extern "C" ITeamManager * CreateTeamManager();
#	define	CreateTeamManagerProc	CreateTeamManager
#else													/// 动态库版本
typedef ITeamManager * (RKT_CDECL * procCreateTeamManager)();
#	define	CreateTeamManagerProc	DllApi<procCreateTeamManager>::load(MAKE_DLL_NAME(TeamService), "CreateTeamManager")
#endif