#ifndef __REDIS_CONNECTOR_H__
#define __REDIS_CONNECTOR_H__

#include "IRedisConnector.h"
#include "redis.h"
#include "Setting.h"
using namespace Redis;

class RedisConnector : public IRedisConnector
{
public:
	/////////////////// RedisConnector ///////////////////////////////////////////
	// 构造
	RedisConnector();

	// 析构
	~RedisConnector();

	/////////////////// IRedisConnector ///////////////////////////////////////////
	// 创建
	virtual bool			create(const char* ip = "127.0.0.1", WORD port = 6379);

	// 释放
	virtual void			release();

	// 关闭
	virtual void			stop();

	// 获取Value操作器
	virtual IValueOp*		valueOp() { return m_pValueOp; }
	// 获取List操作器
	virtual IListOp*		listOp() { return m_pListOp; }
	//// 获取Set操作器
	//virtual ISetOp*			setOp();
	//// 获取Hash操作器
	//virtual IHashOp*		hashOp();
	// 获取其他指令操作器
	virtual IKeysOp*		keysOp() { return m_pKeysOp; }
	// Server指令操作器
	virtual	IServerOp*		serverOp() { return m_pServerOp; }

	//virtual IPipeline*		pipeline() { return m_pPipelineOp; }

	/* interfaces for hash */
	// 添加（单对）
	virtual bool	setHash(const string& strKey, const string& strField, const string& strValue);

	// 添加（多对）
	virtual bool	mSetVecHash(const string& strKey, const vector<string>& vecField, const vector<string>& vecValue);

	// 添加（多对）
	virtual bool	mSetMapHash(const string& strKey, const map<string, string>& mapFv);

	// 添加（long:无Field添加，有Field加和）
	virtual bool	incrbyHash(const string& strKey, const string& strField, long nIncr, long * pnValue);

	// 添加（float:无Field添加，有Field加和）
	virtual bool	incrbyfloatHash(const string& strKey, const string& strField, double dIncr, double * pdValue);

	// 删除
	virtual bool	delHash(const string& strKey, const string& strField);

	// 获取（单对）
	virtual bool	getHash(const string& strKey, const string& strField, string * pstrValue);

	// 获取（多对，vector）
	virtual bool	mGetVecHash(const string& strKey, const vector<string>& vecField, vector<string> * pvecValue);

	// 获取（多对，map）
	virtual bool	mGetMapHash(const string& strKey, const vector<string>& vecField, map<string, string> * pmapFv);

	// 获取（所有Keys）
	virtual bool	keysHash(const string& strKey, vector<string>* pvecField);

	// 获取（所有Values）
	virtual bool	valuesHash(const string& strKey, vector<string>* pvecValue);

	// 获取（所有键值对）
	virtual bool	getAllHash(const string& strKey, map<string, string>* pmapFv);

	// 存在
	virtual bool	existHash(const string& strKey, const string& strField, bool* pbValue);

	// 数量
	virtual bool	lenHash(const string& strKey, long* pnValue);


	/* interfaces for set */
	// 添加
	virtual bool	addSet(const string& strKey, const string& strValue);

	// 删除（单个）
	virtual bool	remSet(const string& strKey, const string& strValue);

	// 删除（多个）
	virtual bool	remSet(const string& strKey, const vector<string>& vecValue);

	// 删除（随机）
	virtual bool	popSet(const string& strKey, string* pstrValue);

	// 获取（所有）
	virtual bool	membersSet(const string& strKey, vector<string>* pvecValue);

	//// 获取（随机）
	//virtual bool	randMemberSet(const string& strKey, long nCount, vector<string>* pvecValue);

	// 存在
	virtual bool	isMemberSet(const string& strKey, const string& strValue, bool* pbValue);

	// 数量
	virtual bool	cardSet(const string& strKey, long* pnValue);


	///* interfaces for sorted set */
	//// 添加
	//virtual bool	addZset(const string& strKey, double dScore, const string& strElem);
	//// 添加（无Elem添加，有Elem加和）
	//virtual bool	incrbyZset(const string& strKey, double dIncr, const string& strElem);
	//// 删除（单个）
	//virtual bool	remZset(const string& strKey, const string& strElem);
	//// 删除（多个）
	//virtual bool	remZset(const string& strKey, const vector<string>& vecElem);
	//// 删除（元素区间）
	//virtual bool	remRangeByLexZset(const string& strKey, const string& strMin, const string& strMax);
	//// 删除（名次区间）
	//virtual bool	remRangeByRankZset(const string& strKey, long nStart, long nStop);
	//// 删除（分数区间）
	//virtual bool	remRangeByScoreZset(const string& strKey, double dMin, double dMax);
	//// 获取（名次区间，升序，vec）
	//virtual bool	rangeZset(const string& strKey, long nStart, long nStop, vector<string>* pvecValue);
	//// 获取（名次区间，升序，map）
	//virtual bool	rangeZset(const string& strKey, long nStart, long nStop, map<string, string>* pmapValue);
	//// 获取（元素区间，升序，vec）
	//virtual bool	rangeByLexZset(const string& strKey, const string& strMin, const string& strMax, vector<string>* pvecValue);
	//// 获取（分数区间，升序，vec）
	//virtual bool	rangeByScoreZset(const string& strKey, double dMin, double dMax, vector<string>* pvecValue);
	//// 获取（分数区间，升序，map）
	//virtual bool	rangeByScoreZset(const string& strKey, double dMin, double dMax, map<string, string>* pmapValue);
	//// 获取（名次区间，降序，vec）
	//virtual bool	revRangeZset(const string& strKey, long nStart, long nStop, vector<string>* pvecValue);
	//// 获取（分数区间，降序，vec）
	//virtual bool	revRangeByScoreZset(const string& strKey, double dMax, double dMin, vector<string>* pvecValue);
	//// 获取（分数区间，降序，map）
	//virtual bool	revRangeByScoreZset(const string& strKey, double dMax, double dMin, map<string, string>* pmapValue);
	//// 数量（全部）
	//virtual bool	cardZset(const string& strKey, long* pnValue);
	//// 数量（分数区间）
	//virtual bool	countZset(const string& strKey, double dMin, double dMax, long* pnValue);
	//// 数量（元素区间）
	//virtual bool	countByLexZset(const string& strKey, const string& strMin, const string& strMax, long* pnValue);
	//// 名次（升序）
	//virtual bool	rankZset(const string& strKey, const string& strElem, long* pnValue);
	//// 名次（降序）
	//virtual bool	revRankZset(const string& strKey, const string& strElem, long* pnValue);
	//// 分数
	//virtual bool	scoreZset(const string& strKey, const string& strElem, double* pdValue);

protected:
	/* interfaces for Pipeline */
	// 在执行Pipline操作之前的准备工作
	virtual IPipeline*	pipeBegin();
	// 在执行Pipline操作之后的工作
	virtual bool		pipeEnd(IPipeline* pPipe);


private:
	CRedisClient m_redis;

	// Value操作器
	IValueOp*		m_pValueOp;
	// List操作器
	IListOp*		m_pListOp;
	//// Set操作器
	//ISetOp*			m_pSetOp;
	//// Hash操作器
	//IHashOp*		m_pHashOp;
	// 其他指令操作器
	IKeysOp*		m_pKeysOp;
	// Server指令操作器
	IServerOp*		m_pServerOp;

	CSetting		m_Setting;
};


#endif	//__REDIS_CONNECTOR_H__