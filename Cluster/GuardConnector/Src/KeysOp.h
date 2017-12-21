#pragma once
#include "IRedisConnector.h"


class CRedisClient;

class CKeysOp : public Redis::IKeysOp
{
public:
	CKeysOp(CRedisClient& redis);

	// É¾³ý
	virtual bool	delKey(const string& strKey) override;

	// ¸ü¸Ä
	virtual bool	renameKey(const string& strKey, const string& strNewKey) override;

	// ´æÔÚ
	virtual bool	existKey(const string& strKey, bool* pbExist) override;

private:
	CRedisClient&	m_redis;
};