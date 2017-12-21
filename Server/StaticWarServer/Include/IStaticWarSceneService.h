/*******************************************************************
** 文件名:	E:\speed\Server\OrderServer\Include\IStaticWarSceneService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 
** 创建人:	秦其勇
** 日  期:	10/19/2015
** 版  本:	1.0
** 描  述:	所有战场数据服务 服务提供的接口
            
********************************************************************/

#pragma once

#include "NetMessageDef.h"
#include "GatewayServiceDef.h"
#include "PackagePtr.h"

using Gateway::ClientID;

struct  IStaticWarSceneService
{
    // 处理其它服务器发送过来的消息
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg) = 0;

    // 处理客户端发送过来的消息
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg) = 0;

	// 请求其他玩家基本数据
	virtual void reqOtherPlayerInfo(PDBID dwSelfID, PDBID dwReqID, bool bFake = false) = 0;
	
	// 请求其他玩家英雄数据
	virtual void reqOtherPlayerHeroInfo(PDBID dwSelfID, PDBID dwReqID, bool bFake = false) = 0;

};
