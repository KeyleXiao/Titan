/*******************************************************************
** 文件名:	PerformanceService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	18/10/2014
** 版  本:	1.0
** 描  述:	性能服务(打印每个线程的压力，优化调度策略等)

			
********************************************************************/


#pragma once

#include "IPerformanceService.h"
#include "ITimerService.h"
#include "TimerHandler.h"
#include "WorkThreadScheduler.h"
#include "ServiceMgr.h"
#include "TimerService.h"

/**
	性能服务:
	1.打印每个线程的压力
    2.将超时的调用挂机
	3.线程负载均衡策略等
*/
class PerformanceService : public IPerformanceService,public TimerHandler
{
private:
    std::map<int, THREAD_RUNTIME_INFO> m_thread_runtime_info;

public:
	virtual const string snapshot_work_threads()
	{
		return WorkThreadScheduler::getInstance().dump_scheduler_info();
	}

	virtual const string snapshot_work_services(int thread_id)
	{
		return WorkThreadScheduler::getInstance().dump_work_services(thread_id);
	}

    virtual void receive_thread_report(int thread_id, THREAD_RUNTIME_INFO& refThreadRti)
    {
        m_thread_runtime_info[thread_id] = refThreadRti; 

        dump_thread_runtime_info(thread_id);
    }

    virtual void dump_thread_runtime_info(int thread_id)
    {
        std::map<int, THREAD_RUNTIME_INFO>::iterator itFind = m_thread_runtime_info.find(thread_id);
        if(itFind == m_thread_runtime_info.end())
        {
            return;
        }

       
        WarningLn("[thread id=" << thread_id << ", service count=" << itFind->second.vecServiceRti.size() << ", coroutine_count=" << itFind->second.nCoCount << "]");
        for(std::vector<SERVICE_RUNTIME_INFO>::iterator it = itFind->second.vecServiceRti.begin(); it != itFind->second.vecServiceRti.end(); ++it)
        {
            if(it->queue_len > 0)
            {
                ErrorLn("******service: id=" << it->service_id << " ,name=" << it->name << ", queue_count=" << it->queue_len << ", process_rate(/s)=" << it->process_rate);
            }
            else
            {
                WarningLn("******service: id=" << it->service_id << " ,name=" << it->name << ", queue_count=" << it->queue_len << ", process_rate(/s)=" << it->process_rate);
             }
        }
    }

	// 启动
	bool on_start()
	{
		//if ( !TimerService::getInstance().SetTimer(0,32,this,INFINITY_CALL,"PerformanceService" ) )
		//{
		//	ErrorLn("settimer failed!PerformanceService Start Failed!");
		//	return false;
		//}

		return true;
	}

	// 停止
	bool on_stop()
	{
		//TimerService::getInstance().KillTimer(0,this );

		return true;
	}

    // 这样打印 正式情况打印信息很多-
	virtual void OnTimer( unsigned long dwTimerID )
	{
		//WorkThreadScheduler::getInstance().check_performance();
	}
};
