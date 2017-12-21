#include "stdafx.h"
#include "RenderViewTask.h"
#include "ConfigIni.h"
#include "IClientGlobal.h"
#include "IClientEntity.h"

//#define OUTPUT_RENDERVIEW_CALL_STAT

void CRenderViewTask::init(void* pEntry, int nIndex)
{
    m_RenderViewEntry = *(IRenderViewStruct*)pEntry;
    m_nIndex = nIndex;
}

void CRenderViewTask::sendControllerCommand(int nCmdID,int nParam,const char* pszParam, void* pParam, int nLen)
{
    if((nLen == 0 && pParam != 0) || (nLen != 0 && pParam == 0))
    {
        ErrorLn(__FUNCTION__ << ", param error, nCmdID=" << nCmdID);
        return;
    }

    if(pszParam == 0)
    {
        pszParam = "";
    }

    obuf256 t_data;
    t_data << nCmdID << nParam << pszParam << nLen;

    if(nLen > 0)
    {
        t_data.push_back(pParam, nLen);
    }
    
    post_task_message(COMMAND_CONTROLLER, nCmdID, t_data);
}

void CRenderViewTask::loadModel(EntityViewItem& ite)
{
    obuf256 t_data;
    t_data << ite;

    post_task_message(COMMAND_LOAD_MODEL, ENTITY_TOVIEW_LOAD_MODEL, t_data);
}

void CRenderViewTask::createEntityView(EntityViewItem& ite)
{
    obuf256 t_data;
    t_data << ite;

    post_task_message(COMMAND_CREATE_ENTITY_VIEW, ENTITY_TOVIEW_CREATE_ENTITY_VIEW, t_data);
}

void CRenderViewTask::destroyEntityView(int nEntityID)
{
    obuf256 t_data;
    t_data << nEntityID;


    post_task_message(COMMAND_DESTROY_ENTITY_VIEW, ENTITY_TOVIEW_DESTROY_ENTITY_VIEW, t_data);
}

void CRenderViewTask::sendEntityCommand(int nEntityID, int nEntityType, int nCmdID, 
    int nParam, const char* pszParam, void* pParam, int nLen)
{
    if((nLen == 0 && pParam != 0) || (nLen != 0 && pParam == 0))
    {
        ErrorLn(__FUNCTION__ << ", param error, nCmdID=" << nCmdID);
        return;
    }

    if(pszParam == 0)
    {
        pszParam = "";
    }

    obuf256 t_data;
    t_data << nEntityID << nEntityType << nCmdID << nParam << pszParam << nLen;

    if(nLen > 0)
    {
        t_data.push_back(pParam, nLen);
    }

    post_task_message(COMMAND_SEND_ENTITY_COMMAND, nCmdID, t_data);

// √¸¡ÓºÏ≤‚
#ifdef PERFORMANCE_CLIENT_TEST
    if(m_isOpenCmdStat)
    {
        bool bCanAdd = false;
        bool bActor = (TYPE_PLAYER_ROLE == UID_2_TYPE(nEntityID));

        // º‡øÿ◊‘º∫Ω«…´µƒ√¸¡Ó
        if( bActor && (m_nCmdFlag & PERF_CHECK_ENTITY_CMD_SELF) )
        {
            IClientEntity * pHero = gClientGlobal->getHero();
            bCanAdd = (pHero && pHero->getUID() == nEntityID) ;
        }
        else if( bActor && (m_nCmdFlag & PERF_CHECK_ENTITY_CMD_OTHERHERO) )
        {
            bCanAdd = true;
        }
        else if( (m_nCmdFlag & PERF_CHECK_ENTITY_CMD_MONSTER) && (TYPE_MONSTER == UID_2_TYPE(nEntityID)))
        {
            bCanAdd = true;
        }

        // ‘ˆº”√¸¡Ó
        if(bCanAdd)
        {
            m_entityCmdStat.push(nEntityID, nCmdID);
        } 
    }   
#endif
}

void CRenderViewTask::loadPrefab(EntityViewItem& ite)
{
    obuf256 t_data;
    t_data << ite;

    post_task_message(COMMAND_LOAD_PREFAB, ENTITY_TOVIEW_LOAD_PREFAB, t_data);
}

void CRenderViewTask::loadSkin(EntityViewItem& ite)
{
    obuf256 t_data;
    t_data << ite;

    post_task_message(COMMAND_LOAD_SKIN, ENTITY_TOVIEW_LOAD_SKIN, t_data);
}


void CRenderViewTask::print(void)
{
    CTask::print();
}


