#pragma once
#include "IRedisTest.h"


struct IListOp;

class ListOpTest : public IRedisTest
{
public:
	ListOpTest(Redis::IListOp* pValueOp);

	virtual const char* GetName() override;


	virtual bool Run() override;

private:
	Redis::IListOp*	m_pValueOp;
};