/*******************************************************************
** 文件名:	AsynIoFrame.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	01/23/2008
** 版  本:	1.0
** 描  述:	异步IO框架
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __ASYN_IO_FRAME_H__
#define __ASYN_IO_FRAME_H__

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

//////////////////////////////////////////////////////////////////////////
/********************** 异步IO框架综述 **********************************/

/// 通常应用程序在处理IO操作时可以采用以下几种模型：
/// 1.同步阻塞 (IO模型)
/// 2.同步非阻塞 (IO模型) + 同步事件多路分解(Reactor事件模型) (select/WaitForMultipleObjects)
/// 3.异步IO (IO模型) + 异步事件多路分解(Proactor事件模型) (IOCP完成端口)

/// IOCP模型的基本步骤:
/// 1.创建完成端口(Proactor)
/// 2.打开IO设备(File or Socket)
/// 3.向完成端口注册IO设备,使之监听该设备的完成事件
/// 4.向IO设备发起异步IO操作,该步骤的详细过程是
///     1).创建完成对象
///     2).把异步IO要用到的缓冲区等信息设置在完成对象中
///     3).调用API,向设备发起重叠IO
/// 5.完成端口等待异步IO操作完成,并将完成情况设置在完成对象中
/// 6.通知应用层操作完成
/// 7.销毁完成对象，释放内存

/// 这里我们封装一下以上步骤,配合Proactor的具体实现就可以形成一个常用的异步IO框架
/// 可用于高性能网络通信以及大数据量的文件读写
//////////////////////////////////////////////////////////////////////////

namespace rkt
{
	struct AsynIoResult;
	struct AsynIoDevice;
	struct AsynIoBuffer;
	struct Reactor;
	struct Proactor;

	/// 异步IO类型
	enum AsynIoType
	{
		ASYN_IO_UNKOWN = 0,
		ASYN_IO_READ      ,
		ASYN_IO_WRITE     ,
		ASYN_IO_ACCEPT    ,
		ASYN_IO_CONNECT   ,
		ASYN_IO_READFROM  ,
		ASYN_IO_WRITETO   ,
		ASYN_IO_CLOSE     ,
		ASYN_IO_DISCONNECT,
	};
	
	/**
	@name : 异步IO处理器
	@brief: 常规用法:
	1.发起一个异步IO操作时指定一个异步IO处理器
	2.当异步IO操作完成时调用之前指定的处理器处理IO数据
	*/
	struct AsynIoHandler
	{
		/**
		@purpose          : 异步IO完成时回调该函数（在这里写处理代码）
		@param	  result  : IO完成对象,可以通过它获取到IO完成的情况,例如是否成功,传输了多少字节以及字节的内容等 
		@return		      :
		*/
		virtual void HandleIoComplete( AsynIoResult * result ) = 0;

		/**
		@purpose          : 释放引用
		@note             : 由于AsynIoHandler可能被多个线程使用.所以必须支持引用计数机制,否则销毁会有问题
		@return	          : 
		*/
		virtual void ReleaseRef() = 0;

		/**
		@purpose          : 增加引用计数
		@note             : 由于AsynIoHandler可能被多个线程使用.所以必须支持引用计数机制,否则销毁会有问题
		@note             : 否则的话想要释放Handler时会非常麻烦,因为你不知道是否有其他的线程正在引用
		*/
		virtual void  AddRef() = 0;
	};

	/**
	@name : 异步IO完成对象
	@brief: 功能:
			1.存放异步IO要用到的缓冲区等信息
			2.操作完成时通过它获取完成情况,例如是否成功,传输了多少字节以及字节的内容等
	@warning: 每一个异步IO完成对象都必须继承一个异步IO重叠结构
	*/
	struct AsynIoResult : public OVERLAPPED
	{
		/**
		@purpose      : 设置完成处理器 
		@param handler: 异步IO处理器
		*/
		virtual void SetHandler( AsynIoHandler * handler) = 0;

		/**
		@purpose      : 设置完成通知事件
		@param	event : 事件句柄
		@note         : 采用事件模型时设置这个事件,如果采用proactor则不需要设置这个句柄
		*/
		virtual void SetEventHandle( HANDLE event ) = 0;

		/**
		@purpose      : 通知该操作完成
		@note         : 内部调用Handler处理该完成事件
		*/
		virtual void Complete() = 0;

		/**
		@purpose   : 准备异步IO要用的缓冲区
		@param len : 缓冲区长度
		@warning   : 如果之前曾经调用PrepareBuffer准备了缓冲区,则再次调用时会把先前的缓冲区释放掉并重新分配
		*/
		virtual void PrepareBuffer( unsigned long len ) = 0;

		/**
		@brief            : 设置Buff(注意:如果先前有分配buff,这里不会把它释放掉)
		@param pBuffList  : 新缓冲区指针
		@param dwBuffCount: 缓冲区长度
		*/
		virtual void  SetBuffer(AsynIoBuffer * pBuffList,DWORD dwBuffCount) = 0;

		/**
		@purpose   : 取得缓冲区列表
		@return	   : 返回缓冲区列表指针,如果当前还没准备缓冲区则返回0
		*/
		virtual AsynIoBuffer * GetBufferList() = 0;

