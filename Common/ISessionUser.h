#pragma once
#include "net.h"

class ISessionUser : public IConnectionEventHandler
{
public:
	// 连接建立时生成的自增ID
	virtual DWORD	GetID() = 0;
};