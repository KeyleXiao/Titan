/*******************************************************************
** 文件名:	EffectAddBuffOnDamage.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李建伟
** 日  期:	7/16/2015
** 版  本:	1.0
** 描  述:	效果-伤害时概率加buff
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
#include "EventDef.h"
#include "ISpell.h"

class CEffectAddBuffOnDamage : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_AddBuffOnDamage    SCHEME_DATA;

	CEffectAddBuffOnDamage( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_dwLastTick(0)
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

        // 注册伤害事件
        g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_DAMAGE, "CEffectAddDebuffOnDamage");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
            // 注销伤害事件
            g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_DAMAGE);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectAddBuffOnDamage(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
		switch (wEventID)
		{
		case EVENT_ENTITY_DAMAGE:	// 伤害事件
			{
                if (pszContext == NULL || nLen != sizeof(event_entity_damage))
                {
                    return;
                }
                if (m_pEntity == NULL)
                {
                    return;
                }

                // 是否在CD
                DWORD dwCurrentTick = getTickCountEx();
                if (dwCurrentTick < m_dwLastTick + m_data.nCD)
                {
                    return;
                }

                // 概率是否命中
                int nRandNum = 0;
                rktRangeRand(nRandNum, 1, 100);
                if(nRandNum > m_data.fRate * 100)
                {
                    return;
                }

                event_entity_damage *pDamage = (event_entity_damage*)pszContext;

                int nSpellEffectType = pDamage->nSpellEffectType;
                // 判断技能效果类型
                if ((m_data.nSpellEffectType & nSpellEffectType) != m_data.nSpellEffectType)
                {
                    return;
                }

				UID uidOperator = pDamage->uidOperator;
				if (isInvalidUID(uidOperator))
				{
					return;
				}

				UID uidTarget = pDamage->uidTarget;
				if (isInvalidUID(uidTarget))
				{
					return;
				}

				// 检测目标
				if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetFilter, uidOperator))
				{
					return;
				}
				if ((pDamage->nDamageType&m_data.nDamageType) == 0)
				{
					return;
				}

                SBuffContext BuffContext;
                BuffContext.nID = pDamage->nID;

                UID uidAdd = INVALID_UID;
                // buff添加者
                if(m_data.nAddType == MA_TAGET_TO_SELF)
                {
                    // 被伤害时给自己加buff 自己就是伤害目标
                    uidAdd = pDamage->uidTarget; 
                }
                else
                {
                    // 被伤害时给伤害发出者加buff
                    uidAdd = pDamage->uidOperator; 
                }

                // 加buff
                if(m_data.nTagetType == MA_TAGET_TO_SELF)
                {
                    // 被伤害时给自己加buff 自己就是伤害目标
                    AddBuff( pDamage->uidTarget, m_data.nBuffID, m_data.nBuffLevel, uidAdd, 0, &BuffContext, sizeof(BuffContext) );
                }
                else if(m_data.nTagetType == MA_TAGET_TO_TARGET)
                {
                    // 被伤害时给伤害发出者加buff
                    AddBuff( pDamage->uidOperator, m_data.nBuffID, m_data.nBuffLevel, uidAdd, 0, &BuffContext, sizeof(BuffContext) );
                }

                m_dwLastTick = dwCurrentTick;
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
