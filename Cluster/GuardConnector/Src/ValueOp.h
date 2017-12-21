#pragma once
#include "IRedisConnector.h"


class CRedisClient;

class CValueOp : public Redis::IValueOp
{
public:
	CValueOp(CRedisClient& redis);

	// ���
	virtual bool	set(const string& strKey, const string& strValue);

	// ��ӣ�long:��Key��ӣ���Key�ӷ���
	virtual bool	incrbyString(const string& strKey, long nIncr, long* pnValue);

	// ��ӣ�float:��Key��ӣ���Key�ӷ���
	virtual bool	incrbyfloatString(const string& strKey, double dIncr, double* pdValue);

	// ��ȡ
	virtual bool	getString(const string& strKey, string* pstrValue);

private:
	CRedisClient&		m_redis;
};