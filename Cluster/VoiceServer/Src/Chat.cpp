/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceServer\Src\Chat.cpp
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
#include "stdafx.h"
#include "Chat.h"
#include "ChatList.h"
#include "ClientUser.h"
#include "GlobalVoiceServer.h"


#define		VOICE_CHAT_INVITE_WAIT_TIME		180		// 私聊邀请等待时间为3分钟

// 构造函数
CChat::CChat( DWORD dwID )
	: m_dwID(dwID)
	, m_dwState(VoiceChatState_None)
	, m_dwInviter(0)
	, m_dwAcceptor(0)
	, m_dwStateStartTime(0)
	, m_dwStateTimOut(0)
{
#ifdef VOICE_PRINT_DEBUG_INF
	ErrorLn("CChat::CChat m_dwID=" << m_dwID);
#endif
}

// 析构函数
CChat::~CChat(void)
{

}


///////////////////// TimerHandler /////////////////////////////////////////////////////
// 定时器，计算麦序时间等
void CChat::OnTimer( unsigned long dwTimerID )
{
	switch(dwTimerID)
	{
	case ETimerEventID_State:
		{
			DoTask();
		}
		break;
	}
}


// 创建
bool CChat::Create(void)
{
	m_dwState = VoiceChatState_None;
	gGlobalServer->SetTimer( ETimerEventID_State, 1000, this, "CChat::Create" );
	return true;
}

// 释放
void CChat::Release(void)
{
#ifdef VOICE_PRINT_DEBUG_INF
	ErrorLn("CChat::Release");
#endif

	// 关闭定时器
	gGlobalServer->KillTimer( ETimerEventID_State, this );

	CChatList::getInstance().Del(m_dwID);

	delete this;
}

// 获取ID
DWORD CChat::GetID(void)
{
	return m_dwID;
}

// 添加一个用户
void CChat::AddChatUser( CClientUser & user, DWORD dwChatUserType )
{
	DWORD dwClientID = user.GetClientID();

	// 要先把以前的频道退出，此种情况一般都是针对GM
	CChat * pChat = user.GetChatInstance();
	if( pChat != NULL )
	{
		pChat->OnUserLeave(user);
	}

	switch(dwChatUserType)
	{
	case VoiceChatUserType_Inviter:	//  邀请者
		{
			m_dwInviter = dwClientID;
			m_clientList.push_back(dwClientID);
			user.SetChatIDAndType( m_dwID, dwChatUserType );
			user.SetFlag( VoiceUserStateFlag_Chat, true );
		}
		break;

	case VoiceChatUserType_Acceptor:	// 接受者
		{
			m_dwAcceptor = dwClientID;		
			m_clientList.push_back(dwClientID);
			user.SetChatIDAndType( m_dwID, dwChatUserType );
			user.SetFlag( VoiceUserStateFlag_Chat, true );
		}
		break;

	case VoiceChatUserType_Visitor:		// 访客，主要针对GM
		{
			// 如果已经存在就不添加了
			for( list<DWORD>::iterator it = m_clientList.begin(); it != m_clientList.end(); ++it  )
			{
				if( dwClientID == *it )
				{
					break;
				}
			}
			
			m_clientList.push_back(dwClientID);
			user.SetChatIDAndType( m_dwID, dwChatUserType );
		}
		break;

	default:
		break;
	}
}

// 删除一个用户
void CChat::DelChatUser( CClientUser & user )
{
	DWORD dwClientID = user.GetClientID();

	// 从列表中清除
	for( list<DWORD>::iterator it = m_clientList.begin(); it != m_clientList.end(); ++it  )
	{
		if( dwClientID == *it )		
		{
			if( m_dwInviter == dwClientID )
			{
				user.SetFlag( VoiceUserStateFlag_Chat, false );
				m_dwInviter = 0;
			}
			else if( m_dwAcceptor == dwClientID )
			{
				user.SetFlag( VoiceUserStateFlag_Chat, false );
				m_dwAcceptor = 0;
			}

			// 找到了才能更新用户身上的数据
			user.SetChatIDAndType( 0, VoiceChatUserType_None );
			m_clientList.erase(it);
			break;
		}
	}
}

// 获取私聊参与者，不能用此方法获取私聊旁听者
DWORD CChat::GetChatUser( DWORD dwChatUserType )
{
	DWORD dwClientID = 0;

	switch(dwChatUserType)
	{
	case VoiceChatUserType_Inviter:	//  邀请者
		dwClientID = m_dwInviter;
		break;

	case VoiceChatUserType_Acceptor:	// 接受者
		dwClientID = m_dwAcceptor;
		break;

	default:
		dwClientID = 0;
		break;
	}

	return dwClientID;
}

