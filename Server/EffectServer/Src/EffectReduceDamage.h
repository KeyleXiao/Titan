/*******************************************************************
** �ļ���:	EffectReduceDamage.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	�ΰ
** ��  ��:	7/17/2015
** ��  ��:	1.0
** ��  ��:	Ч��-�ֵ��˺�Ч��
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
#include "IBuffServer.h"

class CEffectReduceDamage : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_ReduceDamage    SCHEME_DATA;

	CEffectReduceDamage( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_dwLastTick(0), m_nCurTimes(0)
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

        // ע��׼���˺�
        g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_DAMAGE, "CEffectReduceDamage");

		return true;
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
            // ע���˺��¼�
            g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_DAMAGE);

			m_pEntity = 0;
		}
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone()
	{
		return new CEffectReduceDamage(m_data);
	}

	// �ͷ�
	virtual void			Release()
	{
		Stop();
		delete this;
	}

    /////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
	/** 
	@param   wEventID ���¼�ID
	@param   bSrcType ������Դ����
	@param   dwSrcID : ����Դ��ʶ��ʵ��ΪUID��"���к�"���ݣ���ʵ���Ϊ0��
	@param   pszContext : ������
	@param   nLen : �����ĳ���
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
    {
		switch (wEventID)
		{
		case EVENT_ENTITY_PREPARE_DAMAGE:
			{
				if (m_pEntity == 0)
				{
					return;
				}
				if (pszContext == NULL || nLen != sizeof(DamageCalcContext))
				{
					return;
				}

				// �ֵ�һ���˺�
				DamageCalcContext *pDamage = (DamageCalcContext *)pszContext;
				// ��ʵ�˺����ֵ�
				if ((pDamage->nDamageType&DAMAGE_TYPE_TURE_DAMAGE) == DAMAGE_TYPE_TURE_DAMAGE)
				{
					return;
				}
				if (pDamage->nDamageHP <= 0)
				{
					return;
				}

				// �����δ�������ж��Ƿ񴥷�
				if(m_nCurTimes == 0)
				{
					// �ж���ȴʱ��
					DWORD dwTick = getTickCountEx();
					if(dwTick < m_dwLastTick + m_data.nCD)
					{
						return;
					}

					// �жϸ���
                    int nRandNum = 0;
                    rktRangeRand(nRandNum, 1, 100);
					if(nRandNum > m_data.fRate * 100)
					{
						return;
					}
				}

				int nSpellEffectType = pDamage->nSpellEffectType;
				// �жϼ���Ч������
				if ((m_data.nSpellEffectType & nSpellEffectType) != m_data.nSpellEffectType)
				{
					return;
				}

				UID uidSelf = m_pEntity->getUID();

				UID uidOperator = pDamage->uidOperator;
				if (isInvalidUID(uidOperator))
				{
					return;
				}

				// ��������Ŀ��
				if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetFilter, uidOperator))
				{
					return;
				}
				if ((pDamage->nDamageType&m_data.nDamageType) == 0)
				{
					return;
				}

				int nDamageHP = pDamage->nDamageHP;

				switch (m_data.nValueType)
				{
				case MA_VALUE_NUM:
					{
						nDamageHP -= (int)m_data.fValue;
					}
					break;
				case MA_VALUE_PER:
					{
						nDamageHP -= (int)(m_data.fValue * nDamageHP);
					}
					break;
				default:
					{
						ErrorLn("CEffectReduceDamage error nValueType="<<m_data.nValueType);
						return;
					}
					break;
				}

				if(nDamageHP < 0)
				{
					nDamageHP = 0;
				}

				pDamage->nDamageHP = nDamageHP;

				// �����ֵ�����
				++m_nCurTimes;

				// ����ֵ��������ˣ����ֽ���
				if(m_nCurTimes >= m_data.nTimes)
				{
					// ��������BUFF
					if (m_data.nBuffID > 0 && m_data.nBuffLvel > 0)
					{
                        SBuffContext BuffContext;
                        BuffContext.nID = pDamage->nID;
						// ����BUFF
						AddBuff(uidSelf, m_data.nBuffID, m_data.nBuffLvel, uidSelf, 0, &BuffContext, sizeof(BuffContext));
					}
					m_nCurTimes = 0;

					// ������ȴ
					m_dwLastTick = getTickCountEx();
				}
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

    // �Ѿ��ֵ�����
    int                     m_nCurTimes;       

	// �ϴ�ʱ��
	DWORD					m_dwLastTick;
};
