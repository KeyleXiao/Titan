/*******************************************************************
** �ļ���:	Handler.h
** ��  Ȩ:	(C) ���ڱ�������ɷ����޹�˾
** ������:	baoliang.shen
** ��  ��:	2017/11/27
** ��  ��:	1.0
** ��  ��:	�������Ϣ��������̳д˻���
** Ӧ  ��:  	
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/
#pragma once
#include "IHandler.h"
#include "ISerializableData.h"

// TYPEΪ���ӵĳ����ߣ�MsgTΪҪ�������Ϣ��(MsgT�в������麯��)
template <class TYPE, class MsgT>
class Handler : public IHandler<TYPE>
{
public:
	// ģ���
	virtual	BYTE	GetModuleId() { return MsgT::GetModuleId(); }
	// ��Ϣ��
	virtual	BYTE	GetActionId() { return MsgT::GetActionId(); }

	virtual void	_Handle(TYPE* pSession, SGameMsgHead* head, void* data, size_t len)
	{
		Assert(pSession);
		Assert(head);
		Assert(data);
		if (pSession==NULL || head ==NULL || data == NULL)
			return;

		if (std::is_base_of<ISerializableData, MsgT>::value)
		{
			/*
			����������쳣�������¿��ܣ�
			1.toBytes��fromBytes����Ĵ���д���ˣ�����Ѿ���Ϊ�ɹ������ɴ��룬�򲻿���д����
			2.�Է������Ķ�����Ϣ��
			��ˣ����￼�Ǽ�try...catch	TODO
			*/
			MsgT tMsgT;
			ISerializableData* pISD = (ISerializableData*)(&tMsgT);
			pISD->fromBytes((char*)data, len);
			Handle(pSession, head, &tMsgT);
		}
		else
		{
			MsgT * pMsg = (MsgT *)(data);	// ֻҪ������MsgT���麯����������ǰ�ȫ��
			if (len != sizeof(MsgT))
			{
				Assert(len == sizeof(MsgT));
				return;
			}
			Handle(pSession, head, pMsg);
		}
	}

protected:
	//************************************
	// Method:    Handle
	// FullName:  ModuleViewHandler<TYPE, MsgT>::Handle
	// Access:    virtual protected 
	// Returns:   void
	// Qualifier:	��Щ������ʵ�����У�������Ҫ����NULL�ˣ���Ϊ�������Ѿ��жϹ���
	// Parameter: TYPE * pSession		��Ϣ��Դ�ڴ�����
	// Parameter: SGameMsgHead * head	ͨ����Ϣͷ
	// Parameter: MsgT * pMsg			�������Ϣ����
	//************************************
	virtual void	Handle(TYPE* pSession, SGameMsgHead* head, MsgT* pMsg) = 0;
};