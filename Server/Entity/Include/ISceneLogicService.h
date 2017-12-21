/*******************************************************************
** 文件名:	ISceneLogicService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-3-10
** 版  本:	1.0
** 描  述:	

		单个场景逻辑服务，管理所有非玩家对象，包括怪物、NPC和载具等(一个协程)
********************************************************************/

#pragma once

#include "INPCService.h"
#include "ITankService.h"
#include "ILuaService.h"
#include "IEventEngine.h"

struct SEntityReliveInfo;

struct IPathFindService;

/**
	职责: 
	1.管理某个场景的所有怪物，NPC的创建销毁
	2.管理某个场景的所有怪物AI
	3.管理某个场景的所有NPC对话
	4.管理某个场景的所有载具创建销毁
	5.某个场景中LUA的逻辑处理
*/
struct ISceneLogicService
{
	// 取得NPCService对象
	virtual INPCService * getNPCService(void) = 0;

	// 取得TankService对象
	virtual ITankService * getTankService(void) = 0;

	// 取得LuaService对象
	virtual ILuaService * getLuaService(void) = 0;

	// 取得场景事件对象
	virtual IEventEngine * getEventService(void) = 0;

	// 取得寻路对象
	virtual IPathFindService * getPathFindService(void) = 0;

    // 取得AIService
    virtual SERVICE_PTR getAIService(void) = 0;

    // 请求销毁角色
    // 因为假如actorservice发出全局下线事件 会是post过来的 
    // Lua中处理下线的时候 角色很可能已经下线。
    // 所以下线之前 post一条消息过来 让场景逻辑处理完一些逻辑 之后才真正下线
    // 注意：要保证角色销毁之前 场景逻辑一直都在!
    virtual void destroyActor(UID uid, int nReason) = 0; 

	// 释放
	virtual void release() = 0;
};