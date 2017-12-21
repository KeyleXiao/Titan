/*******************************************************************
** 文件名:	ServerUser.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	02/14/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "stdafx.h"
#include "ServerUser.h"
#include "ServerList.h"
//#include "netbuffer.h"
#include <vector>
#include <strstream>
#include "TimeSyncServer.h"

// 打印该服务器信息
#define  DUMP_THIS_SERVER  ToString()

///////////////////////////////////////////////////////////////////////////////////////////////
ServerUser::ServerUser(TimeSyncServer* pTimeSyncServer, IConnection * conn)
    : m_pTimeSyncServer(pTimeSyncServer)
    , m_pConnection(conn)
{
    m_ID = 0;
	const char * ip = 0;
	conn->GetRemoteAddr(&ip,m_dwRemotePort);
	assert(ip!=0);
	m_szRemoteIP = ip;

	TraceLn("A server connected!" << DUMP_THIS_SERVER);
}

///////////////////////////////////////////////////////////////////////////////////////////////
ServerUser::~ServerUser()
{
	if ( m_pConnection )
	{
		m_pConnection->Release();
		m_pConnection=0;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
void ServerUser::OnDisconnected( IConnection * conn,DWORD reason )
{
	TraceLn("Connection disconnected!" << DUMP_THIS_SERVER);
	ServerList::getInstance().DelUser(this);

	delete this;
}

///////////////////////////////////////////////////////////////////////////////////////////////
void ServerUser::OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen )
{
    assert(conn==m_pConnection);

    m_pTimeSyncServer->OnRecv(conn, pData, dwDataLen);
}

///////////////////////////////////////////////////////////////////////////////////////////////
void ServerUser::OnError( IConnection * conn,DWORD dwErrorCode )
{
	TraceLn("Connection occured an error!" << dwErrorCode << DUMP_THIS_SERVER);

	ServerList::getInstance().DelUser(this);
	delete this;
}

///////////////////////////////////////////////////////////////////////////////////////////////
bool ServerUser::SendData( const char * pData,DWORD dwDataLen,DWORD dwFlag)
{
#ifdef OPEN_BVTTEST
	PP_BY_NAME("ServerUser::SendData");
#endif

	Assert(m_pConnection);
	return m_pConnection->SendData(pData,dwDataLen,dwFlag);
}


///////////////////////////////////////////////////////////////////////////////////////////////
std::string ServerUser::ToString()
{
	ostrstream buffer;
	buffer << " TCP [" << m_ID << "][" << m_szRemoteIP <<":" << m_dwRemotePort <<"]" << std::ends;
	return buffer.str();
}
