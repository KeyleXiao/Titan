#include "stdafx.h"
#include "PipelineTest.h"


PipelineTest::PipelineTest(Redis::IRedisConnector* pRedis)
	: m_pRedis(pRedis)
{
}

const char* PipelineTest::GetName()
{
	return "PipelineTest";
}

bool PipelineTest::Run()
{
	string keyPipeList = "keyPipeList";

	// ´æ
	bool bIn1 = true;
	BYTE byIn1 = 101;
	string strIn1 = "Value1";
	string strIn2 = "Value2";
	string strIn3 = "Value3";
	m_pRedis->pipeline([&](Redis::IPipeline* pPipe)
	{
		pPipe->valueOp()->set("bIn1", bIn1);
		pPipe->valueOp()->set("byIn1", byIn1);
		pPipe->valueOp()->set("key1", strIn1);
		pPipe->valueOp()->set("key2", strIn2);
		pPipe->valueOp()->set("key3", strIn3);

		pPipe->listOp()->lPush(keyPipeList, bIn1);
		pPipe->listOp()->lPush(keyPipeList, byIn1);
		pPipe->listOp()->lPush(keyPipeList, strIn1);
		pPipe->listOp()->lPush(keyPipeList, strIn2);
		pPipe->listOp()->lPush(keyPipeList, strIn3);
	});

	// È¡
	{
		bool bOut1 = false;
		BYTE byOut1 = 0;
		string strOut1;
		string strOut2;
		string strOut3;
		m_pRedis->pipeline([&](Redis::IPipeline* pPipe)
		{
			pPipe->valueOp()->get("bIn1", bOut1);
			pPipe->valueOp()->get("byIn1", byOut1);
			pPipe->valueOp()->get("key1", strOut1);
			pPipe->valueOp()->get("key2", strOut2);
			pPipe->valueOp()->get("key3", strOut3);
		});

		if (bOut1 != bIn1
			|| byOut1 != byIn1
			|| strIn1.compare(strOut1) || strIn2.compare(strOut2) || strIn3.compare(strOut3))
		{
			//ErrorLn("PipelineTest fail!!!!!!!!!!!!!!!!!!!");
			return false;
		}
	}
	{
		bool bOut1 = false;
		BYTE byOut1 = 0;
		string strOut1;
		string strOut2;
		string strOut3;
		m_pRedis->pipeline([&](Redis::IPipeline* pPipe)
		{
			pPipe->listOp()->rPop(keyPipeList, bOut1);
			pPipe->listOp()->rPop(keyPipeList, byOut1);
			pPipe->listOp()->rPop(keyPipeList, strOut1);
			pPipe->listOp()->rPop(keyPipeList, strOut2);
			pPipe->listOp()->rPop(keyPipeList, strOut3);
		});

		if (bOut1 != bIn1
			|| byOut1 != byIn1
			|| strIn1.compare(strOut1) || strIn2.compare(strOut2) || strIn3.compare(strOut3))
		{
			//ErrorLn("PipelineTest fail!!!!!!!!!!!!!!!!!!!");
			return false;
		}
	}

	return true;
}
