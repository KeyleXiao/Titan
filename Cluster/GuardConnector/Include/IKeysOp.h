#pragma once
#include "TFunc.h"


namespace Redis
{
	/*命令分类参考 http://redis.cn/commands.html*/
	// Keys指令操作器
	struct IKeysOp
	{
		// 删除
		virtual bool	delKey(const string& strKey) = 0;

		// 更改
		virtual bool	renameKey(const string& strKey, const string& strNewKey) = 0;

		// 存在
		virtual bool	existKey(const string& strKey, bool* pbExist) = 0;
	};

};