#include "stdafx.h"
#include "RedisConnector.h"
#include "ValueOp.h"
#include "PipelineExecuter.h"
#include "ListOp.h"
#include "KeysOp.h"
#include "ServerOp.h"

using namespace Redis;

RedisConnector::RedisConnector()
{
}

RedisConnector::~RedisConnector()
{

}

// string ip, int port, int timeout, int connum
bool RedisConnector::create(const char* ip, WORD port)
{
	TraceLn(__FUNCTION__);

	const int nTimeout = 2;
	if (!m_redis.Initialize(ip, port, nTimeout, 10))
		return false;
	m_Setting.setTimeout(nTimeout);

	// Value操作器
	m_pValueOp = new CValueOp(m_redis);
	// List操作器
	m_pListOp = new CListOp(m_redis);
	//// Set操作器
	//ISetOp*			m_pSetOp;
	//// Hash操作器
	//IHashOp*		m_pHashOp;
	// 其他指令操作器
	m_pKeysOp = new CKeysOp(m_redis);

	m_pServerOp = new CServerOp(m_redis);

	return true;
}

void RedisConnector::release()
{
	stop();

	delete this;
}

void RedisConnector::stop()
{
	m_redis.release();
}


IPipeline* RedisConnector::pipeBegin()
{
	return new PipelineExecuter(m_redis);
}
bool RedisConnector::pipeEnd(IPipeline* pPipe)
{
	delete pPipe;
	return true;
}

//bool RedisConnector::setString(const string& strKey, const string& strValue)
//{
//	return m_redis.Set(strKey, strValue) == RC_SUCCESS;
//}
//
//bool RedisConnector::incrbyString(const string& strKey, long nIncr, long* pnValue)
//{
//	if(pnValue == nullptr)
//		return false;
//
//	return m_redis.Incrby(strKey, nIncr, pnValue) == RC_SUCCESS;
//}
//
//bool RedisConnector::incrbyfloatString(const string& strKey, double dIncr, double* pdValue)
//{
//	if (pdValue == nullptr)
//		return false;
//
//	return m_redis.Incrbyfloat(strKey, dIncr, pdValue) == RC_SUCCESS;
//}
//
//bool RedisConnector::getString(const string& strKey, string* pstrValue)
//{
//	if(pstrValue == nullptr)
//		return false;
//
//	return m_redis.Get(strKey, pstrValue) == RC_SUCCESS;
//}



bool RedisConnector::setHash(const string& strKey, const string& strField, const string& strValue)
{
	return m_redis.Hset(strKey, strField, strValue) == RC_SUCCESS;
}

bool RedisConnector::mSetVecHash(const string& strKey, const vector<string>& vecField, const vector<string>& vecValue)
{
	return m_redis.Hmset(strKey, vecField, vecValue) == RC_SUCCESS;
}

bool RedisConnector::mSetMapHash(const string& strKey, const map<string, string>& mapFv)
{
	return m_redis.Hmset(strKey, mapFv) == RC_SUCCESS;
}

bool RedisConnector::incrbyHash(const string& strKey, const string& strField, long nIncr, long * pnValue)
{
	if (pnValue == nullptr)
		return false;

	return m_redis.Hincrby(strKey, strField, nIncr, pnValue) == RC_SUCCESS;
}

bool RedisConnector::incrbyfloatHash(const string& strKey, const string& strField, double dIncr, double * pdValue)
{
	if (pdValue == nullptr)
		return false;

	return m_redis.Hincrbyfloat(strKey, strField, dIncr, pdValue) == RC_SUCCESS;
}

bool RedisConnector::delHash(const string& strKey, const string& strField)
{
	return m_redis.Hdel(strKey, strField) == RC_SUCCESS;
}

bool RedisConnector::getHash(const string& strKey, const string& strField, string * pstrValue)
{
	if (pstrValue == nullptr)
		return pstrValue;

	return  m_redis.Hget(strKey, strField, pstrValue) == RC_SUCCESS;
}

