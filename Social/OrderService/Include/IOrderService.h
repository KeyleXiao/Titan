/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\MatchServer\Include\IMatchService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	10/15/2015	17:50
** 版  本:	1.0
** 描  述:	
            聊天系统服务提供的接口
********************************************************************/

#pragma once

#include "GatewayServiceDef.h"
using Gateway::ClientID;

struct IOrderService
{
    // 处理其它服务器发送过来的消息
    virtual void     handleServerMsg(DWORD serverID, SNetMsgHead head, void* pData, size_t nLen) = 0;

    // 处理客户端发送过来的消息
    virtual void     handleClientMsg(DWORD client, SNetMsgHead head, void* pData, size_t nLen) = 0;
    // 获取战队排名
    // 返回人员数量
    virtual DWORD getKinOrder(DWORD dwKinID)= 0;

    // 获得帮会排名
    // 返回人员数量
    virtual DWORD getClanOrder(DWORD dwClanID)= 0;

    // 数据返回
    // 返回读取排行榜配置
    virtual void onDBReturnOrderInfoSetting(int nQueueIndex, PACKAGE_PTR msg) = 0;
};