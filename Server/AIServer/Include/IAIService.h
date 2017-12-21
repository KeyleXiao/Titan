#pragma once
#include "AI_ManagedDef.h"

#define ANY_CAMP -1

struct CAIProxy;
class AIManager;

// AI服务
struct IAIService
{
    // 注册AI代理
    virtual void registerAIProxy(UID uidMaster, CAIProxy* pAIProxy) = 0;

    // 反注册AI代理
    virtual void unregisterAIProxy(UID uidMaster) = 0;

    // 创建AI
    virtual void createAI(UID uidMaster, int nCamp, int nWarTypeID, int nSchemeID) = 0;

    // 开启AI
    virtual void startAI(UID uidMaster) = 0;

    // 销毁AI
    virtual void destroyAI(UID uidMaster) = 0;
    
    // 处理AI事件
    virtual void onEvent(UID uidMaster, int nAIEventID, AI_CONTEXT context, AIManager* pAIManager = NULL) = 0;

    //事件广播给AI英雄  bNear2Far 由近及远
    virtual void broadcastEvent2Actor(int nCampMask, int nAIEventID, AI_CONTEXT context, bool bNear2Far = false) = 0;

    // 事件广播
    virtual void groupcastEvent(UID* pUIDArray, int nUIDNum, int nAIEventID, AI_CONTEXT& context) = 0;

    // AI消息
    virtual void onMessage(UID uidMaster, int nMsgID) = 0;

    // trace实体AI
    virtual void traceAIInfo(UID uidMaster, bool bEnable = true) = 0;
};