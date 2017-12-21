/*******************************************************************
** 文件名:	ServerUser.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	2015-8-17
** 版  本:	1.0
** 描  述:	数据监护服务器用户
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/


#ifndef __SERVER_USER_H__
#define __SERVER_USER_H__


#include "net.h"
using namespace rkt;

#include <vector>
using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
@name : 数据监护服务器用户
@brief: 
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ServerUser : public IConnectionEventHandler
{
public:
	virtual void OnAccept( IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler ){assert(false);}

	virtual void OnConnected( IConnection * conn ){assert(false);}

	virtual void OnDisconnected( IConnection * conn,DWORD reason );

	virtual void OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen );

	virtual void OnError( IConnection * conn,DWORD dwErrorCode );

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool SendData( const char * pData,DWORD dwDataLen,DWORD dwFlag=0);

	DWORD GetServerID();

	void SetServerID(DWORD dwID);

	/**
	@name         : 取得服务器类型
	@brief        :
	*/
	WORD GetServerType();

	const char * GetRemoteIP();

	DWORD GetRemotePort();

	std::string ToString();

	void Release();

	ServerUser(IConnection * conn);
	
	virtual ~ServerUser();

public:
	/** 收到桥服连接器的检测请求，向各个服务器转发，并将桥服本身的数据传回
	@param   
	@param   
	@return  
	*/
	virtual void HandleServerCheckReq(LPVOID pData,DWORD dwDataLen);

	/** 收到中心服连接器的回应，进行转发
	@param   
	@param   
	@return  
	*/
	virtual void HandleServerCheckRes(LPVOID pData,DWORD dwDataLen);


private:
	/** 收到往数据监护服务器写入数据
	@param actionId		: 
	@param head	        : 
	@param data	        : 数据
	@param len	        : 数据长度
	@return
	*/
	void	OnMsgWriteImageData(ulong actionId, SGameMsgHead* head, void* data, size_t len);

	/** 收到从数据监护服务器读取数据
	@param actionId		: 
	@param head	        : 
	@param data	        : 数据
	@param len	        : 数据长度
	@return
	*/
	void	OnMsgReadImageData(ulong actionId, SGameMsgHead* head, void* data, size_t len);
protected:
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	IConnection	*		m_pConnection;			// 服务器与网关之间的连接
	DWORD				m_dwID;					// 服务器ID
	std::string			m_szRemoteIP;			// 服务器IP
	DWORD				m_dwRemotePort;			// 连进来的端口

public:
	DWORD				m_dwWorldID;			// 服务器所在游戏世界ID
	WORD				m_wServerType;			// 服务器类型,会根据参见消息结构头第一个字段转发消息给不同的服务器
	WORD				m_wSubID;				// 第n号服务器
	bool				m_bIsPublic;			// 是否是公共区的
	char				m_szName[SERVER_NAME_MAXSIZE];			// 服务器名
	DWORD				m_dwStateFlag;			// 服务器状态
	bool				m_bIsAlive;				// 是否有心跳
	bool				m_bIsAnswer;			// 是否有回应
	DWORD				m_dwLastAnswerTime;		// 最后收到心跳Tick时间
	DWORD				m_dwAddTime;			// 连入时间
};

#endif//__SERVER_USER_H__