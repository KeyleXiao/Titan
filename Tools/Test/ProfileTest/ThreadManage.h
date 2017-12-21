#pragma once

#include <vector>
#include "Thread.h"
#include "Singleton.h"

struct IDataSource;

// 线程管理器
class ThreadManage : public Singleton<ThreadManage>
{
public:
	ThreadManage(void);
	~ThreadManage(void);

	rkt::ThreadPool* getThreadPool();
	void StartThread();
	void TerminateThread();

public:
    template <class _CreateClass> 
    void CeateThread(int nThreadNum, IDataSource* pDataSource)
    {
        for (int i=0; i<nThreadNum; ++i)
        {
            _CreateClass * pObj = new _CreateClass(pDataSource);
            if(pObj)
            {
                m_ThreadVector.push_back(pObj);
                		
                getThreadPool()->add(pObj);
            }
        }
    }

    void StopThread()
    {
        for (IRUNNABLEVECTOR::iterator iter = m_ThreadVector.begin(); iter != m_ThreadVector.end(); ++iter)
        { 
            ::rkt::IRunnable* p = (*iter);
            if (p)
            {
                p->release();
            }
        }
        m_ThreadVector.clear();
    }
	
private:
	rkt::ThreadPool m_threadPool;

	typedef std::vector<::rkt::IRunnable*>	IRUNNABLEVECTOR;
	IRUNNABLEVECTOR m_ThreadVector;
};
