/*******************************************************************
** �ļ���:	EffectApendDamage.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	�ΰ
** ��  ��:	7/16/2015
** ��  ��:	1.0
** ��  ��:	Ч��-�����˺�(ʹ����ͨ�������߼��ܹ���Ŀ��ʱ�и��ʸ����˺���������Ч��)
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

class CEffectApendDamage : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_ApendDamage    SCHEME_DATA;

	CEffectApendDamage( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_dwLastTick(0),m_dwLastAttackSerialID(0)
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

        // ע�ṥ���¼�(�����˺� ����ƮѪ)
        g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_ATTACK_EX, "CEffectApendDamage");

		return true;
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
            // ע�������¼�
            g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_ATTACK_EX);

			m_pEntity = 0;
		}
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone()
	{
		return new CEffectApendDamage(m_data);
	}

	// �ͷ�
	virtual void			Release()
	{
		Stop();
		delete this;
	}

    void CreateApendDamage(event_entity_attack_ex* pAttack)
    {
		// ������UID
		UID uidOperator = pAttack->uidOperator;
		if (isInvalidUID(uidOperator))
		{
			return;
		}
        // �˺�����
        int nDamageType = m_data.nDamageType;

		int nAttack = 0;
        int nPenetration = 0;
        int nPenetrationPCT = 0;
        if ((nDamageType&DAMAGE_TYPE_PHYSICS) == DAMAGE_TYPE_PHYSICS)
        {
            nPenetration = getProperty_Integer(uidOperator, PROPERTY_PDP);
            nPenetrationPCT = getProperty_Integer(uidOperator, PROPERTY_PCT_PDP);
        }
        else if ((nDamageType&DAMAGE_TYPE_MAGIC) == DAMAGE_TYPE_MAGIC)
        {
            nPenetration = getProperty_Integer(uidOperator, PROPERTY_MDP);
            nPenetrationPCT = getProperty_Integer(uidOperator, PROPERTY_PCT_MDP);
        }

		UID uidTarget = INVALID_UID;
		switch (m_data.nTagetType)
		{
		case MA_TAGET_TO_SELF:
			{
				uidTarget = uidOperator;
			}
			break;
		case MA_TAGET_TO_TARGET:
			{
				uidTarget = pAttack->uidTarget;
			}
			break;
		default:
			{
				ErrorLn("CEffectApendDamage effect nID="<< GetID() <<"m_data.nTagetType"<<m_data.nTagetType);
				return;
			}
			break;
		}
		if (isInvalidUID(uidTarget))
		{
			return;
		}

		switch (m_data.nExtraAttackType)
		{
		case EXTRAATTACK_PA:
			{
				nAttack = getProperty_Integer(uidTarget, PROPERTY_PA);
			}
			break;
		case EXTRAATTACK_MA:
			{
				nAttack = getProperty_Integer(uidTarget, PROPERTY_MA);
			}
			break;
		case EXTRAATTACK_LEVEL:
			{
				nAttack = getProperty_Integer(uidTarget, PROPERTY_LEVEL);
			}
			break;
		case EXTRAATTACK_HP:
			{
				nAttack = getProperty_Integer(uidTarget, PROPERTY_HP);
			}
			break;
		case EXTRAATTACK_MAX_HP:
			{
				nAttack = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
			}
			break;
		case EXTRAATTACK_LOSE_HP:
			{
				int nHp = getProperty_Integer(uidTarget, PROPERTY_HP);
				int nMaxHp = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
				nAttack = nMaxHp-nHp;
			}
			break;
		case EXTRAATTACK_LOSE_HP_PCT:
			{
				int nHp = getProperty_Integer(uidTarget, PROPERTY_HP);
				int nMaxHp = getProperty_Integer(uidTarget, PROPERTY_MAX_HP);
				if (nMaxHp <= 0)
				{
					return;
				}
				float fPercent = (float)(nMaxHp-nHp)/nMaxHp;
				nAttack = (int)(fPercent*EFFECT_RATE);
			}
			break;
		case EXTRAATTACK_MP:
			{
				nAttack = getProperty_Integer(uidTarget, PROPERTY_MP);
			}
			break;
		case EXTRAATTACK_MAX_MP:
			{
				nAttack = getProperty_Integer(uidTarget, PROPERTY_MAX_MP);
			}
			break;
		case EXTRAATTACK_LOSE_MP_PCT:
			{
				int nMp = getProperty_Integer(uidTarget, PROPERTY_MP);
				int nMaxMp = getProperty_Integer(uidTarget, PROPERTY_MAX_MP);
				if (nMaxMp <= 0)
				{
					return;
				}
				float fPercent = (float)(nMaxMp-nMp)/nMaxMp;
				nAttack = (int)(fPercent*EFFECT_RATE);
			}
			break;
		default:
			{
				ErrorLn("CEffectApendDamage effect nID="<< GetID() <<"nExtraAttackType error"<<m_data.nExtraAttackType);
				return;
			}
			break;
		}

		int nCRD = 0;
		// ��������
		if (m_data.nActivateCRD == MA_TRUE)
		{
			// ���һ�£��Ƿ񱩻�
            int nRandNum = 0;
            rktRangeRand(nRandNum, 1, (int)ZOOM_IN_MULTIPLE);
			if (nRandNum <= getProperty_Integer(uidOperator, PROPERTY_CRC))
			{
				// �Ǳ���(1+�����˺��ӳɱ�)
				nCRD = (int)ZOOM_IN_MULTIPLE + getProperty_Integer(uidOperator, PROPERTY_CRD);
			}
		}

        int nSpellPower = m_data.nSpellPower;
        int nSpellAttackPCT = m_data.nSpellAttackPCT;
        // ��������
        if (m_data.nFocoEnergia)
        {
            nSpellPower += (int)((m_data.nSpellPowerLimit - m_data.nSpellPower)*pAttack->fFocoEnergiaPCT);
            nSpellAttackPCT += (int)((m_data.nSpellAttackPCTLimit - m_data.nSpellAttackPCT)*pAttack->fFocoEnergiaPCT);
        }

        // �˺���Ϣ
        msg_entity_damage damage;
        damage.nID = pAttack->nSpellID;
        damage.uidOperator = uidOperator;
        damage.nDamageType = nDamageType;
        damage.nAttack = nAttack;
        damage.nPenetration = nPenetration;
        damage.nPenetrationPCT = nPenetrationPCT;
        damage.nSpellPower= nSpellPower;
        damage.nLevel = getProperty_Integer(uidOperator, PROPERTY_LEVEL);
        damage.nLevelFactor = m_data.nLevelFactor;
        damage.nSpellAttackPCT = nSpellAttackPCT;
        damage.nDeadType = m_data.nDeadType;
        damage.nSpellEffectType = m_data.nDamageSpellEffectType;
		damage.nCRD = nCRD;
		damage.nAttackCount = pAttack->nAttackCount;
		damage.nDamageLimitType = m_data.nDamageLimitType;
		damage.nDamageLimit = m_data.nDamageLimit;
        damage.nDamageBonus = getProperty_Integer(uidOperator, PROPERTY_DAMAGE_BONUS);
        damage.fDamageBonusPCT = getProperty_Integer(uidOperator, PROPERTY_DAMAGE_BONUS_PCT)/ZOOM_IN_MULTIPLE;
        damage.fAttackFactor = (float)getProperty_Integer(uidOperator, PROPERTY_AF);
        damage.nPASD         = getProperty_Integer(uidOperator, PROPERTY_PASD);
        damage.fAppendPCTPDP = static_cast<float>(getProperty_Integer(uidOperator, PROPERTY_APPEND_PCT_PDP)) / ZOOM_IN_MULTIPLE;      // ���Ӷ���ٷֱȻ��״�͸
        damage.fAppendPCTPMP = static_cast<float>(getProperty_Integer(uidOperator, PROPERTY_APPEND_PCT_MDP)) / ZOOM_IN_MULTIPLE;      // ���Ӷ���ٷֱ�ħ����͸
        damage.nUseFlag = m_pEntity->getUseFlag()->getAll();    // ��;��ʶ

		// ����ʵ����Ϣ
		g_EHelper.sendEntityMessage(pAttack->uidTarget, PART_DAMAGE, DAMAGE_MSG_DAMAGE, (char *)&damage, sizeof(damage));

        // ��¼�ϴι������
        m_dwLastAttackSerialID = pAttack->dwAttackSerialID;

        // ������ȴ
        m_dwLastTick = getTickCountEx();
    }

	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
		switch (wEventID)
		{
		case EVENT_ENTITY_ATTACK_EX:	// �����¼�
			{
                if (pszContext == NULL || nLen != sizeof(event_entity_attack_ex))
                {
                    return;
                }
                if (m_pEntity == NULL)
                {
                    return;
                }

                event_entity_attack_ex* pAttack = (event_entity_attack_ex*)pszContext;

				int nSpellEffectType = pAttack->nSpellEffectType;
				// �жϼ���Ч������
				if ((m_data.nSpellEffectType & nSpellEffectType) != m_data.nSpellEffectType)
				{
					return;
				}

				UID uidOperator = pAttack->uidOperator;
				if (isInvalidUID(uidOperator))
				{
					return;
				}

				UID uidTarget = pAttack->uidTarget;
				if (isInvalidUID(uidTarget))
				{
					return;
				}
				// ���Ŀ��
				if (!chooseTarget(uidOperator, m_data.nTargetFilter, uidTarget))
				{
					return;
				}

                if(m_dwLastAttackSerialID != pAttack->dwAttackSerialID)
                {
                    m_dwLastAttackSerialID = 0;
                }

                // Ⱥ������һ��
                if(m_dwLastAttackSerialID != 0 && m_dwLastAttackSerialID == pAttack->dwAttackSerialID)
                {
                    CreateApendDamage(pAttack);
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

                CreateApendDamage(pAttack);
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

    // �ϴι������
    DWORD                   m_dwLastAttackSerialID;

	// �ϴ�ʱ��
	DWORD					m_dwLastTick;
};
