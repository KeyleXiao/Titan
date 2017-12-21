/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceGateway\Src\VoiceUser.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	谢晓鑫
** 日  期:	2013-01-03
** 版  本:	1.0
** 描  述:	语音客户端用户
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "stdafx.h"
#include "AudioUser.h"
#include "AudioList.h"
#include <strstream>
using namespace std;
#include "Winsock2.h"
#include "GlobalVoiceGateway.h"


CAudioUser::CAudioUser( IConnection * conn )
	: m_pConnection(conn)
{

	const char * ip = 0;
	conn->GetRemoteAddr(&ip,m_dwRemotePort);
	assert(ip!=0);
	m_szRemoteIP = ip;

	m_dwConnectTime = (DWORD)time(NULL);
	m_dwLastClientTick = 0;

	m_dwLatency  = 0;
	m_dwLastPingTime  = 0;
	m_dwLastTimeStamp = 0;

	m_bVerifyed = false;		// 是否验证通过的
	m_bPingReplay = TRUE;

	m_dwID = 0;

	m_dwLastKeepaliveTime = ::GetTickCount();
	m_dwLastDropTraceTime = 0;	// 最后抛包提示Tick时间
	m_dwLastDropDataTime = 0;	// 最后抛包Tick时间

	gGlobalServer->SetTimer( ETimerEventID_Check, VOICE_KEEPALIVE_TIMEOUT*100, this, "CAudioUser::CAudioUser" );
}


CAudioUser::~CAudioUser(void)
{
}

/// purpose: 客户端断开
void CAudioUser::OnDisconnected( IConnection * conn,DWORD reason )
{
	WarningLn("用户断开!" << "reason["<<reason<<"]，" << ToString());
	Release();
}

/// purpose: 客户端接收数据
void CAudioUser::OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen )
{
	assert(conn==m_pConnection);

	if ( dwDataLen<sizeof(SGameMsgHead))
	{
		WarningLn("CAudioUser::OnRecv Package length is too short.");
		return;
	}

	Assert(pData != NULL);
	SGameMsgHead* head = (SGameMsgHead*)pData;
	ulong moduleId = head->byKeyModule;
	ulong actionId = head->byKeyAction;
	uchar* data = (uchar*)pData + sizeof(SGameMsgHead);
	size_t new_len =(size_t)(dwDataLen-sizeof(SGameMsgHead));
	if (new_len == 0) data = 0;

	if( moduleId != MSG_MODULEID_VOICE )
	{
		ErrorLn(_GT("CAudioUser::OnRecv 客户端发来一个错误模块消息，moduleId:") << moduleId);
		return ;
	}

	// 是否验证通过的
	if (!m_bVerifyed)
	{
		// 如果没有验证通过，只收握手消息
		if( actionId != MSG_VOICE_HANDSHAKE )
		{
			ErrorLn(_GT("CAudioUser::OnRecv 未验证客户端发来一个错误消息，actionId:") << actionId);
			return ;
		}
	}

	if( MSG_ENDPOINT_VOICEGATE == head->DestEndPoint )
	{
		gGlobalServer->onAudioClientMessage((*this), head->byKeyAction, head, data, new_len);
	}
	else
	{
		// 转发到语音服务器
		gGlobalServer->onAudioTransVoiceServer((*this), head->byKeyAction, head, pData, dwDataLen);
	}

	((GlobalVoiceGateway*)gGlobalServer)->m_dwRecvSpeedClient += dwDataLen;		// 统计收包流量
}

/// purpose: 客户端连接错误
void CAudioUser::OnError( IConnection * conn,DWORD dwErrorCode )
{
	TraceLn(_GT("网络Error[") << dwErrorCode << _GT("]，关闭语音通道连接，") << ToString());
	Release();
}

// 定时器
void CAudioUser::OnTimer( unsigned long dwTimerID )
{
	switch(dwTimerID)
	{
	case ETimerEventID_Check:
		if( ::GetTickCount() >= m_dwLastKeepaliveTime + VOICE_KEEPALIVE_TIMEOUT*100 )
		{
			Release();
		}
		break;
	}
}


