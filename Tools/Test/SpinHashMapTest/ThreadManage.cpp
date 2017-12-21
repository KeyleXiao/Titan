#include "StdAfx.h"
#include "ThreadManage.h"
#include "LockfreeQueueProducer.h"
#include "LockfreeQueueConsumer.h"

#include "Thread.h"


ThreadManage::ThreadManage(void)
{
}


ThreadManage::~ThreadManage(void)
{
}

rkt::ThreadPool* ThreadManage::getThreadPool()
{
	return &m_threadPool;
}


void ThreadManage::ThreadStart()
{
	m_threadPool.start();
}

void ThreadManage::ThreadTerminate()
{
	m_threadPool.clear();
}

//////////////////////////////////////////////////////////////////////////

void ThreadManage::LockfreeQueueThreadCreate(int nThreadNum, void (*ProductEndLock)(int, int), void (*ConcurEndLock)(int, int), bool (*bComsumFinished)(void))
{	
	LockfreeQueueProducerThreadCreate(nThreadNum, ProductEndLock);
	LockfreeQueueConsumerThreadCreate(nThreadNum, ConcurEndLock, bComsumFinished);
}


// ¿ªÆôÏß³Ì
void ThreadManage::LockfreeQueueProducerThreadCreate(int nThreadNum, void (*ProductEndLock)(int, int))
{	
	for (int i=0; i<nThreadNum; ++i)
	{
		LockfreeQueueProducer * p = new LockfreeQueueProducer(i+1, ProductEndLock);
		p->Create(THREAD_RUN_COUNT);
		getThreadPool()->add(p);

		m_LockfreeQueueProducerThreadVector.push_back(p);		
	}
	printf("Producer Num:%d\n", m_LockfreeQueueProducerThreadVector.size());
}
void ThreadManage::LockfreeQueueProducerThreadClose()
{
	for (IRUNNABLEVECTOR::iterator iter = m_LockfreeQueueProducerThreadVector.begin(); iter != m_LockfreeQueueProducerThreadVector.end(); ++iter)
	{ 
		LockfreeQueueProducer* p = (LockfreeQueueProducer*)(*iter);
		if (p)
		{
			p->Close();
		}
	}
	m_LockfreeQueueProducerThreadVector.clear();
}
DWORD ThreadManage::GetLockfreeQueueProducerCount()
{
	DWORD dwCount = 0;
	for (IRUNNABLEVECTOR::iterator iter = m_LockfreeQueueProducerThreadVector.begin(); iter != m_LockfreeQueueProducerThreadVector.end(); ++iter)
	{ 
		LockfreeQueueProducer* p = (LockfreeQueueProducer*)(*iter);
		if (p)
		{
			dwCount += p->GetCount();
	//		printf("lockfree producer: %d\t", p->GetCount());
		}
	}
	//printf("\n");

	return dwCount;
}

void ThreadManage::StartConsumerLockfreeQueue()
{
	for (IRUNNABLEVECTOR::iterator iter = m_LockfreeQueueConsumerThreadVector.begin(); iter != m_LockfreeQueueConsumerThreadVector.end(); ++iter)
	{ 
		LockfreeQueueConsumer* p = (LockfreeQueueConsumer*)(*iter);
		if (p)
		{
			rkt::Thread* thread = new rkt::Thread();			
			if(!thread->spawn(p))
			{
				return;
			}			
			thread->start();
		}
	}
}

void ThreadManage::LockfreeQueueConsumerThreadCreate(int nThreadNum, void (*ConcurEndLock)(int, int), bool (*bComsumFinished)(void))
{
	for (int i=0; i<nThreadNum; ++i)
	{
		LockfreeQueueConsumer * p = new LockfreeQueueConsumer(i+1, ConcurEndLock, bComsumFinished);
		p->Create();
		getThreadPool()->add(p);

		m_LockfreeQueueConsumerThreadVector.push_back(p);
	}
}
void ThreadManage::LockfreeQueueConsumerThreadClose()
{
	for (IRUNNABLEVECTOR::iterator iter = m_LockfreeQueueConsumerThreadVector.begin(); iter != m_LockfreeQueueConsumerThreadVector.end(); ++iter)
	{ 
		LockfreeQueueConsumer* p = (LockfreeQueueConsumer*)(*iter);
		if (p)
		{
			p->Close();
		}
	}
	m_LockfreeQueueConsumerThreadVector.clear();
}
DWORD ThreadManage::GetLockfreeQueueConsumerCount()
{
	DWORD dwCount = 0;
	for (IRUNNABLEVECTOR::iterator iter = m_LockfreeQueueConsumerThreadVector.begin(); iter != m_LockfreeQueueConsumerThreadVector.end(); ++iter)
	{ 
		LockfreeQueueConsumer* p = (LockfreeQueueConsumer*)(*iter);
		if (p)
		{
			dwCount += p->GetCount();
	//		printf("Consumer: %d\t", p->GetCount());
		}
	}
//	printf("\n");

	return dwCount;
}
