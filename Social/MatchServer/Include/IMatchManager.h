/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\MatchServer\Include\IMatch.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	3/18/2015	12:38
** 版  本:	1.0
** 描  述:	
			比赛大厅管理

			获取比赛大厅 现在只有一个大厅
			gSocialGlobal->getMatchManage()->GetMatchService( 1 );     
********************************************************************/

#pragma once

#include "IServiceContainer.h"
#include "MatchDef.h"

struct SMapSchemeInfo;
struct SSchemeMatchRoom;
struct SchemeMatchRoomRule;
struct SSchemeSelectControl;


struct IMatchManager
{
	// 获取比赛大厅服务
	virtual SERVICE_PTR		getMatchService() = 0;

    // 获取地图配置
    virtual SMapSchemeInfo*	    getSchemeMap(DWORD MapID) = 0;

    virtual SSchemeMatchRoom*	getSchemeMatchRoomByMatchType(DWORD dwMatchTypeID) = 0;

    virtual SchemeMatchRoomRule* getSchemeMatchRoomRule( int nRuleID) = 0;

    virtual SSchemeSelectControl* getSchemeMatchRoomSelectControl( int nID) = 0;

	//virtual void onClientMsg(ClientID clientID, void * data, size_t len) = 0;
	
	// 辅助函数 方便调用 
	//////////////////////////////////////////////////////////////////////////
	

	// 下面是系统调用的，已经写死位置，使用者不要调用
	//////////////////////////////////////////////////////////////////////////
	// 加载 
	virtual bool load() = 0;
	// 释放 
	virtual void release() = 0;


};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(MATCHSERVICE_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(MatchService))
extern "C" IMatchManager * CreateMatchManager();
#	define	CreateMatchManagerProc	CreateMatchManager
#else													/// 动态库版本
typedef IMatchManager * (RKT_CDECL * procCreateMatchManager)();
#	define	CreateMatchManagerProc	DllApi<procCreateMatchManager>::load(MAKE_DLL_NAME(MatchService), "CreateMatchManager")
#endif

