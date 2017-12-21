/*******************************************************************
** 文件名:	EffectEntityKillAddTalentPoint.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/22/2015
** 版  本:	1.0
** 描  述:	效果-击杀额外获得天赋点数
需求描述：http://172.16.0.120/redmine/issues/786
实现描述：EVENT_GAME_ACTOR_KILL_ADD_TALENT_POINT事件额外增加天赋点数
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

class CEffectEntityKillAddTalentPoint : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_EntityKillAddTalentPoint    SCHEME_DATA;

	CEffectEntityKillAddTalentPoint( SCHEME_DATA & data ) : m_data(data),m_pEntity(0)
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
		pEventEngine->Subscibe(this, EVENT_GAME_ACTOR_KILL_ADD_TALENT_POINT, UID_2_TYPE(uid), uid, "CEffectEntityKillAddTalentPoint");

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
			if (pEventEngine==NULL)
			{
				ErrorLn(_GT("CEffectEntityKillAddTalentPoint pEventEngine==NULL,反注销事件失败，严重！！"));
				return;
			}
			pEventEngine->UnSubscibe(this, EVENT_GAME_ACTOR_KILL_ADD_TALENT_POINT, UID_2_TYPE(uid), uid);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectEntityKillAddTalentPoint(m_data);
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
		case EVENT_GAME_ACTOR_KILL_ADD_TALENT_POINT:	// 击杀获得天赋点数
			{
				if (m_pEntity == NULL)
				{
					return;
				}

				if (pszContext == NULL || nLen != sizeof(event_entity_kill_add_talent_point))
				{
					return;
				}

				UID uid = m_pEntity->getUID();

				int nValue = 0;
				event_entity_kill_add_talent_point *pKillAddTalentPoint = (event_entity_kill_add_talent_point *)pszContext;
				switch (m_data.nValueType)
				{
				case MA_VALUE_NUM:
					{
						nValue = m_data.nValue;
					}
					break;
				case MA_VALUE_PER:
					{
						nValue = (int)(m_data.nValue/EFFECT_RATE*pKillAddTalentPoint->nValue);
					}
					break;
				default:
					{
						ErrorLn("CEffectEntityKillAddTalentPoint error nValueType="<<m_data.nValueType);
						return;
					}
					break;
				}

				// 增加天赋点数
                updateTalentPoint(uid, nValue, ENUM_TALENT_EFFECT);
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
