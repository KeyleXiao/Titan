/*******************************************************************
** 文件名:	EffectBreakSpell.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/27/2015
** 版  本:	1.0
** 描  述:	效果-打断技能
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;

class CEffectBreakSpell : public IEffectEx
{
public:
	typedef  EffectServer_BreakSpell    SCHEME_DATA;

	CEffectBreakSpell( SCHEME_DATA & data ) : m_data(data)
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

		__IEntityEvent *pEntityEvent = context->pEntity->getEntityEvent();
		if (pEntityEvent == NULL)
		{
			return false;
		}
        // 先发投票事件
        if (!pEntityEvent->FireVote(EVENT_ENTITY_EFFECT_BREAK, NULL, 0))
        {
            return false;
        }
        // 执行事件
		pEntityEvent->FireExecute(EVENT_ENTITY_EFFECT_BREAK, NULL, 0);

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectBreakSpell(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA             m_data;
};
