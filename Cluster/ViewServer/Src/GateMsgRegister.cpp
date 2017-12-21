#include "stdafx.h"
#include "GateMsgRegister.h"
#include "GV_HandshakeResponseHandler.h"
#include "GV_CRequestReplayHandler.h"


void GateMsgRegister::RegisterMsgs(Processer<GateConnector>& processer)
{
	processer.Register(new GV_HandshakeResponseHandler);
	processer.Register(new GV_CRequestReplayHandler);
}