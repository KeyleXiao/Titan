/*******************************************************************
** 文件名:	EventService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	20/11/2014
** 版  本:	1.0
** 描  述:	跨线程事件服务

			
********************************************************************/

#pragma once

// 开启多线程事件
#define TEVENT_MULTITHREAD

#include "IEventEngine.h"
#include "Singleton.h"
#include "ServiceContainer.h"
#include "Framework.h"
#include "WorkThreadScheduler.h"
#include "Simple_Atom_Lock.h"
#include "EventHandlerProxy.h"
#include "EventEngineT.h"
#include "Event_ManagedDef.h"
#include "IEventEngine.h"
using namespace rkt;

/**
	事件服务:
	1.采用线程安全存储的方式，实现线程安全的跨线程事件服务
*/

class EventService : public rkt::IEventEngine
{
	TEventEngine< IEventExecuteSink,
				  EventService,
				  WORD,
				  EVENT_SCENE_CREATED,
				  EVEN_MAX_ID - EVENT_SCENE_CREATED>   m_pEngine;

public:
	EventService() : m_pEngine(this)
	{
	}

	virtual void		Release(void)
	{
		delete this;
	}

	// 回调执行事件
	bool operator() (IEventExecuteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
		if(pSink == NULL)
		{
			return false;
		}

		pSink->OnExecute(wEventID, bSrcType, dwSrcID, pszContext, nLen);

		return true;
	}

	virtual void		FireExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
		m_pEngine.Fire( wEventID, bSrcType, dwSrcID, pszContext, nLen);
	}

	virtual bool		Subscibe(IEventExecuteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszDesc)
	{
		EventHandlerProxy * pProxy = new EventHandlerProxy;
		if ( !pProxy->Init( (INT_PTR)this, pSink,wEventID,bSrcType,dwSrcID,pszDesc ) )
		{
			delete pProxy;
			pProxy = 0;
			return false;
		}
	
		return m_pEngine.Subscibe( pProxy,wEventID,pszDesc );
	}

	virtual bool		UnSubscibe(IEventExecuteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID)
	{
		EventHandlerProxy * pProxy = EventHandlerProxy::CloseProxy( (INT_PTR)this,pSink,wEventID,bSrcType,dwSrcID );
		if ( pProxy==0 )
			return false;

		m_pEngine.UnSubscibe( pProxy,wEventID );

        // 释放对象
        pProxy->Release();

        return true;
	}

	/** 输出事件订阅的统计信息
	@param   
	@param   
	@return  
	*/
	virtual void		OutputSubscibeEventStatInfo(void)
	{
		m_pEngine.OutputSubscibeEventStatInfo(true);
	}


	/** 发送投票事件   
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度	  
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool		FireVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen){
		return false;
	}

	/** 订阅投票事件
	@param   pSink : 回调sink
	@param   wEventID : 事件ID
	@param   bSrcType ：发送源类型
	@param	 dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
	@param	 pDesc : 订阅者描述，用来调试程序
	@return  成功返回true,失败返回false
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool		Subscibe(IEventVoteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszDesc){
		return false;
	}

	/** 取消订阅投票事件
	@param   pSink : 回调sink
	@param   wEventID : 事件ID
	@param   bSrcType ：发送源类型
	@param	 dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
	@param	 pDesc : 订阅者描述，用来调试程序
	@return  成功返回true,失败返回false
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool		UnSubscibe(IEventVoteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID)
	{
		return false;
	}

	virtual void		OutputAnalyseInfo(void)
	{
	}
};