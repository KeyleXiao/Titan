//==========================================================================
/**
* @file	  : IMessageDispatch.h
* @author : PeakGao <peakgao163@163.com>
* created : 2008-1-15   16:58
* purpose : 网络消息分发器
*/
//==========================================================================

#ifndef __IMESSAGEDISPATCH_H__
#define __IMESSAGEDISPATCH_H__
#include "DGlobalMessage.h"

/// 客户端网络消息处理者
struct IMessageHandler
{
	/** 消息处理
	@param msg 网络消息
	*/
	virtual void OnMessage(ulong nCenterSeverID, ulong nSrcPointID, ulong nMsgID, SGameMsgHead* pHead, void* pData, size_t len) = 0;

	/** 更新中心服的连接信息
	@param ServerID 中心服ID
	@param nState 连接状态（连上/断开）
	*/
	virtual void CServerConnected(ulong nServerID, TWServer_State nState) = 0;

	/** 更新中心服下面的区域服务器的连接信息
	@param ServerID 区域服务器ID
	@param nState 连接状态（连上/断开）
	*/
	virtual void AServerConnected(ulong nCServerID, ulong nAServerID, int nSeverType, TWServer_State nState) = 0;
};

/**
	网络消息分发器负责从底层获取网络消息，然后派发到指定的模块进行处理
*/
/// 消息分发器
struct IMessageDispatch
{
	/** 分发指定的消息
	@param buf 网络层输入的消息缓冲
	@param len 缓冲区长度
	*/
	virtual void dispatchMessage(void* buf, size_t len) = 0;

	/** 更新中心服的连接信息
	@param ServerID 中心服ID
	@param nState 连接状态（连上/断开）
	*/
	virtual void dispatchCServerConnected(ulong nServerID, TWServer_State nState) = 0;

	/** 更新中心服下面的区域服务器的连接信息
	@param ServerID 区域服务器ID
	@param nState 连接状态（连上/断开）
	*/
	virtual void dispatchAServerConnected(ulong nCServerID, ulong nAServerID, int nSeverType, TWServer_State nState) = 0;

	/** 注册消息处理者
	@param moduleId 模块ID，参考InputMessage.h中的枚举定义
	@param hander 消息处理者实例
	*/
	virtual void registerMessageHandler(ulong moduleId, IMessageHandler* handler) = 0;


	/** 注销消息处理者
	@param moduleId 模块ID，参考DGlobalMessage.h中的枚举定义
	*/
	virtual void unregisterMessageHandler(ulong moduleId) = 0;
};

#endif // __IMESSAGEDISPATCH_H__