/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\MatchServer\Include\IMatchService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	10/15/2015	17:50
** 版  本:	1.0
** 描  述:	聊天系统服务提供的接口

			使用方法：
            EMChatTipID：在EMChatTipID枚举中定义好新的枚举值后，在SystemMessage.csv中配置相应的参数（显示位置、文字描述等）
			EMChatBroadcastType: 该参数决定了该消息会广播的范围（世界、家族、战队、个人等）
			EMChatChannelType: 该参数决定了广播给聊天框的哪个频道
********************************************************************/

#pragma once

#include "GatewayServiceDef.h"
#include "ChatDef.h"
#include "SharePropertyDef.h"
using Gateway::ClientID;

struct IChatService
{
    // 处理其它服务器发送过来的消息
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg) = 0;

    // 处理客户端发送过来的消息
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg) = 0;

	// 发送系统广播(参数用分号分隔)
	// @param tipID: EMChatTipID(SystemMessage.csv中配置相应)
	// @param broadType: EMChatBroadcas
	// @param broadTargetID: 根据broadType参数来填，具体看EMChatBroadcastType定义tType
	// 描述：SystemMessage.csv脚本里配置tips的位置
	virtual void broadcastSystemMessage(int broadType, int broadTargetID, int tipID, LPCSTR szParam = NULL)  = 0;

	// 伪装角色发送信息(参数用分号分隔, 只发给特定角色时传入目标PDBID)
	// @param tipID见EMChatTipID定义
	virtual void broadcastActorTipMessage(SSharePersonProperty actorInfo, int nChannel, int tipID, LPCSTR szParam = NULL, PDBID nTargetID = 0) = 0;

	// 广播公告消息
	// @param isAdd: true(新增）,false(重置) 
	// @param szParam: 参数字符串
	// @param nCount: 广播次数
	// @param nInternal: 广播间隔
	virtual void broadcastADMessage(bool isAdd, LPCSTR szParam = NULL, int nCount = 1, int nInternal = 0) = 0;

	// 广播消息到聊天框(参数用分号分隔)
	// @param tipID: EMChatTipID(SystemMessage.csv中配置相应)
	// @param broadType: EMChatBroadcastType
	// @param broadTargetID: 根据broadType参数来填，具体看EMChatBroadcastType定义
	// 描述：要在SystemMessage.csv脚本里配置tips的位置为聊天框位置
	virtual void broadcastSystemMessageToChatBox(int broadType, int broadTargetID, int tipID, int chatChannel, LPCSTR szParam = NULL) = 0;
};