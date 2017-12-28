/*******************************************************************
** 文件名:	E:\speed\Server\LegendCupServer\Include\ILegendCupSceneService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 
** 创建人:	秦其勇
** 日  期:	10/19/2015
** 版  本:	1.0
** 描  述:	聊天系统场景服 服务提供的接口
            
********************************************************************/

#pragma once

#include "NetMessageDef.h"
#include "GatewayServiceDef.h"
#include "PackagePtr.h"

using Gateway::ClientID;

struct  ILegendCupSceneService
{
    // 处理其它服务器发送过来的消息
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, void* pData, size_t nLen) = 0;

    // 处理客户端发送过来的消息
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, void* pData, size_t nLen) = 0;

};
