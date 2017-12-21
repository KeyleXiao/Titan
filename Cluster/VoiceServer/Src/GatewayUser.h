/*******************************************************************
** 文件名:	GatewayUser.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2011-05-16
** 版  本:	1.0
** 描  述:	语音网关用户
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __GATEWAY_USER_H__
#define __GATEWAY_USER_H__


#include "net.h"
using namespace rkt;

#include <vector>
using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
@name : 语音网关用户
@brief: 
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class GatewayUser : public IConnectionEventHandler
{
public:

	////////////////////////// IConnectionEventHandler 接口实现 ////////////////////////////////////////////////
	virtual void OnAccept( IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler ){assert(false);}

	virtual void OnConnected( IConnection * conn ){ assert(false); }

	virtual void OnDisconnected( IConnection * conn,DWORD reason );

	virtual void OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen );

	virtual void OnError( IConnection * conn,DWORD dwErrorCode );

	/////////////////////////// GatewayUser 接口 ///////////////////////////////////////////////////////////////
	bool SendData( const char * pData,DWORD dwDataLen,DWORD dwFlag=0);

	bool SendDataToClient( DWORD dwClientID, const char * pData, DWORD dwLen, BYTE nSendChennel = VoiceSendChannel_Control );

	bool BroadcastDataToClient( DWORD * pClientArray, WORD wClientNum, const char * pData, DWORD dwLen, BYTE nSendChennel = VoiceSendChannel_Control );

	DWORD GetServerID();

	void SetServerID(DWORD dwID);

	DWORD GetGatewayID();

	void SetGatewayID(DWORD dwGatewayID);

	/**
	@name         : 取得服务器类型
	@brief        :
	*/
	WORD GetServerType();

	const char * GetRemoteIP();

	DWORD GetRemotePort();

	std::string ToString();

	void Release();

	IConnection * GetConnection();

	GatewayUser(IConnection * conn);
	
	virtual ~GatewayUser();

protected:
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	IConnection	*		m_pConnection;		// 服务器与网关之间的连接
	DWORD				m_dwID;				// 服务器ID
	std::string			m_szRemoteIP;		// 服务器IP
	DWORD				m_dwRemotePort;		// 连进来的端口
	DWORD				m_dwGatewayID;		// 网关ID

public:
	DWORD				m_dwMaxCounts;			// 最大人数
	DWORD				m_dwWorldID;			// 服务器所在游戏世界ID
	WORD				m_wServerType;			// 服务器类型,会根据参见消息结构头第一个字段转发消息给不同的服务器
	WORD				m_wSubID;				// 第n号服务器
	bool				m_bIsPublic;			// 是否是公共区的
	char				m_szName[VOICE_NAME_MAXSIZE];			// 服务器名
	char				m_szWorldName[VOICE_NAME_MAXSIZE];		// 游戏世界名
	DWORD				m_dwStateFlag;			// 服务器状态
	bool				m_bIsAlive;				// 是否有心跳
	bool				m_bIsAnswer;			// 是否有回应
	DWORD				m_dwLastAnswerTime;		// 最后收到心跳Tick时间
	DWORD				m_dwAddTime;			// 连入时间


};

#endif//__GATEWAY_USER_H__