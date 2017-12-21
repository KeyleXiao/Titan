#pragma once


namespace Redis
{
	struct IBaseValueOp
	{
		/* interfaces for string */
		// 添加
		virtual bool	set(const string& strKey, const string& strValue) = 0;

		// 添加（long:无Key添加，有Key加法）
		virtual bool	incrbyString(const string& strKey, long nIncr, long* pnValue) = 0;

		// 添加（float:无Key添加，有Key加法）
		virtual bool	incrbyfloatString(const string& strKey, double dIncr, double* pdValue) = 0;

		template<typename T>
		bool	set(const string& strKey, const T& tValue)
		{
			return set(strKey, TToString(tValue));
		}

	protected:
		// 获取
		virtual bool	getString(const string& strKey, string* pstrValue) = 0;
	};
};