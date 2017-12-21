/*******************************************************************
** 文件名:	GameServiceDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-4-8
** 版  本:	1.0
** 描  述:	游戏服务全局定义

			
********************************************************************/

#pragma once

#ifndef __COMMON_GAMESERVICEDEF_H__
#define __COMMON_GAMESERVICEDEF_H__


// 游戏逻辑服务下所含类型
enum GAMELOGIC_SERVICE_TYPE
{
    GAMELOGIC_SERVICE_CENTER=0,		// 中心服务
    GAMELOGIC_SERVICE_MONSTER,		// 怪物
    GAMELOGIC_SERVICE_TANK,			// 载具
    GAMELOGIC_SERVICE_LUA,			// LUA
    GAMELOGIC_SERVICE_WAR,			// 战场
    GAMELOGIC_SERVICE_EVENT,		// Event
    GAMELOGIC_SERVICE_FINDPATH,		// 寻路
    GAMELOGIC_SERVICE_NONE,         // 场景逻辑
};


#endif  // __COMMON_GAMESERVICEDEF_H__
