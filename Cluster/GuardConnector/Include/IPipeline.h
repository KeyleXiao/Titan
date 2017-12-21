#pragma once
#include "TFunc.h"


namespace Redis
{
	struct IPipelineValueOp;
	struct IPipelineListOp;
	// 批量操作
	struct IPipeline
	{
		friend struct IRedisConnector;

		// 获取Value操作器
		virtual IPipelineValueOp*	valueOp() = 0;
		// 获取List操作器
		virtual IPipelineListOp*	listOp() = 0;
		//// 获取Set操作器
		//virtual ISetOp*			setOp() = 0;
		//// 获取Hash操作器
		//virtual IHashOp*		hashOp() = 0;

	protected:
		// 在执行Pipline操作之后的工作
		virtual bool	pipeEnd() = 0;
	};
};