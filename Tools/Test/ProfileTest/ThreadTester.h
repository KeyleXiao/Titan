#pragma once

#include "Thread.h"

// œﬂ≥Ã≤‚ ‘’ﬂ
class ThreadTester : public ::rkt::IRunnable
{
    bool         m_isRead;

public:
    ThreadTester(int nIndex)
    {
        m_isRead = (nIndex % 2 == 0);
    }

    /////////// IRunnable ///////////////////////////////////////////////////////////////
    virtual void release()
    {

    }


    virtual void run()
    {
        m_isRead ? onRead() : onWrite();
    }

private:
    void onRead()
    {

    }

    void onWrite()
    {

    }
};