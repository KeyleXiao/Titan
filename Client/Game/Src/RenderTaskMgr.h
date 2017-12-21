#pragma once
#include "IRenderView.h"

class CRenderViewTask;

class CRenderTaskMgr : public IRenderViewProxy, public Singleton<CRenderTaskMgr>
{
public:
    enum
    {
        CONTROLL_RENDER_TASK,  // ��������
        ACTOR_RENDER_TASK,     // ��ɫ����
        NPC_RENDER_TASK,       // npc����
        SCENE_RENDER_TASK,     // ��������

        DEFAULT_RENDER_TASK,   // Ĭ�϶���

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