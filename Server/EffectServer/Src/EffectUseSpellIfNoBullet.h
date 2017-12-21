/*******************************************************************
** 文件名:	EffectUseSpellIfNoBullet.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	ljw
** 日  期:	7/21/2017
** 版  本:	1.0
** 描  述:	效果-如果没有子弹了使用某个技能
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "ISpell.h"
#include "SpellContextDef.h"
using namespace EFFECT_SERVER;

class CEffectUseSpellIfNoBullet : public IEffectEx
{
public:
	typedef  EffectServer_UseSpellIfNoBullet    SCHEME_DATA;

	CEffectUseSpellIfNoBullet( SCHEME_DATA & data ) : m_data(data)
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

		int nBulletCount = context->pEntity->getIntProperty(PROPERTY_BULLET_COUNT);
		if(nBulletCount <= 0)
		{
			__ISpellPart* pSpellPart = (__ISpellPart*)context->pEntity->getEntityPart(PART_SPELL);
			if(pSpellPart != NULL)
			{
				SPELL_CONTEXT spellContext;
				Vector3 loc = context->pEntity->getLocation();
				UID uid = context->pEntity->getUID();
				DWORD now = getTickCountEx();
				spellContext.moveState.vPosition = loc;
				spellContext.CC.pEntity = context->pEntity;
				spellContext.CC.uidOperator = uid;
				spellContext.CC.ptStartTile = loc;
				spellContext.CC.nID = m_data.nSpellID;	
				spellContext.CC.dwStartTick = now;
				spellContext.EC.pEntity = context->pEntity;
				spellContext.EC.uidOperator = uid;
				spellContext.EC.ptStartTile = loc;
				spellContext.EC.nID = m_data.nSpellID;
				spellContext.EC.dwStartTick = now;

				return pSpellPart->castSpell(&spellContext);
			}
		}

		return false;
	}

	// 效果停止
	virtual void			Stop()
	{
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectUseSpellIfNoBullet(m_data);
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
