#include "StdAfx.h"
#include "LockfreeQueueProducer.h"
#include "time.h"

//speed::queue<Product*> lockfree_queue_list(int(THREAD_RUN_COUNT*THREAD_COUNT +2));  
//speed::queue<Product*, false> lockfree_queue_list(128);  
//speed::queue_m<Product*>::type_s lockfree_queue_list(128);

LockfreeQueueProducer::LockfreeQueueProducer(int nIndex, void (*ProductEndLock)(int, int))
{
	m_dwCount = 0;
	m_Index = nIndex;
	m_ProductEnd = ProductEndLock;
}


LockfreeQueueProducer::~LockfreeQueueProducer(void)
{
}


bool LockfreeQueueProducer::Create(int nCount)
{
	if (nCount <= 0)
	{
		return false;
	}

	std::string lTest(Test_String); 
	for (int i = 0; i < nCount; ++i)
	{		
		m_vecProduct.push_back( new Product(i+1) );
//		m_vecProduct.push_back( new Product(lTest.c_str(), lTest.size()) );
	}

	return true;
}

void LockfreeQueueProducer::Close()
{
	release();

	delete this;
}

void LockfreeQueueProducer::release()
{
	for (std::vector<Product*>::iterator iter = m_vecProduct.begin(); iter != m_vecProduct.end(); ++iter)
	{  
		SAFE_DELETE(*iter);
	}
	m_vecProduct.clear();
}

void LockfreeQueueProducer::run()
{
	int nfail = 0;
	m_dwCount = 0;	
	srand(m_Index * 2654435769U + time(NULL)*2654435769U);
//	srand(m_Index * 2654435769U );
	int nRandNum[THREAD_RUN_MIN_COUNT] = {};
	for (int i=0; i<THREAD_RUN_MIN_COUNT; ++i)
	{
		nRandNum[i] = i+THREAD_RUN_MIN_COUNT*m_Index;
	}
/*	int nSwapTemp = 0;
	for (int i=0; i<THREAD_RUN_MIN_COUNT; ++i)
	{
		int nRand = rand()%(THREAD_RUN_MIN_COUNT - i);
		if (nRand != 0)
		{
			nSwapTemp = nRandNum[nRand+i];
			nRandNum[nRand+i] = nRandNum[i];
			nRandNum[i] = nSwapTemp;
		}		
	}
	//*/
//	for (std::vector<Product*>::iterator iter = m_vecProduct.begin(); iter != m_vecProduct.end(); ++iter)
	for (size_t i=0; i<m_vecProduct.size(); ++i)
	{  
/*		while(!lockfree_queue_list.push((*iter)))
		{
			Sleep(1);
		};
		//*/

		// RingQueue:: test some type cost time(ms) E.g:16*10W push, 16*10W pop
		// push(FFFF)、push2(FFFF)、spin_push(90+)、 spin1_push(110+)、spin2_push(110+)、
		// spin3_push(90+)、spin8_push(90+)、mutex_push(400~1500)
		// push、push2 CAS方式,ABA处理?，原子操作不安全
/*		while (!g_queue->spin_push(*(*iter))) {
		//	Sleep(1);
		};
		//*/

/*		while(!g_hashmap.insert(make_pair(nRandNum[i]+THREAD_RUN_MIN_COUNT*m_Index, m_vecProduct[i])))
		{
		//	spin_hashmap::hashmap<int, Product*>::value_type* pValue = g_hashmap.find(nRandNum[m_dwCount]+m_Index*THREAD_RUN_MIN_COUNT);			
			++nfail;
			if(nfail % 100 == 99)
			{
				printf("Fail Time:%d !!!!!!!!!!!\n", nfail);
				Sleep(1);
			}
		//	Sleep(1);
		}		
		//*/

/*		while(!g_lockfree_hmap.insert(nRandNum[i], m_vecProduct[i]->m_nData))
		{		
			++nfail;
			if(nfail % 100 == 99)
			{
				printf("Fail Time:%d !!!!!!!!!!!\n", nfail);
				Sleep(1);
			}
			//	Sleep(1);
		}		
		//*/

		g_hashmap.setData(nRandNum[i], m_vecProduct[i]);
/*		while(!g_hashmap.insert(nRandNum[i], m_vecProduct[i]))
		{
		//	spin_hashmap::hashmap<int, Product*>::value_type* pValue = g_hashmap.find(nRandNum[m_dwCount]+m_Index*THREAD_RUN_MIN_COUNT);			
			printf("Fail Time:%d !!!!!!!!!!!\n", nfail);
			Sleep(1);
		}		
		//*/
		++m_dwCount;
	}
//	printf("LockfreeQueueProducer Index:%d\n", m_Index);	
	m_ProductEnd(m_Index, m_dwCount);
}