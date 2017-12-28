/*******************************************************************
** 文件名:	IMigrateService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	彭璐
** 日  期:	7/15/2015
** 版  本:	1.0
** 描  述:	

modify: ljw

			管理玩家登陆、切地图或场景等各种数据迁移服务
********************************************************************/

#pragma once

#include "IServiceContainer.h"
#include "PackagePtr.h"
#include "NetMessageDef.h"
#include "MigrateDef.h"

// 迁移service
struct IMigrateService
{
    // 处理其它服务器发送过来的消息
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, const char* pData, size_t nLen) = 0;

    // 处理客户端发送过来的消息
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, const char* pData, size_t nLen) = 0;

	// 预进入场景
	virtual void preEnterScene(void* pData, size_t len) = 0;

    // 动态场景ID不同进程之间可以重复 所以进动态地图 要指出去哪个场景服的动态地图
    // 去静态地图 不需要指出目标场景服ID
    virtual void enterScene(PDBID idActor, int nDestSceneID, DWORD dwDestWorlID, DWORD cidDestServerID = 0) = 0;

	// 获取本场景服中场景的预进入场景人数
	virtual DWORD getLocalPreEnterPlayerCount(int nSceneID) = 0;

	// 返回一个合适可用的静态场景ID（动态场景返回到静态场景，需要根据所有静态场景的人数来判断适合去哪个静态场景）
	// @param nMapType: 目标地图类型
	// @param nPlayerCount: 需要迁移的人数
	// @param destScene: 目标场景信息
	virtual bool getAvailableScene(int nDestMapType, int nPlayerCount, SSceneNode* destScene) = 0;
};
