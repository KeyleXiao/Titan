#pragma once
#include "TFunc.h"
#include "IBaseListOp.h"


namespace Redis
{
	/*命令分类参考 http://redis.cn/commands.html*/
	// List操作器
	struct IListOp : public IBaseListOp
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
			string strValue;
			if (!lPopString(strKey, &strValue))
				return false;

			TFromString(strValue, tValue);
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
			string strValue;
			if (!rPopString(strKey, &strValue))
				return false;

			TFromString(strValue, tValue);
			return true;
		}

		//************************************
		// Returns:   bool	是否取到元素
		// Qualifier: 从List左侧弹出一个元素并删除(如果没有元素，则阻塞的等)
		// Parameter: const std::string & strKey
		// Parameter: T & tValue	要取的元素
		// Parameter: long nTimeout	超时时间，单位：秒（默认为0，即无限等）
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
		// Returns:   bool	是否取到元素
		// Qualifier: 从List右侧弹出一个元素并删除(如果没有元素，则阻塞的等)
		// Parameter: const std::string & strKey
		// Parameter: T & tValue	要取的元素
		// Parameter: long nTimeout	超时时间，单位：秒（默认为0，即无限等）
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
		// Returns:   bool	是否取到元素
		// Qualifier: 从List左侧弹出一个元素并删除(如果没有元素，则阻塞的等)
		// Parameter: const std::vector<std::string> & vecKey	按给定 key 参数排列的先后顺序，依次检查各个List。
		// Parameter: T & tValue	要取的元素
		// Parameter: long nTimeout	超时时间，单位：秒（默认为0，即无限等）
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
		// Returns:   bool	是否取到元素
		// Qualifier: 从List右侧弹出一个元素并删除(如果没有元素，则阻塞的等)
		// Parameter: const std::vector<std::string> & vecKey	按给定 key 参数排列的先后顺序，依次检查各个List。
		// Parameter: T & tValue	要取的元素
		// Parameter: std::string * pStrKey	弹出元素的那个List的key
		// Parameter: long nTimeout	超时时间，单位：秒（默认为0，即无限等）
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
		// Returns:   bool	是否取到元素
		// Qualifier: 左弹出并删除(如果没有元素，则阻塞的等)
		// Parameter: const std::string & strKey
		// Parameter: std::vector<std::string> * pvecVal
		// Parameter: long nTimeout		超时时间，单位：秒（默认为0，即无限等）
		//************************************
		virtual bool	Blpop(const std::string &strKey, std::vector<std::string> *pvecVal, long nTimeout = 0) = 0;

		//************************************
		// Returns:   bool	是否取到元素
		// Qualifier: 右弹出并删除(如果没有元素，则阻塞的等)
		// Parameter: const std::string & strKey
		// Parameter: std::vector<std::string> * pvecVal
		// Parameter: long nTimeout		超时时间，单位：秒（默认为0，即无限等）
		//************************************
		virtual bool	Brpop(const std::string &strKey, std::vector<std::string> *pvecVal, long nTimeout = 0) = 0;

		// 阻塞删除（左弹出）
		virtual bool	Blpop(const std::vector<std::string> &vecKey, std::vector<std::string> *pvecVal, long nTimeout = 0) = 0;

		// 阻塞删除（右弹出）
		virtual bool	Brpop(const std::vector<std::string> &vecKey, std::vector<std::string> *pvecVal, long nTimeout = 0) = 0;
	};
};