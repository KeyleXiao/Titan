/*******************************************************************
** 文件名:	TimeSyncClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	7/1/2014
** 版  本:	1.0
** 描  述:	

			时间同步客户端
********************************************************************/

#pragma  once

#include "net/net.h"
#include "buffer.h"
#include <string>
#include "TimerHandler.h"

#ifndef TIME_SYNC_PING
	#define  TIME_SYNC_PING    1        // ping
	#define  TIME_SYNC_TEST    2        // 测试
	#define  TIME_SYNC_ADJUST  3		// 校正
#endif

/**
	网络对时原因
	1.首先客户端发ping包给服务器,根据服务器tick+网络延时的1/2确定初始tick (优先使用udp协议,udp协议不通才使用tcp)
	2.确定了初始tick后，这个值可能误差还比较大,后面会定时发包测试，试图缩小误差
	3.测试方法：双方互发测试包，测试包里带发包时的逻辑tick
	    a.如果服务器收到时发现客户端发包时的tick大于服务器当前的tick,说明客户端快了,则通知客户端调整将初始tick - delta
		b.如果客户端收到时发现服务器发包时的tick大于客户端当前的tick,说明客户端慢了,则客户端调整将初始tick + delta
	4.持续步骤3,可将误差缩小的尽可能小
*/
class TimeSyncClient : public IUDPHandler,public IConnectionEventHandler,public TimerHandler
{
public:
	DWORD GetTick(){return getTickCountEx()-m_dwServerTick;}
	DWORD GetPing(){return m_dwPingLatency; }
	char * Getctime() { return ctime(&m_tStartTime); }

	bool DoSyncUPD()
	{
		m_dwPingLatency = 0;

		if ( m_pUDPSocket==0 )
		{
			m_pUDPSocket = rkt::CreateUDPSocket( 0,this );
			if ( m_pUDPSocket==0 )
			{
				ErrorLn("can not create CreateUDPSocket" << getLastError());
				return false;
			}
		}

		rkt::obuf32 ob;
		unsigned char opcode = TIME_SYNC_PING;
		DWORD nClientTick    = getTickCountEx();
		ob << opcode << nClientTick;

		if ( !m_pUDPSocket->SendData( m_strServerIP.c_str(),m_nUPDPort,ob.begin(),ob.size() ) )
		{
			ErrorLn("upd socket send error" << getLastError());
			return false;
		}

		return true;
	}

	bool DoSyncTCP()
	{
		m_dwPingLatency = 0;

		if ( m_pTCPConnection!=0 )
		{
			m_pTCPConnection->Release();
			m_pTCPConnection = 0;
		}

		m_pTCPConnection = rkt::CreateTCPConnection(0,this );
		if ( m_pTCPConnection==0 )
		{
			ErrorLn("can not create CreateTCPConnection" << getLastError());
			return false;
		}

		return m_pTCPConnection->Connect(m_strServerIP.c_str(),m_nTCPPort );
	}

	bool DoTestUPD()
	{
		m_dwPingLatency = 0;

		if ( m_pUDPSocket==0 )
		{
			m_pUDPSocket = rkt::CreateUDPSocket( 0,this );
			if ( m_pUDPSocket==0 )
			{
				ErrorLn("can not create CreateUDPSocket" << getLastError());
				return false;
			}
		}

		rkt::obuf32 ob;
		unsigned char opcode = TIME_SYNC_TEST;
		DWORD nClientTick    = getTickCountEx();
		DWORD nSendTick      = GetTick();
		ob << opcode << nClientTick << nSendTick;

		if ( !m_pUDPSocket->SendData( m_strServerIP.c_str(),m_nUPDPort,ob.begin(),ob.size() ) )
		{
			ErrorLn("upd socket send error" << getLastError());
			return false;
		}

		return true;
	}

	void __doSync( LPVOID pData,DWORD dwDataLen )
	{
		rkt::ibuffer inbuf( pData,dwDataLen );
		unsigned char opcode = 0;

		inbuf >> opcode;
		if ( !inbuf )
		{
			return;
		}

		switch( opcode )
		{
		case TIME_SYNC_PING:
			OnRecvPing( opcode,inbuf );
			//break; 所有的包前面格式都是一样的
		case TIME_SYNC_TEST:
			break;
		case TIME_SYNC_ADJUST:
			OnRecvAdjust( opcode,inbuf );
			break;
		}
	}

	// ping包
	void OnRecvPing( unsigned char opcode,rkt::ibuffer & inbuf )
	{
		DWORD nClientTick = 0;
		DWORD nServerTick = 0;
		inbuf >> nClientTick >> nServerTick;
		if ( !inbuf )
		{
			return;
		}

		// 取时间
		if ( opcode == TIME_SYNC_PING )
		{
			inbuf >> m_tStartTime;
		}

		m_dwPingLatency = (getTickCountEx() - nClientTick)/2;

		if ( m_dwServerTick==0 )
		{
			// 首次ping
			m_dwServerTick = getTickCountEx() - (nServerTick + m_dwPingLatency);
			m_dwLastLatency= m_dwPingLatency;
		}else
		{
			DWORD nCurTick  = GetTick();

			// 客户端慢了
			if ( nServerTick >nCurTick  )
			{
				Adjust( nServerTick-nCurTick+10 );
			}else
			{
				// 延迟越低，对得越准
				if ( m_dwPingLatency<m_dwLastLatency/2 )
				{
					m_dwServerTick = getTickCountEx() - (nServerTick + m_dwPingLatency);
					m_dwLastLatency= m_dwPingLatency;
				}
			}
		}

		// 误差很小时不用再对
		if ( m_dwLastLatency>0 && m_dwLastLatency<30 )
		{
			m_pTimerAixs->SetTimer(1,10000,this);
		}
	}

