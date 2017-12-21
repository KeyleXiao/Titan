/*******************************************************************
** 文件名:	EffectAssistAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	10/10/2015
** 版  本:	1.0
** 描  述:	效果-助攻增加Buff
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"
#include "EntityHelper.h"

class CEffectAssistAddBuff : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_AssistAddBuff    SCHEME_DATA;

	CEffectAssistAddBuff( SCHEME_DATA & data ) : m_data(data),m_pEntity(0),m_dwLastTick(0)
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

		UID uid = m_pEntity->getUID();

		// 获得事件中心接口
        SceneLogicEventHelper eventHelper(m_pEntity->getSceneID());
		IEventEngine *pEventEngine = eventHelper.m_ptr;
		if (pEventEngine==NULL)
		{
			return false;
		}
		pEventEngine->Subscibe(this, EVENT_GAME_ACTOR_ASSIST_CHANGE, UID_2_TYPE(uid), uid, "CEffectAssistAddBuff");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			UID uid = m_pEntity->getUID();

			// 获得事件中心接口
            SceneLogicEventHelper eventHelper(m_pEntity->getSceneID());
			IEventEngine *pEventEngine = eventHelper.m_ptr;
			if (pEventEngine != NULL)
			{
				pEventEngine->UnSubscibe(this, EVENT_GAME_ACTOR_ASSIST_CHANGE, UID_2_TYPE(uid), uid);
			}

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectAssistAddBuff(m_data);
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
		case EVENT_GAME_ACTOR_ASSIST_CHANGE:
			{
				if (m_pEntity == NULL)
				{
					return;
				}

				if (pszContext == NULL || nLen != sizeof(event_assist_add))
				{
					return;
				}
				event_assist_add *pAssistAdd = (event_assist_add *)pszContext;

				DWORD dwTick = getTickCountEx();
				if (dwTick < m_dwLastTick + m_data.nInterval)
				{
					return;
				}

				UID uidSelf = m_pEntity->getUID();
				if (isInvalidUID(uidSelf))
				{
					return;
				}

				UID uidOperator = pAssistAdd->uKillId;
				if (isInvalidUID(uidOperator))
				{
					return;
				}

				UID uidTarget = pAssistAdd->uDeadId;
				if (isInvalidUID(uidTarget))
				{
					return;
				}

				// 检测击杀目标
				if (!g_EHelper.chooseTarget(m_pEntity, m_data.nKillTargetFilter, uidOperator))
				{
					return;
				}

				// 检测死亡目标
				if (!g_EHelper.chooseTarget(m_pEntity, m_data.nDieTargetFilter, uidTarget))
				{
					return;
				}

				UID uidAddBuff = INVALID_UID;
				switch (m_data.AssistAddBuffType)
				{
				case ASSIST_ADD_BUFF_ASSIST:
					{
						uidAddBuff = uidSelf;
					}
					break;
				case ASSIST_ADD_BUFF_KILL:
					{
						uidAddBuff = uidOperator;
					}
					break;
				case ASSIST_ADD_BUFF_DIE:
					{
						uidAddBuff = uidTarget;
					}
					break;
				default:
					{
						ErrorLn("CEffectAssistAddBuff error AssistAddBuffType="<<m_data.AssistAddBuffType);
						return;
					}
					break;
				}

				// 添加buff
				AddBuff(uidAddBuff, m_data.nBuffID, m_data.nBuffLevel, uidAddBuff);

				// 设置上次时间
				m_dwLastTick = dwTick;
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
