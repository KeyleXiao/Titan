/*******************************************************************
** 文件名:	Framework.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	11/13/2014
** 版  本:	1.0
** 描  述:	基于简单接口的框架系统

			
********************************************************************/

#include "stdafx.h"

#include <map>
#include <string>
#include "Framework.h"
#include "ServiceMgr.h"
#include "ServiceContainer.h"
#include "WorkThreadScheduler.h"
#include "CloudService.h"
#include "tinyxml.h"
#include "PerformanceService_Stub.h"
#include "TimerService.h"
#include "Simple_Atom_Lock.h"
#include "EventService.h"
#include "DBEngineProxy.h"
#include "PropertySet.h"
#include "DateTriggerService.h"

using namespace std;

bool Framework::start( const char * path, bool isMulitThreadMode )
{
    m_isMulitThreadMode = isMulitThreadMode;

	if ( !doc.LoadFile(path) )
	{
		ErrorLn("load xml file [" << path << "] failed!" << doc.ErrorDesc() );
		return false;
	}

	TiXmlElement * root = doc.FirstChildElement("Services");
	if ( root==0 )
	{
		ErrorLn("find root element in file [" << path << "] failed!" );
		return false;
	}

	// 启动工作线程
	int thread_num = 1;
    // 非强制单线程读取配置
    if(m_isMulitThreadMode)
    {
	    root->QueryIntAttribute("work_thread_num",&thread_num);
    }
    else
    {
        EmphasisLn(__FUNCTION__":Open service force with a single thread! thread_num="<<thread_num);
    }

	if ( !WorkThreadScheduler::getInstance().start(thread_num) )
	{
		ErrorLn("start work thread scheduler failed! thread num" << thread_num << GetLastError() );
		return false;
	}

	// 载入配置列表中的服务
	TiXmlElement * pService = root->FirstChildElement("Service");
	for (;pService;pService=pService->NextSiblingElement("Service"))
	{
		SERVICE_SCHEME_STATIC & scheme = *(new SERVICE_SCHEME_STATIC);
		memset(&scheme,0,sizeof(scheme));
		scheme.pExtend = pService;

		sstrcpyn( scheme.create_func,pService->Attribute("create_func"),sizeof(scheme.create_func) );
#ifdef _WIN64
		if (pService->Attribute("dll_path_64"))
			sstrcpyn( scheme.dll_path,pService->Attribute("dll_path_64"),sizeof(scheme.dll_path));
		else
			sstrcpyn( scheme.dll_path,pService->Attribute("dll_path"),sizeof(scheme.dll_path));
#else
		sstrcpyn( scheme.dll_path,pService->Attribute("dll_path"),sizeof(scheme.dll_path));
#endif
		sstrcpyn( scheme.name,pService->Attribute("name"),sizeof(scheme.name));

		pService->QueryIntAttribute("stack_size",&scheme.stack_size);
		pService->QueryIntAttribute("time_slice",&scheme.time_slice);

		int thread_safe = 0;
		pService->QueryIntAttribute("thread_safe",&thread_safe);
		scheme.thread_safe = thread_safe==1;

		int inner_timer = 0;
		pService->QueryIntAttribute("inner_timer",&inner_timer);
		scheme.inner_timer = inner_timer==1;
		
		int watch_min = 0;
		pService->QueryIntAttribute("watch_min", &watch_min);
		int watch_max = 0;
		pService->QueryIntAttribute("watch_max", &watch_max);
		scheme.watch_min = min(watch_min, watch_max);
		scheme.watch_max = max(watch_min, watch_max);

        int evaluate_score = 10;
		pService->QueryIntAttribute("score", &evaluate_score);
        scheme.evaluate_score = evaluate_score;

		SERVICE_SCHEME data;
		data.scheme = &scheme;
		data.thread_id = -1;
        // 强制单线程时，所有的线程ID位0
        if(!m_isMulitThreadMode) {
            data.thread_id = 0;
        }else{
            pService->QueryIntAttribute("thread_id",&data.thread_id);
        }
		
		pService->QueryIntAttribute("is_prefab",&data.is_prefab);
 
		// 保存配置信息
        Simple_Atom_Lock loc(&m_mutex);
		m_scheme_map.insert( SCHEME_MAP::value_type(scheme.name,data));
	}
    
    // 优先为每个工作线程创建timerservice，不然有些service启动就settimer会失败
    for(int i = 0; i < thread_num; ++i)
    {
        TimerService::getInstance().createTimerService(i);
        DateTriggerService::getInstance().createDateTriggerService(i);
    }

    for(SCHEME_MAP::iterator it = m_scheme_map.begin(); it != m_scheme_map.end(); ++it)
    {
        // 如果是预制的就表示暂时不要创建实例,后面会动态创建
        if(it->second.is_prefab != 1)
        {
            if(m_isMulitThreadMode)
            {
                // 是否新开并且独占工作线程
                int newthread = 0;
                ((TiXmlElement*)it->second.scheme->pExtend)->QueryIntAttribute("newthread", &newthread);
                if(newthread != 0)
                {
                    it->second.thread_id = add_work_thread(true);

                    // 为这个新开的线程创建timer
                    TimerService::getInstance().createTimerService(it->second.thread_id);
                    DateTriggerService::getInstance().createDateTriggerService(it->second.thread_id);
                }
            }

            SERVICE_PTR ptrService(new ServiceContainer);
            if ( !ptrService->start(ptrService,&it->second,0) )
            {
                ErrorLn("start service [" << it->second.scheme->name << "] failed!" << GetLastError() );
                return false;
            }
        }
    }

	TraceLn("start framework success!" );
	return true;
}

