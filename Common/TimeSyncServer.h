/*******************************************************************
** 文件名:	TimeSyncServer.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	7/1/2014
** 版  本:	1.0
** 描  述:	

			时间同步服务器
********************************************************************/

#pragma  once

#include "net/net.h"
#include "buffer.h"
#include <time.h>

#define  TIME_SYNC_PING    1        // ping
#define  TIME_SYNC_TEST    2        // 测试
#define  TIME_SYNC_ADJUST  3		// 校正

/**
	网络对时原因
	1.首先客户端发ping包给服务器,根据服务器tick+网络延时的1/2确定初始tick (优先使用udp协议,udp协议不通才使用tcp)
	2.确定了初始tick后，这个值可能误差还比较大,后面会定时发包测试，试图缩小误差
	3.测试方法：双方互发测试包，测试包里带发包时的逻辑tick
	    a.如果服务器收到时发现客户端发包时的tick大于服务器当前的tick,说明客户端快了,则通知客户端调整将初始tick - delta
		b.如果客户端收到时发现服务器发包时的tick大于客户端当前的tick,说明客户端慢了,则客户端调整将初始tick + delta
	4.持续步骤3,可将误差缩小的尽可能小
*/
class TimeSyncServer : public IUDPHandler,public IConnectionEventHandler
{
public:
	virtual void OnRecv( IUDPSocket * socket,LPVOID pData,DWORD dwDataLen,void * pRemoteAddr )
	{
		if ( pRemoteAddr==0 )
			return;

		rkt::ibuffer inbuf( pData,dwDataLen );
		unsigned char opcode = 0;
		
		inbuf >> opcode;
		if ( !inbuf )
		{
			return;
		}

		rkt::obuf32 ob;

		switch( opcode )
		{
		case TIME_SYNC_PING:
			OnRecvPing( opcode,inbuf,ob );
			break;
		case TIME_SYNC_TEST:
			OnRecvTest( opcode,inbuf,ob );
			break;
		}

		socket->SendData(pRemoteAddr,ob.begin(),ob.size() );
	}

	// ping包
	void OnRecvPing( unsigned char opcode,rkt::ibuffer & inbuf,rkt::obuf32 & ob )
	{
		DWORD nClientTick = 0;
		inbuf >> nClientTick;
		if ( !inbuf )
		{
			return;
		}

		ob << opcode;
		ob << nClientTick;
		ob << GetTick();
		ob << m_tStartTime;
	}

	// 测试包
	void OnRecvTest( unsigned char opcode,rkt::ibuffer & inbuf,rkt::obuf32 & ob )
	{
		DWORD nClientTick = 0;
		DWORD nSendTick = 0;
		inbuf >> nClientTick >> nSendTick;

		if ( !inbuf )
		{
			return;
		}

		// 客户端快了
		DWORD dwServerCur = GetTick();
		if ( nSendTick>dwServerCur )
		{
			opcode = TIME_SYNC_ADJUST;
			long lAdjust = nSendTick-dwServerCur;
			lAdjust += 10;

			ob << opcode;
			ob << nClientTick;
			ob << dwServerCur;
			ob << lAdjust;
		}else
		{
			// 回复测试包
			ob << opcode;
			ob << nClientTick;
			ob << dwServerCur;
		}
	}

	virtual void OnError( IUDPSocket * socket,DWORD dwErrorCode )
	{
		ErrorLn("udpsocket error " <<dwErrorCode );
	}


	virtual void OnAccept( IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler )
	{

	}

	virtual void OnConnected( IConnection * conn )
	{

	}

	virtual void OnDisconnected( IConnection * conn,DWORD reason )
	{

	}

	virtual void OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen )
	{
		rkt::ibuffer inbuf( pData,dwDataLen );
		unsigned char opcode = 0;

		inbuf >> opcode;
		if ( !inbuf )
		{
			return;
		}

		rkt::obuf32 ob;

		switch( opcode )
		{
		case TIME_SYNC_PING:
			OnRecvPing( opcode,inbuf,ob );
			break;
		case TIME_SYNC_TEST:
			OnRecvTest( opcode,inbuf,ob );
			break;
		}

		conn->SendData( ob.begin(),ob.size() );
	}

	virtual void OnError( IConnection * conn,DWORD dwErrorCode )
	{

	}

	DWORD GetTick()
	{
		return getTickCountEx() - m_dwStartTick;
	}

	void AdjustStart( DWORD dwTick  )
	{
		DWORD dwMyTick = GetTick();
		long delta = dwMyTick - dwTick;
		m_dwStartTick += delta;
		m_tStartTime  += delta/1000; // 从1970年1月1日00:00:00到现在总共的秒数
	}

	bool Start( unsigned short nPort )
	{ 
		m_tStartTime = time(0);             // 从1970年1月1日00:00:00到现在总共的秒数
		m_dwStartTick= getTickCountEx();    // 使用高性能时钟计数

		m_pUDPSocket = rkt::CreateUDPSocket( nPort,this );
		if ( m_pUDPSocket==0 )
		{
			return false;
		}

		m_pTCPConnection = rkt::CreateTCPConnection( nPort + 1,this );
		if ( m_pTCPConnection==0 )
		{
			return false;
		}

		return true;
	}

	void Stop()
	{
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

	TimeSyncServer() : m_pUDPSocket(0),m_pTCPConnection(0){}

protected:
	IUDPSocket   *    m_pUDPSocket;
	IConnection  *    m_pTCPConnection;
	
	DWORD             m_dwStartTick;
	time_t            m_tStartTime;    // 从1970年1月1日00:00:00到现在总共的秒数
};