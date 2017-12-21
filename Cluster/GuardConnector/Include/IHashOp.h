#pragma once
#include "TFunc.h"



namespace Redis
{
	// Hash操作器
	struct IHashOp
	{
		/* interfaces for hash */
		// 添加（单对）
		virtual bool	setHash(const string& strKey, const string& strField, const string& strValue) = 0;

		// 添加（多对）
		virtual bool	mSetVecHash(const string& strKey, const vector<string>& vecField, const vector<string>& vecValue) = 0;

		// 添加（多对）
		virtual bool	mSetMapHash(const string& strKey, const map<string, string>& mapFv) = 0;

		// 添加（long:无Field添加，有Field加和）
		virtual bool	incrbyHash(const string& strKey, const string& strField, long nIncr, long * pnValue) = 0;

		// 添加（float:无Field添加，有Field加和）
		virtual bool	incrbyfloatHash(const string& strKey, const string& strField, double dIncr, double * pdValue) = 0;

		// 删除
		virtual bool	delHash(const string& strKey, const string& strField) = 0;

		// 获取（单对）
		virtual bool	getHash(const string& strKey, const string& strField, string * pstrValue) = 0;

		// 获取（多对，vector）
		virtual bool	mGetVecHash(const string& strKey, const vector<string>& vecField, vector<string> * pvecValue) = 0;

		// 获取（多对，map）
		virtual bool	mGetMapHash(const string& strKey, const vector<string>& vecField, map<string, string> * pmapFv) = 0;

		// 获取（所有Keys）
		virtual bool	keysHash(const string& strKey, vector<string>* pvecField) = 0;

		// 获取（所有Values）
		virtual bool	valuesHash(const string& strKey, vector<string>* pvecValue) = 0;

		// 获取（所有键值对）
		virtual bool	getAllHash(const string& strKey, map<string, string>* pmapFv) = 0;

		// 存在
		virtual bool	existHash(const string& strKey, const string& strField, bool* pbValue) = 0;

		// 数量
		virtual bool	lenHash(const string& strKey, long* pnValue) = 0;
	};

};