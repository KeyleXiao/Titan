#pragma once
#include "Thread.h"
#include "Simple_Atom_Lock.h"
#include "buffer.h"
#include <list>
#include <string>
#include <map>
#include "TaskCommand.h"
#include "SwappingList.h"
#include "TaskCmdMonitor.h"
#include "EntityViewDef.h"

using namespace rkt;
using namespace GameLogic;

#define TIMESLICE_LIMITLESS (1024 * 1024)

// TASK类型
enum TASK_TYPE
{
    TASK_BASE,
    TASK_LOGIC,
    TASK_VIEW,
};

class CTask : public IRunnable
{
protected:
    typedef task_command Message;
public:

    CTask();

    virtual void autoRun(ThreadPool* pPool);
    // nTimeSlice:每帧最大处理时间 毫秒
    virtual void update();
    virtual void stop();
    virtual void release(){};
    virtual void clear();
    
    void setTimeSlice(int nSlice = TIMESLICE_LIMITLESS);

    DWORD   getMessageCount() {
        return m_messageList.Count(); 
    }

    DWORD   getVipMessageCount() {
        return m_vipMessageList.Count();
    }
    DWORD   getLastRunInterval() { return m_dwLastRunInterval; }

protected:
    void run();
    void postMessage(int nCmdType, int nCmd, const char* data, size_t len, bool isVip=false);

    virtual void onStart() = 0;
    virtual void doMessage(Message* pMsg) = 0;
    virtual void onStop() = 0;
    virtual void onPrepareUpdate(){}
    virtual void onAfterUpdate(){}
    virtual void print(void);
    virtual int  mytype() { return TASK_BASE; }

    char*   getTypeName()
    {
        int t = mytype();
        if(t== TASK_BASE) return "TASK_BASE";
        if(t== TASK_LOGIC) return "TASK_LOGIC";
        if(t== TASK_VIEW) return "TASK_VIEW";
        return "";
    }

    bool m_bAutoRun;
    LARGE_INTEGER m_liPerfFreq;

protected:

    // 技能专门通道
    SwappingList<Message *>    m_vipMessageList;
	SwappingList<Message *>    m_messageList;



    bool m_bStart;
    bool m_bStop;
    bool m_bOnStopCalled;
    int  m_nTimeSlice;

    DWORD   m_dwLastUpdateRunTick;
    DWORD   m_dwLastRunInterval;

#ifdef MOUDLE_TASK_DEBUG
    // 命令统计
    CTaskCmdMonitor<ENTITY_TOVIEW_BEGIN, (ENTITY_CMD_MAX - ENTITY_TOVIEW_BEGIN) >  m_cmdMonitor;
#endif
};