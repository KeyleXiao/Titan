/*******************************************************************
** 文件名:	EffectCastSpell.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	9/7/2016
** 版  本:	1.0
** 描  述:	效果-使用技能
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_CLIENT;
#include "SlotDef.h"

class CEffectCastSpell : public IEffect
{
public:
	typedef EffectClient_CastSpell SCHEME_DATA;

	CEffectCastSpell( SCHEME_DATA & data ) : m_data(data)
	{
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT *pContext )
	{
		if ( pContext==0 || pContext->pEntity==0 )
        {
			return false;
        }

        ISpellPart *pSpellPart = (ISpellPart *)pContext->pEntity->getEntityPart(PART_SPELL);
        if (pSpellPart == NULL)
        {
            return false;
        }

        SSlotSkillInfo sSlotInfo;
        sSlotInfo.nSkillID = m_data.nSpellID;
        pSpellPart->spellAttribute(sSlotInfo);

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() { return new CEffectCastSpell(m_data); }

	// 取得效果ID
	virtual int				GetEffectID(){ return m_data.nID; }

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA             m_data;
};
