/*******************************************************************
** �ļ���:	ProgressBarPart.cpp
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	���� (Carl Chen)
** ��  ��:	2/26/2015
** ��  ��:	1.0
** ��  ��:	

			����������
********************************************************************/

#include "stdafx.h"
#include "ProgressBarPart.h"
#include "IServerGlobal.h"
#include "ITimerService.h"
#include "IEntityEvent.h"
#include "EventDef.h"
#include "EntityHelper.h"

bool ProgressBarPart::onLoad( __IEntity* pEntity, int nLoginMode)
{
	m_pMaster = pEntity;
	return true;
}

bool ProgressBarPart::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
	return true;
}

void ProgressBarPart::release()
{
	CloseProgressBar(0,false);
	m_pMaster = 0;
	delete this;
}

bool ProgressBarPart::StartProgressBar( ProgressBarContext * context )
{
	if ( m_pContext!=0 )
		return false;

    if (m_pMaster == NULL)
    {
        return false;
    }
    m_vStartPoint.x = 0;
    m_vStartPoint.y = 0;
    m_vStartPoint.z = 0;

	// �����ֳ�
	m_pContext = new ProgressBarContext(*context);
    // ����ֱ����sizeof(ProgressBarContext) ��ΪProgressBarContext����һ����Աstd::string ����δ����
	//memcpy( m_pContext,context,sizeof(ProgressBarContext) );

    // ���ý�������ʼʱ��
    m_dwStartTime = getTickCountEx();

	// ������ʱ��
	gServerGlobal->getTimerService()->SetTimer( 0,m_pContext->nTime,this,1,__FILE__);

	// �����¼�
	if ( !SubscibeEvents() )
		return false;

	// ���͸��ͻ���
	if ( m_pContext->bSendClient )
	{

	}

	return true;
}

void ProgressBarPart::CloseProgressBar(IEventExecuteSink * pSink,bool bBreak)
{
	if ( m_pMaster==0 || m_pContext==0 )
		return;

	// ��Ҫ�ѱ��˵Ľ�����ͣ����
	if ( pSink!=0 && pSink!=m_pContext->pCallBack )
		return;

	// ֹͣ��ʱ
	gServerGlobal->getTimerService()->KillTimer( 0,this);

	// �˶��¼�
	UnSubscibeEvents();

	// ���͸��ͻ���
	if ( m_pContext->bSendClient )
	{

	}

	// ���ʹ���¼�
	if ( bBreak && m_pContext->pCallBack )
	{
		UID uidEntity = m_pMaster->getUID();
		/* ���¼�����ܻ��ͷ�m_pContext,
		   ������ùرս�����CloseProgressBar��
		   m_pContextʹ��Ҫ�жϣ������Ƶ�����ʵ��
		*/
		m_pContext->pCallBack->OnExecute(EVENT_ENTITY_PROGRESS_BREAK, UID_2_TYPE(uidEntity), uidEntity, (LPCSTR)m_pContext, sizeof(ProgressBarContext));
	}

	// �����ֳ�
	safeDelete(m_pContext);
}

ProgressBarContext * ProgressBarPart::GetCurrentContext()
{
	return m_pContext;
}

void ProgressBarPart::OnTimer( unsigned long dwTimerID )
{
	if ( dwTimerID==0 )
	{
		// ��������¼�
		if ( m_pMaster!=0 && m_pContext!=0 && m_pContext->pCallBack )
		{
			UID uidEntity = m_pMaster->getUID();
			/* ���¼�����ܻ��ͷ�m_pContext,
			   ������ùرս�����CloseProgressBar��
			   m_pContextʹ��Ҫ�жϣ������Ƶ�����ʵ��
			*/
			m_pContext->pCallBack->OnExecute(EVENT_ENTITY_PROGRESS_OK, UID_2_TYPE(uidEntity), uidEntity, (LPCSTR)m_pContext, sizeof(ProgressBarContext));
		}

		CloseProgressBar(0,false);
	}

	if ( dwTimerID==1 )
	{
		int nBreakFlag = m_pContext->nBreakFlag;

		// ����������
		if ((nBreakFlag & BREAK_DISTANCE) == BREAK_DISTANCE )
		{
            // ����ʱ�䣬��������
            if (getTickCountEx() > m_dwStartTime + BREAK_DISTANCE_TIME)
            {
                Vector3 ptMyLoc = m_pMaster->getLocation();
                Vector3 ptTarget= getLocation(m_pContext->uidTarget);
                if ( ptMyLoc.checkDistance( ptTarget,m_pContext->fDistanceRange )==false )
                {
                    CloseProgressBar( 0,true ); // ���
                }
            }
		}
	}
}

