/*******************************************************************
** 文件名:	E:\speed\Server\MatchSceneServer\Include\IMatchSceneService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	3/19/2015	15:45
** 版  本:	1.0
** 描  述:	比赛大厅场景服 服务提供的接口
            
********************************************************************/

#pragma once

#include "MatchDef.h"
#include "GatewayServiceDef.h"
#include "IMatchScene.h"
#include "MigrateDef.h"


struct  IMatchSceneService
{
    // 处理其它服务器发送过来的消息
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg) = 0;

    // 处理客户端发送过来的消息
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg) = 0;

	// 大厅通知 开始战场 这边要创建场景
	virtual void start() = 0;
	// 通知大厅 房间ID的战场结束 
	virtual void warEnd(DWORD dwWarID, LONGLONG llDBWarID) = 0;

	// 发送创建结果
	virtual void sendCreateMobaResult(DWORD dwRoomID, int nWarID, int nSceneID, bool bResult) = 0;

    // 设置战场流程结束 但是未释放
    virtual void setWarProcessEnd(int nWarID) = 0;

    virtual void setRuneTalentPageIndex(PDBID idActor,int nRunePageIndex,int nTalentPageIndex) = 0;

    virtual int  getTalentPageSelectIndex(PDBID idActor) = 0;

    virtual int  getRunePageSelectIndex(PDBID idActor) = 0;

    virtual void onUpdateRoomPlayerClient(PDBID idActor, ClientID client) = 0;

    // 设置玩家场景加载进度
    virtual void setClientSceneLoadingProgress(PDBID idActor, int nProgress, long lPing) = 0;
    
    // 角色是否在战场
    virtual bool isActorInBattleField(PDBID idActor) = 0;

    // 获取角色的房间ID
    virtual int getRoomID(PDBID idActor) = 0;

	// 设置战场结果
	virtual void setWarResult(SMsgSetWarResultData resultData) = 0;

    // 释放无效的玩家（包括电脑）
    virtual void warEndDestoryInvalidActor(int nWarID) = 0;

    // 主动退出战场
    virtual void accordLeaveWar(PDBID idActor) = 0;
    
};
