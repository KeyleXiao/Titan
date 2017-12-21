/*******************************************************************
** 文件名:	PerformanceService_Stub.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	
** 日  期:	
** 版  本:	1.0
** 描  述:	本文件为根据IPerformanceService接口定义，自动生成的PerformanceService接口
            代理文件，该文件是为符合Framework对接口进行的包装

			
********************************************************************/

#pragma once

/**
	服务容器:
	1.框架根据配置创建容器，容器负责与框架进行底层通讯，收到调用请求再转交给应用层提供的服务存根进行处理
	2.代理/存根，从COM架构借鉴过来的名词，应用层调用某个服务的接口，实际交由代理封包成消息，再提交给框架进行转发
	            而框架里的容器收到请求消息，则需要调用存根进行解释和真正处理
*/

#include <string>

#include "IServiceContainer.h"
#include "IPerformanceService.h"
#include "PerformanceService.h"
#include "buffer.h"


using namespace rkt;
using namespace std;

/** 
   代理对象:
   该代理实现IPerformanceService接口，把接口函数调用封装为框架系统的消息转发
   对于外界使用接口的其他类来说，这个过程是无缝对接的
*/
class PerformanceService_Proxy : public IPerformanceService
{
public:
	SERVICE_PTR   m_pContainer;


	PerformanceService_Proxy() : m_pContainer(0){
	}

	virtual ~PerformanceService_Proxy(){
	}

    virtual const string  snapshot_work_threads(  )
    {
	    BUILD_MSG_CONTEXT_VOID( IPerformanceService::snapshot_work_threads);

	    rkt::obuf resultBuf;
	    m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
	    if ( resultBuf.good() && resultBuf.size() > 0  )
	    {
			ibuffer data(resultBuf.data(),resultBuf.size());
			std::string vReturn;
			data >> vReturn;
		    return std::string(vReturn);
	    }

		return "";
    };

	virtual const string snapshot_work_services(int thread_id)
	{
		BUILD_MSG_CONTEXT( IPerformanceService::snapshot_work_services,int);
		*pParam = thread_id;

		rkt::obuf resultBuf;
		m_pContainer->post_message( pMsg,nMsgLen,&resultBuf);
		if ( resultBuf.good() && resultBuf.size() > 0  )
		{
			ibuffer data(resultBuf.data(),resultBuf.size());
			std::string vReturn;
			data >> vReturn;
			return std::string(vReturn);
		}

		return "";
	}

    virtual void receive_thread_report(int thread_id, THREAD_RUNTIME_INFO& refThreadRti)
    {
        // 这包有可能很大
        obuf256 data;
        data << thread_id;
        data << refThreadRti.nCoCount;
        data << refThreadRti.vecServiceRti.size();
        for(std::vector<SERVICE_RUNTIME_INFO>::iterator it = refThreadRti.vecServiceRti.begin(); 
            it != refThreadRti.vecServiceRti.end(); ++it)
        {
            data << *it;
        }

		BUILD_MSG_BUFFER( IPerformanceService::receive_thread_report,data );

        rkt::obuf resultBuf;
        m_pContainer->post_message( pMsg,nMsgLen,&resultBuf, MSG_FLAG_NO_BLOCK);
    }
};

/** 
   存根对象:
   1.处理收到的消息调用请求，然后把他转发给真正的服务接口函数
*/
class PerformanceService_Stub : public IServiceStub
{
	PerformanceService_Proxy  m_proxy;
	PerformanceService *      m_real_service;

public:
	PerformanceService_Stub(PerformanceService * pReal) : m_real_service(pReal){
	}

	// 启动
	virtual bool on_start( SERVICE_PTR pContainer,void * data,int len )
	{
		m_proxy.m_pContainer = pContainer;

		return m_real_service->on_start();
	}

	// 停止
	virtual bool on_stop()
	{
		bool bResult = m_real_service->on_stop();
        m_proxy.m_pContainer = SERVICE_PTR(0);
        return bResult;
	}

	// 处理消息
	virtual bool handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf )
	{
		if ( TEST_SAME_FUNCTION(IPerformanceService::snapshot_work_threads) )
		{
			const string & vReturn= m_real_service->snapshot_work_threads();
			if ( resultBuf ) *resultBuf << vReturn;
			return true;
		}

		if ( TEST_SAME_FUNCTION(IPerformanceService::snapshot_work_services) )
		{
			int thread_id = 0;
			GET_MSG_PARAM_1(int,thread_id);

			const string & vReturn= m_real_service->snapshot_work_services(thread_id);
			if ( resultBuf ) *resultBuf << vReturn;
			return true;
		}

        if ( TEST_SAME_FUNCTION(IPerformanceService::receive_thread_report) )
        {
			rkt::ibuffer in(pMsg->context,pMsg->context_len);

            THREAD_RUNTIME_INFO threadRti;

            int thread_id; int size;
            in >> thread_id >> threadRti.nCoCount >> size;

            std::vector<SERVICE_RUNTIME_INFO> vecList;
            for(int i = 0; i < size; ++i)
            {
                SERVICE_RUNTIME_INFO sInfo;
                in >> sInfo;
                threadRti.vecServiceRti.push_back(sInfo);
            }

            m_real_service->receive_thread_report(thread_id, threadRti);
            return true;
        }

		return false;
	}

	// 返回代理对象即可
	virtual void * query_interface( int iid )
	{
		return &m_proxy;
	}

	// 销毁
	virtual void release()
	{
		if ( m_real_service )
		{
			delete m_real_service;
			m_real_service = 0;
		}

		delete this;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreatePerformanceService()
{
	return new PerformanceService_Stub(new PerformanceService);
}