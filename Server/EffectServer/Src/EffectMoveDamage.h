/*******************************************************************
** 文件名:	EffectMoveDamage.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	2/26/2016
** 版  本:	1.0
** 描  述:	效果-移动伤害
            http://172.16.0.120/redmine/issues/1655
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;

class CEffectMoveDamage : public IEffectEx, public rkt::TimerHandler
{
public:
	typedef  EffectServer_MoveDamage    SCHEME_DATA;

	CEffectMoveDamage( SCHEME_DATA & data ) : m_data(data),m_pEntity(NULL),m_uidOperator(INVALID_UID),m_uidTarget(INVALID_UID),m_vLastLoc(0,0,0),m_nSpellID(0)
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
			return false;

        m_pEntity = context->pEntity;

        // 上次位置
        m_vLastLoc = m_pEntity->getLocation();

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

        if(m_data.nCount >= 1)
        {
            g_EHelper.SetTimer(0, m_data.nInterval, this, m_data.nCount, "CEffectContinuedSetProperty");
        }
        else
        {
            g_EHelper.SetTimer(0, m_data.nInterval, this, INFINITY_CALL, "CEffectContinuedSetProperty");
        }

        return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        if (m_pEntity != NULL)
        {
            g_EHelper.KillTimer(0, this);

            m_pEntity = NULL;
        }
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectMoveDamage(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

    virtual void OnTimer( unsigned long dwTimerID )
    {
        if (m_pEntity == NULL)
        {
            return;
        }
        // 取得玩家现在位置
        Vector3 vLoc = m_pEntity->getLocation();
        float fDistance = vLoc.getDistance(m_vLastLoc);
        // 距离太小不处理
        if (fDistance <= 0.1f)
        {
            return;
        }
        m_vLastLoc = vLoc;

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
                ErrorLn("CEffectMoveDamage effect nID="<< GetID() <<"m_data.nTagetType"<<m_data.nTagetType);
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
                ErrorLn("CEffectMoveDamage effect nID="<< GetID() <<"nExtraAttackType error"<<m_data.nExtraAttackType);
                return;
            }
            break;
        }

        int nSpellAttackPCT = (int)(m_data.nSpellAttackPCT*fDistance);

        switch (m_data.nType)
        {
        case MOVE_DAMAGE_HP:
            {
                int nPenetration = 0;
                int nPenetrationPCT = 0;
                int nDamageType = m_data.nDamageType;
                if ((nDamageType&DAMAGE_TYPE_PHYSICS) == DAMAGE_TYPE_PHYSICS)
                {
                    nPenetration = getProperty_Integer(uidTarget, PROPERTY_PDP);
                    nPenetrationPCT = getProperty_Integer(uidTarget, PROPERTY_PCT_PDP);
                }
                else if ((nDamageType&DAMAGE_TYPE_MAGIC) == DAMAGE_TYPE_MAGIC)
                {
                    nPenetration = getProperty_Integer(uidTarget, PROPERTY_MDP);
                    nPenetrationPCT = getProperty_Integer(uidTarget, PROPERTY_PCT_MDP);
                }

                // 伤害消息
                msg_entity_damage damage;
                damage.nID = m_nSpellID;
                damage.uidOperator = m_uidOperator;
                damage.nDamageType = m_data.nDamageType;
                damage.nAttack = nAttack;
                damage.nPenetration = nPenetration;
                damage.nPenetrationPCT = nPenetrationPCT;
                damage.nSpellPower= m_data.nSpellPower;
                damage.nLevel = getProperty_Integer(uidTarget, PROPERTY_LEVEL);
                damage.nLevelFactor = m_data.nLevelFactor;
                damage.nSpellAttackPCT = nSpellAttackPCT;
                damage.nDeadType = m_data.nDeadType;
                damage.nSpellEffectType = m_data.nSpellEffectType;
                damage.fAttackFactor = (float)getProperty_Integer(m_uidOperator, PROPERTY_AF);
                damage.nPASD         = getProperty_Integer(m_uidOperator, PROPERTY_PASD);
                damage.nDamageBonus = getProperty_Integer(m_uidOperator, PROPERTY_DAMAGE_BONUS);
                damage.fDamageBonusPCT = getProperty_Integer(m_uidOperator, PROPERTY_DAMAGE_BONUS_PCT)/ZOOM_IN_MULTIPLE;
                damage.fAppendPCTPDP = static_cast<float>(getProperty_Integer(m_uidOperator, PROPERTY_APPEND_PCT_PDP)) / ZOOM_IN_MULTIPLE;      // 附加额外百分比护甲穿透
                damage.fAppendPCTPMP = static_cast<float>(getProperty_Integer(m_uidOperator, PROPERTY_APPEND_PCT_MDP)) / ZOOM_IN_MULTIPLE;      // 附加额外百分比魔抗穿透


                // 发送实体消息
                g_EHelper.sendEntityMessage(m_uidTarget, PART_DAMAGE, DAMAGE_MSG_DAMAGE, (char *)&damage, sizeof(damage));
            }
            break;
        case MOVE_DAMAGE_MP:
            {
                int nMp = m_data.nSpellPower + nAttack*nSpellAttackPCT;
                if (nMp <= 0)
                {
                    break;
                }

                // 减少魔法
                addProperty_Integer(m_pEntity->getUID(), PROPERTY_MP, -nMp);
            }
            break;
        default:
            break;
        }
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
        if (IsMainMap(nMapID) )
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

    __IEntity                 *m_pEntity;

    // 操作者UID
    UID						  m_uidOperator;
    // 目标UID
    UID						  m_uidTarget;

    // 上次位置
    Vector3                   m_vLastLoc;

    // 技能ID
    int                       m_nSpellID;
};
