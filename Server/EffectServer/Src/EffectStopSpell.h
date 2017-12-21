/*******************************************************************
** 文件名:	EffectStopSpell.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	ljw
** 日  期:	5/4/2017
** 版  本:	1.0
** 描  述:	效果-停止当前技能
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "EventDef.h"
#include "ISpell.h"
#include "EntityHelper.h"
#include "SpellContextDef.h"

class CEffectStopSpell : public IEffectEx
{
public:
	typedef  EffectServer_StopSpell    SCHEME_DATA;

	CEffectStopSpell( SCHEME_DATA & data ) : m_data(data)
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


        __ISpellPart* pSpellPart = (__ISpellPart*)context->pEntity->getEntityPart(PART_SPELL);
        if(pSpellPart == NULL)
            return false;

        ISpell* pSpell = pSpellPart->findSpell(m_data.nSpellID);
        if(pSpell == NULL)
            return false;

        if(!pSpell->isWorking())
            return false;

        pSpell->onSpellEvent(SPELL_EVENT_FINISH, pSpell->getCurSpellContext());

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{

	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectStopSpell(m_data);
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
