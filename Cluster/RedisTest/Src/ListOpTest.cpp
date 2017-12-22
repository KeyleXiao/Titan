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

	// 清理
	long oldLength = 0;
	if (m_pValueOp->lLen(key, &oldLength))
	{
		string strTmp;
		for (size_t i = 0; i < oldLength; i++)
		{
			m_pValueOp->rPop(key, strTmp);
		}
	}



	const int length = 10;
	// 存
	int aArray[length] = {0,9,8,7,6,5,4,3,2,1};
	for (size_t i = 0; i < length; i++)
	{
		m_pValueOp->rPush(key, aArray[i]);
	}

	// 取
	vector<int> vecInt;
	m_pValueOp->lRange(key, -length, -1, &vecInt);

	// 验证
	for (size_t i = 0; i < length; i++)
	{
		if (aArray[i] != vecInt.at(i))
			return false;
	}

	return true;
}