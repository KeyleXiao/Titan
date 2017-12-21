/*******************************************************************
** 文件名:	EffectDistanceDamage.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	2/25/2016
** 版  本:	1.0
** 描  述:	效果-距离伤害
            http://172.16.0.120/redmine/issues/1647
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;

class CEffectDistanceDamage : public IEffectEx
{
public:
	typedef  EffectServer_DistanceDamage    SCHEME_DATA;

	CEffectDistanceDamage( SCHEME_DATA & data ) : m_data(data)
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
		if ( context==0 || context->pEntity==0 )
        {
			return false;
        }

        __IEntityProperty *pEntityProperty = (__IEntityProperty *)context->pEntity->getEntityPart(PART_PROPERTY);
        if (pEntityProperty == NULL)
        {
            return false;
        }

        __ISpellPart *pSpellPart = (__ISpellPart *)context->pEntity->getEntityPart(PART_SPELL);
        if (pSpellPart == NULL)
        {
            return false;
        }

        ISpell *pSpell = pSpellPart->findSpell(context->nID);
        if (pSpell == NULL)
        {
            return false;
        }

        IAttackObject *pAttackObject = pSpellPart->getAttackObject(context->dwAttackSerialID);
        if (pAttackObject == NULL)
        {
            return false;
        }
        Vector3 vStartLoc = pAttackObject->getStartTile();

        UID uidProperty = INVALID_UID;
        switch (m_data.nPropertyTargetType)
        {
        case MA_TAGET_TO_SELF:
            {
                uidProperty = context->pEntity->getUID();
            }
            break;
        case MA_TAGET_TO_TARGET:
            {
                uidProperty = context->uidTarget;
            }
            break;
        case MA_TAGET_TO_OPERATOR:
            {
                uidProperty = context->uidOperator;
            }
            break;
        default:
            {
                ErrorLn("CEffectDistanceDamage error nPropertyTargetType="<<m_data.nPropertyTargetType);
                return false;
            }
            break;
        }
        if (isInvalidUID(uidProperty))
        {
            return false;
        }
        // 取得属性
        int nPropertyValue = getProperty_Integer(uidProperty, m_data.nPropertyType);

        UID uidTarget = INVALID_UID;
        switch (m_data.nLocType)
        {
        case DISTANCE_DAMAGE_LOC_SELF:
            {
                uidTarget = context->pEntity->getUID();
            }
            break;
        case DISTANCE_DAMAGE_LOC_TARGET:
            {
                uidTarget = context->uidTarget;
            }
            break;
        case DISTANCE_DAMAGE_LOC_OPERATOR:
            {
                uidTarget = context->uidOperator;
            }
            break;
        default:
            {
                ErrorLn("CEffectDistanceDamage error nLocType="<<m_data.nLocType);
                return false;
            }
            break;
        }
        if (isInvalidUID(uidTarget))
        {
            return false;
        }

        // 取得目标位置
        Vector3 vTargetLoc = getLocation(uidTarget);
        // 算出距离
        float fDistance = vStartLoc.getDistance(vTargetLoc);
        float fSpellDistance = (float)pSpell->getSpellData( SPDATA_DISTANCE )/SPELL_FLOAT_VALUE;
        // 判断长度
        if (fSpellDistance <= 0.01f)
        {
            return false;
        }
        if (fDistance >= fSpellDistance)
        {
            fDistance = fSpellDistance;
        }
        // 距离系数
        float fDistanceFactor = fDistance/fSpellDistance;

        int nLevel = getProperty_Integer(uidTarget, PROPERTY_LEVEL);

        float fMinDamageHP = m_data.nMinDamageHP + m_data.nMinLevelFactor/SPELL_FLOAT_VALUE*nLevel;
        float fMaxDamageHP = m_data.nMaxDamageHP + m_data.nMaxLevelFactor/SPELL_FLOAT_VALUE*nLevel;

        int nDamageHp = (int)(m_data.nBaseDamageHP + nPropertyValue*m_data.fMinFactor + 
            (fMaxDamageHP - fMinDamageHP)*fDistanceFactor + 
            nPropertyValue*(m_data.fMaxFactor - m_data.fMinFactor)*fDistanceFactor);

        // 伤害消息
        msg_entity_damage damage;
        damage.nID = context->nID;
        damage.uidOperator = uidProperty;
        damage.nDamageType = (m_data.nDamageType|DAMAGE_TYPE_EFFECT);
        damage.nDeadType = m_data.nDeadType;
        damage.nSpellEffectType = m_data.nSpellEffectType;
        damage.nDamageHP = nDamageHp;
        damage.nDamageBonus = getProperty_Integer(uidProperty, PROPERTY_DAMAGE_BONUS);
        damage.fDamageBonusPCT = getProperty_Integer(uidProperty, PROPERTY_DAMAGE_BONUS_PCT)/ZOOM_IN_MULTIPLE;
        damage.fAppendPCTPDP = static_cast<float>(getProperty_Integer(uidProperty, PROPERTY_APPEND_PCT_PDP)) / ZOOM_IN_MULTIPLE;      // 附加额外百分比护甲穿透
        damage.fAppendPCTPMP = static_cast<float>(getProperty_Integer(uidProperty, PROPERTY_APPEND_PCT_MDP)) / ZOOM_IN_MULTIPLE;      // 附加额外百分比魔抗穿透

        // 发送实体消息
        g_EHelper.sendEntityMessage(uidTarget, PART_DAMAGE, DAMAGE_MSG_DAMAGE, (char *)&damage, sizeof(damage));

        return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectDistanceDamage(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA               m_data;
};
