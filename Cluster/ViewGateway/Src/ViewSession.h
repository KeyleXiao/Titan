/*******************************************************************
** 文件名:	ViewSession.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/12/04
** 版  本:	1.0
** 描  述:	Gate上的ViewServer连接对象
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "SessionUser.h"

class ViewSession :	public SessionUser<ViewSession, ViewID>
{
public:
	ViewSession(IConnection * conn, IUserList<ViewID>& pUserList, Processer<ViewSession>& processer, TimerAxis& timerAxis, IByteRecord* pByteRecord);
	virtual ~ViewSession();

	virtual std::string ToString() override;

	// 处理未注册的消息（比如要转发的消息）
	virtual	void		UnHandled(LPVOID pData, DWORD dwDataLen) override;

protected:
	virtual WORD		GetKeepAliveID();

	virtual	const obuf&	GetAnswerKeepAliveMsg();
};

