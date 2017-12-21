/*******************************************************************
** �ļ���:	EffectBeSneered.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	�ΰ
** ��  ��:	2/9/2017
** ��  ��:	1.0
** ��  ��:	Ч��-������Ч��
����������http://172.16.0.120/redmine/issues/3575
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

class CEffectBeSneered : public IEffectEx,public TimerHandler,public IEventVoteSink,public IEventExecuteSink
{
public:
	typedef  EffectServer_BeSneered    SCHEME_DATA;

	CEffectBeSneered( SCHEME_DATA & data ) : m_data(data),m_pEntity(0),m_uidTarget(INVALID_UID),m_nSpellID(-1)
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

        m_uidTarget = context->uidOperator;
        if( m_uidTarget==INVALID_UID )
            return false;

        __ISlotPart* pSlotPart = (__ISlotPart*)m_pEntity->getEntityPart(PART_SLOTLOGIC);
        if( pSlotPart==NULL )
        {
            return false;
        }
        m_nSpellID = pSlotPart->GetSlotSkillID(SLOT_TYPE_SKILL, m_data.nForceAttackSpellSlot);

        SPELL_DATA* pData = gServerGlobal->getSpellFactory()->getSpellData(m_nSpellID);
        if( pData==NULL )
        {
            ErrorLn(__FUNCTION__ << __LINE__ << ", spell not found, id=" << m_nSpellID);
            return false;
        }

        // ע��ͻ��˼���ͶƱ�¼�
        m_pEntity->getEntityEvent()->Subscibe(static_cast<IEventVoteSink*>(this), EVENT_ENTITY_PREPARE_CAST_SPELL, __FUNCTION__);

        // ע�������¼�
        SceneLogicEventHelper eventHelper(m_pEntity->getSceneID());
        if(eventHelper.m_ptr)
        {
            eventHelper.m_ptr->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_DIE, TYPE_PLAYER_ROLE, 0, __FUNCTION__);
        }
        
        g_EHelper.SetTimer(0, 300, this, INFINITY_CALL, __FUNCTION__);

        // ��ֹ�����ƶ�
        bool bFlag = true;
        g_EHelper.sendEntityMessage(m_pEntity->getUID(), PART_MOVE, SS_MSG_FORBID_MOVE_INITIATIVELY, (LPCSTR)&bFlag, sizeof(bFlag));

		g_EHelper.SetTimer(1, m_data.fTimeout, this, 1, __FUNCTION__);

		return true;
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
            m_pEntity->getEntityEvent()->UnSubscibe(static_cast<IEventVoteSink*>(this), EVENT_ENTITY_PREPARE_CAST_SPELL);

            // ע�������¼�
            SceneLogicEventHelper eventHelper(m_pEntity->getSceneID());
            if(eventHelper.m_ptr)
            {
                eventHelper.m_ptr->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_DIE, TYPE_PLAYER_ROLE, 0);
            }

            // ֹͣ�ƶ�
            __IMovePart* pMovePart = (__IMovePart*)m_pEntity->getEntityPart(PART_MOVE);
            if( pMovePart!=NULL )
            {
                pMovePart->stopMoving();
            }

            // ȡ����ֹ�����ƶ�
            bool bFlag = false;
            g_EHelper.sendEntityMessage(m_pEntity->getUID(), PART_MOVE, SS_MSG_FORBID_MOVE_INITIATIVELY, (LPCSTR)&bFlag, sizeof(bFlag));

			m_pEntity = 0;
		}

        g_EHelper.KillTimer(0, this);
		g_EHelper.KillTimer(1, this);
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone()
	{
		return new CEffectBeSneered(m_data);
	}

	// �ͷ�
	virtual void			Release()
	{
		Stop();
		delete this;
	}

    virtual void OnTimer( unsigned long dwTimerID )
    {
		if (dwTimerID == 0)
		{
			if (m_pEntity == NULL)
			{
				return;
			}

			__IMovePart* pMovePart = (__IMovePart*)m_pEntity->getEntityPart(PART_MOVE);
			if (pMovePart == NULL)
			{
				return;
			}

			// Ŀ������ֹͣ
			int nTargetState = GetState(m_uidTarget);
			if (nTargetState == ENTITY_STATE_DIE || nTargetState == ENTITY_STATE_UNKNOW)
			{
				Stop();
				return;
			}

			Vector3 ptSelf = m_pEntity->getLocation();
			Vector3 ptTarget = getLocation(m_uidTarget);
			float fDistance = (ptTarget - ptSelf).getLength();
			if (fDistance > m_data.fCloseDistance)
			{
				float fCloseDistance = m_data.fCloseDistance;
				if (fCloseDistance > 0.01f)
					fCloseDistance -= 0.01f;

				// ����
				Vector3 ptClose = ptSelf + (ptTarget - ptSelf) * ((fDistance - fCloseDistance) / fDistance);
				pMovePart->moveTo(ptClose);
			}
			else
			{
				// �չ�
				__ISpellPart* pSpellPart = (__ISpellPart*)m_pEntity->getEntityPart(PART_SPELL);
				if (pSpellPart == NULL)
				{
					return;
				}
				DWORD dwStartTick = getTickCountEx();
				SPELL_CONTEXT spellContext;
				spellContext.moveState = pMovePart->getMoveState();
				spellContext.CC.pEntity = m_pEntity;
				spellContext.CC.uidOperator = m_pEntity->getUID();
				spellContext.CC.uidTarget = m_uidTarget;
				spellContext.CC.ptStartTile = ptSelf;
				spellContext.CC.ptTargetTile = ptTarget;
				spellContext.CC.nID = m_nSpellID;
				spellContext.CC.dwStartTick = dwStartTick;
				spellContext.CC.dwUpdateFrame = 0;
				spellContext.EC.pEntity = m_pEntity;
				spellContext.EC.uidOperator = m_pEntity->getUID();
				spellContext.EC.uidTarget = m_uidTarget;
				spellContext.EC.ptStartTile = ptSelf;
				spellContext.EC.ptTargetTile = ptTarget;
				spellContext.EC.nID = m_nSpellID;
				spellContext.EC.dwStartTick = dwStartTick;

				pSpellPart->castSpell(&spellContext);
			}
		}

		if (dwTimerID == 1)
		{
			Stop();
		}
    }

    virtual bool OnVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
    {
        switch(wEventID)
        {
        case EVENT_ENTITY_PREPARE_CAST_SPELL:
            {
                // ����������ʹ�ü���
                // return false;
            }
        }

        return true;
    }

    virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
    {
        switch(wEventID)
        {
        case EVENT_GAME_ENTITY_DIE:
            {
                if (pszContext == NULL || nLen != sizeof(event_entity_die))
                {
                    return;
                }
                event_entity_die* pEvent = (event_entity_die*)pszContext;
                if((m_pEntity != NULL && pEvent->uidTarget == m_pEntity->getUID()) || pEvent->uidTarget == m_uidTarget)
                {
                    Stop();
                }
            }
            break;
        }
    }

private:
	SCHEME_DATA             m_data;

	// ʵ��ָ��
	__IEntity				*m_pEntity;

    UID                     m_uidTarget;
    int                     m_nSpellID;
};