// 手动创建一个服务
SERVICE_PTR Framework::create_service( SERVICE_SCHEME * scheme,IServiceStub * pStub,void * pData,int nLen, ServiceStartedCallback pStartedCallback, ServiceStopedCallback pStopedCallback )
{
    // 服务是否配置了新开并且独占线程
    if(m_isMulitThreadMode)
    {
        int newthread = 0;
        ((TiXmlElement*)scheme->scheme->pExtend)->QueryIntAttribute("newthread", &newthread);
        if(newthread != 0)
        {
            scheme->thread_id = add_work_thread(true);

            // 为这个新开的线程创建timer
            TimerService::getInstance().createTimerService(scheme->thread_id);

            DateTriggerService::getInstance().createDateTriggerService(scheme->thread_id);
        }
    }


	SERVICE_PTR ptrService(new ServiceContainer);

	if ( !ptrService->start(ptrService,scheme,pStub,pData,nLen, pStartedCallback, pStopedCallback) )
	{
		ErrorLn("start service [" << scheme->scheme->name << "] failed!" << GetLastError() );
		ptrService = SERVICE_PTR(0);
	}
	return ptrService;
}

// 创建一个安全的属性集，这样简单的属性设置和获取就不需要通过post_message来调用
// @param pDataChunk : 外部传入属性集保存的指针，这样内部就不需要分配了,否则会分配一个bytes字节的内存
IPropertySet * Framework::create_propertyset( size_t bytes,void * pDataChunk )
{
	PropertySet * pSet = new PropertySet;
	if ( !pSet->create( bytes,pDataChunk ) )
	{
		pSet->release();
		return 0;
	}

	return pSet;
}

// 获取某类服务的配置信息
SERVICE_SCHEME * Framework::get_service_scheme( const char * name )
{
	Simple_Atom_Lock loc(&m_mutex);
	SCHEME_MAP::iterator it = m_scheme_map.find(name);
	if ( it==m_scheme_map.end() )
		return 0;

	return &(it->second);
}

// 取得服务管理器接口
IServiceMgr * Framework::get_service_manager()
{
	return &ServiceMgr::getInstance();
}

// 取得定时器服
ITimerService * Framework::get_service_timer()
{
	if (m_pTimerService == 0)
	{
		m_pTimerService = &TimerService::getInstance();
	}
	return m_pTimerService;
}

void Framework::remove_service_timer()
{
	m_pTimerService = 0;
}

// 取得定时器服务
IDateTriggerService * Framework::get_service_datetrigger()
{
    if (m_pDateTriggerService == 0)
    {
        m_pDateTriggerService = &DateTriggerService::getInstance();
    }
    return m_pDateTriggerService;
}


void Framework::remove_service_datetrigger()
{
    m_pDateTriggerService = 0;
}


// 取得事件服务
rkt::IEventEngine * Framework::create_event_engine()
{
	return new EventService;
}

// 创建DB代理
IDBProxy * Framework::create_db_proxy()
{
    return new DBEngineProxy;
}

// 取得同步时间服务
ITimeSyncService* Framework::get_service_timesync()
{
	if ( m_pTimeSyncService==0 )
	{
		SERVICE_PTR pContainer = ServiceMgr::getInstance().find_service("TimeSyncService");
		if ( pContainer!=0 )
		{
			m_pTimeSyncService = (ITimeSyncService*)pContainer->query_interface();
		}
	}
	return m_pTimeSyncService;
}

void Framework::remove_service_timesync()
{
	m_pTimeSyncService = 0;
}

// 获取反应器：这样可以允许该服务监听一个事件,事件触发时回调
Reactor * Framework::get_reactor()
{
	RUNNING_CONTEXT * pRunning = WorkThreadScheduler::getInstance().get_running();
	if ( pRunning==0 )
		return 0;

	return pRunning->reactor;
}

// 添加一个工作线程
// @param bAutoRun : 自动运行 还是通过 work_thread_run手动驱动
// @return         : 返回工作线程ID
int Framework::add_work_thread( bool bAutoRun )
{
	return WorkThreadScheduler::getInstance().add_work_thread( bAutoRun );
}

// 手动驱动工作线程工作,以此达到线程同步
void Framework::work_thread_run(int nThreadID )
{
	return WorkThreadScheduler::getInstance().work_thread_run( nThreadID );
}

// 卸载框架
void Framework::release()
{
	WorkThreadScheduler::getInstance().stop();

	// 注意：Framework是一个单例，这里不必删（在应用程序退出时会自动删）
	Simple_Atom_Lock loc(&m_mutex);
	SCHEME_MAP::iterator it = m_scheme_map.begin();
	for ( ;it!=m_scheme_map.end();++it )
	{
		delete it->second.scheme;
	}

	m_scheme_map.clear();
	
	ServiceMgr::getInstance().release();
	WorkThreadScheduler::getInstance().release();
	releaseInstance();
}

SERVICE_PTR Framework::get_running_service()
{
    RUNNING_CONTEXT* pRunning = WorkThreadScheduler::getInstance().get_running();
    return ServiceMgr::getInstance().get_service(pRunning->service_id);
}

// 暂时加的，应用层调试用。
SERVICE_ID Framework::get_call_service()
{
    RUNNING_CONTEXT* pRunning = WorkThreadScheduler::getInstance().get_running();
    return pRunning->call_service;
}

// 打印线程运行时服务信息
void Framework::dump_service_runtime_info()
{
    WorkThreadScheduler::getInstance().report_service_runtime_info();
}

// 是否多线程模式开启service
bool Framework::isMulitThreadMode()
{
    return m_isMulitThreadMode;
}


extern "C" __declspec(dllexport) IFramework * GetFramework()
{
	return &Framework::getInstance();
}