// 邀请操作
bool CChat::Invite( CClientUser & inviter, CClientUser & acceptor )
{
	AddChatUser( inviter, VoiceChatUserType_Inviter );

	// 更新被邀请者的信息给邀请者
	UpdateChatUserInfoToClient( m_dwInviter, acceptor, 0 );	

	// 发送请求
	SendInvite( inviter, acceptor );

	// 进入等待状态
	GoState( VoiceChatState_WaitAccepter, VOICE_CHAT_INVITE_WAIT_TIME );

	// 通知邀请者，已经发送邀请成功了
	gGlobalServer->SendVoiceDataToClient( inviter.GetClientID(), MSG_VOICE_DATA_SUBMSG, VoiceSubMsg_ChatInviteSuccess );

	return true;
}

// 接受
bool CChat::Accept( CClientUser & inviter, CClientUser & acceptor )
{
	UpdateChatUserInfoToClient( m_dwAcceptor, inviter, 0 );	
	
	GoState(VoiceChatState_Accepted);

	return true;
}

// 私聊用户离开
void CChat::OnUserLeave( CClientUser & user )
{
	DWORD dwClientID = user.GetClientID();

	if( m_dwState == VoiceChatState_WaitAccepter )		// 等待状态
	{
		if( dwClientID == m_dwInviter )	// 如果是邀请者离开，说明是取消的私聊邀请
		{
			CClientUser * pOther = CClientList::getInstance().GetUser(m_dwAcceptor);
			if( NULL != pOther )
			{
				char szMsg[128] = {0};
				sprintf_s( szMsg, sizeof(szMsg), "[%s]已取消私聊邀请。", user.GetUserInfo().szName );
				pOther->SendChatMsg(szMsg);
			}

			GoState(VoiceChatState_End);
			return;
		}
		else if( dwClientID == m_dwAcceptor )	// 如果是被邀请者离开，说明直接拒绝了
		{
			CClientUser * pOther = CClientList::getInstance().GetUser(m_dwInviter);
			if( NULL != pOther )
			{
				char szMsg[128] = {0};
				sprintf_s( szMsg, sizeof(szMsg), "[%s]拒绝了您的私聊邀请。", user.GetUserInfo().szName );
				pOther->SendChatMsg(szMsg);
			}

			GoState(VoiceChatState_End);
			return;
		}
	}
	else	// 已经通话状态
	{
		// 如果离开的人是邀请或被邀请的人，本次私聊就需要关闭了
		if( dwClientID == m_dwInviter || dwClientID == m_dwAcceptor )
		{
			DWORD dwOther = 0;
			if( user.GetChatUserType() == VoiceChatUserType_Acceptor )
			{
				dwOther = m_dwInviter;
			}
			else if( user.GetChatUserType() == VoiceChatUserType_Inviter )
			{
				dwOther = m_dwAcceptor;
			}

			CClientUser * pOther = CClientList::getInstance().GetUser(dwOther);
			if( NULL != pOther )
			{
				char szMsg[128] = {0};
				sprintf_s( szMsg, sizeof(szMsg), "[%s]退出了私聊。", user.GetUserInfo().szName );
				pOther->SendChatMsg(szMsg);
			}

			GoState(VoiceChatState_End);
			return;
		}
	}

	// 否则，只要删除这个用户即可
	DelChatUser(user);
}

// 私聊结束
void CChat::End(void)
{
#ifdef VOICE_PRINT_DEBUG_INF
	ErrorLn("CChat::End");
#endif

	DWORD dwClientArray[2] = {0};
	DWORD dwIndex = 0;
	CClientList & clientList = CClientList::getInstance();

	// 清空列表
	for( list<DWORD>::iterator it = m_clientList.begin(); it != m_clientList.end(); ++it  )
	{
		CClientUser * pUser = clientList.GetUser(*it);
		if( NULL != pUser )
		{
			pUser->SetChatIDAndType( 0, VoiceChatUserType_None );
			DWORD dwClientID = pUser->GetClientID();

#ifdef VOICE_PRINT_DEBUG_INF
			ErrorLn("CChat::End dwClientID =" << dwClientID );
			ErrorLn( "CChat::End m_dwInviter=" << m_dwInviter << ", m_dwAcceptor=" << m_dwAcceptor );
#endif
			if( dwClientID == m_dwInviter || dwClientID == m_dwAcceptor )
			{	
				if( dwIndex <= 1 )
				{
					dwClientArray[dwIndex] = dwClientID;
					dwIndex ++;
				}

				// 如果开麦了，要关麦
				if( pUser->GetFlag(VoiceUserStateFlag_ChatMic) )
				{	
					pUser->SetFlag( VoiceUserStateFlag_ChatMic, false );
				}

				pUser->SetFlag( VoiceUserStateFlag_Chat, false );
			}
		}
	}

#ifdef VOICE_PRINT_DEBUG_INF
	ErrorLn("VoiceSubMsg_ChatEnd dwClientArray[0]=" << dwClientArray[0]);
	ErrorLn("VoiceSubMsg_ChatEnd dwClientArray[1]=" << dwClientArray[1]);
#endif

	// 通知邀请者，已经发送邀请成功了
	gGlobalServer->BroadcastVoiceDataToClient( dwClientArray, 2, MSG_VOICE_DATA_SUBMSG, VoiceSubMsg_ChatEnd );
	
	m_dwInviter = 0;
	m_dwAcceptor = 0;

	m_clientList.clear();
}


