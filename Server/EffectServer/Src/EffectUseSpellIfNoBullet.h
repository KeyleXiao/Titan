/*******************************************************************
** �ļ���:	EffectUseSpellIfNoBullet.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	ljw
** ��  ��:	7/21/2017
** ��  ��:	1.0
** ��  ��:	Ч��-���û���ӵ���ʹ��ĳ������
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

	// ��ȡЧ��ID
	virtual int             GetID()
	{
		return m_data.nID;
	}

	// Ч������
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

	// Ч��ֹͣ
	virtual void			Stop()
	{
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone() 
	{
		return new CEffectUseSpellIfNoBullet(m_data);
	}

	// �ͷ�
	virtual void			Release() 
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA               m_data;
};
