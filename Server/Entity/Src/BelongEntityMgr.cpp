#include "stdafx.h"
#include "EntityDef.h"
#include "IFramework.h"
#include "IServiceMgr.h"
#include "IServerGlobal.h"
#include "EntityHelper.h"
#include "IAIPart.h"
#include "IMobaGoodsPart.h"
#include "EventDef.h"
#include "IEntityEvent.h"
#include "BelongEntityMgr.h"


/////////////////////////////////////////////////////////////////////////

CBelongEntityMgr::CBelongEntityMgr()
    : m_pMaster(nullptr)
{

}


CBelongEntityMgr::~CBelongEntityMgr()
{
    m_pMaster = nullptr;
}


bool CBelongEntityMgr::onload(__IEntity  * pMaster)
{
    if( !pMaster )
        return false;

    m_pMaster = pMaster;

    return true;
}

void CBelongEntityMgr::release(void)
{
    restore();

    m_pMaster = nullptr;

    delete this;
}

void CBelongEntityMgr::restore(void)
{
    ENTITY_TABLE entities = m_mapEntity;
    for(ENTITY_TABLE::iterator iter = entities.begin(); iter != entities.end(); ++iter)
    {
        remove(iter->first);
    }

    m_mapEntity.clear();
}


// 插入一个附属实体信息
void CBelongEntityMgr::push_back(EntityBelongContext context)
{
    if(!m_pMaster)
    {
        return;
    }
    
    // 触发附属实体通知事件
    __IEntityEvent * pEntityEvent = m_pMaster->getEntityEvent();
    if (pEntityEvent)
    {
        event_entity_belong_monster_notify eventData;
        eventData.uidMaster = m_pMaster->getUID();
        eventData.uidMonster = context.uid;
        eventData.nFlag = MBEN_FLAG_LIFE;
        eventData.nValue = context.isCreated;

        pEntityEvent->FireExecute(EVENT_ENTITY_BELONG_MONSTER_NOTIFY, (LPCSTR) &eventData, sizeof(eventData));
    }

    if(m_pMaster->getType() == TYPE_PLAYER_ROLE)
    {
        // 通知到AI部件
        __IAIPart* pAIPart = (__IAIPart*)m_pMaster->getEntityPart(PART_AI);
        if(pAIPart)
        { 
            pAIPart->onAddRemoveSummonEntity( context.uid, context.isCreated);
        }
    }

    // TODO 后面这些都可以用事件的方式处理
    // 通知GOODS部件
    __IMobaGoodsPart* pMobaGoodsPart = (__IMobaGoodsPart*)m_pMaster->getEntityPart(PART_MOBAGOODS);
    if (pMobaGoodsPart)
    {
        if( context.isCreated )
        {
            // 分身怪
            if( (context.nFlag & MONSTER_CREATE_CLONE) == MONSTER_CREATE_CLONE )
            {
                pMobaGoodsPart->AddInheritMonster(context.uid);
            }                
        }
        else
        {
            pMobaGoodsPart->RemoveInheritMonster(context.uid);
        }
    }
    
    // 移除时
    if( context.isCreated )
    {
        m_mapEntity[context.uid] = context;
    }
    else
    {
        m_mapEntity.erase(context.uid);
    }
}

// 删除实体并移除附属信息
void CBelongEntityMgr::remove(UID uid)
{
    //TraceLn(__FUNCTION__":uid="<< uid);
    switch ( UID_2_TYPE(uid) )
    {
    case TYPE_PLAYER_ROLE:
        {
            SceneLogicHelper helper(UID_2_SCENE(uid));
            if(helper.m_ptr)
            {

                helper.m_ptr->destroyActor(uid, elogout_offline);
            }            
        }
        break;

    case TYPE_MONSTER:
        {
            MonsterHelper helper(uid);
            INPCService * pNpcService = helper.m_ptr;       
            if( pNpcService )
            {
                pNpcService->destroyNPC(uid);
            }
        }
        break;

    case TYPE_TANK:
        {

        }
        break;

    default: break;
    }

    m_mapEntity.erase(uid);
}

// 获取附属实体信息
EntityBelongContext *  CBelongEntityMgr::get(UID uid)
{
    ENTITY_TABLE::iterator iter = m_mapEntity.find(uid);
    return (iter!=m_mapEntity.end() ? &(iter->second) : NULL);
}