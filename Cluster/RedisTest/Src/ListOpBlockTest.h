#pragma once
#include "IRedisTest.h"


struct IListOp;

class ListOpBlockTest : public IRedisTest
{
public:
	ListOpBlockTest(Redis::IListOp* pValueOp);

	virtual const char* GetName() override;


	virtual bool Run() override;

private:
	Redis::IListOp*	m_pValueOp;
};