/*******************************************************************
** �ļ���:	EffectDisableSpell.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	6/08/2015
** ��  ��:	1.0
** ��  ��:	Ч��-���ü���
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

class CEffectDisableSpell : public IEffectEx,public IEventVoteSink
{
public:
	typedef  EffectServer_DisableSpell    SCHEME_DATA;

	CEffectDisableSpell( SCHEME_DATA & data ) : m_data(data),m_pEntity(0)
	{
		vector<string> tempVector;
		tempVector.clear();

		m_DisableSpellIDMap.clear();
		StringHelper::split(tempVector, m_data.strSpellID, ';', " ");
		for (UINT nCount=0; nCount<tempVector.size(); ++nCount)
		{
			int nSpellID = StringHelper::toInt(tempVector[nCount]);
			if (nSpellID <= 0)
			{
				continue;
			}
			m_DisableSpellIDMap.insert(DISABLE_SPELL_MAP::value_type(nSpellID, nSpellID));
		}
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

        // ע��׼��ʩ������
        g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_CAST_SPELL, "CEffectDisableSpell");
		// ע��ʩ������
		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_CAST_SPELL, "CEffectDisableSpell");

		return true;
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
            // ע��׼��ʩ������
            g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_PREPARE_CAST_SPELL);
			// ע��ʩ������
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_CAST_SPELL);

			m_pEntity = 0;
		}
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone() 
	{
		return new CEffectDisableSpell(m_data);
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
        case EVENT_ENTITY_PREPARE_CAST_SPELL:
            {
                if (pszContext == NULL || nLen != sizeof(event_prepare_cast_spell))
                {
                    return false;
                }

                // �����ʾ�������м���
                if (m_DisableSpellIDMap.empty())
                {
                    return false;
                }

                event_prepare_cast_spell *pPrepareCastSpell = (event_prepare_cast_spell *)pszContext;
                DISABLE_SPELL_MAP::iterator Iter = m_DisableSpellIDMap.find(pPrepareCastSpell->nID);
                if (Iter != m_DisableSpellIDMap.end())
                {
                    return false;
                }
            }
            break;
		case EVENT_ENTITY_CAST_SPELL:
			{
				if (pszContext == NULL || nLen != sizeof(event_cast_spell))
				{
					return false;
				}
                event_cast_spell *pCastSpell = (event_cast_spell *)pszContext;

                // �����ʾ�������м���
                if (m_DisableSpellIDMap.empty())
                {
                    // ���ü��ܷ�ħ
                    pCastSpell->nFailResult = SpellCastFailResult_Silence;
                    return false;
                }
				
				DISABLE_SPELL_MAP::iterator Iter = m_DisableSpellIDMap.find(pCastSpell->nID);
				if (Iter != m_DisableSpellIDMap.end())
				{
                    // ���ü��ܷ�ħ
                    pCastSpell->nFailResult = SpellCastFailResult_Silence;
					return false;
				}
			}
			break;
		default:
			break;
		}

		return true;
	}

private:
	SCHEME_DATA               m_data;

	// ʵ��ָ��
	__IEntity				  *m_pEntity;

	// ���ü��ܱ�
	typedef map<int, int>	  DISABLE_SPELL_MAP;	
	DISABLE_SPELL_MAP		  m_DisableSpellIDMap;
};
