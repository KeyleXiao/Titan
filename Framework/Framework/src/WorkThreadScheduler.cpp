/*******************************************************************
** 文件名:	WorkThreadScheduler.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	11/10/2014
** 版  本:	1.0
** 描  述:	工作线程调度程序

			
********************************************************************/

#include "stdafx.h"
#include "WorkThreadScheduler.h"
#include "ServiceMgr.h"

void WorkThreadScheduler::check_performance()
{
	//TheadSpecialData<RUNNING_CONTEXT *>::DATA_LIST snapshot;
	//m_running.getList(snapshot);

	//unsigned long cur_tick = getTickCountEx();
	//unsigned long time_out = 0;

	//TheadSpecialData<RUNNING_CONTEXT *>::DATA_LIST::iterator it = snapshot.begin();
	//for ( ;it!=snapshot.end();++it )
	//{
	//	RUNNING_CONTEXT * pContext = it->second;
	//	time_out = pContext->start_tick==0 ? 0 : cur_tick-pContext->start_tick;
	//	if ( time_out >= 32 && pContext->service_id!=INVALID_SERVICE_ID )
	//	{
	//		SERVICE_PTR pContainer = ServiceMgr::getInstance().get_service(pContext->service_id);
	//		if ( pContainer==0 )
	//			continue;

	//		// 确实超时了就挂起
	//		unsigned long time_slice = pContainer->get_scheme().scheme->time_slice;
	//		if ( time_slice!=0 && time_out >= time_slice )
	//		{
 //               if(time_out >= 2*time_slice)
 //               {
	//			    ErrorLn("[" << pContainer->get_scheme().scheme->name << _GT("]执行超时，被系统挂起")
	//				    <<",function="<<pContainer->get_wait_function()<<",timeout="<< time_out);
 //               }
 //               else
 //               {
 //                   WarningLn("[" << pContainer->get_scheme().scheme->name << _GT("]执行超时，被系统挂起")
 //                       <<",function="<<pContainer->get_wait_function()<<",timeout="<< time_out);
 //               }
	//			//pContainer->yield();
	//		}
	//	}
	//}
}

// 显示调度信息
const string WorkThreadScheduler::dump_scheduler_info()
{
	TheadSpecialData<RUNNING_CONTEXT *>::DATA_LIST snapshot;
	m_running.getList(snapshot);

	obuf1024 buffer;

	TheadSpecialData<RUNNING_CONTEXT *>::DATA_LIST::iterator it = snapshot.begin();
	for ( ;it!=snapshot.end();++it )
	{
		RUNNING_CONTEXT * pContext = it->second;

		buffer << *pContext;
	}

	return string(buffer.data(),buffer.size());
}

// 这个地方其实有安全隐患，先用于测试...
const string WorkThreadScheduler::dump_work_services(int thread_id)
{
	obuf1024 buffer;

	if ( thread_id>=0 && thread_id<m_threads.size())
	{
		buffer << thread_id;

		std::set<SERVICE_PTR> & service_list = m_threads[thread_id]->get_list();

		std::set<SERVICE_PTR>::iterator it = service_list.begin();
		for ( ;it!=service_list.end();++it )
		{
			SERVICE_PTR pService = *it;
			buffer << pService->get_scheme().scheme->name;
		}
	}

	return string(buffer.data(),buffer.size());
}
