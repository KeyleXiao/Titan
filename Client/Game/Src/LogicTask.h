#pragma once
#include "Task.h"
#include "TimerAxis.h"

// ”≈ªØ
//#define LOGIC_TASK_OPTIMIZE

#ifdef LOGIC_TASK_OPTIMIZE
#    include "NetController.h"
#    include "TimerController.h"
#endif

class CLogicTask : public CTask, public Singleton<CLogicTask>
{
public:
    void OnEntityEvent(int nEntityID, int nEventID,int nParam, const char* strParam,void* pParam, int nLen);
    void OnGameViewEvent(int nEventID,int nParam,const char* strParam,void* ptrParam, int nLen);
    void traceLog(const char* msg,int32 level);
    
    TimerAxis* getTimerAxis();

    DWORD getNetworkDispatchInterval() {
        return m_dwNetDispatchInterval ;
    }
    DWORD getTimerCheckInterval() { 
        return  m_dwTimerCheckInterval;
    }

    virtual void print(void);
	
#ifdef LOGIC_TASK_OPTIMIZE
    CLogicTask() : NetController(CNetController(NetworkUserType_Default))
#else
    CLogicTask()
#endif
    {
        m_dwNetDispatchInterval = 0;
        m_dwTimerCheckInterval = 0;
    }

protected:
    virtual void onPrepareUpdate();
    virtual void onAfterUpdate();
    virtual void onStart();
    virtual void onStop();
    virtual void doMessage(Message* pMsg);
    virtual int  mytype() { return TASK_LOGIC; }
 
private:

#ifdef LOGIC_TASK_OPTIMIZE
    CNetController     NetController;
    CTimerController   TimerController;
#else
    TimerAxis           m_timerAxis;
    LARGE_INTEGER       m_liPerfCheckTimer;
#endif
    DWORD               m_dwNetDispatchInterval;
    DWORD               m_dwTimerCheckInterval;
};