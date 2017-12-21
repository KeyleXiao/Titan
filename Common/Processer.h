/*******************************************************************
** 文件名:	Processer.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/11/27
** 版  本:	1.0
** 描  述:	通用的消息分发器
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "IHandler.h"
using namespace std;

union MsgKey
{
	WORD	key;	// 在Map用到的Key
	BYTE	by[2];	// 0为ModuleId，1为ActionId

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
	// Qualifier: 注册消息
	// Parameter: IHandler<TYPE> * pHandler	消息Handler
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
	// Qualifier: 分发处理消息
	// Parameter: TYPE * pSession		消息来源于此连接
	// Parameter: LPVOID pData			未经处理的消息数据
	// Parameter: DWORD dwDataLen		未经处理的消息长度
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
			ErrorLn(_GT("未注册的消息号，actionId = ") << msgKey.key);
			return;
		}
		IHandler<TYPE>* pHandler = it->second;
		pHandler->_Handle(pSession, head, data, new_len);
	}

private:
	stdext::hash_map<WORD, IHandler<TYPE>*> m_map;
};
