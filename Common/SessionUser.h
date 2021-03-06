/*******************************************************************
** 文件名:	SessionUser.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/11/28
** 版  本:	1.0
** 描  述:	监听到的连接基类
** 应  用:  如果达到指定时间未收到心跳包，会自动断开连接
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "Processer.h"
#include "ISessionUser.h"
#include "IByteRecord.h"
#include "IUserList.h"
#include "IMsg.h"


// 用于监听到的连接
template<typename TYPE, typename TypeID = DWORD>
class SessionUser : public ISessionUser<TypeID>, public TimerHandler
{
	// 这里已经使用了1个TimerID，子类在使用Timer时，注意不要重复
	enum
	{
		ETimerEventID_CheckKeepalive = 0,
	};
	static const DWORD LISTENER_KEEPALIVE_TIMEOUT = 300 * 1000;	// 默认心跳超时时间
	static const DWORD LISTENER_KEEPALIVE_INTERVAL = 30 * 1000;	// 默认心跳包检测间隔时间

public:
	////////////////////////////////IConnectionEventHandler//////////////////////////////////////////////////////
	//因为是接到连接后，才创建此类，所以不会调用此方法
	virtual void OnAccept(IConnection * pIncomingConn, IConnectionEventHandler ** ppHandler) { assert(false); }
	//因为是用于监听的，所以不会主动去连接，自然不会调用此方法
	virtual void OnConnected(IConnection * conn) { assert(false); }

	virtual void OnDisconnected(IConnection * conn, DWORD reason);

	virtual void OnRecv(IConnection * conn, LPVOID pData, DWORD dwDataLen);

	virtual void OnError(IConnection * conn, DWORD dwErrorCode);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////TimerHandler//////////////////////////////////////////////////////
	virtual void OnTimer(unsigned long dwTimerID);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
	SessionUser(IConnection * conn, IUserList<TypeID>& pUserList, Processer<TYPE>& processer, TimerAxis& timerAxis, IByteRecord* pByteRecord = nullptr)
		: m_UserList(pUserList)
		, m_pConnection(conn)
		, m_Processer(processer)
		, m_TimerAxis(timerAxis)
		, m_pByteRecord(pByteRecord)
		, m_dwID(0)
	{
		const char * ip = 0;
		conn->GetRemoteAddr(&ip, m_dwRemotePort);
		assert(ip != 0);
		m_szRemoteIP = ip;
		m_dwLastKeepaliveTime = ::GetTickCount();

		m_TimerAxis.SetTimer(ETimerEventID_CheckKeepalive, GetKeepaliveInterval(), this, INFINITY_CALL, "SessionUser::ETimerEventID_CheckKeepalive");
	}
	virtual ~SessionUser(){}

	// 关闭连接，关闭成功后回调OnDisconnected
	void			Disconnect();

	void			Release();

	// 处理未注册的消息（比如要转发的消息）。如有必要，欢迎override
	virtual	void	UnHandled(LPVOID pData, DWORD dwDataLen);

	// 发送消息，简化版
	template<typename TMsg>
	bool			SendMsg(TMsg& msg)
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

	bool SendData(const char * pData, size_t dwDataLen);

	const char*		GetRemoteIP() { return m_szRemoteIP.c_str(); }
	DWORD			GetRemotePort() { return m_dwRemotePort; }

	void			SetID(const TypeID& id) { m_dwID = id; }
	virtual TypeID	GetID() { return m_dwID; }

protected:
	// 打印该连接的信息
	virtual std::string ToString() = 0;

	// 心跳包消息号(由模块号+消息号拼接而成)
	virtual WORD		GetKeepAliveID() = 0;
	// 心跳回复包
	virtual	const obuf&	GetAnswerKeepAliveMsg() = 0;
	// 心跳包超时时间，单位：毫秒
	virtual	DWORD		GetKeepaliveTimeout() { return LISTENER_KEEPALIVE_TIMEOUT; };
	// 心跳包检测间隔，单位：毫秒
	virtual	DWORD		GetKeepaliveInterval() { return LISTENER_KEEPALIVE_INTERVAL; }

private:
	void OnSendData(DWORD dwDataLen) { if (m_pByteRecord)	m_pByteRecord->OnSendData(dwDataLen); }
	void OnRecvData(DWORD dwDataLen) { if (m_pByteRecord)	m_pByteRecord->OnRecvData(dwDataLen); }

protected:
	IConnection	*		m_pConnection;		// 服务器与网关之间的连接
	TypeID				m_dwID;				// 两种来源：1.连接建立的时候生成，自增的；2.外部传入的；
	std::string			m_szRemoteIP;		// 连进来的IP
	DWORD				m_dwRemotePort;		// 连进来的端口
	DWORD				m_dwLastKeepaliveTime;	// 上次收到心跳包的时间

	/*以下为引用别人的，无需释放*/
	TimerAxis&			m_TimerAxis;
	IUserList<TypeID>&	m_UserList;
	Processer<TYPE>&	m_Processer;
	IByteRecord*		m_pByteRecord;
};

