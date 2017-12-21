#include "StdAfx.h"
#include "LockfreeQueueConsumer.h"
#include "time.h"


LockfreeQueueConsumer::LockfreeQueueConsumer(int nIndex, void (*ConcurEndLock)(int, int), bool (*bComsumFinished)(void))
{
	m_dwCount = 0;
	m_Index = nIndex;
	m_ConcurEnd = ConcurEndLock;
	m_ComsumFinish = bComsumFinished;
}


LockfreeQueueConsumer::~LockfreeQueueConsumer(void)
{
}



bool LockfreeQueueConsumer::Create()
{
	return true;
}

void LockfreeQueueConsumer::Close()
{
	release();

	delete this;
}

void LockfreeQueueConsumer::release()
{

}

void LockfreeQueueConsumer::run()
{
	m_dwCount = 0;
	srand(m_Index * 2654435769U + time(NULL)*2654435769U);
//	srand(m_Index * 2654435769U);
	Product *p = nullptr;
	bool bPopSuc = true;
	double dwTime = GetTickCount();	
	int nRandNum[THREAD_RUN_MIN_COUNT] = {0};
	for (int i=0; i<THREAD_RUN_MIN_COUNT; ++i)
	{
		nRandNum[i] = i+THREAD_RUN_MIN_COUNT*m_Index;
	}
	int nSwapTemp = 0;
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
//	while ((bPopSuc || !m_ComsumFinish())) 
//	while ((bPopSuc || m_dwCount < THREAD_RUN_MIN_COUNT)) 
	while (m_dwCount < THREAD_RUN_MIN_COUNT) 
	{
/*		bPopSuc = lockfree_queue_list.pop(p);
		if (bPopSuc)
		{
			m_ConcurEnd(0, p->GetNumData());
			++m_dwCount;
		}
		else
		{
			Sleep(1);
		}
		//*/

/*		// RingQueue::(pop、pop2、spin_pop、 spin1_pop、spin2_pop、spin3_pop、spin8_pop、mutex_pop)
		Product msg;
		if (g_queue->spin_pop(msg)) {
			bPopSuc = true;
			m_ConcurEnd(0, msg.GetNumData());
			++m_dwCount;
		}
		else {
			bPopSuc = false;
			Sleep(1);
		}		
		//*/
	
/*		spin_hashmap::hashmap<int, Product*>::value_type* pValue = g_hashmap.find(nRandNum[m_dwCount]+m_Index*THREAD_RUN_MIN_COUNT);			
		if (pValue != NULL)
		{
			bPopSuc = true;
			m_ConcurEnd(0, pValue->second->GetNumData());
//			g_hashmap.erase(pValue->first);
			++m_dwCount;
		}
		else
		{
			bPopSuc = false;
		}
		//*/

//		speed::hashmap<int, Product*> g_lockfree_hmap;
/*		int nTemp = 0;
//		bPopSuc = g_lockfree_hmap.find(nRandNum[m_dwCount], nTemp);	
		bPopSuc = g_lockfree_hmap.erase(nRandNum[m_dwCount]);
		if (bPopSuc)
		{
		//	m_ConcurEnd(0, nTemp);			
			++m_dwCount;
		}
		else
		{
			Sleep(1);
		}
		//*/
		
		Product* pData;
		int nTemp = nRandNum[m_dwCount];
	//	bool bPopSuc = g_hashmap.getData(nTemp, pData);
		bool bPopSuc = g_hashmap.find_erase(nTemp, pData);	
	//	bool bPopSuc = g_hashmap.find(nTemp);
		if (bPopSuc)
		{				
			m_ConcurEnd(0, pData->GetNumData());			
			++m_dwCount;
		}
		else
		{
			Sleep(1);
		}
		//*/
	}
	g_queue->tryClear();
	m_ConcurEnd(m_Index, m_dwCount);
}
