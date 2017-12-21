#include "stdafx.h"
#include "TestConcur.hpp"
#include <time.h>

#define NUM_DEF 150

struct timezone
{
	__int32  tz_minuteswest; /* minutes W of Greenwich */
	bool  tz_dsttime;     /* type of dst correction */
};

/* FILETIME of Jan 1 1970 00:00:00. */
static const unsigned __int64 epoch = ((unsigned __int64) 116444736000000000ULL);
/*
 * timezone information is stored outside the kernel so tzp isn't used anymore.
 *
 * Note: this function is not for Win32 high precision timing purpose. See
 * elapsed_time().
 */
int	gettimeofday(struct timeval * tp, struct timezone * tzp)
{
    FILETIME    file_time;
    SYSTEMTIME  system_time;
    ULARGE_INTEGER ularge;

    GetSystemTime(&system_time);
    SystemTimeToFileTime(&system_time, &file_time);
    ularge.LowPart = file_time.dwLowDateTime;
    ularge.HighPart = file_time.dwHighDateTime;

    tp->tv_sec = (long) ((ularge.QuadPart - epoch) / 10000000L);
    tp->tv_usec = (long) (system_time.wMilliseconds * 1000);

    return 0;
}

CTestConcur::CTestConcur()
{
}

bool CTestConcur::StartTest(const std::string &strHost)
{
    if (!m_redis.Initialize(strHost, 6379, 2, 100))
    {
        std::cout << "Connect to redis failed" << std::endl;
        return false;
    }

    if (!InitStringEnv(100, 10))
    {
        std::cout << "Initialize environment failed" << std::endl;
        return false;
    }

    m_bExit = false;
    const int nGetTrdNum = 20;
    const int nSetTrdNum = 20;
    //std::thread *pthreadGet[nGetTrdNum] = {nullptr};
    //std::thread *pthreadSet[nSetTrdNum] = {nullptr};
	//for (int i = 0; i < nGetTrdNum; ++i)
	//	pthreadGet[i] = new std::thread(std::bind(&CTestConcur::Test_Get, this));
	//for (int i = 0; i < nSetTrdNum; ++i)
	//	pthreadSet[i] = new std::thread(std::bind(&CTestConcur::Test_Set, this));
	rkt::Thread  *pthreadGet[nGetTrdNum] = {nullptr};
	rkt::Thread  *pthreadSet[nGetTrdNum] = {nullptr};
	for (int i = 0; i < nGetTrdNum; ++i)
	{
		pthreadGet[i] = new rkt::Thread ();
		CTestConcurRunGet cTestConcurRunGet(this);
		pthreadGet[i]->spawn(&cTestConcurRunGet);
	}
	for (int i = 0; i < nSetTrdNum; ++i)
	{
		pthreadSet[i] = new rkt::Thread ();
		CTestConcurRunSet cTestConcurRunSet(this);
		pthreadSet[i]->spawn(&cTestConcurRunSet);
	}

    for (int i = 0; i < nGetTrdNum; ++i)
    {
        if (pthreadGet[i])
            pthreadGet[i]->wait();
    }
    for (int i = 0; i < nSetTrdNum; ++i)
    {
        if (pthreadSet[i])
            pthreadSet[i]->wait();
    }

    std::cout << "TestFinish" << std::endl;
    return true;
}

void CTestConcur::Test_GetS()
{
    std::string strVal;
    while (!m_bExit)
    {
        int nRet = m_redis.Get("tk_str_1", &strVal);
        if (nRet != RC_SUCCESS)
        {
            if (nRet == RC_NO_RESOURCE)
                std::cout << "No resource: " << time(nullptr) << std::endl;
            else
                std::cout << "Get Failed: " << time(nullptr) << std::endl;
        }
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));
		rkt::Thread::sleep(10);
    }
}

void CTestConcur::Test_Get()
{
    struct timeval tv;
    struct timezone tz;
    int nIndex = 1;
    int npc = 0;
    while (!m_bExit)
    {
        std::string strVal;
        std::stringstream ss;
        ss << "tk_str_" << nIndex++;
        int nRet = m_redis.Get(ss.str(), &strVal);
        if (nRet == RC_SUCCESS)
        {
            m_mutex.Lock();
            if (++npc > NUM_DEF)
            {
                std::cout << "Get OK: " << strVal << std::endl;
                npc = 0;
            }
            m_mutex.Unlock();
        }
        else
        {
            gettimeofday(&tv, &tz);
            m_mutex.Lock();
            if (++npc > NUM_DEF)
            {
                if (nRet == RC_NO_RESOURCE)
                    std::cout << "No resource: " << tv.tv_usec << std::endl;
                else
                    std::cout << "Get Failed: " << tv.tv_usec << std::endl;
                npc = 0;
            }
            m_mutex.Unlock();
            //m_bExit = true;
        }
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));
		rkt::Thread::sleep(10);
        if (nIndex > 10)
            nIndex = 1;
    }
}

void CTestConcur::Test_Set()
{
    int nCounter = 2;
    int nIndex = 1;
    int npc = 0;
    struct timeval tv;
    struct timezone tz;
    while (!m_bExit)
    {
        std::stringstream ssKey, ssVal;
        ssKey << "tk_str_" << nIndex++;
        ssVal << "value_" << nCounter++;
        int nRet = m_redis.Set(ssKey.str(), ssVal.str());
        if (nRet == RC_SUCCESS)
        {
            m_mutex.Lock();
            if (++npc > NUM_DEF)
            {
                std::cout << "Set OK" << std::endl;
                npc = 0;
            }
            m_mutex.Unlock();
        }
        else
        {
            gettimeofday(&tv, &tz);
            m_mutex.Lock();
            if (++npc > NUM_DEF)
            {
                if (nRet == RC_NO_RESOURCE)
                    std::cout << "No resource: " << tv.tv_usec << std::endl;
                else
                    std::cout << "Get Failed: " << tv.tv_usec << std::endl;
                npc = 0;
            }
            m_mutex.Unlock();
            //m_bExit = true;
        }
        //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		rkt::Thread::sleep(10);
        if (nIndex > 10)
            nIndex = 1;
    }
}

void CTestConcurRunGet::run()
{
	m_pTestConcur->Test_Get();
}
void CTestConcurRunGet::release()
{

}

void CTestConcurRunSet::run()
{
	m_pTestConcur->Test_Set();
}
void CTestConcurRunSet::release()
{

}