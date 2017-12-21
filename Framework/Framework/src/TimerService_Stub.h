/*******************************************************************
** 文件名:	TimerService_Stub.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	
** 日  期:	
** 版  本:	1.0
** 描  述:	本文件为根据ITimerService接口定义，自动生成的TimerService接口
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
#include "ITimerService.h"
#include "TimerAxis.h"
#include "buffer.h"

#include "Framework.h"
#include "net/Reactor.h"
#include "net/net.h"
#include "CallbackProxy.h"
using namespace rkt;
using namespace std;

/** 
   存根对象:
   1.处理收到的消息调用请求，然后把他转发给真正的服务接口函数
*/
class TimerService_Stub : public IServiceStub,public EventHandler,public CallbackProxy<ITimerService>
{
protected:
	SERVICE_PTR         m_pContainer;
	TimerAxis           m_time_aixs;
	HANDLE              m_eventHandle;
	int                 m_nCheckPending;     // 当程序的处理速度达不到定时事件的频率时会累积太多消息

	typedef    CallbackProxy<ITimerHandler>   TIMER_PROXY;

public:
	TimerService_Stub() : m_pContainer(0),m_eventHandle(0),m_nCheckPending(0){
	}

	// 启动
	virtual bool on_start( SERVICE_PTR pContainer,void * data,int len )
	{
		m_pContainer = pContainer;

		// 挂接网络事件
		Reactor * reactor = Framework::getInstance().get_reactor();
		if ( reactor==0 )
			return false;
		
		TCHAR szName[128];
		_stprintf( szName,_T("Timer Hearbeat %d"),::GetCurrentThreadId() );
		HANDLE m_eventHandle = CreateWaitableTimer(NULL, FALSE,szName);
		if ( m_eventHandle==0 )
			return false;

		// 1毫秒=1千万纳秒
		// 第一次启动时间，如果值是正的，代表一个特定的时刻。如果值是负的，代表以100纳秒为单位的相对时间
		LARGE_INTEGER liDueTime;
		liDueTime.QuadPart = -(10000000);   // 1秒后触发

		if (!SetWaitableTimer(m_eventHandle, &liDueTime, 8, NULL, NULL, 0))
			return false;

		reactor->AddEvent(m_eventHandle);
        reactor->RegisterEventHandler( m_eventHandle,this,__FUNCTION__);

        // 启动会有eventhander内存坏掉的问题,加日志
        char szAddr[64] = {0};
        sprintf_s(szAddr, "0x%p", this);
        EmphasisLn(__FILE__ << ", " << __FUNCTION__ << " event hander=" << szAddr);

        memcpy(m_pMessage->context + strlen(m_strPrefix), "ts_checktimer", strlen("ts_checktimer"));

		return true;
	}

	// 停止
	virtual bool on_stop()
	{
		if ( m_eventHandle!=0 )
		{
			Reactor * reactor = Framework::getInstance().get_reactor();
			if ( reactor!=0 )
			{
				reactor->RemoveEvent(m_eventHandle);
			}
			::CloseHandle(m_eventHandle);
			m_eventHandle = 0;
		}
        m_pContainer = SERVICE_PTR(0);
		return true;
	}

	virtual void OnEvent( HANDLE event )
	{
		// CheckTimer(m_pContainer->get_scheme().scheme->time_slice);
		
		// 切换到timer容器的协程去执行才能有执行现场

		// 当程序的处理速度达不到定时事件的频率时会累积太多消息,所以如果还有消息未处理，就不要发消息了
		if ( m_nCheckPending<1)
		{
			++m_nCheckPending;

			if ( m_pContainer!=0 )
			{
				m_pContainer->recv_message( /*&m_Message*/m_pMessage,false );
			}
		}
	}

#pragma pack(push, 1)

	// 函数参数
	struct SetTimer_Param
	{
		DWORD dwFunID;
		DWORD timerID;
		DWORD interval;
		TIMER_PROXY * handler;
		DWORD callTimes;
#ifdef SUPPORT_TIMEAXIS_DEBUG_INFO
        char  debugInfo[32];
#else
		char  debugInfo[4];
#endif
	};

	struct KillTimer_Param
	{
		TIMER_PROXY* handler; 
		DWORD timerID;
        char dummy[64];
        TIMER_PROXY* handlerbak; 
        DWORD timerIDbak;
	};

#pragma pack(pop)

