#include "stdafx.h"
#include "MngMsgRegister.h"
#include "HandshakeResponseHandler.h"
#include "GatesInfoHandler.h"


void MngMsgRegister::RegisterMsgs(Processer<MngConnector>& processer)
{
	processer.Register(new HandshakeResponseHandler);
	processer.Register(new GatesInfoHandler);
	
}