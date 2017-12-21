#include "stdafx.h"
#include "KeysOp.h"
#include "redis.h"


CKeysOp::CKeysOp(CRedisClient& redis)
	: m_redis(redis)
{

}

bool CKeysOp::delKey(const string& strKey)
{
	return m_redis.Del(strKey) == RC_SUCCESS;
}

bool CKeysOp::renameKey(const string& strKey, const string& strNewKey)
{
	return m_redis.Rename(strKey, strNewKey) == RC_SUCCESS;
}

bool CKeysOp::existKey(const string& strKey, bool* pbExist)
{
	if (pbExist == nullptr)
		return false;

	long nValue;
	if (m_redis.Exists(strKey, &nValue) != RC_SUCCESS)
		return false;

	*pbExist = nValue > 0;
	return true;
}