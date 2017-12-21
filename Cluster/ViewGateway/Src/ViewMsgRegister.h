#pragma once
#include "Processer.h"


class ViewSession;

class ViewMsgRegister
{
public:
	static void RegisterMsgs(Processer<ViewSession>& processer);
};