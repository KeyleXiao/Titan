#pragma once
#include "IRedisTest.h"


struct IValueOp;

class ValueOpStringTest : public IRedisTest
{
public:
	ValueOpStringTest(Redis::IValueOp* pValueOp);
	
	virtual	const char*	GetName() { return "ValueOpStringTest"; };

	virtual	bool		Run();
protected:
private:
	Redis::IValueOp*	m_pValueOp;
};