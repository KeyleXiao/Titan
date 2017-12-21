/*******************************************************************
** 文件名:	VoiceConnector.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2012-11-21
** 版  本:	1.0
** 描  述:	语音服务器连接器
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __VOICE_SERVER_CONNECTOR_H__
#define __VOICE_SERVER_CONNECTOR_H__

#include "net.h"
using namespace rkt;

#include <string>
#include <map>
using namespace std;

#include "TimerHandler.h"

class VoiceConnector : public IConnectionEventHandler
					 , public TimerHandler
{
	enum
	{
		ETimerEventID_ReconnectNet = 0,
		ETimerEventID_Keepalive,
	};

public:

	VoiceConnector();

	virtual ~VoiceConnector();

	////////////////////// IConnectionEventHandler ////////////////////////////////////////////////////
	virtual void OnAccept( IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler );

	virtual void OnConnected( IConnection * conn );

	virtual void OnDisconnected( IConnection * conn,DWORD reason );

	virtual void OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen );

	virtual void OnError( IConnection * conn,DWORD dwErrorCode );

	virtual void OnTimer( unsigned long dwTimerID );

	virtual bool Create( DWORD dwGetewayID );
	
	virtual bool Connect(const char * szCSIP,WORD nCSPort);

	virtual void  Close();

	virtual bool  IsConnected();

	// 主动重连
	virtual bool ReConnect();

	virtual void  Release();
	// 发给语音服务器数据
	virtual void  SendData(const char * pData,DWORD dwLen);

	std::string ToString();

	void SetID(DWORD dwID);

	/**
	@name         : 获取队列里未处理的数据包个数
	@name         : 获取队列是发包,否则是收包
	@return       : 
	*/
	unsigned long GetPendingCount(bool bIsSend=false);

protected:
	bool SendHandshake();

protected:

	IConnection   *		m_pConnection;
	std::string			m_szRemoteIP;
	WORD				m_wRemotePort;
	DWORD 				m_dwID;
	bool				m_bIsConnected;
	DWORD				m_dwGetewayID;
	TimerAxis     *		m_pTimerAxis;
public:
	bool m_bIsAnswer;				// 是否有回应
	DWORD m_dwLastAnswerTime;		// 最后收到心跳Tick时间

};



#endif//__VOICE_SERVER_CONNECTOR_H__