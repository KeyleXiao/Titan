#pragma once
#include "Processer.h"


class ClientSession;

class ClientMsgRegister
{
public:
	static void RegisterMsgs(Processer<ClientSession>& processer);
};