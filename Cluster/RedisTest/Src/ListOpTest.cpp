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

	// ����
	long oldLength = 0;
	if (m_pOp->lLen(key, &oldLength))
	{
		string strTmp;
		for (size_t i = 0; i < oldLength; i++)
		{
			m_pOp->rPop(key, strTmp);
		}
	}


	// 1.���� lRange
	const int length = 10;
	// ��
	int aArray[length] = { 0,9,8,7,6,5,4,3,2,1 };
	for (size_t i = 0; i < length; i++)
	{
		m_pOp->rPush(key, aArray[i]);
	}

	// ȡ
	vector<int> vecInt;
	m_pOp->lRange(key, -length, -1, &vecInt);

	// ��֤
	for (size_t i = 0; i < length; i++)
	{
		if (aArray[i] != vecInt.at(i))
			return false;
	}

	// 2.���� lIndex
	long nIndex = 2;
	int n2 = 0;
	m_pOp->lIndex(key, nIndex, n2);
	if (n2 != aArray[nIndex])
		return false;

	return true;
}