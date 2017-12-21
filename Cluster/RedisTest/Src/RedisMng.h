#pragma once
#include "IRedisConnector.h"
#include "IRedisTest.h"


class RedisMng : public Singleton<RedisMng>
{
public:
	RedisMng();
	virtual	~RedisMng();

	void	Init();

	void	RunTests();

private:
	bool	StartUnitTest();

	void	Register(Redis::IRedisConnector* pRedis);

private:
	Redis::IRedisConnector*		m_pRedis;
	std::vector<IRedisTest*>	m_vecTest;
};
extern RedisMng& gRedisMng;


