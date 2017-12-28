/*******************************************************************
** 文件名:	ServiceContainer.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	11/10/2014
** 版  本:	1.0
** 描  述:	基于简单接口的框架系统

			
********************************************************************/

#pragma once

#include "share_ptr.h"
#include "IServiceContainer.h"
#include "net/producer_consumer_queue.h"
#include "Coroutine.h"
#include "ServiceMgr.h"
#include "Thread.h"
#include "SimpleTimerAxis.h"
#include "SimpleDateTriggerAxis.h"
#include <map>
#include <string>
using namespace rkt;


/**
	服务容器:
	1.框架根据配置创建容器，容器负责与框架进行底层通讯，收到调用请求再转交给应用层提供的服务存根进行处理
	2.代理/存根，从COM架构借鉴过来的名词，应用层调用某个服务的接口，实际交由代理封包成消息，再提交给框架进行转发
	            而框架里的容器收到请求消息，则需要调用存根进行解释和真正处理
*/

class ServiceContainer : public IServiceContainer, public ICoTask
{
protected:
    ICoroutine *     m_pCoroutine;              // 内部协程
	IServiceStub *   m_pInstance;				// 真正的服务实例
	
	typedef producer_consumer_queue_ex<SERVICE_MESSAGE *>   MESSAGE_QUEUE;
    
    std::map<unsigned long, SERVICE_MESSAGE*>* m_pReplaceableMQ;  // 某些(AI移动)消息直接放到这里 防止堆积 
    CAtomLock               m_lockReplaceableMQ;    // 互斥m_pReplaceableMessageQ
	MESSAGE_QUEUE			m_request_queue;		// 请求队列 : 其他服务需要调用我的函数
	MESSAGE_QUEUE           m_emergent_queue;       // 紧急队列 ：需要立即响应的调用

	Atom                    m_bLockingFlag;         // 其他线程是否正在执行一个函数
	int                     m_bLockingThread;       // 当前执行的线程ID，相同线程可重入

	SERVICE_SCHEME			m_scheme;				// 配置信息

	int						m_session_count;		// 递增的会话ID
	int						m_wait_session;			// 等待某个会话的返回
	SERVICE_MESSAGE			*m_response_msg;		// 函数调用返回结果
	SERVICE_ID				m_id;
	void*					m_handleDll;			// dll句柄

	int                     m_nMethodFlag;          // 下一次投递采用什么方法?异步 or 同步,<0则无效
	bool                    m_bEmergent;            // 是不是紧急处理
    bool                    m_bFinished;            // 上次domsg是否完成了，可能被挂起了。

    MY_SERVICE_STATUS       m_status;               // 服务状态

    ServiceStartedCallback  m_pStartedCallback;     // 启动后回调函数
    ServiceStopedCallback   m_pStopedCallback;      // 停止后回调函数

	SimpleTimerAxis *       m_pInnerTimerAxis;      // 内部时间轴，不需要跨线程

    bool                    m_bBlocking;            // 是否阻塞中

#ifdef   FRAMEWORK_DEBUG
	std::string             m_waitFuncName;         // 等待的函数
	unsigned                m_waitStartTick;        // 等待开始时间
	DWORD                   m_statisticTick;        // 统计每秒处理数据包吞吐量
	DWORD                   m_statisticCount;
	DWORD                   m_statisticTotal;
	DWORD                   m_statisticYield;     // 每秒阻塞的时间
	DWORD                   m_statisticYieldTotal;// 每秒阻塞的时间
	std::map<std::string,int>  m_statisticRequest;// 统计请求的函数数量
	bool                       m_bStatisticRequest;//是否开启请求统计
#endif

public:

    ServiceContainer();

    virtual ~ServiceContainer();

	// 紧急调用 ：需要优先处理
	bool handle_emergent_message();

	// 启动
	virtual bool start( SERVICE_PTR pService, SERVICE_SCHEME * pScheme,IServiceStub * pInstance=0,void * pData=0,int nLen=0, ServiceStartedCallback pStartedCallback=0, ServiceStopedCallback pStopedCallback=0);

	// 停止
	virtual bool stop();

	// 设置ID
	virtual void set_id( SERVICE_ID id );

	// 取得ID
	virtual SERVICE_ID get_id();

	// 获取配置信息
	virtual SERVICE_SCHEME & get_scheme();

	// 查询接口
	virtual void * query_interface( int iid );

	// 投递一个消息
	virtual bool _post_message( void * pMsg,size_t nMsgLen,rkt::obuf* resultBuf,int nFlag );

	// 投递一个消息(高性能版：注意参数约定)
	// @param pMsg  : 必须是malloc出来的消息对象,这样可以减少一次内存拷贝
	// @param nMsgLen:包含消息内容的长度,nMsgLen必须等于sizeof(SERVICE_MESSAGE)+pMsg->context_len
	// @param nFlag : 该标志位
	virtual bool post_message( SERVICE_MESSAGE * pMsg,size_t nMsgLen,rkt::obuf* resultBuf,int nFlag=MSG_FLAG_DEFAULT ) ;

    // 投递一个可被替换的消息
    virtual bool post_replaceable_message( unsigned long ulKey,SERVICE_MESSAGE * pMsg,size_t nMsgLen );

	// 投递一个请求
	bool post_request( ServiceContainer * pRequest,SERVICE_MESSAGE * pMsg,size_t nMsgLen,rkt::obuf* resultBuf,int nFlag );

	// 投递一个回应
	bool post_response( int session,SERVICE_PTR pService,void * pMsg,size_t nMsgLen );

	// 处理内部消息
	bool handle_inner_message( SERVICE_MESSAGE * pMsg );

	// 收到一个消息
	virtual bool recv_message( SERVICE_MESSAGE * pMsg,bool bEmergent=false );

	// 强制设置下一个消息的投递方法,发完一个消息后会自动回位
	virtual void  switch_method_flag( int nFlag );

	// 工作线程让我工作
	virtual bool work(void* pWorkThread);

	// 协程调用 : 处理消息包
	void work_routine();

	// 挂起 : 任务全部完成了bFinish=true,否则为false
	virtual void yield(bool bFinish=true);

    // 获取状态
    virtual MY_SERVICE_STATUS get_status() { return m_status; }

    // for test
    virtual int get_request_queue_count(){ return m_request_queue.count();}

    // 获取处理包速率(/s) for test
    virtual int get_process_rate() { return m_statisticTotal;}

    virtual void release();

#ifdef  FRAMEWORK_DEBUG
	virtual const char* get_wait_function() { return m_waitFuncName.c_str();}
#endif

	inline SERVICE_PTR get_selfptr()
	{
		return ServiceMgr::getInstance().get_service(m_id);
	}

	// 取得内部时间轴
	virtual ITimerService* get_inner_timeraxis()
	{
		return m_pInnerTimerAxis;
	}

private:
    // 容器状态
    virtual int get_coroutine_status();

	// 执行一个消息
	bool _do_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf,bool wait=true );

	void _handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf );

	const char * get_message_name( SERVICE_MESSAGE * pMsg );

    void _do_replaceable_message();
};
