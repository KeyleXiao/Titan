#pragma once
#include "TFunc.h"


namespace Redis
{
	// Set操作器
	struct ISetOp
	{
		/* interfaces for set */
		// 添加
		virtual bool	addSet(const string& strKey, const string& strValue) = 0;

		// 删除（单个）
		virtual bool	remSet(const string& strKey, const string& strValue) = 0;

		// 删除（多个）
		virtual bool	remSet(const string& strKey, const vector<string>& vecValue) = 0;

		// 删除（随机）
		virtual bool	popSet(const string& strKey, string* pstrValue) = 0;

		// 获取（所有）
		virtual bool	membersSet(const string& strKey, vector<string>* pvecValue) = 0;

		//// 获取（随机）
		//virtual bool	randMemberSet(const string& strKey, long nCount, vector<string>* pvecValue) = 0;

		// 存在
		virtual bool	isMemberSet(const string& strKey, const string& strValue, bool* pbValue) = 0;

		// 数量
		virtual bool	cardSet(const string& strKey, long* pnValue) = 0;
	};

};