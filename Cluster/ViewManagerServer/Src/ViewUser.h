/*******************************************************************
** �ļ���:	ViewUser.h
** ��  Ȩ:	(C) ���ڱ�������ɷ����޹�˾
** ������:	baoliang.shen
** ��  ��:	2017/12/01
** ��  ��:	1.0
** ��  ��:	��ս�����Ӷ���
** Ӧ  ��:  	
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/
#pragma once
#include "SessionUser.h"


class ViewUser : public SessionUser<ViewUser>
{
public:
	ViewUser(IConnection * conn, IUserList& pUserList, Processer<ViewUser>& processer, TimerAxis& timerAxis, IByteRecord* pByteRecord);
	virtual ~ViewUser() {}

	DWORD	GetMaxCount() const { return m_dwMaxCount; }
	void	SetMaxCount(DWORD dwMaxCount) { m_dwMaxCount = dwMaxCount; }

	DWORD	GetCurrCount() const { return m_dwCurrCount; }
	void	SetCurrCount(DWORD val) { m_dwCurrCount = val; }

	// �Ƿ�׼���ö��������
	bool	IsReady() { return GetMaxCount() > 0; }
	// �Ƿ�����
	bool	IsFull() { return GetCurrCount() >= GetMaxCount(); }

protected:
	virtual std::string ToString();

	virtual WORD		GetKeepAliveID();

	virtual	const obuf&	GetAnswerKeepAliveMsg();

private:
	DWORD	m_dwMaxCount;	// �ܳ��ص��������
	DWORD	m_dwCurrCount;	// ��ǰ����
public:
};

