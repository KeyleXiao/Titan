/*******************************************************************
** 文件名:	EffectKillingControl.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	7/17/2015
** 版  本:	1.0
** 描  述:	效果-惊险救治
            添加效果判定
            1、是否是惊险救治 不是则不添加
            2、删除效果发送夺命控制判定
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
#include "TimerHandler.h"

class CEffectKillingControl : public IEffectEx
{
public:
	typedef  EffectServer_KillingControl    SCHEME_DATA;

	CEffectKillingControl( SCHEME_DATA & data ) : m_data(data), m_pEntity(NULL), m_uidOperator(INVALID_UID), m_uidTarget(INVALID_UID)
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
        m_pEntity = context->pEntity;
        m_uidOperator = context->uidOperator;
        m_uidTarget = context->uidTarget;

		return true;
	}


	// 效果停止
	virtual void			Stop()
	{
        // 杀掉定时器定时器
        if (m_pEntity != NULL)
        {
            // 检查人物是否死亡
            if (m_pEntity->getState() == ENTITY_STATE_DIE)
            {
                // 发出惊险救治事件
                // 战场里面要知道，所以发个事件，告诉战场清除
                SceneLogicEventHelper eventHelper(m_pEntity->getSceneID());
                IEventEngine *pGlobalEvent = eventHelper.m_ptr;
                if (pGlobalEvent != NULL)
                {
                    event_game_killingcontrol killingcontrol;
                    killingcontrol.uidOperator = m_uidOperator;
                    killingcontrol.uidTarget = m_uidTarget;
                    pGlobalEvent->FireExecute(EVENT_GAME_KILLINGCONTROL, TYPE_PLAYER_ROLE, 0, (LPCSTR)&killingcontrol, sizeof(killingcontrol));
                }
            }
        }
        
        m_pEntity = 0;
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectKillingControl(m_data);
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
};
