/*******************************************************************
** 文件名:	DateTriggerService_Stub.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	
** 版  本:	1.0
** 描  述:	本文件为根据IDateTriggerService接口定义，自动生成的DateTriggerService接口
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
#include "IDateTriggerService.h"
#include "SimpleDateTriggerAxis.h"
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
class DateTriggerService_Stub : public IServiceStub,public EventHandler,public CallbackProxy<IDateTriggerService>
{
protected:
	SERVICE_PTR         m_pContainer;
	SimpleDateTriggerAxis  m_date_aixs;
	HANDLE              m_eventHandle;
	int                 m_nCheckPending;     // 当程序的处理速度达不到定时事件的频率时会累积太多消息

	typedef    CallbackProxy<IDateHandler>   DATE_PROXY;

public:
	DateTriggerService_Stub() : m_pContainer(0),m_eventHandle(0),m_nCheckPending(0)
    {

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
		_stprintf( szName,_T("datetrigger Hearbeat %d"),::GetCurrentThreadId() );
		HANDLE m_eventHandle = CreateWaitableTimer(NULL, FALSE,szName);
		if ( m_eventHandle==0 )
			return false;

		// 1毫秒=1千万纳秒
		// 第一次启动时间，如果值是正的，代表一个特定的时刻。如果值是负的，代表以100纳秒为单位的相对时间
		LARGE_INTEGER liDueTime;
		liDueTime.QuadPart = -(10000000);   // 1秒后触发

        // 日期触发不用那么频繁触发 设置为2秒
		if (!SetWaitableTimer(m_eventHandle, &liDueTime,1500, NULL, NULL, 0))
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

    virtual void OnEvent( HANDLE event )
    {

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

#pragma pack(push, 1)
	// 函数参数
	struct RegDateTrigger_Param
	{
		DWORD dwFunID;
		DWORD triggerID;
		DATE_PROXY * handler;
        int year;
        int month;
        int day;
        int hour;
        int min;
        int wday;
#ifdef SUPPORT_TIMEAXIS_DEBUG_INFO
        char  debugInfo[32];
#else
		char  debugInfo[4];
#endif
	};
    
	struct UnRegDateTrigger_Param
	{
		DATE_PROXY* handler; 
		DWORD triggerID;
        char dummy[64];
        DATE_PROXY* handlerbak; 
        DWORD triggerIDbak;
	};

#pragma pack(pop)

	// 处理消息
	virtual bool handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf )
	{
        ////////////////////////////////////////////////////////////////////////////
        // 以下自动生成的消息解包，你需要检查

        if ( TEST_SAME_FUNCTION(IDateTriggerService::RegisterDateTrigger) )
        {
			if ( pMsg->context_len!=sizeof(RegDateTrigger_Param) )
				return false;

			RegDateTrigger_Param * pParam = (RegDateTrigger_Param*)pMsg->context;

			if ( pParam->handler==0 )	return false;

            SDateTriggerInfo sTriggerInfo;
            sTriggerInfo.year   = pParam->year;
            sTriggerInfo.month  = pParam->month;
            sTriggerInfo.day    = pParam->day;
            sTriggerInfo.hour   = pParam->hour;
            sTriggerInfo.min    = pParam->min;
            sTriggerInfo.wday   = pParam->wday;
			bool vReturn = m_date_aixs.RegisterDateTrigger( pParam->triggerID,pParam->handler,sTriggerInfo,pParam->debugInfo);
            if ( resultBuf ) *resultBuf << vReturn;

			// 释放引用
			pParam->handler->Release();

            return true;
        }

		if ( TEST_SAME_FUNCTION(IDateTriggerService::UnRegisterDateTrigger) )
        {
            if ( pMsg->context_len!=sizeof(UnRegDateTrigger_Param) )
				return false;

			UnRegDateTrigger_Param * pParam = (UnRegDateTrigger_Param*)pMsg->context;

			if ( pParam->handler==0 )	return false;

			if(!m_date_aixs.UnRegisterDateTrigger(pParam->triggerID,pParam->handler))
            {
                char szAddr[64] = {0};
                sprintf_s(szAddr, "0x%p", pParam->handler);

                ErrorLn("UnRegisterDateTrigger failed:" << pParam->handler->GetDebugString() <<",nTriggerID="<<pParam->triggerID <<",handler=" << szAddr);
            }

            if ( resultBuf ) *resultBuf << bool(true);

			// 释放函数调用的引用
			pParam->handler->Release();

			// 因为是异步调用，所以只能在成功后释放handler,handler在这之前不能释放
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
	// ************************************ IDateTriggerService ********************************************/

    /// purpose: 注册一个触发时刻点
    /// param  : nTriggerID   触发器ID
    /// param  : pDateHander   回调接口
    /// param  : year  年，为-1表示每年都触发
    /// param  : month 月，为-1表示每月都触发
    /// param  : day   日，为-1表示每天都触发
    /// param  : hour  小时
    /// param  : min   分钟
    /// param  : wday  星期几，为-1表示不判断
    /// @note  :如果触发后不需要再触发了请调用UnRegisterDateTrigger删除触发器
    virtual bool RegisterDateTrigger(int nTriggerID, IDateHandler* pDateHander, SDateTriggerInfo sTriggerInfo, const char * debugInfo=0 )
    {
        // 此处必须为单线程调用
        return m_date_aixs.RegisterDateTrigger(nTriggerID, pDateHander,sTriggerInfo,debugInfo);
    }

    /// purpose: 消耗一个触发时刻点
    /// param  : nTriggerID   触发器ID
    /// param  : pDateHander   回调接口
    virtual bool UnRegisterDateTrigger(int nTriggerID, IDateHandler* pDateHander)
    {
        // 此处必须为单线程调用
        return m_date_aixs.UnRegisterDateTrigger(nTriggerID, pDateHander);
    }

	bool RegisterDateTriggerProxy( DWORD triggerID,DATE_PROXY * handler,SDateTriggerInfo sTriggerInfo,const char * debugInfo=0 )
	{
		int nMsgLen = sizeof(SERVICE_MESSAGE)+sizeof(RegDateTrigger_Param);			
		SERVICE_MESSAGE * pMsg = (SERVICE_MESSAGE*)malloc(nMsgLen);		
		pMsg->context_len = sizeof(RegDateTrigger_Param);	
		auto ptr = &IDateTriggerService::RegisterDateTrigger;
		pMsg->function_id = *(DWORD_PTR*)&ptr;								
		RegDateTrigger_Param * pParam= (RegDateTrigger_Param*)(pMsg+1);							 	
		pParam->triggerID = triggerID;
		pParam->handler = handler;
        pParam->year   = sTriggerInfo.year;
        pParam->month  = sTriggerInfo.month;
        pParam->day    = sTriggerInfo.day;
        pParam->hour   = sTriggerInfo.hour;
        pParam->min    = sTriggerInfo.min;
        pParam->wday   = sTriggerInfo.wday;
		
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

	bool UnRegisterDateTriggerProxy( DWORD triggerID,DATE_PROXY * handler )
	{
        int nMsgLen = sizeof(SERVICE_MESSAGE)+sizeof(UnRegDateTrigger_Param);			
        SERVICE_MESSAGE * pMsg = (SERVICE_MESSAGE*)malloc(nMsgLen);		
        pMsg->context_len = sizeof(UnRegDateTrigger_Param);	
        auto ptr = &IDateTriggerService::UnRegisterDateTrigger;
        pMsg->function_id = *(DWORD_PTR*)&ptr;								
        UnRegDateTrigger_Param * pParam= (UnRegDateTrigger_Param*)(pMsg+1);							 	
        pParam->triggerID = triggerID;
        pParam->handler = handler;
        pParam->handlerbak = handler;
        pParam->triggerIDbak = triggerID;
        

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
		m_date_aixs.CheckTimer(timeout);
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreateDateTriggerService()
{
	return new DateTriggerService_Stub();
}