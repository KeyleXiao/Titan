/*******************************************************************
** 文件名:	GameUser.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/12/08
** 版  本:	1.0
** 描  述:	游戏服连接
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
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