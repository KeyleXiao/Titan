#include "stdafx.h"
#include "RedisMng.h"
#include "buffer.h"
#include "Setting.h"
#include "ValueOpStringTest.h"
#include "ValueOpTest.h"
#include "PipelineTest.h"
#include "ListOpPushAndPopTest.h"
#include "ListOpTest.h"


RedisMng& gRedisMng = RedisMng::getInstance();

RedisMng::RedisMng()
	: m_pRedis(nullptr)
{
	m_pRedis = CreateRedisConnectorProc();
}

RedisMng::~RedisMng()
{
	for (size_t i = 0; i < m_vecTest.size(); i++)
	{
		IRedisTest*& pTest = m_vecTest.at(i);
		safeDelete(pTest);
	}
	m_vecTest.clear();
}

void RedisMng::Register(Redis::IRedisConnector* pRedis)
{
	Assert(pRedis);

	m_vecTest.push_back(new ValueOpStringTest(pRedis->valueOp()));
	m_vecTest.push_back(new ValueOpTest(pRedis->valueOp()));
	m_vecTest.push_back(new ListOpPushAndPopTest(pRedis->listOp()));
	m_vecTest.push_back(new ListOpTest(pRedis->listOp()));
	m_vecTest.push_back(new PipelineTest(pRedis));
}

void RedisMng::RunTests()
{
	bool bAllCuccess = true;
	for (size_t i = 0; i < m_vecTest.size(); i++)
	{
		IRedisTest*& pTest = m_vecTest.at(i);

		TraceLn(pTest->GetName() << _GT(" 测试开始!"));
		if (pTest->Run())
		{
			TraceLn(pTest->GetName() << _GT(" 测试成功!"));
		}
		else
		{
			bAllCuccess = false;
			TraceLn(pTest->GetName() << _GT(" 测试失败!!!!!!!!!!!!!!!!!!!!!!!"));
		}
	}

	if (bAllCuccess)
		TraceLn(_GT("=========================== 所有测试 成功!============================"));
}

void RedisMng::Init()
{
	// RedisConnector内部的单元测试
	StartUnitTest();


	string str = "127.0.0.1";
	WORD port = 6379;

	m_pRedis->create(str.c_str(), port);

	Register(m_pRedis);
	RunTests();
}


bool RedisMng::StartUnitTest()
{
	TraceLn("RedisConnector Test Begin");
	int ret = RunTest();
	if (ret == 0)
	{
		ErrorLn("RedisConnector Test Fail!!!!!!!!!!!!!!!!!!!!!!!!");
		return false;
	}
	else
	{
		TraceLn("RedisConnector Test Pass");
		return true;
	}
}