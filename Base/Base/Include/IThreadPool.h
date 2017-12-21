//==========================================================================
/**
* @file	  : IThreadPool.h
* @author : PeakGao <peakgao163@163.com>
* created : 2011-8-26   18:00
* purpose : 线程池对象
*/
//==========================================================================

#ifndef __IThreadPool_h__
#define __IThreadPool_h__

namespace rkt {

	/// 任务处理者接口
	struct ITaskHandler
	{
		/// 处理任务，arg为参数
		virtual void onTask(void* arg) = 0;

		/// 释放任务参数(如果是heap上的对象，需要delete)
		virtual void onRelease(void* arg) = 0;
	};


	/// 线程池接口
	struct IThreadPool
	{
		/** 启动线程池
		@param nStatic	表示启动线程池时，立即创建的静态线程数目
		@param nMax		表示线程池最大允许的线程数目(必须大于等于nStatic，否则启动失败)
		@remark 如果创建的是自动调整的线程池，那么线程池会判断任务的繁忙程度，而在 nStatic 和 nMax 之间增删线程数目
		**/
		virtual bool start(ushort nStatic, ushort nMax) = 0;

		/// 停止线程池，未处理的任务被抛弃
		virtual void stop(void) = 0;

		/// 给线程池邮寄一个任务(注意该方法传入的参数是异步执行，所以一般需要将堆上的对象地址传入)
		virtual void addTask(ITaskHandler* handler, void* arg) const = 0;

		/// 释放线程池
		virtual void release() = 0;
	};


	/// 创建一个线程池，autoAdjust标识该线程池是否自动调整线程数目来处理任务
	RKT_API IThreadPool* createThreadPool(bool autoAdjust = false);

} // rkt

#endif // __IThreadPool_h__