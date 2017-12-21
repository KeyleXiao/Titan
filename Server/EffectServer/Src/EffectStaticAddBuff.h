/*******************************************************************
** 文件名:	EffectStaticAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	8/26/2016
** 版  本:	1.0
** 描  述:	效果-静止增加Buff
            http://172.16.0.120/redmine/issues/2172
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"

class CEffectStaticAddBuff : public IEffectEx,public IEventExecuteSink, public TimerHandler
{
public:
	typedef  EffectServer_StaticAddBuff    SCHEME_DATA;

	CEffectStaticAddBuff( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_dwLastTick(0)
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

        m_dwLastTick = getTickCountEx();

		g_EHelper.Subscibe(m_pEntity, this, (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_LOCATION), "CEffectStaticAddBuff");
        g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_CAST_SPELL, "CEffectStaticAddBuff");
        g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_DAMAGE, "CEffectStaticAddBuff");

        g_EHelper.SetTimer(0, 1000, this, INFINITY_CALL, "CEffectStaticAddBuff");

        // 如果排除死亡状态(死亡不算静止)
        if(m_data.bExcludeDead)
            g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_CHANGE_STATE, __FUNCTION__);

        m_locLast = m_pEntity->getLocation();

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
            g_EHelper.KillTimer(0, this);

            __IBuffPart *pBuffPart = (__IBuffPart *)m_pEntity->getEntityPart(PART_BUFF);
            if (pBuffPart != NULL)
            {
                UID uid = m_pEntity->getUID();
                pBuffPart->Remove(m_data.nBuffID, uid, uid);
            }

            g_EHelper.UnSubscibe(m_pEntity, this, (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_LOCATION));
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_CAST_SPELL);
            g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_DAMAGE);

            if(m_data.bExcludeDead)
                g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_CHANGE_STATE);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectStaticAddBuff(m_data);
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
        case EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_LOCATION:
            {
                if (m_pEntity == NULL)
                {
                    break;
                }

                if (pszContext == NULL || nLen != sizeof(event_entity_property_change))
                {
                    break;
                }

                event_entity_property_change* pEvent = (event_entity_property_change*)pszContext;

                // 只关心位置更新
                if(pEvent->nPropertyID != PROPERTY_LOCATION)
                {
                    break;
                }

                // 距离过小，不打断
                Vector3 locNow;
                memcpy(&locNow, pEvent->data, pEvent->nLen);
                if (m_locLast.checkDistance(locNow, 0.1f))
                {
                    break;
                }

                m_dwLastTick = getTickCountEx();
                m_locLast = locNow; 
            }
            break;
        case EVENT_ENTITY_CHANGE_STATE:
            {
                if(m_pEntity == NULL || pszContext == NULL || nLen != sizeof(event_entity_move))
                {
                    break;
                }

                // 复活时把之前死亡的那段累积时间清掉
                event_entity_change_state* pEvent = (event_entity_change_state*)pszContext;
                if(pEvent->nOldState == ENTITY_STATE_DIE && pEvent->nState != ENTITY_STATE_DIE)
                {
                    m_dwLastTick = getTickCountEx();
                }
            }
            break;
        case EVENT_ENTITY_CAST_SPELL:
        case EVENT_ENTITY_DAMAGE:
            {
                if (m_pEntity == NULL)
                {
                    break;
                }

                m_dwLastTick = getTickCountEx();
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
        
        // 取得当时时间
        DWORD dwTick = getTickCountEx();
        if (m_dwLastTick + m_data.nInterval >= dwTick)
        {
            return;
        }

        // 如果排除飞行状态(飞行不算静止)
        if(m_data.bExcludeFlying)
        {
            __IWingPart* pWingPart = (__IWingPart*)m_pEntity->getEntityPart(PART_WING);
            if(pWingPart != NULL && pWingPart->GetWingState() != WING_STATE_INVALID) 
                return;
        }

        // 如果排除死亡状态(死亡不算静止)
        if(m_data.bExcludeDead)
        {
            if(m_pEntity->getState() == ENTITY_STATE_DIE)
                return;
        }

        __IBuffPart *pBuffPart = (__IBuffPart *)m_pEntity->getEntityPart(PART_BUFF);
        if (pBuffPart == NULL)
        {
            return;
        }
        // 增加BUFF
        pBuffPart->Add(m_data.nBuffID, m_data.nBuffLevel, m_pEntity->getUID());

        m_dwLastTick = dwTick;
    }

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;

	// 上次时间
	DWORD					m_dwLastTick;
    // 上次位置
    Vector3                 m_locLast;
};