void CRenderViewTask::doMessage(Message* pMsg)
{
    LARGE_INTEGER liStart;
    LARGE_INTEGER liEnd;
    QueryPerformanceCounter(&liStart);

    if(pMsg == NULL)
    {
        return;
    }
    ibuffer in(pMsg->context.data(), pMsg->context.size());

    if(pMsg->nCmdType == COMMAND_CONTROLLER /*"sendControllerCommand"*/ )
    {
        int nCmdID; int nParam; char* pszParam; void* pParam = NULL; int nLen;
        in >> nCmdID >> nParam >> pszParam >> nLen;
        if(nLen > 0)
        {
            pParam = _alloca(nLen); 
            in.pop(pParam, nLen);
        }

        m_RenderViewEntry.sendControllerCommand_Handler(nCmdID, nParam, pszParam, pParam, nLen, getTickCountEx());
        
        QueryPerformanceCounter(&liEnd);
        ++m_costStat.mapControllerCommandCost[nCmdID].nCallTimes;
        m_costStat.mapControllerCommandCost[nCmdID].fTotalTimeCost += ((float)(liEnd.QuadPart - liStart.QuadPart) * (float)1000 / (float)m_liPerfFreq.QuadPart); 

        if((int)((liEnd.QuadPart - m_costStat.liStart.QuadPart) * 1000 / m_liPerfFreq.QuadPart) > 60 * 1000)
        {
            PrintCostStat();
            m_costStat.Restart();
        }

        return;
    }

    if(pMsg->nCmdType == COMMAND_LOAD_MODEL /*"loadModel"*/ )
    {
        EntityViewItem ite;
        in >> ite;

        m_RenderViewEntry.loadModel_Handler(ite);

        QueryPerformanceCounter(&liEnd);
        ++m_costStat.costLoadModel.nCallTimes;
        m_costStat.costLoadModel.fTotalTimeCost += ((float)(liEnd.QuadPart - liStart.QuadPart) * (float)1000 / (float)m_liPerfFreq.QuadPart); 

        if((int)((liEnd.QuadPart - m_costStat.liStart.QuadPart) * 1000 / m_liPerfFreq.QuadPart) > 60 * 1000)
        {
            PrintCostStat();
            m_costStat.Restart();
        }

        return;
    }

    if(pMsg->nCmdType == COMMAND_CREATE_ENTITY_VIEW /*"createEntityView"*/ )
    {
        EntityViewItem ite;
        in >> ite;

        m_RenderViewEntry.createEntityView_Handler(ite);

        QueryPerformanceCounter(&liEnd);
        ++m_costStat.costCreateEntityView.nCallTimes;
        m_costStat.costCreateEntityView.fTotalTimeCost += ((float)(liEnd.QuadPart - liStart.QuadPart) * (float)1000 / (float)m_liPerfFreq.QuadPart);

        if((int)((liEnd.QuadPart - m_costStat.liStart.QuadPart) * 1000 / m_liPerfFreq.QuadPart) > 60 * 1000)
        {
            PrintCostStat();
            m_costStat.Restart();
        }

        return;
    }

    if(pMsg->nCmdType == COMMAND_DESTROY_ENTITY_VIEW /*"destroyEntityView"*/ )
    {
        int nEntityID;
        in >> nEntityID;

        m_RenderViewEntry.destroyEntityView_Handler(nEntityID);

        QueryPerformanceCounter(&liEnd);
        ++m_costStat.costDestroyEntityView.nCallTimes;
        m_costStat.costDestroyEntityView.fTotalTimeCost += ((float)(liEnd.QuadPart - liStart.QuadPart) * (float)1000 / (float)m_liPerfFreq.QuadPart);

        if((int)((liEnd.QuadPart - m_costStat.liStart.QuadPart) * 1000 / m_liPerfFreq.QuadPart) > 60 * 1000)
        {
            PrintCostStat();
            m_costStat.Restart();
        }

        return;
    }

    if(pMsg->nCmdType == COMMAND_SEND_ENTITY_COMMAND /*"sendEntityCommand"*/ )
    {
        int nEntityID; int nEntityType; int nCmdID; 
        int nParam; const char* pszParam; void* pParam = NULL; int nLen;
        in >> nEntityID >> nEntityType >> nCmdID >> nParam >> pszParam >> nLen;

        if(nLen > 0)
        {
            pParam = _alloca(nLen);
            in.pop(pParam, nLen);
        }

        m_RenderViewEntry.sendEntityCommand_Handler(nEntityID, nCmdID, nParam, (void*)pszParam, pParam, nLen, getTickCountEx());

        QueryPerformanceCounter(&liEnd);
        ++m_costStat.mapEntityCommandCost[nCmdID].nCallTimes;
        m_costStat.mapEntityCommandCost[nCmdID].fTotalTimeCost += ((float)(liEnd.QuadPart - liStart.QuadPart) * (float)1000 / (float)m_liPerfFreq.QuadPart);

        if((int)((liEnd.QuadPart - m_costStat.liStart.QuadPart) * 1000 / m_liPerfFreq.QuadPart) > 60 * 1000)
        {
            PrintCostStat();
            m_costStat.Restart();
        }

        return;
    }

    if(pMsg->nCmdType == COMMAND_LOAD_PREFAB /*"loadPrefab"*/ )
    {
        EntityViewItem ite;
        in >> ite;
        m_RenderViewEntry.loadPrefab_Handler(ite);

        QueryPerformanceCounter(&liEnd);
        ++m_costStat.costLoadPrefab.nCallTimes;
        m_costStat.costLoadPrefab.fTotalTimeCost += ((float)(liEnd.QuadPart - liStart.QuadPart) * (float)1000 / (float)m_liPerfFreq.QuadPart);

        if((int)((liEnd.QuadPart - m_costStat.liStart.QuadPart) * 1000 / m_liPerfFreq.QuadPart) > 60 * 1000)
        {
            PrintCostStat();
            m_costStat.Restart();
        }

        return;
    }

    if(pMsg->nCmdType == COMMAND_LOAD_SKIN /*"loadSkin"*/ )
    {
        EntityViewItem ite;
        in >> ite;
        m_RenderViewEntry.loadSkin_Handler(ite);

        QueryPerformanceCounter(&liEnd);
        ++m_costStat.costLoadSkin.nCallTimes;
        m_costStat.costLoadSkin.fTotalTimeCost += ((float)(liEnd.QuadPart - liStart.QuadPart) * (float)1000 / (float)m_liPerfFreq.QuadPart);

        if((int)((liEnd.QuadPart - m_costStat.liStart.QuadPart) * 1000 / m_liPerfFreq.QuadPart) > 60 * 1000)
        {
            PrintCostStat();
            m_costStat.Restart();
        }

        return;
    }

    ErrorLn(__FUNCTION__ << "cmd=" << pMsg->nCmdType << ", unhandled");
}

