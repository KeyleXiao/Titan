#include "stdafx.h"
#include "ListOpTest.h"


ListOpTest::ListOpTest(Redis::IListOp* pValueOp)
	: m_pValueOp(pValueOp)
{}

const char* ListOpTest::GetName()
{
	return "ListOpTest";
}

bool ListOpTest::Run()
{
	string key("keyListOpTest");


	return true;
}