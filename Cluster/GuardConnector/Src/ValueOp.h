#pragma once
#include "IRedisConnector.h"


class CRedisClient;

class CValueOp : public Redis::IValueOp
{
public:
	CValueOp(CRedisClient& redis);

	// 添加
	virtual bool	set(const string& strKey, const string& strValue);

	// 添加（long:无Key添加，有Key加法）
	virtual bool	incrbyString(const string& strKey, long nIncr, long* pnValue);

	// 添加（float:无Key添加，有Key加法）
	virtual bool	incrbyfloatString(const string& strKey, double dIncr, double* pdValue);

	// 获取
	virtual bool	getString(const string& strKey, string* pstrValue);

private:
	CRedisClient&		m_redis;
};