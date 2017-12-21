/*******************************************************************
** 文件名:	EffectContinuousAttackBeHitAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李建伟
** 日  期:	2/8/2017
** 版  本:	1.0
** 描  述:	效果-持续攻击/被击增加buff
需求描述：http://172.16.0.120/redmine/issues/3576
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

class CEffectContinuousAttackBeHitAddBuff : public IEffectEx,public IEventExecuteSink,public TimerHandler
{
public:
	typedef  EffectServer_ContinuousAttackBeHitAddBuff    SCHEME_DATA;

	CEffectContinuousAttackBeHitAddBuff( SCHEME_DATA & data ) : m_data(data),m_pEntity(0),m_nCount(0)
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

        if( m_data.nAttackOrBeHit==AOB_ATTACK )
        {
            g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_ATTACK, "CEffectContinuousAttackBeHitAddBuff");
        }
        else if( m_data.nAttackOrBeHit==AOB_BEHIT )
        {
            g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_DAMAGE, "CEffectContinuousAttackBeHitAddBuff");
        }

        m_nCount = 0;

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
            if( m_data.nAttackOrBeHit==AOB_ATTACK )
            {
                g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_ATTACK);
            }
            else if( m_data.nAttackOrBeHit==AOB_BEHIT )
            {
                g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_DAMAGE);
            }

			m_pEntity = 0;
		}

        if(m_nCount > 0)
        {
            g_EHelper.KillTimer(0, this);
        }
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectContinuousAttackBeHitAddBuff(m_data);
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
		case EVENT_ENTITY_ATTACK:	// 攻击事件
            {
                event_entity_attack* pEvent = (event_entity_attack*)pszContext;

                // 目标过滤
                if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetFilter, pEvent->uidTarget))
                {
                    return;
                }

                // 效果类型过滤
                if ((m_data.nSpellEffectType & pEvent->nSpellEffectType) != m_data.nSpellEffectType)
                {
                    return;
                }

                // 增加攻击计数
                addCount();
            }
            break;
        case EVENT_ENTITY_DAMAGE:       // 被击事件
            {
                event_entity_damage* pEvent = (event_entity_damage*)pszContext;

                // 目标过滤
                if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetFilter, pEvent->uidOperator))
                {
                    return;
                }

                // 效果类型过滤
                if ((m_data.nSpellEffectType & pEvent->nSpellEffectType) != m_data.nSpellEffectType)
                {
                    return;
                }

                // 增加受击次数
                addCount();
            }
            break;
		default:
			break;
		}
	}

    virtual void OnTimer( unsigned long dwTimerID )
    {
        m_nCount = 0;
    }

    void addCount()
    {
        ++m_nCount;

        // 连续次数够了加buff
        if(m_nCount >= m_data.nTimes)
        {
            m_nCount = 0;
            AddBuff(m_pEntity->getUID(), m_data.nBuffID, m_data.nBuffLevel, m_pEntity->getUID());
        }

        // 重启清零timer
        if(m_nCount > 0)
        {
            g_EHelper.KillTimer(0, this);
            g_EHelper.SetTimer(0, m_data.nCountTimeOut, this, 1, __FUNCTION__);
        }
    }

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;

    int                     m_nCount;           // 计数
};
