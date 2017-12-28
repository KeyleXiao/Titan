#include "stdafx.h"
#include "ViewUser.h"
#include "ViewMsgDef_Server.h"


ViewUser::ViewUser(IConnection * conn, IUserList<ViewID>& pUserList, Processer<ViewUser>& processer, TimerAxis& timerAxis, IByteRecord* pByteRecord)
	:SessionUser(conn, pUserList, processer, timerAxis, pByteRecord)
	, m_dwMaxCount(0)
	, m_dwCurrCount(0)
{
}

std::string ViewUser::ToString()
{
	// TODO
	std::string str;
	return str;
}

WORD ViewUser::GetKeepAliveID()
{
	static MsgKey msgKey(MSG_MODULEID_VIEW, ENUM_MSG_VIEW_KEEPALIVE);
	return msgKey.key;
}

const obuf&	ViewUser::GetAnswerKeepAliveMsg()
{
	static bool bFlag = true;
	static obuf obufData;
	if (bFlag)
	{
		const SGameMsgHead& header = gMsg.BuildHead_MV(ENUM_MSG_VIEW_ANSWER_KEEPALIVE);
		SMsgView_MV_Answer_KeepAlive msg;

		obufData.push_back(&header, sizeof(SGameMsgHead));
		obufData.push_back(&msg, sizeof(SMsgView_MV_Answer_KeepAlive));
		bFlag = false;
	}

	return obufData;
}

