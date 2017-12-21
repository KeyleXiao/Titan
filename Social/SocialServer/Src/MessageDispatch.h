/*******************************************************************
** 文件名:	MessageDispatch.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-3-19
** 版  本:	1.0
** 描  述:	

		网络消息分发器实现
********************************************************************/


#ifndef ___MESSAGEDISPATCH_H__
#define ___MESSAGEDISPATCH_H__

#include "IMessageDispatch.h"

struct ITransmitHandler;
struct IMessageHandler;

// 消息分发器
class CMessageDispatch : public IMessageDispatch
{
public:
	/////////// IMessageDispatch ///////////////////////////////////////////////////////////////
	/** 分发转发的消息
	@param pData 服务器转发消息的缓冲
	@param nDataLen 缓冲区长度
	*/
	virtual void						dispatchTransmit(DWORD server, void * buf, size_t len);

	/** 注册消息处理者
	@param moduleId 模块ID，参考DGlobalMessage.h中的枚举定义
	@param hander 消息处理者实例
	*/
	virtual void						registerTransmitHandler(ulong moduleId, ITransmitHandler* handler);

	/** 取消注册消息处理者
	@param moduleId 模块ID，参考DGlobalMessage.h中的枚举定义
	*/
	virtual void						unregisterTransmitHandler(ulong moduleId);

	/**
	@name         : 通知服务器列表已经更新
	@brief        : 服务器列表更新只会在有服务器进入退出时触发
	@note         : 具体的列表调用ICenterServerConnector::GetServerList获得
	*/
	virtual void						dispatchServerListUpdated();

	/** 通知服务器信息更新
	@param nState		: SERVER_STATE
	@param pServerData	: ServerData
	@brief        : 例如服务器人数变化等等
	*/
	virtual void						dispatchServerInfoUpdated(DWORD ServerID,BYTE nState,void * pServerData);


	/** 分发指定的消息
	@param buf 网络层输入的消息缓冲
	@param len 缓冲区长度
	*/
	virtual void						dispatchMessage(ClientID clientID, void* buf, size_t len);

	/** 注册消息处理者
	@param moduleId 模块ID，参考InputMessage.h中的枚举定义
	@param hander 消息处理者实例
	*/
	virtual void						registerMessageHandler(ulong moduleId, IMessageHandler* handler);

	/** 注销消息处理者
	@param moduleId 模块ID，参考DGlobalMessage.h中的枚举定义
	*/
	virtual void						unregisterMessageHandler(ulong moduleId);


	/////////// MessageDispatch ///////////////////////////////////////////////////////////////

	/** 构造
	@param moduleId
	*/
	CMessageDispatch(void);

	/** 构造
	@param moduleId
	*/
	virtual ~CMessageDispatch(void);

private:
	ITransmitHandler *  m_TransmitHandlers[MSG_MODULEID_MAXID];		// 处理服务器消息Handler
	IMessageHandler *	m_MessageHandlers[MSG_MODULEID_MAXID];		// 处理客户端消息Handler
};


#endif	// ___MESSAGEDISPATCH_H__