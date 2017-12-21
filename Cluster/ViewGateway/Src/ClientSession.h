/*******************************************************************
** �ļ���:	ClientSession.h
** ��  Ȩ:	(C) ���ڱ�������ɷ����޹�˾
** ������:	baoliang.shen
** ��  ��:	2017/12/06
** ��  ��:	1.0
** ��  ��:	Gate�ϵ�Client���Ӷ���
** Ӧ  ��:  	
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/
#pragma once
#include "SessionUser.h"


class ClientSession : public SessionUser<ClientSession>
{
public:
	ClientSession(IConnection * conn, IUserList& pUserList, Processer<ClientSession>& processer, TimerAxis& timerAxis, IByteRecord* pByteRecord);
	virtual ~ClientSession();

	inline	time_t		GetConnectTime(void) { return m_tConnectTime; }

	virtual std::string ToString();

	// ����Mac��ַ����Ȼ��ʱ��֪����ʲô��
	void SetMacAddress(char szMacAddress[]);

protected:
	virtual WORD		GetKeepAliveID();

	virtual	const obuf&	GetAnswerKeepAliveMsg();

protected:
	time_t		m_tConnectTime;						//	����ʱ��
	char		m_szMacAddress[MAC_ADDRESS_SIZE];	// ������ַ
};