template<class TYPE, typename TypeID>
bool SessionUser<TYPE, TypeID>::SendData(const char * pData, size_t dataLen)
{
	Assert(m_pConnection);
	if (!m_pConnection) { return false; }

	DWORD length = (DWORD)dataLen;
	Assert(length == dataLen);
	OnSendData(length);

	return m_pConnection->SendData(pData, length);
}

template<class TYPE, typename TypeID>
void SessionUser<TYPE, TypeID>::Disconnect()
{
	if (m_pConnection)
		m_pConnection->Disconnect();
}

template<class TYPE, typename TypeID>
void SessionUser<TYPE, TypeID>::Release()
{
	m_TimerAxis.KillTimer(ETimerEventID_CheckKeepalive, this);
	m_UserList.DelUser(this);
	if (m_pConnection)
	{
		m_pConnection->Release();
		m_pConnection = NULL;
	}
	delete this;
}

template<class TYPE, typename TypeID>
void SessionUser<TYPE, TypeID>::UnHandled(LPVOID pData, DWORD dwDataLen)
{
	SGameMsgHead* head = (SGameMsgHead*)pData;

	char szBuf[256];
	sprintf(szBuf, "UnKnown msg: SrcEndPoint=%d, DestEndPoint=%d, byKeyModule=%d, byKeyAction=%d !",
		head->SrcEndPoint, head->DestEndPoint, head->byKeyModule, head->byKeyAction);
	ErrorLn(szBuf);
}

////////////////////////////////IConnectionEventHandler//////////////////////////////////////////////////////
template<class TYPE, typename TypeID>
void SessionUser<TYPE, TypeID>::OnDisconnected(IConnection * conn, DWORD reason)
{
	TraceLn("Connection disconnected!" << ToString());
	Release();
}

template<class TYPE, typename TypeID>
void SessionUser<TYPE, TypeID>::OnRecv(IConnection * conn, LPVOID pData, DWORD dwDataLen)
{
	assert(conn == m_pConnection);
	if (dwDataLen < sizeof(SGameMsgHead))
	{
		WarningLn("Package length is too short.");
		return;
	}

	Assert(pData != NULL);
	SGameMsgHead* head = (SGameMsgHead*)pData;
	MsgKey msgKey(head->byKeyModule, head->byKeyAction);
	if (GetKeepAliveID() == msgKey.key)
	{
		m_dwLastKeepaliveTime = ::GetTickCount();

		const obuf& obufData = GetAnswerKeepAliveMsg();
		SendData(obufData.data(), obufData.size());
	}
	else
	{
		m_Processer.Process((TYPE*)(this), pData, dwDataLen);
	}
	
	OnRecvData(dwDataLen);
}

template<class TYPE, typename TypeID>
void SessionUser<TYPE, TypeID>::OnError(IConnection * conn, DWORD dwErrorCode)
{
	TraceLn("Connection occured an error!" << dwErrorCode << ToString());
	Release();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////TimerHandler//////////////////////////////////////////////////////
template<class TYPE, typename TypeID>
void SessionUser<TYPE, TypeID>::OnTimer(unsigned long dwTimerID)
{
	switch (dwTimerID)
	{
	case ETimerEventID_CheckKeepalive:
	{
		if (::GetTickCount() >= m_dwLastKeepaliveTime + GetKeepaliveTimeout())
		{
			Release();
		}
	}
	break;
	default:
		break;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////