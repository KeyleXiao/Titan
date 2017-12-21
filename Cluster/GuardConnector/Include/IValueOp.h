#pragma once
#include "IBaseValueOp.h"
#include "TFunc.h"


namespace Redis
{
	/*�������ο� http://redis.cn/commands.html*/
	// Value������(����ַ�е�Strings)
	struct IValueOp : public IBaseValueOp
	{
		template<typename T>
		bool	get(const string& strKey, T& tValue)
		{
			string strValue;
			if (!getString(strKey, &strValue))
				return false;

			TFromString(strValue, tValue);
			return true;
		}
	};

};