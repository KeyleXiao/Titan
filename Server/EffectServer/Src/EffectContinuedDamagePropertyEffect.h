/*******************************************************************
** 文件名:	EffectContinuedDamagePropertyEffect.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	3/15/2016
** 版  本:	1.0
** 描  述:	持续伤害-属性效果相关
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;
#include "TimerHandler.h"
#include "DamageDef.h"
#include "EntityHelper.h"

class CEffectContinuedDamagePropertyEffect : public IEffectEx, public TimerHandler
{
public:
	typedef  EffectServer_ContinuedDamagePropertyEffect SCHEME_DATA;

	CEffectContinuedDamagePropertyEffect( SCHEME_DATA & data ) : m_data(data), m_pEntity(nullptr), m_uidOperator(INVALID_UID), m_uidTarget(INVALID_UID), m_nSpellID(0)
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

			g_EHelper.SetTimer(0, m_data.nInterval, this, nTimerCount, "CEffectContinuedDamagePropertyEffect");
		}
        else
        {
            g_EHelper.SetTimer(0, m_data.nInterval, this, INFINITY_CALL, "CEffectContinuedDamagePropertyEffect");
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
		return new CEffectContinuedDamagePropertyEffect(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	virtual void OnTimer( unsigned long dwTimerID )
	{
        UID uidTarget = INVALID_UID;
        switch (m_data.nTagetType)
        {
        case MA_TAGET_TO_SELF:
            {
                uidTarget = m_uidOperator;
            }
            break;
        case MA_TAGET_TO_TARGET:
            {
                uidTarget = m_uidTarget;
            }
            break;
        default:
            {
                ErrorLn("CEffectContinuedDamagePropertyEffect effect nID="<< GetID() <<"m_data.nTagetType"<<m_data.nTagetType);
                return;
            }
            break;
        }
        if (isInvalidUID(uidTarget))
        {
            return;
        }

        int nAttack = 0;
        switch (m_data.nExtraAttackType)
        {
        case EXTRAATTACK_PA:
            {
                nAttack = getProperty_Integer(uidTarget, PROPERTY_PA);
            }
            break;
        case EXTRAATTACK_MA:
            {
                nAttack = getProperty_Integer(uidTarget, PROPERTY_MA);
            }
            break;
        case EXTRAATTACK_LEVEL:
            {
                nAttack = getProperty_Integer(uidTarget, PROPERTY_LEVEL);
            }
            break;
        case EXTRAATTACK_HP:
            {
                nAttack = getProperty_Integer(uidTarget, PROPERTY_HP);
            }
            break;
        case EXTRAATTACK_MAX_HP:
            {
                nAttack = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
            }
            break;
        case EXTRAATTACK_LOSE_HP:
            {
                int nHp = getProperty_Integer(uidTarget, PROPERTY_HP);
                int nMaxHp = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
                nAttack = nMaxHp-nHp;
            }
            break;
        case EXTRAATTACK_LOSE_HP_PCT:
            {
                int nHp = getProperty_Integer(uidTarget, PROPERTY_HP);
                int nMaxHp = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
                if (nMaxHp <= 0)
                {
                    return;
                }
                float fPercent = (float)(nMaxHp-nHp)/nMaxHp;
                nAttack = (int)(fPercent*EFFECT_RATE);
            }
            break;
        case EXTRAATTACK_MP:
            {
                nAttack = getProperty_Integer(uidTarget, PROPERTY_MP);
            }
            break;
        case EXTRAATTACK_MAX_MP:
            {
                nAttack = getProperty_Integer(uidTarget, PROPERTY_MAX_MP);
            }
            break;
        case EXTRAATTACK_LOSE_MP_PCT:
            {
                int nMp = getProperty_Integer(uidTarget, PROPERTY_MP);
                int nMaxMp = getProperty_Integer(uidTarget, PROPERTY_MAX_MP);
                if (nMaxMp <= 0)
                {
                    return;
                }
                float fPercent = (float)(nMaxMp-nMp)/nMaxMp;
                nAttack = (int)(fPercent*EFFECT_RATE);
            }
            break;
        case EXTRAATTACK_PD:
            {
                nAttack = getProperty_Integer(uidTarget, PROPERTY_PD);
            }
            break;
        case EXTRAATTACK_MD:
            {
                nAttack = getProperty_Integer(uidTarget, PROPERTY_MD);
            }
            break;
        case EXTRAATTACK_SD:
            {
                nAttack = getProperty_Integer(uidTarget, PROPERTY_SD);
            }
            break;
        case EXTRAATTACK_EXTRA_PA:
            {
                int nPA = getProperty_Integer(uidTarget, PROPERTY_PA);
                int nBasePA = OnCalculate_Base(uidTarget, PROPERTY_BASE_PA);
                nAttack = nPA-nBasePA;
                if (nAttack < 0)
                {
                    return;
                }
            }
            break;
        case EXTRAATTACK_EXTRA_MA:
            {
                int nMA = getProperty_Integer(uidTarget, PROPERTY_MA);
                int nBaseMA = OnCalculate_Base(uidTarget, PROPERTY_BASE_MA);
                nAttack = nMA-nBaseMA;
                if (nAttack < 0)
                {
                    return;
                }
            }
            break;
        default:
            {
                ErrorLn("CEffectContinuedDamagePropertyEffect effect nID="<< GetID() <<"nExtraAttackType error"<<m_data.nExtraAttackType);
                return;
            }
            break;
        }

        UID uidPropertyTarget = INVALID_UID;
        switch (m_data.nPropertyTagetType)
        {
        case MA_TAGET_TO_SELF:
            {
                uidPropertyTarget = m_uidOperator;
            }
            break;
        case MA_TAGET_TO_TARGET:
            {
                uidPropertyTarget = m_uidTarget;
            }
            break;
        default:
            {
                ErrorLn("CEffectContinuedDamagePropertyEffect effect nID="<< GetID() <<"m_data.nPropertyTagetType"<<m_data.nPropertyTagetType);
                return;
            }
            break;
        }
        if (isInvalidUID(uidPropertyTarget))
        {
            return;
        }

        int nPropertyValue = 0;
        switch (m_data.nPropertyEffectType)
        {
        case DAMAGE_PROPERTY_EFFECT_HP:
            {
                nPropertyValue = getProperty_Integer(uidPropertyTarget, PROPERTY_HP);
            }
            break;
        case DAMAGE_PROPERTY_EFFECT_MAX_HP:
            {
                nPropertyValue = getProperty_Integer(uidPropertyTarget, PROPERTY_MAX_HP);
            }
            break;
        case DAMAGE_PROPERTY_EFFECT_LOSE_HP:
            {
                int nHp = getProperty_Integer(uidPropertyTarget, PROPERTY_HP);
                int nMaxHp = getProperty_Integer(uidPropertyTarget, PROPERTY_MAX_HP);
                nPropertyValue = nMaxHp-nHp;
            }
            break;
        default:
            {
                ErrorLn("CEffectContinuedDamagePropertyEffect effect nID="<< GetID() <<"nPropertyEffectType error"<<m_data.nPropertyEffectType);
                return;
            }
            break;
        }

        // 算出伤害
        int nDamageHP = (int)((m_data.nValue + nAttack*m_data.fExtraValue)*m_data.fFactor*nPropertyValue);

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
        damage.fAppendPCTPDP = static_cast<float>(getProperty_Integer(m_uidOperator, PROPERTY_APPEND_PCT_PDP)) / ZOOM_IN_MULTIPLE;      // 附加额外百分比护甲穿透
        damage.fAppendPCTPMP = static_cast<float>(getProperty_Integer(m_uidOperator, PROPERTY_APPEND_PCT_MDP)) / ZOOM_IN_MULTIPLE;      // 附加额外百分比魔抗穿透
        if (m_pEntity) {
            damage.nUseFlag = m_pEntity->getUseFlag()->getAll();    // 用途标识
        }

		// 发送实体消息
		g_EHelper.sendEntityMessage(m_uidTarget, PART_DAMAGE, DAMAGE_MSG_DAMAGE, (char *)&damage, sizeof(damage));
	}

    // 计算基础属性
    int OnCalculate_Base(UID uid, int nBaseID)
    {
        ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
        if (pSchemeCenter == NULL)
        {
            return 0;
        }
        // 取得配置人物基础属性对象
        ISchemePersonBaseProp *pSchemePersonBaseProp = pSchemeCenter->getShemePersonBaseProp();
        if (pSchemePersonBaseProp == NULL)
        {
            return 0;
        }

        int nPropType = BASE_PROP_TYPE;
        int nMapID = getMapID(uid);
        if ( IsMainMap(nMapID) )
        {
            nPropType = BASE_PROP_HKEE_TYPE;
        }
        // 职业
        int nVocation = getProperty_Integer(uid, PROPERTY_VOCATION);
        SPersonBasePropScheme *pPersonBasePropScheme = pSchemePersonBaseProp->getPersonBaseProp(nVocation, nPropType);
        if (pPersonBasePropScheme == NULL)
        {
            return 0;
        }

        // 属性索引
        int nPropIndex = -1;
        for (int nIndex=0; nIndex<PERSON_BASE_PROP_COUNT; ++nIndex)
        {
            if (pPersonBasePropScheme->sProp[nIndex].nPropID == nBaseID)
            {
                nPropIndex = nIndex;
                break;
            }
        }
        if (nPropIndex < 0 || nPropIndex >= PERSON_BASE_PROP_COUNT)
        {
            return 0;
        }

        // 等级
        int nLevel = getProperty_Integer(uid, PROPERTY_LEVEL);
        // 基础值 = 等级基础值 + (等级-1)*基础值成长率
        int nBase = (int)(pPersonBasePropScheme->sProp[nPropIndex].nBase + (nLevel-1)*pPersonBasePropScheme->sProp[nPropIndex].fGrowRate);

        return nBase;
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