/// purpose: 客户端信息
std::string CAudioUser::ToString()
{
	ostrstream buffer;
	buffer << "ID[" << m_dwID 
		<< "]，IP[" << m_szRemoteIP
		<< ":" << m_dwRemotePort << ends;

	return buffer.str();
}

/// purpose: 关闭连接
void CAudioUser::Close()
{
	// 关闭之前要把所属的客户端对象里的值清0
	CClientUser * pUser = CClientList::getInstance().FindUser(m_dwClientID);
	if( NULL != pUser )
	{
		pUser->SetAudioUserID(VOICE_MAX_CLIENT_COUNT);
	}

	if ( m_pConnection )
	{
		m_pConnection->Release();
		m_pConnection = 0;
	}
}

/// purpose: 释放
void CAudioUser::Release()
{
	gGlobalServer->KillTimer( ETimerEventID_Check, this );

	CAudioList::getInstance().DelUser(this);

	Close();
	
	delete this;
}

/// purpose: 发送数据
bool CAudioUser::SendData(LPVOID pData,DWORD dwDataLen,DWORD dwFlag)
{
	//Assert(m_pConnection);
	if (NULL==m_pConnection)
	{	
		return false;
	}

	// 语音网关语音视频流专用网络通道最大缓存包数,多于这个将丢弃新来的发包请求(默认40,大概20秒语音数据),用这种方式来控制语音视频流拥堵情况
	DWORD dwPendingCount = m_pConnection->GetPendingCount(true);
	DWORD dwNowTicks = ::GetTickCount();

	// 已触发丢包
	if (m_dwLastDropDataTime>0)
	{
		// 已快发完了,可以正常发,否则等发完
		if (dwPendingCount<8)
		{
			m_dwLastDropDataTime = 0;
		}
		else
		{
			return false;
		}
	}
	else if (dwPendingCount>VOICE_GATEWAY_AUDIONET_MAX)
	{
		m_dwLastDropDataTime = dwNowTicks;	// 开启触发丢包
		if (dwNowTicks>m_dwLastDropTraceTime+30000)
		{
			m_dwLastDropTraceTime = dwNowTicks;
			WarningLn("用户语音视频流拥堵:"<<dwPendingCount<<",用户:"<<ToString());
		}
		return false;
	}

	((GlobalVoiceGateway*)gGlobalServer)->m_dwSendSpeedClient += dwDataLen;		// 统计发包流量

	return m_pConnection->SendData((const char *)pData,dwDataLen,dwFlag);
}

/// purpsoe: 断开连接
void CAudioUser::Disconnect()
{
	if ( m_pConnection )
	{
		m_pConnection->Disconnect();
	}
}

/// purpose: 获取端口
WORD CAudioUser::GetPort()
{
	return (WORD)m_dwRemotePort;
}

/// purpose: 获得IP
const char *CAudioUser::GetClientIP()
{
	return m_szRemoteIP.c_str();
}

/// purpose: 获得整形IP
DWORD CAudioUser::GetIP()
{
	if( NULL == m_pConnection )
	{
		return 0;
	}

	return inet_addr( m_szRemoteIP.c_str() );
}

/// purpose: 设置客户端ID
void CAudioUser::SetID( DWORD dwID )
{
	m_dwID = dwID;
}

/// purpose: 获得客户端ID
DWORD CAudioUser::GetID()
{
	return m_dwID;
}

/// purpose: 获得MAC地址
const char *CAudioUser::GetMacAddress()
{
	return m_szMacAddres.c_str();
}

DWORD CAudioUser::GetConnectTime(void)
{
	return m_dwConnectTime;
}

// 设置所属语音客户端ID
void CAudioUser::SetClientID( DWORD dwClientID )
{
	m_dwClientID = dwClientID;
}

// 获取所属语音客户端ID
DWORD CAudioUser::GetClientID(void)
{
	return m_dwClientID;
}
