#pragma once
#include "IPipelineValueOp.h"


class CRedisClient;
struct IPipelineHolder;

class CPipelineValueOp : public Redis::IPipelineValueOp
{
public:
	CPipelineValueOp(CRedisClient& redis, Pipeline pPipeline);
	virtual	~CPipelineValueOp() {};

	virtual bool set(const string& strKey, const string& strValue) override;


	virtual bool getString(const string& strKey, string* pstrValue) override;


	virtual bool incrbyString(const string& strKey, long nIncr, long* pnValue) override;


	virtual bool incrbyfloatString(const string& strKey, double dIncr, double* pdValue) override;

private:
	CRedisClient&		m_redis;
	Pipeline			m_pPipeline;
};