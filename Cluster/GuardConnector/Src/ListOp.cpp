#include "stdafx.h"
#include "ListOp.h"
#include "redis.h"


CListOp::CListOp(CRedisClient& redis)
	: m_redis(redis)
{
}


bool CListOp::Blpop(const std::string &strKey, std::vector<std::string> *pvecVal, long nTimeout)
{
	if (pvecVal == nullptr)
		return false;

	return m_redis.Blpop(strKey, nTimeout, pvecVal) == RC_SUCCESS;
}

bool CListOp::Blpop(const std::vector<std::string> &vecKey, std::vector<std::string> *pvecVal, long nTimeout)
{
	if (pvecVal == nullptr)
		return false;

	return m_redis.Blpop(vecKey, nTimeout, pvecVal) == RC_SUCCESS;
}

bool CListOp::Brpop(const std::string &strKey, std::vector<std::string> *pvecVal, long nTimeout)
{
	if (pvecVal == nullptr)
		return false;

	return m_redis.Brpop(strKey, nTimeout, pvecVal) == RC_SUCCESS;
}

bool CListOp::Brpop(const std::vector<std::string> &vecKey, std::vector<std::string> *pvecVal, long nTimeout)
{
	if (pvecVal == nullptr)
		return false;

	return m_redis.Brpop(vecKey, nTimeout, pvecVal) == RC_SUCCESS;
}

bool CListOp::lPush(const string& strKey, const string& strValue, long *pnVal)
{
	return m_redis.Lpush(strKey, strValue, pnVal) == RC_SUCCESS;
}

//bool CListOp::lPushVec(const string& strKey, const vector<string>& vecValue)
//{
//	return m_redis.Lpush(strKey, vecValue) == RC_SUCCESS;
//}

bool CListOp::rPush(const string& strKey, const string& strValue, long *pnVal)
{
	return m_redis.Rpush(strKey, strValue, pnVal) == RC_SUCCESS;
}

//bool CListOp::rPushVec(const string& strKey, const vector<string>& vecValue)
//{
//	return m_redis.Rpush(strKey, vecValue) == RC_SUCCESS;
//}

bool CListOp::lInsert(const string& strKey, const string& strPos, const string& strPivot, const string& strValue)
{
	long nValue;
	// 非列表不能执行此操作
	if (m_redis.Linsert(strKey, strPos, strPivot, strValue, &nValue) != RC_SUCCESS)
		return false;
	// 空列表 nValue = 0，空Pivot nValue = -1
	return nValue > 0;
}

bool CListOp::lPopString(const string& strKey, string* pstrValue)
{
	if (pstrValue == nullptr)
		return false;

	return m_redis.Lpop(strKey, pstrValue) == RC_SUCCESS;
}

bool CListOp::rPopString(const string& strKey, string* pstrValue)
{
	if (pstrValue == nullptr)
		return false;

	return m_redis.Rpop(strKey, pstrValue) == RC_SUCCESS;
}

bool CListOp::lRem(const string& strKey, long nCount, const string& strValue, long *pnVal)
{
	return m_redis.Lrem(strKey, nCount, strValue, pnVal) == RC_SUCCESS;
}

bool CListOp::lTrim(const string& strKey, long nStart, long nStop)
{
	return m_redis.Ltrim(strKey, nStart, nStop) == RC_SUCCESS;
}

bool CListOp::lSet(const string& strKey, long nIndex, const string& strValue)
{
	return m_redis.Lset(strKey, nIndex, strValue) == RC_SUCCESS;
}

bool CListOp::lRangeString(const string& strKey, long nStart, long nStop, vector<string>* pvecValue)
{
	if (pvecValue == nullptr)
		return false;

	return m_redis.Lrange(strKey, nStart, nStop, pvecValue) == RC_SUCCESS;
}

bool CListOp::lIndexString(const string& strKey, long nIndex, string* pstrValue)
{
	if (pstrValue == nullptr)
		return false;

	return m_redis.Lindex(strKey, nIndex, pstrValue) == RC_SUCCESS && !(*pstrValue).empty();
}

bool CListOp::lLen(const string& strKey, long* pnValue)
{
	if (pnValue == nullptr)
		return false;

	return m_redis.Llen(strKey, pnValue) == RC_SUCCESS;
}