#pragma once
#include "Handler.h"
#include "ViewMsgDef_Server.h"


class GateConnector;
struct SMsgView_GV_HandshakeResponse;

class GV_HandshakeResponseHandler : public Handler<GateConnector, SMsgView_GV_HandshakeResponse>
{
protected:
	virtual void Handle(GateConnector* pSession, SGameMsgHead* head, SMsgView_GV_HandshakeResponse* pMsg);
};