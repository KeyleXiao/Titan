//==========================================================================
/**
* @file	  : Thread.h
* @author : PeakGao <peakgao163@163.com>
* created : 2008-10-13   15:59
* purpose : 线程相关
*/
//==========================================================================

#ifndef RKT_THREAD_H
#define RKT_THREAD_H




#include <vector>
#include <Windows.h>

namespace rkt {

	typedef unsigned long	ulong;

	//////////////////////////////////////////////////////////////////////////
	// 原子
	//////////////////////////////////////////////////////////////////////////
	/// 伪装的原子
	class FakeAtom
	{
		long mValue;
	public:
		FakeAtom(long val = 0) : mValue(val)	{ }
		long operator++()						{ return ++mValue; }
		long operator--()						{ return --mValue; }
		long operator=(long val)				{ return (mValue = val); }
		long operator+=(long val)				{ return mValue += val; }
		bool cas(long oldVal, long newVal)
		{
			if (mValue == oldVal)
			{
				mValue = newVal;
				return true;
			}
			return false;   
		}
		operator long () const					{ return mValue; }
	};

	/// 多线程原子
	class Atom
	{
		long volatile mValue;
	public:
		Atom(long val = 0);
		long operator++();
		long operator--();
		long operator=(long val);
		long operator+=(long val);
		bool cas(long oldVal, long newVal);
		operator long () const					{ return mValue; }
	};




	//////////////////////////////////////////////////////////////////////////
	// 互斥体
	//////////////////////////////////////////////////////////////////////////
	class Mutex
	{
		CRITICAL_SECTION m_cs;

	public:
		Mutex(bool = true);
		~Mutex();

		void Lock();
		void Unlock();
	};

	/// 伪装的（空的）互斥体
	class FakeMutex
	{
	public:
		FakeMutex(bool = true){}

		void Lock()		{}
		void Unlock()	{}
	};


	//////////////////////////////////////////////////////////////////////////
	// 守卫
	//////////////////////////////////////////////////////////////////////////
	/// 临界区线程安全自动保护
	template<class _Mutex = Mutex>
	class ResGuard
	{
		_Mutex& mMutex;
	public:
		ResGuard(_Mutex& mtx) : mMutex(mtx)								{ mMutex.Lock(); }
		~ResGuard()														{ mMutex.Unlock(); }
	};


	//////////////////////////////////////////////////////////////////////////
	// 线程模型
	//////////////////////////////////////////////////////////////////////////
	/// 单线程模型
	class SingleThread
	{
	public:
		typedef FakeAtom			atom;
		typedef FakeMutex			mutex;
		typedef ResGuard<mutex>		scopelock;
	};

	/// 多线程模型
	class MultiThread
	{
	public:
		typedef Atom				atom;
		typedef Mutex				mutex;
		typedef ResGuard<mutex>		scopelock;
	};

	/// 信号量
	class Semaphore
	{
		void* m_h;
	public:
		Semaphore(int initCount = 0);
		~Semaphore();

		bool Wait(ulong timeOut = 0xffffffff);
		void Post();
	};


	//////////////////////////////////////////////////////////////////////////
	// 线程
	//////////////////////////////////////////////////////////////////////////
	/// 可运行对象
	struct IRunnable
	{
		virtual void run() = 0;
		virtual void release() = 0;
	};


	/// 简单线程封装
	class Thread
	{
	protected:
		ulong	mThreadId;
		void*	mThreadHandle;

	public:
		Thread();
		~Thread();

	public:
		bool spawn(IRunnable* task, int priority = 0, bool bStart=false);
		void wait();
		void start();
		void pause();
		void terminate();
		static void sleep(unsigned long timeout);

	protected:
		static unsigned int __stdcall _dispatch(void* arg);
	};


	//////////////////////////////////////////////////////////////////////////
	// 线程池
	//////////////////////////////////////////////////////////////////////////
	/// 简单线程池
	class ThreadPool
	{
	protected:
		typedef std::vector<Thread*>	ThreadList;
		ThreadList	mThreads;
		bool mAutoRemove;	/// 当工作线程执行结束后，自动删除

	public:
		ThreadPool::ThreadPool(void)
		{
		}

		ThreadPool::~ThreadPool(void)
		{
			clear();
		}

		Thread * ThreadPool::add(IRunnable* task, int priority = 0, bool bStart=false)
		{
			Thread* thread = new Thread();			
			if(!thread->spawn(task, priority))
			{
				return NULL;
			}

			mThreads.push_back(thread);

			return thread;
		}

		void ThreadPool::start()
		{
			for (ThreadList::iterator it=mThreads.begin(); it!=mThreads.end(); ++it)
			{
				(*it)->start();
			}
		}

		void ThreadPool::pause()
		{
			for (ThreadList::iterator it=mThreads.begin(); it!=mThreads.end(); ++it)
			{
				(*it)->pause();
			}
		}

		void ThreadPool::wait()
		{
			for (ThreadList::iterator it=mThreads.begin(); it!=mThreads.end(); ++it)
			{
				(*it)->wait();
			}
		}

		void ThreadPool::clear()
		{
			for (ThreadList::iterator it=mThreads.begin(); it!=mThreads.end(); ++it)
			{
				(*it)->terminate();
				delete *it;
			}

			mThreads.clear();
		}

		void ThreadPool::remove(Thread * thread)
		{
			for (ThreadList::iterator it=mThreads.begin(); it!=mThreads.end(); ++it)
			{
				if(thread == (*it))
				{
					delete *it; mThreads.erase(it); 
					
					return;
				}
			}
		}

		size_t getThreadCount() const	{ return mThreads.size(); }
	};

} // namespace

#endif // #ifdef SUPPORT_THREAD

