/*******************************************************************
** 文件名:	GateUser.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/12/01
** 版  本:	1.0
** 描  述:	Gate连接对象
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
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

	// 是否准备好对外服务了
	inline	bool	IsReady() { return GetMaxCount() > 0; }

protected:
	virtual std::string ToString();

	virtual WORD		GetKeepAliveID();

	virtual	const obuf&	GetAnswerKeepAliveMsg();

private:
	DWORD	m_dwMaxCount;	// 能承载的最大人数
	DWORD	m_dwCurrCount;	// 当前人数
	WORD	m_wPortForView;	// 该Gate监听ViewServer的端口
	WORD	m_wClientPort;	// 该Gate监听Client的端口

};