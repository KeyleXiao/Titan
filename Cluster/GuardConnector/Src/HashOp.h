#pragma once
#include "IRedisConnector.h"


class CRedisClient;

class CHashOp : public Redis::IHashOp
{
public:
	CHashOp(CRedisClient& redis);

private:
	CRedisClient&	m_redis;
};