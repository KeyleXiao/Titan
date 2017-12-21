/*******************************************************************
** 文件名:	EffectSetSlotLevel.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	9/16/2016
** 版  本:	1.0
** 描  述:	效果-设置槽位等级
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_SERVER;
#include "EntityHelper.h"

class CEffectSetSlotLevel : public IEffectEx
{
public:
	typedef  EffectServer_SetSlotLevel    SCHEME_DATA;

	CEffectSetSlotLevel( SCHEME_DATA & data ) : m_data(data)
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
		if( context==0 || context->pEntity==0 )
		{
			return false;
		}

        if (m_data.nSlotType <= SLOT_TYPE_NONE || m_data.nSlotType >= SLOT_TYPE_MAX)
        {
            return false;
        }

		__ISpellPart *pSpellPart = (__ISpellPart *)context->pEntity->getEntityPart(PART_SPELL);
		if (pSpellPart == NULL)
		{
			return false;
		}

        __IEntityEvent *pEntityEvent = context->pEntity->getEntityEvent();
        if (pEntityEvent == NULL)
        {
            return false;
        }

        // 是技能类型,设置槽位等级
        if (m_data.nSlotType == SLOT_TYPE_SKILL)
        {
            pSpellPart->setSpellSlotLevel(m_data.nSlotIndex, m_data.nSlotLevel);
        }

        // 槽位索引更新
        event_slotindex_update slotIndexUpdate;
        slotIndexUpdate.bAdd = true;
        slotIndexUpdate.nSlotIndex = m_data.nSlotIndex;
        slotIndexUpdate.nSlotLevel = m_data.nSlotLevel;
        slotIndexUpdate.nSlotType = m_data.nSlotType;
        slotIndexUpdate.nID = m_data.nSpellID;
        // 槽位索引更新事件
        pEntityEvent->FireExecute(EVENT_SLOTINDEX_UPDATE, (char *)&slotIndexUpdate, sizeof(slotIndexUpdate));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectSetSlotLevel(m_data);
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
