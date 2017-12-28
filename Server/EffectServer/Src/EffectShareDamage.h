/*******************************************************************
** �ļ���:	EffectShareDamage.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	7/21/2016
** ��  ��:	1.0
** ��  ��:	Ч��-�����˺�
            http://172.16.0.120/redmine/issues/2093
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

class CEffectShareDamage : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_ShareDamage    SCHEME_DATA;

	CEffectShareDamage( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_uidTarget(0)
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
                m_uidTarget = m_pEntity->getUID();
            }
            break;
        case MA_TAGET_TO_OPERATOR:
            {
                m_uidTarget = context->uidOperator;
            }
            break;
        case MA_TAGET_TO_TARGET:
            {
                m_uidTarget = context->uidTarget;
            }
            break;
        default:
            {
                ErrorLn("CEffectShareDamage error nTargetType="<<m_data.nTargetType);
                return false;
            }
            break;
        }
        if (isInvalidUID(m_uidTarget))
        {
            return false;
        }

        // ע��׼���˺��¼�
        g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_DAMAGE, "CEffectShareDamage");

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
		return new CEffectShareDamage(m_data);
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
                if (m_pEntity == NULL)
                {
                    break;
                }

                if (pszContext == NULL || nLen != sizeof(DamageCalcContext))
                {
                    break;
                }
                DamageCalcContext *pDamage = (DamageCalcContext*)pszContext;

                int nDamageType = pDamage->nDamageType;
                // �ж��˺�����
                if ((m_data.nDamageType & nDamageType) == 0)
                {
                    break;
                }

                UID uidSelf = m_pEntity->getUID();
                if (isInvalidUID(uidSelf))
                {
                    return;
                }

                UID uidOperator = pDamage->uidOperator;
                if (isInvalidUID(uidOperator))
                {
                    break;
                }

                // ��������Ŀ��
                if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetFilter, uidOperator))
                {
                    break;
                }

                int nDamageHP = pDamage->nDamageHP;

                pDamage->nDamageHP = (int)(pDamage->nDamageHP*(1.0-m_data.fValue));

                switch (m_data.nValueType)
                {
                case MA_VALUE_PER:
                    {
                        nDamageHP = (int)(nDamageHP*m_data.fValue);
                    }
                    break;
                default:
                    {
                        ErrorLn("CEffectShareDamage error nValueType="<<m_data.nValueType);
                        return;
                    }
                    break;
                }

                // �˺���Ϣ
                msg_entity_damage damage;
                damage.nID = pDamage->nID;
                damage.uidOperator = pDamage->uidOperator;
                damage.nDamageHP = nDamageHP;
                damage.nDamageType = (pDamage->nDamageType|DAMAGE_TYPE_EFFECT);
                damage.nSpellEffectType = pDamage->nSpellEffectType;
                damage.nDamageBonus = getProperty_Integer(pDamage->uidOperator, PROPERTY_DAMAGE_BONUS);
                damage.fDamageBonusPCT = getProperty_Integer(pDamage->uidOperator, PROPERTY_DAMAGE_BONUS_PCT)/ZOOM_IN_MULTIPLE;
                damage.nUseFlag = m_pEntity->getUseFlag()->getAll();    // ��;��ʶ

                // ����ʵ����Ϣ
                g_EHelper.sendEntityMessage(m_uidTarget, PART_DAMAGE, DAMAGE_MSG_DAMAGE, (char *)&damage, sizeof(damage));
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

    // Ŀ��UID
    UID                     m_uidTarget;
};
