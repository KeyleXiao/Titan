#pragma once
#include "Processer.h"

class MngConnector;

class MngMsgRegister
{
public:
	static void RegisterMsgs(Processer<MngConnector>& processer);

};