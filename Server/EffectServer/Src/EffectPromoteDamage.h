/*******************************************************************
** 文件名:	EffectPromoteDamage.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	5/24/2016
** 版  本:	1.0
** 描  述:	效果-提升伤害
需求描述：http://172.16.0.120/redmine/issues/1820
实现描述：提升百分比技能伤害
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"
#include "EventDef.h"
#include "ISpell.h"
#include "DamageDef.h"

class CEffectPromoteDamage : public IEffectEx,public IEventVoteSink
{
public:
	typedef  EffectServer_PromoteDamage    SCHEME_DATA;

	CEffectPromoteDamage( SCHEME_DATA & data ) : m_data(data),m_pEntity(0)
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

		// 注册投票事件
		g_EHelper.Subscibe(m_pEntity, this, (EventKey)m_data.nPromoteDamageType, "CEffectPromoteDamage");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			// 注销投票事件
			g_EHelper.UnSubscibe(m_pEntity, this, (EventKey)m_data.nPromoteDamageType);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectPromoteDamage(m_data);
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
    @return  如果返回false : 则中断执行，否则继续向下执行
    @note     
    @warning 
    @retval buffer 
    */
    virtual bool		OnVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
		switch (wEventID)
		{
		case EVENT_ENTITY_PREPARE_DAMAGE:	// 准备伤害事件
			{
				if (m_pEntity == NULL)
				{
					return false;
				}

				if (pszContext == NULL || nLen != sizeof(DamageCalcContext))
				{
					return false;
				}
				DamageCalcContext *pDamage = (DamageCalcContext *)pszContext;

				// 技能效果类型匹配
				if ((m_data.nSpellEffectType & pDamage->nSpellEffectType) == 0)
				{
					return true;
				}
                
                switch (m_data.nValueType)
                {
                case MA_VALUE_NUM:
                    {
                        pDamage->nDamageHP += (int)m_data.fValue;
                    }
                    break;
                case MA_VALUE_PER:
                    {
                        pDamage->nDamageHP = (int)(pDamage->nDamageHP*(1 + m_data.fValue));
                    }
                    break;
                default:
                    {
                        ErrorLn("CEffectPromoteDamage error nValueType="<<m_data.nValueType);
                        return true;
                    }
                    break;
                }
			}
			break;

        case EVENT_ENTITY_ATTACK:   // 攻击事件
            {
                if (m_pEntity == NULL)
                {
                    return false;
                }

                if (pszContext == NULL || nLen != sizeof(event_entity_attack))
                {
                    return false;
                }
                event_entity_attack *pAttack = (event_entity_attack *)pszContext;

                // 技能效果类型匹配
                if ((m_data.nSpellEffectType & pAttack->nSpellEffectType) == 0)
                {
                    return true;
                }

                switch (m_data.nValueType)
                {
                case MA_VALUE_NUM:
                case MA_VALUE_PER:
                    {
                        pAttack->fPromoteFactor = m_data.fValue;
                    }
                    break;
                default:
                    {
                        ErrorLn("CEffectPromoteDamage error nPromoteDamageType="<<m_data.nPromoteDamageType);
                        return true;
                    }
                    break;
                }
            }
            break;
		default:
			break;
		}

        return true;
	}

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;
};
