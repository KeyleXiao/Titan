#include "stdafx.h"
#include "MngConnector.h"
#include "ViewDef_.h"
#include "MsgFactory.h"
#include "Setting.h"
#include "HandshakeResponseHandler.h"


MngConnector::MngConnector(Processer<MngConnector>& processer, ByteRecord& br, TimerAxis& timerAxis) :
	ConnectorWithProcess(processer, br, timerAxis)
	, m_dwID(0)
	, m_dwServerID(0)
{}

bool MngConnector::SendHandshake()
{
	if (m_bIsConnected == false || m_pConnection == NULL)
	{
		WarningLn(_GT("��δ���ӣ��Ժ���������Ϣ��"));
		return false;
	}

	// ��ս������������
	SMsgView_VM_Handshake sendData;
	sendData.dwServerID = m_dwServerID;
	sendData.dwMaxCount = gSetting.m_dwMaxConnection;	// �������
	sendData.dwCurrCount = 0;	// TODO �Ժ�ֵ

	this->SendMsg(sendData);

	TraceLn(_GT("����������Ϣ! ") << a2utf8(ToString().data()));

	return true;
}


const obuf& MngConnector::GetKeepAliveMsg()
{
	// �������
	static bool bFlag = true;
	static obuf obufData;
	if (bFlag)
	{
		// �����ڵ��߳�
		const SGameMsgHead& header = gMsg.BuildHead_VM(ENUM_MSG_VIEW_KEEPALIVE);

		SMsgView_VM_KeepAlive msg;
		
		obufData.push_back(&header, sizeof(SGameMsgHead));
		obufData.push_back(&msg, sizeof(SMsgView_VM_KeepAlive));
		bFlag = false;
	}

	return obufData;
}

//ȡ��Ŀ��ڵ������
const char* MngConnector::GetDestPointName()
{
	static const char* szDestPointName = _GT("��ս�����");
	return szDestPointName;
}

WORD MngConnector::GetAnswerKeepAliveID()
{
	static MsgKey msgKey(MSG_MODULEID_VIEW, ENUM_MSG_VIEW_ANSWER_KEEPALIVE);
	return msgKey.key;
}

void MngConnector::OnDisconnected(IConnection * conn, DWORD reason)
{
	Connector::OnDisconnected(conn, reason);
	ResetID();
}

void MngConnector::OnError(IConnection * conn, DWORD dwErrorCode)
{
	Connector::OnError(conn, dwErrorCode);
	ResetID();
}
