/*******************************************************************
** 文件名:	EffectReplaceSlotSpell.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	3/08/2016
** 版  本:	1.0
** 描  述:	效果-替换槽位技能
            以前技能升级的时候,如果那个槽位正好有被替换技能，升级后技能
            替换回来不了,使用此效果,不会有上述问题
            http://172.16.0.120/redmine/issues/1701
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"
#include "ISlotPart.h"

class CEffectReplaceSlotSpell : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_ReplaceSlotSpell    SCHEME_DATA;

	CEffectReplaceSlotSpell( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_nByReolaceSpellID(0),m_nLastSpellID(0),m_nLastSlotLevel(0),m_nLastLearnSpellID(0)
	{
        vector<string> tempVector;
        tempVector.clear();

        m_vSpellID.clear();
        StringHelper::split(tempVector, m_data.strSpell, ';', " ");
        for (UINT nCount=0; nCount<tempVector.size(); ++nCount)
        {
            m_vSpellID.push_back(StringHelper::toInt(tempVector[nCount]));
        }
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

        __IEntityEvent *pEntityEvent = m_pEntity->getEntityEvent();
        if (pEntityEvent == NULL)
        {
            return false;
        }

        __ISlotPart *pSlotPart = (__ISlotPart *)m_pEntity->getEntityPart(PART_SLOTLOGIC);
        if (pSlotPart == NULL)
        {
            return false;
        }

        __ISpellPart *pSpellPart = (__ISpellPart *)m_pEntity->getEntityPart(PART_SPELL);
        if (pSpellPart == NULL)
        {
            return false;
        }

        // 发送准备替换技能事件
        event_prereplace_spell preReplaceSpell;
        preReplaceSpell.nSlotType = m_data.nSlotType;
        preReplaceSpell.nSlotIndex = m_data.nSlotIndex;
        pEntityEvent->FireExecute(EVENT_ENTITY_PRE_REPLACE_SPELL, (LPCSTR)&preReplaceSpell, sizeof(preReplaceSpell));

        // 取得被替换ID
        m_nByReolaceSpellID = pSlotPart->GetSlotSkillID(m_data.nSlotType, m_data.nSlotIndex);

        // 学会替换技能
        pSpellPart->addSpell(m_data.nSpellID);
        //m_nLastLearnSpellID = m_data.nSpellID;

        // 槽位索引更新
        event_slotindex_update slotIndexUpdate;
        slotIndexUpdate.bAdd = true;
        slotIndexUpdate.nSlotIndex = m_data.nSlotIndex;
        slotIndexUpdate.nSlotLevel = pSpellPart->getSpellSlotLevel(m_data.nSlotIndex);
        slotIndexUpdate.nSlotType = m_data.nSlotType;
        slotIndexUpdate.nID = m_data.nSpellID;
        // 槽位索引更新事件
        pEntityEvent->FireExecute(EVENT_SLOTINDEX_UPDATE, (char *)&slotIndexUpdate, sizeof(slotIndexUpdate));

        // 槽位改变,冷却也需要改变
        effectAddFreezeTime(m_data.nSpellID);

        // 是可升级技能
        if (m_data.nSlotType == SLOT_TYPE_SKILL && m_data.nSlotIndex < SPELL_UPGRADE_SLOT_COUNT)
        {
            g_EHelper.Subscibe(m_pEntity, this, EVENT_UPDATE_SPELL, "CEffectReplaceSlotSpell");
        }

        g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_PRE_REPLACE_SPELL, __FUNCTION__);

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
        if (m_pEntity != 0)
        {
            // 是可升级技能
            if (m_data.nSlotType == SLOT_TYPE_SKILL && m_data.nSlotIndex < SPELL_UPGRADE_SLOT_COUNT)
            {
                g_EHelper.UnSubscibe(m_pEntity, this, EVENT_UPDATE_SPELL);
            }

            g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_PRE_REPLACE_SPELL);

            // 可恢复 恢复到替换之前的技能
            if(!m_data.bNeverRestore)
            {
                // 还原被替换技能
                if (m_nByReolaceSpellID > 0)
                {
                    __IEntityEvent *pEntityEvent = m_pEntity->getEntityEvent();
                    if (pEntityEvent != NULL)
                    {
                        // 槽位索引更新
                        event_slotindex_update slotIndexUpdate;
                        slotIndexUpdate.bAdd = true;
                        slotIndexUpdate.nSlotIndex = m_data.nSlotIndex;
                        slotIndexUpdate.nSlotLevel = m_nLastSlotLevel;
                        slotIndexUpdate.nSlotType = m_data.nSlotType;
                        slotIndexUpdate.nID = m_nByReolaceSpellID;
                        // 槽位索引更新事件
                        pEntityEvent->FireExecute(EVENT_SLOTINDEX_UPDATE, (char *)&slotIndexUpdate, sizeof(slotIndexUpdate));

                        // 槽位改变,冷却也需要改变
                        effectAddFreezeTime(m_nByReolaceSpellID);
                    }
                }
                m_nByReolaceSpellID = 0;

                // 删除上次学会技能
                if (m_nLastLearnSpellID > 0)
                {
                    __ISpellPart *pSpellPart = (__ISpellPart *)m_pEntity->getEntityPart(PART_SPELL);
                    if (pSpellPart != NULL)
                    {
                        pSpellPart->removeSpell(m_nLastLearnSpellID);
                    }
                }
                m_nLastLearnSpellID = 0;

                // 设置最后技能
                if (m_nLastSpellID > 0)
                {
                    __IEntityEvent *pEntityEvent = m_pEntity->getEntityEvent();
                    if (pEntityEvent != NULL)
                    {
                        // 槽位索引更新
                        event_slotindex_update slotIndexUpdate;
                        slotIndexUpdate.bAdd = true;
                        slotIndexUpdate.nSlotIndex = m_data.nSlotIndex;
                        slotIndexUpdate.nSlotLevel = m_nLastSlotLevel;
                        slotIndexUpdate.nSlotType = m_data.nSlotType;
                        slotIndexUpdate.nID = m_nLastSpellID;
                        // 槽位索引更新事件
                        pEntityEvent->FireExecute(EVENT_SLOTINDEX_UPDATE, (char *)&slotIndexUpdate, sizeof(slotIndexUpdate));

                        // 槽位改变,冷却也需要改变
                        effectAddFreezeTime(m_nLastSpellID);
                    }
                }
                m_nLastSpellID = 0;
            }

            m_pEntity = 0;
        }
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectReplaceSlotSpell(m_data);
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
        case EVENT_UPDATE_SPELL:
            {
                if (m_pEntity == NULL)
                {
                    break;
                }

                __ISpellPart *pSpellPart = (__ISpellPart *)m_pEntity->getEntityPart(PART_SPELL);
                if (pSpellPart == NULL)
                {
                    break;
                }

                __IEntityEvent *pEntityEvent = m_pEntity->getEntityEvent();
                if (pEntityEvent == NULL)
                {
                    break;
                }

                if (pszContext == NULL || nLen != sizeof(event_entity_update_spell))
                {
                    break;
                }
                event_entity_update_spell *pUpdateSpell = (event_entity_update_spell*)pszContext;

                if (pUpdateSpell->nSlotType != m_data.nSlotType || pUpdateSpell->nSlotIndex != m_data.nSlotIndex)
                {
                    break;
                }

                if (pUpdateSpell->nSlotLevel > (int)m_vSpellID.size())
                {
                    break;
                }

                int nSpellID = m_vSpellID[pUpdateSpell->nSlotLevel - 1];
                if (nSpellID <= 0)
                {
                    break;
                }

                // 删除上次学会技能
                if (m_nLastLearnSpellID > 0)
                {
                    pSpellPart->removeSpell(m_nLastLearnSpellID);
                }
                // 设置上次学会技能
                m_nLastLearnSpellID = nSpellID;

                m_nLastSpellID = pUpdateSpell->nSpellID;
                m_nLastSlotLevel = pUpdateSpell->nSlotLevel;

                // 学会替换技能
                pSpellPart->addSpell(nSpellID);

                // 槽位索引更新
                event_slotindex_update slotIndexUpdate;
                slotIndexUpdate.bAdd = true;
                slotIndexUpdate.nSlotIndex = pUpdateSpell->nSlotIndex;
                slotIndexUpdate.nSlotLevel = pUpdateSpell->nSlotLevel;
                slotIndexUpdate.nSlotType = pUpdateSpell->nSlotType;
                slotIndexUpdate.nID = nSpellID;
                // 槽位索引更新事件
                pEntityEvent->FireExecute(EVENT_SLOTINDEX_UPDATE, (char *)&slotIndexUpdate, sizeof(slotIndexUpdate));

                // 槽位改变,冷却也需要改变
                effectAddFreezeTime(nSpellID);
            }
            break;
        case EVENT_ENTITY_PRE_REPLACE_SPELL:
            {
                if (pszContext == NULL || nLen != sizeof(event_prereplace_spell))
                {
                    break;
                }
                event_prereplace_spell* pEvent = (event_prereplace_spell*)pszContext;
                if(pEvent->nSlotType == m_data.nSlotType && pEvent->nSlotIndex == m_data.nSlotIndex)
                {
                    Stop();
                }
            }
            break;
		default:
			break;
		}
	}

    void effectAddFreezeTime(int nSpellID)
    {
        if (m_pEntity == NULL)
        {
            return;
        }

        __IFreezePart *pFreezePart = (__IFreezePart *)m_pEntity->getEntityPart(PART_FREEZE);
        if (pFreezePart == NULL)
        {
            return;
        }

        __ISpellPart *pSpellPart = (__ISpellPart *)m_pEntity->getEntityPart(PART_SPELL);
        if (pFreezePart == NULL)
        {
            return;
        }

        ISpell *pSpell = pSpellPart->findSpell(nSpellID);
        if (pSpell == NULL)
        {
            return;
        }

        pFreezePart->AddSpellBreakFreezeTime(EFreeze_ClassID_Spell, pSpell->getSpellData(SPDATA_COOLID), 0);
    }

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;

    // 被替换技能ID
    int                     m_nByReolaceSpellID;

    /////////////可升级技能中途升级,需要的数值记录来处理/////////////
    // 上次技能
    int                     m_nLastSpellID;
    // 上次槽位等级
    int                     m_nLastSlotLevel;
    // 技能ID容器
    vector<int>				m_vSpellID;
    // 上次学会技能技能
    int                     m_nLastLearnSpellID;
    /////////////可升级技能中途升级,需要的数值记录来处理/////////////
};
