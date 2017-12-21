#include "stdafx.h"
#include "ListOpPushAndPopTest.h"
#include "IListOp.h"
#include "TestStruct.h"


ListOpPushAndPopTest::ListOpPushAndPopTest(Redis::IListOp* pValueOp)
	: m_pValueOp(pValueOp)
{}

const char* ListOpPushAndPopTest::GetName()
{
	return "ListOpPushAndPopTest";
}


bool ListOpPushAndPopTest::Run()
{
	string key("keyListOpPushAndPopTest");

	long oldLength = 0;
	if (m_pValueOp->lLen(key, &oldLength))
	{
		// œ»«Â¿Ì
		string strTmp;
		for (size_t i = 0; i < oldLength; i++)
		{
			m_pValueOp->rPop(key, strTmp);
		}
	}

	// ø™ º≤‚ ‘
	bool bIn1 = true;
	BYTE byIn1 = 101;
	string strIn1 = "Value1";
	string strIn2 = "Value2";
	string strIn3 = "Value3";
	TestStruct sTestStruct;
	FixStruct fsIn = MokeFixStruct();

	// 1.1
	// ◊Û±ﬂ¥Ê
	m_pValueOp->lPush(key, bIn1);
	m_pValueOp->lPush(key, byIn1);
	m_pValueOp->lPush(key, strIn1);
	m_pValueOp->lPush(key, strIn2);
	m_pValueOp->lPush(key, strIn3);
	m_pValueOp->lPush(key, sTestStruct);
	m_pValueOp->lPush(key, fsIn);

	// ”“±ﬂ»°
	{
		bool bOut1 = false;
		BYTE byOut1 = 0;
		string strOut1;
		string strOut2;
		string strOut3;
		TestStruct outTS = MokeEmptyTestStruct();
		FixStruct fsOut;

		m_pValueOp->rPop(key, bOut1);
		m_pValueOp->rPop(key, byOut1);
		m_pValueOp->rPop(key, strOut1);
		m_pValueOp->rPop(key, strOut2);
		m_pValueOp->rPop(key, strOut3);
		m_pValueOp->rPop(key, outTS);
		m_pValueOp->rPop(key, fsOut);

		if (bOut1 != bIn1
			|| byOut1 != byIn1
			|| strIn1.compare(strOut1) || strIn2.compare(strOut2) || strIn3.compare(strOut3)
			|| !(sTestStruct == outTS)
			|| !(fsOut == fsIn))
		{
			//ErrorLn(" fail!!!!!!!!!!!!!!!!!!!");
			return false;
		}
	}

	// 1.2
	// ”“±ﬂ¥Ê
	m_pValueOp->rPush(key, bIn1);
	m_pValueOp->rPush(key, byIn1);
	m_pValueOp->rPush(key, strIn1);
	m_pValueOp->rPush(key, strIn2);
	m_pValueOp->rPush(key, strIn3);
	m_pValueOp->rPush(key, sTestStruct);
	m_pValueOp->rPush(key, fsIn);

	// ◊Û±ﬂ»°
	{
		bool bOut1 = false;
		BYTE byOut1 = 0;
		string strOut1;
		string strOut2;
		string strOut3;
		TestStruct outTS = MokeEmptyTestStruct();
		FixStruct fsOut;

		m_pValueOp->lPop(key, bOut1);
		m_pValueOp->lPop(key, byOut1);
		m_pValueOp->lPop(key, strOut1);
		m_pValueOp->lPop(key, strOut2);
		m_pValueOp->lPop(key, strOut3);
		m_pValueOp->lPop(key, outTS);
		m_pValueOp->lPop(key, fsOut);

		if (bOut1 != bIn1
			|| byOut1 != byIn1
			|| strIn1.compare(strOut1) || strIn2.compare(strOut2) || strIn3.compare(strOut3)
			|| !(sTestStruct == outTS)
			|| !(fsOut == fsIn))
		{
			//ErrorLn(" fail!!!!!!!!!!!!!!!!!!!");
			return false;
		}
	}

	// 1.3
	// ◊Û±ﬂ¥Ê
	m_pValueOp->lPush(key, bIn1);
	m_pValueOp->lPush(key, byIn1);
	m_pValueOp->lPush(key, strIn1);
	m_pValueOp->lPush(key, strIn2);
	m_pValueOp->lPush(key, strIn3);
	m_pValueOp->lPush(key, sTestStruct);
	m_pValueOp->lPush(key, fsIn);

	// ◊Û±ﬂ»°
	{
		bool bOut1 = false;
		BYTE byOut1 = 0;
		string strOut1;
		string strOut2;
		string strOut3;
		TestStruct outTS = MokeEmptyTestStruct();
		FixStruct fsOut;

		m_pValueOp->lPop(key, fsOut);
		m_pValueOp->lPop(key, outTS);
		m_pValueOp->lPop(key, strOut3);
		m_pValueOp->lPop(key, strOut2);
		m_pValueOp->lPop(key, strOut1);
		m_pValueOp->lPop(key, byOut1);
		m_pValueOp->lPop(key, bOut1);

		if (bOut1 != bIn1
			|| byOut1 != byIn1
			|| strIn1.compare(strOut1) || strIn2.compare(strOut2) || strIn3.compare(strOut3)
			|| !(sTestStruct == outTS)
			|| !(fsOut == fsIn))
		{
			//ErrorLn(" fail!!!!!!!!!!!!!!!!!!!");
			return false;
		}
	}

	// 1.4
	// ”“±ﬂ¥Ê
	m_pValueOp->rPush(key, bIn1);
	m_pValueOp->rPush(key, byIn1);
	m_pValueOp->rPush(key, strIn1);
	m_pValueOp->rPush(key, strIn2);
	m_pValueOp->rPush(key, strIn3);
	m_pValueOp->rPush(key, sTestStruct);
	m_pValueOp->rPush(key, fsIn);

	// ”“±ﬂ»°
	{
		bool bOut1 = false;
		BYTE byOut1 = 0;
		string strOut1;
		string strOut2;
		string strOut3;
		TestStruct outTS = MokeEmptyTestStruct();
		FixStruct fsOut;

		m_pValueOp->rPop(key, fsOut);
		m_pValueOp->rPop(key, outTS);
		m_pValueOp->rPop(key, strOut3);
		m_pValueOp->rPop(key, strOut2);
		m_pValueOp->rPop(key, strOut1);
		m_pValueOp->rPop(key, byOut1);
		m_pValueOp->rPop(key, bOut1);

		if (bOut1 != bIn1
			|| byOut1 != byIn1
			|| strIn1.compare(strOut1) || strIn2.compare(strOut2) || strIn3.compare(strOut3)
			|| !(sTestStruct == outTS)
			|| !(fsOut==fsIn))
		{
			//ErrorLn(" fail!!!!!!!!!!!!!!!!!!!");
			return false;
		}
	}

	return true;
}
