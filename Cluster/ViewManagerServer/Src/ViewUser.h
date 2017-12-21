/*******************************************************************
** 文件名:	ViewUser.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/12/01
** 版  本:	1.0
** 描  述:	观战服连接对象
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
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

	// 是否准备好对外服务了
	bool	IsReady() { return GetMaxCount() > 0; }
	// 是否人满
	bool	IsFull() { return GetCurrCount() >= GetMaxCount(); }

protected:
	virtual std::string ToString();

	virtual WORD		GetKeepAliveID();

	virtual	const obuf&	GetAnswerKeepAliveMsg();

private:
	DWORD	m_dwMaxCount;	// 能承载的最大人数
	DWORD	m_dwCurrCount;	// 当前人数
public:
};

