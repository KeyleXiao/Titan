/*******************************************************************
** �ļ���:	SessionUser.h
** ��  Ȩ:	(C) ���ڱ�������ɷ����޹�˾
** ������:	baoliang.shen
** ��  ��:	2017/11/28
** ��  ��:	1.0
** ��  ��:	�����������ӻ���
** Ӧ  ��:  ����ﵽָ��ʱ��δ�յ������������Զ��Ͽ�����
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/
#pragma once
#include "Processer.h"
#include "ISessionUser.h"
#include "IByteRecord.h"
#include "IUserList.h"


// ���ڼ�����������
template<class TYPE>
class SessionUser : public ISessionUser, public TimerHandler
{
	// �����Ѿ�ʹ����1��TimerID��������ʹ��Timerʱ��ע�ⲻҪ�ظ�
	enum
	{
		ETimerEventID_CheckKeepalive = 0,
	};
	static const DWORD LISTENER_KEEPALIVE_TIMEOUT = 300 * 1000;	// Ĭ��������ʱʱ��
	static const DWORD LISTENER_KEEPALIVE_INTERVAL = 30 * 1000;	// Ĭ�������������ʱ��

public:
	////////////////////////////////IConnectionEventHandler//////////////////////////////////////////////////////
	//��Ϊ�ǽӵ����Ӻ󣬲Ŵ������࣬���Բ�����ô˷���
	virtual void OnAccept(IConnection * pIncomingConn, IConnectionEventHandler ** ppHandler) { assert(false); }
	//��Ϊ�����ڼ����ģ����Բ�������ȥ���ӣ���Ȼ������ô˷���
	virtual void OnConnected(IConnection * conn) { assert(false); }

	virtual void OnDisconnected(IConnection * conn, DWORD reason);

	virtual void OnRecv(IConnection * conn, LPVOID pData, DWORD dwDataLen);

	virtual void OnError(IConnection * conn, DWORD dwErrorCode);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////TimerHandler//////////////////////////////////////////////////////
	virtual void OnTimer(unsigned long dwTimerID);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
	SessionUser(IConnection * conn, IUserList& pUserList, Processer<TYPE>& processer, TimerAxis& timerAxis, IByteRecord* pByteRecord = nullptr)
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

	// �ر����ӣ��رճɹ���ص�OnDisconnected
	void			Disconnect();

	void			Release();
	bool			SendData(const char * pData, size_t dwDataLen);

	const char*		GetRemoteIP() { return m_szRemoteIP.c_str(); }
	DWORD			GetRemotePort() { return m_dwRemotePort; }

	void			SetID(const DWORD& id) { m_dwID = id; }
	virtual DWORD	GetID() { return m_dwID; }

protected:
	// ��ӡ�����ӵ���Ϣ
	virtual std::string ToString() = 0;

	// ��������Ϣ��(��ģ���+��Ϣ��ƴ�Ӷ���)
	virtual WORD		GetKeepAliveID() = 0;
	// �����ظ���
	virtual	const obuf&	GetAnswerKeepAliveMsg() = 0;
	// ��������ʱʱ�䣬��λ������
	virtual	DWORD		GetKeepaliveTimeout() { return LISTENER_KEEPALIVE_TIMEOUT; };
	// ���������������λ������
	virtual	DWORD		GetKeepaliveInterval() { return LISTENER_KEEPALIVE_INTERVAL; }

private:
	void OnSendData(DWORD dwDataLen) { if (m_pByteRecord)	m_pByteRecord->OnSendData(dwDataLen); }
	void OnRecvData(DWORD dwDataLen) { if (m_pByteRecord)	m_pByteRecord->OnRecvData(dwDataLen); }

protected:
	IConnection	*		m_pConnection;		// ������������֮�������
	DWORD				m_dwID;				// ������Դ��1.���ӽ�����ʱ�����ɣ������ģ�2.�ⲿ����ģ�
	std::string			m_szRemoteIP;		// ��������IP
	DWORD				m_dwRemotePort;		// �������Ķ˿�
	DWORD				m_dwLastKeepaliveTime;	// �ϴ��յ���������ʱ��

	/*����Ϊ���ñ��˵ģ������ͷ�*/
	TimerAxis&			m_TimerAxis;
	IUserList&			m_UserList;
	Processer<TYPE>&	m_Processer;
	IByteRecord*		m_pByteRecord;
};

template<class TYPE>
bool SessionUser<TYPE>::SendData(const char * pData, size_t dwDataLen)
{
	Assert(m_pConnection);
	if (!m_pConnection) { return false; }

	DWORD length = (DWORD)dwDataLen;
	Assert(length == dwDataLen);
	OnSendData(length);

	return m_pConnection->SendData(pData, length);
}

template<class TYPE>
void SessionUser<TYPE>::Disconnect()
{
	if (m_pConnection)
		m_pConnection->Disconnect();
}

template<class TYPE>
void SessionUser<TYPE>::Release()
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

////////////////////////////////IConnectionEventHandler//////////////////////////////////////////////////////
template<class TYPE>
void SessionUser<TYPE>::OnDisconnected(IConnection * conn, DWORD reason)
{
	TraceLn("Connection disconnected!" << ToString());
	Release();
}

template<class TYPE>
void SessionUser<TYPE>::OnRecv(IConnection * conn, LPVOID pData, DWORD dwDataLen)
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

template<class TYPE>
void SessionUser<TYPE>::OnError(IConnection * conn, DWORD dwErrorCode)
{
	TraceLn("Connection occured an error!" << dwErrorCode << ToString());
	Release();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////TimerHandler//////////////////////////////////////////////////////
template<class TYPE>
void SessionUser<TYPE>::OnTimer(unsigned long dwTimerID)
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