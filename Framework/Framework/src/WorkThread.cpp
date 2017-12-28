/*******************************************************************
** 文件名:	WorkThread.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	11/10/2014
** 版  本:	1.0
** 描  述:	工作线程

			
********************************************************************/

#include "stdafx.h"
#include "WorkThread.h"
#include "WorkThreadScheduler.h"
#include "Framework.h"
#include "IServiceMgr.h"
#include "IPerformanceService.h"
#include "CoroutineWin.h"

void WorkThread::run()
{
	m_pReactor = rkt::CreateReactor();
	
	if ( m_pReactor==0 )
	{
		ErrorLn("CreateReactor failed! WorkThread Initialize failed!");
		return;
	}

	m_pReactor->AddEvent( m_event.getHandle() );
    m_pReactor->RegisterEventHandler( m_event.getHandle(),this, __FUNCTION__);

	// 初始化线程现场
	RUNNING_CONTEXT * pThreadContext = WorkThreadScheduler::getInstance().get_running();
	pThreadContext->thread_index = m_thread_id;
	pThreadContext->reactor      = m_pReactor;
	pThreadContext->work_event   = &m_event;

    // 转换到纤程模式
    LPVOID fiber = ::ConvertThreadToFiber(this);

	if ( m_bAutoRun )
	{
		// 反应器循环，有事件来才处理
		m_pReactor->HandleEventsLoop();

		release();
	}

    ConvertFiberToThread();
}

void WorkThread::OnEvent( HANDLE event )
{
    DWORD dwStartTick = getTickCountEx();

	RUNNING_CONTEXT * pRunning = WorkThreadScheduler::getInstance().get_running();

	// 先将事件复位
	m_event.resetEvent();

	// 处理添加消息
	SERVICE_PTR pNeedAdd = SERVICE_PTR(0);
	while( m_add_queue.pop(pNeedAdd) )
	{
		pNeedAdd->get_scheme().thread_id = m_thread_id;
		pNeedAdd->get_scheme().work_event= (void*)&m_event;
		m_service_list.insert(pNeedAdd);
	}

	// 处理删除消息
	SERVICE_PTR pNeedRemove = SERVICE_PTR(0);
	while( m_remove_queue.pop(pNeedRemove) )
	{
		if ( pNeedRemove==0 ) continue;

		pNeedRemove->get_scheme().thread_id = -1;
		pNeedRemove->get_scheme().work_event= 0;
		m_service_list.erase(pNeedRemove);
	}


	// 处理消息
	std::set<SERVICE_PTR>::iterator it = m_service_list.begin();
	for ( ;it!=m_service_list.end();++it )
	{
		// 优先处理紧急任务
		//if ( (long)pRunning->emergent_count>0 )
		//{
		//	std::set<SERVICE_PTR>::iterator em_it = m_service_list.begin();
		//	for ( ;em_it!=m_service_list.end();++em_it )
		//	{
		//		(*em_it)->work(true);

		//		if ( (long)pRunning->emergent_count==0 )
		//			break;
		//	}
		//}

		(*it)->work(this);
	}

    m_nResponseTime = getTickCountEx()-dwStartTick;

    if(m_nResponseTime > 1000)
    {
        WarningLn(__FUNCTION__": workthread execute slow, threadid="<< m_thread_id <<", tick="<< m_nResponseTime <<"ms, servicecount="<<m_service_list.size());
    }
    else if(m_nResponseTime > 2000)
    {
        ErrorLn(__FUNCTION__": workthread Perform too slow slow! threadid="<< m_thread_id <<", tick="<< m_nResponseTime <<"ms, servicecount="<<m_service_list.size());
    }

    // 有上报运行时信息命令
    if( m_bSendPerformance )
    {
        report_performance();
        m_bSendPerformance = false;
    }
}

void WorkThread::send_report_performance_cmd()
{
    m_bSendPerformance = true;
}

