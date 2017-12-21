/*******************************************************************
** 文件名:	EffectGroupControl.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	7/17/2015
** 版  本:	1.0
** 描  述:	效果-团控效果

            通过给其他玩家添加的控制效果回调给添加控制效果释放的玩家
            （每个玩家命中控制或者定身技能都会给技能释放者添加这个buff）
            buff累加到特定等级后发送团控事件
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

class CEffectGroupControl : public IEffectEx
{
public:
	typedef  EffectServer_GroupControl    SCHEME_DATA;

	CEffectGroupControl( SCHEME_DATA & data ) : m_data(data), m_pEntity(NULL)
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

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        if(!m_pEntity)
        {
            return;
        }

        if (m_data.nLevel > 1)
        {
            // 发送团控事件
            SceneLogicEventHelper eventHelper(m_pEntity->getSceneID());
            IEventEngine *pGlobalEvent = eventHelper.m_ptr;
            UID uidGroupControl = m_pEntity->getUID();
            if (pGlobalEvent != NULL)
            {
                event_game_groupcontrol groupControl;
                groupControl.uidGroupControl = uidGroupControl;
				groupControl.nGroupControlNum = m_data.nLevel;
                pGlobalEvent->FireExecute(EVENT_GAME_GROUPCONTROL, TYPE_PLAYER_ROLE, 0, (LPCSTR)&groupControl, sizeof(groupControl));
            }
        }
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectGroupControl(m_data);
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

};
