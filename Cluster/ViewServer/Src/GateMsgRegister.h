#pragma once
#include "Processer.h"


class GateConnector;

class GateMsgRegister
{
public:
	static void RegisterMsgs(Processer<GateConnector>& processer);
};