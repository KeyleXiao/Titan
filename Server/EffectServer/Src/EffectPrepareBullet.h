/*******************************************************************
** �ļ���:	EffectPrepareBullet.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	ljw
** ��  ��:	7/21/2017
** ��  ��:	1.0
** ��  ��:	Ч��-׼���ӵ�
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "ISpell.h"
using namespace EFFECT_SERVER;

class CEffectPrepareBullet : public IEffectEx
{
public:
	typedef  EffectServer_PrepareBullet    SCHEME_DATA;

	CEffectPrepareBullet( SCHEME_DATA & data ) : m_data(data)
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

		context->pEntity->setIntProperty(PROPERTY_BULLET_COUNT, m_data.nBulletCount);

		return true;
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone() 
	{
		return new CEffectPrepareBullet(m_data);
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
