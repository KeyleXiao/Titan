#pragma once
#include "net.h"


template<typename TypeID = DWORD>
class ISessionUser : public IConnectionEventHandler
{
public:
	// ���ӽ���ʱ���ɵ�����ID
	virtual TypeID	GetID() = 0;
};