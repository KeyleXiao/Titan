#pragma once
#include "IBaseValueOp.h"
#include "IPipelineOp.h"

namespace Redis
{
	/*�������ο� http://redis.cn/commands.html*/
	// Pipeline��Value������(����ַ�е�Strings)
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