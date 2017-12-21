/*******************************************************************
** �ļ���:	EffectContinuousAttackBeHitAddBuff.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	�ΰ
** ��  ��:	2/8/2017
** ��  ��:	1.0
** ��  ��:	Ч��-��������/��������buff
����������http://172.16.0.120/redmine/issues/3576
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"
#include "EventDef.h"
#include "ISpell.h"

class CEffectContinuousAttackBeHitAddBuff : public IEffectEx,public IEventExecuteSink,public TimerHandler
{
public:
	typedef  EffectServer_ContinuousAttackBeHitAddBuff    SCHEME_DATA;

	CEffectContinuousAttackBeHitAddBuff( SCHEME_DATA & data ) : m_data(data),m_pEntity(0),m_nCount(0)
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

        if( m_data.nAttackOrBeHit==AOB_ATTACK )
        {
            g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_ATTACK, "CEffectContinuousAttackBeHitAddBuff");
        }
        else if( m_data.nAttackOrBeHit==AOB_BEHIT )
        {
            g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_DAMAGE, "CEffectContinuousAttackBeHitAddBuff");
        }

        m_nCount = 0;

		return true;
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
            if( m_data.nAttackOrBeHit==AOB_ATTACK )
            {
                g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_ATTACK);
            }
            else if( m_data.nAttackOrBeHit==AOB_BEHIT )
            {
                g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_DAMAGE);
            }

			m_pEntity = 0;
		}

        if(m_nCount > 0)
        {
            g_EHelper.KillTimer(0, this);
        }
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone()
	{
		return new CEffectContinuousAttackBeHitAddBuff(m_data);
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
		case EVENT_ENTITY_ATTACK:	// �����¼�
            {
                event_entity_attack* pEvent = (event_entity_attack*)pszContext;

                // Ŀ�����
                if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetFilter, pEvent->uidTarget))
                {
                    return;
                }

                // Ч�����͹���
                if ((m_data.nSpellEffectType & pEvent->nSpellEffectType) != m_data.nSpellEffectType)
                {
                    return;
                }

                // ���ӹ�������
                addCount();
            }
            break;
        case EVENT_ENTITY_DAMAGE:       // �����¼�
            {
                event_entity_damage* pEvent = (event_entity_damage*)pszContext;

                // Ŀ�����
                if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetFilter, pEvent->uidOperator))
                {
                    return;
                }

                // Ч�����͹���
                if ((m_data.nSpellEffectType & pEvent->nSpellEffectType) != m_data.nSpellEffectType)
                {
                    return;
                }

                // �����ܻ�����
                addCount();
            }
            break;
		default:
			break;
		}
	}

    virtual void OnTimer( unsigned long dwTimerID )
    {
        m_nCount = 0;
    }

    void addCount()
    {
        ++m_nCount;

        // �����������˼�buff
        if(m_nCount >= m_data.nTimes)
        {
            m_nCount = 0;
            AddBuff(m_pEntity->getUID(), m_data.nBuffID, m_data.nBuffLevel, m_pEntity->getUID());
        }

        // ��������timer
        if(m_nCount > 0)
        {
            g_EHelper.KillTimer(0, this);
            g_EHelper.SetTimer(0, m_data.nCountTimeOut, this, 1, __FUNCTION__);
        }
    }

private:
	SCHEME_DATA             m_data;

	// ʵ��ָ��
	__IEntity				*m_pEntity;

    int                     m_nCount;           // ����
};
