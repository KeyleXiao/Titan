/*******************************************************************
** 文件名:	EffectDisableSpell.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	6/08/2015
** 版  本:	1.0
** 描  述:	效果-禁用技能
********************************************************************/

#pragma once

#include "IEffect.h"
#include "IEntity.h"
#include "EffectDef.h"
#include "ISlotLogicPart.h"

using namespace rkt;
using namespace EFFECT_CLIENT;

class CEffectDisableSpell : public IEffect, public IEventVoteSink
{
public:
	typedef  EffectClient_DisableSpell    SCHEME_DATA;

	CEffectDisableSpell( SCHEME_DATA & data ) : m_data(data),m_pEntity(0)
	{
		vector<string> tempVector;
		tempVector.clear();

		m_DisableSpellIDSet.clear();
		StringHelper::split(tempVector, m_data.strSpellID, ';', " ");
		for (UINT nCount=0; nCount<tempVector.size(); ++nCount)
		{
			int nSpellID = StringHelper::toInt(tempVector[nCount]);
			if (nSpellID <= 0)
			{
				continue;
			}
			m_DisableSpellIDSet.insert(nSpellID);
		}
	}

    // 获取效果ID
    virtual int             GetID()
    {
        return m_data.nID;
    }

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * context )
	{
		if (context == NULL || context->pEntity == NULL)
		{
			return false;
		}

        if(!context->pEntity->isHero())
        {
            return false;
        }

		if (m_DisableSpellIDSet.empty())
		{
			return false;
		}

		m_pEntity = context->pEntity;

		ISlotLogicPart *pSlotLogicPart = (ISlotLogicPart *)m_pEntity->getEntityPart(PART_SLOTLOGIC);
		if (pSlotLogicPart == NULL)
		{
			return false;
		}

		cmd_creature_disable_spell disableapell;
		disableapell.nType = 0;
		for (DISABLE_SPELL_SET::iterator it = m_DisableSpellIDSet.begin(); it != m_DisableSpellIDSet.end(); ++it)
		{
			if (disableapell.nSpellCount >= DISABLE_SPELL_COUNT)
			{
				break;
			}

			int nSpellID = *it;
			SLOT_TYPE nSlotType = SLOT_TYPE_NONE;
			int nSlotIndex = 0;
			if (!pSlotLogicPart->GetSkillIDInfo(nSpellID, nSlotType, nSlotIndex))
			{
				continue;
			}

			disableapell.nSlotType[disableapell.nSpellCount] = nSlotType;
			disableapell.nSlotIndex[disableapell.nSpellCount] = nSlotIndex;

			++disableapell.nSpellCount;
		}

        // 策划在封魔后有时候会配置播放动画 而显示层通知逻辑层使用技能是post过来的
        // 假如逻辑层不屏蔽技能 可能会额外处理post队列的技能 导致打断策划配置的动画
        IEventEngine* pEventEngine = gClientGlobal->getEventEngine();
        if(pEventEngine != NULL)
        {
             pEventEngine->Subscibe(static_cast<IEventVoteSink*>(this), EVENT_ENTITY_CAST_SPELL, TYPE_PLAYER_ROLE, m_pEntity->getUID(), __FILE__);
             pEventEngine->Subscibe(static_cast<IEventVoteSink*>(this), EVENT_ENTITY_RESPONSE_SPELL_ATTR_QUERY, TYPE_PLAYER_ROLE, m_pEntity->getUID(), __FILE__);
        }

		// 告诉显示层禁用技能
		m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_DISABLE_SPELL, 0, 0, &disableapell, sizeof(disableapell));

		return true;
	}

    virtual bool OnVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
    {
        switch(wEventID)
        {
        case EVENT_ENTITY_CAST_SPELL:
            {
                event_cast_spell* pEvent = (event_cast_spell*)pszContext;

                return m_DisableSpellIDSet.find(pEvent->nID) == m_DisableSpellIDSet.end();
            }
            break;
        case EVENT_ENTITY_RESPONSE_SPELL_ATTR_QUERY:
            {
                event_response_spell_attr_query* pEvent = (event_response_spell_attr_query*)pszContext;
                return m_DisableSpellIDSet.find(pEvent->nID) == m_DisableSpellIDSet.end();
            }
            break;
        }

        return true;
    }

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0 && m_pEntity->isHero())
		{
			ISlotLogicPart *pSlotLogicPart = (ISlotLogicPart *)m_pEntity->getEntityPart(PART_SLOTLOGIC);
			if (pSlotLogicPart != NULL)
			{
				cmd_creature_disable_spell disableapell;
				disableapell.nType = 1;
				for (DISABLE_SPELL_SET::iterator it = m_DisableSpellIDSet.begin(); it != m_DisableSpellIDSet.end(); ++it)
				{
					if (disableapell.nSpellCount >= DISABLE_SPELL_COUNT)
					{
						break;
					}

					int nSpellID = *it;
					SLOT_TYPE nSlotType = SLOT_TYPE_NONE;
					int nSlotIndex = 0;
					if (!pSlotLogicPart->GetSkillIDInfo(nSpellID, nSlotType, nSlotIndex))
					{
						continue;
					}

					disableapell.nSlotType[disableapell.nSpellCount] = nSlotType;
					disableapell.nSlotIndex[disableapell.nSpellCount] = nSlotIndex;

					++disableapell.nSpellCount;
				}

				// 告诉显示层取消禁用技能
				m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_DISABLE_SPELL, 0, 0, &disableapell, sizeof(disableapell));
            }

            IEventEngine* pEventEngine = gClientGlobal->getEventEngine();
            if(pEventEngine != NULL)
            {
                pEventEngine->UnSubscibe(static_cast<IEventVoteSink*>(this), EVENT_ENTITY_CAST_SPELL, TYPE_PLAYER_ROLE, m_pEntity->getUID());
                pEventEngine->UnSubscibe(static_cast<IEventVoteSink*>(this), EVENT_ENTITY_RESPONSE_SPELL_ATTR_QUERY, TYPE_PLAYER_ROLE, m_pEntity->getUID());
            }
		}
        m_pEntity = 0;
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectDisableSpell(m_data);
	}

	// 释放
	virtual void			Release() 
	{
		Stop();
		delete this;
	}

	// 取得效果ID
	virtual int				GetEffectID()
	{ 
		return m_data.nID; 
	}

	// 
	virtual bool			IsBusying()
	{
		return false;
	}

	// 
	virtual bool			OnThink()
	{
		return false;
	}

	// 
	virtual bool			IsAbleStart()
	{
		return true;
	}

private:
	SCHEME_DATA               m_data;

	IEntity					  *m_pEntity;

	// 禁用技能表
	typedef set<int>	  DISABLE_SPELL_SET;	
	DISABLE_SPELL_SET		  m_DisableSpellIDSet;
};