	// 处理消息
	virtual bool handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf )
	{
        ////////////////////////////////////////////////////////////////////////////
        // 以下自动生成的消息解包，你需要检查

        if ( TEST_SAME_FUNCTION(ITimerService::SetTimer) )
        {
			if ( pMsg->context_len!=sizeof(SetTimer_Param) )
				return false;

			SetTimer_Param * pParam = (SetTimer_Param*)pMsg->context;

			if ( pParam->handler==0 )	return false;

			bool vReturn = m_time_aixs.SetTimer( pParam->timerID,pParam->interval,pParam->handler,pParam->callTimes,pParam->debugInfo);
            if ( resultBuf ) *resultBuf << vReturn;

			// 释放引用
			pParam->handler->Release();

            return true;
        }

		if ( TEST_SAME_FUNCTION(ITimerService::KillTimer) )
        {
            if ( pMsg->context_len!=sizeof(KillTimer_Param) )
				return false;

			KillTimer_Param * pParam = (KillTimer_Param*)pMsg->context;

			if ( pParam->handler==0 )	return false;

			if(!m_time_aixs.KillTimer(pParam->timerID,pParam->handler))
            {
                char szAddr[64] = {0};
                sprintf_s(szAddr, "0x%p", pParam->handler);

                ErrorLn("KillTimer failed:" << pParam->handler->GetDebugString() <<",timerID="<<pParam->timerID <<",handler=" << szAddr);
            }

            if ( resultBuf ) *resultBuf << bool(true);

			// 释放函数调用的引用
			pParam->handler->Release();

			// 因为是异步调用，所以只能在KillTimer成功后释放handler,handler在这之前不能释放
			pParam->handler->Release();

            return true;
        }


		return true;
	}

	virtual void Do(SERVICE_MESSAGE * pMsg)
	{
		--m_nCheckPending;
		CheckTimer(m_pContainer->get_scheme().scheme->time_slice);
	}

	// 销毁
	virtual void release()
	{
        char szAddr[64] = {0};
        sprintf_s(szAddr, "0x%p", this);
        EmphasisLn(__FILE__ << ", " << __FUNCTION__ << " destroy, addr=" << szAddr);

		delete this;
	}

	// 返回代理对象即可
	virtual void * query_interface( int iid )
	{
		return NULL;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	// ************************************ ITimerService ********************************************/
	virtual bool SetTimer( DWORD timerID,DWORD interval,ITimerHandler * handler,DWORD callTimes=INFINITY_CALL,const char * debugInfo=0 )
	{
		// 此处必须为单线程调用
		return m_time_aixs.SetTimer(timerID,interval,handler,callTimes,debugInfo);
	}

	virtual bool KillTimer( DWORD timerID,ITimerHandler * handler )
	{
		// 此处必须为单线程调用
		return m_time_aixs.KillTimer(timerID, handler);
	}

	bool SetTimerProxy( DWORD timerID,DWORD interval,TIMER_PROXY * handler,DWORD callTimes=INFINITY_CALL,const char * debugInfo=0 )
	{
		//BUILD_MSG_CONTEXT( ITimerService::SetTimer,SetTimer_Param);
		int nMsgLen = sizeof(SERVICE_MESSAGE)+sizeof(SetTimer_Param);			
		SERVICE_MESSAGE * pMsg = (SERVICE_MESSAGE*)malloc(nMsgLen);		
		pMsg->context_len = sizeof(SetTimer_Param);	
		auto ptr = &ITimerService::SetTimer;
		pMsg->function_id = *(DWORD_PTR*)&ptr;								
		SetTimer_Param * pParam= (SetTimer_Param*)(pMsg+1);							 	
		pParam->timerID = timerID;
		pParam->interval= interval;
		pParam->handler = handler;
		pParam->callTimes = callTimes;
		
		if ( debugInfo )
			lstrcpynA( pParam->debugInfo,debugInfo,sizeof(pParam->debugInfo));
		else
			pParam->debugInfo[0]=0;

        while(m_pContainer == 0)
        {
            RUNNING_CONTEXT * pRunning = WorkThreadScheduler::getInstance().get_running();
            SERVICE_PTR pServiceCall = ServiceMgr::getInstance().get_service(pRunning->service_id);

            pServiceCall->yield(false);
        }

		// 因为是异步调用，这里必须要增加引用计数，否则会出现野指针问题
		handler->AddRef();

        m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
        return true;
	}

	bool KillTimerProxy( DWORD timerID,TIMER_PROXY * handler )
	{
        BUILD_MSG_CONTEXT( ITimerService::KillTimer,KillTimer_Param);
		pParam->handler = handler;
		pParam->timerID = timerID;
        pParam->handlerbak = handler;
        pParam->timerIDbak = timerID;

#ifdef VERSEION_DEBUG
        sstrcpyn(pMsg->szName, handler->GetDebugString().c_str(), sizeof(pMsg->szName)); 
#endif

        while(m_pContainer == 0)
        {
            RUNNING_CONTEXT * pRunning = WorkThreadScheduler::getInstance().get_running();
            SERVICE_PTR pServiceCall = ServiceMgr::getInstance().get_service(pRunning->service_id);

            pServiceCall->yield(false);
        }

		// 因为是异步调用，这里必须要增加引用计数，否则会出现野指针问题
		handler->AddRef();

        m_pContainer->post_message( pMsg,nMsgLen,0,MSG_FLAG_NO_BLOCK);
        return true;
	}

	virtual void CheckTimer(ulong timeout = 0)
	{
		m_time_aixs.CheckTimer(timeout);
	}

	/**
	取得时间轴对象
	@desc : 有些老系统，例如网关连接器里用的是老时间轴对象，所以还是暴露一个接口允许传出去
	*/
	virtual void * getAxis()
	{
		return &m_time_aixs;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreateTimerService()
{
	return new TimerService_Stub();
}