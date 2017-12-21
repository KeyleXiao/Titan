/*******************************************************************
** 文件名:	EffectReplaceSlotSpellGroup.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/20/2016
** 版  本:	1.0
** 描  述:	效果-替换槽位技能组
            按槽位当前等级替换技能列表替换，槽位对应等级恢复技能列表替换
            http://172.16.0.120/redmine/issues/2097
** 修  改:	这个效果替换的技能在切换的瞬间，当时放的技能效果全部都没有了，包括动作，光效
            8-29修改,不移除切换技能,因为此技能可能还未结束导致
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"
#include "ISlotPart.h"

class CEffectReplaceSlotSpellGroup : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_ReplaceSlotSpellGroup    SCHEME_DATA;

	CEffectReplaceSlotSpellGroup( SCHEME_DATA &data ) : m_data(data),m_pEntity(0)
	{
        vector<string> tempVector;

        tempVector.clear();
        m_vReplaceSpellID.clear();
        StringHelper::split(tempVector, m_data.strReplaceSpell, ';', " ");
        for (UINT nCount=0; nCount<tempVector.size(); ++nCount)
        {
            m_vReplaceSpellID.push_back(StringHelper::toInt(tempVector[nCount]));
        }

        tempVector.clear();
        m_vSpellID.clear();
        StringHelper::split(tempVector, m_data.strSpell, ';', " ");
        for (UINT nCount=0; nCount<tempVector.size(); ++nCount)
        {
            m_vSpellID.push_back(StringHelper::toInt(tempVector[nCount]));
        }

        if (m_vReplaceSpellID.size() != m_vSpellID.size())
        {
            ErrorLn("nID="<<GetID()<<",error count no same!");
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

        __ISpellPart *pSpellPart = (__ISpellPart *)m_pEntity->getEntityPart(PART_SPELL);
        if (pSpellPart == NULL)
        {
            return false;
        }

        int nSlotLevel = pSpellPart->getSpellSlotLevel(m_data.nSlotIndex);
        if (nSlotLevel <= 0)
        {
            return false;
        }
        if (nSlotLevel > (int)m_vReplaceSpellID.size())
        {
            return false;
        }

        // 发送准备替换技能事件
        event_prereplace_spell preReplaceSpell;
        preReplaceSpell.nSlotType = m_data.nSlotType;
        preReplaceSpell.nSlotIndex = m_data.nSlotIndex;
        pEntityEvent->FireExecute(EVENT_ENTITY_PRE_REPLACE_SPELL, (LPCSTR)&preReplaceSpell, sizeof(preReplaceSpell));

        // 取得替换技能ID
        int nSpellID = m_vReplaceSpellID[nSlotLevel - 1];
        // 学会替换技能
        pSpellPart->addSpell(nSpellID);

        // 槽位索引更新
        event_slotindex_update slotIndexUpdate;
        slotIndexUpdate.bAdd = true;
        slotIndexUpdate.nSlotIndex = m_data.nSlotIndex;
        slotIndexUpdate.nSlotLevel = nSlotLevel;
        slotIndexUpdate.nSlotType = m_data.nSlotType;
        slotIndexUpdate.nID = nSpellID;
        pEntityEvent->FireExecute(EVENT_SLOTINDEX_UPDATE, (char *)&slotIndexUpdate, sizeof(slotIndexUpdate));

        // 槽位改变,冷却也需要改变
        effectAddFreezeTime(nSpellID);

        // 是可升级技能
        if (m_data.nSlotType == SLOT_TYPE_SKILL && m_data.nSlotIndex < SPELL_UPGRADE_SLOT_COUNT)
        {
            g_EHelper.Subscibe(m_pEntity, this, EVENT_UPDATE_SPELL, "CEffectReplaceSlotSpellGroup");
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

            if(!m_data.bNeverRestore)
            {
                do 
                {
                    __IEntityEvent *pEntityEvent = m_pEntity->getEntityEvent();
                    if (pEntityEvent == NULL)
                    {
                        break;
                    }

                    __ISpellPart *pSpellPart = (__ISpellPart *)m_pEntity->getEntityPart(PART_SPELL);
                    if (pSpellPart == NULL)
                    {
                        break;
                    }

                    int nSlotLevel = pSpellPart->getSpellSlotLevel(m_data.nSlotIndex);
                    if (nSlotLevel <= 0)
                    {
                        break;
                    }
                    if (nSlotLevel > (int)m_vSpellID.size())
                    {
                        break;
                    }

                    // 取得恢复技能ID
                    int nSpellID = m_vSpellID[nSlotLevel - 1];
                    // 学会替换技能
                    pSpellPart->addSpell(nSpellID);

                    // 槽位索引更新
                    event_slotindex_update slotIndexUpdate;
                    slotIndexUpdate.bAdd = true;
                    slotIndexUpdate.nSlotIndex = m_data.nSlotIndex;
                    slotIndexUpdate.nSlotLevel = nSlotLevel;
                    slotIndexUpdate.nSlotType = m_data.nSlotType;
                    slotIndexUpdate.nID = nSpellID;
                    pEntityEvent->FireExecute(EVENT_SLOTINDEX_UPDATE, (char *)&slotIndexUpdate, sizeof(slotIndexUpdate));

                    // 槽位改变,冷却也需要改变
                    effectAddFreezeTime(nSpellID);

                } while (false);
            }

            m_pEntity = 0;
        }
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectReplaceSlotSpellGroup(m_data);
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

                int nSlotLevel = pUpdateSpell->nSlotLevel;
                if (nSlotLevel <= 0)
                {
                    break;
                }
                if (nSlotLevel > (int)m_vReplaceSpellID.size())
                {
                    break;
                }

                // 取得替换技能ID
                int nSpellID = m_vReplaceSpellID[nSlotLevel - 1];
                // 学会替换技能
                pSpellPart->addSpell(nSpellID);

                // 槽位索引更新
                event_slotindex_update slotIndexUpdate;
                slotIndexUpdate.bAdd = true;
                slotIndexUpdate.nSlotIndex = m_data.nSlotIndex;
                slotIndexUpdate.nSlotLevel = nSlotLevel;
                slotIndexUpdate.nSlotType = m_data.nSlotType;
                slotIndexUpdate.nID = nSpellID;
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

    // 替换技能ID容器
    vector<int>				m_vReplaceSpellID;

    // 恢复技能ID容器
    vector<int>				m_vSpellID;
};
