#pragma once
#include "share_ptr.h"
#include "ThreadInterface.h"
#include <time.h>

typedef share_ptr<int> PTR_INT;



//////////////////////////////////////////////////////////////////////////
struct IDataSource 
{
 virtual void DoRun(bool isProduct=true) = 0;   
};

// 生产者
class ShareProducter : public IProducter
{
public:
    ShareProducter(IDataSource* obj)
        : m_obj(obj)
    {
        m_time = (DWORD)time(NULL);
    }

    virtual void run()
    {
        while(true)
        {
            DWORD dwCurrentTime = (DWORD) time(NULL);
            if ( dwCurrentTime > m_time + THREAD_RUN_EXPIRED )
            {
                break;
            }

            m_obj->DoRun(true);
        }
    }

    virtual void release()
    {

    }

private:
    IDataSource*  m_obj;
    DWORD           m_time;
};

//////////////////////////////////////////////////////////////////////////
// 消费者
class ShareConsumer : public IConsumer
{
public:
    ShareConsumer(IDataSource* obj)
        : m_obj(obj)
    {
        m_time = (DWORD)time(NULL);
    }

    virtual void run()
    {
        while(true)
        {
            DWORD dwCurrentTime = (DWORD) time(NULL);
            if ( dwCurrentTime > m_time + THREAD_RUN_EXPIRED )
            {
                break;
            }

            m_obj->DoRun(false);
        }
    }

    virtual void release()
    {

    }

private:
    IDataSource*    m_obj;
    DWORD           m_time;
};

//////////////////////////////////////////////////////////////////////////
// share_ptr测试
class SharePtrTester : public IDataSource
{
public:
    SharePtrTester(void);
    ~SharePtrTester(void);

    void Create(void);
    void Release(void);

    // 开启
    void Start(void);

    virtual void DoRun(bool isProduct=true);


private:
    PTR_INT     m_IntArray[TEST_BUFF_SIZE];

    volatile LONG  m_nProduct;
    volatile LONG  m_nConsume;
};

