#pragma once
#include "IBaseValueOp.h"
#include "IPipelineOp.h"

namespace Redis
{
	/*命令分类参考 http://redis.cn/commands.html*/
	// Pipeline的Value操作器(即网址中的Strings)
	struct IPipelineValueOp : public IBaseValueOp, public PipelineOp
	{
		template<typename T>
		bool	get(const string& strKey, T& tValue)
		{
			Converter<T>* pConverter = new Converter<T>(tValue);
			if (!getString(strKey, &(pConverter->m_strValue)))
				return false;

			pushConverter(pConverter);
			return true;
		}
	};
};