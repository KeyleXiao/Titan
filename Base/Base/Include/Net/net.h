/*******************************************************************
** 文件名:	net.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	01/26/2008
** 版  本:	1.0
** 描  述:	网络通讯接口
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/


#ifndef __NET_H__
#define __NET_H__

#include "Config.h"

#ifdef SUPPORT_NET

//////////////////////////////////////////////////////////////////////////
// 导出宏定义
#ifndef NET_API
#define NET_API RKT_API
//#	ifdef _LIB
//#		define NET_API		extern "C"
//#	elif NET_EXPORTS
//#		define NET_API		extern "C" __declspec(dllexport)
//#	else
//#		define NET_API		extern "C" __declspec(dllimport)
//#	endif
#endif//NET_API

#include <Windows.h>
#include "netdef.h"

namespace rkt
{
    enum NetworkProtocol
    {
        PROTOCOL_TCP,           // TCP协议
        PROTOCOL_UDP,           // UDP协议
    };

	struct IConnection;                  // TCP连接
	struct IUDPSocket;                   // UDP套接字
	struct IPacker;                      // 组包接口
	class  TimerAxis;                    // 时间轴

	/// 链接创建标志
	#define NO_PACK       (IPacker*)0        // 不组包
	#define	DEFAULT_PACK  (IPacker*)1        // 使用默认方式组包
	#define	ENCRYPT_PACK  (IPacker*)2        // 使用默认方式组包并加密
	#define	HUFFMAN_PACK  (IPacker*)3        // 使用HUFFMAN算法组包并加密
	#define LZO_PACK      (IPacker*)4        // 使用LZO算法组包并加密
	#define LUA_PACK      (IPacker*)5        // 使用Lua函数封装加密算法，使得客户端更难以跟踪

	// dll库编译时间
	#define COMPILATION_TIME    (__DATE__)

	/**
	@name : 网络事件通知接口
	@brief: 网络事件触发时回调
	*/
	struct IConnectionEventHandler
	{
		/**
		@brief              : 收到被动连接
		@param pIncomingConn: 客户端连入的新连接
		@param ppHandler    : 处理该连接的Handler
		@warning            : 收到连接后一定要设置Handler
		*/
		virtual void OnAccept( IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler ) = 0;

		/**
		@brief      : 连接成功
		@param conn : 连接对象指针
		*/
		virtual void OnConnected( IConnection * conn ) = 0;

		/**
		@brief      : 连接断开
		@param conn : 连接对象指针
		@param reason:断开原因(保留) 
		*/
		virtual void OnDisconnected( IConnection * conn,DWORD reason ) = 0;

		/**
		@brief        : 收到数据
		@param conn   : 连接对象指针
		@param pData  : 数据地址
		@pram dwDataLen:数据长度
		@return       :
		*/
		virtual void OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen ) = 0;

		/**
		@name             : 网络错误事件
		@brief            :
		@param conn       : 出现错误的套接字
		@param dwErrorCode: 错误码
		*/
		virtual void OnError( IConnection * conn,DWORD dwErrorCode ) = 0;
	};

	/**
	@name : TCP连接对象
	@brief: 内部封装了一个TCP套接字对象
	*/
	struct IConnection
	{
		/**
		@brief        : 监听被动连接
		@return       : 没有错误发生返回true,收到连接后回调OnAccept
		*/
		virtual bool Listen() = 0;

		/**
		@brief        : 主动连接远程主机
		@param  ip    : 远程主机IP
		@param  port  : 远程主机端口
		@return       : 没有错误发生返回true,连接成功后回调OnConnected
		*/
		virtual bool Connect( const char * ip,unsigned short port ) = 0;

		/**
		@brief        : 发送数据
		@param  pData : 数据指向的地址
		@param dwDataLen:数据长度
		@param dwFlag : 发送数据的特殊需求,可以由SEND_DATA_FLAG中的枚举值位或组成
		@return       : 没有错误发生返回true
		*/
		virtual bool SendData( const char * pData,DWORD dwDataLen,DWORD dwFlag=0 ) = 0;

		/**
		@brief        : 关闭连接
		@return       : 没有错误发生返回true,关闭成功后回调OnDisconnected
		*/
		virtual bool Disconnect() = 0;

		/**
		@brief        : 设置事件处理接口,用来接收回调事件
		@param pHandler:接口指针
		*/
		virtual void SetEventHandler( IConnectionEventHandler * pHandler ) = 0;

		/**
		@brief        : 取得事件处理接口
		@return       : 接口指针
		*/
		virtual IConnectionEventHandler * GetEventHandler() const = 0;

		/**
		@name              : 取得对方的网络地址
		@param ppRemoteIP  : 返回对方的IP [OUT]
		@param dwRemotePort: 返回对方的端口 [OUT]
		@return            : 如果对方的地址还不确定则返回失败
		*/
		virtual bool GetRemoteAddr( const char ** ppRemoteIP,DWORD & dwRemotePort) = 0;

		/**
		@name         : 是否是被动连接
		@brief        : 
		*/
		virtual bool  IsPassive() const = 0;

		/**
		@name         : 是否连接成功
		@brief        : 
		*/
		virtual bool  IsConnected() const = 0;

		/**
		@brief        : 销毁该对象,释放内存
		*/
		virtual void Release() = 0;

		/**
		@name         : 设置套接字选项
		@param optLevel:协议所属的协议层次,见API setsockopt
		@param pptName: 选项名,见API setsockopt
		@param pValue : 数值内容,见API setsockopt
		@param valueLen:数值长度,见API setsockopt
		*/
		virtual bool SetSocketOption(int optLevel,int pptName,const char * pValue,int valueLen) = 0;

		/**
		@name         : 设置保护堆栈
		@note         : 防外挂需求,不允许别的模块调用这个通讯层发包(放在接口里的好处是不会在导出函数中出现)
		@param        : 允许的低水位地址
		@param        : 允许的高水位地址
		*/
		virtual void SetSendDataProtect( DWORD dwLowAddress,DWORD dwUpperAddress ) = 0;

		/**
		@name         : 取得保护标志位
		@return       : 返回>0表示存在外挂(放在接口里的好处是不会在导出函数中出现)
		*/
		virtual int  GetProtectedFlag() const = 0;

		/**
		@name         : 检测某个IP的连接是否存在
		@note         : 该命令等同于netstat查看的结果，用于检测与外挂服务器的通讯是否存在
		@return       : 返回true表示存在(放在接口里的好处是不会在导出函数中出现)
		*/
		virtual bool CheckExistConnection( DWORD dwIP ) = 0;

		/**
		@name         : 设置最后收到包的系统高性能计数,用来计算处理包时延迟用
		@param  LONGLONG nTime : 要设定的时间
		@return 
		*/
		virtual void SetLastRecvDataTimes( LONGLONG nTime ) = 0;

		/**
		@name         : 取得最后收到包的系统高性能计数,用来计算处理包时延迟用
		@return       : LONGLONG  最后收到包的系统高性能计数
		*/
		virtual LONGLONG GetLastRecvDataTimes() = 0;

		/**
		@name         : 获取队列里未处理的数据包个数
		@name         : 获取队列是发包,否则是收包
		@return       : 
		*/
		virtual unsigned long GetPendingCount(bool bIsSend=false) = 0;

		// 设定名称，用来调试显示拥有者
		virtual bool setName(const char * pstrName) = 0;

		//  取得名称
		virtual const char * getName() = 0;

		// 临时阻塞一个连接，让他不要再收取数据（该方法只在很极端的情况下可能用到）
		virtual void Blocking( bool bBlock ) = 0;

		// 设置所属网络应用类型 EMNetworkUserType,这个一般在低层初始化时严格按应用线程来设定，如搞错，会导致其他应的跑到另一个应用,没有特殊情况用默认
		// 一般都只有一个默认网络应用,不用手动设定,只有想加多个时才严格加上并分清楚
		virtual void setNetworkUserType( DWORD dwType ) = 0;

		// 取得所属网络应用类型 EMNetworkUserType
		virtual DWORD getNetworkUserType() = 0;

		// 设置位状态标记 dwFlag EMNetworkFlag
		virtual void setFlag( DWORD dwFlag, bool bSet) = 0;

		// 获取位状态标记
		virtual bool getFlag( DWORD dwFlag ) = 0;

        // 获取当前Connect是哪种网络协议
        virtual DWORD getNetworkProtocol(void) = 0;
	};

	/**
	@name : UDP事件处理接口
	@brief: 
	*/
	struct IUDPHandler
	{
		/**
		@name             : 数据到达事件
		@brief            : UDP套接字收到数据时回调用
		@para  socket     : 收到数据的套接字
		@param pData      : 数据地址
		@pram dwDataLen   : 数据长度
		@param pRemoteAddr : 对方IP,端口地址,为 sockaddr_in * 类型,要取得字符IP请用 inet_ntoa(pAddr->sin_addr)
		*/
		virtual void OnRecv( IUDPSocket * socket,LPVOID pData,DWORD dwDataLen,void * pRemoteAddr ) = 0;

		/**
		@name             : 网络错误事件
		@brief            :
		@param socket     : 出现错误的套接字
		@param dwErrorCode: 错误码
		*/
		virtual void OnError( IUDPSocket * socket,DWORD dwErrorCode ) = 0;
	};

	/**
	@name : UDP套接字
	@brief: 
	*/
	struct IUDPSocket
	{

		/**
		@brief            : 发送数据
		@param pRemoteAddr : 对方IP,端口地址,为 sockaddr_in * 类型 ,主要是为了加快发包速度,不用转换IP
		@param  pData     : 数据指向的地址
		@param dwDataLen  : 数据长度
		@param dwFlag     : 发送数据的特殊需求,可以由SEND_DATA_FLAG中的枚举值位或组成
		@return           : 没有错误发生返回true
		*/
		virtual bool SendData( void * pRemoteAddr,const char * pData,DWORD dwDataLen,DWORD dwFlag=0) = 0;

		/**
		@brief            : 发送数据
		@param szRemoteIP : 对方IP
		@param nRemotePort: 对方端口
		@param  pData     : 数据指向的地址
		@param dwDataLen  : 数据长度
		@param dwFlag     : 发送数据的特殊需求,可以由SEND_DATA_FLAG中的枚举值位或组成
		@return           : 没有错误发生返回true
		*/
		virtual bool SendData( const char * szRemoteIP,WORD nRemotePort,const char * pData,DWORD dwDataLen,DWORD dwFlag=0 ) = 0;


		/**
		@brief        : 设置事件处理接口,用来接收回调事件
		@param pHandler:接口指针
		*/
		virtual void SetEventHandler( IUDPHandler * pHandler ) = 0;

		/**
		@brief        : 取得事件处理接口
		@return       : 接口指针
		*/
		virtual IUDPHandler * GetEventHandler() const = 0;

		/**
		@brief        : 销毁该对象,释放内存
		*/
		virtual void Release() = 0;

		/**
		@name         : 获取队列里未处理的数据包个数
		@name         : 获取队列是发包,否则是收包
		@return       : 
		*/
		virtual unsigned long GetPendingCount(bool bIsSend=false) = 0;

		// 设定名称，用来调试显示拥有者
		virtual bool setName(const char * pstrName) = 0;

		//  取得名称
		virtual const char * getName() = 0;

		// 取得绑定端口
		virtual WORD GetBindPort(void) = 0;

		// 设置所属网络应用类型 EMNetworkUserType,这个一般在低层初始化时严格按应用线程来设定，如搞错，会导致其他应的跑到另一个应用,没有特殊情况用默认
		// 一般都只有一个默认网络应用,不用手动设定,只有想加多个时才严格加上并分清楚
		virtual void setNetworkUserType( DWORD dwType ) = 0;

		// 取得所属网络应用类型 EMNetworkUserType
		virtual DWORD getNetworkUserType() = 0;

		// 设置位状态标记 dwFlag EMNetworkFlag
		virtual void setFlag( DWORD dwFlag, bool bSet) = 0;

		// 获取位状态标记
		virtual bool getFlag( DWORD dwFlag ) = 0;

	};

	/**
	@name         : 初始化通讯层
	@brief        : 初始化过程:
	              : 1.初始化异步IO框架
    @param dwThreadNum : 使用多少个线程处理异步IO
	@param bEventNotify: 是否使用事件方式通知应用层调度网络事件,如果不使用事件通知的方式,应用层可在空闲或定时调度
	@return       : 1 ... OK
	                -1... 异步IO框架初始化失败
	*/
	NET_API int  InitializeNetwork(DWORD dwThreadNum=2,bool bEventNotify=true,const char * str=COMPILATION_TIME);

	/**
	@name         : 释放通讯层
	@brief        : 如果在程序退出时应用层还没有调用该函数,通讯层会自己释放
	@brief        : 所以这个函数可以不调用
	*/
	NET_API int  UninitializeNetwork();

	/**
	@name            : 获取回调用内容
	@param dwNetworkUserType : 所属网络应用类型,通讯层多实例网络应用类型,用来实现多个实例以支持多个处理线程收发,没有特殊情况用默认
	@brief           : 通讯层的各种事件都是在通讯线程触发的,而用户希望收到的通讯事件必须在用户线程
                     : 这样上层代码就不需要额外处理线程同步的操作,所以在用户线程的主循环里,需要持续
                     : 调用DispatchNetworkEvents函数来驱动通讯事件的派发,类似在主循环收取窗口消息一样
	@return          : 数据包全部处理完了,返回true,数据包太多一次处理不完返回false
	*/
	NET_API bool DispatchNetworkEvents(ulong timeout = 0,DWORD dwNetworkUserType = NetworkUserType_Default);

	/**
	@name            : 取得网络事件的通知句柄
	@brief           : 应用层可以监听这个事件然后调用DispatchNetworkEvents收取网络消息
	@param dwNetworkUserType : 所属网络应用类型,通讯层多实例网络应用类型,用来实现多个实例以支持多个处理线程收发,没有特殊情况用默认
	@return          : 返回事件句柄
	*/
	NET_API HANDLE GetNetworkEventHandle(DWORD dwNetworkUserType = NetworkUserType_Default);

	/**
	@name            : 创建一个TCP连接对象
	@param dwBindPort: 绑定端口,传0主要用在主动方发起连接时随机选择一个可用端口
	@param pHandler  : 事件处理接口指针
	@param pPacker   : 组包接口,传0表示不进行组包,传1表示使用默认的简单组包方法
	@param dwNetworkUserType : 所属网络应用类型,通讯层多实例网络应用类型,用来实现多个实例以支持多个处理线程收发,没有特殊情况用默认
	@param dwSendBuffMaxCount : 一次发送最大Buff数目，不能超过MAX_ASYN_IO_BUFF_COUNT
	@brief:
	*/
	NET_API IConnection * CreateTCPConnection(USHORT dwBindPort=0,IConnectionEventHandler * pHandler=0,IPacker * pPacker=DEFAULT_PACK,
		DWORD dwNetworkUserType = NetworkUserType_Default,DWORD dwSendBuffMaxCount = MAX_ASYN_IO_SEND_BUFF_COUNT);

	/**
	@name            : 创建一个KCP连接对象(用UDP模拟的TCP）
	@param isServer  : 是服务器还是客户端
	@param dwBindPort: 绑定端口,传0主要用在主动方发起连接时随机选择一个可用端口
	@param pHandler  : 事件处理接口指针
	@param pPacker   : 组包接口,传0表示不进行组包,传1表示使用默认的简单组包方法
	@brief:
	*/
	NET_API IConnection * CreateKCPConnection(bool isServer,USHORT dwBindPort=0,IConnectionEventHandler * pHandler=0,IPacker * pPacker=DEFAULT_PACK);

	/**
	@name            : 创建一个UDP套接字对象
	@param dwBindPort: 绑定端口
	@param pHandler  : 事件处理接口指针
	@param dwNetworkUserType : 所属网络应用类型,通讯层多实例网络应用类型,用来实现多个实例以支持多个处理线程收发,没有特殊情况用默认
	@brief:
	*/
	NET_API IUDPSocket * CreateUDPSocket(USHORT dwBindPort,IUDPHandler * pHandler=0,DWORD dwNetworkUserType = NetworkUserType_Default);

	/**
	@name         : 取得本地IP
	@brief        : 如果本机在局域网内则返回的是局域网IP
	@return       : 如果失败返回"0.0.0.0",否则返回本地IP
	*/
	NET_API const char * GetLocalIP();

	/**
	@name         : 是否是内网机器
	@brief        : 根据枚举到的IP判断是否是内网机器
	*/
	NET_API bool IsNated();

	/**
	@name          : 取得本地网卡地址
	@brief         :
	@param nAdapter: 第几个网卡
	@return        : 如果失败返回空字符串"",否则返回网卡地址串
	*/
	NET_API const char * GetMacAddress(int nAdapter = 0);

	/**
	@name          : 是否禁用通讯层压缩
	@note          : 动态打开关闭通讯层压缩
	*/
	NET_API void EnableNetCompress( bool bEnable );

	/**
	@name          : 网络数据包统计
	@param nTotalSend : 总发送数据
	@param nCompressed: 压缩后的数据
	*/
	NET_API void NetDataStatistic( unsigned long & nTotalSend,unsigned long & nCompressed );


///////////////////////////////////////////////////////////////////////////////////////////////////////
//#if defined(_LIB) || defined(NET_STATIC_LIB)	/// 静态库版本
#define	CreateTCPConnectionProc	CreateTCPConnection
//#else													/// 动态库版本
//typedef IConnection * (RKT_CDECL *procCreateTCPConnection)(USHORT dwBindPort,IConnectionEventHandler * pHandler,IPacker * pPacker);
//#	define	CreateTCPConnectionProc	DllApi<procCreateTCPConnection>::load(MAKE_DLL_NAME(Net), "CreateTCPConnection")
//#endif

};

#endif // #ifdef SUPPORT_NET

#endif//__NET_H__