#include "stdafx.h"
#include "GateUser.h"
#include "ViewMsgDef_Server.h"


GateUser::GateUser(IConnection * conn, IUserList& pUserList, Processer<GateUser>& processer, TimerAxis& timerAxis, IByteRecord* pByteRecord)
	:SessionUser(conn, pUserList, processer, timerAxis, pByteRecord)
	, m_dwMaxCount(0)
	, m_dwCurrCount(0)
	, m_wPortForView(0)
{
}

std::string GateUser::ToString()
{
	// TODO
	std::string str;
	return str;
}

WORD GateUser::GetKeepAliveID()
{
	static MsgKey msgKey(MSG_MODULEID_VIEW, ENUM_MSG_VIEW_KEEPALIVE);
	return msgKey.key;
}

const obuf&	GateUser::GetAnswerKeepAliveMsg()
{
	static bool bFlag = true;
	static obuf obufData;
	if (bFlag)
	{
		const SGameMsgHead& header = gMsg.BuildHead_MG(ENUM_MSG_VIEW_ANSWER_KEEPALIVE);
		SMsgView_MG_Answer_KeepAlive msg;

		obufData.push_back(&header, sizeof(SGameMsgHead));
		obufData.push_back(&msg, sizeof(SMsgView_MG_Answer_KeepAlive));
		bFlag = false;
	}

	return obufData;
}
