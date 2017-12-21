#pragma once
#include "TFunc.h"


namespace Redis
{
	/*�������ο� http://redis.cn/commands.html*/
	// Keysָ�������
	struct IKeysOp
	{
		// ɾ��
		virtual bool	delKey(const string& strKey) = 0;

		// ����
		virtual bool	renameKey(const string& strKey, const string& strNewKey) = 0;

		// ����
		virtual bool	existKey(const string& strKey, bool* pbExist) = 0;
	};

};