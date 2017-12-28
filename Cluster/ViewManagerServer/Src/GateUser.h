/*******************************************************************
** �ļ���:	GateUser.h
** ��  Ȩ:	(C) ���ڱ�������ɷ����޹�˾
** ������:	baoliang.shen
** ��  ��:	2017/12/01
** ��  ��:	1.0
** ��  ��:	Gate���Ӷ���
** Ӧ  ��:  	
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/
#pragma once
#include "SessionUser.h"


class GateUser : public SessionUser<GateUser, GateID>
{
public:
	GateUser(IConnection * conn, IUserList<GateID>& pUserList, Processer<GateUser>& processer, TimerAxis& timerAxis, IByteRecord* pByteRecord);
	virtual ~GateUser() {}

	inline	DWORD	GetMaxCount() const { return m_dwMaxCount; }
	inline	void	SetMaxCount(DWORD dwMaxCount) { m_dwMaxCount = dwMaxCount; }

	inline	DWORD	GetCurrCount() const { return m_dwCurrCount; }
	inline	void	SetCurrCount(DWORD val) { m_dwCurrCount = val; }

	inline	WORD	GetPortForView() const { return m_wPortForView; }
	inline	void	SetPortForView(WORD val) { m_wPortForView = val; }

	inline	WORD	GetPortForClient() { return m_wClientPort; }
	inline	void	SetPortForClient(WORD wClientPort) { m_wClientPort = wClientPort; }

	// �Ƿ�׼���ö��������
	inline	bool	IsReady() { return GetMaxCount() > 0; }

protected:
	virtual std::string ToString();

	virtual WORD		GetKeepAliveID();

	virtual	const obuf&	GetAnswerKeepAliveMsg();

private:
	DWORD	m_dwMaxCount;	// �ܳ��ص��������
	DWORD	m_dwCurrCount;	// ��ǰ����
	WORD	m_wPortForView;	// ��Gate����ViewServer�Ķ˿�
	WORD	m_wClientPort;	// ��Gate����Client�Ķ˿�

};