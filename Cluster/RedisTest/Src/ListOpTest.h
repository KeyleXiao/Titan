#pragma once
#include "IRedisTest.h"


struct IListOp;

class ListOpTest : public IRedisTest
{
public:
	ListOpTest(Redis::IListOp* pOp);

	virtual const char* GetName() override;


	virtual bool Run() override;

private:
	Redis::IListOp*	m_pOp;
};