// 上报给性能服务器
void WorkThread::report_performance()
{
    SERVICE_PTR pContainer = Framework::getInstance().get_service_manager()->find_service("PerformanceService");
    if(pContainer == 0)
    {
        return;
    }

    IPerformanceService* pPerformaceService = (IPerformanceService*)pContainer->query_interface();
    if(pPerformaceService == 0)
    {
        return;
    }

    THREAD_RUNTIME_INFO threadRti;
    threadRti.nCoCount = m_nCoCount;

    for(std::set<SERVICE_PTR>::iterator it = m_service_list.begin(); it != m_service_list.end(); ++it)
    {
        if(*it == 0)
        {
            continue;
        }

        SERVICE_RUNTIME_INFO sInfo;
        sInfo.service_id = (*it)->get_id();
        memcpy(sInfo.name, (*it)->get_scheme().scheme->name, sizeof(sInfo.name));
        sInfo.queue_len = (*it)->get_request_queue_count();
        sInfo.process_rate = (*it)->get_process_rate();
        threadRti.vecServiceRti.push_back(sInfo);
    }

    pPerformaceService->receive_thread_report(m_thread_id, threadRti);
}

ICoroutine* WorkThread::allot_co(int nStackSize)
{
    std::map<int, std::list<ICoroutine*>>::iterator itFreeList = m_co_free_list.find(nStackSize); 
    if(itFreeList != m_co_free_list.end() && !itFreeList->second.empty())
    {
        ICoroutine* pReturn = itFreeList->second.front();
        itFreeList->second.pop_front();
        return pReturn;
    }

    ICoroutine* pCo = new Coroutine_Win;
    if(pCo == NULL)
    {
        ErrorLn(__FUNCTION__ << " pCo == NULL");
        return NULL;
    }

    if(!pCo->create(nStackSize))
    {
        ErrorLn(__FUNCTION__ << " create coroutine failed");
        pCo->release();
        return NULL;
    }

    ++m_nCoCount;

    return pCo;
}

#define co_free_list_max_size 30
void WorkThread::revert_co(ICoroutine* pCo)
{
    int nStackSize = pCo->get_stack_size();

    std::list<ICoroutine*>& freeList = m_co_free_list[nStackSize];
    freeList.push_back(pCo);

    while(freeList.size() > co_free_list_max_size)
    {
        freeList.front()->release();
        freeList.pop_front();

        --m_nCoCount;
    }
}

void WorkThread::release()
{
	// 销毁
	SERVICE_PTR pNeedAdd = SERVICE_PTR(0);
	while( m_add_queue.pop(pNeedAdd) )
	{
		m_service_list.insert(pNeedAdd);
	}

	// 处理消息
	std::set<SERVICE_PTR>::iterator it = m_service_list.begin();
	for ( ;it!=m_service_list.end();++it )
	{
		(*it)->get_scheme().thread_id = -1;
	}

    char szAddr[64] = {0};
    sprintf_s(szAddr, "0x%p", this);
    EmphasisLn(__FILE__ << ", " << __FUNCTION__ << " destroy, addr=" << szAddr);

	m_service_list.clear();
	delete this;
}

// 获取当前线程权重数
unsigned int WorkThread::get_evaluate_score()
{
    unsigned int nScore  = 0;

    // 获取配置分数
    std::set<SERVICE_PTR>::iterator it = m_service_list.begin();
    for ( ;it!=m_service_list.end();++it )
    {        
        nScore += (*it)->get_scheme().scheme->evaluate_score;
    }

    // 线程响应时间
    static unsigned long s_response_threshold[]     = { 50, 100, 200, 300, 400, 500, 700,   1000,  10000,  20000    };
    static int s_threshold_score[]                  = { 10, 15,  20,  25,  30,  50,  100,   200,   300,    400      };

    for(BYTE i=0; i<sizeof(s_response_threshold)/ sizeof(int); ++i)
    {
        if(m_nResponseTime <= s_response_threshold[i])
        {
            nScore += s_threshold_score[i];
            break;
        }
    }

    return nScore;
}