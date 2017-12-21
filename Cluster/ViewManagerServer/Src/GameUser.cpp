#include "stdafx.h"
#include "GameUser.h"
#include "ViewMsgDef_Server.h"


GameUser::GameUser(IConnection * conn, IUserList& pUserList, Processer<GameUser>& processer, TimerAxis& timerAxis, IByteRecord* pByteRecord)
	:SessionUser(conn, pUserList, processer, timerAxis, pByteRecord)
{
}

std::string GameUser::ToString()
{
	// TODO
	std::string str;
	return str;
}

WORD GameUser::GetKeepAliveID()
{
	static MsgKey msgKey(MSG_MODULEID_VIEW, ENUM_MSG_VIEW_KEEPALIVE);
	return msgKey.key;
}

const obuf&	GameUser::GetAnswerKeepAliveMsg()
{
	static bool bFlag = true;
	static obuf obufData;
	if (bFlag)
	{
		const SGameMsgHead& header = gMsg.BuildHead_MS(ENUM_MSG_VIEW_ANSWER_KEEPALIVE);
		SMsgView_MS_Answer_KeepAlive msg;

		obufData.push_back(&header, sizeof(SGameMsgHead));
		obufData.push_back(&msg, sizeof(SMsgView_MS_Answer_KeepAlive));
		bFlag = false;
	}

	return obufData;
}