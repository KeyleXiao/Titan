/*******************************************************************
** �ļ���:	GameUser.h
** ��  Ȩ:	(C) ���ڱ�������ɷ����޹�˾
** ������:	baoliang.shen
** ��  ��:	2017/12/08
** ��  ��:	1.0
** ��  ��:	��Ϸ������
** Ӧ  ��:  	
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/
#pragma once
#include "SessionUser.h"



class GameUser : public SessionUser<GameUser, GameID>
{
public:
	GameUser(IConnection * conn, IUserList<GameID>& pUserList, Processer<GameUser>& processer, TimerAxis& timerAxis, IByteRecord* pByteRecord);
	virtual ~GameUser() {}

protected:
	virtual std::string ToString();

	virtual WORD		GetKeepAliveID();

	virtual	const obuf&	GetAnswerKeepAliveMsg();
};