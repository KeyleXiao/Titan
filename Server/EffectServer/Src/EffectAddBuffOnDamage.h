/*******************************************************************
** �ļ���:	EffectAddBuffOnDamage.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	�ΰ
** ��  ��:	7/16/2015
** ��  ��:	1.0
** ��  ��:	Ч��-�˺�ʱ���ʼ�buff
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
#include "ISpell.h"

class CEffectAddBuffOnDamage : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_AddBuffOnDamage    SCHEME_DATA;

	CEffectAddBuffOnDamage( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_dwLastTick(0)
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

        // ע���˺��¼�
        g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_DAMAGE, "CEffectAddDebuffOnDamage");

		return true;
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
            // ע���˺��¼�
            g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_DAMAGE);

			m_pEntity = 0;
		}
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone()
	{
		return new CEffectAddBuffOnDamage(m_data);
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
		case EVENT_ENTITY_DAMAGE:	// �˺��¼�
			{
                if (pszContext == NULL || nLen != sizeof(event_entity_damage))
                {
                    return;
                }
                if (m_pEntity == NULL)
                {
                    return;
                }

                // �Ƿ���CD
                DWORD dwCurrentTick = getTickCountEx();
                if (dwCurrentTick < m_dwLastTick + m_data.nCD)
                {
                    return;
                }

                // �����Ƿ�����
                int nRandNum = 0;
                rktRangeRand(nRandNum, 1, 100);
                if(nRandNum > m_data.fRate * 100)
                {
                    return;
                }

                event_entity_damage *pDamage = (event_entity_damage*)pszContext;

                int nSpellEffectType = pDamage->nSpellEffectType;
                // �жϼ���Ч������
                if ((m_data.nSpellEffectType & nSpellEffectType) != m_data.nSpellEffectType)
                {
                    return;
                }

				UID uidOperator = pDamage->uidOperator;
				if (isInvalidUID(uidOperator))
				{
					return;
				}

				UID uidTarget = pDamage->uidTarget;
				if (isInvalidUID(uidTarget))
				{
					return;
				}

				// ���Ŀ��
				if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetFilter, uidOperator))
				{
					return;
				}
				if ((pDamage->nDamageType&m_data.nDamageType) == 0)
				{
					return;
				}

                SBuffContext BuffContext;
                BuffContext.nID = pDamage->nID;

                UID uidAdd = INVALID_UID;
                // buff�����
                if(m_data.nAddType == MA_TAGET_TO_SELF)
                {
                    // ���˺�ʱ���Լ���buff �Լ������˺�Ŀ��
                    uidAdd = pDamage->uidTarget; 
                }
                else
                {
                    // ���˺�ʱ���˺������߼�buff
                    uidAdd = pDamage->uidOperator; 
                }

                // ��buff
                if(m_data.nTagetType == MA_TAGET_TO_SELF)
                {
                    // ���˺�ʱ���Լ���buff �Լ������˺�Ŀ��
                    AddBuff( pDamage->uidTarget, m_data.nBuffID, m_data.nBuffLevel, uidAdd, 0, &BuffContext, sizeof(BuffContext) );
                }
                else if(m_data.nTagetType == MA_TAGET_TO_TARGET)
                {
                    // ���˺�ʱ���˺������߼�buff
                    AddBuff( pDamage->uidOperator, m_data.nBuffID, m_data.nBuffLevel, uidAdd, 0, &BuffContext, sizeof(BuffContext) );
                }

                m_dwLastTick = dwCurrentTick;
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

	// �ϴ�ʱ��
	DWORD					m_dwLastTick;
};
