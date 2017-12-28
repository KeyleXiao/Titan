/*******************************************************************
** 文件名:	IClanSceneService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/6/2016
** 版  本:	1.0
** 描  述:	帮会场景服务接口
********************************************************************/

#pragma once

#include "GatewayServiceDef.h"
#include "PackagePtr.h"
#include "IClanScene.h"
#include "NetMessageDef.h"
using Gateway::ClientID;

struct IClanSceneService
{
	// 获取帮会名称
	virtual string getClanName(DWORD nClanID) = 0;

    // 处理其它服务器发送过来的消息
    virtual void    handleServerMsg(DWORD serverID, SNetMsgHead head, void *data, size_t len) = 0;

    //// 处理客户端发送过来的消息
    //virtual void    handleClientMsg(DWORD client, SNetMsgHead head, void *data, size_t len) = 0;

    virtual int getNumProp(DWORD dwClaneID, DWORD dwProp) = 0;

	virtual bool setNumProp(DWORD dwClanID, DWORD dwProp, int nValue) = 0;

	// 检查创建联盟杯赛
	virtual bool checkCreateClanLegendCup(DWORD dwClanID, int LegendCupConfigID) = 0;
};
