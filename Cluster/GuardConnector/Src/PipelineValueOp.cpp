#include "stdafx.h"
#include "PipelineValueOp.h"
#include "redis.h"


CPipelineValueOp::CPipelineValueOp(CRedisClient& redis, Pipeline pPipeline)
	: m_redis(redis)
	, m_pPipeline(pPipeline)
{
	Assert(pPipeline);
}

bool CPipelineValueOp::set(const string& strKey, const string& strValue)
{
	return m_redis.Set(strKey, strValue, m_pPipeline) == RC_SUCCESS;
}

bool CPipelineValueOp::incrbyString(const string& strKey, long nIncr, long* pnValue)
{
	if (pnValue == nullptr)
		return false;

	return m_redis.Incrby(strKey, nIncr, pnValue, m_pPipeline) == RC_SUCCESS;
}

bool CPipelineValueOp::incrbyfloatString(const string& strKey, double dIncr, double* pdValue)
{
	if (pdValue == nullptr)
		return false;

	return m_redis.Incrbyfloat(strKey, dIncr, pdValue, m_pPipeline) == RC_SUCCESS;
}

bool CPipelineValueOp::getString(const string& strKey, string* pstrValue)
{
	if (pstrValue == nullptr)
		return false;

	return m_redis.Get(strKey, pstrValue, m_pPipeline) == RC_SUCCESS;
}
