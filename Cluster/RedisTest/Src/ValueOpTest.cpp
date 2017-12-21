#include "stdafx.h"
#include "ValueOpTest.h"
#include "TestStruct.h"


template<typename T>
void TCheck(const T& a, const T& b)
{
	if (a != b)
	{
		ErrorLn("------------------------Redis 反序列化失败!!!");
	}
}
template<>
void TCheck(const string& a, const string& b)
{
	if (a.compare(b) != 0)
	{
		ErrorLn("------------------------Redis 反序列化失败!!!");
	}
}

template<typename T>
void TTest(Redis::IRedisConnector* pRedis, const char* key, const T& a)
{
	pRedis->valueOp()->set(key, a);
	T b;
	pRedis->valueOp()->get(key, b);
	TCheck(a, b);
}


ValueOpTest::ValueOpTest(Redis::IValueOp* pValueOp)
	: m_pValueOp(pValueOp)
{}
const char* ValueOpTest::GetName()
{
	return "ValueOpTest";
}


#define DSetGet(TYPE, _a)	{			\
	string keyStr = #TYPE;				\
	TYPE a = _a;						\
	m_pValueOp->set(keyStr, a);			\
	TYPE b;								\
	m_pValueOp->get(keyStr, b);			\
	if(a != b)							\
	{									\
		string strErr = #TYPE;			\
		ErrorLn(strErr.c_str() << " is not equal! a=" << a << ", b=" << b);	\
		return false;					\
	}									\
	else								\
	{									\
		TraceLn(keyStr << " Set/Get Test Pass.");	\
	}									\
}


bool ValueOpTest::Run()
{
	// 基本类型
	TraceLn("Base type test begin!");
	DSetGet(bool, true);
	DSetGet(bool, false);
	DSetGet(char, 127);
	DSetGet(BYTE, 127);
	DSetGet(short, -32768);
	DSetGet(short, 2768);
	DSetGet(WORD, 2768);
	DSetGet(int, 123456789);
	DSetGet(int, -123456789);
	DSetGet(long, 987654321);
	DSetGet(long, -987654321);
	DSetGet(DWORD, 123454321);
	DSetGet(__int64, -123456789);
	TraceLn("Base type test completed!");

	// 对象
	//////////////////////////////////////////////////////////////////////////
	if (!TestEmptyStruct())
		return false;

	if (!TestFixStruct())
		return false;

	if (!TestSerializableData())
		return false;
	//////////////////////////////////////////////////////////////////////////

	return true;
}

bool ValueOpTest::TestEmptyStruct()
{
	string key = "keyEmptyFixStruct";

	EmptyFixStruct in;
	m_pValueOp->set(key, in);

	EmptyFixStruct out;
	m_pValueOp->get(key, out);

	TraceLn("TestEmptyStruct pass.");
	return true;
}

bool ValueOpTest::TestFixStruct()
{
	string key = "keyFix";

	FixStruct fixIn;
	char* sztmp = "qwertyuiop";
	strcpy(fixIn.sz, sztmp);
	fixIn.a = 100859;
	fixIn.b = 110;
	fixIn.f = 9.99;
	m_pValueOp->set(key, fixIn);

	FixStruct fixOut;
	m_pValueOp->get(key, fixOut);

	if (fixIn.a != fixOut.a 
		|| fixIn.b != fixOut.b
		|| fixIn.f != fixOut.f
		|| strcmp(fixIn.sz, fixOut.sz))
	{
		ErrorLn("TestFixStruct fail!!!!!!!!!!!!!!!!!!!");
		return false;
	}
	
	TraceLn("TestFixStruct pass.");
	return true;
}

bool ValueOpTest::TestSerializableData()
{
	string key = "keySerializableData";

	TestStruct in;
	m_pValueOp->set(key, in);

	TestStruct out;
	out.a = 0;
	out.b = 0;
	out.str = "";
	m_pValueOp->get(key, out);
	if (in.a != out.a || in.str.compare(out.str))
	{
		ErrorLn("TestSerializableData fail!!!!!!!!!!!!!!!!!!!");
		return false;
	}

	TraceLn("TestSerializableData pass.");
	return true;
}
