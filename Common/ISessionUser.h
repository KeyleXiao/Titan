#pragma once
#include "net.h"

class ISessionUser : public IConnectionEventHandler
{
public:
	// ���ӽ���ʱ���ɵ�����ID
	virtual DWORD	GetID() = 0;
};