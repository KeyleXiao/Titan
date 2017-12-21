/*******************************************************************
** 文件名:	Acceptor.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	02/15/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __NET_ACCEPTOR_H__
#define __NET_ACCEPTOR_H__

#include "net.h"
using namespace rkt;

struct IAcceptorHandler
{
	virtual void OnAccept( WORD wListenPort,IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler ) = 0;
};

/**
@name : 连接接收器
@brief:
*/
class Acceptor : public IConnectionEventHandler
{
public:
	//////////////////////////////////////////////////////////////////////////
	virtual void OnAccept( IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler )
	{
         m_pHandler->OnAccept(m_wListenPort,pIncomingConn,ppHandler);
	}

	virtual void OnConnected( IConnection * conn )
	{
		__debugbreak();
	}

	virtual void OnDisconnected( IConnection * conn,DWORD reason )
	{
	}

	virtual void OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen )
	{
		__debugbreak();	
	}

	virtual void OnError( IConnection * conn,DWORD dwErrorCode )
	{
		ErrorLn(_GT("监听端口") << m_wListenPort << _GT("出错,错误代码") << dwErrorCode);
		m_pListener->SetEventHandler(this);
	}

	bool Create(WORD wListenPort,IAcceptorHandler * pHandler,IPacker * protocol)
	{
		m_pListener = CreateTCPConnection(wListenPort,this,protocol);
		if ( m_pListener==0 || m_pListener->Listen()==false )
		{
			ErrorLn("bind port error!" << wListenPort )
			return false;
		}

		m_wListenPort   = wListenPort;
		m_pHandler      = pHandler;
		return true;
	}

    bool Release()
    {
        if ( m_pListener )
        {
            m_pListener->Release();
            m_pListener = 0;
            return true;
        }
        return false;
    }

	WORD GetListenPort() { return m_wListenPort; }

	Acceptor()
        : m_pListener(0),m_pHandler(0),m_wListenPort(0)
    {

    }

	virtual ~Acceptor() 
	{
		if ( m_pListener )
		{
			m_pListener->Release();
			m_pListener = 0;
		}
	}

protected:
	IConnection      *       m_pListener;
	IAcceptorHandler *       m_pHandler;
	WORD                     m_wListenPort;
};

#endif//__NET_ACCEPTOR_H__