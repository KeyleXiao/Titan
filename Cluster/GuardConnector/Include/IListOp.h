#pragma once
#include "TFunc.h"
#include "IBaseListOp.h"


namespace Redis
{
	/*�������ο� http://redis.cn/commands.html*/
	// List������
	struct IListOp : public IBaseListOp
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
			string strValue;
			if (!lPopString(strKey, &strValue))
				return false;

			TFromString(strValue, tValue);
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
			string strValue;
			if (!rPopString(strKey, &strValue))
				return false;

			TFromString(strValue, tValue);
			return true;
		}

		//************************************
		// Returns:   bool	�Ƿ�ȡ��Ԫ��
		// Qualifier: ��List��൯��һ��Ԫ�ز�ɾ��(���û��Ԫ�أ��������ĵ�)
		// Parameter: const std::string & strKey
		// Parameter: T & tValue	Ҫȡ��Ԫ��
		// Parameter: long nTimeout	��ʱʱ�䣬��λ���루Ĭ��Ϊ0�������޵ȣ�
		//************************************
		template<typename T>
		bool	Blpop(const std::string &strKey, T& tValue, long nTimeout = 0)
		{
			std::vector<std::string> vecVal;
			if (!Blpop(strKey, &vecVal, nTimeout))
				return false;
			if (vecVal.size() != 2)
				return false;

			std::string& strTmp = vecVal.at(1);
			TFromString(strTmp, tValue);
			return true;
		}

		//************************************
		// Returns:   bool	�Ƿ�ȡ��Ԫ��
		// Qualifier: ��List�Ҳ൯��һ��Ԫ�ز�ɾ��(���û��Ԫ�أ��������ĵ�)
		// Parameter: const std::string & strKey
		// Parameter: T & tValue	Ҫȡ��Ԫ��
		// Parameter: long nTimeout	��ʱʱ�䣬��λ���루Ĭ��Ϊ0�������޵ȣ�
		//************************************
		template<typename T>
		bool	Brpop(const std::string &strKey, T& tValue, long nTimeout = 0)
		{
			std::vector<std::string> vecVal;
			if (!Brpop(strKey, &vecVal, nTimeout))
				return false;
			if (vecVal.size() != 2)
				return false;

			std::string& strTmp = vecVal.at(1);
			TFromString(strTmp, tValue);
			return true;
		}

		//************************************
		// Returns:   bool	�Ƿ�ȡ��Ԫ��
		// Qualifier: ��List��൯��һ��Ԫ�ز�ɾ��(���û��Ԫ�أ��������ĵ�)
		// Parameter: const std::vector<std::string> & vecKey	������ key �������е��Ⱥ�˳�����μ�����List��
		// Parameter: T & tValue	Ҫȡ��Ԫ��
		// Parameter: long nTimeout	��ʱʱ�䣬��λ���루Ĭ��Ϊ0�������޵ȣ�
		//************************************
		template<typename T>
		bool	Blpop(const std::vector<std::string> &vecKey, T& tValue, std::string* pStrKey = nullptr, long nTimeout = 0)
		{
			std::vector<std::string> vecVal;
			if (!Blpop(vecKey, &vecVal, nTimeout))
				return false;
			if (vecVal.size() != 2)
				return false;

			std::string& strTmp = vecVal.at(1);
			TFromString(strTmp, tValue);
			if (pStrKey)
				*pStrKey = std::move(vecVal.at(0));

			return true;
		}

		//************************************
		// Returns:   bool	�Ƿ�ȡ��Ԫ��
		// Qualifier: ��List�Ҳ൯��һ��Ԫ�ز�ɾ��(���û��Ԫ�أ��������ĵ�)
		// Parameter: const std::vector<std::string> & vecKey	������ key �������е��Ⱥ�˳�����μ�����List��
		// Parameter: T & tValue	Ҫȡ��Ԫ��
		// Parameter: std::string * pStrKey	����Ԫ�ص��Ǹ�List��key
		// Parameter: long nTimeout	��ʱʱ�䣬��λ���루Ĭ��Ϊ0�������޵ȣ�
		//************************************
		template<typename T>
		bool	Brpop(const std::vector<std::string> &vecKey, T& tValue, std::string* pStrKey = nullptr, long nTimeout = 0)
		{
			std::vector<std::string> vecVal;
			if (!Brpop(vecKey, &vecVal, nTimeout))
				return false;
			if (vecVal.size() != 2)
				return false;

			std::string& strTmp = vecVal.at(1);
			TFromString(strTmp, tValue);
			if (pStrKey)
				*pStrKey = std::move(vecVal.at(0));

			return true;
		}

	protected:
		//************************************
		// Returns:   bool	�Ƿ�ȡ��Ԫ��
		// Qualifier: �󵯳���ɾ��(���û��Ԫ�أ��������ĵ�)
		// Parameter: const std::string & strKey
		// Parameter: std::vector<std::string> * pvecVal
		// Parameter: long nTimeout		��ʱʱ�䣬��λ���루Ĭ��Ϊ0�������޵ȣ�
		//************************************
		virtual bool	Blpop(const std::string &strKey, std::vector<std::string> *pvecVal, long nTimeout = 0) = 0;

		//************************************
		// Returns:   bool	�Ƿ�ȡ��Ԫ��
		// Qualifier: �ҵ�����ɾ��(���û��Ԫ�أ��������ĵ�)
		// Parameter: const std::string & strKey
		// Parameter: std::vector<std::string> * pvecVal
		// Parameter: long nTimeout		��ʱʱ�䣬��λ���루Ĭ��Ϊ0�������޵ȣ�
		//************************************
		virtual bool	Brpop(const std::string &strKey, std::vector<std::string> *pvecVal, long nTimeout = 0) = 0;

		// ����ɾ�����󵯳���
		virtual bool	Blpop(const std::vector<std::string> &vecKey, std::vector<std::string> *pvecVal, long nTimeout = 0) = 0;

		// ����ɾ�����ҵ�����
		virtual bool	Brpop(const std::vector<std::string> &vecKey, std::vector<std::string> *pvecVal, long nTimeout = 0) = 0;
	};
};