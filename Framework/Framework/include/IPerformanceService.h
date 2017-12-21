/*******************************************************************
** 文件名:	IPerformanceService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	18/10/2014
** 版  本:	1.0
** 描  述:	性能服务(打印每个线程的压力，优化调度策略等)

			
********************************************************************/


#pragma once

#include <string>
using namespace std;

/**
	性能服务:
	1.打印每个线程的压力
    2.将超时的调用挂机
	3.线程负载均衡策略等
*/
struct IPerformanceService
{
	virtual const string snapshot_work_threads() = 0;

	virtual const string snapshot_work_services(int thread_id) = 0;

    virtual void receive_thread_report(int thread_id, THREAD_RUNTIME_INFO& refThreadRti) = 0;
};
