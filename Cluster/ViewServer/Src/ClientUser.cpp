/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceGateway\Src\ClientUser.cpp
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	宋文武
** 日  期:	2012-11-21
** 版  本:	1.0
** 描  述:	语音客户端用户
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "stdafx.h"
#include "ClientUser.h"
#include "ClientList.h"
#include <strstream>
using namespace std;
#include "Winsock2.h"
#include "GlobalViewServer.h"


CClientUser::CClientUser( IConnection * conn )
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
	m_dwAudioUserID = VOICE_MAX_CLIENT_COUNT;

	m_nKeyLen = 0;

	memset( m_pKeyData, 0, sizeof(m_pKeyData) );

	CreateKeyData();

	m_dwLastKeepaliveTime = ::GetTickCount();
	gGlobalServer->SetTimer( ETimerEventID_Check, VOICE_KEEPALIVE_TIMEOUT*100, this, "CAudioUser::CClientUser" );
}


CClientUser::~CClientUser(void)
{
}

// 生成密钥
void CClientUser::CreateKeyData(void)
{
	m_nKeyLen = VOICE_RC4_KEY_SIZE;
	for ( int i=0;i<m_nKeyLen;++i )
	{
		m_pKeyData[i] = rand()% ('z'-'A') + 'A';
	}
}

/// purpose: 客户端断开
void CClientUser::OnDisconnected( IConnection * conn,DWORD reason )
{
	WarningLn("用户断开!" << "reason["<<reason<<"]，" << ToString());
	Release();
}

/// purpose: 客户端接收数据
void CClientUser::OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen )
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
	ulong actionId = head->byKeyAction;
	uchar* data = (uchar*)pData + sizeof(SGameMsgHead);
	size_t new_len =(size_t)(dwDataLen-sizeof(SGameMsgHead));
	if (new_len == 0) data = 0;

	if( moduleId != MSG_MODULEID_VOICE )
	{
		ErrorLn(_GT("客户端发来一个错误模块消息，moduleId:") << moduleId);
		return ;
	}

	// 是否验证通过的
	if (!m_bVerifyed)
	{
		// 如果没有验证通过，只收握手消息
		if( actionId != MSG_VOICE_HANDSHAKE )
		{
			ErrorLn(_GT("未验证客户端发来一个错误消息，actionId:") << actionId);
			return ;
		}
	}

	if( MSG_ENDPOINT_VOICEGATE == head->DestEndPoint )
	{
		gGlobalServer->onClientMessage((*this), head->byKeyAction, head, data, new_len);
	}
	else
	{
		//// 转发到语音服务器
		//gGlobalServer->onTransVoiceServer((*this), head->byKeyAction, head, pData, dwDataLen);
	}

	((GlobalViewServer*)gGlobalServer)->m_dwRecvSpeedClient += dwDataLen;		// 统计收包流量
}

/// purpose: 客户端连接错误
void CClientUser::OnError( IConnection * conn,DWORD dwErrorCode )
{
	TraceLn(_GT("网络Error[") << dwErrorCode << _GT("]，关闭客户端连接，") << ToString());
	Release();
}

// 定时器
void CClientUser::OnTimer( unsigned long dwTimerID )
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
std::string CClientUser::ToString()
{
	ostrstream buffer;
	buffer << "ID[" << m_dwID 
		<< "]，IP[" << m_szRemoteIP
		<< ":" << m_dwRemotePort
		<< "]，昵称[" << m_szNickname
		<< "]，PDBID[" << m_dwPDBID << "]" << ends;

	return buffer.str();
}

/// purpose: 关闭连接
void CClientUser::Close()
{
	if ( m_pConnection )
	{
		m_pConnection->Release();
		m_pConnection = 0;
	}
}

/// purpose: 释放
void CClientUser::Release()
{
	IGlobalViewServer* pGlobalServer = gGlobalServer;
	if (pGlobalServer!=NULL)
	{
		pGlobalServer->KillTimer( ETimerEventID_Check, this );
		pGlobalServer->OnClientUserLeave(this);
	}

	// 告诉游戏服务器该连接断开了
	CClientList::getInstance().DelUser(this);

	Close();

	delete this;
}

/// purpose: 发送数据
bool CClientUser::SendData(LPVOID pData,DWORD dwDataLen,DWORD dwFlag)
{
	Assert(m_pConnection);
	if (!m_pConnection)	{	return false;	}

	((GlobalViewServer*)gGlobalServer)->m_dwSendSpeedClient += dwDataLen;		// 统计发包流量
	return m_pConnection->SendData((const char *)pData,dwDataLen,dwFlag);
}

/// purpsoe: 断开连接
void CClientUser::Disconnect()
{
	if ( m_pConnection )
	{
		m_pConnection->Disconnect();
	}
}

/// purpose: 获取端口
WORD CClientUser::GetPort()
{
	return (WORD)m_dwRemotePort;
}

/// purpose: 获得IP
const char *CClientUser::GetClientIP()
{
	return m_szRemoteIP.c_str();
}

/// purpose: 获得整形IP
DWORD CClientUser::GetIP()
{
	if( NULL == m_pConnection )
	{
		return 0;
	}

	return inet_addr( m_szRemoteIP.c_str() );
}

/// purpose: 设置客户端ID
void CClientUser::SetID( DWORD dwID )
{
	m_dwID = dwID;
}

/// purpose: 获得客户端ID
DWORD CClientUser::GetID()
{
	return m_dwID;
}

/// purpose: 获得网关ID
DWORD CClientUser::GetGatewayID()
{
	return ((VClientIDInfo *)&m_dwID)->dwGatewayID;
}

/// purpose: 获得MAC地址
const char *CClientUser::GetMacAddress()
{
	return m_szMacAddres.c_str();
}

DWORD CClientUser::GetConnectTime(void)
{
	return m_dwConnectTime;
}

// 校验数据
bool CClientUser::CheckKeyData( char* szKeyData, DWORD dwKeyLen )
{
	if( szKeyData == NULL )
	{
		return false;
	}

	if( m_nKeyLen != (BYTE)dwKeyLen )
	{
		return false;
	}

	for ( int i=0; i<m_nKeyLen; ++i )
	{
		if( m_pKeyData[i] != szKeyData[i] )
		{
			return false;
		}
	}

	return true;
}

// 设置语音通道ID
void CClientUser::SetAudioUserID( DWORD dwID )
{
	m_dwAudioUserID = dwID;
}

// 获取语音通道ID
DWORD CClientUser::GetAudioUserID(void)
{
	return m_dwAudioUserID;
}
