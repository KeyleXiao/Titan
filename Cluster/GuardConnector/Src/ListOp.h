#pragma once
#include "IRedisConnector.h"

class CRedisClient;
using namespace Redis;

class CListOp : public IListOp
{
public:
	CListOp(CRedisClient& redis);

	// 阻塞删除（左弹出）
	virtual bool	Blpop(const std::string &strKey, std::vector<std::string> *pvecVal, long nTimeout = 0);

	// 阻塞删除（左弹出）
	virtual bool	Blpop(const std::vector<std::string> &vecKey, std::vector<std::string> *pvecVal, long nTimeout = 0);

	// 阻塞删除（右弹出）
	virtual bool	Brpop(const std::string &strKey, std::vector<std::string> *pvecVal, long nTimeout = 0);

	// 阻塞删除（右弹出）
	virtual bool	Brpop(const std::vector<std::string> &vecKey, std::vector<std::string> *pvecVal, long nTimeout = 0);

	// 添加（左单个）
	virtual bool	lPush(const string& strKey, const string& strValue, long *pnVal = nullptr);

	//// 添加（左批量）
	//virtual bool	lPushVec(const string& strKey, const vector<string>& vecValue);

	// 添加（右单个）
	virtual bool	rPush(const string& strKey, const string& strValue, long *pnVal = nullptr);

	//// 添加（右批量）
	//virtual bool	rPushVec(const string& strKey, const vector<string>& vecValue);

	// 添加（插入，strPos:"before","after"）
	virtual bool	lInsert(const string& strKey, const string& strPos, const string& strPivot, const string& strValue);

	// 删除（左弹出）
	virtual bool	lPopString(const string& strKey, string* pstrValue);

	// 删除（右弹出）
	virtual bool	rPopString(const string& strKey, string* pstrValue);

	// 删除（移除，nCount:+n左n个，-n右n个，0所有）
	virtual bool	lRem(const string& strKey, long nCount, const string& strValue, long *pnVal = nullptr);

	// 删除（修剪）
	virtual bool	lTrim(const string& strKey, long nStart, long nStop);

	// 更改
	virtual bool	lSet(const string& strKey, long nIndex, const string& strValue);

	// 获取（区间元素）
	virtual bool	lRangeString(const string& strKey, long nStart, long nStop, vector<string>* pvecValue);

	// 获取（index元素）
	virtual bool	lIndex(const string& strKey, long nIndex, string* pstrValue);

	// 数量
	virtual bool	lLen(const string& strKey, long* pnValue);

private:
	CRedisClient&		m_redis;
};