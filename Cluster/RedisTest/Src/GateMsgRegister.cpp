#include "stdafx.h"
#include "GateMsgRegister.h"
#include "GV_HandshakeResponseHandler.h"


void GateMsgRegister::RegisterMsgs(Processer<GateConnector>& processer)
{
	processer.Register(new GV_HandshakeResponseHandler);

}