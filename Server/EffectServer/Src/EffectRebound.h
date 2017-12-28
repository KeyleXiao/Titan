/*******************************************************************
** 文件名:	EffectRebound.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/15/2015
** 版  本:	1.0
** 描  述:	效果-反弹伤害
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"
#include "DamageDef.h"
#include "EntityHelper.h"

class CEffectRebound : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_Rebound    SCHEME_DATA;

	CEffectRebound( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_dwLastTick(0)
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

		// 注册反弹类型事件
		g_EHelper.Subscibe(m_pEntity, this,(EventKey) m_data.nReboundType, "CEffectRebound");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			// 注销反弹类型事件
			g_EHelper.UnSubscibe(m_pEntity, this, (EventKey)m_data.nReboundType);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectRebound(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
	/** 
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
		switch (wEventID)
		{
		case EVENT_ENTITY_DAMAGE:	// 伤害事件
			{
				if (m_pEntity == NULL)
				{
					return;
				}
				DWORD dwTick = getTickCountEx();
				if (dwTick < m_dwLastTick + m_data.nInterval)
				{
					return;
				}
				if (pszContext == NULL || nLen != sizeof(event_entity_damage))
				{
					return;
				}
				event_entity_damage *pDamage = (event_entity_damage*)pszContext;

				// 自己UID
				UID uidSelf = m_pEntity->getUID();
				// 是自己不要反弹了
				if (uidSelf == pDamage->uidOperator)
				{
					return;
				}
				int nDamageType = pDamage->nDamageType;
				// 反弹过来的伤害，不再反弹
				if ((nDamageType&DAMAGE_TYPE_REBOUND) == DAMAGE_TYPE_REBOUND ||
                    pDamage->bRebound)
				{
					return;
				}
				int nSpellEffectType = pDamage->nSpellEffectType;
				// 判断技能效果类型
				if ((m_data.nSpellEffectType & nSpellEffectType) != m_data.nSpellEffectType)
				{
					return;
				}

                UID uidTarget = INVALID_UID;
                switch (m_data.nTagetType)
                {
                case MA_TAGET_TO_SELF:
                    {
                        uidTarget = m_pEntity->getUID();
                    }
                    break;
                case MA_TAGET_TO_TARGET:
                    {
                        uidTarget = pDamage->uidTarget;
                    }
                    break;
                case MA_TAGET_TO_OPERATOR:
                    {
                        uidTarget = pDamage->uidOperator;
                    }
                    break;
                default:
                    {
                        ErrorLn("CEffectRebound effect nID="<< GetID() <<"m_data.nTagetType"<<m_data.nTagetType);
                        return;
                    }
                    break;
                }
                if (isInvalidUID(uidTarget))
                {
                    return;
                }

                int nExtra = 0;
                switch (m_data.nExtraAttackType)
                {
                case EXTRAATTACK_PA:
                    {
                        nExtra = getProperty_Integer(uidTarget, PROPERTY_PA);
                    }
                    break;
                case EXTRAATTACK_MA:
                    {
                        nExtra = getProperty_Integer(uidTarget, PROPERTY_MA);
                    }
                    break;
                case EXTRAATTACK_LEVEL:
                    {
                        nExtra = getProperty_Integer(uidTarget, PROPERTY_LEVEL);
                    }
                    break;
                case EXTRAATTACK_HP:
                    {
                        nExtra = getProperty_Integer(uidTarget, PROPERTY_HP);
                    }
                    break;
                case EXTRAATTACK_MAX_HP:
                    {
                        nExtra = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
                    }
                    break;
                case EXTRAATTACK_LOSE_HP:
                    {
                        int nHp = getProperty_Integer(uidTarget, PROPERTY_HP);
                        int nMaxHp = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
                        nExtra = nMaxHp-nHp;
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
                        nExtra = (int)(fPercent*EFFECT_RATE);
                    }
                    break;
                case EXTRAATTACK_MP:
                    {
                        nExtra = getProperty_Integer(uidTarget, PROPERTY_MP);
                    }
                    break;
                case EXTRAATTACK_MAX_MP:
                    {
                        nExtra = getProperty_Integer(uidTarget, PROPERTY_MAX_MP);
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
                        nExtra = (int)(fPercent*EFFECT_RATE);
                    }
                    break;
                case EXTRAATTACK_MD:
                    {
                        nExtra = getProperty_Integer(uidTarget, PROPERTY_MD);
                    }
                    break;
                case EXTRAATTACK_PD:
                    {
                        nExtra = getProperty_Integer(uidTarget, PROPERTY_PD);
                    }
                    break;
                case EXTRAATTACK_SD:
                    {
                        nExtra = getProperty_Integer(uidTarget, PROPERTY_SD);
                    }
                    break;
                default:
                    {
                        ErrorLn("CEffectRebound effect nID="<< GetID() <<"nExtraAttackType error"<<m_data.nExtraAttackType);
                        return;
                    }
                    break;
                }

				int nDamageHP = 0;
				switch (m_data.nValueType)
				{
				case MA_VALUE_NUM:
					{
						nDamageHP = m_data.nValue + (int)(nExtra*m_data.fExtraFactor);
					}
					break;
				case MA_VALUE_PER:
					{
						if (m_data.nInitialDamage == MA_TRUE)
						{
							nDamageHP = (int)(m_data.nValue/EFFECT_RATE*pDamage->nInitialDamageHP + nExtra*m_data.fExtraFactor);
						}
						else
						{
							nDamageHP = (int)(m_data.nValue/EFFECT_RATE*pDamage->nDamageHP + nExtra*m_data.fExtraFactor);
						}
					}
					break;
				default:
					{
						ErrorLn("CEffectRebound error nValueType="<<m_data.nValueType);
						return;
					}
					break;
				}

				// 伤害消息
				msg_entity_damage damage;
                damage.nID = pDamage->nID;
				damage.uidOperator = uidSelf;
				damage.nDamageType = (m_data.nDamageType|DAMAGE_TYPE_REBOUND);
                // 反弹出去的伤害效果类型SPELL_EFFECT_TYPE
				damage.nSpellEffectType = m_data.nMySpellEffectType; /*m_data.nSpellEffectType;*/
				damage.nDamageHP = nDamageHP;
                // 伤害加成(固定值)
                damage.nDamageBonus = getProperty_Integer(uidSelf, PROPERTY_DAMAGE_BONUS);
                // 伤害加成百分比
                damage.fDamageBonusPCT = getProperty_Integer(uidSelf, PROPERTY_DAMAGE_BONUS_PCT)/ZOOM_IN_MULTIPLE;
                // 设置反弹伤害
                damage.bRebound = true;
                // 技能威力
                damage.nSpellPower = nDamageHP;

                if ( (nDamageType&DAMAGE_TYPE_PHYSICS) == DAMAGE_TYPE_PHYSICS)
                {
                    // 物理防御穿透
                    damage.nPenetration = static_cast<float>(getProperty_Integer(uidSelf, PROPERTY_PDP));
                    // 百分比物理防御穿透
                    damage.nPenetrationPCT = static_cast<float>(getProperty_Integer(uidSelf, PROPERTY_PCT_PDP));
                }
                else if ((nDamageType&DAMAGE_TYPE_MAGIC) == DAMAGE_TYPE_MAGIC)
                {
                    damage.nPenetration = static_cast<float>(getProperty_Integer(uidSelf, PROPERTY_MDP));
                    // 百分比法术防御穿透
                    damage.nPenetrationPCT = static_cast<float>(getProperty_Integer(uidSelf, PROPERTY_PCT_MDP));
                }
                // 附加额外百分比护甲穿透
                damage.fAppendPCTPDP = static_cast<float>(getProperty_Integer(uidSelf, PROPERTY_APPEND_PCT_PDP)) / ZOOM_IN_MULTIPLE;
                // 附加额外百分比魔抗穿透
                damage.fAppendPCTPMP = static_cast<float>(getProperty_Integer(uidSelf, PROPERTY_APPEND_PCT_MDP)) / ZOOM_IN_MULTIPLE;
                damage.nUseFlag = m_pEntity->getUseFlag()->getAll();    // 用途标识

				// 发送实体消息
				g_EHelper.sendEntityMessage(pDamage->uidOperator, PART_DAMAGE, DAMAGE_MSG_DAMAGE, (char *)&damage, sizeof(damage));

                m_dwLastTick = dwTick;
			}
			break;
        case EVENT_ENTITY_PREPARE_DAMAGE:
            {
                if (m_pEntity == NULL)
                {
                    return;
                }
                DWORD dwTick = getTickCountEx();
                if (dwTick < m_dwLastTick + m_data.nInterval)
                {
                    return;
                }
                if (pszContext == NULL || nLen != sizeof(DamageCalcContext))
                {
                    return;
                }
                DamageCalcContext *pDamage = (DamageCalcContext*)pszContext;

                // 自己UID
                UID uidSelf = m_pEntity->getUID();
                // 是自己不要反弹了
                if (uidSelf == pDamage->uidOperator)
                {
                    return;
                }
                int nDamageType = pDamage->nDamageType;
                // 反弹过来的伤害，不再反弹
                if ((nDamageType&DAMAGE_TYPE_REBOUND) == DAMAGE_TYPE_REBOUND ||
                    pDamage->bRebound)
                {
                    return;
                }
                int nSpellEffectType = pDamage->nSpellEffectType;
                // 判断技能效果类型
                if ((m_data.nSpellEffectType & nSpellEffectType) != m_data.nSpellEffectType)
                {
                    return;
                }

                UID uidTarget = INVALID_UID;
                switch (m_data.nTagetType)
                {
                case MA_TAGET_TO_SELF:
                    {
                        uidTarget = m_pEntity->getUID();
                    }
                    break;
                case MA_TAGET_TO_TARGET:
                    {
                        uidTarget = pDamage->uidTarget;
                    }
                    break;
                case MA_TAGET_TO_OPERATOR:
                    {
                        uidTarget = pDamage->uidOperator;
                    }
                    break;
                default:
                    {
                        ErrorLn("CEffectRebound effect nID="<< GetID() <<"m_data.nTagetType"<<m_data.nTagetType);
                        return;
                    }
                    break;
                }
                if (isInvalidUID(uidTarget))
                {
                    return;
                }

                int nExtra = 0;
                switch (m_data.nExtraAttackType)
                {
                case EXTRAATTACK_PA:
                    {
                        nExtra = getProperty_Integer(uidTarget, PROPERTY_PA);
                    }
                    break;
                case EXTRAATTACK_MA:
                    {
                        nExtra = getProperty_Integer(uidTarget, PROPERTY_MA);
                    }
                    break;
                case EXTRAATTACK_LEVEL:
                    {
                        nExtra = getProperty_Integer(uidTarget, PROPERTY_LEVEL);
                    }
                    break;
                case EXTRAATTACK_HP:
                    {
                        nExtra = getProperty_Integer(uidTarget, PROPERTY_HP);
                    }
                    break;
                case EXTRAATTACK_MAX_HP:
                    {
                        nExtra = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
                    }
                    break;
                case EXTRAATTACK_LOSE_HP:
                    {
                        int nHp = getProperty_Integer(uidTarget, PROPERTY_HP);
                        int nMaxHp = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
                        nExtra = nMaxHp-nHp;
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
                        nExtra = (int)(fPercent*EFFECT_RATE);
                    }
                    break;
                case EXTRAATTACK_MP:
                    {
                        nExtra = getProperty_Integer(uidTarget, PROPERTY_MP);
                    }
                    break;
                case EXTRAATTACK_MAX_MP:
                    {
                        nExtra = getProperty_Integer(uidTarget, PROPERTY_MAX_MP);
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
                        nExtra = (int)(fPercent*EFFECT_RATE);
                    }
                    break;
                case EXTRAATTACK_MD:
                    {
                        nExtra = getProperty_Integer(uidTarget, PROPERTY_MD);
                    }
                    break;
                case EXTRAATTACK_PD:
                    {
                        nExtra = getProperty_Integer(uidTarget, PROPERTY_PD);
                    }
                    break;
                case EXTRAATTACK_SD:
                    {
                        nExtra = getProperty_Integer(uidTarget, PROPERTY_SD);
                    }
                    break;
                default:
                    {
                        ErrorLn("CEffectRebound effect nID="<< GetID() <<"nExtraAttackType error"<<m_data.nExtraAttackType);
                        return;
                    }
                    break;
                }

                float fValue = m_data.nValue/EFFECT_RATE;
                int nSpellPower = 0;
                int nSpellAttackPCT = 0;
                switch (m_data.nValueType)
                {
                case MA_VALUE_NUM:
                    {
                        nSpellPower = m_data.nValue + (int)(nExtra*m_data.fExtraFactor);
                        nSpellAttackPCT = 0;
                    }
                    break;
                case MA_VALUE_PER:
                    {
                        nSpellPower = (int)(fValue*pDamage->nSpellPower + nExtra*m_data.fExtraFactor);
                        nSpellAttackPCT = pDamage->nSpellAttackPCT;
                    }
                    break;
                default:
                    {
                        ErrorLn("CEffectRebound error nValueType="<<m_data.nValueType);
                        return;
                    }
                    break;
                }

                msg_entity_damage damage;
                damage.nID = pDamage->nID;
                damage.uidOperator = uidSelf;
                damage.nDamageType = m_data.nDamageType;
                damage.nSpellEffectType = m_data.nMySpellEffectType;
                damage.nAttackCount = 0;
                damage.nAttack = (int)(pDamage->nAttack*fValue);
                damage.nPenetration = pDamage->nPenetration;
                damage.nPenetrationPCT = pDamage->nPenetrationPCT;
                damage.nSpellPower= nSpellPower;
                damage.nLevel = pDamage->nLevel;
                damage.nLevelFactor = pDamage->nLevelFactor;
                damage.nSpellAttackPCT = nSpellAttackPCT;
                damage.nDeadType = pDamage->nDeadType;
                damage.nCRD = pDamage->nCRD;
                damage.nDamageLimitType = pDamage->nDamageLimitType;
                damage.nDamageLimit = pDamage->nDamageLimit;
                damage.nDamageBonus = pDamage->nDamageBonus;
                damage.fDamageBonusPCT = pDamage->fDamageBonusPCT;
                damage.fAttackFactor = pDamage->fAttackFactor;
                damage.fPromoteFactor = pDamage->fPromoteFactor;
                damage.nPASD          = pDamage->nPASD;
                damage.nUseFlag = m_pEntity->getUseFlag()->getAll();    // 用途标识
                // 设置反弹伤害
                damage.bRebound = true;
                // 发送实体消息
                g_EHelper.sendEntityMessage(pDamage->uidOperator, PART_DAMAGE, DAMAGE_MSG_DAMAGE, (char *)&damage, sizeof(damage));

                m_dwLastTick = dwTick;
            }
            break;
		default:
			break;
		}
	}

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;

	// 上次时间
	DWORD					m_dwLastTick;
};
