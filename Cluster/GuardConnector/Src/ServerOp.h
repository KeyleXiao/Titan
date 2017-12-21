#pragma once
#include "IServerOp.h"


class CRedisClient;

class CServerOp : public Redis::IServerOp
{
public:
	CServerOp(CRedisClient& redis);

	virtual bool FlushAll() override;

	virtual bool FlushDb() override;

private:
	CRedisClient&	m_redis;
};