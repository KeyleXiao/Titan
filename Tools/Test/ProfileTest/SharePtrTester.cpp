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
    // ��ʼ��
    for (size_t i=0; i < sizeof(m_IntArray)/sizeof(PTR_INT); ++i)
    {
        m_IntArray[i] = PTR_INT( new int(i) );
    }

    // ���������߳�
    ThreadManageRef.CeateThread<ShareProducter>(PRODUCT_THREAD_COUNT, this);

    // ���������߳�
    ThreadManageRef.CeateThread<ShareConsumer>(CONSUME_THREAD_COUNT, this);
}

void SharePtrTester::Release(void)
{
    // ֹͣ�߳�
    ThreadManageRef.StopThread();
    ThreadManageRef.TerminateThread();

    // �ͷ�
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