// 工作内容
void CChat::DoTask(void)
{
	DWORD dwTimeNow = (DWORD)time(NULL);
	switch(m_dwState)
	{
	case VoiceChatState_WaitAccepter:	// 等待被邀请者接收
		{
			if( dwTimeNow >= m_dwStateStartTime + m_dwStateTimOut )
			{
				CClientUser *pUser = CClientList::getInstance().GetUser(m_dwInviter);
				if( NULL != pUser )
				{
					char szMsg[128] = {0};
					sprintf_s( szMsg, sizeof(szMsg), "对方在%s内未接受您的私聊邀请，系统已自动挂断！", GetRemainTimeString(VOICE_CHAT_INVITE_WAIT_TIME) );
					pUser->SendChatMsg(szMsg);
				}
				GoState(VoiceChatState_End);
				break;
			}
		}
		break;

	case VoiceChatState_Accepted:		// 私聊已接受
		{
			DWORD dwClientArray[2] = {0};
			dwClientArray[0] = m_dwInviter;
			dwClientArray[1] = m_dwAcceptor;
			gGlobalServer->BroadcastVoiceDataToClient( dwClientArray, 2, MSG_VOICE_DATA_SUBMSG, VoiceSubMsg_ChatRunSuccess, m_dwInviter, m_dwAcceptor );
			
			// 自动开麦
			CClientList & clientList = CClientList::getInstance();
			CClientUser * pInviter = clientList.GetUser(m_dwInviter);
			if( NULL != pInviter )
			{
				pInviter->SetFlag( VoiceUserStateFlag_ChatMic, true );
			}
			
			CClientUser * pAcceptor = clientList.GetUser(m_dwAcceptor);
			if( NULL != pAcceptor )
			{
				pAcceptor->SetFlag( VoiceUserStateFlag_ChatMic, true );
			}

			GoState(VoiceChatState_Run);
		}
		break;

	case VoiceChatState_End:			// 私聊结束
		{
			End();
			Release();
		}
		break;

	default:
		break;
	}
}

// 获取私聊里所有人ID列表
DWORD CChat::GetClientIDList( DWORD * pClientIDArray, DWORD dwIDArrayCount, DWORD & dwGetCounts )
{
	if( pClientIDArray == NULL || dwIDArrayCount <= 0 )
	{
		dwGetCounts = 0;
		return 0;
	}

	DWORD dwFindCounts = 0;
	DWORD * pClientID = pClientIDArray;
	for(std::list<DWORD>::iterator it = m_clientList.begin(); it != m_clientList.end(); ++it)
	{
		if (dwFindCounts >= dwIDArrayCount )
		{
			break;
		}

		(*pClientID) = *it;

		pClientID ++;
		dwFindCounts++;
	}

	dwGetCounts = dwFindCounts;

	return dwFindCounts;
}

// 获取状态
DWORD CChat::GetState(void)
{
	return m_dwState;
}

// 添加一个旁观者
void CChat::AddVisitor( CClientUser & user )
{
	AddChatUser( user, VoiceChatUserType_Visitor );

	char szMsg[128] = {0};
	sprintf_s( szMsg, sizeof(szMsg), "您正在旁听私聊[%d]", m_dwID );
	user.SendChatMsg(szMsg);
}



// 发送私聊请求
void CChat::SendInvite( CClientUser & inviter, CClientUser & acceptor )
{
	AddChatUser( acceptor, VoiceChatUserType_Acceptor );

	DWORD dwClientID = acceptor.GetClientID();

	// 更新邀请者的信息给被邀请者
	UpdateChatUserInfoToClient( dwClientID, inviter, 1 );		

	gGlobalServer->SendVoiceDataToClient( dwClientID, MSG_VOICE_DATA_SUBMSG, VoiceSubMsg_ChatInvite, inviter.GetClientID() );
}

// 进入一个状态 dwTimeOut 超时时间 秒
void CChat::GoState( DWORD dwState, DWORD dwTimeOut )
{
	m_dwStateStartTime = (DWORD)time(NULL);
	m_dwStateTimOut = dwTimeOut;
	m_dwState = dwState;
}


// 更新用户信息到客户端 dwUserType 为 0 表示正在私聊的用户，1 表示发来邀请的用户
void CChat::UpdateChatUserInfoToClient( DWORD dwClientID, CClientUser & user, DWORD dwUserType )
{
	obuf512 obufData;

	SGameMsgHead header;
	header.SrcEndPoint = MSG_ENDPOINT_VOICE;
	header.DestEndPoint= MSG_ENDPOINT_CLIENT;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_CHATUSERINFO_UPDATE;			// 游戏玩家信息更新
	obufData.push_back( &header, sizeof(SGameMsgHead) );
}
