#include "stdafx.h"
#include "PipelineListOp.h"
#include "redis.h"


CPipelineListOp::CPipelineListOp(CRedisClient& redis, Pipeline pPipeline)
	: m_redis(redis)
	, m_pPipeline(pPipeline)
{
	Assert(pPipeline);
}

bool CPipelineListOp::lPush(const string& strKey, const string& strValue, long *pnVal)
{
	return m_redis.Lpush(strKey, strValue, pnVal, m_pPipeline) == RC_SUCCESS;
}

//bool CPipelineListOp::lPushVec(const string& strKey, const vector<string>& vecValue)
//{
//	return m_redis.Lpush(strKey, vecValue) == RC_SUCCESS;
//}

bool CPipelineListOp::rPush(const string& strKey, const string& strValue, long *pnVal)
{
	return m_redis.Rpush(strKey, strValue, pnVal, m_pPipeline) == RC_SUCCESS;
}

//bool CPipelineListOp::rPushVec(const string& strKey, const vector<string>& vecValue)
//{
//	return m_redis.Rpush(strKey, vecValue) == RC_SUCCESS;
//}

bool CPipelineListOp::lInsert(const string& strKey, const string& strPos, const string& strPivot, const string& strValue)
{
	long nValue;
	// 非列表不能执行此操作
	if (m_redis.Linsert(strKey, strPos, strPivot, strValue, &nValue, m_pPipeline) != RC_SUCCESS)
		return false;
	// 空列表 nValue = 0，空Pivot nValue = -1
	return nValue > 0;
}

bool CPipelineListOp::lPopString(const string& strKey, string* pstrValue)
{
	if (pstrValue == nullptr)
		return false;

	return m_redis.Lpop(strKey, pstrValue, m_pPipeline) == RC_SUCCESS;
}

bool CPipelineListOp::rPopString(const string& strKey, string* pstrValue)
{
	if (pstrValue == nullptr)
		return false;

	return m_redis.Rpop(strKey, pstrValue, m_pPipeline) == RC_SUCCESS;
}

bool CPipelineListOp::lRem(const string& strKey, long nCount, const string& strValue, long *pnVal)
{
	return m_redis.Lrem(strKey, nCount, strValue, pnVal, m_pPipeline) == RC_SUCCESS;
}

bool CPipelineListOp::lTrim(const string& strKey, long nStart, long nStop)
{
	return m_redis.Ltrim(strKey, nStart, nStop, m_pPipeline) == RC_SUCCESS;
}

bool CPipelineListOp::lSet(const string& strKey, long nIndex, const string& strValue)
{
	return m_redis.Lset(strKey, nIndex, strValue, m_pPipeline) == RC_SUCCESS;
}

bool CPipelineListOp::lRange(const string& strKey, long nStart, long nStop, vector<string>* pvecValue)
{
	if (pvecValue == nullptr)
		return false;

	return m_redis.Lrange(strKey, nStart, nStop, pvecValue, m_pPipeline) == RC_SUCCESS;
}

bool CPipelineListOp::lIndex(const string& strKey, long nIndex, string* pstrValue)
{
	if (pstrValue == nullptr)
		return false;

	return m_redis.Lindex(strKey, nIndex, pstrValue, m_pPipeline) == RC_SUCCESS && !(*pstrValue).empty();
}

bool CPipelineListOp::lLen(const string& strKey, long* pnValue)
{
	if (pnValue == nullptr)
		return false;

	return m_redis.Llen(strKey, pnValue, m_pPipeline) == RC_SUCCESS;
}
