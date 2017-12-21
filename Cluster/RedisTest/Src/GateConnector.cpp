#include "StdAfx.h"
#include "GateConnector.h"
#include "MsgFactory.h"
#include "ViewMsgDef_Server.h"
#include "Setting.h"


GateConnector::GateConnector(Processer<GateConnector>& processer, ByteRecord& br, TimerAxis& timerAxis) :
	ConnectorWithProcess(processer, br, timerAxis)
	, m_wID(0)
{}


GateConnector::~GateConnector()
{}

bool GateConnector::SendHandshake()
{
	if (m_bIsConnected == false || m_pConnection == NULL)
	{
		WarningLn(_GT("��δ���ӣ��Ժ���������Ϣ��"));
		return false;
	}

	// ����
	const SGameMsgHead& header = gMsg.BuildHead_VG(ENUM_MSG_VIEW_HANDSHAKE);
	SMsgView_VG_Handshake sendData;
	sendData.dwServerID = gSetting.getServerID();
	sendData.dwID = gSetting.GetID();

	TSendMsg(this, header, sendData);

	TraceLn(_GT("����������Ϣ! ") << a2utf8(ToString().data()));

	return true;
}


const obuf& GateConnector::GetKeepAliveMsg()
{
	// �������
	static bool bFlag = true;
	static obuf obufData;
	if (bFlag)
	{
		// �����ڵ��߳�
		const SGameMsgHead& header = gMsg.BuildHead_VG(ENUM_MSG_VIEW_KEEPALIVE);

		SMsgView_VG_KeepAlive msg;

		obufData.push_back(&header, sizeof(SGameMsgHead));
		obufData.push_back(&msg, sizeof(SMsgView_VG_KeepAlive));
		bFlag = false;
	}

	return obufData;
}

//ȡ��Ŀ��ڵ������
const char* GateConnector::GetDestPointName()
{
	static const char* szDestPointName = _GT("��ս����");
	return szDestPointName;
}

WORD GateConnector::GetAnswerKeepAliveID()
{
	static MsgKey msgKey(MSG_MODULEID_VIEW, ENUM_MSG_VIEW_ANSWER_KEEPALIVE);
	return msgKey.key;
}