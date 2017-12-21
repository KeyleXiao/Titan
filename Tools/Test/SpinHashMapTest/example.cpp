// example.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <Windows.h>
#include<time.h>
#include<stdlib.h>
#include <vector>
//#include <boost/atomic.hpp>  

//#include "testQ3.h"
#include "SpinLock/ringQueue.h"
#include "SpinLock/hashMap.h"
#include "SpinLock/hashMapElemDef.h"
//#include "HashMap.h"
#include "Lockfree/hashmap.h"
#include "ThreadManage.h"

//#include "RingQueue.h"
//using namespace jimi;
using namespace std;

_LARGE_INTEGER time_start;		/*开始时间*/
_LARGE_INTEGER time_over;       /*结束时间*/
LARGE_INTEGER f;				/*计时器频率*/
double dqFreq ;					/*计时器频率*/

RingQueue<Product>* g_queue;

spin_hashmap::hashmap<int, Product*> g_hashmap(128, 1);
speed::hashmap<int, int> g_lockfree_hmap;

void lockfree_queue_thread_test();

//////////////////////////////////////////////////////////////////////////
#define THREAD_PRINT_INFO(info) \
	QueryPerformanceCounter(&time_over);\
	std::cout << (info) << " run "<< THREAD_COUNT <<" count, take "<< ((time_over.QuadPart-time_start.QuadPart)/dqFreq)*1000 <<" ms" << std::endl << std::endl; 

ThreadManage g_threadManage;

/*
bool CheckProductConcur (void)
{
	return g_threadManage.GetConcurrencyQueueProducerCount() >= THREAD_RUN_COUNT;
}

bool CheckProductLock (void)
{
	return g_threadManage.GetLockfreeQueueProducerCount() >= THREAD_RUN_COUNT*THREAD_COUNT;
}
//*/

atomic_m<int> nConsumCountLock(0);
atomic_m<int> nConsumCountConcur(0);

//////////////////////////////////////////////////////////// Lock free
atomic_m<bool> bShowProLock(false);
void ProductEndLock(int nIndex, int nNum)
{
	nConsumCountLock.AddValue(nNum);
	printf("Product:%d Index:%d Add:%d\n", nConsumCountLock.GetValue(), nIndex, nNum);
	if (!bShowProLock.GetValueUnsafe() && nConsumCountLock.GetValue() >= THREAD_RUN_COUNT*THREAD_COUNT)
	{
		bShowProLock.SetValue(true);
		printf("End ConcurrentHashMap Product !!!\n");
//		g_threadManage.StartConsumerLockfreeQueue();
	}
}


atomic_m<int>  g_nFlagGet[THREAD_RUN_COUNT];
atomic_m<bool> bShowConLock(false);  
atomic_m<bool> bThreshBegin(true); 
void ConcurEndLock(int nIndex, int nNum)
{
	if (nIndex == 0)
	{		
		g_nFlagGet[nNum-1].AddValue(1);
	}
	else
	{
		nConsumCountConcur.AddValue(nNum);
		printf("Consumer:%d Index:%d Del:%d\n", nConsumCountConcur.GetValue(), nIndex, nNum);
		if (!bShowConLock.GetValueUnsafe() && nConsumCountConcur.GetValue() >= THREAD_RUN_COUNT*THREAD_COUNT)
		{			
			bShowConLock.SetValue(true);			
			THREAD_PRINT_INFO("ConcurrentHashMap");			
			int nMin = 100;
			int nMax = 0;
			for (int i=0; i<THREAD_RUN_COUNT; ++i)
			{
				if (nMin > g_nFlagGet[i].GetValueUnsafe())	
				{
					nMin = g_nFlagGet[i].GetValueUnsafe();					
				}
				if (nMax < g_nFlagGet[i].GetValueUnsafe())				
					nMax = g_nFlagGet[i].GetValueUnsafe();
				
			}			
			printf("Min Num = %d  Max Num = %d\n", nMin, nMax);

			printf("ConcurrentHashMap producer_count=%d, consumer_count=%d\n",
				g_threadManage.GetLockfreeQueueProducerCount(), g_threadManage.GetLockfreeQueueConsumerCount());
			

			g_threadManage.LockfreeQueueProducerThreadClose();	
			g_threadManage.LockfreeQueueConsumerThreadClose();		
//			g_threadManage.ThreadTerminate();	
			memset(g_nFlagGet, 0, sizeof(atomic_m<int>)*THREAD_RUN_COUNT);			
			nConsumCountConcur.SetValue(0);
			nConsumCountLock.SetValue(0);			
			bThreshBegin.SetValue(true);

			return;			
			Sleep(2000);
			lockfree_queue_thread_test();			
		}
	}
}


void CallHashMapList()
{
	Sleep(10);
	vector<pair<int, size_t> > vctTemp;
	spin_hashmap::hashmap<int, Product*>::iterator iter = g_hashmap.begin();
	while(!bShowConLock.GetValue())
	{			
		if (iter == g_hashmap.end())
		{			
			continue;
			iter = g_hashmap.begin();					
		}
		vctTemp.push_back(make_pair<int, size_t>(iter->second->GetNumData(), iter->GetHashKey()));		
		++iter;
		
		if (vctTemp.size() > 100 || iter == g_hashmap.end())
		{
			for (size_t i = 0; i<vctTemp.size(); ++i)
			{
				printf("%8X(%5d) ", vctTemp[i].second, vctTemp[i].first);
			}
			vctTemp.clear();
			printf("\n End\n");			
		}		

		Sleep(1);
	}
	printf("END SHOW LIST !!!!!!!!!!!!!!!!!!!!!!!\n");
}
//*/

bool bComsumFinished(void)
{
	return g_threadManage.GetLockfreeQueueConsumerCount() >= THREAD_RUN_COUNT*THREAD_COUNT;
}

//////////////////////////////////////////////////////////////////////////


void lockfree_queue_thread_test()
{
	printf("ConcurrentHashMap_test\n");
	DWORD start= GetTickCount(); 	 
	g_threadManage.LockfreeQueueThreadCreate(THREAD_COUNT, ProductEndLock, ConcurEndLock, bComsumFinished);
	QueryPerformanceCounter(&time_start);
	g_threadManage.ThreadStart();
	bShowConLock.SetValue(false);
	CallHashMapList();
}


int main()
{		
	srand(time(NULL));
	RingQueue<Product> ringQueue;
	g_queue = &ringQueue;
	do
	{
		if (bThreshBegin.GetValueUnsafe())
		{
			bThreshBegin.SetValue(false);
			g_hashmap.clear();
			g_lockfree_hmap.clear();
			
			g_threadManage.ThreadTerminate();
			QueryPerformanceFrequency(&f);
			dqFreq=(double)f.QuadPart;
			//No_Thread_Test();
			lockfree_queue_thread_test();
		}		
		Sleep(2000);
	}while(1);
	
	int n;
	std::cin >> n;

	return 0;
}