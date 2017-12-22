/*******************************************************************
** 文件名:	SNSService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:  6/12/2016
** 版  本:	1.0
** 描  述:	
			SNS管理
********************************************************************/

#pragma once

#include "ISNSService.h"
#include "IMessageHandler.h"
#include "IShareServer.h"
#include "TimerAxis.h"
#include "buffer.h"
#include "IDBRetSink.h"

struct SSNSActorInfo;

class SNSService 
	:public ISNSService
	,public ISharePersonHandler 
	,public IMessageHandler
	,public IEventExecuteSink
	,public IDBRetSink
{
public:
	SNSService();
	virtual ~SNSService();

	// _Stub::release() call
	void release();
	// _Stub::on_start call
	bool create();

public:
    ////////////////////////////////ISNSService//////////////////////////////////////////
	// 从SNS服务器接收数据
	virtual void recv_message(int msgID, const char* data, int nLen);
	// 两个人添加为好友
	virtual void addBuddy(DWORD dwUserID1, DWORD dwUserID2, const char* szActorName1, const char* szActorName2);
	// 邀请一起游戏
	// @param dwTargetActorID : 被邀请者ActorID
	// @param dwRoomID : 房间ID
	// @param szInviterName : 邀请者名字
	virtual void invitePlayGame(DWORD dwInviterActorID, DWORD dwTargetActorID, DWORD dwRoomID);

	////////////////////////////////ISharePersonHandler//////////////////////////////////////////
	// 上线或者跨进程切换地图后，也是在这里得到通知
	virtual void OnLogin(ISharePerson *pSharePerson, ELoginMode nLineType);
	// 下线或者跨进程切换地图后，也是在这里得到通知，当调完以后，就无法再取到
	virtual void OnLogout(ISharePerson *pSharePerson, ELogoutMode nLineType);
	// 更新数据前，此处分一前一后，是让应用层知道什么数据改变了，而nUpdateFlag表示是因为什么原因改变了
	virtual void OnPre_Update(ISharePerson *pSharePerson, SHAREUPDATE_REASON nUpdateReason);
	// 更新数据后，此处分一前一后，是让应用层知道什么数据改变了，而nUpdateFlag表示是因为什么原因改变了
	virtual void OnPost_Update(ISharePerson *pSharePerson, SHAREUPDATE_REASON nUpdateReason);


	////////////////////////////////IMessageHandler//////////////////////////////////////////
	// 客户端消息
	virtual void	onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead* head, void* data, size_t len);

	////////////////////////////////IEventExecuteSink//////////////////////////////////////////
	// 事件执行回调
	virtual void OnExecute( WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen );

	////////////////////////////////IDBRetSink//////////////////////////////////////////
	virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

private:
	void onReqLogin(SSharePersonProperty& shareProperty);
	void onReqUpdateRemark(SSharePersonProperty& shareProperty, SNetMsgHead* head, void* data, size_t len);
	void onReqUpdateBuddy(SSharePersonProperty& shareProperty, SNetMsgHead* head, void* data, size_t len);
	void onReqRspAddBuddy(SSharePersonProperty& shareProperty, SNetMsgHead* head, void* data, size_t len);
	void onReqAddBuddyByName(SSharePersonProperty& shareProperty, SNetMsgHead* head, void* data, size_t len);
	void onReqInvitePlayGame(SSharePersonProperty& shareProperty, SNetMsgHead* head, void* data, size_t len);
	void onReqInviteMatchTeam(SSharePersonProperty& shareProperty, SNetMsgHead* head, void* data, size_t len);

private:
	// 填充聊天消息头
	void fillSNSHead(rkt::obuf &ob, int destEnd, int msg) const;
	// 获取玩家角色信息
	void fillActorInfo(SSNSActorInfo* pActorInfo, const SSharePersonProperty& shareProperty);
	
	// 转发消息到SNSServer
	void transferToSNSServer(SSharePersonProperty& shareProperty, ulong uMsgID, void* data, size_t len);
	void transferToSNSServer(UDBID dwUserID, ulong uMsgID, void* data, size_t len);

	// 添加好友
	void addBuddy(UDBID dwUserID, PDBID dwPdbid, BYTE bySex, char* szName, UDBID dwOptUserID, PDBID dwOptPdbid);

	// 添加好友
	void addBuddyByName(SSharePersonProperty & shareProperty, const char* szActorName);
};