		/**
		@purpose   : 取得缓冲区个数
		@return	   : 返回缓冲区个数,如果当前还没准备缓冲区则返回0
		*/
		virtual DWORD GetBufferCount() const = 0;

		/**
		@purpose   : 取得成功传输的字节数
		@return	   : 如果IO没有正常完成则返回0
		*/
		virtual DWORD GetTransferredBytes() const = 0;

		/**
		@purpose    :设置成功传输的字节数
		@param bytes:成功传输的字节数
		*/
		virtual void  SetTransferredBytes( DWORD bytes) = 0;

		/**
		@purpose   : 操作是否成功完成
		@return    :
		*/
		virtual BOOL  Success() const = 0;

		/**
		@purpose   : 取得错误码,如果操作成功完成则返回0
		@return    :
		*/
		virtual DWORD GetErrorCode() const  = 0;

		/**
		@purpose   : 设置错误码
		@err       : 错误码
		@return    :
		*/
		virtual void  SetErrorCode( DWORD err ) = 0;

		/**
		@purpose   : 取得触发该完成事件的设备指针
		@return	   : 
		*/
		virtual AsynIoDevice * GetAsynDevice() const = 0;

		/**
		@purpose   : 设置触发该完成事件的设备指针
		@param device: 设备指针
		*/
		virtual void SetAsynDevice( AsynIoDevice * device) = 0;

		/**
		@purpose      : 返回异步IO类型
		@return		  : 类型定义见AsynIoOperation.h
		@note         : 这个变量主要用于Handler处理时区分是什么操作
		*/
		virtual AsynIoType GetType() const = 0;

		/**
		@purpose      : 设置异步IO类型
		@param op_type: 类型定义见AsynIoOperation.h
		@note         : 这个变量主要用于Handler处理时区分是什么操作
		*/
		virtual void SetType( AsynIoType op_type ) = 0;

		/**
		@name         : 是不是通过Post模拟的完成操作
		@brief        :
		*/
		virtual bool IsSimulation() = 0;

		virtual void SetSimulation(bool is_simulation) = 0;

		virtual void Release() = 0;
	};

	/// IO缓冲区
	struct AsynIoBuffer
	{
		DWORD        len;
		char *       buf;
	};

	/// 最大Buff数目
#	define	MAX_ASYN_IO_BUFF_COUNT      128

	/**
	@name             : 初始化异步IO框架
	@param dwThreadNum: 创建多少个工作线程去处理异步IO
	@param dwConcurrentThread : 同一时刻允许多少个线程并发工作
	@return		      : 1 ... OK
						-1... 初始化前摄器失败
						-2... 启动工作线程池失败
						-3... 初始化网络失败
	*/
	NET_API int InitializeAsynIoFrame(DWORD dwThreadNum=1,DWORD dwConcurrentThread=0);

	/**
	@name         : 释放异步IO框架
	@brief        : 如果在程序退出时应用层还没有调用该函数,异步IO框架会自己释放
	@brief        : 所以这个函数可以不调用
	*/
	NET_API int UninitializeAsynIoFrame();

	/**
	@name         : 取得用于异步IO的前摄器指针
	@brief        : 其他有些地方可以用得到
	*/
	NET_API Proactor * GetAsynIoProactor();

	/**
	@name         : 取得用于异步IO的反应器指针
	@brief        : 其他有些地方可以用得到
	*/
	NET_API Reactor * GetAsynIoReactor();

	/**
	@name         : 创建一个反应器
	@brief        : 通过GetAsynIoReactor获得的反应器是在IO线程监听的,如果
	              : 你需要监听事件又不想在IO线程可以自己创建一个Reactor去监听
	*/
	NET_API Reactor * CreateReactor();

	/**
	@name         : 分配一段内存,用于异步IO
	@brief        : 应用层提前分配好可以减少一次多余的内存拷贝，提高效率
	@param dwSize : 需要分配的内存大小
	@return       :
	*/
	NET_API void * AllocateAsynIoBuffer(size_t dwSize);

	/**
	@name         : 释放一段用于异步IO的内存
	@brief        : 应用层提前分配好可以减少一次多余的内存拷贝，提高效率
	@param   buf  : 指向需要释放的内存块的指针
	@return       :
	*/
	NET_API void   DeallocateAsynIoBuffer(void * buf);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 下面一组API与内存池有关，不需要了解
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 启动跨线程辅助释放功能
	NET_API bool MT_StartThreadFreeHelp();

	// 停止跨线程辅助释放功能
	NET_API void MT_StopThreadFreeHelp();

	// 设置内存池配置
	NET_API void MT_GetMTMemPoolConfigs( int & cache_level,int & mem_threshold,int & auto_check );

	// 设置内存池配置
	NET_API void MT_SetMTMemPoolConfigs( int cache_level,int mem_threshold,int auto_check );

	// 检查内存占用
	NET_API unsigned long MT_CheckThreadMemCache( bool bDump );

	// 设置当前线程为主线程
	NET_API void MT_SetThreadPriority( int priority );
};

#endif // #ifdef SUPPORT_NET

#endif//__ASYN_IO_FRAME_H__