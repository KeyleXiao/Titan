#include "StdAfx.h"
#include "ViewSession.h"
#include "ViewMsgDef_Server.h"
#include "MsgFactory.h"


ViewSession::ViewSession(IConnection * conn, IUserList& pUserList, Processer<ViewSession>& processer, TimerAxis& timerAxis, IByteRecord* pByteRecord)
	:SessionUser(conn, pUserList, processer, timerAxis, pByteRecord)
{}


ViewSession::~ViewSession()
{}

std::string ViewSession::ToString()
{
	// TODO
	std::string str;
	return str;
}

void ViewSession::UnHandled(LPVOID pData, DWORD dwDataLen)
{
	SGameMsgHead* head = (SGameMsgHead*)pData;
	uchar* data = (uchar*)pData + sizeof(SGameMsgHead);
	size_t new_len = (size_t)(dwDataLen - sizeof(SGameMsgHead));

	if (head->DestEndPoint != MSG_ENDPOINT_CLIENT
		|| new_len < sizeof(ViewID))
	{
		SessionUser<ViewSession>::UnHandled(pData, dwDataLen);
		return;
	}

	// TODO
}

WORD ViewSession::GetKeepAliveID()
{
	static MsgKey msgKey(MSG_MODULEID_VIEW, ENUM_MSG_VIEW_KEEPALIVE);
	return msgKey.key;
}

const obuf&	ViewSession::GetAnswerKeepAliveMsg()
{
	static bool bFlag = true;
	static obuf obufData;
	if (bFlag)
	{
		const SGameMsgHead& header = gMsg.BuildHead_GV(ENUM_MSG_VIEW_ANSWER_KEEPALIVE);
		SMsgView_GV_Answer_KeepAlive msg;

		obufData.push_back(&header, sizeof(SGameMsgHead));
		obufData.push_back(&msg, sizeof(SMsgView_GV_Answer_KeepAlive));
		bFlag = false;
	}

	return obufData;
}