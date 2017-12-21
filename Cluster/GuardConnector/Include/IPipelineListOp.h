#pragma once
#include "IBaseListOp.h"
#include "IPipelineOp.h"



namespace Redis
{
	/*�������ο� http://redis.cn/commands.html*/
	// Pipeline��List������
	struct IPipelineListOp : public IBaseListOp, public PipelineOp
	{
		//************************************
		// Returns:   bool	�Ƿ�ɹ�
		// Qualifier: ��List��൯��һ��Ԫ�ز�ɾ��
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
		// Returns:   bool	�Ƿ�ɹ�
		// Qualifier: ��List�Ҳ൯��һ��Ԫ�ز�ɾ��
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