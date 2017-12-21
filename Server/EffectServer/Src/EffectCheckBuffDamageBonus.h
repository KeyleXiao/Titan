/*******************************************************************
** 文件名:	EffectCheckBuffDamageBonus.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	8/30/2016
** 版  本:	1.0
** 描  述:	效果-检测BUFF伤害加成
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

class CEffectCheckBuffDamageBonus : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_CheckBuffDamageBonus    SCHEME_DATA;

	CEffectCheckBuffDamageBonus( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_uidCheck(0)
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

        switch (m_data.nTargetType)
        {
        case MA_TAGET_TO_SELF:
            {
                m_uidCheck = m_pEntity->getUID();
            }
            break;
        case MA_TAGET_TO_TARGET:
            {
                m_uidCheck = context->uidTarget;
            }
            break;
        case MA_TAGET_TO_OPERATOR:
            {
                m_uidCheck = context->uidOperator;
            }
            break;
        default:
            {
                ErrorLn("CEffectCheckBuffAddBuff error nTargetType="<<m_data.nTargetType);
                return false;
            }
            break;
        }
        if (isInvalidUID(m_uidCheck))
        {
            return false;
        }

        // 注册准备伤害事件
        g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_DAMAGE, "CEffectAddDebuffOnDamage");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
            // 注销准备伤害事件
            g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_DAMAGE);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectCheckBuffDamageBonus(m_data);
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
		case EVENT_ENTITY_PREPARE_DAMAGE:	// 准备伤害事件
			{
                if (m_pEntity == NULL)
                {
                    break;
                }

                if (pszContext == NULL || nLen != sizeof(DamageCalcContext))
                {
                    break;
                }
                DamageCalcContext *pDamage = (DamageCalcContext*)pszContext;

                // 检测目标不匹配
                if (m_uidCheck != pDamage->uidOperator)
                {
                    break;
                }

                int nSpellEffectType = pDamage->nSpellEffectType;
                // 判断技能效果类型
                if ((m_data.nSpellEffectType & nSpellEffectType) != m_data.nSpellEffectType)
                {
                    break;
                }

                // 伤害加成
                pDamage->nDamageHP += (int)(pDamage->nDamageHP * m_data.fValue);
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

    // 检测目标
    UID                     m_uidCheck;
};
