/*******************************************************************
** 文件名:	EffectChangeDodgeState.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	9/13/2016
** 版  本:	1.0
** 描  述:	效果-改变轻功状态
            http://172.16.0.120/redmine/issues/2337
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_CLIENT;

class CEffectChangeDodgeState : public IEffect
{
public:
	typedef EffectClient_ChangeDodgeState SCHEME_DATA;

	CEffectChangeDodgeState( SCHEME_DATA & data ) : m_data(data),m_pEntity(NULL)
	{
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT *pContext )
	{
		if ( pContext==0 || pContext->pEntity==0 )
			return false;

        m_pEntity = pContext->pEntity;

        // 改变轻功状态
        cmd_Entity_Change_Dodge_State ChangeDodgeState;
        ChangeDodgeState.nType = 1;                                 // 轻功状态(0退出,不为0进入)
        ChangeDodgeState.fSpeedUpGround = m_data.fSpeedUpGround;    // 地面上时加速的速度
        ChangeDodgeState.nSpellID       = m_data.nSpellID;          // 退出加速技能I
        ChangeDodgeState.fSpeed0        = m_data.fSpeed0;           // 第一次加速的速度
        ChangeDodgeState.fSpeed1        = m_data.fSpeed1;           // 第二次加速的速度
        ChangeDodgeState.fSpeed2        = m_data.fSpeed2;           // 第三次加速的速度
  
        m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_CHANGE_DODGE_STATE, 0, 0, &ChangeDodgeState, sizeof(ChangeDodgeState));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        if (m_pEntity != NULL)
        {
            // 改变轻功状态
            cmd_Entity_Change_Dodge_State ChangeDodgeState;
            ChangeDodgeState.nType = 0;                                 // 轻功状态(0退出,不为0进入)
            ChangeDodgeState.fSpeedUpGround = m_data.fSpeedUpGround;    // 地面上时加速的速度
            ChangeDodgeState.nSpellID       = m_data.nSpellID;          // 退出加速技能I
            ChangeDodgeState.fSpeed0        = m_data.fSpeed0;           // 第一次加速的速度
            ChangeDodgeState.fSpeed1        = m_data.fSpeed1;           // 第二次加速的速度
            ChangeDodgeState.fSpeed2        = m_data.fSpeed2;           // 第三次加速的速度
            m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_CHANGE_DODGE_STATE, 0, 0, &ChangeDodgeState, sizeof(ChangeDodgeState));

            m_pEntity = NULL;
        }
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() { return new CEffectChangeDodgeState(m_data); }

	// 取得效果ID
	virtual int				GetEffectID(){ return m_data.nID; }

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA             m_data;

    IEntity                 *m_pEntity;
};
