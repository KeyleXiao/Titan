#pragma once


namespace Redis
{
	struct IBaseValueOp
	{
		/* interfaces for string */
		// ���
		virtual bool	set(const string& strKey, const string& strValue) = 0;

		// ��ӣ�long:��Key��ӣ���Key�ӷ���
		virtual bool	incrbyString(const string& strKey, long nIncr, long* pnValue) = 0;

		// ��ӣ�float:��Key��ӣ���Key�ӷ���
		virtual bool	incrbyfloatString(const string& strKey, double dIncr, double* pdValue) = 0;

		template<typename T>
		bool	set(const string& strKey, const T& tValue)
		{
			return set(strKey, TToString(tValue));
		}

	protected:
		// ��ȡ
		virtual bool	getString(const string& strKey, string* pstrValue) = 0;
	};
};