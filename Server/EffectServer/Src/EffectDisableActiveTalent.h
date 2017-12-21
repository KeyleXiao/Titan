/*******************************************************************
** �ļ���:	EffectDisableActiveTalent.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	1/18/2017
** ��  ��:	1.0
** ��  ��:	Ч��-��ֹѧϰ�츳
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

class CEffectDisableActiveTalent : public IEffectEx,public IEventVoteSink
{
public:
	typedef  EffectServer_DisableActiveTalent    SCHEME_DATA;

	CEffectDisableActiveTalent( SCHEME_DATA & data ) : m_data(data),m_pEntity(0)
	{
		vector<string> tempVector;
		tempVector.clear();

		m_DisableActiveTalentIDMap.clear();
		StringHelper::split(tempVector, m_data.strTalentID, ';', " ");
		for (UINT nCount=0; nCount<tempVector.size(); ++nCount)
		{
			int nTalentID = StringHelper::toInt(tempVector[nCount]);
			if (nTalentID <= 0)
			{
				continue;
			}
			m_DisableActiveTalentIDMap.insert(DISABLE_SPELL_MAP::value_type(nTalentID, nTalentID));
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

        // ע����Ҽ����츳
		g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_ACTIVE_TALENT, "CEffectDisableActiveTalent");

		return true;
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			// ע����Ҽ����츳
			g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_ACTIVE_TALENT);

			m_pEntity = 0;
		}
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone() 
	{
		return new CEffectDisableActiveTalent(m_data);
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
		case EVENT_ENTITY_ACTIVE_TALENT:
			{
				if (pszContext == NULL || nLen != sizeof(event_entity_active_talent))
				{
					return false;
				}
                event_entity_active_talent *pTalent = (event_entity_active_talent *)pszContext;

                // �����ʾ�������м���
                if (m_DisableActiveTalentIDMap.empty())
                {
                    return false;
                }
				
				DISABLE_SPELL_MAP::iterator Iter = m_DisableActiveTalentIDMap.find(pTalent->nTalentID);
				if (Iter != m_DisableActiveTalentIDMap.end())
				{
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

	// �����츳��
	typedef map<int, int>	  DISABLE_SPELL_MAP;	
	DISABLE_SPELL_MAP		  m_DisableActiveTalentIDMap;
};
