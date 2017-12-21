#pragma once
#include "Processer.h"

class GateUser;

class GateMsgRegister
{
public:
	static void RegisterMsgs(Processer<GateUser>& processer);
};