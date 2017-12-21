
#include "stdafx.h"
#include <iostream>
#include <Windows.h>

#include "test.h"

#include "ProductQueue.h"
#include "ProducerConsumerQueue.h"
#include "ProductConcurrentQueue.h"
#include "LockfreeQueue.h"
#include "LockfreeStack.h"
#include "LockfreeSpscQueue.h"


ProductQueue* lpProductQueue = ProductQueue::GetInstance();
ProductConcurrentQueue* lpProductConcurrentQueue = ProductConcurrentQueue::GetInstance();
ProducerConsumerQueue* lpProducerConsumerQueue = ProducerConsumerQueue::GetInstance();
LockfreeQueue *lpLockfreeQueue = LockfreeQueue::GetInstance();
LockfreeStack *lpLockfreeStack = LockfreeStack::GetInstance();
LockfreeSpscQueue *lpLockfreeSpscQueue = LockfreeSpscQueue::GetInstance();


void ProductQueue_Test()
{
	printf("begin mutex lock one thread test\n");  
	for (int i = 0; i < Times; ++i)
	{
		std::string lTest(Test_String); 
		Product* lpProduct = new Product(lTest.c_str(), lTest.size());  

		lpProductQueue->Put(lpProduct);

		Product* lpNewProduct = lpProductQueue->Take();
		if (lpNewProduct != nullptr)  
		{  
			delete lpNewProduct;  
			lpNewProduct = nullptr;  
		}  
	}
}

void ProducerConsumerQueue_Test()
{
	printf("begin producer_consumer_queue_ex one thread test\n");  
	DWORD start= GetTickCount();  


	for (int i = 0; i < Times; ++i)  
	{  
		std::string lTest(Test_String);  
		Product* lpProduct = new Product(lTest.c_str(), lTest.size());
		lpProducerConsumerQueue->Put(lpProduct);  

		Product* lpNewProduct = lpProducerConsumerQueue->Take();  
		if (lpNewProduct != nullptr)  
		{  
			delete lpNewProduct;  
			lpNewProduct = nullptr;  
		}  
	}
}

void ProductConcurrentQueue_Test()
{
	printf("begin concurrent_queue one thread test\n");  
	DWORD start= GetTickCount();  

	for (int i = 0; i < Times; ++i)  
	{  
		std::string lTest(Test_String);  
		Product* lpProduct = new Product(lTest.c_str(), lTest.size());
		lpProductConcurrentQueue->Put(lpProduct);  

		Product* lpNewProduct = lpProductConcurrentQueue->Take();  
		if (lpNewProduct != nullptr)  
		{  
			delete lpNewProduct;  
			lpNewProduct = nullptr;  
		}  
	}
}

void LockfreeQueue_Test()
{
	printf("begin boost_lockfree_queue one thread test\n");  
	DWORD start= GetTickCount();  

	for (int i = 0; i < Times; ++i)  
	{  
		std::string lTest(Test_String);  
		Product* lpProduct = new Product(lTest.c_str(), lTest.size());
		lpLockfreeQueue->Put(lpProduct);  

		Product* lpNewProduct = lpLockfreeQueue->Take();  
		if (lpNewProduct != nullptr)  
		{  
			delete lpNewProduct;  
			lpNewProduct = nullptr;  
		}  
	}
}

void LockfreeStack_Test()
{
	printf("begin boost_lockfree_stack one thread test\n");  
	DWORD start= GetTickCount();  

	for (int i = 0; i < Times; ++i)  
	{  
		std::string lTest(Test_String);  
		Product* lpProduct = new Product(lTest.c_str(), lTest.size());
		lpLockfreeStack->Put(lpProduct);  

		Product* lpNewProduct = lpLockfreeStack->Take();  
		if (lpNewProduct != nullptr)  
		{  
			delete lpNewProduct;  
			lpNewProduct = nullptr;  
		}  
	}
}

void LockfreeSpscQueue_Test()
{
	printf("begin boost_lockfree_spec_queue one thread test\n");  
	DWORD start= GetTickCount();  

	for (int i = 0; i < Times; ++i)  
	{  
		std::string lTest(Test_String);  
		Product* lpProduct = new Product(lTest.c_str(), lTest.size());
		lpLockfreeSpscQueue->Put(lpProduct);  

		Product* lpNewProduct = lpLockfreeSpscQueue->Take();  
		if (lpNewProduct != nullptr)  
		{  
			delete lpNewProduct;  
			lpNewProduct = nullptr;  
		}  
	}
}
