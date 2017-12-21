/*******************************************************************
** 文件名:	IEntityEvent.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	2/26/2015
** 版  本:	1.0
** 描  述:	

			实体事件接口
********************************************************************/


#pragma once

#include "Event_ManagedDef.h"
#include "IEventEngine.h"
using namespace rkt;

// 事件ID
typedef ENTITYEVENT_KEY  EventKey;

// 实体事件中心
struct __IEntityEvent
{
	/** 发送投票事件   
	@param   wEventID ：事件ID
	@param   pszContext : 上下文
	@param   nLen : 上下文长度	  
	*/
	virtual bool		FireVote(EventKey eventID, LPCSTR pszContext, int nLen) = 0;

	/** 发送执行事件
	@param   wEventID ：事件ID
	@param   pszContext : 上下文
	@param   nLen : 上下文长度	  
	*/
	virtual void		FireExecute(EventKey eventID, LPCSTR pszContext, int nLen) = 0;

	/** 发送全局执行事件
	@param   wEventID ：事件ID
	@param   pszContext : 上下文
	@param   nLen : 上下文长度	  
	*/
	virtual void		FireExecute(GAMEEVENT_KEY eventID, LPCSTR pszContext, int nLen) = 0;

	/** 发送投票事件通过后执行事件
	@param   wEventID ：事件ID
	@param   pszContext : 上下文
	@param   nLen : 上下文长度	  
	*/
	virtual bool		FireVoteExecute(EventKey eventID, LPCSTR pszContext, int nLen) = 0;

	/** 订阅投票事件
	@param   pSink : 回调sink
	@param   wEventID : 事件ID
	@param	 pDesc : 订阅者描述，用来调试程序
	@return  成功返回true,失败返回false
	*/
	virtual bool		Subscibe(IEventVoteSink * pSink,EventKey eventID, LPCSTR pszDesc) = 0;

	/** 订阅执行事件
	@param   pSink : 回调sink
	@param   wEventID : 事件ID
	@param	 pDesc : 订阅者描述，用来调试程序
	@return  成功返回true,失败返回false
	*/
	virtual bool		Subscibe(IEventExecuteSink * pSink,EventKey eventID, LPCSTR pszDesc) = 0;

	/** 取消订阅投票事件
	@param   pSink : 回调sink
	@param   wEventID : 事件ID
	@return  成功返回true,失败返回false
	*/
	virtual bool		UnSubscibe(IEventVoteSink * pSink,EventKey eventID) = 0;

	/** 取消订阅执行事件
	@param   pSink : 回调sink
	@param   wEventID : 事件ID
	@return  成功返回true,失败返回false
	*/
	virtual bool		UnSubscibe(IEventExecuteSink * pSink,EventKey eventID) = 0;
};