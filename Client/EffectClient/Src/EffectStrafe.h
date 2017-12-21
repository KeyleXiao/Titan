/*******************************************************************
** 文件名:	EffectStrafe.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/01/2015
** 版  本:	1.0
** 描  述:	效果-扫射效果
			适用于吟唱技能效果
			传入时间，次数，由显示层返回命令次数
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
using namespace EFFECT_CLIENT;

class CEffectStrafe : public IEffect
{
public:
	typedef  EffectClient_Strafe    SCHEME_DATA;

	CEffectStrafe( SCHEME_DATA & data ) : m_data(data)
	{
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * context )
	{
		if ( context==0 || context->pEntity==0 )
			return false;

		ISpellPart *pSpellPart = (ISpellPart *)context->pEntity->getEntityPart(PART_SPELL);
		if (pSpellPart == NULL)
			return false;

		ISpell *pSpell = pSpellPart->findSpell(m_data.nSpellID);
		if (pSpell == NULL)
			return false;

		// 未使用技能
		if (!pSpell->isWorking())
			return false;

		cmd_effect_Strafe strafe;
		strafe.nSelectType = pSpell->getSpellData(SPDATA_SELECT_TYPE);
		strafe.fDistance = (float)pSpell->getSpellData(SPDATA_DISTANCE)/SPELL_FLOAT_VALUE;
		strafe.fAttackRange = (float)pSpell->getSpellData(SPDATA_ATTACK_RANGE)/SPELL_FLOAT_VALUE;
		strafe.nID = context->nID;
		strafe.nTime = m_data.nTime;
		strafe.nCount = m_data.nCount;
		strafe.nLightID = m_data.nLightID;
        strafe.fFlyingSpeed = pSpell->getSpellData( SPDATA_FLYING_SPEED )/SPELL_FLOAT_VALUE;
        strafe.nStrafeType = m_data.nStrafeType;
        strafe.nDrawType = m_data.nDrawType;
        strafe.nTargetType = pSpell->getSpellData(SPDATA_SELECT_TARGET);

		// 扫射命令
		context->pEntity->sendCommandToEntityView( ENTITY_TOVIEW_EFFECT_STRAFE, 0, 0, &strafe, sizeof(strafe));

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectStrafe(m_data);
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

private:
	SCHEME_DATA               m_data;
};
