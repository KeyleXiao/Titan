/*******************************************************************
** 文件名:	Framework.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	11/10/2014
** 版  本:	1.0
** 描  述:	基于简单接口的框架系统

			
********************************************************************/

#pragma once


/**
	设计综述：
	1. 本框架参照Actor模式实现（参考Erlang与skynet)
	2. 以服务（Service)为单位组织服务器代码，每个服务提供简单接口供外界调用
	3. 根据分布式配置调度服务（Service)的运行，每个服务可能运行在不同线程，甚至不同机器
	4. 封装服务（Service)之间的通讯
	5. 服务（Service)基于Lua虚拟机来构建，服务间的消息传递用lua来封装和解析，可以刷脚本修改游戏逻辑
	6. 整个框架大致可以理解为基于Lua语言来实现的Erlang(因为Erlang的函数语法实在太另类了）
*/

#include "IFramework.h"
#include "IServiceContainer.h"
#include "Singleton.h"
#include <map>
#include <string>

using namespace std;

class Framework : public IFramework,public Singleton<Framework>
{
public:
	Framework() : m_mutex(0), m_pTimerService(0), m_pEventEngine(0), m_pTimeSyncService(0), m_isMulitThreadMode(true),m_pDateTriggerService(0) {}

	// 启动框架
	virtual bool start( const char * path, bool isMulitThreadMode );

	// 手动创建一个服务
	virtual SERVICE_PTR create_service( SERVICE_SCHEME * scheme,IServiceStub * pStub=0,void * pData=0,int nLen=0, ServiceStartedCallback pStartedCallback=0, ServiceStopedCallback pStopedCallback=0 );

	// 创建一个安全的属性集，这样简单的属性设置和获取就不需要通过post_message来调用
	// @param pDataChunk : 外部传入属性集保存的指针，这样内部就不需要分配了,否则会分配一个bytes字节的内存
	virtual IPropertySet * create_propertyset( size_t bytes,void * pDataChunk=0 );

	// 获取某类服务的配置信息
	virtual SERVICE_SCHEME * get_service_scheme( const char * name );

	// 取得服务管理器接口
	virtual IServiceMgr * get_service_manager();

	// 取得定时器服
	virtual ITimerService * get_service_timer();

    // 清除定时器服
    virtual void remove_service_timer();

    // 取得定时器服
    virtual IDateTriggerService * get_service_datetrigger();

    // 清除定时器服
    virtual void remove_service_datetrigger();
	

	// 取得事件服务
	virtual rkt::IEventEngine * create_event_engine();

    // 创建DB代理
    virtual IDBProxy * create_db_proxy();

	// 取得同步时间服务
	virtual ITimeSyncService* get_service_timesync();

	// 清除同步时间服务
	virtual void remove_service_timesync();

	// 获取反应器：这样可以允许该服务监听一个事件,事件触发时回调
	virtual Reactor * get_reactor();

	// 添加一个工作线程
	// @param bAutoRun : 自动运行 还是通过 work_thread_run手动驱动
	// @return         : 返回工作线程ID
	virtual int add_work_thread( bool bAutoRun=true );

	// 手动驱动工作线程工作,以此达到线程同步
	virtual void work_thread_run(int nThreadID );

	// 释放框架
	virtual void release();

    virtual SERVICE_PTR get_running_service();

    // 暂时加的，应用层调试用。
    virtual SERVICE_ID get_call_service();

    // 打印线程运行时服务信息
    virtual void dump_service_runtime_info();

    // 是否多线程模式开启service
    virtual bool isMulitThreadMode();

protected:
	TiXmlDocument		doc;
	volatile LONG		m_mutex;

	typedef std::map<std::string,SERVICE_SCHEME> SCHEME_MAP;
	SCHEME_MAP			m_scheme_map;


	
	ITimerService *		m_pTimerService;	// 定时器服
	rkt::IEventEngine*	m_pEventEngine;
	ITimeSyncService*	m_pTimeSyncService;
    bool                m_isMulitThreadMode;   // 是否为单线程开启
    IDateTriggerService* m_pDateTriggerService; // 日期触发器
};
