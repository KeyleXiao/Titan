/*******************************************************************
** �ļ���:	EffectFakeDeadRelive.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	���к�
** ��  ��:	2017/12/1
** ��  ��:	1.0
** ��  ��:	Ч��-�����߸���
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "EntityHelper.h"
#include "IWarPart.h"
#include "WarHelper.h"

class CEffectFakeDeadRelive: public IEffectEx
{
public:
	typedef  EffectServer_FakeDeadRelive    SCHEME_DATA;

	CEffectFakeDeadRelive( SCHEME_DATA &data ) : m_data(data),m_pEntity(0)
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
		if ( context==0 || context->pEntity==0 )
			return false;

		m_pEntity = context->pEntity;

		__IWarPart* pWarPart = (__IWarPart*)m_pEntity->getEntityPart(PART_WAR);
		if (pWarPart == NULL)
		{
			return false;
		}

		DWORD dwWarID = pWarPart->getWarID();
		if (dwWarID == INVALID_WAR_ID)
		{
			return false;
		}

		WarHelper warHelper(dwWarID);
		IWarService* pWarService = warHelper.m_ptr;
		if (pWarService == NULL)
		{
			return false;
		}

		pWarService->onFakeDeadRelive(m_pEntity->getUID());

		return true;
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			m_pEntity = 0;
		}
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone()
	{
		return new CEffectFakeDeadRelive(m_data);
	}

	// �ͷ�
	virtual void			Release()
	{
		Stop();
		delete this;
	}


private:
	SCHEME_DATA             m_data;

	// ʵ��ָ��
	__IEntity				*m_pEntity;
};
