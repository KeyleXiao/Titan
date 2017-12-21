#include "stdafx.h"
#include "HandshakeHandler.h"
#include "ViewSession.h"
#include "ViewMsgDef_Server.h"
#include "ViewContainer.h"
#include "MsgFactory.h"
#include "Setting.h"


void HandshakeHandler::Handle(ViewSession* pSession, SGameMsgHead* head, SMsgView_VG_Handshake* pMsg)
{
	if (!gViewContainer.OnSessionRcvID(pSession, pMsg->dwID))
	{
		TraceLn(_GT("收到ViewServer握手信息，但本服ViewServer连接数已满! ") << a2utf8(pSession->ToString().data()));
		pSession->Disconnect();
		return;
	}

	TraceLn(_GT("收到ViewServer握手信息! ") << a2utf8(pSession->ToString().data()));

	// 回复握手消息
	const SGameMsgHead& header = gMsg.BuildHead_GV(ENUM_MSG_VIEW_HANDSHAKE_RESPONSE);
	SMsgView_GV_HandshakeResponse sendData;
	sendData.dwID = gSetting.GetID();
	TSendMsg(pSession, header, sendData);
}