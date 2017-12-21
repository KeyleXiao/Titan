/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\MentorshipServer\Include\IMentorshipService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	11/24/2016	18:10
** 版  本:	1.0
** 描  述:	
********************************************************************/

#pragma once

#include "GatewayServiceDef.h"
using Gateway::ClientID;

struct IMentorshipService
{

    // 处理其它服务器发送过来的消息
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg) = 0;

    // 处理客户端发送过来的消息
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg) = 0;

};