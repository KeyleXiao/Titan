#pragma once
#include "Thread.h"
// #include <boost/lockfree/queue.hpp>
#include "Lockfree/queue.h"
#include "Lockfree/hashmap.h"
#include "SpinLock/ringQueue.h"
#include "SpinLock/hashMap.h"
//#include "HashMap.h"
#include <iostream>
#include <vector>

//#include "RingQueue.h"
//using namespace jimi;

//extern speed::queue<Product*, false> lockfree_queue_list;
//extern speed::queue_m<Product*>::type_s lockfree_queue_list;
extern RingQueue<Product>* g_queue;
extern spin_hashmap::hashmap<int, Product*> g_hashmap;
extern speed::hashmap<int, int> g_lockfree_hmap;

// 生产队列
class LockfreeQueueProducer : public ::rkt::IRunnable
{
public:
	LockfreeQueueProducer(int nIndex, void (*ProductEndLock)(int, int));
	~LockfreeQueueProducer(void);

	bool Create(int nCount);
	void Close();

	DWORD GetCount()	{ return m_dwCount; }
	/////////// IRunnable ///////////////////////////////////////////////////////////////
	virtual void release();
	virtual void run();

	void ProductEnd(int nNum);
private:
	std::vector<Product*> m_vecProduct;
	int	m_dwCount;

	int m_Index;
	void (*m_ProductEnd)(int, int);		// 检查

};

