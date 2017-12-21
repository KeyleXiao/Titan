/*******************************************************************
** 文件名:	WorkThreadScheduler.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	11/10/2014
** 版  本:	1.0
** 描  述:	工作线程调度程序

			
********************************************************************/


#pragma once

#include "Framework.h"
#include "WorkThread.h"
#include "ThreadSpecialData.h"
#include "Singleton.h"
#include "Thread.h"
#include <vector>
using namespace std;

// 每个线程的运行现场
struct RUNNING_CONTEXT
{
	int                 thread_index;   // 当前线程序号
	Reactor  *          reactor;        // 当前线程的反应器,上层服务可以把事件挂接到反应器上
	FastEvent*          work_event;     // 当前线程的工作事件,事件不触发则线程不工作

	SERVICE_ID          service_id;	    // 正在运行的服务ID
	SERVICE_ID          call_service;   // 正在跨线程调用其他线程的函数
	unsigned long       start_tick;		// 开始运行的时间
	unsigned long       work_time;      // 最近一次工作耗时,以此代表线程当前的繁忙程度
	Atom                emergent_count; // 紧急调用计数

	RUNNING_CONTEXT() : thread_index(-1),service_id(INVALID_SERVICE_ID),start_tick(0),work_time(0),reactor(0),work_event(0){}
};

/**
	工作线程调度程序:
	1.将本地服务分配到各工作线程进行调度
	2.调度程序运行在主线程，这样可以安全的访问各工作线程的生产消费队列
*/

class WorkThreadScheduler : public ThreadPool,public Singleton<WorkThreadScheduler>
{
protected:
	std::vector<WorkThread*>             m_threads;
	TheadSpecialData<RUNNING_CONTEXT *>  m_running;
	int                                  m_nInnerThreadNum;     // 内部可调度线程数,其他的外部调度的工作线程

public:
	WorkThreadScheduler()
		:m_nInnerThreadNum(0)
	{

	}

	virtual ~WorkThreadScheduler()
	{
		stop();
	}

	// 启动调度器 : 需要多少个工作线程
	bool start( int thread_num )
	{
		m_nInnerThreadNum = thread_num;

		for ( int i=0;i<thread_num;++i )
		{
			WorkThread * pThread = new WorkThread(i,true);
			m_threads.push_back(pThread);
			ThreadPool::add(pThread, THREAD_PRIORITY_TIME_CRITICAL);
		}

		ThreadPool::start();
		return true;
	}

	// 停止调度
	void stop()
	{
		for ( int i=0;i<m_threads.size();++i )
		{
			m_threads[i]->get_reactor()->NotifyExit();
		}

		// 释放资源
		typedef std::map<unsigned long, RUNNING_CONTEXT *>  RUNNING_CONTEXT_PTR_MAP;
		RUNNING_CONTEXT_PTR_MAP  mapContext;
		m_running.getList(mapContext);
		for (RUNNING_CONTEXT_PTR_MAP::iterator iter = mapContext.begin(); iter != mapContext.end(); ++iter)
		{
			RUNNING_CONTEXT* &pContext = iter->second;
			if(pContext)
			{
				delete pContext;
				pContext = 0;
			}
		}

		ThreadPool::wait();
		m_threads.clear();
		ThreadPool::clear(); 
	}

	// 调度服务： 将服务安排给某个工作线程进行调度
	bool schedule( SERVICE_PTR pService,int thread_id )
	{
		size_t thread_num = m_threads.size();
		if ( thread_num==0 )
			return false;

		// 如果服务配置了必须在某个工作线程运行
		if ( thread_id>=0 )
		{
			// 配置错误，没有这个线程
			if ( thread_id >= thread_num )
			{
				EmphasisLn("can not find thread by thread id="<<thread_id );
				return false;
			}

			m_threads[thread_id]->add( pService );

			return true;
		}else
		{
            // 有些用户手动调度的线程，是不能自动分配任务的
            int nDispatchNum = min( (int)thread_num,m_nInnerThreadNum );

            // 如果有多个可用线程时，让AI独占一个0号线程
            int nBeginIndex = nDispatchNum > 1 ? 1 : 0;
			// 小于0则表示自动选择空闲的工作线程
			unsigned long nMinResponseTime = 10000;
			int nThreadID = nBeginIndex;

            unsigned int nMinScore = INT_MAX;
			for ( int i=nBeginIndex;i<nDispatchNum;++i )
			{
				//unsigned long nResponseTime = m_threads[i]->get_response_time();
				//if ( nResponseTime<nMinResponseTime )
				//{
				//	nMinResponseTime = nResponseTime;
				//	nThreadID = i;
				//}
    //            else if ( nResponseTime==nMinResponseTime && rand()%4==1 )
				//{
				//	nThreadID = i;
				//}

                unsigned int nScore = m_threads[i]->get_evaluate_score();
                if(nScore < nMinScore)
                {
                    nMinScore = nScore; nThreadID = i;
                }
                //WarningLn(__FUNCTION__": thread_"<<i<<", score="<<nScore <<", minscore="<<nMinScore);
			}
			m_threads[nThreadID]->add( pService );
            //WarningLn(__FUNCTION__": nThreadID="<<nThreadID<<", name="<< pService->get_scheme().scheme->name );
			return true;
		}

		return false;
	}

	// 取消调度
	bool unschedule( SERVICE_PTR pService )
	{
		SERVICE_SCHEME & scheme = pService->get_scheme();
		int & current_thread = scheme.thread_id;

		if ( current_thread>=0 && current_thread<m_threads.size() )
		{
			m_threads[current_thread]->remove(pService);
		}

		return true;
	}

	// 添加一个工作线程序: 系统忙碌时使用
	int add_work_thread( bool bAutoRun )
	{
		WorkThread * pThread = new WorkThread((int)m_threads.size(),bAutoRun);
		m_threads.push_back(pThread);
		ThreadPool::add(pThread, THREAD_PRIORITY_TIME_CRITICAL);

		return (int)m_threads.size() - 1;
	}

	// 切换工作线程： 将某服务从当前工作线程切换到其他工作线程
	bool switch_work_thread( SERVICE_PTR pService,int thread_id )
	{
		unschedule(pService);

		// 重新调度
		return schedule(pService,thread_id);
	}

	// 手动驱动工作线程工作
	void work_thread_run(int nThreadID )
	{
		if ( nThreadID>=m_nInnerThreadNum && nThreadID<m_threads.size() )
		{
			m_threads[nThreadID]->work();
		}
	}

	// 获取线程运行现场信息
	RUNNING_CONTEXT * get_running()
	{
		RUNNING_CONTEXT * pContext = m_running.getData();
		if ( pContext==0 )
		{
			pContext = new RUNNING_CONTEXT;
			m_running.setData(pContext);
		}

		return pContext;
	}

	// 设置当前正在运行的服务
	void set_running_service(SERVICE_ID id)
	{
		RUNNING_CONTEXT * pContext = get_running();

		pContext->service_id = id;
		pContext->call_service= id;
		pContext->start_tick = ((id!=INVALID_SERVICE_ID ) ? getTickCountEx() : 0);
	}

	void release()
	{
		releaseInstance();
	}

	// 检查性能,将执行超时的协程停掉
	void check_performance();

	// 显示调度信息
	const string dump_scheduler_info();

	const string dump_work_services(int thread_id);

    // 上报service运行时信息
    void report_service_runtime_info()
    {
        size_t thread_num = m_threads.size();

        for(int i = 0; i < thread_num; ++i)
        {
            if(m_threads[i] != NULL)
            {
                m_threads[i]->send_report_performance_cmd();
            }
        }
    }
};