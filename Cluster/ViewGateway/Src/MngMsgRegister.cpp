#include "stdafx.h"
#include "MngMsgRegister.h"
#include "HandshakeResponseHandler.h"
#include "MG_RequestAddPlayerHandler.h"


void MngMsgRegister::RegisterMsgs(Processer<MngConnector>& processer)
{
	processer.Register(new HandshakeResponseHandler);
	processer.Register(new MG_RequestAddPlayerHandler);
}