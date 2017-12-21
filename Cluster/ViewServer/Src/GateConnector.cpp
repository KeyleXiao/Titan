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
		WarningLn(_GT("还未连接，稍后发送握手信息！"));
		return false;
	}

	// 握手
	const SGameMsgHead& header = gMsg.BuildHead_VG(ENUM_MSG_VIEW_HANDSHAKE);
	SMsgView_VG_Handshake sendData;
	sendData.dwServerID = gSetting.getServerID();
	sendData.dwID = gSetting.GetID();

	TSendMsg(this, header, sendData);

	TraceLn(_GT("发送握手信息! ") << a2utf8(ToString().data()));

	return true;
}


const obuf& GateConnector::GetKeepAliveMsg()
{
	// 心跳检查
	static bool bFlag = true;
	static obuf obufData;
	if (bFlag)
	{
		// 适用于单线程
		const SGameMsgHead& header = gMsg.BuildHead_VG(ENUM_MSG_VIEW_KEEPALIVE);

		SMsgView_VG_KeepAlive msg;

		obufData.push_back(&header, sizeof(SGameMsgHead));
		obufData.push_back(&msg, sizeof(SMsgView_VG_KeepAlive));
		bFlag = false;
	}

	return obufData;
}

//取得目标节点的名称
const char* GateConnector::GetDestPointName()
{
	static const char* szDestPointName = _GT("观战网关");
	return szDestPointName;
}

WORD GateConnector::GetAnswerKeepAliveID()
{
	static MsgKey msgKey(MSG_MODULEID_VIEW, ENUM_MSG_VIEW_ANSWER_KEEPALIVE);
	return msgKey.key;
}