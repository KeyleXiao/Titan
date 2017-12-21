/*******************************************************************
** �ļ���:	EffectNotifyClientData.h 
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	��绪
** ��  ��:	3/8/2017
** ��  ��:	1.0
** ��  ��:	����֪ͨ�ͻ���
            http://172.16.0.120/redmine/issues/3939
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "ICommonPart.h"

using namespace EFFECT_SERVER;

class CEffectNotifyClientData : public IEffectEx
{
public:
	typedef  EffectServer_NotifyClientData    SCHEME_DATA;

	CEffectNotifyClientData( SCHEME_DATA & data ) : m_data(data),m_pEntity(NULL)
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
        // ����
        if (m_pEntity->getType() == TYPE_PLAYER_ROLE)
        {
             __ICommonPart * pPart = (__ICommonPart *)m_pEntity->getEntityPart(PART_COMMON);
            if(pPart)
            {
                pPart->notifyClientData(m_data.nType, m_data.nValue);
            }
        }

        return true;
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
        m_pEntity = NULL;
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone() 
	{
		return new CEffectNotifyClientData(m_data);
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
