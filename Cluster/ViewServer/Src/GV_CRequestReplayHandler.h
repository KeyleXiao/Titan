#pragma once
#include "Handler.h"
#include "ViewMsgDef_Server.h"
#include "GateConnector.h"


class GV_CRequestReplayHandler : public Handler<GateConnector, SMsgView_GV_CRequestReplay>
{
protected:
	virtual void Handle(GateConnector* pSession, SGameMsgHead* head, SMsgView_GV_CRequestReplay* pMsg)
	{

	}
};