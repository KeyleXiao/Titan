/*******************************************************************
** 文件名:	IMessageDispatch.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-10-14
** 版  本:	1.0
** 描  述:	网络消息分发器接口
		
********************************************************************/

#ifndef __IMESSAGEDISPATCH_H__
#define __IMESSAGEDISPATCH_H__

#include "IMessageHandler.h"

// 使用Service来处理消息
#define SERVICE_HANDLE_MESSAGE

/** 消息分发器
	网络消息分发器负责从底层获取网络消息，然后派发到指定的模块进行处理
*/
struct IMessageDispatch
{
	/** 分发转发的消息
	@param pData 服务器转发消息的缓冲
	@param nDataLen 缓冲区长度
	*/
	virtual void						dispatchTransmit(DWORD server, void * buf, size_t len) = 0;

	/** 注册消息处理者
	@param moduleId 模块ID，参考DGlobalMessage.h中的枚举定义
	@param hander 消息处理者实例
	*/
	virtual void						registerTransmitHandler(ulong moduleId, ITransmitHandler* handler) = 0;

	/** 取消注册消息处理者
	@param moduleId 模块ID，参考DGlobalMessage.h中的枚举定义
	*/
	virtual void						unregisterTransmitHandler(ulong moduleId) = 0;

	/**
	@name         : 通知服务器列表已经更新
	@brief        : 服务器列表更新只会在有服务器进入退出时触发
	@note         : 具体的列表调用ICenterServerConnector::GetServerList获得
	*/
	virtual void						dispatchServerListUpdated() = 0;

	/** 通知服务器信息更新
	@param nState		: SERVER_STATE
	@param pServerData	: ServerData
	@brief        : 例如服务器人数变化等等
	*/
	virtual void						dispatchServerInfoUpdated(DWORD ServerID,BYTE nState,void * pServerData) = 0;


	/** 分发指定的消息
	@param buf 网络层输入的消息缓冲
	@param len 缓冲区长度
	*/
	virtual void						dispatchMessage(ClientID clientID, void* buf, size_t len) = 0;

	/** 注册消息处理者
	@param moduleId 模块ID，参考InputMessage.h中的枚举定义
	@param hander 消息处理者实例
	*/
	virtual void						registerMessageHandler(ulong moduleId, IMessageHandler* handler) = 0;

	/** 注销消息处理者
	@param moduleId 模块ID，参考DGlobalMessage.h中的枚举定义
	*/
	virtual void						unregisterMessageHandler(ulong moduleId) = 0;
};



#endif // __IMESSAGEDISPATCH_H__