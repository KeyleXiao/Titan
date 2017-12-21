#include "StdAfx.h"
#include <iostream>
#include "SharePtrTester.h"
#include "ThreadManage.h"
using namespace std;

#define ThreadManageRef ThreadManage::getInstance()

SharePtrTester::SharePtrTester(void)
    : m_nProduct(0)
    , m_nConsume(0)
{

}


SharePtrTester::~SharePtrTester(void)
{

}

void SharePtrTester::Create(void)
{
    // 初始化
    for (size_t i=0; i < sizeof(m_IntArray)/sizeof(PTR_INT); ++i)
    {
        m_IntArray[i] = PTR_INT( new int(i) );
    }

    // 创建生产线程
    ThreadManageRef.CeateThread<ShareProducter>(PRODUCT_THREAD_COUNT, this);

    // 创建消费线程
    ThreadManageRef.CeateThread<ShareConsumer>(CONSUME_THREAD_COUNT, this);
}

void SharePtrTester::Release(void)
{
    // 停止线程
    ThreadManageRef.StopThread();
    ThreadManageRef.TerminateThread();

    // 释放
    for (size_t i=0; i < sizeof(m_IntArray)/sizeof(PTR_INT); ++i)
    {
        m_IntArray[i] = PTR_INT( 0 );
    }
}


void SharePtrTester::Start(void)
{
    ThreadManageRef.StartThread();
}


void SharePtrTester::DoRun(bool isProduct)
{
    int index = rkt_range_rand(0, (TEST_BUFF_SIZE-1), nullptr);
    if(index <0 || index >= TEST_BUFF_SIZE)
        index = 0;

    DWORD nThreadID = ::GetCurrentThreadId();

    if(isProduct)
    {
        if(index == 0)
        {
            m_IntArray[index] = PTR_INT(0);
        }
        else
        {
            m_IntArray[index] = PTR_INT(new int(index));
        }        

        ::InterlockedIncrement(&m_nProduct);
    }
    else
    {
        PTR_INT ptr = m_IntArray[index];

        ::InterlockedIncrement(&m_nConsume);
    }

    cout <<"ThreadID="<< nThreadID <<", nProduct ="<< m_nProduct <<", nConsum="<<m_nConsume <<", index="<< index << endl;

}