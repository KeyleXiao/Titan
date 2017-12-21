#include "stdafx.h"
#include "GateMsgRegister.h"
#include "GM_HandshakeHandler.h"
#include "GM_RequestAddPlayerReplyHandler.h"
#include "GM_AuthDelHandler.h"


void GateMsgRegister::RegisterMsgs(Processer<GateUser>& processer)
{
	processer.Register(new GM_HandshakeHandler);
	processer.Register(new GM_RequestAddPlayerReplyHandler);
	processer.Register(new GM_AuthDelHandler);
}