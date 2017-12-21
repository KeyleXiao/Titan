#pragma once
#include "IBaseValueOp.h"
#include "TFunc.h"


namespace Redis
{
	/*命令分类参考 http://redis.cn/commands.html*/
	// Value操作器(即网址中的Strings)
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