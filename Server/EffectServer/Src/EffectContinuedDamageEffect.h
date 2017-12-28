/*******************************************************************
** 文件名:	EffectContinuedDamageEffect.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/15/2015
** 版  本:	1.0
** 描  述:	效果-持续伤害-效果相关
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;
#include "TimerHandler.h"
#include "DamageDef.h"
#include "EntityHelper.h"

class CEffectContinuedDamageEffect : public IEffectEx, public TimerHandler
{
public:
	typedef  EffectServer_ContinuedDamageEffect SCHEME_DATA;

	CEffectContinuedDamageEffect( SCHEME_DATA & data ) : m_data(data), m_pEntity(nullptr), m_uidOperator(INVALID_UID), m_uidTarget(INVALID_UID), m_nSpellID(0)
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
		if( context==0 || context->pEntity == 0)
			return false;
        m_pEntity = context->pEntity;
		m_uidOperator = context->uidOperator;
		if (isInvalidUID(m_uidOperator))
		{
			return false;
		}

        m_uidTarget = context->uidTarget;
		if (isInvalidUID(m_uidTarget))
		{
			return false;
		}

        m_nSpellID = context->nID;

		// 首次执行，也算次数
		int nCount = 0;
		// 首次伤害立即执行
		if (m_data.nFirst > 0)
		{
			OnTimer(0);

			nCount = 1;
		}

		if(m_data.nCount >= 1)
		{
			int nTimerCount = m_data.nCount - nCount;
			if (nTimerCount <= 0)
			{
				return true;
			}

			g_EHelper.SetTimer(0, m_data.nInterval, this, nTimerCount, "CEffectContinuedDamageEffect");
		}
        else
        {
            g_EHelper.SetTimer(0, m_data.nInterval, this, INFINITY_CALL, "CEffectContinuedDamageEffect");
        }
		
		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		g_EHelper.KillTimer(0, this);

		// 清理数据
        m_pEntity = nullptr;
		m_uidOperator = INVALID_UID;
		m_uidTarget = INVALID_UID;
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectContinuedDamageEffect(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	virtual void OnTimer( unsigned long dwTimerID )
	{
		if (isInvalidUID(m_uidTarget))
		{
			return;
		}

		if(GetState(m_uidTarget) == ENTITY_STATE_DIE)
		{
			return;
		}

		int nDamageHP = 0;
		switch (m_data.nValueType)
		{
		case MA_VALUE_NUM:
			{
				nDamageHP = m_data.nValue;
			}
			break;
		case MA_VALUE_PER:
			{
				int nMaxHP = getProperty_Integer(m_uidTarget, PROPERTY_MAX_HP);
				nDamageHP = (int)(m_data.nValue/EFFECT_RATE*nMaxHP);
			}
			break;
		case MA_VALUE_LOSE_PER:
			{
				int nHP = getProperty_Integer(m_uidTarget, PROPERTY_HP);
				int nMaxHP = getProperty_Integer(m_uidTarget, PROPERTY_MAX_HP);
				nDamageHP = (int)(m_data.nValue/EFFECT_RATE*(nMaxHP-nHP));
			}
			break;
		default:
			{
				ErrorLn("CEffectContinuedDamageEffect error nValueType="<<m_data.nValueType);
				return;
			}
			break;
		}

		// 伤害消息
		msg_entity_damage damage;
        damage.nID = m_nSpellID;
		damage.uidOperator = m_uidOperator;
		damage.nDamageType = (m_data.nDamageType|DAMAGE_TYPE_EFFECT);
		damage.nDeadType = m_data.nDeadType;
		damage.nSpellEffectType = m_data.nSpellEffectType;
		damage.nDamageHP = nDamageHP;
        damage.nDamageBonus = getProperty_Integer(m_uidOperator, PROPERTY_DAMAGE_BONUS);
        damage.fDamageBonusPCT = getProperty_Integer(m_uidOperator, PROPERTY_DAMAGE_BONUS_PCT)/ZOOM_IN_MULTIPLE;
        damage.nUseFlag = m_pEntity->getUseFlag()->getAll();    // 用途标识
		// 发送实体消息
		g_EHelper.sendEntityMessage(m_uidTarget, PART_DAMAGE, DAMAGE_MSG_DAMAGE, (char *)&damage, sizeof(damage));
	}

private:
	SCHEME_DATA               m_data;
    __IEntity   *             m_pEntity;
	// 操作者UID
	UID						  m_uidOperator;
	// 目标UID
	UID						  m_uidTarget;
    // 技能ID
    int                       m_nSpellID;
};
