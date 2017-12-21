#pragma once
#include "IRedisConnector.h"



class RedisMng : public Singleton<RedisMng>
{
public:
	RedisMng();

	void	Init();

private:
	void	StartUnitTest();

	void	TempTest();

private:
	Redis::IRedisConnector*		m_pRedis;
};
extern RedisMng& gRedisMng;


