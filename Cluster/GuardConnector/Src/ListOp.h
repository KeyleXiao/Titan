#pragma once
#include "IRedisConnector.h"

class CRedisClient;
using namespace Redis;

class CListOp : public IListOp
{
public:
	CListOp(CRedisClient& redis);

	// ����ɾ�����󵯳���
	virtual bool	Blpop(const std::string &strKey, std::vector<std::string> *pvecVal, long nTimeout = 0);

	// ����ɾ�����󵯳���
	virtual bool	Blpop(const std::vector<std::string> &vecKey, std::vector<std::string> *pvecVal, long nTimeout = 0);

	// ����ɾ�����ҵ�����
	virtual bool	Brpop(const std::string &strKey, std::vector<std::string> *pvecVal, long nTimeout = 0);

	// ����ɾ�����ҵ�����
	virtual bool	Brpop(const std::vector<std::string> &vecKey, std::vector<std::string> *pvecVal, long nTimeout = 0);

	// ��ӣ��󵥸���
	virtual bool	lPush(const string& strKey, const string& strValue, long *pnVal = nullptr);

	//// ��ӣ���������
	//virtual bool	lPushVec(const string& strKey, const vector<string>& vecValue);

	// ��ӣ��ҵ�����
	virtual bool	rPush(const string& strKey, const string& strValue, long *pnVal = nullptr);

	//// ��ӣ���������
	//virtual bool	rPushVec(const string& strKey, const vector<string>& vecValue);

	// ��ӣ����룬strPos:"before","after"��
	virtual bool	lInsert(const string& strKey, const string& strPos, const string& strPivot, const string& strValue);

	// ɾ�����󵯳���
	virtual bool	lPopString(const string& strKey, string* pstrValue);

	// ɾ�����ҵ�����
	virtual bool	rPopString(const string& strKey, string* pstrValue);

	// ɾ�����Ƴ���nCount:+n��n����-n��n����0���У�
	virtual bool	lRem(const string& strKey, long nCount, const string& strValue, long *pnVal = nullptr);

	// ɾ�����޼���
	virtual bool	lTrim(const string& strKey, long nStart, long nStop);

	// ����
	virtual bool	lSet(const string& strKey, long nIndex, const string& strValue);

	// ��ȡ������Ԫ�أ�
	virtual bool	lRangeString(const string& strKey, long nStart, long nStop, vector<string>* pvecValue);

	// ��ȡ��indexԪ�أ�
	virtual bool	lIndex(const string& strKey, long nIndex, string* pstrValue);

	// ����
	virtual bool	lLen(const string& strKey, long* pnValue);

private:
	CRedisClient&		m_redis;
};