/*******************************************************************
** �ļ���:	IHandler.h
** ��  Ȩ:	(C) ���ڱ�������ɷ����޹�˾
** ������:	baoliang.shen
** ��  ��:	2017/11/27
** ��  ��:	1.0
** ��  ��:	��Ϣ����ӿ�
** Ӧ  ��:  	
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/
#pragma once
#include "DGlobalMessage.h"

template <class TYPE>
class IHandler
{
public:
	// ģ���
	virtual	BYTE	GetModuleId() = 0;
	// ��Ϣ��
	virtual	BYTE	GetActionId() = 0;
	// �ַ������ʹ�õĽӿڣ�ʹ���߲���Ҫʹ��
	virtual	void	_Handle(TYPE* pSession, SGameMsgHead* head, void* data, size_t len) = 0;
};
