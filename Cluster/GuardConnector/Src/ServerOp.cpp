#include "stdafx.h"
#include "ServerOp.h"
#include "redis.h"


CServerOp::CServerOp(CRedisClient& redis)
	: m_redis(redis)
{
}

bool CServerOp::FlushAll()
{
	return m_redis.FlushAll() == RC_SUCCESS;
}

bool CServerOp::FlushDb()
{
	return m_redis.FlushDb() == RC_SUCCESS;
}
