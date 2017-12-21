/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceServer\Src\Chat.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	谢晓鑫
** 日  期:	2012-03-21
** 版  本:	1.0
** 描  述:	语音私聊
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#ifndef __CHAT_H__
#define __CHAT_H__

#include "VoiceDef.h"

class CClientUser;


// 私聊状态枚举
enum EMVoiceChatState
{
	VoiceChatState_None,
	VoiceChatState_WaitAccepter,	// 等待被邀请者接收
	VoiceChatState_Accepted,		// 私聊已接受
	VoiceChatState_Run,				// 私聊运行中
	
	VoiceChatState_End,				// 私聊结束

	VoiceChatState_Max,
};


// 语音房间类
class CChat : public TimerHandler
{
	enum 
	{
		ETimerEventID_State = 0,		// 状态定时器
	};

public:
	
	// 构造函数
	CChat( DWORD dwID );

	// 析构函数
	virtual ~CChat(void);

	///////////////////// TimerHandler /////////////////////////////////////////////////////
	// 定时器，计算麦序时间等
	virtual void OnTimer( unsigned long dwTimerID );

	// 创建
	bool Create(void);

	// 释放
	void Release(void);

	// 获取ID
	DWORD GetID(void);

	// 添加一个用户
	void AddChatUser( CClientUser & user, DWORD dwChatUserType );

	// 删除一个用户
	void DelChatUser( CClientUser & user );

	// 获取私聊参与者，不能用此方法获取私聊旁听者
	DWORD GetChatUser( DWORD dwChatUserType );

	// 邀请
	bool Invite( CClientUser & inviter, CClientUser & acceptor );

	// 接受
	bool Accept( CClientUser & inviter, CClientUser & acceptor );

	// 私聊用户离开
	void OnUserLeave( CClientUser & user );

	// 私聊结束
	void End(void);

	// 工作内容
	void DoTask(void);

	// 获取私聊里所有人ID列表
	DWORD GetClientIDList( DWORD * pClientIDArray, DWORD dwIDArrayCount, DWORD & dwGetCounts );

	// 获取状态
	DWORD GetState(void);

	// 添加一个旁观者
	void AddVisitor( CClientUser & user );

private:
	// 发送私聊请求
	void SendInvite( CClientUser & inviter, CClientUser & acceptor );

	// 进入一个状态 dwTimeOut 超时时间 秒
	void GoState( DWORD dwState, DWORD dwTimeOut = 0 );

	// 更新用户信息到客户端 dwUserType 为 0 表示正在私聊的用户，1 表示发来邀请的用户
	void UpdateChatUserInfoToClient( DWORD dwClientID, CClientUser & user, DWORD dwUserType );

private:
	// 私聊对象ID
	DWORD m_dwID;

	// 邀请者ClientID
	DWORD m_dwInviter;	

	// 接受者ClientID
	DWORD m_dwAcceptor;

	// 参与语音私聊的用户客户端ID
	std::list<DWORD> m_clientList;

	// 私聊状态
	DWORD m_dwState;
	
	// 状态开始时间
	DWORD m_dwStateStartTime;

	// 状态超时时间 秒
	DWORD m_dwStateTimOut;
};



#endif