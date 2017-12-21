/*******************************************************************
** 文件名:	Proactor.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	01/23/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  异步事件分解模型 （前摄器 ）	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __PROACTOR_H__
#define __PROACTOR_H__

#include "Config.h"

#ifdef SUPPORT_NET

namespace rkt{

	struct AsynIoDevice;
	struct AsynIoResult;

	/**
	@name : 异步事件分解模型 （前摄器 ）
	@brief: 功能 : 分解完成事件
			实现 : Win32下调用完成端口API实现 / unix下暂无
	*/
	struct Proactor
	{
		/**
		@purpose         : 创建完成端口
		@param nThreadNum: 该完成端口允许的最大并发工作线程数
		@note            : 线程数如果指定为0则系统会启动和CPU相同个数的线程
		@note            : 推荐的线程数 = CPU个数 * 2 + 2
		@return		     : 创建成功则返回True
		*/
		virtual bool Create(unsigned long nConcurrentThreadsNum =0) = 0;

		/**
		@purpose          : 关闭完成端口
		@return		      :
		*/
		virtual void Close() = 0;

		/**
		@purpose          : 注册设备(把支持异步IO的设备和完成端口关联)
		@param	   device : 设备指针 
		@return		      : 注册成功则返回true
		*/
		virtual bool RegisterDevice(AsynIoDevice * device) = 0;

		/**
		@purpose          : 等待完成事件,如有操作完成则调用AsynIoHandler
		@param wait       : 等待的毫秒数
		@note	          : 在消息循环中持续调用该函数
		@note             : 例: Proactor proactor;
		@note             :     while(proactor.HandleEvents()) {}
		@return		      :
		*/
		virtual bool HandleEvents(unsigned long wait=INFINITE) = 0;

		/**
		@purpose          : 完成事件处理循环
		@note             : 等同: while(proactor.HandleEvents()) {}
		@return		      :
		*/
		virtual void HandleEventsLoop() = 0;

		/**
		@purpose          : 人工投递一个完成事件(用来模拟异步IO的过程)
		@param   result   : 完成对象指针
		@return		      : 
		*/
		virtual bool PostCompletion( AsynIoResult * result) = 0;
	};
};

#endif // #ifdef SUPPORT_NET

#endif//__PROACTOR_H__