/*******************************************************************
** 文件名:	ServerUser.h 
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

#ifndef __SERVER_USER_H__
#define __SERVER_USER_H__

#include "net.h"
using namespace rkt;

#include <string>
using namespace std;

class TimeSyncServer;
//#include "DGlobalMessage.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
@name : 内网连进来的其他游戏服务器
@brief: 内网其他游戏服务器和网关的通讯协议见IGatewayService.h
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ServerUser : public IConnectionEventHandler
{
public:
	virtual void    OnAccept( IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler ){assert(false);}

	virtual void    OnConnected( IConnection * conn ){assert(false);}

	virtual void    OnDisconnected( IConnection * conn,DWORD reason );

	virtual void    OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen );

	virtual void    OnError( IConnection * conn,DWORD dwErrorCode );

    IConnection*    GetConnection() { return m_pConnection;}

    DWORD           GetID() { return m_ID; }

    void            SetID(DWORD id) { m_ID = id; }

    bool            SendData( const char * pData,DWORD dwDataLen,DWORD dwFlag=0);

	std::string     ToString();

	ServerUser(TimeSyncServer* pTimeSyncServer, IConnection * conn);
	
	virtual ~ServerUser();

protected:


protected:
    DWORD                   m_ID;
    TimeSyncServer*         m_pTimeSyncServer;
	IConnection    *        m_pConnection;    // 服务器与网关之间的连接
	string                  m_szRemoteIP;     // 服务器IP
	DWORD                   m_dwRemotePort;   // 连进来的端口
};

#endif//__SERVER_USER_H__