void ProgressBarPart::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	if ( m_pContext==0 || m_pMaster==0 )
		return;

	switch(wEventID)
	{
	case EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_LOCATION:
		{
			if (pszContext == NULL || nLen != sizeof(event_entity_property_change))
			{
				return;
			}

            event_entity_property_change* pEvent = (event_entity_property_change*)pszContext;

            if(pEvent->nPropertyID != PROPERTY_LOCATION)
            {
                ErrorLn(__FUNCTION__ << __LINE__);
                return;
            }

            // ������ʱ�䣬��������
            if (getTickCountEx() <= m_dwStartTime + BREAK_DISTANCE_TIME)
            {
                return;
            }

            // ��Ϊ�ƶ�ʱ,ʹ�ü����ǿͻ������꣬ȡ��ʱ�����꣬�����ױ����,�������Ϻ�ʱ�������
            if (equalto0(m_vStartPoint.x) && equalto0(m_vStartPoint.y) && equalto0(m_vStartPoint.z))
            {
                m_vStartPoint = m_pMaster->getLocation();
            }

            Vector3 loc;
            memcpy(&loc, pEvent->data, pEvent->nLen);

			// �����С�������
			if (m_vStartPoint.checkDistance(loc, 0.5))
			{
				return;
			}
		}
		break;
	case /*EVENT_ENTITY_CAST_SPELL*/EVENT_ENTITY_SPELL_PREPARE:
		{
			if (pszContext == NULL || nLen != sizeof(event_spell_prepare))
			{
				return;
			}
			event_spell_prepare *pCastSpell = (event_spell_prepare *)pszContext;
			// ����Ͻ�����
			if ((pCastSpell->nCastType&SpellCastType_BreakProgressBar) != SpellCastType_BreakProgressBar)
			{
				return;
			}

            // ͬ����ID�������
            if (pCastSpell->nID == m_pContext->nSpellID)
            {
                return;
            }
		}
		break;
	default:
		break;
	}

	// ��϶���
	CloseProgressBar( 0,true );
}

bool ProgressBarPart::SubscibeEvents()
{
	if ( m_pContext==0 || m_pMaster==0 )
		return false;

	__IEntityEvent * pEventEngine = m_pMaster->getEntityEvent();
	if ( pEventEngine==0 )
		return false;

	int nBreakFlag = m_pContext->nBreakFlag;

	// �ƶ����
	if ( nBreakFlag & BREAK_MOVE )
	{
        pEventEngine->Subscibe(this, (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_LOCATION), "ProgressBar");
	}

	// �������
	if ( nBreakFlag & BREAK_HIT )
	{
		pEventEngine->Subscibe( this,EVENT_ENTITY_DAMAGE,"ProgressBar");
	}

	// �������
	if ( nBreakFlag & BREAK_DEAD )
	{
		pEventEngine->Subscibe( this,EVENT_ENTITY_DIE,"ProgressBar");
	}

	// �������������������ⶨʱ��
	if ( nBreakFlag & BREAK_DISTANCE )
	{
		gServerGlobal->getTimerService()->SetTimer( 1,UPDATE_FRAME_TIMER,this,INFINITY_CALL,__FILE__);
	}

	// Ч�����
	if ( nBreakFlag & BREAK_EFFECT )
	{
		pEventEngine->Subscibe( this,EVENT_ENTITY_EFFECT_BREAK,"ProgressBar");
	}

	// ȡ��ʩ�����
	if ( nBreakFlag & BREAK_CANCEL_CAST )
	{
		pEventEngine->Subscibe( this,EVENT_ENTITY_CANCEL_CAST,"ProgressBar");
	}

	// ʩ�����ܴ��
	if ( nBreakFlag & BREAK_CAST_SPELL )
	{
		pEventEngine->Subscibe( this,/*EVENT_ENTITY_CAST_SPELL*/EVENT_ENTITY_SPELL_PREPARE,"ProgressBar");
	}

	return true;
}

bool ProgressBarPart::UnSubscibeEvents()
{
	if ( m_pContext==0 || m_pMaster==0 )
		return false;

	__IEntityEvent * pEventEngine = m_pMaster->getEntityEvent();
	if ( pEventEngine==0 )
		return false;

	int nBreakFlag = m_pContext->nBreakFlag;

	// �ƶ����
	if ( nBreakFlag & BREAK_MOVE )
	{
        pEventEngine->UnSubscibe(this, (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_LOCATION));
	}

	// �������
	if ( nBreakFlag & BREAK_HIT )
	{
		pEventEngine->UnSubscibe( this,EVENT_ENTITY_DAMAGE);
	}

	// �������
	if ( nBreakFlag & BREAK_DEAD )
	{
		pEventEngine->UnSubscibe( this,EVENT_ENTITY_DIE);
	}

	// �������������������ⶨʱ��
	if ( nBreakFlag & BREAK_DISTANCE )
	{
		gServerGlobal->getTimerService()->KillTimer( 1,this );
	}

	// Ч�����
	if ( nBreakFlag & BREAK_EFFECT )
	{
		pEventEngine->UnSubscibe( this,EVENT_ENTITY_EFFECT_BREAK);
	}

	// ȡ��ʩ�����
	if ( nBreakFlag & BREAK_CANCEL_CAST )
	{
		pEventEngine->UnSubscibe( this,EVENT_ENTITY_CANCEL_CAST);
	}

	// ʩ�����ܴ��
	if ( nBreakFlag & BREAK_CAST_SPELL )
	{
		pEventEngine->UnSubscibe( this,/*EVENT_ENTITY_CAST_SPELL*/EVENT_ENTITY_SPELL_PREPARE);
	}

	return true;
}

void ProgressBarPart::restore()
{

}

//////////////////////////////////////////////////////////////////////////

extern "C" __declspec(dllexport) __IProgressBarPart * CreateEntityProgressBar()
{
	return new ProgressBarPart;
}