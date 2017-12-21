#pragma once
#include "AI_ManagedDef.h"

#define ANY_CAMP -1

struct CAIProxy;
class AIManager;

// AI����
struct IAIService
{
    // ע��AI����
    virtual void registerAIProxy(UID uidMaster, CAIProxy* pAIProxy) = 0;

    // ��ע��AI����
    virtual void unregisterAIProxy(UID uidMaster) = 0;

    // ����AI
    virtual void createAI(UID uidMaster, int nCamp, int nWarTypeID, int nSchemeID) = 0;

    // ����AI
    virtual void startAI(UID uidMaster) = 0;

    // ����AI
    virtual void destroyAI(UID uidMaster) = 0;
    
    // ����AI�¼�
    virtual void onEvent(UID uidMaster, int nAIEventID, AI_CONTEXT context, AIManager* pAIManager = NULL) = 0;

    //�¼��㲥��AIӢ��  bNear2Far �ɽ���Զ
    virtual void broadcastEvent2Actor(int nCampMask, int nAIEventID, AI_CONTEXT context, bool bNear2Far = false) = 0;

    // �¼��㲥
    virtual void groupcastEvent(UID* pUIDArray, int nUIDNum, int nAIEventID, AI_CONTEXT& context) = 0;

    // AI��Ϣ
    virtual void onMessage(UID uidMaster, int nMsgID) = 0;

    // traceʵ��AI
    virtual void traceAIInfo(UID uidMaster, bool bEnable = true) = 0;
};