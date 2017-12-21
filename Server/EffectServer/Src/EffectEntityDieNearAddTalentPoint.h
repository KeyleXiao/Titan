/*******************************************************************
** 文件名:	EffectEntityDieNearAddTalentPoint.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/22/2015
** 版  本:	1.0
** 描  述:	效果-实体死亡附加增加天赋点数
需求描述：http://172.16.0.120/redmine/issues/786
实现描述：某类实体死亡，附近增加天赋点数
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
#include "EntityHelper.h"


class CEffectEntityDieNearAddTalentPoint : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_EntityDieNearAddTalentPoint    SCHEME_DATA;

	CEffectEntityDieNearAddTalentPoint( SCHEME_DATA & data ) : m_data(data),m_pEntity(0)
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
			ErrorLn(_GT("CEffectEntityDieNearAddTalentPoint Start 取得全局事件失败，问题严重！"));
			return false;
		}
		if (m_data.nEntityType == TYPE_ENTITY || m_data.nEntityType == TYPE_MAX)
		{
			// 注册英雄死亡事件
			pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_PLAYER_ROLE, 0, "CEffectEntityDieNearAddTalentPoint");
			// 注册怪物死亡事件
			pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_MONSTER, 0, "CEffectEntityDieNearAddTalentPoint");
			// 注册Tank死亡事件
			pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_TANK, 0, "CEffectEntityDieNearAddTalentPoint");
		}
		else
		{
			// 注册类型死亡事件
			pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DIE, m_data.nEntityType, 0, "CEffectEntityDieNearAddTalentPoint");
		}

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
				if (m_data.nEntityType == TYPE_ENTITY || m_data.nEntityType == TYPE_MAX)
				{
					// 注销英雄死亡事件
					pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_PLAYER_ROLE, 0);
					// 注销怪物死亡事件
					pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_MONSTER, 0);
					// 注销Tank死亡事件
					pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_TANK, 0);
				}
				else
				{
					// 注册类型死亡事件
					pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DIE, m_data.nEntityType, 0);
				}
			}

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectEntityDieNearAddTalentPoint(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

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
		case EVENT_GAME_ENTITY_DIE:	// 死亡事件
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
				// 操作者
				UID uidOperator = pDie->uidOperator;
				if (isInvalidUID(uidOperator))
				{
					return;
				}
				// 死亡者
				UID uidTarget = pDie->uidTarget;
				if (isInvalidUID(uidTarget))
				{
					return;
				}
				// 自己
				UID uidSelf = m_pEntity->getUID();
				if (isInvalidUID(uidSelf))
				{
					return;
				}
				Vector3 vSelfTile = getLocation(uidSelf);
				Vector3 vTargetTile = getLocation(uidTarget);
				// 不在范围内
				if (!vSelfTile.checkDistance(vTargetTile, m_data.fDistance))
				{
					return;
				}

				// 检测操作者目标
				if (!g_EHelper.chooseTarget(m_pEntity, m_data.nOperatorFilter, uidOperator))
				{
					return;
				}

				// 检测目标目标
				if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetFilter, uidTarget))
				{
					return;
				}

				// 增加天赋点数
                updateTalentPoint(uidSelf, m_data.nValue, ENUM_TALENT_EFFECT);
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
