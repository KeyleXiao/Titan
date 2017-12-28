/*******************************************************************
** 文件名:	E:\speed\Server\ChatServer\Include\IChatSceneService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 
** 创建人:	马传飞
** 日  期:	10/16/2015	15:11
** 版  本:	1.0
** 描  述:	聊天系统场景服 服务提供的接口
            
********************************************************************/

#pragma once

#include "GatewayServiceDef.h"
#include "PackagePtr.h"
#include "ChatDef.h"
using Gateway::ClientID;

struct IChatSceneService
{
	// 发送系统广播(参数用分号分隔)
	// @param tipID: EMChatTipID(SystemMessage.csv中配置相应)
	// @param broadType: EMChatBroadcast
	// @param broadTargetID: 根据broadType参数来填，具体看EMChatBroadcastType定义tType
	// 描述：SystemMessage.csv脚本里配置tips的位置
	virtual void broadcastSystemMessage(int broadType, int broadTargetID, int tipID, LPCSTR szParam = NULL, int nDelayTime = 0, WORD nReserveData = 0) = 0;

	// 广播消息到聊天框(参数用分号分隔)
	// @param tipID: EMChatTipID(SystemMessage.csv中配置相应)
	// @param broadType: EMChatBroadcastType
	// @param broadTargetID: 根据broadType参数来填，具体看EMChatBroadcastType定义
	// 描述：要在SystemMessage.csv脚本里配置tips的位置为聊天框位置
	virtual void broadcastSystemMessageToChatBox(int broadType, int broadTargetID, int tipID, int chatChannel, LPCSTR szParam = NULL, int nDelayTime = 0, WORD nReserveData = 0) = 0;

	// 怪物说话
	virtual void broadcastMonsterMessage(UID &uid, int msgIndex) const = 0;

	// 伪装角色发送信息(参数用分号分隔, 只发给特定角色时传入目标PDBID)
	virtual void broadcastActorTipMessage(PDBID nActorID, LPCSTR szActorName, int nChannel, int nChannelTargetID, EMChatTipID tipID, LPCSTR szParam = NULL, PDBID nTargetID = 0) = 0;

    // 处理其它服务器发送过来的消息
    virtual void    handleServerMsg(DWORD serverID, SNetMsgHead head, const char* pData, size_t nLen) = 0;

    // 处理客户端发送过来的消息
    virtual void    handleClientMsg(DWORD client, SNetMsgHead head, const char* pData, size_t nLen) = 0;

};
