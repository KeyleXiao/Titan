#pragma once
#include "IRedisConnector.h"


class CRedisClient;

class CSetOp : public Redis::ISetOp
{
public:
	CSetOp(CRedisClient& redis);

private:
	CRedisClient&	m_redis;
};