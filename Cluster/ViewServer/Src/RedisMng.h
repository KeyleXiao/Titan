#pragma once
#include "IRedisConnector.h"



class RedisMng : public Singleton<RedisMng>
{
public:
	RedisMng();

	bool	Init();

	Redis::IRedisConnector*	GetRedis() { return m_pRedis; }

private:


private:
	Redis::IRedisConnector*		m_pRedis;
};
extern RedisMng& gRedisMng;


