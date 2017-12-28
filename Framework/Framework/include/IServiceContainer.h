/*******************************************************************
** 文件名:	IServiceContainer.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	11/10/2014
** 版  本:	1.0
** 描  述:	基于简单接口的框架系统

			
********************************************************************/

#pragma once

#include "buffer.h"
#include "share_ptr.h"
#include "StubMacroDef.h"
#pragma pack(push, 1)

/**
	设计综述：
	1. 本框架参照Actor模式实现（参考Erlang与skynet)
	2. 以服务（Service)为单位组织服务器代码，每个服务提供简单接口供外界调用
	3. 根据分布式配置调度服务（Service)的运行，每个服务可能运行在不同线程，甚至不同机器
	4. 封装服务（Service)之间的通讯
	5. 服务（Service)基于Lua虚拟机来构建，服务间的消息传递用lua来封装和解析，可以刷脚本修改游戏逻辑
	6. 整个框架大致可以理解为基于Lua语言来实现的Erlang(因为Erlang的函数语法实在太另类了）
*/

// 无效服务ID
#define  INVALID_SERVICE_ID         (size_t(-1))

// 服务（Service)ID
struct SERVICE_ID
{
	UINT_PTR    cloud  : 8;        // 服务（Service)所在的机器节点ID,即云ID
	UINT_PTR    index  :24;        // 服务（Service)序号
	SERVICE_ID(){ *(UINT_PTR*)this = INVALID_SERVICE_ID; }
	SERVICE_ID( UINT_PTR value ){ *(UINT_PTR*)this=value; }

	operator UINT_PTR() { return *(UINT_PTR*)this;}
};


// 开启调试信息
#define FRAMEWORK_DEBUG

// 会话ID定义
#define   MSG_SESSION_START         0
#define   MSG_SESSION_END           1
#define   MSG_SESSION_CALLBACK      2
#define   MSG_SESSION_USER          4


// 消息标志位
#define   MSG_FLAG_PENDING          0             // 阻塞式，返回值
#define   MSG_FLAG_NO_BLOCK         1             // 不阻塞，也不返回值
#define   MSG_FLAG_REENTRY_CODE     (1<<1)        // 跨线程调用的代码必须可重入,不然会出现死锁情况(只读的函数都是可以重入的)
#define   MSG_FLAG_THREAD_SAFE      (1<<2)        // 线程安全的调用
#define   MSG_FLAG_THREAD_MUTEX     (1<<3)        // 互斥调用的话就安全，即任一时刻只允许一个线程调用
#define   MSG_FLAG_EMERGENT         (1<<4)        // 紧急调用,需要第一时间处理
#define   MSG_FLAG_DEFAULT         (MSG_FLAG_PENDING | MSG_FLAG_THREAD_MUTEX | MSG_FLAG_REENTRY_CODE)
#define   MSG_FLAG_READ_ONLY        MSG_FLAG_DEFAULT


// 查询接口定义
#define   IID_STUB                  0             // 存根对象
#define   IID_PROXY                 1             // 代理接口
#define   IID_REAL                  2             // 真实接口

#pragma warning(disable : 4200)
// 消息现场
struct SERVICE_MESSAGE
{
	SERVICE_ID    from;			// 消息发送者
	SERVICE_ID    to;           // 消息接收者

	int           session;      // 会话标识

    DWORD_PTR     enqueue_tick;  // 进队列的时间
    
	DWORD_PTR     function_id;   // 调用的函数ID,方便知道回调时找到函数名
	
	bool          is_request;    // 是请求消息还是响应消息
	bool          need_response; // 是否需要发回响应

#ifdef VERSEION_DEBUG
    char          szName[32];
#endif

	int           context_len;   // 消息现场长度
	char          context[0];	 // 消息现场内容

#ifdef VERSEION_DEBUG
    SERVICE_MESSAGE()
    {
        session = context_len = 0;
        enqueue_tick = function_id= 0;
        is_request = need_response = false;
        memset(szName, 0, sizeof(szName));
    }
#endif
};

// 服务配置
struct SERVICE_SCHEME_STATIC
{
	char          name[64];         // 服务名称
	char          dll_path[256];    // 所在的dll路径
	char          create_func[64];  // 创建函数名
	int           stack_size;       // 堆栈大小 
	int           time_slice;       // 时间片大小，超过这个时间片的长调用，会被自动挂起
	bool          thread_safe;      // 是否多线程安全,这样就可以直接调用
	bool          inner_timer;      // 开启内部定时器，不用线程共享定时器
	int           watch_min;        // 包监控超过正常报警数量
	int           watch_max;        // 包监控严重报警数量
    int           evaluate_score;   // 权重数

	void *        pExtend;          // 这里存放的实际就是配置的xml节点对象，底层容器只关心上面那些配置，其他配置上次服务可以自己去读
};

struct SERVICE_SCHEME
{
	int           thread_id;        // 分配到哪个工作线程上
	void *        work_event;       // 工作事件
	SERVICE_SCHEME_STATIC * scheme; // 静态配置信息
    int           is_prefab;

	SERVICE_SCHEME(){memset(this,0,sizeof(SERVICE_SCHEME) );}
};

// service 运行时信息 // for test
struct SERVICE_RUNTIME_INFO
{
	UINT_PTR    service_id;
    char		name[64];          // 服务名
    int			queue_len;         // 队列长度
    int			process_rate;      // 每秒处理包的数量

    SERVICE_RUNTIME_INFO(){memset(this, 0, sizeof(SERVICE_RUNTIME_INFO));}
};

