#pragma once
#include "net.h"


template<typename TypeID = DWORD>
class ISessionUser : public IConnectionEventHandler
{
public:
	// 连接建立时生成的自增ID
	virtual TypeID	GetID() = 0;
};