/*******************************************************************
** �ļ���:	ViewSession.h
** ��  Ȩ:	(C) ���ڱ�������ɷ����޹�˾
** ������:	baoliang.shen
** ��  ��:	2017/12/04
** ��  ��:	1.0
** ��  ��:	Gate�ϵ�ViewServer���Ӷ���
** Ӧ  ��:  	
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/
#pragma once
#include "SessionUser.h"

class ViewSession :	public SessionUser<ViewSession, ViewID>
{
public:
	ViewSession(IConnection * conn, IUserList<ViewID>& pUserList, Processer<ViewSession>& processer, TimerAxis& timerAxis, IByteRecord* pByteRecord);
	virtual ~ViewSession();

	virtual std::string ToString() override;

	// ����δע�����Ϣ������Ҫת������Ϣ��
	virtual	void		UnHandled(LPVOID pData, DWORD dwDataLen) override;

protected:
	virtual WORD		GetKeepAliveID();

	virtual	const obuf&	GetAnswerKeepAliveMsg();
};

