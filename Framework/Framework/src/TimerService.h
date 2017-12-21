/*******************************************************************
** 文件名:	TimerService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	18/10/2014
** 版  本:	1.0
** 描  述:	时间轴服务

			
********************************************************************/

#pragma once

#include "ITimerService.h"
#include "ThreadSpecialData.h"
#include "Singleton.h"
#include "IServiceContainer.h"
#include "Framework.h"
#include "WorkThreadScheduler.h"
#include "TimerHandlerProxy.h"
#include "Simple_Atom_Lock.h"
#include "TimerService_Stub.h"
#include "WorkThreadScheduler.h"
using namespace rkt;

#define THREAD_MAX 256


/**
	时间轴服务:
	1.采用线程安全存储的方式，实现线程安全的定时器服务
*/
class TimerService : public ITimerService,public Singleton<TimerService>
{
public:
    TheadSpecialData<TimerService_Stub*>  m_thread_timer_axis_old;

    TimerService_Stub* m_thread_timer_axis[THREAD_MAX];

public:
	TimerService() 
    {   
        memset(m_thread_timer_axis, 0, THREAD_MAX * sizeof(TimerService_Stub*));
    }

    // 为工作线程创建timerservice
    void createTimerService(int nThreadID)
    {
        if(nThreadID >= THREAD_MAX)
        {
            ErrorLn("thread id out of range: id=" << nThreadID << ", please redefine THREAD_MAX");
            return;
        }

        if(m_thread_timer_axis[nThreadID] != NULL)
        {
            ErrorLn("createTimerService for thread,id=" << nThreadID << " failed, already created");
            return;
        }

        SERVICE_SCHEME * scheme = Framework::getInstance().get_service_scheme("TimerService");
        if(scheme == NULL)
        {
            ErrorLn("TimerService start failed, scheme not found!");
            return;
        }

        SERVICE_SCHEME my_scheme;
        my_scheme.scheme = scheme->scheme;
        my_scheme.thread_id = nThreadID;

        SERVICE_PTR ptrService(new ServiceContainer);
        TimerService_Stub* pTimer = new TimerService_Stub;
        if(ptrService->start(ptrService,&my_scheme,pTimer))
        {
            m_thread_timer_axis[nThreadID] = pTimer;
        }
        else
        {
            ErrorLn("start timer service failed, thread_id=" << nThreadID);
            delete pTimer; pTimer = NULL;
        }
    }

    // 根据call_service获取该线程上的时间轴
    bool getTimerAxis(ITimerService *& pAxis,TimerService_Stub*& pStub)
    {
        RUNNING_CONTEXT* pRunning = WorkThreadScheduler::getInstance().get_running();
        SERVICE_PTR pContainer = Framework::getInstance().get_service_manager()->get_service(pRunning->call_service);
        if(pContainer == 0)
        {
            __debugbreak();
            ErrorLn("getTimerAxis failed,cant find service, call_service=" << pRunning->call_service);
            return false;
        }

        if(pContainer->get_scheme().thread_id >= THREAD_MAX)
        {
            ErrorLn("getTimerAxis failed, thread_id out of range: thread id=" << pContainer->get_scheme().thread_id);
            return false;
        }

		// 先取内部时间轴
		pAxis = pContainer->get_inner_timeraxis();
		if ( pAxis ) return true;

        pStub = m_thread_timer_axis[pContainer->get_scheme().thread_id];
        
        if(pStub == NULL)
        {
            ErrorLn("getTimerAxis failed, thread_id=" << pContainer->get_scheme().thread_id);
			return false;
        }

        return true;
    }

	/**
	@purpose          : 设置一个定时器
	@param	 timerID  : 定时器ID
	@param   interval : 定时器调用间隔
	@param   handler  : 处理接口
	@param   callTimes: 调用次数,默认调用无穷次
	@param   debugInfo: 调试信息
	@return		      : 如果设置成功则返回true
	*/
	virtual bool SetTimer( DWORD timerID,DWORD interval,ITimerHandler * handler,DWORD callTimes=INFINITY_CALL,const char * debugInfo=0 )
    {
        TimerService_Stub* pStubAxis = NULL;
		ITimerService    * pInnerAxis = NULL;
		
        if(getTimerAxis(pInnerAxis,pStubAxis)==false)
        {
            ErrorLn("SetTimer failed, pTimerAxis == NULL, " << debugInfo);
            return false;
        }

		// 内部定时器
		if ( pInnerAxis!=NULL )
		{
			return pInnerAxis->SetTimer( timerID,interval,handler,callTimes,debugInfo );
		}

        KillTimer(timerID, handler);

		// 线程共享代理定时器
		TimerHandlerProxy * proxy = new TimerHandlerProxy;
		if ( proxy->Init( handler,timerID,callTimes,debugInfo )==false )
		{
            ErrorLn("SetTimer failed," << debugInfo << ", proxy->Init failed timerID ="<<timerID);
			delete proxy;
			return false;
		}

		// 永远传无限次,否则释放顺序难控制
        return pStubAxis->SetTimerProxy(timerID, interval, proxy, INFINITY_CALL, debugInfo);
	}

	/**
	@purpose          : 删除定时器
	@param	 timerID  : 定时器ID
	@param   handler  : 处理接口
	@return		      : 返回是否删除成功
	*/
	virtual bool KillTimer( DWORD timerID,ITimerHandler * handler )
	{
        TimerService_Stub* pStubAxis = NULL;
		ITimerService    * pInnerAxis = NULL;
		
        if(getTimerAxis(pInnerAxis,pStubAxis)==false)
        {
            ErrorLn("KillTimer failed, pTimerAxis == NULL, ");
            return false;
        }

		// 内部定时器
		if ( pInnerAxis!=NULL )
		{
			return pInnerAxis->KillTimer( timerID,handler );
		}

		// 线程共享代理定时器
		TimerHandlerProxy * proxy = TimerHandlerProxy::CleanProxy(handler,timerID);
		if ( proxy==0 )
			return false;

	    return pStubAxis->KillTimerProxy( timerID,proxy );
    }

	/**
	@purpose          : 检查定时器是否需要触发
	@param timeout    : 单次执行时间片
	*/
	virtual void CheckTimer(ulong timeout = 0)
	{
        ErrorLn("CheckTimer function not defined");
	}

	/**
	取得时间轴对象
	@desc : 有些老系统，例如网关连接器里用的是老时间轴对象，所以还是暴露一个接口允许传出去
	*/
	virtual void * getAxis()
	{
        TimerService_Stub* pStubAxis = NULL;
		ITimerService    * pInnerAxis = NULL;
		
        if(getTimerAxis(pInnerAxis,pStubAxis)==false)
        {
            return 0;
        }

		if ( pInnerAxis ) return pInnerAxis->getAxis();
		else return pStubAxis->getAxis();

        return NULL;
	}

	virtual void release()
	{
		Framework::getInstance().remove_service_timer();
		delete this;
	}
};