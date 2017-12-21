/*******************************************************************
** 文件名:	EffectThrilingCure.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	7/17/2015
** 版  本:	1.0
** 描  述:	效果-惊险救治
            添加效果判定
            1、是否是惊险救治
            2、最后删除时判定发送惊险救治事件
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;
#include "IEntity.h"
#include "IBuffServer.h"
#include "BuffMessageDef.h"
#include "ExternalHelper.h"
#include "WarHelper.h"
#include "IWarPart.h"
#include "AOI_Define.h"
#include "EntityHelper.h"
#include "TimerHandler.h"

class CEffectThrilingCure : public IEffectEx
{
public:
	typedef  EffectServer_ThrillingCure    SCHEME_DATA;

    CEffectThrilingCure( SCHEME_DATA & data ) : m_data(data),m_bIsThrillingCure(false), m_pEntity(NULL), m_uidOperator(INVALID_UID),m_uidTarget(INVALID_UID)

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
            m_bIsThrillingCure = false;
			return false;
        }
        m_pEntity = context->pEntity;
        // 惊险救治判定
         __IEntity *pEntity = context->pEntity;
         m_uidOperator = context->uidOperator;
         m_uidTarget = context->uidTarget;
         int nSceneID = pEntity->getSceneID();
         int nCamp = pEntity->getIntProperty(PROPERTY_CAMP);
         if (!IsDynamicScene(nSceneID))
         {
             m_bIsThrillingCure = false;
             return false;
         }

        Vector3 loc = pEntity->getLocation();
        SceneServiceHelper sceneHelper(nSceneID);
        IGameSceneService * pSceneService = sceneHelper.m_ptr;
        if ( pSceneService == 0 )
        {
            m_bIsThrillingCure = false;
            return false;
        }

        bool bFindEnimy = false;
        AOI_PROXY_PTR pProxyArray[MAX_INTEREST_OBJECT];
        int num = pSceneService->k_nearest( loc, m_data.fCheckDistans, pProxyArray, MAX_INTEREST_OBJECT, CAMP_EXCEPT_MASK(nCamp), true);
        for (int i=0; i<num; ++i)
        {
            if(pProxyArray[i] == 0 )
                continue;

            UID uid = pProxyArray[i]->m_uid;
            if ( UID_2_TYPE(uid) == TYPE_PLAYER_ROLE)
            {
                bFindEnimy = true;
                break;
            }
        }

        if (!bFindEnimy)
        {
            m_bIsThrillingCure = false;
        }

        // 血量判定
        int nCurHP = pEntity->getIntProperty(PROPERTY_HP);
        int nCurMaxHP = pEntity->getIntProperty(PROPERTY_MAX_HP);
        if (nCurHP <= (int)(nCurMaxHP*0.2))
        {
            m_bIsThrillingCure =  true;
        }
		return true;
	}


	// 效果停止
	virtual void			Stop()
	{
        // 杀掉定时器定时器
        // 检查人物是否死亡
        if (m_pEntity != NULL && m_bIsThrillingCure == true)
        {
            if (m_pEntity->getState() != ENTITY_STATE_DIE)
            {
                // 发出惊险救治事件
                // 战场里面要知道，所以发个事件，告诉战场清除
                SceneLogicEventHelper eventHelper(m_pEntity->getSceneID());
                IEventEngine *pGlobalEvent = eventHelper.m_ptr;
                if (pGlobalEvent != NULL)
                {
                    event_game_thrillingcure thrillingcure;
                    thrillingcure.uidOperator = m_uidOperator;
                    thrillingcure.uidTarget = m_uidTarget;
                    thrillingcure.dwTime = getTickCountEx();
                    pGlobalEvent->FireExecute(EVENT_GAME_THRILLINGCURE, TYPE_PLAYER_ROLE, 0, (LPCSTR)&thrillingcure, sizeof(thrillingcure));
                }
            }
        }
        
        m_pEntity = 0;
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectThrilingCure(m_data);
	}

	// 释放
	virtual void			Release() 
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA               m_data;
    // 实体指针
    __IEntity*                m_pEntity;

    UID                       m_uidOperator;

    UID                       m_uidTarget;

    bool                      m_bIsThrillingCure;
};
