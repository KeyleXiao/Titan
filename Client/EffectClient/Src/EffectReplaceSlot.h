/*******************************************************************
** 文件名:	EffectReplaceSlot.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	2/03/2015
** 版  本:	1.0
** 描  述:	效果-替换技能槽
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IClientEntity.h"
#include "IClientGlobal.h"
#include "IFreezeClient.h "
using namespace EFFECT_CLIENT;

class CEffectReplaceSlot : public IEffect
{
public:
	typedef EffectClient_ReplaceSlot SCHEME_DATA;

	CEffectReplaceSlot( SCHEME_DATA & data ):m_data(data),m_pEntity(0)
	{
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * pContext )
	{
		if ( pContext==0 )
			return false;

		m_pEntity = pContext->pEntity;
		if (m_pEntity == 0)
			return false;

		ISpellPart *pSpellPart = (ISpellPart *)m_pEntity->getEntityPart(PART_SPELL);
		if (pSpellPart == NULL)
			return false;

		ISpell *pSpell = pSpellPart->findSpell(m_data.nReplaceSpellID);
		if(pSpell == NULL)
			return false;

		// 取得冷却部件
		IFreezePart *pFreezePart = (IFreezePart*)m_pEntity->getEntityPart(PART_FREEZE);
		if (pFreezePart == NULL)
			return false;

		IFreezeClient *pFreezeClient = gClientGlobal->getFreezeClient();
		if (pFreezeClient == NULL)
			return false;
		
		int nSpellIcon = pSpell->getSpellData(SPDATA_ICON);
		int nCoolID = pSpell->getSpellData(SPDATA_COOLID);
		int nTotalTime = 0;
		int nSpellFreezeTime = pFreezePart->GetFreezeInfo(EFreeze_ClassID_Spell, nCoolID, nTotalTime);
		nTotalTime = pFreezeClient->GetFreezeShowTime(EFreeze_ClassID_Spell, nCoolID);

		int nSpellID = 0;
		switch (m_data.nReplaceSlotType)
		{
		case REPLACE_SLOT_SPELL:
			{
				nSpellID = m_data.nReplaceSpellID;
			}
			break;
		case REPLACE_SLOT_ICON:
			{
				nSpellID = m_data.nSpellID;
			}
			break;
		default:
			{
				ErrorLn("EffectClient_ReplaceSlot error nReplaceSlotType="<<m_data.nReplaceSlotType);
				return false;
			}
			break;
		}

        int nOverlaySpell = 0;
        do 
        {
            ISpell *pSpell = pSpellPart->findSpell(nSpellID);
            if(pSpell == NULL)
            {
                break;
            }
            int nSpellFlag = pSpell->getSpellData(SPDATA_FLAG);
            if ((nSpellFlag&SpellFlag_Overlay) == SpellFlag_Overlay)
            {
                nOverlaySpell = 1;
            }
        } while (false);

		cmd_creature_set_slot setSpellSlot;
		setSpellSlot.nSlotIndex = m_data.nSlotIndex;
		setSpellSlot.nSlotType = SLOT_TYPE_SKILL;
		setSpellSlot.nSlotLevel = pSpellPart->getSpellSlotLevel(m_data.nSlotIndex);
        setSpellSlot.nOverlaySpell = nOverlaySpell;
		setSpellSlot.slotview.nType		= EFreeze_ClassID_Spell;
		setSpellSlot.slotview.nId		= nSpellID;
		setSpellSlot.slotview.nViewType	= ViewType_LeftCircle;		// 显示类型（方式）：0101 1111
		setSpellSlot.slotview.nCurData	= nSpellFreezeTime;			// 槽位当前相关数据（根据显示类型（方式）判定是否启用 nMaxData）
		setSpellSlot.slotview.nMaxData	= nTotalTime;				// 最大相关数据
		setSpellSlot.slotview.nIconID	= nSpellIcon;				// 图片ID
		setSpellSlot.slotview.bEnable	= true;						// 是否可用
		setSpellSlot.slotview.bCanDrag	= false;					// 能否拖动
		setSpellSlot.slotview.bIsOnDrag	= false;					// 拖动状态
		setSpellSlot.slotview.nSlotGroupType = 1;					// 槽位组类型
		sstrcpyn(setSpellSlot.slotview.szSlotDesc, a2utf8(pSpell->getSpellDataStr(SPDATA_DESC)), sizeof(setSpellSlot.slotview.szSlotDesc));
		// 设置技能槽
		m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_SET_SPELL_SLOT, 0, 0, &setSpellSlot, sizeof(setSpellSlot));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity == NULL)
		{
			return;
		}

		// 结束的时候恢复技能替换
		do 
		{
            // 恢复的时候，使用原来技能即可
            int nSpellID = m_data.nSpellID;

			ISpellPart *pSpellPart = (ISpellPart *)m_pEntity->getEntityPart(PART_SPELL);
			if (pSpellPart == NULL)
				break;

			ISpell *pSpell = pSpellPart->findSpell(nSpellID);
			if(pSpell ==NULL)
				break;

			// 取得冷却部件
			IFreezePart *pFreezePart = (IFreezePart*)m_pEntity->getEntityPart(PART_FREEZE);
			if (pFreezePart == NULL)
				break;

			IFreezeClient *pFreezeClient = gClientGlobal->getFreezeClient();
			if (pFreezeClient == NULL)
				break;

			int nSpellIcon = pSpell->getSpellData(SPDATA_ICON);
			int nCoolID = pSpell->getSpellData(SPDATA_COOLID);
			int nTotalTime = 0;
			int nSpellFreezeTime = pFreezePart->GetFreezeInfo(EFreeze_ClassID_Spell, nCoolID, nTotalTime);
			nTotalTime = pFreezeClient->GetFreezeShowTime(EFreeze_ClassID_Spell, nCoolID);

            int nOverlaySpell = 0;
            int nSpellFlag = pSpell->getSpellData(SPDATA_FLAG);
            if ((nSpellFlag&SpellFlag_Overlay) == SpellFlag_Overlay)
            {
                nOverlaySpell = 1;
            }

			cmd_creature_set_slot setSpellSlot;
			setSpellSlot.nSlotIndex = m_data.nSlotIndex;
			setSpellSlot.nSlotType = SLOT_TYPE_SKILL;
			setSpellSlot.nSlotLevel = pSpellPart->getSpellSlotLevel(m_data.nSlotIndex);
            setSpellSlot.nOverlaySpell = nOverlaySpell;
			setSpellSlot.slotview.nType		= EFreeze_ClassID_Spell;
			setSpellSlot.slotview.nId		= nSpellID;
			setSpellSlot.slotview.nViewType	= ViewType_LeftCircle;		// 显示类型（方式）：0101 1111
			setSpellSlot.slotview.nCurData	= nSpellFreezeTime;			// 槽位当前相关数据（根据显示类型（方式）判定是否启用 nMaxData）
			setSpellSlot.slotview.nMaxData	= nTotalTime;				// 最大相关数据
			setSpellSlot.slotview.nIconID	= nSpellIcon;				// 图片ID
			setSpellSlot.slotview.bEnable	= true;						// 是否可用
			setSpellSlot.slotview.bCanDrag	= false;					// 能否拖动
			setSpellSlot.slotview.bIsOnDrag	= false;					// 拖动状态
			setSpellSlot.slotview.nSlotGroupType = 1;					// 槽位组类型
			sstrcpyn(setSpellSlot.slotview.szSlotDesc, a2utf8(pSpell->getSpellDataStr(SPDATA_DESC)), sizeof(setSpellSlot.slotview.szSlotDesc));
			// 设置技能槽
			m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_SET_SPELL_SLOT, 0, 0, &setSpellSlot, sizeof(setSpellSlot));
		} while (false);
		
		m_pEntity = NULL;
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() { return new CEffectReplaceSlot(m_data); }

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	// 取得效果ID
	virtual int				GetEffectID() { return m_data.nID; }
private:
	SCHEME_DATA             m_data;
	IEntity					*m_pEntity;
};
