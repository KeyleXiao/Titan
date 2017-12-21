#pragma once
#include "TFunc.h"


namespace Redis
{
	struct IPipelineValueOp;
	struct IPipelineListOp;
	// ��������
	struct IPipeline
	{
		friend struct IRedisConnector;

		// ��ȡValue������
		virtual IPipelineValueOp*	valueOp() = 0;
		// ��ȡList������
		virtual IPipelineListOp*	listOp() = 0;
		//// ��ȡSet������
		//virtual ISetOp*			setOp() = 0;
		//// ��ȡHash������
		//virtual IHashOp*		hashOp() = 0;

	protected:
		// ��ִ��Pipline����֮��Ĺ���
		virtual bool	pipeEnd() = 0;
	};
};