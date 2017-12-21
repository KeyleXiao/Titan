/*******************************************************************
** 文件名:	ServerMessageHandler.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-9-14
** 版  本:	1.0
** 描  述:	服务器系统消息处理器
            
********************************************************************/

#pragma once;

#include "IMessageHandler.h"


// 服务器系统消息处理器
class CServerMessageHandler : public ITransmitHandler
{
public:
    ///////////////////////////////////ITransmitHandler/////////////////////////////////////////
	/** 消息处理
	@param server	源服务器ID
	@param uMsgID	消息ID
	@param head		消息头
	@param data		消息
	@param len		消息长度
	*/
	virtual void	onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len);

	/**
	@name         : 通知服务器列表已经更新
	@brief        : 服务器列表更新只会在有服务器进入退出时触发
	@note         : 具体的列表调用ICenterServerConnector::GetServerList获得
	*/
	virtual void	onServerListUpdated();

	/** 通知服务器信息更新
	@param nState		: SERVER_STATE
	@param pServerData	: ServerData
	@brief        : 例如服务器人数变化等等
	*/
	virtual void	onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData);

	////////////////// CServerMessageHandler ////////////////////////////////////////////////////////

	// 构造函数
	CServerMessageHandler();

	// 析构函数
	~CServerMessageHandler();

    bool create();
    void release();

private:
    void onTransmitNewDay(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len);
};