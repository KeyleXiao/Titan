#pragma once
#include "TFunc.h"


namespace Redis
{
	/*命令分类参考 http://redis.cn/commands.html*/
	// Server指令操作器
	struct IServerOp
	{
		// 清空所有数据库 (确实要删除所有Key时才用！！！不了解严重性者可以参考：“从删库到跑路”...)
		virtual	bool	FlushAll() = 0;

		// 清空当前数据库 (确实要删除所有Key时才用！！！不了解严重性者可以参考：“从删库到跑路”...)
		virtual	bool	FlushDb() = 0;
	};

};