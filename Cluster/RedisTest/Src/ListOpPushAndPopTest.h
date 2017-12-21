#pragma once
#include "IRedisTest.h"


struct IListOp;

class ListOpPushAndPopTest : public IRedisTest
{
public:
	ListOpPushAndPopTest(Redis::IListOp* pValueOp);

	virtual const char* GetName() override;


	virtual bool Run() override;

private:
	Redis::IListOp*	m_pValueOp;
};