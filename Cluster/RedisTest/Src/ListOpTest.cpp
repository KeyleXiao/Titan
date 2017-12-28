#include "stdafx.h"
#include "ListOpTest.h"


ListOpTest::ListOpTest(Redis::IListOp* pOp)
	: m_pOp(pOp)
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
	if (m_pOp->lLen(key, &oldLength))
	{
		string strTmp;
		for (size_t i = 0; i < oldLength; i++)
		{
			m_pOp->rPop(key, strTmp);
		}
	}


	// 1.测试 lRange
	const int length = 10;
	// 存
	int aArray[length] = { 0,9,8,7,6,5,4,3,2,1 };
	for (size_t i = 0; i < length; i++)
	{
		m_pOp->rPush(key, aArray[i]);
	}

	// 取
	vector<int> vecInt;
	m_pOp->lRange(key, -length, -1, &vecInt);

	// 验证
	for (size_t i = 0; i < length; i++)
	{
		if (aArray[i] != vecInt.at(i))
			return false;
	}

	// 2.测试 lIndex
	long nIndex = 2;
	int n2 = 0;
	m_pOp->lIndex(key, nIndex, n2);
	if (n2 != aArray[nIndex])
		return false;

	return true;
}