#pragma once
#include "IRedisConnector.h"


class CRedisClient;

class CKeysOp : public Redis::IKeysOp
{
public:
	CKeysOp(CRedisClient& redis);

	// ɾ��
	virtual bool	delKey(const string& strKey) override;

	// ����
	virtual bool	renameKey(const string& strKey, const string& strNewKey) override;

	// ����
	virtual bool	existKey(const string& strKey, bool* pbExist) override;

private:
	CRedisClient&	m_redis;
};