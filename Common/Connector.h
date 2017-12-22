/*******************************************************************
** 文件名:	Connector.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/11/21
** 版  本:	1.0
** 描  述:	有保持心跳和自动重连功能
** 应  用:  主动连接器基类
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "net.h"
#include <string>
#include "TimerHandler.h"
#include "IMsg.h"
#include <strstream>

using namespace rkt;
using namespace std;

class Connector : public IConnectionEventHandler
					 , public TimerHandler
{
	// 这里已经使用了2个TimerID，子类在使用Timer时，注意不要重复
	enum
	{
		ETimerEventID_ReconnectNet = 0,
		ETimerEventID_Keepalive,
	};

public:

	Connector(TimerAxis& timerAxis) :
		m_pConnection(0)
		, m_TimerAxis(timerAxis)
		, m_wRemotePort(0)
		, m_bIsAnswer(true)
		, m_dwLastAnswerTime(0)
		, m_bIsConnected(false)
	{}

	virtual ~Connector() {}

	////////////////////// TimerHandler ////////////////////////////////////////////////////
	virtual void OnTimer(unsigned long dwTimerID)
	{
		switch (dwTimerID)
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
					if (dwNowTicks - m_dwLastAnswerTime > GetMaxTimeout())
					{
						m_dwLastAnswerTime = dwNowTicks;
						bIsHandered = true;
						// 重连
						Close();
						m_TimerAxis.SetTimer(ETimerEventID_ReconnectNet, GetReconnectInterval(), this, 1);

						WarningLn(GetDestPointName() << "连接超过" << GetMaxTimeout() << "毫秒没有心跳,重连之：  " << m_szRemoteIP.c_str() << ":" << m_wRemotePort);
					}
				}

				if (!bIsHandered)
				{
					m_bIsAnswer = false;

					const obuf& obufData = GetKeepAliveMsg();
					const DWORD dwLen = (DWORD)obufData.size();
					Assert(dwLen == obufData.size());
					SendData(obufData.data(), dwLen);
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
			TraceLn(_GT("重新连接：") << GetDestPointName() << m_szRemoteIP.c_str() << ":" << m_wRemotePort);
			Connect(m_szRemoteIP.c_str(), m_wRemotePort);
		}
		break;

		default:break;
		}

	}
	///////////////////////////////////////////////////////////////////////////////////////////

	////////////////////// IConnectionEventHandler ////////////////////////////////////////////
	virtual void OnAccept(IConnection * pIncomingConn, IConnectionEventHandler ** ppHandler) {assert(false);}

	virtual void OnConnected( IConnection * conn )
	{
		m_bIsConnected = true;
		SendHandshake();
		m_TimerAxis.SetTimer(ETimerEventID_Keepalive, VOICE_KEEPALIVE_INTERVAL, this);
	}

	virtual void OnDisconnected(IConnection * conn, DWORD reason)
	{
		if (m_pConnection == conn)
		{
			Close();
			m_TimerAxis.SetTimer(ETimerEventID_ReconnectNet, VOICE_RECONNECT_INTERVAL, this, 1);
		}
		WarningLn("与语音服务器的连接断开，reason：" << reason);
	}

	virtual void OnError( IConnection * conn,DWORD dwErrorCode )
	{
		if (m_pConnection == conn)
		{
			Close();
			m_TimerAxis.SetTimer(ETimerEventID_ReconnectNet, VOICE_RECONNECT_INTERVAL, this, 1);
		}
		WarningLn(_GT("与语音服务器的连接出错，ErrorCode：") << dwErrorCode);
	}
	///////////////////////////////////////////////////////////////////////////////////////////
	

	inline const char*	GetRemoteIP() { return m_szRemoteIP.c_str(); }
	inline WORD			GetRemotePort() { return m_wRemotePort; }

	bool Connect(const char * szCSIP,WORD nCSPort)
	{
		if (NULL == szCSIP || 0 == nCSPort)
			return false;
		if (m_pConnection != NULL)
		{
			Close();
		}

		m_pConnection = CreateTCPConnection(0, this);
		if (m_pConnection == NULL)
			return false;

		m_szRemoteIP = szCSIP;
		m_wRemotePort = nCSPort;

		m_bIsConnected = false;

		char szSetName[256] = { 0 };
		::_snprintf_s(szSetName, sizeof(szSetName), _TRUNCATE, GetDestPointName());
		m_pConnection->setName(szSetName);

		return m_pConnection->Connect(szCSIP, nCSPort);
	}

	void Close()
	{
		if (m_pConnection)
		{
			m_pConnection->Release();
			m_pConnection = 0;
		}

		m_TimerAxis.KillTimer(ETimerEventID_ReconnectNet, this);
		m_TimerAxis.KillTimer(ETimerEventID_Keepalive, this);

		m_bIsConnected = false;
	}

	bool IsConnected() { return m_bIsConnected; }

	// 主动重连
	bool ReConnect()
	{
		Close();
		WarningLn("主动重连:  " << GetDestPointName() << m_szRemoteIP.c_str() << ":" << m_wRemotePort);

		return Connect(m_szRemoteIP.c_str(), m_wRemotePort);
	}

	void Release()
	{
		Close();
		delete this;
	}

	// 发送消息，简化版
	template<typename TMsg>
	bool SendMsg(TMsg& msg)
	{
		if (m_pConnection == NULL)
			return false;

		SGameMsgHead header;
		header.SrcEndPoint = msg.GetSrcEndPoint();
		header.DestEndPoint = msg.GetDestEndPoint();
		header.byKeyModule = msg.GetModuleId();
		header.byKeyAction = msg.GetActionId();

		obuf obufData;
		TBuildObufMsg(obufData, header, msg);

		const DWORD dwLen = (DWORD)obufData.size();
		Assert(dwLen == obufData.size());

		if (!m_pConnection->SendData(obufData.data(), dwLen))
			return false;

		OnSendData(dwLen);

		return true;
	}

	virtual std::string ToString() = 0;

	/**
	@name         : 获取队列里未处理的数据包个数
	@name         : 获取队列是发包,否则是收包
	@return       : 
	*/
	unsigned long GetPendingCount(bool bIsSend=false)
	{
		if (NULL == m_pConnection)
		{
			return 0;
		}
		return m_pConnection->GetPendingCount(bIsSend);
	}

protected:
	// 取得目标节点的名称
	virtual	const char*		GetDestPointName() = 0;
	// 发送完数据，会被调用，用来统计流量
	virtual void			OnSendData(DWORD dwDataLen) = 0;
	// 发送握手消息
	virtual	bool			SendHandshake() = 0;
	// 取缓冲好的心跳包
	virtual	const obuf&		GetKeepAliveMsg() = 0;
	// 连接允许的最大超时时间，单位：毫秒
	virtual	DWORD			GetMaxTimeout() = 0;
	// 重连的时间间隔，单位：毫秒
	virtual	DWORD			GetReconnectInterval() = 0;

private:
	// 发送消息
	void SendData(const char * pData, DWORD dwLen)
	{
		if (m_pConnection)
		{
			m_pConnection->SendData(pData, dwLen);
			OnSendData(dwLen);
		}
	}

protected:
	TimerAxis&			m_TimerAxis;	// 时间轴（由外部传入）

	IConnection*		m_pConnection;
	std::string			m_szRemoteIP;
	WORD				m_wRemotePort;
	bool				m_bIsConnected;

public:
	bool	m_bIsAnswer;			// 是否有回应
	DWORD	m_dwLastAnswerTime;		// 最后收到心跳Tick时间

};
