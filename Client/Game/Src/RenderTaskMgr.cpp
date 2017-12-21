#include "stdafx.h"
#include "RenderTaskMgr.h"
#include "RenderViewTask.h"
#include "IClientGlobal.h"
#include "IClientEntity.h"

CRenderTaskMgr::CRenderTaskMgr()
{
    memset(m_arrayRenderTask, 0, RENDER_TASK_MAX * sizeof(CRenderViewTask*));
}

void CRenderTaskMgr::sendControllerCommand( int cmdID,int nParam,const char* strParam,void* ptrParam, int len)
{
    m_arrayRenderTask[CONTROLL_RENDER_TASK]->sendControllerCommand(cmdID,
        nParam, strParam, ptrParam, len);
}

void CRenderTaskMgr::loadModel( EntityViewItem& ite)
{
    if(ite.EntityType == ENTITY_TYPE::TYPE_PLAYER_ROLE)
    {
        m_arrayRenderTask[ACTOR_RENDER_TASK]->loadModel(ite);
    }
    else
    {
        m_arrayRenderTask[NPC_RENDER_TASK]->loadModel(ite);
    }
}

void CRenderTaskMgr::createEntityView( EntityViewItem& ite)
{
    if(ite.EntityType == ENTITY_TYPE::TYPE_PLAYER_ROLE)
    {
        m_arrayRenderTask[ACTOR_RENDER_TASK]->createEntityView(ite);
    }
    else
    {
        m_arrayRenderTask[NPC_RENDER_TASK]->createEntityView(ite);
    }
}

void CRenderTaskMgr::destroyEntityView( int nViewID, int nEntityType)
{
    if(nEntityType == TYPE_PLAYER_ROLE)
    {
        m_arrayRenderTask[ACTOR_RENDER_TASK]->destroyEntityView(nViewID);
    }
    else
    {
        m_arrayRenderTask[NPC_RENDER_TASK]->destroyEntityView(nViewID);
    }
}

void CRenderTaskMgr::sendEntityCommand( int entityID, int nEntityType, int cmdID,int nParam,const char* strParam,void* ptrParam, int len)
{
    // 光效的，算场景任务
    if(cmdID == ENTITY_TOVIEW_ADD_LIGHTING || cmdID == ENTITY_TOVIEW_REMOVE_LIGHTING)
    {
        m_arrayRenderTask[SCENE_RENDER_TASK]->sendEntityCommand(entityID, nEntityType, cmdID, nParam, strParam, ptrParam, len);
        return;
    }

    if(nEntityType == TYPE_PLAYER_ROLE)
    {
        m_arrayRenderTask[ACTOR_RENDER_TASK]->sendEntityCommand(entityID, nEntityType, cmdID, nParam, strParam, ptrParam, len);
    }
    else
    {
        m_arrayRenderTask[NPC_RENDER_TASK]->sendEntityCommand(entityID, nEntityType, cmdID, nParam, strParam, ptrParam, len);
    }
}

void CRenderTaskMgr::loadPrefab( EntityViewItem& ite)
{
    if(ite.EntityType == ENTITY_TYPE::TYPE_PLAYER_ROLE)
    {
        m_arrayRenderTask[ACTOR_RENDER_TASK]->loadPrefab(ite);
    }
    else
    {
        m_arrayRenderTask[NPC_RENDER_TASK]->loadPrefab(ite);
    }
}

void CRenderTaskMgr::loadSkin( EntityViewItem& ite)
{
    if(ite.EntityType == ENTITY_TYPE::TYPE_PLAYER_ROLE)
    {
        m_arrayRenderTask[ACTOR_RENDER_TASK]->loadSkin(ite);
    }
    else
    {
        m_arrayRenderTask[NPC_RENDER_TASK]->loadSkin(ite);
    }
}

bool CRenderTaskMgr::create(void* pRenderEntry)
{
    for(int i = 0; i < RENDER_TASK_MAX; ++i)
    {
        m_arrayRenderTask[i] = new CRenderViewTask(); 
        if(m_arrayRenderTask[i] == NULL)
        {
            ErrorLn(__FUNCTION__ << ", memory allocate failed!");
            return false;
        }
        m_arrayRenderTask[i]->setTimeSlice(20);
        m_arrayRenderTask[i]->init(pRenderEntry, i);
    }

    return true;
}

void CRenderTaskMgr::update()
{
    for(int i = 0; i < RENDER_TASK_MAX; ++i)
    {
        CRenderViewTask* pTask = m_arrayRenderTask[i];
        if(pTask)
        {
            pTask->update();
        }
    }
}

void CRenderTaskMgr::stop()
{
    for(int i = 0; i < RENDER_TASK_MAX; ++i)
    {
        if(m_arrayRenderTask[i])
        {
            m_arrayRenderTask[i]->stop();
            m_arrayRenderTask[i]->update();
        }
    }
}

void CRenderTaskMgr::release()
{
    for(int i = 0; i < RENDER_TASK_MAX; ++i)
    {
        if(m_arrayRenderTask[i])
        {
            delete m_arrayRenderTask[i]; 
            m_arrayRenderTask[i] = 0;
        }
    }

    releaseInstance();
}