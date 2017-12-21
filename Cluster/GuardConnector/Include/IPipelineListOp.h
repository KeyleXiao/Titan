#pragma once
#include "IBaseListOp.h"
#include "IPipelineOp.h"



namespace Redis
{
	/*命令分类参考 http://redis.cn/commands.html*/
	// Pipeline的List操作器
	struct IPipelineListOp : public IBaseListOp, public PipelineOp
	{
		//************************************
		// Returns:   bool	是否成功
		// Qualifier: 从List左侧弹出一个元素并删除
		// Parameter: const string & strKey
		// Parameter: T & tValue
		//************************************
		template<typename T>
		bool	lPop(const string& strKey, T& tValue)
		{
			Converter<T>* pConverter = new Converter<T>(tValue);
			if (!lPopString(strKey, &pConverter->m_strValue))
				return false;

			pushConverter(pConverter);
			return true;
		}

		//************************************
		// Returns:   bool	是否成功
		// Qualifier: 从List右侧弹出一个元素并删除
		// Parameter: const string & strKey
		// Parameter: T & tValue
		//************************************
		template<typename T>
		bool	rPop(const string& strKey, T& tValue)
		{
			Converter<T>* pConverter = new Converter<T>(tValue);
			if (!rPopString(strKey, &pConverter->m_strValue))
				return false;

			pushConverter(pConverter);
			return true;
		}
	};
};