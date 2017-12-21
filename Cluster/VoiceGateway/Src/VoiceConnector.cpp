/*******************************************************************
** 文件名:	VoiceConnector.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2009/8/25
** 版  本:	1.0
** 描  述:	语音服务器连接器
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "stdafx.h"
#include <strstream>
#include "ostrbuf.h"
#include "time.h"
#include "VoiceConnector.h"
#include "VoiceGatewaySetting.h"
#include "GlobalVoiceGateway.h"


/// purpose: 构造函数
VoiceConnector::VoiceConnector():
	 m_pConnection(0)
	, m_wRemotePort(0)
	, m_dwID(0)
	, m_bIsAnswer(true)
	, m_dwLastAnswerTime(0)
	, m_bIsConnected(false)
	, m_dwGetewayID(0)
{
	m_pTimerAxis = NULL;
}

/// purpose: 析构函数
VoiceConnector::~VoiceConnector()
{

}

/// purpose: 连入，不处理
void VoiceConnector::OnAccept( IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler )
{
	assert(false);
}

/// purpose: 连接成功
void VoiceConnector::OnConnected( IConnection * conn )
{
	m_bIsConnected = true;

	SendHandshake();

	m_pTimerAxis->SetTimer(ETimerEventID_Keepalive,VOICE_KEEPALIVE_INTERVAL,this);
}

/// purpose: 断开连接
void VoiceConnector::OnDisconnected( IConnection * conn,DWORD reason )
{
	if ( m_pConnection==conn )
	{
		Close();
		m_pTimerAxis->SetTimer(ETimerEventID_ReconnectNet,VOICE_RECONNECT_INTERVAL,this,1);
	}

	WarningLn("与语音服务器的连接断开，reason：" << reason );
}

/// purpose: 接收语音服务器的网络数据
void VoiceConnector::OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen )
{
	assert(conn==m_pConnection);

	if ( dwDataLen<sizeof(SGameMsgHead))
	{
		WarningLn("Package length is too short.");
		return;
	}

	Assert(pData != NULL);
	SGameMsgHead* head = (SGameMsgHead*)pData;
	ulong moduleId = head->byKeyModule;
	uchar* data = (uchar*)pData + sizeof(SGameMsgHead);
	size_t new_len =(size_t)(dwDataLen-sizeof(SGameMsgHead));
	if (new_len == 0) data = 0;

	((GlobalVoiceGateway*)gGlobalServer)->m_dwRecvSpeedServer += dwDataLen;		// 统计收包流量

	gGlobalServer->onServerMessage( head->byKeyAction, head, data, new_len);
}

/// purpose: 连接出错
void VoiceConnector::OnError( IConnection * conn,DWORD dwErrorCode )
{
	if ( m_pConnection==conn )
	{
		Close();
		m_pTimerAxis->SetTimer(ETimerEventID_ReconnectNet,VOICE_RECONNECT_INTERVAL,this,1);
	}

	WarningLn(_GT("与语音服务器的连接出错，ErrorCode：") << dwErrorCode );
}

/// purpose: 定时器
void VoiceConnector::OnTimer( unsigned long dwTimerID )
{
	switch(dwTimerID)
	{
	case ETimerEventID_Keepalive:			//  检查心跳
		{
			if (m_bIsConnected)
			{
				bool bIsHandered = false;
				// 回应心跳检查
				if (!m_bIsAnswer)
				{
					DWORD dwNowTicks = ::GetTickCount();		// 最后收到心跳Tick时间
					if (dwNowTicks-m_dwLastAnswerTime>VOICE_KEEPALIVE_MAXTIMEOUT)
					{
						m_dwLastAnswerTime = dwNowTicks;
						bIsHandered = true;
						// 重连
						Close();
						m_pTimerAxis->SetTimer(ETimerEventID_ReconnectNet,VOICE_RECONNECT_INTERVAL,this,1);

						WarningLn("与语音服务器连接超过"<<VOICE_KEEPALIVE_MAXTIMEOUT<<"毫秒没有心跳,重连语音服务器：  " << m_szRemoteIP.c_str() <<":" << m_wRemotePort);
					}
				}

				if (!bIsHandered)
				{
					m_bIsAnswer = false;
					// 心跳检查
					SGameMsgHead header;
					header.SrcEndPoint = MSG_ENDPOINT_VOICEGATE;
					header.DestEndPoint= MSG_ENDPOINT_VOICE;
					header.byKeyModule  = MSG_MODULEID_VOICE;
					header.byKeyAction  = MSG_VOICE_KEEPALIVE;

					obuf obufData;
					obufData.push_back(&header, sizeof(SGameMsgHead));

					SendData(obufData.data(),obufData.size());

					//TraceLn("发送心跳检查!");
				}
			}
			else
			{
				m_bIsAnswer = true;
				m_dwLastAnswerTime = ::GetTickCount();		// 最后收到心跳Tick时间
			}
		}
		break;
	case ETimerEventID_ReconnectNet:					// 重连定时器
		{
			TraceLn(_GT("重新连接语音服务器：") << m_szRemoteIP.c_str() <<":" << m_wRemotePort);
			Connect(m_szRemoteIP.c_str(),m_wRemotePort);
		}
		break;

	default:break;
	}

}

/// purpose: 创建操作
bool VoiceConnector::Create( DWORD dwGetewayID )
{ 
	m_dwGetewayID = dwGetewayID;

	IGlobalVoiceGateway* pGlobalServer = gGlobalServer;
	if (pGlobalServer == NULL)
	{
		return false;
	}

	m_pTimerAxis = pGlobalServer->getTimerAxis();
	if( NULL == m_pTimerAxis )
	{
		return false;
	}

	return true;
}

/// purpose: 连接
bool VoiceConnector::Connect(const char * szCSIP,WORD nCSPort)
{
	if (NULL==szCSIP || 0==nCSPort)
	{
		return false;
	}
	if (m_pConnection!=NULL)
	{
		Close();
	}

	m_pConnection = CreateTCPConnection(0,this);
	if ( m_pConnection==0 )
	{
		return false;
	}

	m_szRemoteIP = szCSIP;
	m_wRemotePort= nCSPort;

	m_bIsConnected = false;

	char szSetName[256] = {0};
	::_snprintf_s(szSetName, sizeof(szSetName),_TRUNCATE, "语音服连接");
	m_pConnection->setName(_GT("语音服连接"));

	return m_pConnection->Connect(szCSIP,nCSPort);
}

// purpose: 关闭与语音服务器之间的连接
void VoiceConnector::Close()
{
	if ( m_pConnection )
	{
		m_pConnection->Release();
		m_pConnection = 0;
	}

	m_pTimerAxis->KillTimer(ETimerEventID_ReconnectNet,this);
	m_pTimerAxis->KillTimer(ETimerEventID_Keepalive,this);

	m_bIsConnected = false;
}

/// purpose: 是否连接
bool VoiceConnector::IsConnected()
{
	return m_bIsConnected;
}

// purpose: 主动重连
bool VoiceConnector::ReConnect()
{
	Close();

	WarningLn("主动重连 语音服务器 " << m_szRemoteIP.c_str() <<":" << m_wRemotePort);

	return Connect(m_szRemoteIP.c_str(),m_wRemotePort);
}

/// porpose: 释放操作
void VoiceConnector::Release()
{
	Close();

	delete this;
}

/// purpose: 发给语音服务器数据
void  VoiceConnector::SendData(const char * pData,DWORD dwLen)
{
	if ( m_pConnection )
	{
		((GlobalVoiceGateway*)gGlobalServer)->m_dwSendSpeedServer += dwLen;		// 统计发包流量

		m_pConnection->SendData(pData,dwLen);
	}
}

/// purpose: 语音网关信息
std::string VoiceConnector::ToString()
{
	ostrstream buffer;
	buffer << "语音网关[网关ID=" << m_dwGetewayID  << ",序号ID:"<<m_dwID<<"] 语音服务器[" << m_szRemoteIP <<":" << m_wRemotePort <<"]" << std::ends;
	return buffer.str();
}

/// purpose: 设置连接序号
void VoiceConnector::SetID(DWORD dwID)
{
	m_dwID = dwID;
}


/**
@name         : 获取队列里未处理的数据包个数
@name         : 获取队列是发包,否则是收包
@return       : 
*/
unsigned long VoiceConnector::GetPendingCount(bool bIsSend)
{
	if (NULL==m_pConnection)
	{
		return 0;
	}
	return m_pConnection->GetPendingCount(bIsSend);
}

/// purpose: 发送握手信息
bool VoiceConnector::SendHandshake()
{
	if ( m_bIsConnected==false || m_pConnection==0 )
	{
		WarningLn(_GT("还未连接，稍后发送握手信息！"));
		return false;
	}

	SGameMsgHead header;
	header.SrcEndPoint = MSG_ENDPOINT_VOICEGATE;
	header.DestEndPoint= MSG_ENDPOINT_VOICE;
	header.byKeyModule  = MSG_MODULEID_VOICE;
	header.byKeyAction  = MSG_VOICE_HANDSHAKE;

	// 语音网关与语音服务器握手
	SMsgVoiceHandshake_GV sendData;
	sendData.dwGatewayID	= m_dwGetewayID;			// 语音网关
	sendData.dwMaxCounts	= VoiceGatewaySetting::getInstance().m_dwMaxConnection;			// 最多人数

	obuf obufData;
	obufData.push_back(&header, sizeof(SGameMsgHead));
	obufData.push_back(&sendData, sizeof(SMsgVoiceHandshake_GV));

	SendData(obufData.data(),obufData.size());

	TraceLn(_GT("发送握手信息! ")<< a2utf8(ToString().data()));

	return true;
}