// thread 运行时信息
struct THREAD_RUNTIME_INFO
{
    int                                 nCoCount;       // 协程数量
    std::vector<SERVICE_RUNTIME_INFO>   vecServiceRti;  // 服务运行时信息

    THREAD_RUNTIME_INFO(){nCoCount=0; vecServiceRti.clear();}
};

struct IServiceStub;

/**
	服务容器:
	1.框架根据配置创建容器，容器负责与框架进行底层通讯，收到调用请求再转交给应用层提供的服务存根进行处理
	2.代理/存根，从COM架构借鉴过来的名词，应用层调用某个服务的接口，实际交由代理封包成消息，再提交给框架进行转发
	            而框架里的容器收到请求消息，则需要调用存根进行解释和真正处理
*/

// 对于服务接口都采用智能指针访问，这样可以保证多线程的安全（本地记录了指针，其他线程不能释放掉)
struct IServiceContainer;
typedef share_ptr<IServiceContainer, true>    SERVICE_PTR;

// Service启动完成回调函数
typedef void (* ServiceStartedCallback)(SERVICE_PTR pService);
// Service停止完成回调函数
typedef void (* ServiceStopedCallback)(void);

// service状态
enum MY_SERVICE_STATUS
{
    SERVICE_STATUS_STARTING,    // 服务正在启动
    SERVICE_STATUS_WORKING,     // 服务启动完成
    SERVICE_STATUS_STOPING,     // 服务正在关闭
    SERVICE_STATUS_CLOSED,       // 服务已关闭
} ;

struct ITimerService;
struct IDateTriggerService;
struct IServiceContainer
{
	// 启动
	virtual bool start( SERVICE_PTR pService,SERVICE_SCHEME * pScheme,IServiceStub * pInstance=0,void * pData=0,int nLen=0, ServiceStartedCallback pStartedCallback=0, ServiceStopedCallback pStopedCallback=0) = 0;

	// 停止
	virtual bool stop() = 0;

	// 设置ID
	virtual void set_id( SERVICE_ID id ) = 0;

	// 取得ID
	virtual SERVICE_ID get_id()  = 0;

	// 获取配置信息
	virtual SERVICE_SCHEME & get_scheme() = 0;

	// 查询接口
	virtual void * query_interface( int iid=IID_PROXY ) = 0;

	// 投递一个消息
	// @param nFlag : 该标志位
	virtual bool _post_message( void * pMsg,size_t nMsgLen,rkt::obuf* resultBuf,int nFlag=MSG_FLAG_DEFAULT ) = 0;

	// 投递一个消息(高性能版：注意参数约定)
	// @param pMsg  : 必须是malloc出来的消息对象,这样可以减少一次内存拷贝
	// @param nMsgLen:包含消息内容的长度,nMsgLen必须等于sizeof(SERVICE_MESSAGE)+pMsg->context_len
	// @param nFlag : 该标志位
	virtual bool post_message( SERVICE_MESSAGE * pMsg,size_t nMsgLen,rkt::obuf* resultBuf,int nFlag=MSG_FLAG_DEFAULT ) = 0;

	// 收到一个消息
	virtual bool recv_message( SERVICE_MESSAGE * pMsg,bool bReadonlyRequest=false ) = 0;

	// 强制设置下一个消息的投递方法,发完一个消息后会自动回位
	virtual void  switch_method_flag( int nFlag ) = 0;

	// 工作线程让我工作
	virtual bool work(void* pWorkThread) = 0;

	// 挂起
	virtual void yield(bool finish) = 0;

	// 服务状态
	virtual MY_SERVICE_STATUS get_status() = 0;

#ifdef   FRAMEWORK_DEBUG
	virtual const char* get_wait_function() = 0;
#endif

    virtual void release() = 0;

    // 获取队列包数量
    virtual int get_request_queue_count() = 0;

    // 获取处理包速率(/s)
    virtual int get_process_rate() = 0;

    // 投递一个可被替换的消息
	// @param pMsg  : 必须是malloc出来的消息对象,这样可以减少一次内存拷贝
	// @param nMsgLen:包含消息内容的长度,nMsgLen必须等于sizeof(SERVICE_MESSAGE)+pMsg->context_len
    virtual bool post_replaceable_message( unsigned long ulKey,SERVICE_MESSAGE * pMsg,size_t nMsgLen ) = 0;

	// 取得内部时间轴
	virtual ITimerService* get_inner_timeraxis() = 0;
};


/*
	1.代理/存根，从COM架构借鉴过来的名词，应用层调用某个服务的接口，实际交由代理封包成消息，再提交给框架进行转发
	而框架里的容器收到请求消息，则需要调用存根进行解释和真正处理	
	2.存根需要继承自IServiceStub,代理则必须继承自应用层的服务接口，比如IXXXXService
*/
struct IServiceStub
{
	// 启动
	// @param data : 在目前这版本中,data里的内容是指向配置信息的TiXmlElement *指针
	virtual bool on_start( SERVICE_PTR pContainer,void * data,int len ) = 0;

	// 停止
	virtual bool on_stop() = 0;

	// 处理消息
	virtual bool handle_message( SERVICE_MESSAGE * pMsg,rkt::obuf * resultBuf ) = 0;

	// 返回代理对象即可
	virtual void * query_interface( int iid ) = 0;

	// 销毁
	virtual void release() = 0;
};

// 回调代理
struct ICallbackProxy
{
	virtual void Do(SERVICE_MESSAGE * pMsg) = 0;
};


#pragma pack(pop)