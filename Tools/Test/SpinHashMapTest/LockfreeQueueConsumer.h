#pragma once
#include "Thread.h"
//#include <boost/lockfree/queue.hpp>  
#include "Lockfree/queue.h"
#include "Lockfree/hashmap.h"
#include "SpinLock/ringQueue.h"
#include "SpinLock/hashMap.h"
//#include "HashMap.h"
//#include <boost/atomic.hpp> 
#include <iostream>
#include <vector>

//#include "RingQueue.h"
//using namespace jimi;

//extern speed::queue<Product*, false> lockfree_queue_list;
//extern speed::queue_m<Product*>::type_s lockfree_queue_list;
extern RingQueue<Product>* g_queue;
extern spin_hashmap::hashmap<int, Product*> g_hashmap;
extern speed::hashmap<int, int> g_lockfree_hmap;

class LockfreeQueueConsumer : public ::rkt::IRunnable
{
public:
	LockfreeQueueConsumer(int nIndex, void (*ConcurEndLock)(int, int), bool (*bComsumFinished)(void));
	~LockfreeQueueConsumer(void);

	bool Create();
	void Close();
	DWORD GetCount()	{ return m_dwCount; }

	/////////// IRunnable ///////////////////////////////////////////////////////////////
	virtual void release();
	virtual void run();

private:
	int	 m_dwCount;

	int  m_Index;
	void (*m_ConcurEnd)(int, int);		// ¼ì²é
	bool (*m_ComsumFinish)(void);
};

