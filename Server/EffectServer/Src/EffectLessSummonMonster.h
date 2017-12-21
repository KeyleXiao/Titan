/*******************************************************************
** 文件名:	EffectLessSummonMonster.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/21/2015
** 版  本:	1.0
** 描  述:	效果-减少召唤怪物
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"

using namespace EFFECT_SERVER;

class CEffectLessSummonMonster : public IEffectEx
{
public:
	typedef  EffectServer_LessSummonMonster    SCHEME_DATA;

	CEffectLessSummonMonster( SCHEME_DATA & data ) : m_data(data)
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
		if (context == NULL || context->pEntity == NULL)
		{
			return false;
		}
		UID uidTarget = context->uidTarget;
		if (isInvalidUID(uidTarget))
		{
			return false;
		}
		if (!isMonster(uidTarget))
		{
			return false;
		}
		// 取得怪物ID
		int nMonsterID = getProperty_Integer(uidTarget, PROPERTY_ID);
		if (nMonsterID == 0)
		{
			return false;
		}
		if (m_data.nMonsterID > 0 && m_data.nMonsterID != nMonsterID)
		{
			return false;
		}

		__ISpellPart *pSpellPart = (__ISpellPart *)context->pEntity->getEntityPart(PART_SPELL);
		if (pSpellPart == NULL)
		{
			return false;
		}
		ISpell *pSpell = pSpellPart->findSpell(context->nID);
		if ( pSpell == 0 )
		{
			return false;
		}
		// 移除召唤实体
		pSpell->removeEntity(uidTarget);

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectLessSummonMonster(m_data);
	}

	// 释放
	virtual void			Release() 
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA               m_data;
};