	void OnRecvAdjust( unsigned char opcode,rkt::ibuffer & inbuf )
	{
		long nAdjust = 0;
		inbuf >> nAdjust;
		if ( !inbuf )
		{
			return;
		}

		Adjust(-nAdjust);
	}

	void Adjust(long delta )
	{
		m_dwServerTick += delta;
		ErrorLn("TimeSync Adjust " << delta);
	}

	virtual void OnTimer( unsigned long dwTimerID )
	{
		if ( dwTimerID==0 )
		{
			DoSyncUPD();
		}

		if ( dwTimerID==2 )
		{
			// 超时了就是用tcp协议重新对时
			if ( m_dwPingLatency==0 )
			{
				DoSyncTCP();
			}else
			{
				m_pTimerAixs->KillTimer(2,this);
			}
		}

		if ( dwTimerID==1 )
		{
			// 向服务器发送测试包，校对tick准确性
			DoTestUPD();
		}		
	}

	/**
	@name             : 数据到达事件
	@brief            : UDP套接字收到数据时回调用
	@para  socket     : 收到数据的套接字
	@param pData      : 数据地址
	@pram dwDataLen   : 数据长度
	@param pRemoteAddr : 对方IP,端口地址,为 sockaddr_in * 类型,要取得字符IP请用 inet_ntoa(pAddr->sin_addr)
	*/
	virtual void OnRecv( IUDPSocket * socket,LPVOID pData,DWORD dwDataLen,void * pRemoteAddr )
	{
		__doSync( pData,dwDataLen );
	}

	/**
	@name             : 网络错误事件
	@brief            :
	@param socket     : 出现错误的套接字
	@param dwErrorCode: 错误码
	*/
	virtual void OnError( IUDPSocket * socket,DWORD dwErrorCode )
	{
		ErrorLn("udpsocket error " <<dwErrorCode );
	}

	/**
	@brief              : 收到被动连接
	@param pIncomingConn: 客户端连入的新连接
	@param ppHandler    : 处理该连接的Handler
	@warning            : 收到连接后一定要设置Handler
	*/
	virtual void OnAccept( IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler )
	{

	}

	/**
	@brief      : 连接成功
	@param conn : 连接对象指针
	*/
	virtual void OnConnected( IConnection * conn )
	{
		unsigned char opcode = TIME_SYNC_PING;
		DWORD nClientTick = getTickCountEx();

		rkt::obuf32 ob;
		ob << opcode;
		ob << nClientTick;

		conn->SendData( ob.begin(),ob.size() );
	}

	/**
	@brief      : 连接断开
	@param conn : 连接对象指针
	@param reason:断开原因(保留) 
	*/
	virtual void OnDisconnected( IConnection * conn,DWORD reason )
	{
		if ( m_pTCPConnection )
		{
			m_pTCPConnection->Release();
			m_pTCPConnection = 0;
		}
	}

	/**
	@brief        : 收到数据
	@param conn   : 连接对象指针
	@param pData  : 数据地址
	@pram dwDataLen:数据长度
	@return       :
	*/
	virtual void OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen )
	{
		__doSync( pData,dwDataLen );

		conn->Disconnect();
	}

	/**
	@name             : 网络错误事件
	@brief            :
	@param conn       : 出现错误的套接字
	@param dwErrorCode: 错误码
	*/
	virtual void OnError( IConnection * conn,DWORD dwErrorCode )
	{
		if ( m_pTCPConnection )
		{
			m_pTCPConnection->Release();
			m_pTCPConnection = 0;
		}
	}

	bool Start( const char * strIP,unsigned short nUDPPort,unsigned short mTCPPort,TimerAxis * pAxis)
	{
		m_pTimerAixs = pAxis;

		m_strServerIP = strIP;
		m_nUPDPort = nUDPPort;
		m_nTCPPort = mTCPPort;

		m_pTimerAixs->SetTimer(0,1,this,1 );
		m_pTimerAixs->SetTimer(1,10000,this,100 );
		m_pTimerAixs->SetTimer(2,1000,this );
		return true;
	}

	void Stop()
	{
		if ( m_pTimerAixs )
		{
			m_pTimerAixs->KillTimer(this);
		}

		if ( m_pUDPSocket )
		{
			m_pUDPSocket->Release();
			m_pUDPSocket = 0;
		}

		if ( m_pTCPConnection )
		{
			m_pTCPConnection->Release();
			m_pTCPConnection = 0;
		}
	}

	TimeSyncClient() : m_pTimerAixs(0),m_nUPDPort(0),m_nTCPPort(0),m_pUDPSocket(0),m_pTCPConnection(0),m_dwServerTick(0),m_dwPingLatency(0),m_dwLastLatency(0),m_tStartTime(0){}

	virtual ~TimeSyncClient() { Stop(); }

protected:
	TimerAxis    *    m_pTimerAixs;
	IUDPSocket   *    m_pUDPSocket;
	IConnection  *    m_pTCPConnection;

	std::string       m_strServerIP;
	unsigned short    m_nUPDPort;
	unsigned short    m_nTCPPort;

	DWORD             m_dwServerTick;	// 服务器逻辑开始时间
	DWORD             m_dwPingLatency;	// 当前的网络延时
	DWORD             m_dwLastLatency;	// 最后一次更新m_dwServerTick时的网络延时，后面再更新，延迟必须小于之前的1/2
	time_t            m_tStartTime;     // 从1970年1月1日00:00:00到现在总共的秒数
};