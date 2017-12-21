//==========================================================================
/**
* @file	  : Thread.cpp
* @author : PeakGao <peakgao163@163.com>
* created : 2008-7-29   15:45
*/
//==========================================================================

#include "stdafx.h"
#include "Thread.h"

#define USE_ASSEMBLY_ATOM // 是否使用汇编的原子操作

namespace rkt {


	//////////////////////////////////////////////////////////////////////////
	// 原子
	//////////////////////////////////////////////////////////////////////////
	Atom::Atom(long val)
	{
		operator=(val);
	}

	long Atom::operator++()
	{
#ifdef USE_ASSEMBLY_ATOM
		long volatile* target = &mValue;
		__asm
		{
			mov      ecx, target;
			mov      eax, 1;
			lock xadd [ecx], eax;
			inc      eax;
		}
#else
		return ::InterlockedIncrement(&mValue);
#endif
	}

	long Atom::operator--()
	{
#ifdef USE_ASSEMBLY_ATOM
		long volatile* target = &mValue;
		__asm
		{
			mov     ecx, target;
			mov     eax, 0FFFFFFFFh;	// -1
			lock xadd [ecx], eax;		// 加-1
			dec     eax;
		}
#else
		return ::InterlockedDecrement(&mValue);
#endif
	}

	long Atom::operator=(long val)
	{
#ifdef USE_ASSEMBLY_ATOM
		long volatile* target = &mValue;
		__asm
		{
			mov      ecx, target;
			mov      edx, val;
label:
			lock cmpxchg [ecx], edx;
			jnz      short label;
		}
#else
		return ::InterlockedExchange(&mValue, val);
#endif
	}

	long Atom::operator+=(long val)
	{
#ifdef USE_ASSEMBLY_ATOM
		long volatile* target = &mValue;
		_asm
		{
			mov      ecx, target;
			mov      eax, val;
			lock xadd [ecx], eax;
		}
#else
		return ::InterlockedExchangeAdd(&mValue, val);
#endif
	}

	bool Atom::cas(long oldVal, long newVal)
	{
#ifdef USE_ASSEMBLY_ATOM
		long volatile* target = &mValue;
		register bool f;
		_asm
		{
			mov ecx, target;
			mov eax,oldVal;
			mov ebx,newVal;
			lock cmpxchg [ecx],ebx;
			setz f;
		}
		return f;
#else
		return ::InterlockedCompareExchange(&mValue, newVal, oldVal) == oldVal;
#endif
	}






	//////////////////////////////////////////////////////////////////////////
	// 互斥体
	Mutex::Mutex(bool)
	{ 
		::InitializeCriticalSection(&m_cs); 
	}

	Mutex::~Mutex()
	{
		::DeleteCriticalSection(&m_cs); 
	}


	void Mutex::Lock()								
	{ ::EnterCriticalSection(&m_cs); }

	void Mutex::Unlock()							
	{ ::LeaveCriticalSection(&m_cs); }



	// 信号量
	Semaphore::Semaphore(int initCount)
	{
		m_h = ::CreateSemaphore(NULL, initCount, 0x7fffffff, NULL);	
	}

	Semaphore::~Semaphore()
	{ 
		if (m_h)
			::CloseHandle(m_h); 
	}

	bool Semaphore::Wait(ulong timeOut)
	{		
		ulong ret = ::WaitForSingleObject(m_h, timeOut);
		if (ret == WAIT_OBJECT_0) 
			return true;
		
		return false;
	}

	void Semaphore::Post()
	{ 
		::ReleaseSemaphore(m_h, 1, NULL);
	}




	// 简单线程封装
	Thread::Thread() : mThreadId(0), mThreadHandle(0)
	{
	}

	Thread::~Thread()
	{
		if ( mThreadHandle !=0 )
		{
			::TerminateThread(mThreadHandle,0);
			mThreadHandle = 0;
		}
	}

	bool Thread::spawn(IRunnable* task, int priority, bool bStart) 
	{
		mThreadHandle = ::CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&_dispatch, task, CREATE_SUSPENDED, (DWORD*)&mThreadId);
		::SetThreadPriority(mThreadHandle, priority);
		if (bStart)
		{
			::ResumeThread(mThreadHandle);
		}
		return mThreadHandle != NULL;
	}

	void Thread::wait()
	{
		if(mThreadHandle == NULL)
			return;
		::WaitForSingleObject(mThreadHandle, INFINITE);

		mThreadHandle = NULL;
	}

	void Thread::start()
	{
		::ResumeThread(mThreadHandle);
	}

	void Thread::pause()
	{
		::SuspendThread(mThreadHandle);
	}

	void Thread::terminate()
	{
		::TerminateThread(mThreadHandle, 0);
		::CloseHandle(mThreadHandle);
		mThreadHandle = NULL;		
	}

	void Thread::sleep(unsigned long timeout)
	{
		Sleep(timeout);
	}

	unsigned int __stdcall Thread::_dispatch(void* arg)
	{
		IRunnable* task = reinterpret_cast<IRunnable*>(arg);
		task->run();
		return 0;
	}


} // namespace

