#pragma once
#include "TFunc.h"


namespace Redis
{
	// Set������
	struct ISetOp
	{
		/* interfaces for set */
		// ���
		virtual bool	addSet(const string& strKey, const string& strValue) = 0;

		// ɾ����������
		virtual bool	remSet(const string& strKey, const string& strValue) = 0;

		// ɾ���������
		virtual bool	remSet(const string& strKey, const vector<string>& vecValue) = 0;

		// ɾ���������
		virtual bool	popSet(const string& strKey, string* pstrValue) = 0;

		// ��ȡ�����У�
		virtual bool	membersSet(const string& strKey, vector<string>* pvecValue) = 0;

		//// ��ȡ�������
		//virtual bool	randMemberSet(const string& strKey, long nCount, vector<string>* pvecValue) = 0;

		// ����
		virtual bool	isMemberSet(const string& strKey, const string& strValue, bool* pbValue) = 0;

		// ����
		virtual bool	cardSet(const string& strKey, long* pnValue) = 0;
	};

};