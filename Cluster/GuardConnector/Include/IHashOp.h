#pragma once
#include "TFunc.h"



namespace Redis
{
	// Hash������
	struct IHashOp
	{
		/* interfaces for hash */
		// ��ӣ����ԣ�
		virtual bool	setHash(const string& strKey, const string& strField, const string& strValue) = 0;

		// ��ӣ���ԣ�
		virtual bool	mSetVecHash(const string& strKey, const vector<string>& vecField, const vector<string>& vecValue) = 0;

		// ��ӣ���ԣ�
		virtual bool	mSetMapHash(const string& strKey, const map<string, string>& mapFv) = 0;

		// ��ӣ�long:��Field��ӣ���Field�Ӻͣ�
		virtual bool	incrbyHash(const string& strKey, const string& strField, long nIncr, long * pnValue) = 0;

		// ��ӣ�float:��Field��ӣ���Field�Ӻͣ�
		virtual bool	incrbyfloatHash(const string& strKey, const string& strField, double dIncr, double * pdValue) = 0;

		// ɾ��
		virtual bool	delHash(const string& strKey, const string& strField) = 0;

		// ��ȡ�����ԣ�
		virtual bool	getHash(const string& strKey, const string& strField, string * pstrValue) = 0;

		// ��ȡ����ԣ�vector��
		virtual bool	mGetVecHash(const string& strKey, const vector<string>& vecField, vector<string> * pvecValue) = 0;

		// ��ȡ����ԣ�map��
		virtual bool	mGetMapHash(const string& strKey, const vector<string>& vecField, map<string, string> * pmapFv) = 0;

		// ��ȡ������Keys��
		virtual bool	keysHash(const string& strKey, vector<string>* pvecField) = 0;

		// ��ȡ������Values��
		virtual bool	valuesHash(const string& strKey, vector<string>* pvecValue) = 0;

		// ��ȡ�����м�ֵ�ԣ�
		virtual bool	getAllHash(const string& strKey, map<string, string>* pmapFv) = 0;

		// ����
		virtual bool	existHash(const string& strKey, const string& strField, bool* pbValue) = 0;

		// ����
		virtual bool	lenHash(const string& strKey, long* pnValue) = 0;
	};

};