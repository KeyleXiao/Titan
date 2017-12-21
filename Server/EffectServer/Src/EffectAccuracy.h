/*******************************************************************
** 文件名:	EffectAccuracy.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/21/2015
** 版  本:	1.0
** 描  述:	效果-命中
需求描述：http://172.16.0.120/redmine/issues/789
实现描述：指定类型技能命中，不可躲闪
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

class CEffectAccuracy : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_Accuracy    SCHEME_DATA;

	CEffectAccuracy( SCHEME_DATA & data ) : m_data(data),m_pEntity(0)
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

		// 注册攻击事件
		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_ATTACK, "CEffectAccuracy");

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			// 注销攻击事件
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_ATTACK);

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectAccuracy(m_data);
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
		case EVENT_ENTITY_ATTACK:	// 攻击事件
			{
				if (m_pEntity == NULL)
				{
					return;
				}

				if (pszContext == NULL || nLen != sizeof(event_entity_attack))
				{
					return;
				}

				event_entity_attack *pAttack = (event_entity_attack *)pszContext;
				__ISpellPart *pSpellPart = (__ISpellPart *)m_pEntity->getEntityPart(PART_SPELL);
				if (pSpellPart == NULL)
				{
					return;
				}
				ISpell *pSpell = pSpellPart->findSpell(pAttack->nID);
				if (pSpell == NULL)
				{
					return;
				}
				int	nSpellEffectType = pSpell->getSpellData(SPDATA_SPELL_EFFECT_TYPE);
				// 技能效果类型匹配
				if ((m_data.nSpellEffectType & nSpellEffectType) != m_data.nSpellEffectType)
				{
					return;
				}
				pAttack->nAttackResultType |= AttackResult_Accuracy;
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
