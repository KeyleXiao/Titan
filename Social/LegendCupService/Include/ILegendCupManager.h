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
#include "LegendCupDef.h"

struct ILegendCupManager
{
	// 获得聊天管理器
	virtual SERVICE_PTR getLegendCupService() = 0;

	// 加载 
	virtual bool	load() = 0;
	// 释放 
	virtual void	release() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(LEDENDCUPSERVICE_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(LegendCupService))
extern "C" ILegendCupManager * CreateLegendCupManager();
#	define	CreateLegendCupManagerProc	CreateLegendCupManager
#else													/// 动态库版本
typedef ILegendCupManager * (RKT_CDECL * procCreateLegendCupManager)();
#	define	CreateLegendCupManagerProc	DllApi<procCreateLegendCupManager>::load(MAKE_DLL_NAME(LegendCupService), "CreateLegendCupManager")
#endif