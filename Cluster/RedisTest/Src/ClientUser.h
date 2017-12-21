/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceGateway\Src\ClientUser.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	宋文武
** 日  期:	2012-11-21
** 版  本:	1.0
** 描  述:	语音客户端用户
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/


#ifndef __CLIENT_USER_H__
#define __CLIENT_USER_H__

#include "ViewDef.h"
#include "net.h"
using namespace rkt;


class CClientUser : public IConnectionEventHandler
				  , public TimerHandler
{
public:
	//		定时器ID
	enum
	{
		ETimerEventID_Check = 0,	// 死连接检测定时器
	};

public:
	CClientUser( IConnection * conn );

	virtual ~CClientUser(void);

	void CreateKeyData(void);

	//////////////////////////////////////////////////////////////////////////
	virtual void OnAccept( IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler ){assert(false);}

	virtual void OnConnected( IConnection * conn ){ assert(false);}

	virtual void OnDisconnected( IConnection * conn,DWORD reason );

	virtual void OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen );

	virtual void OnError( IConnection * conn,DWORD dwErrorCode );

	//////////////////////////////////////////////////////////////////////////
	// 定时器
	virtual void OnTimer( unsigned long dwTimerID );

	//////////////////////////////////////////////////////////////////////////
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

	// 校验数据
	bool CheckKeyData( char* szKeyData, DWORD dwKeyLen );

	// 设置语音通道ID
	void SetAudioUserID( DWORD dwID );

	// 获取语音通道ID
	DWORD GetAudioUserID(void);

	// 获取语音通道对象
	CAudioUser * GetAudioUser(void);

private:

	DWORD				m_dwID;

	DWORD				m_dwAudioUserID;		// 语音通道ID

	IConnection	*		m_pConnection;

	DWORD				m_dwConnectTime;	//	连入时间
	DWORD				m_dwLatency;		//	网络延时
	DWORD				m_dwLastPingTime;	//	最后一次发送PING的时间
	DWORD				m_dwLastTimeStamp;	//	最后一次检查的时间戳
	DWORD				m_dwLastClientTick;	//	客户端上次的时间戳
	BOOL				m_bPingReplay;		//	是否回复了Ping

public:
	// 密钥长度
	BYTE	m_nKeyLen;	

	// 密钥内容
	char	m_pKeyData[VOICE_RC4_MAX_SIZE];	

	bool				m_bVerifyed;		// 是否验证通过的
	std::string			m_szRemoteIP;		//	该客户端IP
	std::string			m_szMacAddres;		//  MAC地址
	DWORD				m_dwRemotePort;		//	该客户端端口

	std::string			m_szUsername;		// 帐号
	std::string			m_szNickname;		// 昵称
	DWORD				m_dwPDBID;			// PDBID

	DWORD				m_dwLastKeepaliveTime;	// 上次保持心跳时间

};

#endif