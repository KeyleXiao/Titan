/*******************************************************************
** 文件名:	EffectSetSpellSlot.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/30/2015
** 版  本:	1.0
** 描  述:	效果-设置技能槽
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IClientEntity.h"
#include "IClientGlobal.h"
#include "IFreezeClient.h "
using namespace EFFECT_CLIENT;

class CEffectSetSpellSlot : public IEffect
{
public:
	typedef EffectClient_SetSpellSlot SCHEME_DATA;

	CEffectSetSpellSlot( SCHEME_DATA & data ):m_data(data)
	{
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * pContext )
	{
		if ( pContext==0 )
			return false;

		IEntity *pEntity = pContext->pEntity;
		if (pEntity == 0)
		{
			return false;
		}

		ISpellPart *pSpellPart = (ISpellPart *)pEntity->getEntityPart(PART_SPELL);
		if (pSpellPart == NULL)
		{
			return false;
		}

		int nSpellIcon = 0;
		int nCoolID = 0;
		char szSlotDesc[SLOT_DESC_LEN] = {0};
        int nOverlaySpell = 0;
		if (m_data.nSpellFindType == SPELL_FIND_SPELL_PART)
		{
			ISpell *pSpell = pSpellPart->findSpell(m_data.nSpellID);
			if(pSpell ==NULL)
			{
				return false;
			}

			nSpellIcon = pSpell->getSpellData(SPDATA_ICON);
			nCoolID = pSpell->getSpellData(SPDATA_COOLID);
			sstrcpyn(szSlotDesc, pSpell->getSpellDataStr(SPDATA_DESC), sizeof(szSlotDesc));
            int nSpellFlag = pSpell->getSpellData(SPDATA_FLAG);
            if ((nSpellFlag&SpellFlag_Overlay) == SpellFlag_Overlay)
            {
                nOverlaySpell = 1;
            }
		}
		else
		{
			if (m_data.nSpellID > 0)
			{
				ISpellFactory *pSpellFactory = gClientGlobal->getSpellFactory();
				if (pSpellFactory == NULL)
				{
					return false;
				}
				SPELL_DATA *pSpellData = pSpellFactory->getSpellData(m_data.nSpellID);
				if (pSpellData == NULL)
				{
					return false;
				}

				nSpellIcon = pSpellData->nIcon;
				nCoolID = pSpellData->nCoolID;
				sstrcpyn(szSlotDesc, pSpellData->szDesc, sizeof(szSlotDesc));
                int nSpellFlag = pSpellData->nSpellFlag;
                if ((nSpellFlag&SpellFlag_Overlay) == SpellFlag_Overlay)
                {
                    nOverlaySpell = 1;
                }
			}
		}

		// 取得冷却部件
		IFreezePart *pFreezePart = (IFreezePart*)pEntity->getEntityPart(PART_FREEZE);
		if (pFreezePart == NULL)
		{
			return false;
		}

		IFreezeClient *pFreezeClient = gClientGlobal->getFreezeClient();
		if (pFreezeClient == NULL)
		{
			return false;
		}
		
		int nTotalTime = 0;
		int nSpellFreezeTime = pFreezePart->GetFreezeInfo(EFreeze_ClassID_Spell, nCoolID, nTotalTime);
		nTotalTime = pFreezeClient->GetFreezeShowTime(EFreeze_ClassID_Spell, nCoolID);

		cmd_creature_set_slot setSpellSlot;
		setSpellSlot.nSlotIndex = m_data.nSlotIndex;
		setSpellSlot.nSlotType = m_data.nSlotType;
		setSpellSlot.nSlotLevel = max(m_data.nSlotLevel, pSpellPart->getSpellSlotLevel(m_data.nSlotIndex));
        setSpellSlot.nOverlaySpell = nOverlaySpell;
		setSpellSlot.slotview.nType		= EFreeze_ClassID_Spell;
		setSpellSlot.slotview.nCoolID	= nCoolID;
		setSpellSlot.slotview.nId		= m_data.nSpellID;
		setSpellSlot.slotview.nViewType	= ViewType_LeftCircle;		// 显示类型（方式）：0101 1111
		setSpellSlot.slotview.nCurData	= nSpellFreezeTime;			// 槽位当前相关数据（根据显示类型（方式）判定是否启用 nMaxData）
		setSpellSlot.slotview.nMaxData	= nTotalTime;				// 最大相关数据
		setSpellSlot.slotview.nIconID	= nSpellIcon;				// 图片ID
		setSpellSlot.slotview.bEnable	= true;						// 是否可用
		setSpellSlot.slotview.bCanDrag	= false;					// 能否拖动
		setSpellSlot.slotview.bIsOnDrag	= false;					// 拖动状态
		setSpellSlot.slotview.nSlotGroupType = 1;					// 槽位组类型
		sstrcpyn(setSpellSlot.slotview.szSlotDesc, a2utf8(szSlotDesc), sizeof(setSpellSlot.slotview.szSlotDesc));
		// 设置技能槽
		pEntity->sendCommandToEntityView(ENTITY_TOVIEW_SET_SPELL_SLOT, 0, 0, &setSpellSlot, sizeof(setSpellSlot));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() { return new CEffectSetSpellSlot(m_data); }

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
};
