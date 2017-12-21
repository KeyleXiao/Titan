/*******************************************************************
** �ļ���:	EffectCheckBuffDamageBonus.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	8/30/2016
** ��  ��:	1.0
** ��  ��:	Ч��-���BUFF�˺��ӳ�
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"
#include "DamageDef.h"
#include "EntityHelper.h"
#include "EventDef.h"

class CEffectCheckBuffDamageBonus : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_CheckBuffDamageBonus    SCHEME_DATA;

	CEffectCheckBuffDamageBonus( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_uidCheck(0)
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

        switch (m_data.nTargetType)
        {
        case MA_TAGET_TO_SELF:
            {
                m_uidCheck = m_pEntity->getUID();
            }
            break;
        case MA_TAGET_TO_TARGET:
            {
                m_uidCheck = context->uidTarget;
            }
            break;
        case MA_TAGET_TO_OPERATOR:
            {
                m_uidCheck = context->uidOperator;
            }
            break;
        default:
            {
                ErrorLn("CEffectCheckBuffAddBuff error nTargetType="<<m_data.nTargetType);
                return false;
            }
            break;
        }
        if (isInvalidUID(m_uidCheck))
        {
            return false;
        }

        // ע��׼���˺��¼�
        g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_DAMAGE, "CEffectAddDebuffOnDamage");

		return true;
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
            // ע��׼���˺��¼�
            g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_DAMAGE);

			m_pEntity = 0;
		}
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone()
	{
		return new CEffectCheckBuffDamageBonus(m_data);
	}

	// �ͷ�
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
		switch (wEventID)
		{
		case EVENT_ENTITY_PREPARE_DAMAGE:	// ׼���˺��¼�
			{
                if (m_pEntity == NULL)
                {
                    break;
                }

                if (pszContext == NULL || nLen != sizeof(DamageCalcContext))
                {
                    break;
                }
                DamageCalcContext *pDamage = (DamageCalcContext*)pszContext;

                // ���Ŀ�겻ƥ��
                if (m_uidCheck != pDamage->uidOperator)
                {
                    break;
                }

                int nSpellEffectType = pDamage->nSpellEffectType;
                // �жϼ���Ч������
                if ((m_data.nSpellEffectType & nSpellEffectType) != m_data.nSpellEffectType)
                {
                    break;
                }

                // �˺��ӳ�
                pDamage->nDamageHP += (int)(pDamage->nDamageHP * m_data.fValue);
			}
			break;
		default:
			break;
		}
	}

private:
	SCHEME_DATA             m_data;

	// ʵ��ָ��
	__IEntity				*m_pEntity;

    // ���Ŀ��
    UID                     m_uidCheck;
};
