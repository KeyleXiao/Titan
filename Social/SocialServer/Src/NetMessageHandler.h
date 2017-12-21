/*******************************************************************
** 文件名:	NetMessageHandler.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-10-10
** 版  本:	1.0
** 描  述:	网络消息处理

		
********************************************************************/


#ifndef ___NETMESSAGEHANDLER_H__
#define ___NETMESSAGEHANDLER_H__

#include "INetMessageHandler.h"

struct IMessageDispatch;


// 网络消息处理
class CNetMessageHandler : public INetMessageHandler
{
public:
public:
	///////ICenterServerMessageHandler///////////////////////////////////////////////////////////////////
	/**
	@name         : 通知服务器列表已经更新
	@brief        : 服务器列表更新只会在有服务器进入退出时触发
	@note         : 具体的列表调用ICenterServerConnector::GetServerList获得
	*/
	virtual void handleServerListUpdated();

	/**
	@name         : 通知服务器信息更新
	@brief        : 例如服务器人数变化等等
	*/
	virtual void handleServerInfoUpdated(CSID ServerID, SERVER_STATE nState, ServerData * pInfo);

	/**
	@name         : 处理其他服务器通过中心服务器转发的消息
	@brief        : 具体内容不清楚
	@param server : 该消息是哪个服务器发过来的
	@param pData  : 消息内容
	@param wDataLen:消息长度
	*/
	virtual void handlePostMessage(CGID server,const char * pData,WORD wDataLen);

	/**
	@name         : 收到请求,执行一个命令
	@brief        : 
	@param server : 发送命令的服务器ID
	@param szCommand : 命令行
	@return       : 执行后调用ICenterServerConnector::SendCommandResponse返回命令执行结果
	*/
	virtual void handleCommandRequest(CGID server,const char * szCommand);

	/**
	@name          : 处理命令执行的结果
	@brief         :
	@param szCommand:执行的命令字符串
	@param bSuccess: 命令是否执行成功
	@param szOutput: 输出的提示信息
	@return        :
	*/
	virtual void handleCommandResponse(const char * szCommand,bool bSuccess,const char * szOutput );


	///////IGatewayMessageHandler///////////////////////////////////////////////////////////////////
	/**
	@name         : 网关服务器通知游戏服务器有一个新的客户端连进来
	@param  client: 该客户端在网关服务器上的ID
	@return       :
	*/
	virtual void handleClientIncoming(ClientID client);

	// GM Chat Client
	virtual void HandleClientIncoming(SMsgGatewayHandshakeGMChatClient& mp);

	/**
	@name         : 网关服务器通知游戏服务器有新的数据包到达
	@brief        : 该数据包是客户端发给该游戏服务器的,通过数据包包头的wKeyServer字节指定
	@param client : 发送数据的客户端ID
	@param pData  : 数据缓冲区位置
	@param wDataLen:数据长度
	*/
	virtual void handleReceivedData(ClientID client,LPVOID pData,DWORD wDataLen);

	/**
	@name         : 网关服务器通知游戏服务器有连接断开
	@param client : 断开连接的客户端ID
	@param dwReason:断开的原因或错误码
	*/
	virtual void handleClientDisconnect(ClientID client,DWORD dwReason);

    ///////管理器///////////////////////////////////////////////////////////////////
	/** 
	@name         : 收到设置游戏ID
	*/
	virtual void handleManagerSetGameWorldID();

  	/** 
	@name         : 收到管理器下发的资源数据
	*/
	virtual void handleManagerResourceData(LPVOID pData,DWORD dwDataLen);

  	/** 
	@name         : 收到管理器下发的资源数据结果
	*/
    virtual void handleManagerResourceResult(LPVOID pData,DWORD dwDataLen);

private:
	// 取得消息分发器
	IMessageDispatch* getMessageDispatch();
};

#endif // ___NETMESSAGEHANDLER_H__