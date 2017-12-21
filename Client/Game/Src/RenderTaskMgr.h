#pragma once
#include "IRenderView.h"

class CRenderViewTask;

class CRenderTaskMgr : public IRenderViewProxy, public Singleton<CRenderTaskMgr>
{
public:
    enum
    {
        CONTROLL_RENDER_TASK,  // 控制任务
        ACTOR_RENDER_TASK,     // 角色任务
        NPC_RENDER_TASK,       // npc任务
        SCENE_RENDER_TASK,     // 场景任务

        DEFAULT_RENDER_TASK,   // 默认队列

        RENDER_TASK_MAX,
    };


    CRenderTaskMgr();

    virtual void  sendControllerCommand( int cmdID,int nParam,const char* strParam,void* ptrParam, int len);
    virtual void  loadModel( EntityViewItem& ite);
    virtual void  createEntityView( EntityViewItem& ite);
    virtual void  destroyEntityView( int entityID, int nEntityType);
    virtual void  sendEntityCommand( int entityID, int nEntityType, int cmdID,int nParam,const char* strParam,void* ptrParam, int len);
    virtual void  loadPrefab( EntityViewItem& ite);
    virtual void  loadSkin( EntityViewItem& ite);

    bool create(void* pRenderEntry);

    void update();

    void stop();

    void release();

private:
    CRenderViewTask* m_arrayRenderTask[RENDER_TASK_MAX];
};