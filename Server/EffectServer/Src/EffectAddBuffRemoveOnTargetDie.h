/*******************************************************************
** 文件名:	EffectAddBuffRemoveOnTargetDie.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	ljw
** 日  期:	11/02/2015
** 版  本:	1.0
** 描  述:	效果-给自己加buff 并且在目标死亡时移除
            用于解决虞姬附身后 附身目标死亡后需要取消附身buff的问题
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "EntityHelper.h"
#include "IBuffServer.h"
using namespace EFFECT_SERVER;
using namespace rkt;

class CEffectAddBuffRmvOnTargetDie : public IEffectEx, public IEventExecuteSink
{
public:
    typedef  EffectServer_AddBuff_And_RmoveOnTargetDie    SCHEME_DATA;

    CEffectAddBuffRmvOnTargetDie( SCHEME_DATA & data ) : m_data(data),m_pMaster(0),m_uidTarget(INVALID_UID)
    {
    }

    // 获取效果ID
    virtual int             GetID()
    {
        return m_data.nID;
    }

    // 效果启用
    virtual bool			Start( EFFECT_CONTEXT * context,CONDITION_CONTEXT *pConditionContext )
    {
        if( context==0 || context->pEntity==0 )
        {
            return false;
        }

        __IBuffPart *pBuffPart = (__IBuffPart *)context->pEntity->getEntityPart(PART_BUFF);
        if (pBuffPart == NULL)
        {
            return false;
        }

        SceneLogicEventHelper helper(context->pEntity->getSceneID());
        if(helper.m_ptr == 0)
        {
            return false;
        }

        m_pMaster = context->pEntity;
        m_uidTarget = context->uidTarget; 

        // 给自己加buff
        pBuffPart->Add(m_data.nBuffID, m_data.nBuffLevel, m_pMaster->getUID(), 0);

        // 订阅目标死亡事件
        helper.m_ptr->Subscibe(this, EVENT_GAME_ENTITY_DIE, UID_2_TYPE(m_uidTarget), m_uidTarget, __FUNCTION__);

        m_pMaster->getEntityEvent()->Subscibe(this, EVENT_ENTITY_REMOVEBUFF, __FUNCTION__);

        return true;
    }

    virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
    {
        switch(wEventID)
        {
        case EVENT_GAME_ENTITY_DIE:
            {
                if(dwSrcID != m_uidTarget)
                {
                    return;
                }

                // 目标死亡移除自己buff
                if(m_pMaster != NULL)
                {
                    __IBuffPart *pBuffPart = (__IBuffPart *)m_pMaster->getEntityPart(PART_BUFF);
                    if (pBuffPart != NULL)
                    {
                        pBuffPart->Remove(m_data.nBuffID, m_pMaster->getUID(), m_pMaster->getUID());
                    }
                }

                Stop();
            }
            break;
        case EVENT_ENTITY_REMOVEBUFF:
            {
                event_entity_remove_buff* pEvent = (event_entity_remove_buff*)pszContext;
                if(pEvent->dwBuffID == m_data.nBuffID && pEvent->dwBuffLevel == m_data.nBuffLevel)
                {
                    Stop();
                }
            }
            break;
        }
    }

    // 效果停止
    virtual void			Stop()
    {
        if(m_pMaster != 0)
        {
            SceneLogicEventHelper helper(m_pMaster->getSceneID());
            if(helper.m_ptr != 0)
            {
                // 反注册目标死亡事件
                helper.m_ptr->UnSubscibe(this, EVENT_GAME_ENTITY_DIE, UID_2_TYPE(m_uidTarget), m_uidTarget);
            }

            m_pMaster->getEntityEvent()->UnSubscibe(this, EVENT_ENTITY_REMOVEBUFF);
        }
        m_pMaster = 0;
        m_uidTarget = INVALID_UID;
    }

    // 克隆一个新效果
    virtual IEffect *       Clone() 
    {
        return new CEffectAddBuffRmvOnTargetDie(m_data);
    }

    // 释放
    virtual void			Release() 
    {
        Stop();
        delete this;
    }

private:
    SCHEME_DATA               m_data;
    __IEntity*                m_pMaster;
    UID                       m_uidTarget;
};
