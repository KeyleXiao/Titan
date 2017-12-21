/*******************************************************************
** 文件名:	ISNSService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	12/6/2016
** 版  本:	1.0
** 描  述:	SNS服务
********************************************************************/

#pragma once

struct ISNSService
{
	// 从SNSServer接收消息
	virtual void recv_message(int msgID, const char* data, int nLen) = 0;

	// 直接加好友
	virtual void addBuddy(DWORD dwUserID1, DWORD dwUserID2, const char* szActorName1, const char* szActorName2) = 0;

	// 邀请一起游戏
	// @param dwTargetActorID : 被邀请者ActorID
	// @param dwRoomID : 房间ID
	// @param szInviterName : 邀请者名字
	virtual void invitePlayGame(DWORD dwInviterActorID, DWORD dwTargetActorID, DWORD dwRoomID) = 0;
};