/*******************************************************************
** 文件名:	ServerUser.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2011-05-16
** 版  本:	1.0
** 描  述:	语音服务器用户
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "stdafx.h"
#include "ServerUser.h"
#include "ServerList.h"
#include "GlobalVoiceServer.h"
#include "VoiceServerSetting.h"
#include <strstream>

// 打印该服务器信息
#define  DUMP_THIS_SERVER  ToString()

///////////////////////////////////////////////////////////////////////////////////////////////
ServerUser::ServerUser(IConnection * conn) : m_pConnection(conn),m_dwID(0)
{
	const char * ip = 0;
	conn->GetRemoteAddr(&ip,m_dwRemotePort);
	assert(ip!=0);
	m_szRemoteIP = ip;

	m_bIsPublic = false;						// 是否是公共区的
	m_bIsAlive	= true;							// 是否有心跳
	m_bIsAnswer	= true;							// 是否有回应
	m_dwLastAnswerTime	= GetTickCount();		// 最后收到心跳Tick时间

	m_dwAddTime = (DWORD)time(NULL);			// 连入时间
	m_dwWorldID	= 0;			// 服务器所在游戏世界ID
	m_wServerType = 0;			// 服务器类型,会根据参见消息结构头第一个字段转发消息给不同的服务器
	m_wSubID = 0;				// 第n号服务器
	m_dwStateFlag = 0;			// 服务器状态

	memset(m_szName,0,sizeof(m_szName));

	TraceLn("A server connected!" << DUMP_THIS_SERVER);

}

///////////////////////////////////////////////////////////////////////////////////////////////
ServerUser::~ServerUser()
{
}


void ServerUser::Release()
{
	IGlobalVoiceServer* pGlobalServer = gGlobalServer;
	if (pGlobalServer!=NULL)
	{
		pGlobalServer->OnServerUserLeave(this);
	}

	ServerList::getInstance().DelUser(this);
	if ( m_pConnection )
	{
		m_pConnection->Release();
		m_pConnection=0;
	}
	delete this;
}

///////////////////////////////////////////////////////////////////////////////////////////////
void ServerUser::OnDisconnected( IConnection * conn,DWORD reason )
{
	TraceLn("Connection disconnected!" << DUMP_THIS_SERVER);
	Release();
}

///////////////////////////////////////////////////////////////////////////////////////////////
void ServerUser::OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen )
{
	assert(conn==m_pConnection);
	
	((GlobalVoiceServer*)gGlobalServer)->m_dwRecvSpeedServer += dwDataLen;		// 收包速度

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

	gGlobalServer->onServerMessage((*this), head->byKeyAction, head, data, new_len);
}

///////////////////////////////////////////////////////////////////////////////////////////////
void ServerUser::OnError( IConnection * conn,DWORD dwErrorCode )
{
	TraceLn("Connection occured an error!" << dwErrorCode << DUMP_THIS_SERVER);
	Release();
}

///////////////////////////////////////////////////////////////////////////////////////////////
DWORD ServerUser::GetServerID()
{
	return m_dwID;
}

///////////////////////////////////////////////////////////////////////////////////////////////
void ServerUser::SetServerID(DWORD dwID)
{
	m_dwID = dwID;
}

///////////////////////////////////////////////////////////////////////////////////////////////
WORD ServerUser::GetServerType()
{
	return m_wServerType;
}


const char * ServerUser::GetRemoteIP()
{
	return m_szRemoteIP.c_str();
}

DWORD ServerUser::GetRemotePort()
{
	return m_dwRemotePort;
}

///////////////////////////////////////////////////////////////////////////////////////////////
bool ServerUser::SendData( const char * pData,DWORD dwDataLen,DWORD dwFlag)
{
	Assert(m_pConnection);
	if (!m_pConnection)	{	return false;	}
	
	((GlobalVoiceServer*)gGlobalServer)->m_dwSendSpeedServer += dwDataLen;		// 发包速度

	return m_pConnection->SendData(pData,dwDataLen,dwFlag);
}

///////////////////////////////////////////////////////////////////////////////////////////////
std::string ServerUser::ToString()
{
	ostrstream buffer;
	buffer << " server["<<m_szName<<",ID="<< m_dwID << ",WorldID="<<m_dwWorldID<<",Type=" << m_wServerType<< ",SubID="<<m_wSubID<<"][" << m_szRemoteIP <<":" << m_dwRemotePort <<"]" << std::ends;
	return buffer.str();
}
