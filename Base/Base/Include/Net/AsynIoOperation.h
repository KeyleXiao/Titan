/*******************************************************************
** 文件名:	AsynIoOperation.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	01/23/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  定义几种常用的异步IO操作	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __ASYN_IO_OPERATION_H__
#define __ASYN_IO_OPERATION_H__

#include "Config.h"

#ifdef SUPPORT_NET

namespace rkt{

	struct AsynIoHandler;// 处理接口,见AsynIoFrame.h
	struct AsynIoBuffer; // 缓冲区结构,见AsynIoFrame.h
	struct AsynIoResult; // 结果对象,见AsynIoResult.h

	/// 异步读
	struct AsynIoOperation_Reader
	{
		/**
		@purpose  : 发起一个读操作
		@param expect_bytes : 期望读取的字节数 
		@param handler      : 操作完成后的处理类  
		@note     : 该函数立即返回,读取的结果通过AsynIoResult返回
		@note     : 如果在读的过程中发生错误则抛出异常
		*/
		virtual bool read(DWORD expect_bytes,AsynIoHandler * handler) = 0;

		/**
		@purpose  : 高级读取:用户提供一个result对象,可以用来实现批量读取等高级特性
		@param handler      : 操作完成后的处理类  
		@note     : 该函数立即返回,读取的结果通过AsynIoResult返回
		@note     : 如果在读的过程中发生错误则抛出异常
		*/
		virtual bool readEx(AsynIoResult * result,AsynIoHandler * handler) = 0;
	};

	/// 异步写
	struct AsynIoOperation_Writer
	{
		/**
		@purpose    : 发起一个写操作
		@param data : 需要写入的数据地址(这个内存可以分配在栈上,内部实现时会拷贝一下内存)
		@param len  : 需要写入的字节数
		@param handler: 操作完成后的处理类  
		@note       : 该函数立即返回,读取的结果通过AsynIoResult返回
		@note       : 如果在写的过程中发生错误则抛出异常
		@warning    : UDP的设备因为发送数据时需要指定对方的地址所以实现比较特殊,默认地址(sockaddr_in)放在缓冲区的最前面
		*/
		virtual bool write(const char * data,DWORD len,AsynIoHandler * handler) = 0;

		/**
		@name             : 批量写入(聚集IO)
		@brief            : 主要是为了提升性能,特别是打包的时候可以减少一次内存拷贝
		@warning          : 注意:pBuffList指向的内存必须是由AllocAsynIoBuff()分配的,里面不会再拷贝了
		@param pBuffList  : 缓冲区列表
		@param dwBuffCount: 缓冲区个数
		@param handler    : 操作完成后的处理类  
		@note             : 该函数立即返回,读取的结果通过AsynIoResult返回
		@note             : 如果在写的过程中发生错误则抛出异常
		@warning          : UDP的设备因为发送数据时需要指定对方的地址所以实现比较特殊,默认第一个Buff用来放地址(sockaddr_in)
		*/
		virtual bool writeEx( AsynIoBuffer * pBuffList,DWORD dwBuffCount,AsynIoHandler * handler) = 0;
	};

	/// 接收器
	struct AsynIoOperation_Acceptor
	{
		/**
		@purpose    : 接收收到的连接
		@param handler: 操作完成后的处理类 
		@note       : 该函数立即返回,读取的结果通过AsynIoResult返回
		@note       : 如果在写的过程中发生错误则抛出异常
		*/
		virtual bool accept(AsynIoHandler * handler) = 0;
	};

	/// 连接器
	struct AsynIoOperation_Connector
	{
		/**
		@purpose    : 向远程主机发起连接
		@param addr : 远程主机地址
		@param handler: 操作完成后的处理类 
		@note       : 该函数立即返回,读取的结果通过AsynIoResult返回
		@note       : 如果在写的过程中发生错误则抛出异常
		*/
		virtual bool connect( sockaddr_in * addr,AsynIoHandler * handler ) = 0;
	};
};

#endif // #ifdef SUPPORT_NET

#endif//__ASYN_IO_OPERATION_H__