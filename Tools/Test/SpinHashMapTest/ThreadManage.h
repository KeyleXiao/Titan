#pragma once

#include <vector>
#include "Thread.h"

class LockfreeQueueProducer;
class LockfreeQueueConsumer;

// 线程管理器
class ThreadManage
{
public:
	ThreadManage(void);
	~ThreadManage(void);

	rkt::ThreadPool* getThreadPool();

	void ThreadStart();
	void ThreadTerminate();

	void LockfreeQueueThreadCreate(int nThreadNum, void (*ProductEndLock)(int, int), void (*ConcurEndLock)(int, int), bool (*bComsumFinished)(void));
	DWORD GetLockfreeQueueProducerCount();
	DWORD GetLockfreeQueueConsumerCount();
	void StartConsumerLockfreeQueue();

	// 开启线程
	void LockfreeQueueProducerThreadCreate(int nThreadNum, void (*ProductEndLock)(int, int));
	void LockfreeQueueProducerThreadClose();

	void LockfreeQueueConsumerThreadCreate(int nThreadNum, void (*ConcurEndLock)(int, int), bool (*bComsumFinished)(void));
	void LockfreeQueueConsumerThreadClose();

private:
	
private:
	rkt::ThreadPool m_threadPool;

	typedef std::vector<::rkt::IRunnable*>	IRUNNABLEVECTOR;
	IRUNNABLEVECTOR m_LockfreeQueueProducerThreadVector;
	IRUNNABLEVECTOR m_LockfreeQueueConsumerThreadVector;
};

