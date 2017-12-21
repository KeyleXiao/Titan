#include "stdafx.h"
#include "ViewMsgRegister.h"
#include "HandshakeHandler.h"
#include "ViewMsgDef_Server.h"


void ViewMsgRegister::RegisterMsgs(Processer<ViewSession>& processer)
{
	processer.Register(new HandshakeHandler);
}