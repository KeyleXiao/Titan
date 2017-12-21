/*******************************************************************
** 文件名:	EffectBLDDamage.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/12/2016
** 版  本:	1.0
** 描  述:	效果-布兰登伤害
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;
#include "TimerHandler.h"
#include "DamageDef.h"
#include "EntityHelper.h"

class CEffectBLDDamage : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_BLDDamage SCHEME_DATA;

	CEffectBLDDamage( SCHEME_DATA & data ) : m_data(data),m_pEntity(0),m_dwTick(0)
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
		if(context == 0 || context->uidOperator == NULL )
			return false;

        m_pEntity = context->pEntity;

        // 注册准备伤害
        g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_DAMAGE, "CEffectBLDDamage");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        if (m_pEntity != 0)
        {
            // 注销准备伤害
            g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_DAMAGE);

            m_pEntity = 0;
        }
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectBLDDamage(m_data);
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
		case EVENT_ENTITY_PREPARE_DAMAGE:
			{
				if (pszContext == NULL || nLen != sizeof(DamageCalcContext))
				{
					return;
				}
				DamageCalcContext *pDamage = (DamageCalcContext *)pszContext;

                int	nSpellEffectType = pDamage->nSpellEffectType;
                // 技能效果类型匹配
                if ((m_data.nSpellEffectType & nSpellEffectType) != m_data.nSpellEffectType)
                {
                    return;
                }

                DWORD dwTick = getTickCountEx();
                if (dwTick <= m_dwTick + m_data.nCoolTime)
                {
                    // 设置免疫
                    pDamage->nAttackResultType |= AttackResult_Immune;
                    return;
                }
				
                UID uidTarget = pDamage->uidTarget;
                int nDamageHP = 0;
                switch (m_data.nValueType)
                {
                case MA_VALUE_NUM:
                    {
                        nDamageHP = (int)m_data.fValue;
                    }
                    break;
                case MA_VALUE_PER:
                    {
                        int nMaxHP = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
                        nDamageHP = (int)(m_data.fValue*nMaxHP);
                    }
                    break;
                case MA_VALUE_LOSE_PER:
                    {
                        int nHP = getProperty_Integer(uidTarget, PROPERTY_HP);
                        int nMaxHP = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
                        nDamageHP = (int)(m_data.fValue*(nMaxHP-nHP));
                    }
                    break;
                default:
                    {
                        ErrorLn("CEffectBLDDamage error nValueType="<<m_data.nValueType);
                        return;
                    }
                    break;
                }

                pDamage->nDamageHP = nDamageHP;

                m_dwTick = dwTick;
			}
			break;
		default:
			break;
		}
	}

private:
	SCHEME_DATA               m_data;

    // 实体指针
    __IEntity				  *m_pEntity;

    // 冷却时间
    DWORD                     m_dwTick;
};
