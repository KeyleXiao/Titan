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

#include "Config.h"

#ifdef SUPPORT_THREAD

#include <vector>

#if defined(RKT_WIN32) && !defined(USE_ASSEMBLY_MUTEX)
#include <Windows.h>
#endif

namespace rkt {

// Create thread
#ifdef RKT_WIN32
	typedef unsigned int (RKT_STDCALL * _FN_THREAD_ROUTINE)(void* pvContext);
	typedef _FN_THREAD_ROUTINE FN_THREAD_ROUTINE;

	#define beginThread(psa, iStack, fnStartAddr, \
		pvContext, dwCreate, piThreadId) \
		((HANDLE)::_beginthreadex( \
		(void*)						(psa), \
		(unsigned)					(iStack), \
		(rkt::FN_THREAD_ROUTINE)	(fnStartAddr), \
		(void*)						(pvContext), \
		(unsigned)					(dwCreate), \
		(unsigned*)					(piThreadId)))
#else
	typedef void* (SYS_API * _FN_THREAD_ROUTINE)(void* pvContext);
	typedef _FN_THREAD_ROUTINE FN_THREAD_ROUTINE;

	#define beginThread(pThreadId, pAttr, fnStartAddr, pvContext) \
		::pthread_create( \
									pThreadId, \
									pAttr, \
		(rkt::FN_THREAD_ROUTINE)	(fnStartAddr), \
		(void*)						(pvContext))
#endif

#define BEGIN_THREAD	beginThread // for compatible



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
	class RKT_EXPORT Atom
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
#ifdef RKT_WIN32
#ifdef USE_ASSEMBLY_MUTEX
	class RKT_EXPORT Mutex
	{
		Atom	mAtom;
#ifdef RKT_DEBUG
	public:
		const char* file;
		int			line;
#endif

	public:
		Mutex(bool = true);

#ifdef RKT_DEBUG
		void Lock(const char* =0, int =0);
#else
		void Lock();
#endif
		void Unlock();
	};
#else
	class RKT_EXPORT Mutex
	{
		CRITICAL_SECTION m_cs;

	#ifdef RKT_DEBUG
		const char* file;
		int			line;
	#endif

	public:
		Mutex(bool = true);
		~Mutex();

	#ifdef RKT_DEBUG
		void Lock(const char* f=0, int l=0);
		void Unlock();
	#else
		void Lock();
		void Unlock();
	#endif
	//	bool trylock(){ return ::TryEnterCriticalSection(&m_cs); }
	};
#endif // USE_ASSEMBLY_MUTEX
#else
	class RKT_EXPORT Mutex
	{
	public:
		Mutex(bool recursive = true)
		{
			if(recursive)
			{
				pthread_mutexattr_t attr;
				::pthread_mutexattr_init(&attr);
				::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
				::pthread_mutex_init(&m_mutex, &attr);
				::pthread_mutexattr_destroy(&attr);
			}
			else
			{
				::pthread_mutex_init(&m_mutex, NULL);
			}
		}
		~Mutex(){ ::pthread_mutex_destroy(&m_mutex); }
		void Lock(){ ::pthread_mutex_lock(&m_mutex); }
		void Unlock(){ ::pthread_mutex_unlock(&m_mutex); }
	//	bool trylock(){ return ::pthread_mutex_trylock(&m_mutex) == 0; }
	private:
		pthread_mutex_t m_mutex;
	};
