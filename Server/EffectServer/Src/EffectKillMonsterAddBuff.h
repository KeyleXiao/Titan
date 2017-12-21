/*******************************************************************
** 文件名:	EffectKillMonsterAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/14/2016
** 版  本:	1.0
** 描  述:	效果-击杀怪物获得BUFF
            http://172.16.0.120/redmine/issues/1752
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
#include "ICamp.h"
#include "IGameSceneService.h"
class CEffectKillMonsterAddBuff : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_KillMonsterAddBuff    SCHEME_DATA;

	CEffectKillMonsterAddBuff( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_dwLastTick(0)
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

		DWORD dwSceneID = m_pEntity->getSceneID();
		SceneLogicEventHelper eventHelper(dwSceneID);
		IEventEngine *pGlobalEvent = eventHelper.m_ptr;
		if (pGlobalEvent == NULL)
		{
			ErrorLn(_GT("CEffectKillMonsterAddBuff Start 取得全局事件失败，问题严重！"));
            return false;
		}
        // 注册怪物死亡事件
        pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_MONSTER, 0, "CEffectKillMonsterAddBuff");

		return true;
	}


	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			DWORD dwSceneID = m_pEntity->getSceneID();
			SceneLogicEventHelper eventHelper(dwSceneID);
			IEventEngine *pGlobalEvent = eventHelper.m_ptr;
			if (pGlobalEvent != NULL)
			{
                // 注销怪物死亡事件
                pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_MONSTER, 0);
			}

			m_pEntity = 0;
		}
	}
	 
	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectKillMonsterAddBuff(m_data);
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
		case EVENT_GAME_ENTITY_DIE:
			{
                if (m_pEntity == NULL)
                {
                    return;
                }

				if (pszContext == NULL || nLen != sizeof(event_entity_die))
				{
					return;
				}
				event_entity_die *pDie = (event_entity_die *)pszContext;

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

				UID uidOperator = pDie->uidOperator;
				if (isInvalidUID(uidOperator))
				{
					return;
				}

				UID uidTarget = pDie->uidTarget;
				if (isInvalidUID(uidTarget))
				{
					return;
				}

                // 取得怪物ID
                int nMonsterID = getProperty_Integer(uidTarget, PROPERTY_ID);
                if ((m_data.nMonsterID != 0 && m_data.nMonsterID != nMonsterID))
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

				UID uidBuff = INVALID_UID;
				switch (m_data.nBuffType)
				{
				case MA_TAGET_TO_SELF:
					{
						uidBuff = uidSelf;
					}
					break;
                case MA_TAGET_TO_OPERATOR:
                    {
                        uidBuff = uidOperator;
                    }
                    break;
				case MA_TAGET_TO_TARGET:
					{
						uidBuff = uidTarget;
					}
					break;
				default:
					{
						ErrorLn("CEffectKillMonsterAddBuff error nBuffType="<<m_data.nBuffType);
						return;
					}
					break;
				}
                if (isInvalidUID(uidBuff))
                {
                    return;
                }

                UID uidAddBuff = INVALID_UID;
                switch (m_data.nAddBuffType)
                {
                case MA_TAGET_TO_SELF:
                    {
                        uidAddBuff = uidSelf;
                    }
                    break;
                case MA_TAGET_TO_OPERATOR:
                    {
                        uidAddBuff = uidOperator;
                    }
                    break;
                case MA_TAGET_TO_TARGET:
                    {
                        uidAddBuff = uidTarget;
                    }
                    break;
                default:
                    {
                        ErrorLn("CEffectKillMonsterAddBuff error nAddBuffType="<<m_data.nAddBuffType);
                        return;
                    }
                    break;
                }
                if (isInvalidUID(uidAddBuff))
                {
                    return;
                }

                SBuffContext BuffContext;
                BuffContext.nID = pDie->nSpellID;
                // 给buff添加者添加buff
                AddBuff(uidBuff, m_data.nBuffID, m_data.nBuffLevel, uidAddBuff, 0, &BuffContext, sizeof(BuffContext));

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
