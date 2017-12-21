#pragma once
#include "IPipelineListOp.h"


class CRedisClient;

class CPipelineListOp : public Redis::IPipelineListOp
{
public:
	CPipelineListOp(CRedisClient& redis, Pipeline pPipeline);
	virtual	~CPipelineListOp() {};

	virtual bool lRem(const string& strKey, long nCount, const string& strValue, long *pnVal = nullptr) override;


	virtual bool lTrim(const string& strKey, long nStart, long nStop) override;


	virtual bool lSet(const string& strKey, long nIndex, const string& strValue) override;


	virtual bool lRange(const string& strKey, long nStart, long nStop, vector<string>* pvecValue) override;


	virtual bool lIndex(const string& strKey, long nIndex, string* pstrValue) override;


	virtual bool lLen(const string& strKey, long* pnValue) override;

protected:

	virtual bool lPush(const string& strKey, const string& strValue, long *pnVal = nullptr) override;


	virtual bool rPush(const string& strKey, const string& strValue, long *pnVal = nullptr) override;


	virtual bool lInsert(const string& strKey, const string& strPos, const string& strPivot, const string& strValue) override;


	virtual bool lPopString(const string& strKey, string* pstrValue) override;


	virtual bool rPopString(const string& strKey, string* pstrValue) override;

private:
	CRedisClient&		m_redis;
	Pipeline			m_pPipeline;
};