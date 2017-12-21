/*******************************************************************
** 文件名:	DateTriggerService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	18/10/2014
** 版  本:	1.0
** 描  述:	日期触发器服务

			
********************************************************************/

#pragma once

#include "IDateTriggerService.h"
#include "ThreadSpecialData.h"
#include "Singleton.h"
#include "IServiceContainer.h"
#include "Framework.h"
#include "WorkThreadScheduler.h"
#include "DateTriggerHandlerProxy.h"
#include "Simple_Atom_Lock.h"
#include "DateTriggerService_Stub.h"
#include "WorkThreadScheduler.h"
using namespace rkt;

#define THREAD_MAX 256

class DateTriggerService : public IDateTriggerService,public Singleton<DateTriggerService>
{
public:

    DateTriggerService_Stub* m_thread_date_trigger_axis[THREAD_MAX];

public:
	DateTriggerService() 
    {   
        memset(m_thread_date_trigger_axis, 0, THREAD_MAX * sizeof(DateTriggerService_Stub*));
    }

    // 为工作线程创建DateTriggerService
    void createDateTriggerService(int nThreadID)
    {
        if(nThreadID >= THREAD_MAX)
        {
            ErrorLn("thread id out of range: id=" << nThreadID << ", please redefine THREAD_MAX");
            return;
        }

        if(m_thread_date_trigger_axis[nThreadID] != NULL)
        {
            ErrorLn("createDateTriggerService for thread,id=" << nThreadID << " failed, already created");
            return;
        }

        SERVICE_SCHEME * scheme = Framework::getInstance().get_service_scheme("DateTriggerService");
        if(scheme == NULL)
        {
            ErrorLn("DateTriggerService start failed, scheme not found!");
            return;
        }

        SERVICE_SCHEME my_scheme;
        my_scheme.scheme = scheme->scheme;
        my_scheme.thread_id = nThreadID;

        SERVICE_PTR ptrService(new ServiceContainer);
        DateTriggerService_Stub* pStubDateTrigger = new DateTriggerService_Stub;
        if(ptrService->start(ptrService,&my_scheme,pStubDateTrigger))
        {
            m_thread_date_trigger_axis[nThreadID] = pStubDateTrigger;
        }
        else
        {
            ErrorLn("start StubDateTrigger service failed, thread_id=" << nThreadID);
            delete pStubDateTrigger; pStubDateTrigger = NULL;
        }
    }

    // 根据call_service获取该线程上的时间轴
    bool getDateTriggerAxis(DateTriggerService_Stub*& pStub)
    {
        RUNNING_CONTEXT* pRunning = WorkThreadScheduler::getInstance().get_running();
        SERVICE_PTR pContainer = Framework::getInstance().get_service_manager()->get_service(pRunning->call_service);
        if(pContainer == 0)
        {
            __debugbreak();
            ErrorLn("getDateTriggerAxis failed,cant find service, call_service=" << pRunning->call_service);
            return false;
        }

        if(pContainer->get_scheme().thread_id >= THREAD_MAX)
        {
            ErrorLn("getDateTriggerAxis failed, thread_id out of range: thread id=" << pContainer->get_scheme().thread_id);
            return false;
        }

        pStub = m_thread_date_trigger_axis[pContainer->get_scheme().thread_id];
        
        if(pStub == NULL)
        {
            ErrorLn("getDateTriggerAxis failed, thread_id=" << pContainer->get_scheme().thread_id);
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
	virtual bool RegisterDateTrigger(int nTriggerID, IDateHandler* pDateHander, SDateTriggerInfo sTriggerInfo, const char * debugInfo=0 )
    {
        DateTriggerService_Stub* pStubAxis = NULL;
		
        if(getDateTriggerAxis(pStubAxis)==false)
        {
            ErrorLn("getDateTriggerAxis failed, pStubAxis == NULL, " << debugInfo);
            return false;
        }

        UnRegisterDateTrigger(nTriggerID, pDateHander);

		// 线程共享代理定时器
        bool bRepeatFlag = getRepeatFlag(&sTriggerInfo);
		DateHandlerProxy * proxy = new DateHandlerProxy;
		if ( proxy->Init( pDateHander, bRepeatFlag, nTriggerID, debugInfo )==false )
		{
            ErrorLn("RegisterDateTrigger failed," << debugInfo << ", proxy->Init failed timerID ="<<nTriggerID);
			delete proxy;
			return false;
		}

        return pStubAxis->RegisterDateTriggerProxy(nTriggerID,  proxy, sTriggerInfo, debugInfo);
	}

	/**
	@purpose          : 删除定时器
	@param	 timerID  : 定时器ID
	@param   handler  : 处理接口
	@return		      : 返回是否删除成功
	*/
	virtual bool UnRegisterDateTrigger(int nTriggerID, IDateHandler* pDateHander)
	{
        DateTriggerService_Stub* pStubAxis = NULL;
		
        if(getDateTriggerAxis(pStubAxis)==false)
        {
            ErrorLn("UnRegisterDateTrigger failed, pTimerAxis == NULL, ");
            return false;
        }

		// 线程共享代理定时器
		DateHandlerProxy * proxy = DateHandlerProxy::CleanProxy(pDateHander,nTriggerID);
		if ( proxy==0 )
			return false;

	    return pStubAxis->UnRegisterDateTriggerProxy( nTriggerID, proxy );
    }

	/**
	@purpose          : 检查定时器是否需要触发
	@param timeout    : 单次执行时间片
	*/
	virtual void CheckTimer(ulong timeout = 0)
	{
        ErrorLn("CheckTimer function not defined");
	}

	virtual void release()
	{
		Framework::getInstance().remove_service_datetrigger();
		delete this;
	}

    /**
	@purpose          : 检查定时器是否需要重复触发

	*/
	virtual bool getRepeatFlag(SDateTriggerInfo *sTriggerInfo)
	{
        return (sTriggerInfo->day == -1     || sTriggerInfo->year == -1    || 
                sTriggerInfo->month == -1   || sTriggerInfo->day == -1     ||
                sTriggerInfo->hour == -1    || sTriggerInfo->wday== -1);
	}
};