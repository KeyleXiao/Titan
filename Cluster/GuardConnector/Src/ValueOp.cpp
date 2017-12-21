#include "stdafx.h"
#include "ValueOp.h"
#include "redis.h"


CValueOp::CValueOp(CRedisClient& redis)
	: m_redis(redis)
{
}

bool CValueOp::set(const string& strKey, const string& strValue)
{
	return m_redis.Set(strKey, strValue, nullptr) == RC_SUCCESS;
}

bool CValueOp::incrbyString(const string& strKey, long nIncr, long* pnValue)
{
	if (pnValue == nullptr)
		return false;

	return m_redis.Incrby(strKey, nIncr, pnValue, nullptr) == RC_SUCCESS;
}

bool CValueOp::incrbyfloatString(const string& strKey, double dIncr, double* pdValue)
{
	if (pdValue == nullptr)
		return false;

	return m_redis.Incrbyfloat(strKey, dIncr, pdValue, nullptr) == RC_SUCCESS;
}

bool CValueOp::getString(const string& strKey, string* pstrValue)
{
	if (pstrValue == nullptr)
		return false;

	return m_redis.Get(strKey, pstrValue, nullptr) == RC_SUCCESS;
}