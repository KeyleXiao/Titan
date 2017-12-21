#include "stdafx.h"
#include "GameMsgRegister.h"
#include "SM_HandshakeHandler.h"
#include "SM_RequestReplayHandler.h"


void GameMsgRegister::RegisterMsgs(Processer<GameUser>& processer)
{
	processer.Register(new SM_HandshakeHandler);
	processer.Register(new SM_RequestReplayHandler);
	
}