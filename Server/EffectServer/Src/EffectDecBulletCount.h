/*******************************************************************
** �ļ���:	EffectDecBulletCount.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	ljw
** ��  ��:	7/21/2017
** ��  ��:	1.0
** ��  ��:	Ч��-�����ӵ�����
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntityProperty.h"
using namespace EFFECT_SERVER;

class CEffectDecBulletCount : public IEffectEx
{
public:
	typedef  EffectServer_DecBulletCount    SCHEME_DATA;

	CEffectDecBulletCount( SCHEME_DATA & data ) : m_data(data)
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

		__IEntityProperty* pProperty = (__IEntityProperty*)(context->pEntity->getEntityPart(PART_PROPERTY));

		int nOriCount = pProperty->getProperty_Integer(PROPERTY_BULLET_COUNT);
		int nNowCount = nOriCount - m_data.nCount;
		if(nNowCount < 0)
		{
			nNowCount = 0;
		}

		pProperty->setProperty_Integer(PROPERTY_BULLET_COUNT, nNowCount);

		return true;
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone() 
	{
		return new CEffectDecBulletCount(m_data);
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
