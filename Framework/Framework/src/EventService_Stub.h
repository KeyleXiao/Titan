/*******************************************************************
** 文件名:	EventService_Stub.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	
** 日  期:	
** 版  本:	1.0
** 描  述:	本文件为根据IEventService接口定义，自动生成的EventService接口
            代理文件，该文件是为符合Framework对接口进行的包装

			
********************************************************************/

#pragma once

/**
	服务容器:
	1.框架根据配置创建容器，容器负责与框架进行底层通讯，收到调用请求再转交给应用层提供的服务存根进行处理
	2.代理/存根，从COM架构借鉴过来的名词，应用层调用某个服务的接口，实际交由代理封包成消息，再提交给框架进行转发
	            而框架里的容器收到请求消息，则需要调用存根进行解释和真正处理
*/

#include "IServiceContainer.h"
#include "IEventEngine.h"
#include "buffer.h"

using namespace rkt;
using namespace std;

/** 
   存根对象:
   1.处理收到的消息调用请求，然后把他转发给真正的服务接口函数
*/
class EventService_Stub : public IServiceStub,public rkt::IEventEngine
{
protected:
	SERVICE_PTR         m_pContainer;
	IEventEngine *      m_pEventEngine;

public:
	EventService_Stub() : m_pContainer(0),m_pEventEngine(0){
	}

	// 启动
	virtual bool on_start( SERVICE_PTR pContainer,void * data,int len )
	{
		m_pContainer = pContainer;

		if ( m_pEventEngine==0 )
		{
			m_pEventEngine = CreateEventEngine();
		}

		return true;
	}

	// 停止
	virtual bool on_stop()
	{
		return true;
	}

	// 处理消息
	virtual bool handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf & resultBuf )
	{
		if ( pMsg->context_len==0 || pMsg->context==0 )
			return false;

		ibuffer in(pMsg->context,pMsg->context_len);
		std::string function_name;
		in >> function_name;

		if ( function_name==string("FireVote") )
		{
			WORD wEventID;
			BYTE bSrcType;
			DWORD dwSrcID;
			void * pszContext = 0;
			int nLen;

			in >> wEventID >> bSrcType >> dwSrcID >> pszContext >> nLen;

			bool vReturn= m_pEventEngine->FireVote(wEventID,bSrcType,dwSrcID,(LPCSTR)pszContext,nLen);
			resultBuf << vReturn;
			return true;
		}

		if ( function_name==string("FireExecute") )
		{
			WORD wEventID;
			BYTE bSrcType;
			DWORD dwSrcID;
			void * pszContext = 0;
			int nLen;

			in >> wEventID >> bSrcType >> dwSrcID >> pszContext >> nLen;

			m_pEventEngine->FireExecute(wEventID,bSrcType,dwSrcID,(LPCSTR)pszContext,nLen);
			return true;
		}

		ErrorLn("can not found function name:" << function_name);
		return false;
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
	virtual bool		FireVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
		obuf256 data;
		data << string("FireVote");
		data << wEventID << bSrcType << dwSrcID << (void*)pszContext << nLen;

		void * pReturn = 0;
		int nReturnLen = 0;
		m_pContainer->post_message( data.data(),data.size(),pReturn,nReturnLen);
		if ( nReturnLen!=0 && pReturn!=0)
		{
			ibuffer data(pReturn,nReturnLen);
			bool vReturn;
			data >> vReturn;
			return vReturn;
		}

		return true;
	}

	/** 发送执行事件
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
	virtual void		FireExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
		obuf256 data;
		data << string("FireExecute");
		data << wEventID << bSrcType << dwSrcID << (void*)pszContext << nLen;

		void * pReturn = 0;
		int nReturnLen = 0;
		m_pContainer->post_message( data.data(),data.size(),pReturn,nReturnLen);
		return;
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
	virtual bool		Subscibe(IEventVoteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszDesc)
	{
		if (m_pEventEngine)
			return m_pEventEngine->Subscibe(pSink,wEventID,bSrcType,dwSrcID,pszDesc);
		return false;
	}

	/** 订阅执行事件
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
	virtual bool		Subscibe(IEventExecuteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszDesc)
	{
		if (m_pEventEngine)
			return m_pEventEngine->Subscibe(pSink,wEventID,bSrcType,dwSrcID,pszDesc);
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
		if (m_pEventEngine)
			return m_pEventEngine->UnSubscibe(pSink,wEventID,bSrcType,dwSrcID);
		return false;
	}

	/** 取消订阅执行事件
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
	virtual bool		UnSubscibe(IEventExecuteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID)
	{
		if (m_pEventEngine)
			return m_pEventEngine->UnSubscibe(pSink,wEventID,bSrcType,dwSrcID);
		return false;
	}

	/** 输出事件订阅的统计信息
	@param   
	@param   
	@return  
	*/
	virtual void		OutputSubscibeEventStatInfo(void)
	{
		return m_pEventEngine->OutputSubscibeEventStatInfo();
	}

	/** 输出耗时统计信息 
	@param   
	@param   
	@return  
	*/
	virtual void		OutputAnalyseInfo(void)
	{
		return m_pEventEngine->OutputAnalyseInfo();
	}

	virtual void Release()
	{
		release();
	}

	// 销毁
	virtual void release()
	{
		if ( m_pEventEngine )
		{
			m_pEventEngine->Release();
			m_pEventEngine = 0;
		}

		Framework::getInstance().remove_service_event();

		delete this;
	}

	// 返回代理对象即可
	virtual void * query_interface( int iid )
	{
		return (IEventEngine*)this;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreateEventService()
{
	return new EventService_Stub();
}