/*******************************************************************
** 文件名:	EffectEvade.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/21/2015
** 版  本:	1.0
** 描  述:	效果-躲闪
需求描述：http://172.16.0.120/redmine/issues/788
实现描述：指定类型技能可概率躲闪，不可躲闪命中技能
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

class CEffectEvade : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_Evade    SCHEME_DATA;

	CEffectEvade( SCHEME_DATA & data ) : m_data(data),m_pEntity(0)
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

		// 注册准备伤害事件
		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_DAMAGE, "CEffectEvade");

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
		return new CEffectEvade(m_data);
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
		case EVENT_ENTITY_PREPARE_DAMAGE:	// 准备伤害事件
			{
				if (m_pEntity == NULL)
				{
					return;
				}

				if (pszContext == NULL || nLen != sizeof(DamageCalcContext))
				{
					return;
				}

				DamageCalcContext *pDamage = (DamageCalcContext *)pszContext;
				// 必命中,就不躲闪了
				if ((pDamage->nAttackResultType&AttackResult_Accuracy) == AttackResult_Accuracy)
				{
					return;
				}
				if (pDamage->nID <= 0)
				{
					return;
				}
				// 技能效果类型匹配
				if ((m_data.nSpellEffectType & pDamage->nSpellEffectType) != m_data.nSpellEffectType)
				{
					return;
				}
				// 计算概率
				int nRandNum = 0;
				rktRangeRand(nRandNum, 1, EFFECT_PROB_VALUE);
				if(nRandNum > m_data.nProb)
				{
					return;
				}
				pDamage->nAttackResultType |= AttackResult_Evade;
                UID uidSelf=m_pEntity->getUID();

                SBuffContext BuffContext;
                BuffContext.nID = pDamage->nID;

                if(m_data.nBuffAddTarget==0)
                {
                    AddBuff(uidSelf,m_data.nBuffID,m_data.nBuffLevel,uidSelf, 0, &BuffContext, sizeof(BuffContext));
                }
                else
                {
                    UID uidTarget=pDamage->uidOperator;
                    AddBuff(uidTarget,m_data.nBuffID,m_data.nBuffLevel,uidSelf, 0, &BuffContext, sizeof(BuffContext));
                }
                
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
};
