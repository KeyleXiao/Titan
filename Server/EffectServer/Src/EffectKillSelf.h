/*******************************************************************
** 文件名:	EffectKillSelf.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	rjc
** 日  期:	2015/3/27
** 版  本:	1.0
** 描  述:	自杀
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;
#include "EntityHelper.h"

class CEffectKillSelf : public IEffectEx, public TimerHandler
{
public:
	typedef  EffectServer_KillSelf    SCHEME_DATA;

	CEffectKillSelf( SCHEME_DATA & data ) : m_data(data),m_pMaster(NULL),m_uidOperator(INVALID_UID),m_uidTarget(INVALID_UID),m_nSpellID(0)
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
			return false;

        m_pMaster = context->pEntity;
        m_uidOperator = context->uidOperator;
        m_uidTarget = context->uidTarget;

        m_nSpellID = context->nID;

        if (m_data.nDelayTime == 0)
        {
            OnTimer(0);
        }
        else if (m_data.nDelayTime > 0)
        {
            g_EHelper.SetTimer(0, m_data.nDelayTime, this, 1, "EffectKillSelf");
        }
			
		return true;
	}

	///////////////////////////////// TimerHandler /////////////////////////////////////////
	virtual void OnTimer( unsigned long dwTimerID )
	{
		if (m_pMaster == NULL)
		{
			return;
		}

        UID uidOperator = INVALID_UID;
        switch (m_data.nOperatorType)
        {
        case MA_TAGET_TO_SELF:
            {
                uidOperator = m_pMaster->getUID();
            }
            break;
        case MA_TAGET_TO_TARGET:
            {
                uidOperator = m_uidOperator;
            }
            break;
        case MA_TAGET_TO_OPERATOR:
            {
                uidOperator = m_uidTarget;
            }
            break;
        default:
            {
                ErrorLn("CEffectKillSelf error nOperatorType="<<m_data.nOperatorType);
                return;
            }
            break;
        }
        if (isInvalidUID(uidOperator))
        {
            ErrorLn("CEffectKillSelf error uidOperator="<<uidOperator);
            return;
        }

        UID uidTarget = INVALID_UID;
        switch (m_data.nTargetType)
        {
        case MA_TAGET_TO_SELF:
            {
                uidTarget = m_pMaster->getUID();
            }
            break;
        case MA_TAGET_TO_TARGET:
            {
                uidTarget = m_uidOperator;
            }
            break;
        case MA_TAGET_TO_OPERATOR:
            {
                uidTarget = m_uidTarget;
            }
            break;
        default:
            {
                ErrorLn("CEffectKillSelf error nTargetType="<<m_data.nTargetType);
                return;
            }
            break;
        }
        if (isInvalidUID(uidTarget))
        {
            ErrorLn("CEffectKillSelf error uidTarget="<<uidTarget);
            return;
        }

        int nMaxHp = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);

        // 伤害消息
        msg_entity_damage damage;
        damage.nID = m_nSpellID;
        damage.uidOperator = m_uidOperator;
        damage.nDamageType = (m_data.nDamageType|DAMAGE_TYPE_EFFECT);
        damage.nDeadType = m_data.nDeadType;
        damage.nSpellEffectType = m_data.nSpellEffectType;
        damage.nDamageHP = nMaxHp;
        damage.nDamageBonus = getProperty_Integer(m_uidOperator, PROPERTY_DAMAGE_BONUS);
        damage.fDamageBonusPCT = getProperty_Integer(m_uidOperator, PROPERTY_DAMAGE_BONUS_PCT)/ZOOM_IN_MULTIPLE;
        damage.nUseFlag = m_pMaster->getUseFlag()->getAll();    // 用途标识

        // 发送实体消息
        g_EHelper.sendEntityMessage(m_uidTarget, PART_DAMAGE, DAMAGE_MSG_DAMAGE, (char *)&damage, sizeof(damage));
	}

	// 效果停止
	virtual void			Stop()
	{
        if (m_pMaster != NULL)
        {
            g_EHelper.KillTimer(0, this);
            m_pMaster = NULL;
        }
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectKillSelf(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA               m_data;
	__IEntity *				  m_pMaster;
    UID                       m_uidOperator;
	UID						  m_uidTarget;
    int                       m_nSpellID;
};
