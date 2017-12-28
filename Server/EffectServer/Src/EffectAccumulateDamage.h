/*******************************************************************
** 文件名:	EffectAccumulateDamage.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	11/27/2015
** 版  本:	1.0
** 描  述:	效果-累积伤害
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

class CEffectAccumulateDamage : public IEffectEx,public IEventExecuteSink, public TimerHandler
{
public:
	typedef  EffectServer_AccumulateDamage    SCHEME_DATA;

	CEffectAccumulateDamage( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_uidAddBuff(0),m_dwStartTime(0),m_nTotalDamage(0),m_nSpellID(0)
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

        m_nSpellID = context->nID;

        // 此效果必定是buff调用,记录Buff增加者
        m_uidAddBuff = context->uidOperator;

        if (isInvalidUID(m_uidAddBuff))
        {
            return false;
        }

        m_dwStartTime = getTickCountEx();
        
        // 初始化伤害血量
        m_nTotalDamage = 0;

		// 注册伤害事件
		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_DAMAGE, "CEffectAccumulateDamage");

        g_EHelper.SetTimer(0, 500, this, INFINITY_CALL, "CEffectAccumulateDamage");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
            g_EHelper.KillTimer(0, this);

			// 注销伤害事件
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_DAMAGE);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectAccumulateDamage(m_data);
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
					break;
				}
				if (pszContext == NULL || nLen != sizeof(event_entity_damage))
				{
					break;
				}
				event_entity_damage *pDamage = (event_entity_damage*)pszContext;

				// 自己UID
				UID uidSelf = m_pEntity->getUID();
				// 是自己伤害自己不处理
				if (uidSelf == pDamage->uidOperator)
				{
					break;
				}
                // 不是增加此buff的人造成伤害不处理
                if (pDamage->uidOperator != m_uidAddBuff)
                {
                    break;
                }

                m_nTotalDamage += pDamage->nDamageHP;
			}
			break;
		default:
			break;
		}
	}

    virtual void OnTimer( unsigned long dwTimerID )
    {
        if (m_pEntity == NULL)
        {
            return;
        }
        if (getTickCountEx() - m_dwStartTime < (DWORD)m_data.nTime)
        {
            return;
        }
        g_EHelper.KillTimer(0, this);
        // 注销伤害事件
        g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_DAMAGE);

        if (isInvalidUID(m_uidAddBuff))
        {
            return;
        }

        // 检测目标
        if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetFilter, m_uidAddBuff))
        {
            return;
        }

        // 伤害消息
        msg_entity_damage damage;
        damage.nID = m_nSpellID;
        damage.uidOperator = m_uidAddBuff;
        damage.nDamageType = (m_data.nDamageType|DAMAGE_TYPE_EFFECT);
        damage.nDamageHP = (int)(m_nTotalDamage*m_data.fValue);
        damage.nSpellEffectType = m_data.nSpellEffectType;
        damage.fAppendPCTPDP = static_cast<float>(getProperty_Integer(m_uidAddBuff, PROPERTY_APPEND_PCT_PDP)) / ZOOM_IN_MULTIPLE;      // 附加额外百分比护甲穿透
        damage.fAppendPCTPMP = static_cast<float>(getProperty_Integer(m_uidAddBuff, PROPERTY_APPEND_PCT_MDP)) / ZOOM_IN_MULTIPLE;      // 附加额外百分比魔抗穿透
        damage.nUseFlag = m_pEntity->getUseFlag()->getAll();    // 用途标识

        // 发送实体消息
        g_EHelper.sendEntityMessage(m_pEntity->getUID(), PART_DAMAGE, DAMAGE_MSG_DAMAGE, (char *)&damage, sizeof(damage));
    }

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;

    // 增加buff者
    UID                     m_uidAddBuff;

    // 总共伤害
    int                     m_nTotalDamage;

    // 开始时间
    DWORD                   m_dwStartTime;

    // 技能ID
    int                     m_nSpellID;
};
