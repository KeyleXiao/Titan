/*******************************************************************
** �ļ���:	EffectSetWing.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	1/21/2016
** ��  ��:	1.0
** ��  ��:	Ч��-���ó��
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IWingPart.h"

using namespace EFFECT_SERVER;

class CEffectSetWing : public IEffectEx
{
public:
	typedef  EffectServer_SetWing    SCHEME_DATA;

	CEffectSetWing( SCHEME_DATA & data ) : m_data(data),m_pEntity(NULL)
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

        m_pEntity = context->pEntity;

		__IWingPart *pWingPart = (__IWingPart *)m_pEntity->getEntityPart(PART_WING);
		if (pWingPart == NULL)
		{
			return false;
		}
		// ���ó��ID
		return pWingPart->SetWingID(m_data.nWingID);
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
        if (m_pEntity != NULL)
        {
            __IWingPart *pWingPart = (__IWingPart *)m_pEntity->getEntityPart(PART_WING);
            if (pWingPart != NULL)
            {
                // ��ճ������
                pWingPart->SetWingID(0);
            }

            m_pEntity = NULL;
        }
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone() 
	{
		return new CEffectSetWing(m_data);
	}

	// �ͷ�
	virtual void			Release() 
	{
		Stop();
		delete this;
	}

private:
	SCHEME_DATA               m_data;
    __IEntity                 *m_pEntity;
};
