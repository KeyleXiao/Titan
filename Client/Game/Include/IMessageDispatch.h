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

#include "NetMessageDef.h"

/**
	网络消息处理者
	用法: 由各模块实现该接口，然后调用IMessageDispatch的注册接口,由此接管属于该模块的包
	举例: 例如任务系统，设定moduleId=4,则任务系统创建一个类，继承IMessageHandler，并调用IMessageDispatch注册该消息
	      则以后碰到消息头中标明模块为4的消息，都发给该类处理
*/ 
struct IMessageHandler
{
	/** 消息处理
	@param head : 消息头，通常为一个消息包的前4个字节
    @param data : 消息的实际内容，除去了消息头的
	@param len  : 内容长度
	*/
	virtual void onMessage( SNetMsgHead* head, void* data, size_t len) = 0;
};


/** 消息分发器
	网络消息分发器负责从底层获取网络消息，然后派发到指定的模块进行处理
*/
struct IMessageDispatch
{
	/** 分发指定的消息
	@param buf 网络层输入的消息缓冲
	@param len 缓冲区长度
	*/
	virtual void dispatchMessage(void* buf, size_t len) = 0;


	/** 注册消息处理者
	@param moduleId 模块ID，参考InputMessage.h中的枚举定义
	@param hander 消息处理者实例
	*/
	virtual void registerMessageHandler(ulong moduleId, IMessageHandler* handler) = 0;


	/** 注销消息处理者
	@param moduleId 模块ID，参考DGlobalMessage.h中的枚举定义
	*/
	virtual void unregisterMessageHandler(ulong moduleId, IMessageHandler* handler) = 0;

	/**
	阻塞派发： 暂时不要调度网络包
	比如Unity3d加载地图时无法做到真正的同步加载，此时创建了人物，加载完成后又会卸载掉
	*/
	virtual void choke(bool bEnable) = 0;
};



#endif // __IMESSAGEDISPATCH_H__