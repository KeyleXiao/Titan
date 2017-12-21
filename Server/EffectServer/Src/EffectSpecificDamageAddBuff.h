/*******************************************************************
** 文件名:	EffectSpecificDamageAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/13/2016
** 版  本:	1.0
** 描  述:	效果-特定伤害增加Buff
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

class CEffectSpecificDamageAddBuff : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_SpecificDamageAddBuff    SCHEME_DATA;

	CEffectSpecificDamageAddBuff( SCHEME_DATA &data ) : m_data(data),m_pEntity(0)
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
            return false;
        }

        if (m_data.nEntityType == TYPE_ENTITY || m_data.nEntityType == TYPE_MAX)
        {
            // 注册英雄伤害事件
            pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DAMAGE, TYPE_PLAYER_ROLE, 0, "CEffectSpecificDamageAddBuff");
            // 注册怪物伤害事件
            pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DAMAGE, TYPE_MONSTER, 0, "CEffectSpecificDamageAddBuff");
            // 注册Tank伤害事件
            pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DAMAGE, TYPE_TANK, 0, "CEffectSpecificDamageAddBuff");
        }
        else
        {
            // 注册类型伤害事件
            pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DAMAGE, m_data.nEntityType, 0, "CEffectSpecificDamageAddBuff");
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
                    // 注销英雄伤害事件
                    pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DAMAGE, TYPE_PLAYER_ROLE, 0);
                    // 注销怪物伤害事件
                    pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DAMAGE, TYPE_MONSTER, 0);
                    // 注销Tank伤害事件
                    pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DAMAGE, TYPE_TANK, 0);
                }
                else
                {
                    // 注册类型伤害事件
                    pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DAMAGE, m_data.nEntityType, 0);
                }
            }

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectSpecificDamageAddBuff(m_data);
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
		case EVENT_GAME_ENTITY_DAMAGE:
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

                int nSpellEffectType = pDamage->nSpellEffectType;
                // 判断技能效果类型
                if ((m_data.nSpellEffectType & nSpellEffectType) != m_data.nSpellEffectType)
                {
                    break;
                }

                UID uidSelf = m_pEntity->getUID();
                if (isInvalidUID(uidSelf))
                {
                    return;
                }

                UID uidOperator = pDamage->uidOperator;
                if (isInvalidUID(uidOperator))
                {
                    break;
                }

                UID uidTarget = pDamage->uidTarget;
                if (isInvalidUID(uidTarget))
                {
                    break;
                }

                // 检测操作者目标
                if (!g_EHelper.chooseTarget(m_pEntity, m_data.nOperatorTargetFilter, uidOperator))
                {
                    break;
                }

                // 检测目标目标
                if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetTargetFilter, uidTarget))
                {
                    break;
                }

                UID uid = INVALID_UID;
                switch (m_data.nTargetType)
                {
                case MA_TAGET_TO_SELF:
                    {
                        uid = uidSelf;
                    }
                    break;
                case MA_TAGET_TO_OPERATOR:
                    {
                        uid = uidOperator;
                    }
                    break;
                case MA_TAGET_TO_TARGET:
                    {
                        uid = uidTarget;
                    }
                    break;
                default:
                    {
                        ErrorLn("CEffectSpecificDamageAddBuff error nTargetType="<<m_data.nTargetType);
                        return;
                    }
                    break;
                }

                UID uidAddBuff = INVALID_UID;
                switch (m_data.nAddBuffTagetType)
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
                        ErrorLn("CEffectSpecificDamageAddBuff error nAddBuffTagetType="<<m_data.nAddBuffTagetType);
                        return;
                    }
                    break;
                }
                
                SBuffContext BuffContext;
                BuffContext.nID = pDamage->nID;
                AddBuff(uid, m_data.nBuffID, m_data.nBuffLevel, uidAddBuff, 0, &BuffContext, sizeof(BuffContext));
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
