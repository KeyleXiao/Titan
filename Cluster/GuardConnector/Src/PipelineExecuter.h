#pragma once
#include "IRedisConnector.h"


using namespace Redis;
class CRedisClient;

class PipelineExecuter : public IPipeline
{
	//// ����Operationö���������������������������±�
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
	/*����Pipeline�õ�Op����ȻҲ������ʹ�ã���������ʹ�õ�ָ��Ͳ�һ��������Pipeline����*/
	// ��ȡValue������
	virtual IPipelineValueOp*	valueOp() { return m_pValueOp; }
	// ��ȡList������
	virtual IPipelineListOp*	listOp() { return m_pListOp; }

	//// ��ȡSet������
	//virtual ISetOp*			setOp();
	//// ��ȡHash������
	//virtual IHashOp*		hashOp();
	///////////////////////////////////////////////////////////////////////

protected:
	virtual bool	pipeEnd();

private:
	CRedisClient&		m_redis;
	Pipeline			m_pPipeline;

	// Value������
	IPipelineValueOp*	m_pValueOp;
	// List������
	IPipelineListOp*	m_pListOp;
	//// Set������
	//ISetOp*			m_pSetOp;
	//// Hash������
	//IHashOp*		m_pHashOp;
};