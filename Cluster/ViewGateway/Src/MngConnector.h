/*******************************************************************
** 文件名:	MngConnector.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/11/21
** 版  本:	1.0
** 描  述:	观战管理服连接器
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "ConnectorWithProcess.h"
#include "ViewMsgDef.h"

class MngConnector : public ConnectorWithProcess<MngConnector>
{
public:

	MngConnector(Processer<MngConnector>& processer, ByteRecord& br, TimerAxis& timerAxis);

	virtual ~MngConnector() { ResetID(); }

	inline	void	SetServerID( DWORD dwServerID){ m_dwServerID = dwServerID;}

	inline	void	SetID(DWORD dwID) { m_dwID = dwID; }
	// 是否已与管理服握手完毕
	inline	bool	IsReady() { return m_dwID > 0; }

	virtual void	OnDisconnected(IConnection * conn, DWORD reason);

	virtual void	OnError(IConnection * conn, DWORD dwErrorCode);

	virtual std::string ToString()
	{
		ostrstream buffer;
		buffer << "MngConnector[Local ID=" << m_dwServerID  << "  ID="<<m_dwID<<"], Mng[" << m_szRemoteIP <<":" << m_wRemotePort <<"]" << std::ends;
		return buffer.str();
	}

protected:
	//取得目标节点的名称
	virtual	const char*		GetDestPointName();
	//发送握手消息
	virtual	bool			SendHandshake();

	virtual	const obuf&		GetKeepAliveMsg();
	virtual WORD			GetAnswerKeepAliveID();

	// 两次KeepAlive探测间的时间间隔，单位：毫秒
	virtual	DWORD			GetKeepAliveInterval() { return VIEW_KEEPALIVE_INTERVAL; }
	// 连接允许的最大超时时间，单位：毫秒
	virtual	DWORD			GetMaxTimeout() { return VIEW_KEEPALIVE_MAXTIMEOUT; }
	// 重连的时间间隔，单位：毫秒
	virtual	DWORD			GetReconnectInterval() { return VIEW_RECONNECT_INTERVAL; }

private:
	inline	void	ResetID() { m_dwID = 0; }

protected:
	DWORD 		m_dwID;			// Mng为我生成的自增ID
	DWORD		m_dwServerID;	// 我依据本地文件目录名生成的ID
};
