#pragma once
#include "IRedisConnector.h"



class RedisMng : public Singleton<RedisMng>
{
public:
	RedisMng();

	bool	Init();

private:
	//************************************
	// Returns:   bool 只在Redis无法连接的情况下返回false
	// Qualifier: 检查版本号。（如果Redis中版本号与我的不一致，则清空Redis，并写入新的版本号）
	//************************************
	bool	CheckVersion();

private:
	Redis::IRedisConnector*		m_pRedis;
};
extern RedisMng& gRedisMng;


