/*******************************************************************
** �ļ���:	Processer.h
** ��  Ȩ:	(C) ���ڱ�������ɷ����޹�˾
** ������:	baoliang.shen
** ��  ��:	2017/11/27
** ��  ��:	1.0
** ��  ��:	ͨ�õ���Ϣ�ַ���
** Ӧ  ��:  	
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/
#pragma once
#include "IHandler.h"
using namespace std;

union MsgKey
{
	WORD	key;	// ��Map�õ���Key
	BYTE	by[2];	// 0ΪModuleId��1ΪActionId

	MsgKey(BYTE byModuleId, BYTE byActionId)
	{
		by[0] = byModuleId;
		by[1] = byActionId;
	}
};

template <class TYPE> class Processer
{
public:
	~Processer()
	{
		for (auto it = m_map.begin(); it!= m_map.end(); ++it)
		{
			IHandler<TYPE>*& pHandler = it->second;
			safeDelete(pHandler);
		}
		m_map.clear();
	}

	//************************************
	// Method:    Register
	// FullName:  Processer<TYPE>::Register
	// Access:    public 
	// Returns:   void
	// Qualifier: ע����Ϣ
	// Parameter: IHandler<TYPE> * pHandler	��ϢHandler
	//************************************
	void Register(IHandler<TYPE>* pHandler)
	{
		Assert(pHandler);
		MsgKey msgKey(pHandler->GetModuleId(), pHandler->GetActionId());
		m_map[msgKey.key] = pHandler;
	}

	//************************************
	// Method:    Process
	// FullName:  Processer<TYPE>::Process
	// Access:    public 
	// Returns:   void
	// Qualifier: �ַ�������Ϣ
	// Parameter: TYPE * pSession		��Ϣ��Դ�ڴ�����
	// Parameter: LPVOID pData			δ���������Ϣ����
	// Parameter: DWORD dwDataLen		δ���������Ϣ����
	//************************************
	void Process(TYPE* pSession, LPVOID pData, DWORD dwDataLen )
	{
		Assert(pData != NULL);
		SGameMsgHead* head = (SGameMsgHead*)pData;
		uchar* data = (uchar*)pData + sizeof(SGameMsgHead);
		size_t new_len = (size_t)(dwDataLen - sizeof(SGameMsgHead));
		if (new_len == 0) data = 0;

		MsgKey msgKey(head->byKeyModule, head->byKeyAction);
		auto it = m_map.find(msgKey.key);
		if (it == m_map.end())
		{
			ErrorLn(_GT("δע�����Ϣ�ţ�actionId = ") << msgKey.key);
			return;
		}
		IHandler<TYPE>* pHandler = it->second;
		pHandler->_Handle(pSession, head, data, new_len);
	}

private:
	stdext::hash_map<WORD, IHandler<TYPE>*> m_map;
};
