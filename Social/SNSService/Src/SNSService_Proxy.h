/*******************************************************************
** 文件名:	SNSService_Proxy.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	6/12/2016
** 版  本:	1.0
** 描  述:	
            SNS服务代理
********************************************************************/

#pragma once
#include "IServiceContainer.h"
#include "ISNSService.h"

//using namespace rkt;
//using namespace std;

class SNSService_Proxy:public ISNSService
{
public:
	struct addBuddy_Param
	{
		DWORD dwUserID1;
		DWORD dwUserID2;
		char szActorName1[32];
		char szActorName2[32];
	};

public:
	// 接收数据
	virtual void recv_message(int msgID, const char* data, int nLen);

	// 直接加好友
	virtual void addBuddy(DWORD dwUserID1, DWORD dwUserID2,  const char* szActorName1, const char* szActorName2);

	// 邀请一起游戏
	// @param dwTargetActorID : 被邀请者UserID
	// @param dwRoomID : 房间ID
	// @param szInviterName : 邀请者名字
	virtual void invitePlayGame(DWORD dwInviterActorID, DWORD dwTargetActorID, DWORD dwRoomID);
public:
	SERVICE_PTR m_pContainer;

private:
	void PostMsg(obuf256& oMsg) const;
};