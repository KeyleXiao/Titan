#include "stdafx.h"
#include "RedisMng.h"
#include "buffer.h"
#include "Setting.h"


RedisMng& gRedisMng = RedisMng::getInstance();

RedisMng::RedisMng()
	: m_pRedis(nullptr)
{
	m_pRedis = CreateRedisConnectorProc();
}

void RedisMng::Init()
{
	//StartUnitTest();


	string str = "127.0.0.1";
	WORD port = 6379;

	m_pRedis->create(str.c_str(), port);


	/*****************************************************************/
	// 临时测试代码
	TempTest();
	/*****************************************************************/
}

struct TestStruct : public ISerializableData
{
	int		a;
	string	str;
	double	b;

	TestStruct()
	{
		a = 142857;
		str = "test";
		b = 1.23456789;
	}

	// 反序列化
	virtual	void fromBytes(const char* p, size_t len)
	{
		ibuffer ibufData(p, len);
		ibufData >> a >> str >> b;
	}

	// 序列化
	virtual	void toBytes(obuf& obufData) const
	{
		obufData << a << str << b;
	}
};

struct EmptyFixStruct
{
};
struct FixStruct
{
	EmptyFixStruct ef;
	char	sz[32];
	int		a;
	short	b;
	double	f;

	FixStruct() { memset(this, 0, sizeof(*this)); }
};

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
	if (a.compare(b)!=0)
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

void RedisMng::TempTest()
{
	//{
	//	char sz[] = "valueString";
	//	ibuffer ibufData(sz, sizeof(sz)+1);
	//	string tmp;
	//	ibufData >> tmp;
	//	ErrorLn("tmp.compare('valueString')==" << tmp.compare("valueString"));
	//}
	//////////////////////////////////////////////////////////////////////////
	string keyString("keyString");
	string valueStringRet;

	{
		// char*
		char* psz = "valueString";
		m_pRedis->valueOp()->set(keyString, psz);
		m_pRedis->valueOp()->get(keyString, valueStringRet);
		if (valueStringRet.compare(psz) != 0)
		{
			ErrorLn("--------------- valueStringRet.compare(char*) != 0");
		}
	}
	{
		// char[]
		char sz[] = "valueString";
		m_pRedis->valueOp()->set(keyString, sz);
		m_pRedis->valueOp()->get(keyString, valueStringRet);
		if (valueStringRet.compare(sz) != 0)
		{
			ErrorLn("--------------- valueStringRet.compare(char[]) != 0");
		}
	}
	{
		// std::string
		string valueString("valueString");
		m_pRedis->valueOp()->set(keyString, valueString);
		m_pRedis->valueOp()->get(keyString, valueStringRet);
		if (valueStringRet.compare(valueString) != 0)
		{
			ErrorLn("--------------- valueStringRet.compare(std::string) != 0");
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 阻塞List
	{
		long length = 0;
		string keyBlock("keyBlock");

		m_pRedis->listOp()->lRem(keyBlock, -2, "ValueB1", &length);

		m_pRedis->listOp()->lInsertAfter(keyBlock, "ValueB1", "hahaha");
		m_pRedis->listOp()->lInsertBefore(keyBlock, "hahaha", "heihei");
		
		// 存
		//m_pRedis->listOp()->lPush(keyBlock, "ValueB1", &length);
		//m_pRedis->listOp()->lPush(keyBlock, "ValueB2", &length);
		m_pRedis->listOp()->rPush(keyBlock, 1024, &length);
		int iRet = 0;
		m_pRedis->listOp()->rPop(keyBlock, iRet);
		// 取
		//std::vector<string> vecStrB;
		//m_pRedis->listOp()->Brpop(keyBlock, &vecStrB);
		string keyBlock99("keyBlock99");
		string strB;
		m_pRedis->listOp()->Brpop(keyBlock99, strB, 2);


	}

	// 阻塞List 多个key
	{
		//string keyBlock2("keyBlock2");
		//string keyBlock3("keyBlock3");
		//// 存
		//m_pRedis->listOp()->lPush(keyBlock3, "ValueB1");
		//m_pRedis->listOp()->lPush(keyBlock3, "ValueB2");
		//// 取
		//string strB;
		//std::vector<std::string> vecKey;
		//vecKey.push_back(keyBlock2);
		//vecKey.push_back(keyBlock3);
		//string strKeyR;
		//m_pRedis->listOp()->Brpop(vecKey, strB, &strKeyR);
		//m_pRedis->listOp()->Blpop(vecKey, strB);
	}
	//////////////////////////////////////////////////////////////////////////
	// 基本类型
	TraceLn("Base type test begin!");
	TTest(m_pRedis, "int", 123456789);
	TTest(m_pRedis, "float", 1.19f);
	TTest(m_pRedis, "short", (short)-32768);
	TTest(m_pRedis, "BYTE", (BYTE)128);
	TTest(m_pRedis, "double", 3.1415926);
	TTest(m_pRedis, "long long", 123456789987654321);
	TraceLn("Base type test completed!");

	//////////////////////////////////////////////////////////////////////////
	FixStruct fixS1;
	fixS1.a = 100859;
	fixS1.b = 110;
	fixS1.f = 9.99;
	m_pRedis->valueOp()->set("keyFix1", fixS1);

	FixStruct fixS1_;
	m_pRedis->valueOp()->get("keyFix1", fixS1_);

	if (fixS1.a != fixS1_.a || fixS1.b != fixS1_.b)
	{
		ErrorLn("------------------------Redis 反序列化失败!!!");
	}
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	TestStruct ss;
	//ISerializableData& ssTemp = ss;

	m_pRedis->valueOp()->set("keyObj4", ss);
	TestStruct ss_;
	ss_.a = 0;
	ss_.b = 0;
	ss_.str = "";
	m_pRedis->valueOp()->get("keyObj", ss_);
	if (ss.a != ss_.a || ss.str.compare(ss_.str) != 0)
	{
		ErrorLn("------------------------Redis 反序列化失败!!!");
	}

	// 临时测试代码
	//string str("_测试_");
	//m_pRedis->valueOp()->set("key1", str);
	//string strTemp;
	//m_pRedis->valueOp()->get("key1", &strTemp);
	//Assert(str.compare(strTemp) == 0);

	//m_pRedis->valueOp()->set("key2", "Value2_");

	////string strTest = "Value3_";
	//char szValue[] = "Value3_";
	//m_pRedis->valueOp()->set("key3", szValue);

	string strTemp2;
	string strTemp3;
	m_pRedis->pipeline([&](Redis::IPipeline* pPipe)
	{
		//pPipe->valueOp()->set("key2", "Value2");
		//pPipe->valueOp()->set("key3", "Value3");

		pPipe->valueOp()->get("key2", strTemp2);
		pPipe->valueOp()->get("key3", strTemp3);
	});
}

void RedisMng::StartUnitTest()
{
	int ret = RunTest();
	if (ret == 0)
	{
		ErrorLn("------------------------Redis Test Fail!!!");
	}
	else
	{
		TraceLn("++++++++++++++++++++++++Redis Test Success!!!");
	}
}