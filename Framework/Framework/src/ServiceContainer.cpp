/*******************************************************************
** 文件名:	ServiceContainer.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	11/10/2014
** 版  本:	1.0
** 描  述:	基于简单接口的框架系统

			
********************************************************************/

#include "stdafx.h"
#include "ServiceContainer.h"
#include "WorkThread.h"
#include "LoadLib.h"
#include "WorkThreadScheduler.h"
#include "TimerHandlerProxy.h"
#include "IStackWalk.h"
#include "CallbackProxy.h"
#include "FileSystem\FileStream.h"
#include "intsafe.h"
#include <typeinfo>
#include "StubMacroDef.h"

// void WINAPI ServiceHandleMessageRoutine(void * param)
// {
// 	ServiceContainer * container = (ServiceContainer*)param;
// 
// 	while(true)
// 	{
// 		container->work_routine();
// 	}
// }


/////////////////////////////////////////////////////////////////////////////////////////////////////////

ServiceContainer::ServiceContainer()
    : m_pInstance(0)
    , m_pCoroutine(0)
    , m_wait_session(0)
    , m_session_count(MSG_SESSION_USER)
    , m_response_msg(0)
    , m_id(INVALID_SERVICE_ID)
    , m_handleDll(0)
    , m_nMethodFlag(-1)
    , m_bEmergent(false)
    , m_bLockingThread(-1)
    , m_bFinished(true)
    , m_pStartedCallback(0)
    , m_pStopedCallback(0)
    , m_status(SERVICE_STATUS_STARTING)
#ifdef   FRAMEWORK_DEBUG
    , m_waitStartTick(0)
    , m_statisticTick(0)
    , m_statisticCount(0)
    , m_statisticTotal(0)
    , m_statisticYield(0)
    , m_statisticYieldTotal(0)
    , m_bStatisticRequest(false)
    , m_pInnerTimerAxis(0)
    , m_pReplaceableMQ(0)
    , m_bBlocking(false)
#endif
{
    memset(&m_scheme, 0, sizeof(m_scheme));
}

