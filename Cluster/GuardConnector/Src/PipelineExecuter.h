#pragma once
#include "IRedisConnector.h"


using namespace Redis;
class CRedisClient;

class PipelineExecuter : public IPipeline
{
	//// 各种Operation枚举索引，用来在容器中做数组下标
	//enum
	//{
	//	E_OP_VALUE = 0,
	//	E_OP_LIST,
	//	E_OP_HASH,
	//	E_OP_SET,

	//	E_OP_MAX,
	//};
public:
	PipelineExecuter(CRedisClient& redis);
	virtual ~PipelineExecuter();

	///////////////////////////////////////////////////////////////////////
	/*允许Pipeline用的Op，自然也允许单独使用；但允许单独使用的指令，就不一定允许在Pipeline中用*/
	// 获取Value操作器
	virtual IPipelineValueOp*	valueOp() { return m_pValueOp; }
	// 获取List操作器
	virtual IPipelineListOp*	listOp() { return m_pListOp; }

	//// 获取Set操作器
	//virtual ISetOp*			setOp();
	//// 获取Hash操作器
	//virtual IHashOp*		hashOp();
	///////////////////////////////////////////////////////////////////////

protected:
	virtual bool	pipeEnd();

private:
	CRedisClient&		m_redis;
	Pipeline			m_pPipeline;

	// Value操作器
	IPipelineValueOp*	m_pValueOp;
	// List操作器
	IPipelineListOp*	m_pListOp;
	//// Set操作器
	//ISetOp*			m_pSetOp;
	//// Hash操作器
	//IHashOp*		m_pHashOp;
};