bool RedisConnector::mGetVecHash(const string& strKey, const vector<string>& vecField, vector<string> * pvecValue)
{
	if (pvecValue == nullptr)
		return pvecValue;

	return m_redis.Hmget(strKey, vecField, pvecValue) == RC_SUCCESS;
}

bool RedisConnector::mGetMapHash(const string& strKey, const vector<string>& vecField, map<string, string> * pmapFv)
{
	if (pmapFv == nullptr)
		return false;

	return m_redis.Hmget(strKey, vecField, pmapFv) == RC_SUCCESS;
}

bool RedisConnector::keysHash(const string& strKey, vector<string>* pvecField)
{
	if (pvecField == nullptr)
		return false;

	return m_redis.Hkeys(strKey, pvecField) == RC_SUCCESS;
}

bool RedisConnector::valuesHash(const string& strKey, vector<string>* pvecValue)
{
	if(pvecValue == nullptr)
		return false;

	return m_redis.Hvals(strKey, pvecValue) == RC_SUCCESS;
}

bool RedisConnector::getAllHash(const string& strKey, map<string, string>* pmapFv)
{
	if (pmapFv == nullptr)
		return false;

	return m_redis.Hgetall(strKey, pmapFv) == RC_SUCCESS;
}

bool RedisConnector::existHash(const string& strKey, const string& strField, bool* pbValue)
{
	if (pbValue == nullptr)
		return false;

	long nValue;
	if (m_redis.Hexists(strKey, strField, &nValue) != RC_SUCCESS)
		return false;

	*pbValue = nValue > 0;
	return true;
}

bool RedisConnector::lenHash(const string& strKey, long* pnValue)
{
	if (pnValue == nullptr)
		return false;

	return m_redis.Hlen(strKey, pnValue) == RC_SUCCESS;
}


bool RedisConnector::addSet(const string& strKey, const string& strValue)
{
	return m_redis.Sadd(strKey, strValue) == RC_SUCCESS;
}

bool RedisConnector::remSet(const string& strKey, const string& strValue)
{
	return m_redis.Srem(strKey, strValue) == RC_SUCCESS;
}

// 删除（多个）
bool RedisConnector::remSet(const string& strKey, const vector<string>& vecValue)
{
	return m_redis.Srem(strKey, vecValue) == RC_SUCCESS;
}

// 删除（随机）
bool RedisConnector::popSet(const string& strKey, string* pstrValue)
{
	if (pstrValue == nullptr)
		return false;

	return m_redis.Spop(strKey, pstrValue) == RC_SUCCESS;
}

// 获取（所有）
bool RedisConnector::membersSet(const string& strKey, vector<string>* pvecValue)
{
	if (pvecValue == nullptr)
		return false;

	return m_redis.Smembers(strKey, pvecValue) == RC_SUCCESS;
}

//// 获取（随机）
//bool RedisConnector::randMemberSet(const string& strKey, long nCount, vector<string>* pvecValue)
//{
//	if (pvecValue == nullptr)
//		return false;
//
//	return m_redis.Srandmember(strKey, nCount, pvecValue) == RC_SUCCESS;
//}

// 存在
bool RedisConnector::isMemberSet(const string& strKey, const string& strValue, bool* pbValue)
{
	if (pbValue == nullptr)
		return false;

	long nValue;
	if (m_redis.Sismember(strKey, strValue, &nValue) != RC_SUCCESS)
		return false;

	*pbValue = nValue > 0;
	return true;
}

// 数量
bool RedisConnector::cardSet(const string& strKey, long* pnValue)
{
	if (pnValue == nullptr)
		return false;

	return m_redis.Scard(strKey, pnValue) == RC_SUCCESS;
}

API_EXPORT  Redis::IRedisConnector * RKT_CDECL CreateRedisConnector()
{
	return new RedisConnector();
}