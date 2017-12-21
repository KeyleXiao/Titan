/*******************************************************************
** 文件名:	AsynIoDevice.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	01/23/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  异步IO设备接口

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __ASYN_IO_DEVICE_H__
#define __ASYN_IO_DEVICE_H__

#include "Config.h"

#ifdef SUPPORT_NET

namespace rkt{
	struct AsynIoResult;
	struct AsynIoHandler;

	struct AsynIoOperation_Reader;   // 异步IO读
	struct AsynIoOperation_Writer;   // 异步IO写
	struct AsynIoOperation_Acceptor; // 异步接收
	struct AsynIoOperation_Connector;// 异步连接

	/**
	@name : 异步IO设备
	@brief: 支持异步IO操作的实例，通常是文件或者Socket
	*/
	struct AsynIoDevice
	{
		/**
		@purpose  : 取得设备句柄
		@return	  :
		*/
		virtual HANDLE GetHandle() const = 0;

		/**
		@purpose  : 取得读取器，如果设备不支持该操作则返回NULL
		@return	  :
		*/
		virtual AsynIoOperation_Reader * Reader() = 0;

		/**
		@purpose  : 取得写入器，如果设备不支持该操作则返回NULL
		@return	  :
		*/
		virtual AsynIoOperation_Writer * Writer() = 0;

		/**
		@purpose  : 取得接收器，如果设备不支持该操作则返回NULL
		@return	  :
		*/
		virtual AsynIoOperation_Acceptor * Acceptor() = 0;

		/**
		@purpose  : 取得连接器，如果设备不支持该操作则返回NULL
		@return	  :
		*/
		virtual AsynIoOperation_Connector * Connector() = 0;

		/**
		@purpose      : 创建一个完成对象
		@param iotype : IO操作类型
		@param handler: 完成事件处理对象
		@note         : 由设备创建完成对象的考虑是:
					  : 1.不同设备的完成对象实现可能略有不同
					  : 2.设备销毁时应该释放所有未完成的Result对象,避免内存泄露
		@note         : 因为可能在多个线程调用,这个函数内部已经做了线程同步
		@return		  : 完成对象指针
		*/
		virtual AsynIoResult * CreateAsynIoResult(int iotype=0,AsynIoHandler * handler=0) = 0;
		
		/**
		@purpose      : 删除一个完成对象
		@param	result: 结果对象指针
		@note         : 因为可能在多个线程调用,这个函数内部已经做了线程同步
		@return		  :
		*/
		virtual void DeleteAsynIoResult(AsynIoResult * result) = 0;

		/**
		@purpose  : 销毁异步IO设备
		@note     : 由于现在采用了引用计数的方式,调用一次Release会减少一次引用,当引用计数=0时则真正释放设备
		@param bForce : 强制关闭
		@return	  : 
		*/
		virtual void Release(bool bForce=false) = 0;

		/**
		@purpose  : 增加引用计数
		@note     : 这个功能用于,当用多个线程对同一个设备进行读写操作的时候,必须对该设备增加一个引用计数
		@note     : 否则的话想要释放设备时会非常麻烦,因为你不知道是否有其他的线程正在引用
		*/
		virtual void   AddRef() = 0;
	};
};

#endif // #ifdef SUPPORT_NET

#endif//__ASYN_IO_DEVICE_H__