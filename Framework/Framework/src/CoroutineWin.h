/*******************************************************************
** 文件名:	Coroutine.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	11/12/2014
** 版  本:	1.0
** 描  述:	协程类

			
********************************************************************/

#pragma once

#include "Coroutine.h"

/**
	协程（Coroutine）是一种模型：
	本质就是一段代码在独立的运行堆栈上运行，如果任务无法立即完成，则挂起，下次接着上次运行的堆栈继续往下运行
	1.结合多线程的模型，可以即获得多线程的并发效率，又能兼顾到单线程的习惯逻辑
	2.即每一个服务自己维护一个Coroutine,阻塞了就通知工作线程切换到下一个Coroutine执行，这样就有点类似Erlang的用户进程
	  切换起来性能非常高
	3.C++实现Coroutine的方式：参考(http://blog.codingnow.com/2005/10/fiber.html)
	       1).Windows: Fiber
		   2).Linux: setcontext (https://github.com/cloudwu/coroutine/)
	4.其他编程语言,Lua,C#等基本都有Coroutine(http://blog.codingnow.com/2012/06/continuation_in_lua_52.html)
	5.最开始的思路是用C++写服务，用Lua_state保存运行环境，相当于用lua虚拟机实现协程效果了
*/

VOID CALLBACK DefaultCoProc( PVOID* pCoroutine );


// Windows下的Coroutine实现

class Coroutine_Win : public ICoroutine
{
public:
    ICoTask* m_task;
protected:
	LPVOID   m_fiber;
	LPVOID   m_last_fiber;             // 上一个纤程的地址,因为等会要切回去
	INT_PTR  m_param;
	int      m_status;
    int      m_stack_size;

public:
	Coroutine_Win() : m_fiber(0),m_last_fiber(0),m_param(0),m_status(CO_STATUS_EMPTY),m_task(0),m_stack_size(0){}

	virtual ~Coroutine_Win(){}

	virtual bool create(coroutine_func entry,INT_PTR param,int stack_size)
	{
		m_fiber = CreateFiberEx(
			256,
			stack_size,
			0,//FIBER_FLAG_FLOAT_SWITCH,//XP系统不支持FIBER_FLAG_FLOAT_SWITCH
			entry,
			(LPVOID)param
			);

		if ( m_fiber==0 )
		{
			ErrorLn("create fiber failed! error="<<GetLastError());
			return false;
		}

		m_status= CO_STATUS_CREATED;
		m_param = param;
        m_stack_size = stack_size;
		return true;
	}

    virtual bool create(int stack_size)
    {
        m_fiber = CreateFiberEx(
            256,
            stack_size,
            0,//FIBER_FLAG_FLOAT_SWITCH,//XP系统不支持FIBER_FLAG_FLOAT_SWITCH
            (LPFIBER_START_ROUTINE)DefaultCoProc,
            (LPVOID)this
            );

        if ( m_fiber==0 )
        {
            ErrorLn("create fiber failed! error="<<GetLastError());
            return false;
        }

        m_status= CO_STATUS_CREATED;
        m_param = 0;
        return true;
    }

    virtual void set_task(ICoTask* pTask)
    {
        m_task = pTask;
    }

    virtual bool is_idle()
    {
        return m_task == 0;
    }

	virtual bool resume()
	{
		if ( m_fiber==0 )
		{
			return false;
		}
		
		m_status= CO_STATUS_RUNNING;
		m_last_fiber = GetCurrentFiber();
		if ( m_last_fiber==0 || m_last_fiber==m_fiber)
		{
			ErrorLn("Get Current Fiber failed!m_last_fiber=" << (int)m_last_fiber << " error=" << GetLastError() );
		}
		SwitchToFiber(m_fiber);
		return true;
	}

	virtual int status()
	{
		return m_status;
	}

	virtual void yield()
	{
		m_status= CO_STATUS_YIELD;
		LPVOID fiber = m_last_fiber;
		if ( fiber==0 )
			return;
		
		m_last_fiber = 0;
		SwitchToFiber(fiber);	
	}

	virtual INT_PTR get_param()
	{
		return m_param;
	}

	// 当前正在运行的纤程
	virtual void * running()
	{
		return GetFiberData();
	}

    virtual int get_stack_size()
    {
        return m_stack_size;
    }

	// 销毁
	virtual void release()
	{
		if ( m_fiber )
		{
			DeleteFiber(m_fiber);
			m_fiber = 0;
		}

		m_param  = 0;
		m_status = CO_STATUS_EMPTY;

		delete this;
	}
};

VOID CALLBACK DefaultCoProc( PVOID* pCoroutine )
{
    if( pCoroutine==NULL )
    {
        return;
    }

    Coroutine_Win* pCo = (Coroutine_Win*)pCoroutine;

    do
    {
        if( pCo->m_task )
        {
            unsigned long ulCode = 0;

            __try
            {   
                pCo->m_task->work_routine();
            }
            __except(ulCode = GetExceptionCode(), createStackWalk()->miniDump(GetExceptionInformation(), 1800), EXCEPTION_EXECUTE_HANDLER) //出现了异常, 记录异常的code, 生成dump!!
            {
                char szMsg[256] = {0};
                ssprintf_s(szMsg, sizeof(szMsg), "ERROR: Application  Crash(Exception Code:%x)!!\r\n", ulCode);
                rkt::outputAlert(AlertLevel_Bad, MG_AlertSrcType_Crash, szMsg, __FILE__, __LINE__);
            }
        }

        pCo->m_task = 0;
        pCo->yield();
    }while( 1 );
}