void CRenderViewTask::onStart()
{
#ifdef PERFORMANCE_CLIENT_TEST
    m_entityCmdStat.create("[cmd]entityid", ConfigIni::getInstance().isAccumulate(), ConfigIni::getInstance().getCmdInterVal(), ConfigIni::getInstance().getCmdMinVal());
    m_nCmdFlag = ConfigIni::getInstance().getCmdFlag();
    m_isOpenCmdStat = ConfigIni::getInstance().isOpenCmdStat();
#endif
}

void CRenderViewTask::onStop()
{
    clear();
}

void CRenderViewTask::PrintCostStat()
{
#ifdef OUTPUT_RENDERVIEW_CALL_STAT
    WarningLn("@@@@@@@@@@@@@@@@@@last min,index=" << m_nIndex << "@@@@@@@@@@@@@@@@@@@@@");

    for(std::map<int, SCost>::iterator it = m_costStat.mapControllerCommandCost.begin();
        it != m_costStat.mapControllerCommandCost.end(); ++it)
    {
        WarningLn("sendControllerCommand, cmd=" << it->first << " | call times=" << it->second.nCallTimes << ", total cost(ms)=" << it->second.fTotalTimeCost);
    }

    WarningLn("loadModel | call times=" << m_costStat.costLoadModel.nCallTimes << ", total cost(ms)=" << m_costStat.costLoadModel.fTotalTimeCost);

    WarningLn("createEntityView | call times=" << m_costStat.costCreateEntityView.nCallTimes << ", total cost(ms)=" << m_costStat.costCreateEntityView.fTotalTimeCost);

    WarningLn("destroyEntityView | call times=" << m_costStat.costDestroyEntityView.nCallTimes << ", total cost(ms)=" << m_costStat.costDestroyEntityView.fTotalTimeCost);

    for(std::map<int, SCost>::iterator it = m_costStat.mapEntityCommandCost.begin();
        it != m_costStat.mapEntityCommandCost.end(); ++it)
    {
        WarningLn("sendEntityCommand, cmd=" << it->first << " | call times=" << it->second.nCallTimes << ", total cost(ms)=" << it->second.fTotalTimeCost);
    }

    WarningLn("loadPrefab | call times=" << m_costStat.costLoadPrefab.nCallTimes << ", total cost(ms)=" << m_costStat.costLoadPrefab.fTotalTimeCost);

    WarningLn("loadSkin | call times=" << m_costStat.costLoadSkin.nCallTimes << ", total cost(ms)=" << m_costStat.costLoadSkin.fTotalTimeCost);

    WarningLn("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
#endif // OUTPUT_RENDERVIEW_CALL_STAT
    // OUTPUT_RENDERVIEW_CALL_STAT
}