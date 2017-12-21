/*******************************************************************
** 文件名:	EffectChangeStateAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	9/01/2015
** 版  本:	1.0
** 描  述:	效果-改变状态增加Buff
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "EntityHelper.h"
#include "ExternalHelper.h"

class CEffectChangeStateAddBuff : public IEffectEx, public IEventExecuteSink, public rkt::TimerHandler
{
public:
	typedef  EffectServer_ChangeStateAddBuff    SCHEME_DATA;

	CEffectChangeStateAddBuff( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),
		m_nEntityState(0),m_dwLastTime(0),m_dwLastTransitTime(0),m_bAddBuff(false),m_nAddBuffCount(0)
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

		// 注册切换状态
		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_CHANGE_STATE, "CEffectChangeStateAddBuff");

		// 取得实体状态
		m_nEntityState = m_pEntity->getState();

		// 设置上次过渡时间
		m_dwLastTransitTime = getTickCountEx();

		// 设置增加buff次数
		m_nAddBuffCount = m_data.nAddBuffCount;

		// 启动定时器
		g_EHelper.SetTimer(0, 1000, this, INFINITY_CALL, "CEffectChangeStateAddBuff");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			// 杀掉定时器定时器
			g_EHelper.KillTimer(0, this);

			// 注销切换状态
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_CHANGE_STATE);

			if (m_bAddBuff)
			{
				UID uid = m_pEntity->getUID();
				// 移除Buff
				RemoveBuff(uid, m_data.nBuffID, uid, uid);

				// 设置未增加buff
				m_bAddBuff = false;
			}

			m_nEntityState = 0;
			m_dwLastTime = 0;
			m_dwLastTransitTime = 0;
			m_nAddBuffCount = 0;
			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectChangeStateAddBuff(m_data);
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
		case EVENT_ENTITY_CHANGE_STATE:
			{
				if (pszContext == NULL || nLen != sizeof(event_entity_change_state))
				{
					return;
				}

				event_entity_change_state *pChangeState = (event_entity_change_state *)pszContext;
				// 设置实体状态
				m_nEntityState = pChangeState->nState;

				if (m_nEntityState == m_data.nEntityState)
				{
					// 设置上次过渡时间
					m_dwLastTransitTime = getTickCountEx();
					// 设置增加buff次数
					m_nAddBuffCount = m_data.nAddBuffCount;
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

		UID uid = m_pEntity->getUID();
		if (m_nEntityState == m_data.nEntityState)
		{
			// 取得Tick
			DWORD dwTick = getTickCountEx();
			if (dwTick < m_dwLastTransitTime + m_data.nTransitTime)
			{
				return;
			}

			if (dwTick < m_dwLastTime + m_data.nInterval)
			{
				return;
			}
			if (m_nAddBuffCount <= 0)
			{
				return;
			}
			--m_nAddBuffCount;

			// 增加buff
			AddBuff(uid, m_data.nBuffID, m_data.nBuffLevel, uid);

			// 设置增加Buff
			m_bAddBuff = true;

			// 设置上次时间
			m_dwLastTime = dwTick;
			// 设置上次过渡时间
			m_dwLastTransitTime = dwTick;
		}
		else
		{
			if (m_bAddBuff)
			{
				// 移除Buff
				RemoveBuff(uid, m_data.nBuffID, uid, uid);

				// 设置未增加buff
				m_bAddBuff = false;
			}
		}
	}

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;

	// 实体状态
	int						m_nEntityState;

	// 上次时间
	DWORD					m_dwLastTime;
	// 上次过渡时间
	DWORD					m_dwLastTransitTime;
	// 是否增加buff
	bool					m_bAddBuff;
	// Buff增加次数
	int						m_nAddBuffCount;
};
