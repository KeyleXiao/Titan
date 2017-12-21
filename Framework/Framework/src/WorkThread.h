/*******************************************************************
** 文件名:	WorkThread.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	11/10/2014
** 版  本:	1.0
** 描  述:	工作线程

			
********************************************************************/

#pragma once

#include "Thread.h"
#include "IServiceContainer.h"
#include "net/producer_consumer_queue.h"
#include "Net/Reactor.h"
#include "Net/AsynIoFrame.h"
#include <set>
using namespace rkt;


struct ICoroutine;

/**
	工作线程:
	1.逐个调用分配到该工作线程上的服务的消息处理函数
*/
class WorkThread : public IRunnable,public EventHandler
{
protected:
	producer_consumer_queue<SERVICE_PTR>    m_add_queue;
	producer_consumer_queue<SERVICE_PTR>    m_remove_queue;

	std::set<SERVICE_PTR>				   m_service_list;

	int                                    m_thread_id;			// 当前线程ID	
	unsigned long                          m_nResponseTime;     // 工作线程响应时间，用来判断是否忙碌
	bool                                   m_bAutoRun;	        // 是自动运行，还是由外部驱动

	Reactor  *                             m_pReactor;          // 线程反应器,只有事件触发时才工作
	FastEvent                              m_event;             // 驱动工作线程的事件

    std::map<int, std::list<ICoroutine*>>  m_co_free_list;      // 空闲协程列表
    
    int                                    m_nCoCount;

    // for test
    bool                                    m_bSendPerformance;

	WorkThread() : m_thread_id(0),m_bAutoRun(false),m_nResponseTime(0),m_pReactor(0),m_bSendPerformance(false),m_nCoCount(0){}

public:
	WorkThread( int id,bool bAutoRun) : m_thread_id(id),m_bAutoRun(bAutoRun),m_nResponseTime(0),m_pReactor(0),m_bSendPerformance(false),m_nCoCount(0)
	{
	}

	virtual ~WorkThread()
	{
		if ( m_pReactor )
		{
			m_pReactor->Release();
			m_pReactor = 0;
		}
	}

	virtual void OnEvent( HANDLE event );

	// for test
	std::set<SERVICE_PTR> & get_list() { return   m_service_list; }

	// 添加一个服务到这个工作线程上
	void add( SERVICE_PTR pService )
	{
		m_add_queue.push(pService);
#ifdef SUPPORT_NET_NAMEINF
		m_add_queue.setName(pService->get_scheme().scheme->name);
#endif
		m_event.setEvent();
	}

	// 从该工作线程移除一个服务
	void remove( SERVICE_PTR pService )
	{
		m_remove_queue.push(pService);
#ifdef SUPPORT_NET_NAMEINF
		m_remove_queue.setName(pService->get_scheme().scheme->name);
#endif

		// 这行代码纯粹是因为producer_consumer_queue的特殊实现方式而加的
		// 因为producer_consumer_queue总是会保留最后一个节点的引用
		SERVICE_PTR ptr = SERVICE_PTR(0);
		m_remove_queue.push(ptr);
		m_event.setEvent();
	}

	// 手动驱动工作线程工作
	void work()
	{
        ::ConvertThreadToFiber(this);
		OnEvent(0);
        ConvertFiberToThread();
	}

	// 取得响应时间，用来判断线程是否忙碌
	unsigned long get_response_time()
	{
		return m_nResponseTime;
	}

	virtual void run();

	virtual void release();

	Reactor* get_reactor(){return m_pReactor;}

    void send_report_performance_cmd();

	// 分配一个可用协程
    ICoroutine* allot_co(int nStackSize = 32 * 1024);

	// 交还一个协程,如果别的服务器需要运行，再从这里取
    void revert_co(ICoroutine* pCo);

    // 获取当前线程权重数
    unsigned int get_evaluate_score();

private:
    void report_performance();
};