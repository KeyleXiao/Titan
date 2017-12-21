#include "stdafx.h"
#include "HashOp.h"


CHashOp::CHashOp(CRedisClient& redis)
	: m_redis(redis)
{

}