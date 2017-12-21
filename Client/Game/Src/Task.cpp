#include "Task.h"
#include "stdafx.h"

CTask::CTask():m_bAutoRun(false),m_bStart(false),m_bStop(false),m_bOnStopCalled(false),m_nTimeSlice(TIMESLICE_LIMITLESS)
{
    QueryPerformanceFrequency(&m_liPerfFreq);
    m_dwLastUpdateRunTick = getTickCountEx();
    m_dwLastRunInterval = 0;
}

void CTask::autoRun(ThreadPool* pPool)
{
    if(pPool == NULL)
    {
        return;
    }

    m_bAutoRun = true;

    pPool->add(this);
}

void CTask::postMessage(int nCmdType, int nCmd, const char* data, size_t len, bool isVip/* = false*/)
{
    if(m_bStop) return;

    Message* pMsg = new Message;
    if(pMsg == NULL)
    {
        ErrorLn(__FUNCTION__ << ", memory allocate failed");
        return;
    }
    pMsg->nCmdType = nCmdType;
    pMsg->nCmdID = nCmd;
    pMsg->context.push_back(data, len);

#ifdef MOUDLE_TASK_DEBUG
    // 统计命令
    if(nCmdType != COMMAND_CONTROLLER)
    {
        m_cmdMonitor.count(pMsg->nCmdID);
    }
#endif

    if (isVip)
    {
        m_vipMessageList.Add(pMsg);
    }
    else
    {
        m_messageList.Add(pMsg);
    }

}

void CTask::print(void)
{
#ifdef MOUDLE_TASK_DEBUG
    m_cmdMonitor.print(50);
#endif
}

void CTask::run()
{
    LARGE_INTEGER liPerfStart;
    LARGE_INTEGER liPerfEnd;

    // onStart(); // 为了统一 这两个回调都放update里。这样不自动运行的也能触发onstart onstop
    do
    {
        QueryPerformanceCounter(&liPerfStart);

        update();

        QueryPerformanceCounter(&liPerfEnd);

        int nCost = (int)((liPerfEnd.QuadPart - liPerfStart.QuadPart) * 1000 / m_liPerfFreq.QuadPart); 
        if(nCost < 5)
        {
            Sleep(5 - nCost); // 逻辑层还要检测timer和派发网络事件，5ms间隔检测一次。
        }

    }while(!m_bStop);

    update();
    // onStop();
}

void CTask::update()
{
    DWORD dwThreadStartTick = getTickCountEx();
    if(m_bStop)
    {
        if(!m_bOnStopCalled)
        {
            onStop();
            m_bOnStopCalled = true;
        }
        return;
    }

    if(!m_bStart)
    {
        onStart();
        m_bStart = true;
    }

    ///////////////////////////////////////////////////////////////////////// 

    DWORD dwCurrentTick = getTickCountEx();
    //if (dwCurrentTick > m_dwLastUpdateRunTick + 100)
    //{
    //    ErrorLn(__FUNCTION__":" << getTypeName() << " call this function again wait for " << (dwCurrentTick - m_dwLastUpdateRunTick) << "ms");
    //}
    //else if (dwCurrentTick > m_dwLastUpdateRunTick + 30)
    //{
    //    WarningLn(__FUNCTION__":" << getTypeName() << " call this function again wait for " << (dwCurrentTick - m_dwLastUpdateRunTick) << "ms");
    //}
    m_dwLastUpdateRunTick = dwCurrentTick;


    /////////////////////////////////////////////////////////////////////////
    onPrepareUpdate();

    /////////////////////////////////////////////////////////////////////////    
    LARGE_INTEGER liPerfStart;
    LARGE_INTEGER liPerfEnd;
    if (m_nTimeSlice > 0 && m_nTimeSlice < TIMESLICE_LIMITLESS)
    {
        QueryPerformanceCounter(&liPerfStart);
    }


    Message * pMsg = 0;

    // 先处理VIP消息,VIP消息执行不打断
    int vip_msg_count = m_vipMessageList.Count();
    int i = 0;
    while (i < vip_msg_count && m_vipMessageList.Get(pMsg))
    {
        DWORD dwStartTick = getTickCountEx();

        doMessage(pMsg);

        DWORD dwDiffTick = getTickCountEx() - dwStartTick;
        if (dwDiffTick > 50)
        {
            ErrorLn(__FUNCTION__": " << getTypeName() << " run command=" << pMsg->nCmdID << " cost " << dwDiffTick << " ms");
        }
        else if (dwDiffTick > 20)
        {
            WarningLn(__FUNCTION__": " << getTypeName() << " run command=" << pMsg->nCmdID << " cost " << dwDiffTick << " ms");
        }

        delete pMsg;

        ++i;
    }

    // 处理普通级别的消息
    int msg_count = m_messageList.Count();
    while (m_messageList.Get(pMsg))
    {
        DWORD dwStartTick = getTickCountEx();

        doMessage(pMsg);

        DWORD dwDiffTick = getTickCountEx() - dwStartTick;
        if (dwDiffTick > 50)
        {
            ErrorLn(__FUNCTION__": " << getTypeName() << " run command=" << pMsg->nCmdID << " cost " << dwDiffTick << " ms");
        }
        else if (dwDiffTick > 20)
        {
            WarningLn(__FUNCTION__": " << getTypeName() << " run command=" << pMsg->nCmdID << " cost " << dwDiffTick << " ms");
        }

        delete pMsg;

        if (m_nTimeSlice > 0 && m_nTimeSlice < TIMESLICE_LIMITLESS)
        {
            QueryPerformanceCounter(&liPerfEnd);
            if ((int)((liPerfEnd.QuadPart - liPerfStart.QuadPart) * 1000 / m_liPerfFreq.QuadPart) > m_nTimeSlice)
            {
                break;
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////
    onAfterUpdate();

    ////////////////////////////////////////////////////////////////////////
    m_dwLastRunInterval = getTickCountEx() - dwThreadStartTick;

    //if (m_dwLastRunInterval > 100)
    //{
    //    ErrorLn(__FUNCTION__":  " << getTypeName() << " run count=" << (vip_msg_count + msg_count) << ",vip="<< vip_msg_count <<", normall="<< msg_count <<", cost " << m_dwLastRunInterval << " ms");
    //    print();
    //}
    //else if (m_dwLastRunInterval >= 40)
    //{
    //    WarningLn(__FUNCTION__": " << getTypeName() << "  run count=" << (vip_msg_count + msg_count) << ",vip=" << vip_msg_count << ", normall=" << msg_count << ", cost " << m_dwLastRunInterval << " ms");
    //    print();
    //}

}


void CTask::stop()
{
    m_bStop = true;
}

void CTask::clear()
{
    Message * pMsg = 0;
    while (m_vipMessageList.Get(pMsg))
    {
        delete pMsg;
    }

    while( m_messageList.Get(pMsg))
    {
        delete pMsg;
    }
}

void CTask::setTimeSlice(int nSlice /*= TIMESLICE_LIMITLESS*/)
{
    if(nSlice < 1) {
        nSlice = TIMESLICE_LIMITLESS;
    }

    m_nTimeSlice = nSlice;
}
