#pragma once
#include "Handler.h"
#include "GameUser.h"
#include "ViewMsgDef_Server.h"


class SM_HandshakeHandler : public Handler<GameUser, SMsgView_SM_Handshake>
{
protected:
	virtual void Handle(GameUser* pServer, SGameMsgHead* head, SMsgView_SM_Handshake* pMsg)
	{
		// »Ø¸´ÎÕÊÖ
		SMsgView_MS_HandshakeResponse sendData;
		sendData.dwID = pServer->GetID();
		pServer->SendMsg(sendData);
	}
};