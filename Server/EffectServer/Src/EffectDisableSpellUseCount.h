/*******************************************************************
** �ļ���:	EffectDisableSpellUseCount.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	9/13/2016
** ��  ��:	1.0
** ��  ��:	Ч��-���Ƽ���ʹ�ô���
            http://172.16.0.120/redmine/issues/2337

********************************************************************
** �޸�: http://172.16.0.120/redmine/issues/5005   ��绪
********************************************************************/

#pragma once

#include "IEffect.h"
#include "IEntity.h"
#include "EffectDef.h"
#include "IEntityEvent.h"
#include "IEventEngine.h"
#include "EventDef.h"

using namespace rkt;
using namespace EFFECT_SERVER;

class CEffectDisableSpellUseCount : public IEffectEx,public IEventVoteSink, public IEventExecuteSink
{
public:
	typedef  EffectServer_DisableSpellUseCount    SCHEME_DATA;

	CEffectDisableSpellUseCount( SCHEME_DATA & data ) : m_data(data),m_pEntity(0),m_nSpellCount(0),m_nResetTick(0)
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

		// ע��ʩ������ͶƱ�¼�
		g_EHelper.Subscibe(m_pEntity, static_cast<IEventVoteSink*>(this), EVENT_ENTITY_CAST_SPELL, "CEffectDisableSpellUseCount");

        g_EHelper.Subscibe(m_pEntity, static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CAST_SPELL, "CEffectDisableSpellUseCount");

		return true;
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
            // ע��ʩ������ͶƱ�¼�
			g_EHelper.UnSubscibe(m_pEntity, static_cast<IEventVoteSink*>(this), EVENT_ENTITY_CAST_SPELL);

            g_EHelper.UnSubscibe(m_pEntity, static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CAST_SPELL);

			m_pEntity = 0;
		}
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone() 
	{
		return new CEffectDisableSpellUseCount(m_data);
	}

	// �ͷ�
	virtual void			Release() 
	{
		Stop();
		delete this;
	}

	/** 
	@param   wEventID ���¼�ID
	@param   bSrcType ������Դ����
	@param   dwSrcID : ����Դ��ʶ��ʵ��ΪUID��"���к�"���ݣ���ʵ���Ϊ0��
	@param   pszContext : ������
	@param   nLen : �����ĳ���
	@return  �������false : ���ж�ִ�У������������ִ��
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool OnVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
		switch (wEventID)
		{
		case EVENT_ENTITY_CAST_SPELL:
			{
				if (pszContext == NULL || nLen != sizeof(event_cast_spell))
				{
					return false;
				}
                event_cast_spell *pCastSpell = (event_cast_spell *)pszContext;

                // ���ܲ�ƥ��
                if (pCastSpell->nID != m_data.nSpellID)
                {
                    return true;
                }

                if (m_nSpellCount >= m_data.nSpellCount)
                {
                    // �Ѿ�����
                    if (m_nResetTick > 0 && getTickCountEx() >= m_nResetTick)
                    {
                        // �������ʱ��
                        m_nResetTick = 0;
                        // ����ʹ�ô���
                        m_nSpellCount = 0;
                    }
                    else
                    {
                        if(m_data.nTipID > 0)
                        {
                            pCastSpell->nFailResult = m_data.nTipID;
                        }
                        else
                        {
                            // ʹ�ô�������
                            pCastSpell->nFailResult = SpellCastFailResult_CountLimit;
                        }
                        return false;
                    }
                }
			}
			break;
		default:
			break;
		}

		return true;
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
		case EVENT_ENTITY_CAST_SPELL:
			{
				if (pszContext == NULL || nLen != sizeof(event_cast_spell))
				{
					break;
				}
				event_cast_spell *pCastSpell = (event_cast_spell *)pszContext;

                // ���ܲ�ƥ��
                if (pCastSpell->nID != m_data.nSpellID)
                {
                    break;
                }

                ++m_nSpellCount;

                // �ﵽ���ʹ�ô�����
                if(m_nSpellCount >= m_data.nSpellCount)
                {
                    // �趨����ʱ��
                    if(m_data.nResetInterval > 0)
                    {
                        m_nResetTick = getTickCountEx() + m_data.nResetInterval;
                    }
                }
			}
			break;
		default:
			break;
		}
	}

private:
	SCHEME_DATA                 m_data;

	// ʵ��ָ��
	__IEntity	 *              m_pEntity;

    // ʹ�ü��ܴ���
    int                         m_nSpellCount;

    // ����ʱ��
    ulong                       m_nResetTick;
};
