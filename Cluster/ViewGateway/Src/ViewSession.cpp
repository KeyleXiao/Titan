#include "StdAfx.h"
#include "ViewSession.h"
#include "ViewMsgDef_Server.h"
#include "MsgFactory.h"
#include "ClientContainer.h"


ViewSession::ViewSession(IConnection * conn, IUserList<ViewID>& pUserList, Processer<ViewSession>& processer, TimerAxis& timerAxis, IByteRecord* pByteRecord)
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
		|| new_len < sizeof(size_t)+sizeof(PlayerID))
	{
		__super::UnHandled(pData, dwDataLen);
		return;
	}

	ibuffer ibufData(data, new_len);
	size_t size;
	ibufData >> size;
	size_t length = size / sizeof(PlayerID);
	if (length<=0 || length >= USHRT_MAX)	// 我这里最多也就有USHRT_MAX个玩家
	{
		ErrorLn(GetID() << _GT("号ViewServer发来的广播消息，玩家ID数组size不合法，size=") << size);
		return;
	}

	// 要广播的玩家ID数组
	VecPlayerID vec(length);
	for (size_t i = 0; i < length; i++)
	{
		ibufData >> vec.at(i);
	}
	obuf obufData;
	obufData << *head;
	obufData.push_back(ibufData.current(), ibufData.space());// 这里需要验证 TODO
	gClientContainer.BroadCastData(obufData, vec);
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