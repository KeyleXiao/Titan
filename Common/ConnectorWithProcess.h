/*******************************************************************
** 文件名:	ConnectorWithProcess.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/11/21
** 版  本:	1.0
** 描  述:	加入了消息分发处理功能
** 应  用:  主动连接器基类二
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "Connector.h"
#include "Processer.h"
#include "ByteRecord.h"

template<class TConn>
class ConnectorWithProcess : public Connector
{
public:

	ConnectorWithProcess(Processer<TConn>& processer, ByteRecord& br, TimerAxis& timerAxis) :
		Connector(timerAxis)
		, m_Processer(processer)
		, m_br(br)
	{}

	virtual ~ConnectorWithProcess() {}

	virtual void OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen )
	{
		assert(conn == m_pConnection);
		if (dwDataLen < sizeof(SGameMsgHead))
		{
			WarningLn("Package length is too short.");
			return;
		}

		Assert(pData != NULL);
		SGameMsgHead* head = (SGameMsgHead*)pData;
		MsgKey msgKey(head->byKeyModule, head->byKeyAction);
		if (GetAnswerKeepAliveID() == msgKey.key)
		{
			m_bIsAnswer = true;							// 是否有回应
			m_dwLastAnswerTime = ::GetTickCount();		// 最后收到心跳Tick时间
		} 
		else
		{
			m_Processer.Process((TConn*)this, pData, dwDataLen);
		}
		
		m_br.OnRecvData(dwDataLen);	// 流量记录 接收
	}

	// 处理未注册的消息（比如要转发的消息）
	virtual	void	UnHandled(LPVOID pData, DWORD dwDataLen)
	{
		SGameMsgHead* head = (SGameMsgHead*)pData;

		char szBuf[256];
		sprintf(szBuf, "UnKnown msg: SrcEndPoint=%d, DestEndPoint=%d, byKeyModule=%d, byKeyAction=%d !",
			head->SrcEndPoint, head->DestEndPoint, head->byKeyModule, head->byKeyAction);
		ErrorLn(szBuf);
	}

protected:
	// 流量记录 发送
	virtual	void	OnSendData(DWORD dwDataLen) { m_br.OnSendData(dwDataLen); }
	// 取心跳回复包的ID
	virtual WORD	GetAnswerKeepAliveID() = 0;

protected:
/*******************************************************************/
// 间隔时间，这里给出默认值
/*******************************************************************/
	// 两次KeepAlive探测间的时间间隔，单位：毫秒
	virtual	DWORD	GetKeepAliveInterval() { return 2 * 1000; }
	// 连接允许的最大超时时间，单位：毫秒
	virtual	DWORD	GetMaxTimeout() { return 120 * 1000; }
	// 重连的时间间隔，单位：毫秒
	virtual	DWORD	GetReconnectInterval() { return 10 * 1000; }
/********************************************************************/

protected:
	Processer<TConn>&	m_Processer;	// 消息分发处理器
	ByteRecord&			m_br;			// 流量记录器，由外部注入
};
