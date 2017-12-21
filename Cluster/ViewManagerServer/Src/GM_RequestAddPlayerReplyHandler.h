#pragma once
#include "Handler.h"
#include "ViewMsgDef_Server.h"

class GateUser;

class GM_RequestAddPlayerReplyHandler : public Handler<GateUser, SMsgView_GM_RequestAddPlayerReply>
{
protected:
	virtual void Handle(GateUser* pSession, SGameMsgHead* head, SMsgView_GM_RequestAddPlayerReply* pMsg)
	{
		// TODO
	}
};