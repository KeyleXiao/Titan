/*******************************************************************
** �ļ���:	EffectLessSummonMonster.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	4/21/2015
** ��  ��:	1.0
** ��  ��:	Ч��-�����ٻ�����
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"

using namespace EFFECT_SERVER;

class CEffectLessSummonMonster : public IEffectEx
{
public:
	typedef  EffectServer_LessSummonMonster    SCHEME_DATA;

	CEffectLessSummonMonster( SCHEME_DATA & data ) : m_data(data)
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
		if (context == NULL || context->pEntity == NULL)
		{
			return false;
		}
		UID uidTarget = context->uidTarget;
		if (isInvalidUID(uidTarget))
		{
			return false;
		}
		if (!isMonster(uidTarget))
		{
			return false;
		}
		// ȡ�ù���ID
		int nMonsterID = getProperty_Integer(uidTarget, PROPERTY_ID);
		if (nMonsterID == 0)
		{
			return false;
		}
		if (m_data.nMonsterID > 0 && m_data.nMonsterID != nMonsterID)
		{
			return false;
		}

		__ISpellPart *pSpellPart = (__ISpellPart *)context->pEntity->getEntityPart(PART_SPELL);
		if (pSpellPart == NULL)
		{
			return false;
		}
		ISpell *pSpell = pSpellPart->findSpell(context->nID);
		if ( pSpell == 0 )
		{
			return false;
		}
		// �Ƴ��ٻ�ʵ��
		pSpell->removeEntity(uidTarget);

		return true;
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone() 
	{
		return new CEffectLessSummonMonster(m_data);
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