#endif // RKT_WIN32


	/// 伪装的（空的）互斥体
	class FakeMutex
	{
	#ifdef RKT_DEBUG
	public:
		const char* file;
		int			line;
	#endif

	public:
		FakeMutex(bool = true){}

	#ifdef RKT_DEBUG
		void Lock(const char* =0, int =0)	{}
	#else
		void Lock()		{}
	#endif
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
	#ifdef RKT_DEBUG
		ResGuard(_Mutex& mtx, const char* f=0, int l=0) : mMutex(mtx)	{ mMutex.Lock(f, l); }
		~ResGuard()														{ mMutex.Unlock(); }
	#else
		ResGuard(_Mutex& mtx) : mMutex(mtx)								{ mMutex.Lock(); }
		~ResGuard()														{ mMutex.Unlock(); }
	#endif
	};


	//////////////////////////////////////////////////////////////////////////
	// 线程模型
	//////////////////////////////////////////////////////////////////////////
	/// 单线程模型
	class RKT_EXPORT SingleThread
	{
	public:
		typedef FakeAtom			atom;
		typedef FakeMutex			mutex;
		typedef ResGuard<mutex>		scopelock;
	};

	/// 多线程模型
	class RKT_EXPORT MultiThread
	{
	public:
		typedef Atom				atom;
		typedef Mutex				mutex;
		typedef ResGuard<mutex>		scopelock;
	};


	/// 信号量
#ifdef RKT_WIN32
	class RKT_EXPORT Semaphore
	{
		void* m_h;
	public:
		Semaphore(int initCount = 0);
		~Semaphore();

		bool Wait(ulong timeOut = 0xffffffff);
		void Post();
	};
#else
	class RKT_EXPORT Semaphore
	{
		sem_t m_sem;
	public:
		Semaphore(int initCount = 0)
		{
			int ret = ::sem_init(&m_sem, 0, initCount);		
		}
		~Semaphore(){ ::sem_destroy(&m_sem); }
		bool Wait(ulong timeOut = 0xffffffff)
		{
			int ret = 0;
			if(timeOut == 0xffffffff)
			{
				while(1)
				{
					ret = ::sem_wait(&m_sem);
					if(ret == 0) 
					{
						return TRUE;
					}
					
				}
			}
			else
			{
				timespec ts;
				ret = ::clock_gettime(CLOCK_REALTIME, &ts);			
				ts.tv_sec += timeOut / 1000;
				ts.tv_nsec += (timeOut % 1000) * 1000000;
				if(ts.tv_nsec >= 1000000000)
				{
					ts.tv_sec += 1;
					ts.tv_nsec -= 1000000000;
				}
				while(1)
				{
					ret = ::sem_timedwait(&m_sem, &ts);
					if(ret == 0) return TRUE;
					if(::GetLastError() == EINTR) 
					{
						continue;
					}				
					return FALSE;
				}
			}

			return FALSE;
		}
		void Post(){ int ret = ::sem_post(&m_sem); }
	};
#endif


	// 采用事件通知模式时，每次SetEvent都会切入到内核态，会消耗很多CPU（3.0GHZ10万次31毫秒)
	// 封装一种效率更高的事件,如果上一次SetEvent还没被响应，就不用冗余的调用SetEvent
	// 用法是每次响应Event后，需要调用ResetEvent将状态变量复位
	class RKT_EXPORT FastEvent
	{
		Atom   m_Flag;
		HANDLE m_eventHandle;

	public:
		FastEvent()
		{
			m_eventHandle = ::CreateEvent( NULL, FALSE, FALSE, NULL );
		}

		virtual ~FastEvent()
		{
			if ( m_eventHandle!=INVALID_HANDLE_VALUE )
			{
				::CloseHandle(m_eventHandle);
				m_eventHandle = INVALID_HANDLE_VALUE;
			}
		}

		HANDLE getHandle() const
		{
			return m_eventHandle;
		}

		// 激发事件
		void setEvent()
		{
			if ( (long)m_Flag==0 )
			{
				// 这个地方有多个线程会调用,所以必须用原子操作保证标志位不被写错
				// 没被设成也没关系，只要标志位不被错误的写成1，那样就永远不会被触发了
				if ( m_Flag.cas( 0,1 )==false )
					return;

				::SetEvent(m_eventHandle);
			}
		}

		// 复位事件
		void resetEvent()
		{
			m_Flag.cas( 1,0 );
		}
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
	class RKT_EXPORT Thread
	{
	protected:
		ulong	mThreadId;
		void*	mThreadHandle;

	public:
		Thread();
		~Thread();

	public:
		bool spawn(IRunnable* task, int priority = 0);
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

		Thread * ThreadPool::add(IRunnable* task, int priority = 0)
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

#endif // RKT_THREAD_H
