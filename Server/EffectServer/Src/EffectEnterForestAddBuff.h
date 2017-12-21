/*******************************************************************
** 文件名:	EffectEnterForestAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	3/02/2016
** 版  本:	1.0
** 描  述:	效果-进入草丛增加Buff
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

class CEffectEnterForestAddBuff : public IEffectEx,public IEventExecuteSink, public TimerHandler
{
public:
	typedef  EffectServer_EnterForestAddBuff    SCHEME_DATA;

	CEffectEnterForestAddBuff( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_dwLastTick(0),m_bAddBuff(false)
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

		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_FOREST, "CEffectEnterForestAddBuff");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
            // 杀掉延迟增加Buff定时器
            g_EHelper.KillTimer(0, this);

			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_FOREST);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectEnterForestAddBuff(m_data);
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
		case EVENT_ENTITY_FOREST:
			{
				if (m_pEntity == NULL)
				{
					break;
				}
                UID uidSelf = m_pEntity->getUID();

				DWORD dwTick = getTickCountEx();
				if (dwTick < m_dwLastTick + m_data.nInterval)
				{
					break;
				}

				if (pszContext == NULL || nLen != sizeof(event_entity_forest))
				{
					break;
				}
				event_entity_forest *pForest = (event_entity_forest*)pszContext;

                // 进入草丛
                if (pForest->nType > 0)
                {
                    if (m_data.nDelay <= 0)
                    {
                        // 直接加Buff
                        OnTimer(0);
                    }
                    else
                    {
                        g_EHelper.KillTimer(0, this);

                        // 启动定时器延迟加Buff
                        g_EHelper.SetTimer(0, m_data.nDelay, this, 1, "CEffectEnterForestAddBuff");
                    }
                }
                else
                {
                    // 离开草丛，需要移除Buff
                    if (m_data.nRemoveBuff == MA_TRUE)
                    {
                        // 杀掉延迟增加Buff定时器
                        g_EHelper.KillTimer(0, this);

                        // 已增加Buff,需要移除掉
                        if (m_bAddBuff)
                        {
                            // 移除Buff
                            RemoveBuff(uidSelf, m_data.nBuffID, uidSelf, uidSelf);

                            // 设置未增加Buff
                            m_bAddBuff = false;
                        }
                    }
                }
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
        UID uidSelf = m_pEntity->getUID();

        // 增加Buff
        AddBuff(uidSelf, m_data.nBuffID, m_data.nBuffLevel, uidSelf);

        m_bAddBuff = true;
    }

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;

	// 上次时间
	DWORD					m_dwLastTick;

    // 是否已增加Buff
    bool                    m_bAddBuff;
};
