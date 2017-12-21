/*******************************************************************
** �ļ���:	EffectAddEnmityValue.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	10/28/2015
** ��  ��:	1.0
** ��  ��:	��Ŀ����ӳ��
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEnmityServer.h"
#include "EntityHelper.h"

class CEffectAddEnmityValue:public IEffectEx
{
public:
	typedef  EffectServer_AddEnmity   SCHEME_DATA;

	CEffectAddEnmityValue(SCHEME_DATA &data):m_data(data)
	{
	}

    // ��ȡЧ��ID
    virtual int             GetID()
    {
        return m_data.nID;
    }

	// Ч������
	virtual bool			Start(EFFECT_CONTEXT *pContext,CONDITION_CONTEXT *pConditionContext)
	{
		if (pContext == NULL || pContext->pEntity == NULL)
		{
			return false;
		}

		//�����е�ʵ��id
		UID uidTarget=pContext->uidTarget;
		//Ч������������id
		UID uidOperator=pContext->uidOperator;

		if (isInvalidUID(uidTarget) || isInvalidUID(uidOperator))
		{
			return false;
		}
        //���ӳ��uidTarget
		AddEnmityValue(uidTarget,uidOperator,m_data.nEnmityValue);
		return true;
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone()
	{
		return new CEffectAddEnmityValue(m_data);
	}

	// �ͷ�
	virtual void			Release()
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA             m_data;
};
