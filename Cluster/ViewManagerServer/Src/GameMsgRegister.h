#pragma once
#include "Processer.h"

class GameUser;

class GameMsgRegister
{
public:
	static void RegisterMsgs(Processer<GameUser>& processer);
};