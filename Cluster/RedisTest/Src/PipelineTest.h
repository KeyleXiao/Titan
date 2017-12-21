#pragma once
#include "IRedisTest.h"


class PipelineTest : public IRedisTest
{
public:
	PipelineTest(Redis::IRedisConnector* pRedis);


	virtual const char* GetName() override;


	virtual bool Run() override;

protected:
private:
	Redis::IRedisConnector*		m_pRedis;
};