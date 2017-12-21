#include "stdafx.h"
#include "SetOp.h"


CSetOp::CSetOp(CRedisClient& redis)
	: m_redis(redis)
{

}