/*******************************************************************
** 语音通道:	E:\Rocket\Cluster\VoiceGateway\Src\AudioUser.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 语音通道:	语音通道
** 日  期:	2013-01-03
** 版  本:	1.0
** 描  述:	语音客户端用户
** 应  用:  	

**************************** 修改记录 ******************************
** 语音通道: 
** 日  期: 
** 描  述: 
********************************************************************/


#ifndef __AUDIO_USER_H__
#define __AUDIO_USER_H__

#include "VoiceDef.h"
#include "net.h"
using namespace rkt;


class CAudioUser : public IConnectionEventHandler
				 , public TimerHandler
{
public:
	//		定时器ID
	enum
	{
		ETimerEventID_Check = 0,	// 死连接检测定时器
	};

public:
	CAudioUser( IConnection * conn );

	virtual ~CAudioUser(void);

	//////////////////////////////////////////////////////////////////////////
	virtual void OnAccept( IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler ){assert(false);}

	virtual void OnConnected( IConnection * conn ){ assert(false);}

	virtual void OnDisconnected( IConnection * conn,DWORD reason );

	virtual void OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen );

	virtual void OnError( IConnection * conn,DWORD dwErrorCode );

	//////////////////////////////////////////////////////////////////////////
	// 定时器
	virtual void OnTimer( unsigned long dwTimerID );

	std::string ToString();

	void Close();

	void Release();

	bool SendData(LPVOID pData,DWORD dwDataLen,DWORD dwFlag=0);

	void Disconnect();

	WORD GetPort();

	const char *GetClientIP();

	DWORD GetIP();

	void SetID( DWORD dwID );
	DWORD GetID();

	DWORD GetGatewayID();

	const char *GetMacAddress();

	DWORD GetConnectTime(void);

	// 设置所属语音客户端ID
	void SetClientID( DWORD dwClientID );

	// 获取所属语音客户端ID
	DWORD GetClientID(void);

	// 启动定时检测定时器
	void StartCheckTimer(void);

private:

	DWORD				m_dwID;

	DWORD				m_dwClientID;

	IConnection	*		m_pConnection;

	DWORD				m_dwConnectTime;	//	连入时间
	DWORD				m_dwLatency;		//	网络延时
	DWORD				m_dwLastPingTime;	//	最后一次发送PING的时间
	DWORD				m_dwLastTimeStamp;	//	最后一次检查的时间戳
	DWORD				m_dwLastClientTick;	//	客户端上次的时间戳
	BOOL				m_bPingReplay;		//	是否回复了Ping



public:
	bool				m_bVerifyed;		// 是否验证通过的
	std::string			m_szRemoteIP;		//	该客户端IP
	std::string			m_szMacAddres;		//  MAC地址
	DWORD				m_dwRemotePort;		//	该客户端端口
	DWORD				m_dwLastKeepaliveTime;	// 上次保持心跳时间
	DWORD				m_dwLastDropTraceTime;	// 最后抛包提示Tick时间
	DWORD				m_dwLastDropDataTime;	// 最后抛包Tick时间

};




#endif