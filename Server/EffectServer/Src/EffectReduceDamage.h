/*******************************************************************
** 文件名:	EffectReduceDamage.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李建伟
** 日  期:	7/17/2015
** 版  本:	1.0
** 描  述:	效果-抵挡伤害效果
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
#include "IBuffServer.h"

class CEffectReduceDamage : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_ReduceDamage    SCHEME_DATA;

	CEffectReduceDamage( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_dwLastTick(0), m_nCurTimes(0)
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

        // 注册准备伤害
        g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_DAMAGE, "CEffectReduceDamage");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
            // 注销伤害事件
            g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_DAMAGE);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectReduceDamage(m_data);
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
				if (m_pEntity == 0)
				{
					return;
				}
				if (pszContext == NULL || nLen != sizeof(DamageCalcContext))
				{
					return;
				}

				// 抵挡一次伤害
				DamageCalcContext *pDamage = (DamageCalcContext *)pszContext;
				// 真实伤害不抵挡
				if ((pDamage->nDamageType&DAMAGE_TYPE_TURE_DAMAGE) == DAMAGE_TYPE_TURE_DAMAGE)
				{
					return;
				}
				if (pDamage->nDamageHP <= 0)
				{
					return;
				}

				// 如果还未触发，判断是否触发
				if(m_nCurTimes == 0)
				{
					// 判断冷却时间
					DWORD dwTick = getTickCountEx();
					if(dwTick < m_dwLastTick + m_data.nCD)
					{
						return;
					}

					// 判断概率
                    int nRandNum = 0;
                    rktRangeRand(nRandNum, 1, 100);
					if(nRandNum > m_data.fRate * 100)
					{
						return;
					}
				}

				int nSpellEffectType = pDamage->nSpellEffectType;
				// 判断技能效果类型
				if ((m_data.nSpellEffectType & nSpellEffectType) != m_data.nSpellEffectType)
				{
					return;
				}

				UID uidSelf = m_pEntity->getUID();

				UID uidOperator = pDamage->uidOperator;
				if (isInvalidUID(uidOperator))
				{
					return;
				}

				// 检测操作者目标
				if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetFilter, uidOperator))
				{
					return;
				}
				if ((pDamage->nDamageType&m_data.nDamageType) == 0)
				{
					return;
				}

				int nDamageHP = pDamage->nDamageHP;

				switch (m_data.nValueType)
				{
				case MA_VALUE_NUM:
					{
						nDamageHP -= (int)m_data.fValue;
					}
					break;
				case MA_VALUE_PER:
					{
						nDamageHP -= (int)(m_data.fValue * nDamageHP);
					}
					break;
				default:
					{
						ErrorLn("CEffectReduceDamage error nValueType="<<m_data.nValueType);
						return;
					}
					break;
				}

				if(nDamageHP < 0)
				{
					nDamageHP = 0;
				}

				pDamage->nDamageHP = nDamageHP;

				// 递增抵挡次数
				++m_nCurTimes;

				// 如果抵挡次数满了，本轮结束
				if(m_nCurTimes >= m_data.nTimes)
				{
					// 配置增加BUFF
					if (m_data.nBuffID > 0 && m_data.nBuffLvel > 0)
					{
                        SBuffContext BuffContext;
                        BuffContext.nID = pDamage->nID;
						// 增加BUFF
						AddBuff(uidSelf, m_data.nBuffID, m_data.nBuffLvel, uidSelf, 0, &BuffContext, sizeof(BuffContext));
					}
					m_nCurTimes = 0;

					// 进入冷却
					m_dwLastTick = getTickCountEx();
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

    // 已经抵挡次数
    int                     m_nCurTimes;       

	// 上次时间
	DWORD					m_dwLastTick;
};
