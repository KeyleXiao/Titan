#pragma once
#include "Handler.h"
#include "GameUser.h"
#include "ViewMsgDef_Server.h"


class SM_HandshakeHandler : public Handler<GameUser, SMsgView_SM_Handshake>
{
protected:
	virtual void Handle(GameUser* pSession, SGameMsgHead* head, SMsgView_SM_Handshake* pMsg)
	{
		// »Ø¸´ÎÕÊÖ
		const SGameMsgHead& header = gMsg.BuildHead_MS(ENUM_MSG_VIEW_HANDSHAKE_RESPONSE);
		SMsgView_MS_HandshakeResponse sendData;
		sendData.dwID = pSession->GetID();
		TSendMsg(pSession, header, sendData);
	}
};