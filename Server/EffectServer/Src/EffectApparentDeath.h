/*******************************************************************
** 文件名:	EffectApparentDeath.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/20/2015
** 版  本:	1.0
** 描  述:	效果-假死
需求描述：http://172.16.0.120/redmine/issues/791
实现描述：将死亡状态改变成未死亡状态，效果为实现自由配置，此处只增加一个Buff,Buff调用效果
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
#include "ConditionDef.h"
using namespace CONDITION_SERVER;

#define  EFFECT_INNER_CONDITION( typeName,obj,pContext)    gServerGlobal->getConditionFactory()->TestInnerCondition( #typeName,&obj,sizeof(obj),pContext)
#define  EFFECT_INNER_EFFECT( typeName,obj,pContext)  gServerGlobal->getEffectFactory()->DoInnerEffect( #typeName,&obj,sizeof(obj),pContext)

class CEffectApparentDeath : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_ApparentDeath    SCHEME_DATA;

	CEffectApparentDeath( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_uidOperator(INVALID_UID)
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
        m_uidOperator = context->uidOperator;

		// 注册伤害后投票假死事件
		g_EHelper.Subscibe(m_pEntity, static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_DAMAGE_VOTE_FAKEDEAD, "CEffectApparentDeath");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			// 注销伤害后投票假死事件
			g_EHelper.UnSubscibe(m_pEntity, static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_DAMAGE_VOTE_FAKEDEAD);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectApparentDeath(m_data);
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
		case EVENT_ENTITY_DAMAGE_VOTE_FAKEDEAD:	// 伤害后投票假死事件
			{
                if (m_pEntity == NULL)
                {
                    return;
                }

				if (pszContext == NULL || nLen != sizeof(event_entity_damage))
				{
					return;
				}

                __IBuffPart *pBuffPart = (__IBuffPart *)m_pEntity->getEntityPart(PART_BUFF);
                if (pBuffPart == NULL)
                {
                    return;
                }

				CONDITION_CONTEXT context;
				context.pEntity = m_pEntity;
				// 检查冷却
				ConditionServer_CoolDown CoolDown;
				CoolDown.nClassID = m_data.nClassID;
				CoolDown.nCoolDownID = m_data.nCoolID;
				if (EFFECT_INNER_CONDITION(ConditionServer_CoolDown, CoolDown, &context)==false)
				{
					return;
				}

				event_entity_damage *pDamage = (event_entity_damage*)pszContext;
				// 未死亡时，不处理
				if (!pDamage->bDead)
				{
					return;
				}
				// 设置未死亡状态
				pDamage->bDead = false;
                
                // 用于控制识别假死玩法
                if(m_data.isFakeDead)
                {
                    pDamage->bFakeDead = true;
                }

				EFFECT_CONTEXT effectContext;
				effectContext.pEntity = m_pEntity;
				// 消耗冷却
				EffectServer_CoolDown coolDown;
				coolDown.nClassID = m_data.nClassID;
				coolDown.nCoolDownID = m_data.nCoolID;
				EFFECT_INNER_EFFECT(EffectServer_CoolDown, coolDown, &effectContext);

                UID uidAdd = INVALID_UID;
                switch (m_data.nAddBuffTargetType)
                {
                case APPARENT_DEATH_ADD_BUFF_TARGET_SELF:
                    {
                        uidAdd = m_pEntity->getUID();
                    }
                    break;
                case APPARENT_DEATH_ADD_BUFF_TARGET_OPERATOR:
                    {
                        uidAdd = pDamage->uidOperator; 
                    }
                    break;
                case APPARENT_DEATH_ADD_BUFF_TARGET_TARGET:
                    {
                        uidAdd = pDamage->uidTarget;
                    }
                    break;
                case APPARENT_DEATH_ADD_BUFF_TARGET_APPARENT_DEATH_EFFECT_OPERATOR:
                    {
                        uidAdd = m_uidOperator;
                    }
                    break;
                default:
                    {
                        ErrorLn("CEffectApparentDeath error nAddBuffTargetType="<<m_data.nAddBuffTargetType);
                        return;
                    }
                    break;
                }

                // 用玩家部件直接，否则有额外伤害时，buff未加上时，就死了
                pBuffPart->Add(m_data.nBuffID, m_data.nBuffLevel, uidAdd);
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

    UID                     m_uidOperator;
};