ServiceContainer::~ServiceContainer()
{
    TraceLn("Destory ServiceContainer service id=" << m_id);

    if (m_pReplaceableMQ)
    {
        // 释放残留内存
        if (m_pReplaceableMQ->size() > 0) {
            TraceLn(__FUNCTION__":[" << m_scheme.scheme->name << ",id=" << m_id<<"] free ReplaceableMQ queue count=" << m_pReplaceableMQ->size());

            for (auto iter = m_pReplaceableMQ->begin(); iter != m_pReplaceableMQ->end(); ++iter) {
                SERVICE_MESSAGE* pMsg = iter->second;
                if (pMsg) {
                    free(pMsg);
                }
            }
        }

        delete m_pReplaceableMQ;
        m_pReplaceableMQ = 0;
    }

    
    // 释放残留内存
    auto free_queue = [&](MESSAGE_QUEUE& queue) {
        SERVICE_MESSAGE * pMessage = 0;
        while (queue.count()>0) {
            if (!queue.pop_ex(pMessage))
                return;

            if (NULL == pMessage || pMessage->session != MSG_SESSION_USER)
                continue;

            free(pMessage);
            pMessage = 0;
        }
    };

    DWORD dwCount = 0;
    dwCount = m_request_queue.count();
    if (dwCount > 0)
    {
        TraceLn(__FUNCTION__":[" << m_scheme.scheme->name << ",id=" << m_id << "] free request queue count=" << dwCount);
        free_queue(m_request_queue);
    }    

    dwCount = m_emergent_queue.count();
    if (dwCount > 0)
    {
        TraceLn(__FUNCTION__":[" << m_scheme.scheme->name << ",id=" << m_id << "] free emergent queue count=" << dwCount);
        free_queue(m_emergent_queue);
    }

    if (m_response_msg)
    {
        free(m_response_msg);
        m_response_msg = 0;
    }

    if (m_pInnerTimerAxis)
    {
        m_pInnerTimerAxis->Close();
        delete m_pInnerTimerAxis;
        m_pInnerTimerAxis = 0;
    }

    if (m_pCoroutine != 0)
    {
        ErrorLn(__FUNCTION__ << ", m_pCoroutine != 0");
        safeRelease(m_pCoroutine);
    }

    //if (m_handleDll)
    //{
    //    freelib(m_handleDll);
    //    m_handleDll = 0;
    //}

    safeRelease(m_pInstance);

    m_pStartedCallback = 0;
    m_pStopedCallback = 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ServiceContainer::start( SERVICE_PTR pService,SERVICE_SCHEME * pScheme,IServiceStub * pInstance,void * pData,int nLen, ServiceStartedCallback pStartedCallback, ServiceStopedCallback pStopedCallback)
{
    if ( (IServiceContainer*)pService!=this )
    {
        ErrorLn(__FUNCTION__ ": input param invalid, service_name=" << m_scheme.scheme->name);
        return false;
    }

    m_scheme = *pScheme;

    // 实例不通过DLL创建由外部传入
    m_pInstance = pInstance;

    // 启动后回调函数
    m_pStartedCallback = pStartedCallback;
    // 停止后回调函数
    m_pStopedCallback = pStopedCallback;      

    // 创建协程
//     m_pCoroutine = new Coroutine_Win;
//     if ( m_pCoroutine==0 || !m_pCoroutine->create(ServiceHandleMessageRoutine,(INT_PTR)this,m_scheme.scheme->stack_size) )
//     {
//         ErrorLn("start service failed!" << m_scheme.scheme->name << " can not crate coroutine." );
//         return false;
//     }

    // 注册服务
    if ( !ServiceMgr::getInstance().register_service(pService) )
    {
        ErrorLn("register service failed!" << m_scheme.scheme->name);
        return false;
    }

    char szLog[256];
    ssprintf_s(szLog, sizeof(szLog), " [this=0x%p, Instance=0x%p, Coroutine=0x%p, thread=%d]", this, m_pInstance, m_pCoroutine, GetCurrentThreadId());
    TraceLn("register service " << m_scheme.scheme->name << " success,id=" << m_id << szLog);


    SERVICE_MESSAGE * pStartMsg = (SERVICE_MESSAGE*)malloc(sizeof(SERVICE_MESSAGE) + nLen);
    pStartMsg->from = INVALID_SERVICE_ID;
    pStartMsg->to = m_id;
    pStartMsg->session = MSG_SESSION_START;
    pStartMsg->is_request = true;
    pStartMsg->need_response = false;
    pStartMsg->context_len = 0;

    if ( nLen>0 && pData!=0 )
    {
        pStartMsg->context_len = nLen;
        memcpy( pStartMsg->context,pData,nLen );
    }

    recv_message(pStartMsg);

#ifdef SUPPORT_NET_NAMEINF
    char szQueueName[512] = { 0 };
    ssprintf_s(szQueueName, sizeof(szQueueName), "%s[request]", m_scheme.scheme->name);
    m_request_queue.setName(szQueueName);
    ssprintf_s(szQueueName, sizeof(szQueueName), "%s[readonly]", m_scheme.scheme->name);
    m_emergent_queue.setName(szQueueName);
#endif

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ServiceContainer::stop()
{
    RUNNING_CONTEXT * pRunning = WorkThreadScheduler::getInstance().get_running();
    bool bNeedResponse = !(pRunning->service_id==m_id); 

	SERVICE_MESSAGE * pEndMsg = (SERVICE_MESSAGE*)malloc(sizeof(SERVICE_MESSAGE));
	pEndMsg->from = pRunning->service_id;
	pEndMsg->to   = m_id;
	pEndMsg->session  = MSG_SESSION_END;
	pEndMsg->is_request = true;
	pEndMsg->need_response = bNeedResponse;
	pEndMsg->context_len   = 0;

	recv_message(pEndMsg);

    // 设置服务当前状态
    m_status = SERVICE_STATUS_STOPING;

    if ( bNeedResponse )
    {
        SERVICE_PTR pRunningService = ServiceMgr::getInstance().get_service(pRunning->service_id);
        if ( pRunningService!=0 )
        {
            ((ServiceContainer*)(IServiceContainer*)pRunningService)->m_bBlocking = true;
            pRunningService->yield(false);
        }
    }

	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceContainer::set_id( SERVICE_ID id )
{
	m_id = id;
}


SERVICE_ID ServiceContainer::get_id()
{
	return m_id;
}

SERVICE_SCHEME & ServiceContainer::get_scheme()
{
	return m_scheme;
}

// 查询接口
void * ServiceContainer::query_interface( int iid )
{
	if ( m_pInstance==0 )
		return 0;

	if ( iid==IID_STUB )
	{
		return m_pInstance;
	}

	return m_pInstance->query_interface(iid);
}


// 插入一个消息
bool ServiceContainer::recv_message(SERVICE_MESSAGE * pMsg, bool bEmergent)
{
    // 注意此处多线程访问这个状态(其他线程post)
    // 有个问题：由于多线程问题,可能判断时还是working，判断后变成了closed，消息入队列，但是这个消息不会被消费，
    // 所以极少数消息里的T_BAG可能会造成泄漏。 读写这个标记加锁性价比又好像不高。。
    if(m_status > SERVICE_STATUS_WORKING)
    {
        free(pMsg);
        return false;
    }

    pMsg->enqueue_tick = getTickCountEx();

    if (pMsg->is_request)
    {
        bool is_add = false;
        if (bEmergent)
            is_add = m_emergent_queue.push_ex(pMsg);
        else
            is_add = m_request_queue.push_ex(pMsg);

        if (!is_add)
        {
            ErrorLn("recv_message failed!from=" << pMsg->from <<",to="<< pMsg->to << ",session=" << pMsg->session<<",len="<<pMsg->context_len);
        }
    }
    else
    {
        if (m_wait_session != pMsg->session)
        {
            ErrorLn("recv_message failed!wait_session=" << m_wait_session << " but message session=" << pMsg->session);
            free(pMsg);
            return false;
        }

        m_response_msg = pMsg;
    }

    // 要通知工作线程工作
    if (m_scheme.work_event)
    {
        ((FastEvent*)m_scheme.work_event)->setEvent();
    }

    // 紧急消息通知
    if (bEmergent)
    {
        RUNNING_CONTEXT * pRunning = WorkThreadScheduler::getInstance().get_running();
        ++(pRunning->emergent_count);
    }

    return true;
}

void ServiceContainer::_handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf )
{
	if ( m_pInstance== 0 )
		return;

#ifdef   FRAMEWORK_DEBUG
	m_waitFuncName = (const char *)(pMsg->context);
#endif

    DWORD dwStartTick = getTickCountEx();

    if ( !m_pInstance->handle_message( pMsg,resultBuf ))
    {
        ErrorLn("handle_message error! please check the param context." << m_scheme.scheme->name << get_message_name(pMsg));
    }

    DWORD dwRunTick = getTickCountEx() - dwStartTick; 
    if(dwRunTick > 1000)
    {
        ErrorLn("handle_message cost " << dwRunTick << " service=" << m_scheme.scheme->name << "! func=" << get_message_name(pMsg) << ", call=" << get_wait_function());
    }
    else if(dwRunTick > 100)
    {
        WarningLn("handle_message cost " << dwRunTick << " service=" << m_scheme.scheme->name << "! func=" << get_message_name(pMsg) << ", call=" << get_wait_function());
    }
    else if(dwRunTick > 50)
    {
        TraceLn("handle_message cost " << dwRunTick << " service=" << m_scheme.scheme->name << "! func=" << get_message_name(pMsg) << ", call=" << get_wait_function());
    }
}

// 取得一个消息的名称
const char * ServiceContainer::get_message_name( SERVICE_MESSAGE * pMsg )
{
	if ( pMsg->session==MSG_SESSION_CALLBACK )
		return pMsg->context ? pMsg->context : "";//"callback";

	if ( pMsg->function_id==0 )
		return "";

#ifdef VERSEION_DEBUG
    return pMsg->szName;
#else
    return "";
#endif	
}

// 执行一个消息
bool ServiceContainer::_do_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf,bool wait)
{
	if ( m_pInstance== 0 )
		return false;

	DWORD dwTick = getTickCountEx();

	while(m_bLockingFlag.cas(0,1)==false)
	{
		// 同线程可重入
		if ( m_bLockingThread==GetCurrentThreadId() )
		{
            if ( pMsg->session==MSG_SESSION_CALLBACK )
            {
                ICallbackProxy * pProxy = (ICallbackProxy *)(INT_PTR)(pMsg->to);
                pProxy->Do(pMsg);
                return true;
            }

            if ( pMsg->session==MSG_SESSION_END )
            {
                if ( m_pInstance ) m_pInstance->on_stop();
                return true;
            }

			_handle_message(pMsg,resultBuf);
			return true;
		}

		if ( wait==false )
		{
			// 不需要等待，仅仅尝试下能不能直接调用
			return false;
		}
		else
		{
			DWORD dwRunTick = getTickCountEx() - dwTick ;
			if ( dwRunTick > 100000 )
			{
				ErrorLn("do_message dead-locked! func=" << get_message_name(pMsg) << " service="<<m_scheme.scheme->name << ", call=" << get_wait_function());
				break;
			}

			if ( dwRunTick > 1000 )
			{
				ErrorLn("do_message cost " << dwRunTick << " service=" << m_scheme.scheme->name << "! func=" << get_message_name(pMsg) << ", call=" << get_wait_function());
			}
			else if(dwRunTick > 50)
			{
				WarningLn("do_message cost " << dwRunTick << "service=" << m_scheme.scheme->name << "! func=" << get_message_name(pMsg) << ", call=" << get_wait_function());
			}

			yield(false);
		}
	}

	m_bLockingThread = GetCurrentThreadId();
   
    if ( pMsg->session==MSG_SESSION_CALLBACK )
    {
        DWORD dwDoTick = getTickCountEx();
        ICallbackProxy * pProxy = (ICallbackProxy *)(INT_PTR)(pMsg->to);
        pProxy->Do(pMsg);

        DWORD dwDoRunTick = getTickCountEx() - dwDoTick;
        if (dwDoRunTick > 1000)
        {
            ErrorLn("_do_message::Do cost " << dwDoRunTick << "ms, service=" << m_scheme.scheme->name << "! func=" << get_message_name(pMsg) << ", call=" << get_wait_function());
        }
    }
    else if ( pMsg->session==MSG_SESSION_END )
    {
        if ( m_pInstance ) m_pInstance->on_stop();
    }
    else
    {
        _handle_message(pMsg,resultBuf);
    }

	m_bLockingThread = -1;
	m_bLockingFlag.cas(1,0);

	return true; 
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 挂起
void ServiceContainer::yield(bool bFinish)
{
	if ( m_pCoroutine )
	{
		if ( bFinish==false )
		{
			// 要通知工作线程工作(要补一个事件，不然可能再也激活不了)
			if ( m_scheme.work_event )
			{
				((FastEvent*)m_scheme.work_event)->setEvent();
			}
		}

        m_bLockingFlag.cas(1,0); //阻塞时允许重入，否则会死锁

		DWORD dwTick = getTickCountEx();
		
		m_pCoroutine->yield();
		
#ifdef   FRAMEWORK_DEBUG
        if (bFinish == false) {
            m_statisticYield += getTickCountEx() - dwTick;
        }
#endif
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 容器状态
int ServiceContainer::get_coroutine_status()
{
	if (m_pCoroutine)
	{
		return m_pCoroutine->status();
	}
	
	return CO_STATUS_EMPTY;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 处理消息
bool ServiceContainer::work(void* pWorkThread)
{
    if ( m_bBlocking )
    {
        return false;
    }

	// 没请求就不要调度了，避免一次空的切换，浪费CPU
	if ( m_pReplaceableMQ==0 && m_request_queue.count()==0 && m_pCoroutine==0 )
		return false;

    WorkThread* pThread = (WorkThread*)pWorkThread;

    if(!m_pCoroutine)
    {
        m_pCoroutine = pThread->allot_co(m_scheme.scheme->stack_size);
        if(m_pCoroutine == NULL)
        {
            ErrorLn(__FUNCTION__ << " m_pCoroutine == NULL");
            return false;
        }
        m_pCoroutine->set_task(this);
    }

    WorkThreadScheduler::getInstance().set_running_service(m_id);
    m_pCoroutine->resume();
    if(m_pCoroutine->is_idle())
    {
        pThread->revert_co(m_pCoroutine);
        m_pCoroutine = 0;
    }
	WorkThreadScheduler::getInstance().set_running_service(INVALID_SERVICE_ID);

	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 协程调用 : 处理消息包
void ServiceContainer::work_routine()
{
	RUNNING_CONTEXT * pRunning = WorkThreadScheduler::getInstance().get_running();

    _do_replaceable_message();

    SERVICE_MESSAGE * pMessage = 0;
    while( m_request_queue.count()>0 && m_status!=SERVICE_STATUS_CLOSED )
    {
        // 执行时间片超时，交回控制权
        if ( m_scheme.scheme->time_slice>0 )
        {
            if ( getTickCountEx()-pRunning->start_tick > m_scheme.scheme->time_slice)
            {
                return;	
            }
        }

        if( !m_request_queue.pop_ex(pMessage) )
        {
            ErrorLn(__FUNCTION__ << "request_queue pop error");
            return;
        }
        
#ifdef   FRAMEWORK_DEBUG
        // 统计数量
        if ( getTickCountEx()-m_statisticTick > 10000 )
        {
            m_statisticTick = getTickCountEx();
            m_statisticTotal= m_statisticCount;
            m_statisticYieldTotal=m_statisticYield;
            m_statisticCount= 0;
            m_statisticYield = 0;

			// 打印统计信息
			if ( m_bStatisticRequest )
			{
				ErrorLn( m_scheme.scheme->name << " request statistic:===================" ); 
				std::map<std::string,int>::iterator it = m_statisticRequest.begin();
				for ( ;it!=m_statisticRequest.end();++it )
				{
					if ( it->second>5 )
						ErrorLn( "            " << it->first << ":" << it->second );
				}
				m_statisticRequest.clear();
				m_bStatisticRequest = false;
			}

			size_t dwPackCounts = m_request_queue.count();
			if(m_scheme.scheme->watch_min>0 && dwPackCounts > m_scheme.scheme->watch_min )
			{
				if (m_scheme.scheme->watch_max > 0 && dwPackCounts > m_scheme.scheme->watch_max)
				{
					ErrorLn(__FUNCTION__":[" << m_scheme.scheme->name << "]Queue busying, count=" << dwPackCounts << ", thread_id="<<m_scheme.thread_id<<", id=" << m_id);
					// 统计请求数
					m_bStatisticRequest = true;
				}
				else
				{
					WarningLn(__FUNCTION__":[" << m_scheme.scheme->name << "]Queue busying, count=" << dwPackCounts << ", thread_id="<<m_scheme.thread_id<< ", id=" << m_id);
				}
			}
        }else
        {
            m_statisticCount++;
        }

		// 统计请求
		if ( m_bStatisticRequest )
		{
			const char * szMsgName = get_message_name(pMessage);
			int num = m_statisticRequest[szMsgName];
			m_statisticRequest[szMsgName] = num + 1;
		}
#endif

        // 内部消息自己处理
        if ( pMessage->session!=MSG_SESSION_USER )
        {
            if ( handle_inner_message(pMessage) )
            {
                continue;
            }
        }

        // 外部消息交回给上层处理
        if ( m_pInstance )
        {
//#ifdef		OPEN_BVTTEST
//            char szPName[128] = {0};
//            ssprintf_s(szPName, sizeof(szPName), "%s:%d", m_scheme.scheme->name, m_id);
//            PP_BY_NAME_START(szPName);
//#endif
            _do_message(pMessage,0,true);
//
//#ifdef		OPEN_BVTTEST
//            PP_BY_NAME_STOP(szPName);
//#endif
        }

        free(pMessage);
        pMessage = 0;
    }
}

bool ServiceContainer::handle_emergent_message()
{
	RUNNING_CONTEXT * pRunning = WorkThreadScheduler::getInstance().get_running();

	// 处理下一个消息
	SERVICE_MESSAGE * pMessage = 0;
	if ( m_emergent_queue.pop(pMessage) )
	{
		void * pResult = 0;
		int nResultLen = 0;

		// 内部消息自己处理
		if ( pMessage->session!=MSG_SESSION_USER )
		{
			if ( handle_inner_message(pMessage) )
			{
				return true;
			}
		}

		// 外部消息交回给上层处理
		_do_message(pMessage,0);
		
		// 减少计数
		--(pRunning->emergent_count);

		free(pMessage);
		pMessage = 0;

		return true;
	}

	return false;
}

// 处理内部消息
bool ServiceContainer::handle_inner_message( SERVICE_MESSAGE * pMsg )
{
	// 这样用户层收到start调用时是处在正确的线程
	if ( pMsg->session==MSG_SESSION_START )
	{
		if ( m_pInstance==0 )
		{
			// 载入DLL
			void* handle = loadlib(m_scheme.scheme->dll_path);
			if ( handle==0 )
			{
				ErrorLn("load service ["<<m_scheme.scheme->name <<"] from dll [" << m_scheme.scheme->dll_path << "]failed!");
				stop();
				free(pMsg);
				return true;
			}

			typedef IServiceStub* (RKT_CDECL *procCreateService)();
			procCreateService create_func = (procCreateService)getProcAddress(handle,m_scheme.scheme->create_func);
			if ( create_func==0 )
			{
				ErrorLn("load service ["<<m_scheme.scheme->name <<"] from dll [" << m_scheme.scheme->dll_path << ":" << m_scheme.scheme->create_func << "]failed!");
				stop();
				free(pMsg);
				return true;
			}

			// 创建实例
			m_pInstance = create_func();
			if ( m_pInstance==0 )
			{
				ErrorLn("create service instance failed!" << m_scheme.scheme->create_func );
				stop();
				free(pMsg);
				return true;
			}

            char szLog[256] = {0};
            ssprintf_s(szLog, sizeof(szLog), " [this=0x%p, m_pInstance=0x%p, m_pCoroutine=0x%p]", this, m_pInstance, m_pCoroutine);
            TraceLn("create instance " << m_scheme.scheme->name << " success,service id=" << m_id << szLog);

			m_handleDll = handle;
		}

		// 启动内部定时器
		if ( m_scheme.scheme->inner_timer )
		{
			SimpleTimerAxis * pAxis = new SimpleTimerAxis;
			if ( pAxis->Create() )
			{
				m_pInnerTimerAxis = pAxis;
			}
		}

		if ( !m_pInstance->on_start(get_selfptr(),pMsg->context,pMsg->context_len ) )
		{
			ErrorLn("[service stub]start service failed!"<<m_scheme.scheme->name);
			stop();
			free(pMsg);
			return true;
		}

        // 启动后回调函数
        if(m_pStartedCallback)
        {
            SERVICE_PTR ptrService(this);
            m_pStartedCallback(ptrService);
        }

        // 状态设置为工作状态
        m_status = SERVICE_STATUS_WORKING;

		free(pMsg);

		return true;
	}

	// 这样用户层收到stop调用时是处在正确的线程
	if ( pMsg->session==MSG_SESSION_END )
	{
        // 等消息队列都消费完 再stop
        if( m_pReplaceableMQ!=0 || m_request_queue.count()!=0 )
        {
            m_request_queue.push_ex(pMsg);
            //recv_message(pMsg);
            return true;
        }

        _do_message(pMsg, 0);

        if ( pMsg->need_response )
        {
            ServiceContainer* pCall = (ServiceContainer*)(IServiceContainer*)ServiceMgr::getInstance().get_service( pMsg->from );
            if ( pCall!= NULL ) pCall->m_bBlocking = false;
        }

        free(pMsg);

        // ?? 容器析构之前不释放instance 析构函数里有release这里只屏蔽一下
		//safeRelease(m_pInstance);

		// 卸载dll
		// freelib(m_handleDll);

        // 停止后回调函数
        if(m_pStopedCallback)
            m_pStopedCallback();

		// 释放内部时间轴
		if ( m_pInnerTimerAxis )
		{
			SimpleTimerAxis * pAxis = m_pInnerTimerAxis;
			m_pInnerTimerAxis = 0;

			pAxis->Close();
			delete pAxis;
		}

        // 设置状态
        m_status = SERVICE_STATUS_CLOSED;

		TraceLn("unregister service " << m_scheme.scheme->name << ",id=" << m_id);
		ServiceMgr::getInstance().unregister_service( get_selfptr() );

		// 注意: 上面注销本容器对象之后，容器对象很可能已经销毁，下面就不能再访问成员变量啦

		return true;
	}

	// 定时器临时方案
	if ( pMsg->session==MSG_SESSION_CALLBACK )
	{
		_do_message(pMsg, 0);
		
		return true;
	}

	return false;
}

// 强制设置下一个消息的投递方法,发完一个消息后会自动回位
void  ServiceContainer::switch_method_flag( int nFlag )
{
	m_nMethodFlag = nFlag;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ServiceContainer::_post_message( void * pContext,size_t nContextLen,rkt::obuf* resultBuf, int nFlag )
{
	int nMsgLen = sizeof(SERVICE_MESSAGE)+nContextLen;
	SERVICE_MESSAGE * pMsg = (SERVICE_MESSAGE*)malloc(sizeof(SERVICE_MESSAGE)+nContextLen);
	

	if ( nContextLen>0 )
	{
        pMsg->context_len   = nContextLen;
		memcpy( pMsg->context,pContext,nContextLen );
	}else
	{
		pMsg->context_len   = 0;
	}

	// 同于上层代码强制改变消息的默认调用方式
	if ( m_nMethodFlag>=0 )
	{
		nFlag = m_nMethodFlag;
		m_nMethodFlag = -1;
	}

	ServiceContainer * pCall = 0;
	if ( nFlag & MSG_FLAG_NO_BLOCK )
	{
		// 上层知道没有返回,此时不用报警
		return post_request(pCall,pMsg,nMsgLen,resultBuf,nFlag);
	}else
	{
		RUNNING_CONTEXT * pRunning = WorkThreadScheduler::getInstance().get_running();
		SERVICE_PTR pServiceCall = ServiceMgr::getInstance().get_service(pRunning->service_id);
		pCall = (ServiceContainer *)(IServiceContainer*)pServiceCall;

		if ( pCall==0 )
		{ 
			WarningLn("post message to " << m_scheme.scheme->name << " but can not find who call it.");
		}

		// 这个函数是从代理那调过来的，所以跟我们正常调用的方向是反的，需要调转一下
		SERVICE_ID call_service = pRunning->call_service;
		pRunning->call_service = m_id;
		bool bResult = post_request(pCall,pMsg,nMsgLen,resultBuf,nFlag);
		pRunning->call_service = call_service;

		return bResult;
	}
}

// 投递一个消息(高性能版：注意参数约定)
// @param pMsg  : 必须是malloc出来的消息对象,这样可以减少一次内存拷贝
// @param nMsgLen:包含消息内容的长度,nMsgLen必须等于sizeof(SERVICE_MESSAGE)+pMsg->context_len
// @param nFlag : 该标志位
bool ServiceContainer::post_message( SERVICE_MESSAGE * pMsg,size_t nMsgLen,rkt::obuf* resultBuf,int nFlag )
{
    if ( pMsg==0 )
	{
		return false;
	}

	// 同于上层代码强制改变消息的默认调用方式
	if ( m_nMethodFlag>=0 )
	{
		nFlag = m_nMethodFlag;
		m_nMethodFlag = -1;
	}

	ServiceContainer * pCall = 0;
	if ( nFlag & MSG_FLAG_NO_BLOCK )
	{
		// 上层知道没有返回,此时不用报警
		return post_request(pCall,pMsg,nMsgLen,resultBuf,nFlag);
	}else
	{
		RUNNING_CONTEXT * pRunning = WorkThreadScheduler::getInstance().get_running();
		SERVICE_PTR pServiceCall = ServiceMgr::getInstance().get_service(pRunning->service_id);
		pCall = (ServiceContainer *)(IServiceContainer*)pServiceCall;

		if ( pCall==0 )
		{ 
            WarningLn("post message to " << m_scheme.scheme->name << " but can not find who call it.");
		}

		// 这个函数是从代理那调过来的，所以跟我们正常调用的方向是反的，需要调转一下
		SERVICE_ID call_service = pRunning->call_service;
		pRunning->call_service = m_id;
		bool bResult = post_request(pCall,pMsg,nMsgLen,resultBuf,nFlag);
		pRunning->call_service = call_service;

		return bResult;
	}
}

// 投递一个可被替换的消息
bool ServiceContainer::post_replaceable_message( unsigned long ulKey,SERVICE_MESSAGE * pMsg,size_t nMsgLen )
{
    if(m_status > SERVICE_STATUS_WORKING)
    {
        free(pMsg);
        return false;
    }

    // 检测长度越界
	if ( pMsg->context_len+sizeof(SERVICE_MESSAGE)!=nMsgLen )
	{
		ErrorLn("post_replaceable_message param error! nMsgLen="<< nMsgLen << " context_len=" << pMsg->context_len );
		return false;
	}

    pMsg->enqueue_tick = getTickCountEx();
    pMsg->from = INVALID_SERVICE_ID;
    pMsg->to   = m_id;
    pMsg->session = MSG_SESSION_USER;
    pMsg->is_request = true;
    pMsg->need_response = false;

    m_lockReplaceableMQ.lock();
    if(m_pReplaceableMQ == 0)
    {
        m_pReplaceableMQ = new std::map<unsigned long, SERVICE_MESSAGE*>();
    }
    std::map<unsigned long, SERVICE_MESSAGE*>& mapQ = *m_pReplaceableMQ; 

    SERVICE_MESSAGE* pOldMsg = mapQ[ulKey];
    if(pOldMsg != 0)
    {
        free(pOldMsg);
        pOldMsg = 0;
    }
    mapQ[ulKey] = pMsg;

    m_lockReplaceableMQ.unlock();

    return true;
}

bool ServiceContainer::post_request( ServiceContainer * pRequest,SERVICE_MESSAGE * pMsg,size_t nMsgLen,rkt::obuf* resultBuf,int nFlag )
{
    // 检测长度越界
	if ( pMsg->context_len+sizeof(SERVICE_MESSAGE)!=nMsgLen )
	{
		ErrorLn("post_message param error! nMsgLen="<< nMsgLen << " context_len=" << pMsg->context_len );
		return false;
	}

	bool need_response = (nFlag & MSG_FLAG_NO_BLOCK) ? false : true; 
    
    pMsg->enqueue_tick = getTickCountEx(); // 有的不放入队列，直接do_message的

	if ( pRequest!=0 )
	{
		// 处理回绕问题
		++pRequest->m_session_count;
		if ( pRequest->m_session_count<MSG_SESSION_USER )
			pRequest->m_session_count = MSG_SESSION_USER;

		pMsg->from = pRequest->m_id;
		pMsg->to   = m_id;
		pMsg->session = pRequest->m_session_count;
		pMsg->is_request = true;
		pMsg->need_response = need_response;
	}else
	{
		pMsg->from = INVALID_SERVICE_ID;
		pMsg->to   = m_id;
		pMsg->session = MSG_SESSION_USER;
		pMsg->is_request = true;
		pMsg->need_response = false;
		need_response = false;
	}

	// 同一个对象
	if ( pMsg->to==pMsg->from )
	{
		_handle_message(pMsg,resultBuf);
		free(pMsg);
		return true;
	}

	// 本地服务 
	if ( pMsg->to.cloud==CloudService::getInstance().get_id() )
	{
		if ( nFlag & MSG_FLAG_THREAD_SAFE )
		{
			// 该函数内部做了线程安全,可以多线程直接调用
            _handle_message(pMsg,resultBuf);
            free(pMsg);
            return true;
		}

		if ( nFlag & MSG_FLAG_THREAD_MUTEX )
		{
			// 该函数可以跨线程调用，但每个时刻只能有一个线程执行
			if ( pRequest!=0 )
			{
				DWORD dwTick = getTickCountEx();
				while( _do_message(pMsg,resultBuf,false)==false )
				{
                    if ( getTickCountEx() - dwTick > 100000 )
                    {
                        char szErr[128];
                        ssprintf_s(szErr, sizeof(szErr), "post request dead-locked! func=%s, call=%s", get_message_name(pMsg), pRequest->m_scheme.scheme->name);
                        ErrorLn(szErr);
                        break;
                    }

					if ( getTickCountEx() - dwTick > 100 )
					{
						char szErr[128];
                        ssprintf_s(szErr, sizeof(szErr), "post request locked! func=%s, call=%s", get_message_name(pMsg), pRequest->m_scheme.scheme->name);
						WarningLn(szErr);
						continue;
                    }

					pRequest->yield(false);

                    RUNNING_CONTEXT * pRunning = WorkThreadScheduler::getInstance().get_running();
                    pRunning->call_service = m_id;
				}

                free(pMsg);
                return true;
			}
		}

        return recv_message(pMsg,nFlag & MSG_FLAG_EMERGENT); 
	}

	return false;
}

// 投递一个回应
bool ServiceContainer::post_response( int session,SERVICE_PTR pService,void * pContext,size_t nContextLen )
{
	SERVICE_MESSAGE * pMsg = (SERVICE_MESSAGE*)malloc(sizeof(SERVICE_MESSAGE)+nContextLen);
	pMsg->from = m_id;
	pMsg->to   = pService->get_id();
	pMsg->session = session;
	pMsg->is_request = false;
	pMsg->need_response = false;
	pMsg->context_len   = nContextLen;
	memcpy( pMsg->context,pContext,nContextLen );

	// 本地服务 
	if ( pMsg->to.cloud==CloudService::getInstance().get_id() )
	{
		pService->recv_message(pMsg,true);

		// 注意:::放到消息队列之后,pMsg就可能被另外一个线程释放了，不要再使用 
		//pMsg = 0;
	}else
	{
		// 不同进程
	}

	return true;
}

void ServiceContainer::_do_replaceable_message()
{
    if(m_pReplaceableMQ == 0)
    {
        return;
    }

    RUNNING_CONTEXT * pRunning = WorkThreadScheduler::getInstance().get_running();

    m_lockReplaceableMQ.lock();
    std::map<unsigned long, SERVICE_MESSAGE*>* pMapQ = m_pReplaceableMQ;
    m_pReplaceableMQ = 0;
    m_lockReplaceableMQ.unlock();

    for(std::map<unsigned long, SERVICE_MESSAGE*>::iterator it = (*pMapQ).begin();
        it != (*pMapQ).end(); ++it)
    {
        SERVICE_MESSAGE* pMsg = it->second;
        if(pMsg != 0)
        {
            _do_message(pMsg, 0);
            free(pMsg);
        } 

        // 执行时间片超时，交回控制权
        if ( m_scheme.scheme->time_slice>0 )
        {
            if ( getTickCountEx()-pRunning->start_tick > m_scheme.scheme->time_slice)
            {
                yield(true);
            }
        }
    }
    delete pMapQ; 
}

void ServiceContainer::release()
{
    delete this;
}