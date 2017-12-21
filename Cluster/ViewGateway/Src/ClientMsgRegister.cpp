#include "stdafx.h"
#include "ClientMsgRegister.h"
#include "CG_HandshakeHandler.h"


void ClientMsgRegister::RegisterMsgs(Processer<ClientSession>& processer)
{
	processer.Register(new CG_HandshakeHandler);

}