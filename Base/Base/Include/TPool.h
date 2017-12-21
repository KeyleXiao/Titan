//==========================================================================
/**
* @file	  : TPool.h
* @author : PeakGao <peakgao163@163.com>
* created : 2011-8-26   17:55
* purpose : 
*/
//==========================================================================

#ifndef __TPool_h__
#define __TPool_h__

#include "Config.h"

#ifdef SUPPORT_THREAD
#include "IThreadPool.h"

namespace rkt {

	class FixThreadPool : public IThreadPool
	{
	protected:
		/// 内部工作者线程信息
		struct ThreadInfo
		{
			HANDLE			threadHandle;	/// 线程句柄
			unsigned		threadId;		/// 线程ID
			volatile bool	isBusy;			/// 忙状态
			volatile bool	isQuit;			/// 退出状态

			ThreadInfo() : threadHandle(NULL), threadId(0), isBusy(false), isQuit(false)
			{
			}
		};

		enum { WORKERPROC_RETURN_VALUE = 10001 };


		ThreadInfo*			mThreadList;			/// 线程数组
		HANDLE				mWorkerPort;			/// 工作线程绑定的完成端口

		volatile ushort		mStaticThreadNum;		/// 初始创建的线程数
		volatile ushort		mMaxThreadNum;			/// 最大线程数
		ulong				mWaitTime;				/// Timeout时间

		CRITICAL_SECTION	mCS;					/// 线程数组互斥对象

	public:
		FixThreadPool();
		virtual ~FixThreadPool();

		virtual bool start(ushort nStatic, ushort nMax);
		virtual void stop(void);
		virtual void addTask(ITaskHandler* handler, void* arg) const;
		virtual void release();

	protected:
		static unsigned __stdcall workerProc(void* pThread);
		int getThread(ulong threadId) const;
	};



	class DynThreadPool : public FixThreadPool
	{
	protected:
		enum { MANAGERPROC_RETURN_VALUE	= 10002 };
		enum ThreadStatus { BUSY, NORMAL, IDLE };

		volatile bool		mQuitManager;		/// 退出管理线程的标志
		HANDLE				mManagerPort;		/// 管理者线程的完成端口
		HANDLE				mManagerThread;		/// 管理者线程

	public:
		DynThreadPool();
		virtual ~DynThreadPool();

		virtual bool start(ushort nStatic,ushort nMax);
		virtual void stop(void);

	protected:
		ThreadStatus getWorkThreadStatus() const;
		void addThread(void);
		void removeThread(void);
		static	unsigned __stdcall managerProc(void* pThread);
	};

} // rkt

#endif // #ifdef SUPPORT_THREAD

#endif // __TPool_h__
