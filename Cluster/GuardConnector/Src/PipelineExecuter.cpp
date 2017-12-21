#include "stdafx.h"
#include "PipelineExecuter.h"
#include "redis.h"
#include "ValueOp.h"
#include "ListOp.h"
#include "PipelineValueOp.h"
#include "PipelineListOp.h"


PipelineExecuter::PipelineExecuter(CRedisClient& redis)
	: m_redis(redis)
{
	m_pPipeline = m_redis.CreatePipeline();

	m_pValueOp = new CPipelineValueOp(m_redis, m_pPipeline);
	m_pListOp = new CPipelineListOp(m_redis, m_pPipeline);
}
PipelineExecuter::~PipelineExecuter()
{
	m_redis.FreePipeline(m_pPipeline);
	safeDelete(m_pValueOp);
	safeDelete(m_pListOp);
}

bool PipelineExecuter::pipeEnd()
{
	// ִ��ָ��
	m_redis.FlushPipeline(m_pPipeline);
	// ȡ�����string��
	m_redis.FetchReplys(m_pPipeline);

	// ���ת��
	m_pValueOp->converter();
	m_pListOp->converter();

	return true;
}