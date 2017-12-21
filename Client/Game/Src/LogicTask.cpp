#include "Stdafx.h"
#include "LogicTask.h"
#include "ClientGlobal.h"

void CLogicTask::OnEntityEvent(int nEntityID, int nEventID,int nParam, const char* strParam,void* pParam, int nLen)
{
    if(strParam == 0)
    {
        strParam = "";
    }

    obuf256 t_data;
    t_data << nEntityID<<nEventID<<nParam<<strParam<<nLen;
    if(nLen > 0)
    {
        t_data.push_back(pParam, nLen);
    }

    if (nEventID == ENTITY_CMD_CAST_SPELL)
    {
        post_task_vip_message(COMMAND_ENTITYEVENT, nEventID, t_data);
    }
    else
    {
        post_task_message(COMMAND_ENTITYEVENT, nEventID, t_data);
    }
}

void CLogicTask::OnGameViewEvent(int nEventID,int nParam,const char* strParam,void* ptrParam, int nLen)
{
    if(strParam == 0)
    {
        strParam = "";
    }

    obuf256 t_data;
    t_data << nEventID<<nParam<<strParam<<nLen;
    if(nLen > 0)
    {
        t_data.push_back(ptrParam, nLen);
    }

    post_task_message(COMMAND_VIEWEVENT, nEventID, t_data);
}

void CLogicTask::traceLog(const char* msg,int32 level)
{
    if(msg == NULL)
    {
        return;
    }

    obuf256 t_data;
    t_data << msg << level;

    post_task_message(COMMAND_TRACE, ENTITY_CMD_TRACE, t_data);
}

void CLogicTask::onStart()
{
    int ret = InitializeNetwork();
    if ( ret!=1 )
    {
        ErrorLn("InitializeNetwork failed! error="<<ret << " last error:" << getLastError());
        return;
    }

    // 创建失败
    if(!CClientGlobal::getInstance().create())
    {
        return;
    }
    gClientGlobal->getStageManager()->gotoState(ESS_LOGIN);

#ifndef LOGIC_TASK_OPTIMIZE
    QueryPerformanceCounter(&m_liPerfCheckTimer);
#endif
}

void CLogicTask::onStop()
{
    clear();

    CClientGlobal::getInstance().destroy();
    UninitializeNetwork();
    releaseInstance();
}

void CLogicTask::onPrepareUpdate()
{
    // 直接触发执行命令队列
    IHandleCommandManager* pHandleCommandManager = gClientGlobal->getHandleCommandManager();
    if (pHandleCommandManager)
    {
        pHandleCommandManager->update();
    }
}

void CLogicTask::onAfterUpdate()
{
    // 优先收发网络包
#ifdef LOGIC_TASK_OPTIMIZE
    NetController.update();
    m_dwNetDispatchInterval = NetController.getRunTick();

    TimerController.update();
    m_dwTimerCheckInterval = TimerController.getRunTick();
#else
    DWORD dwCurrentTick = getTickCountEx();

    DispatchNetworkEvents(100, NetworkUserType_Default);

    m_dwNetDispatchInterval = getTickCountEx() - dwCurrentTick;

    // 每隔16ms check一次timer
    //LARGE_INTEGER liPerfNow;
    //QueryPerformanceCounter(&liPerfNow); 
    //if((liPerfNow.QuadPart - m_liPerfCheckTimer.QuadPart) * 1000 / m_liPerfFreq.QuadPart > 16)
    //{
        dwCurrentTick = getTickCountEx();;
        m_timerAxis.CheckTimer();
        m_dwTimerCheckInterval = getTickCountEx() - dwCurrentTick;
    //    m_liPerfCheckTimer = liPerfNow; 
    //}
#endif
}

TimerAxis* CLogicTask::getTimerAxis()
{
#ifdef LOGIC_TASK_OPTIMIZE
    return TimerController.getTimerAxis();
#else
    return &m_timerAxis;
#endif
}

void CLogicTask::doMessage(Message* pMsg)
{
    if(pMsg == NULL)
    {
        return;
    }
    ibuffer in(pMsg->context.data(), pMsg->context.size());

    if(pMsg->nCmdType == COMMAND_ENTITYEVENT /*"OnEntityEvent"*/ )
    {
        int nEntityID;int nEventID;int nParam;char* strParam = 0;void* pParam = 0; int len = 0;
        in >> nEntityID>>nEventID>>nParam>>strParam>>len;

        if(len > 0)
        {
            pParam = _alloca(len);
            in.pop(pParam, len);
        }
        else
        {
            pParam = NULL;
        }

        IEntityClient *pEntityClient = gClientGlobal->getEntityClient();
        if(!pEntityClient)
        {
            return;
        }

        pEntityClient->onEntityEvent(nEntityID, nEventID, nParam,strParam,pParam);
        return;
    }

    if(pMsg->nCmdType == COMMAND_VIEWEVENT /*"OnGameViewEvent"*/ )
    {
        int nEventID;int nParam;char* strParam = 0;void* ptrParam = 0; int len = 0;
        in >> nEventID>>nParam>>strParam>>len;

        if(len > 0)
        {
            ptrParam = _alloca(len);
            in.pop(ptrParam, len);
        }
        else
        {
            ptrParam = NULL;
        }

        IEntityClient *pEntityClient = gClientGlobal->getEntityClient();
        if(!pEntityClient)
        {
            return;
        }
        pEntityClient->onGameViewEvent(nEventID, nParam, strParam, ptrParam);
        return;
    }

    if(pMsg->nCmdType == COMMAND_TRACE /*"traceLog"*/ )
    {
        const char* msg; int32 level;
        in >> msg >> level;

        if(msg == NULL)
        {
            return;
        }
        switch(level)
        {
        case levelInfo:
            InfoLn(msg);
            break;
        case levelTrace:
            TraceLn(msg);
            break;
        case levelWarning:
            WarningLn(msg);
            break;
        case levelError:
            ErrorLn(msg);
            break;
        case levelEmphasis:
            EmphasisLn(msg);
            break;
        default:
            InfoLn(msg);
            break;
        }
        
        return;
    }

    ErrorLn(__FUNCTION__ << "cmd=" << pMsg->nCmdType << ", unhandled");
}

void CLogicTask::print(void)
{
    CTask::print();

    WarningLn("net dispatch " << m_dwNetDispatchInterval << "ms, timer check " << m_dwTimerCheckInterval << "ms");
#ifdef LOGIC_TASK_OPTIMIZE
    WarningLn("net[ timeout="<< NetController.getTimeout() <<"],timer[ timeout="<< TimerController.getTimeout() << "]");
#endif
}