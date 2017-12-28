/*******************************************************************
** 文件名:	IMentorshipSceneService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	11/29/2016
** 版  本:	1.0
** 描  述:	师徒系统场景服务接口
********************************************************************/

#pragma once

#include "GatewayServiceDef.h"
#include "PackagePtr.h"
#include "IMentorshipScene.h"
#include "NetMessageDef.h"
using Gateway::ClientID;

struct IMentorshipSceneService
{
    // 处理其它服务器发送过来的消息
    virtual void    handleServerMsg(DWORD serverID, SNetMsgHead head, void *data, size_t len) = 0;

    // 处理客户端发送过来的消息
    virtual void    handleClientMsg(DWORD client, SNetMsgHead head, void *data, size_t len) = 0;

	// 发送战斗结果
	virtual void	sendFightResult(DWORD dwPDBID, bool bFightResult, bool bFirstWin, BYTE nMentorshipPost) = 0;

	// 获取师徒职位
	virtual BYTE	getMyMentorshipPost(DWORD dwSrcPDBID, DWORD dwDesPDBID) = 0;

	// 获取师徒奖励
	virtual void	getMentorshipAward(DWORD dwPDBID, int* nExpPercent, int* nHeroExpPercent, int* nGoldPercent) = 0;

	// 获取师傅个数
	virtual int getPlayerMasterCount(DWORD dwDBID) = 0;

	// 获取学徒个数
	virtual int getPlayerPrenticeCount(DWORD dwDBID) = 0;

	// 检测目标是否为玩家徒弟
	virtual int checkIsPlayerPrentice(DWORD dwDBID, DWORD dwTargetDBID) = 0;

};
