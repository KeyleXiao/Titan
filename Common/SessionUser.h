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
#include "IMsg.h"


// ���ڼ�����������
template<typename TYPE, typename TypeID = DWORD>
class SessionUser : public ISessionUser<TypeID>, public TimerHandler
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

	// �ر����ӣ��رճɹ���ص�OnDisconnected
	void			Disconnect();

	void			Release();

	// ����δע�����Ϣ������Ҫת������Ϣ�������б�Ҫ����ӭoverride
	virtual	void	UnHandled(LPVOID pData, DWORD dwDataLen);

	// ������Ϣ���򻯰�
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
	TypeID				m_dwID;				// ������Դ��1.���ӽ�����ʱ�����ɣ������ģ�2.�ⲿ����ģ�
	std::string			m_szRemoteIP;		// ��������IP
	DWORD				m_dwRemotePort;		// �������Ķ˿�
	DWORD				m_dwLastKeepaliveTime;	// �ϴ��յ���������ʱ��

	/*����Ϊ���ñ��˵ģ������ͷ�*/
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