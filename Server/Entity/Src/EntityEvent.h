/*******************************************************************
** �ļ���:	EntityEvent.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	���� (Carl Chen)
** ��  ��:	2/26/2015
** ��  ��:	1.0
** ��  ��:	

			ʵ���¼�����
********************************************************************/

#pragma once

#include "IEntity.h"
#include "IEntityEvent.h"
#include "EventEngineT.h"
#include "IServerGlobal.h"
#include "EntityHelper.h"

// ʵ���¼�������
class EntityEvent : public __IEntityEvent
{
private:
	TEventEngine<IEventVoteSink,
		         EntityEvent,
				 EventKey,
				 EVENT_ENTITY_CREATE,
				 EVENT_ENTITY_MAX-EVENT_ENTITY_CREATE>       m_Voter;   // ͶƱ��

	TEventEngine<IEventExecuteSink,
		         EntityEvent,
				 EventKey,
				 EVENT_ENTITY_CREATE,
				 EVENT_ENTITY_MAX-EVENT_ENTITY_CREATE>    m_Executer;// ͶƱ��

	UID    m_uidEntity;
    BYTE m_bSrcType;

public:
	virtual bool		FireVote(EventKey eventID, LPCSTR pszContext, int nLen)
	{
		if ( !m_Voter.Fire( eventID,m_bSrcType,m_uidEntity,pszContext,nLen ) )
			return false;

		// �����߳����ж��ģ�������Ƿ�����Ϣ(Ŀǰ��û��һ�����̶߳��ĵ�����)
		//if ( !gServerGlobal->getEventEngine()->FireVote(eventID,UID_2_TYPE(m_uidEntity), m_uidEntity, pszContext, nLen) )
		//	return false;		
		return true;
	}

	virtual void		FireExecute(EventKey eventID, LPCSTR pszContext, int nLen)
	{
		m_Executer.Fire( eventID,m_bSrcType,m_uidEntity,pszContext,nLen );
	}

	/** ����ȫ��ִ���¼�
	@param   wEventID ���¼�ID
	@param   pszContext : ������
	@param   nLen : �����ĳ���	  
	*/
	virtual void		FireExecute(GAMEEVENT_KEY eventID, LPCSTR pszContext, int nLen)
    {
        int nSceneID = UID_2_SCENE(m_uidEntity);
        // ���������ﲻ��Ҫ�������¼�
        if (IsMainMap(nSceneID))
        {
            if (eventID == EVENT_GAME_ENTITY_LEARN_SPELL
             || eventID == EVENT_GAME_ENTITY_FREEZE_CHANGED
             || eventID == EVENT_GAME_ENTITY_PREPARE_CURE
             || eventID == EVENT_GAME_ACTOR_TALENT_CHANGE
            )
               return;
        }

        // �¼����������߼�
        SceneLogicEventHelper eventHelper(nSceneID);
        rkt::IEventEngine * pEventEngine = eventHelper.m_ptr;
        if(pEventEngine)
        {
            pEventEngine->FireExecute(eventID,m_bSrcType,m_uidEntity, pszContext, nLen);
        }
    }

	virtual bool		FireVoteExecute(EventKey eventID, LPCSTR pszContext, int nLen)
	{
		// �Ƚ���ͶƱ
		if (!FireVote( eventID,pszContext,nLen ))
			return false;

		// ��ִ���¼�
		FireExecute( eventID,pszContext,nLen );

		return true;
	}

	virtual bool		Subscibe(IEventVoteSink * pSink,EventKey eventID, LPCSTR pszDesc)
	{
		return m_Voter.Subscibe( pSink,eventID,pszDesc );
	}

	virtual bool		Subscibe(IEventExecuteSink * pSink,EventKey eventID, LPCSTR pszDesc)
	{
		return m_Executer.Subscibe( pSink,eventID,pszDesc );
	}

	virtual bool		UnSubscibe(IEventVoteSink * pSink,EventKey eventID)
	{
		return m_Voter.UnSubscibe( pSink,eventID );
	}

	virtual bool		UnSubscibe(IEventExecuteSink * pSink,EventKey eventID) 
	{
		return m_Executer.UnSubscibe( pSink,eventID );
	}

	// �ص�ִ���¼�
	bool operator() (IEventExecuteSink * pSink, EventKey wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
		if(pSink == NULL)
		{
			return false;
		}

		pSink->OnExecute(wEventID,bSrcType,dwSrcID, pszContext, nLen);

		return true;
	}

	// �ص�ͶƱ�¼�
	bool operator() (IEventVoteSink * pSink, EventKey wEventID, BYTE bSrcType, DWORD dwSrcID,LPCSTR pszContext, int nLen)
	{
		if(pSink == NULL)
		{
			return false;
		}

		return pSink->OnVote(wEventID, bSrcType, dwSrcID, pszContext, nLen);		
	}

	EntityEvent(UID uidEntity) : m_uidEntity(uidEntity),m_Voter(this),m_Executer(this)
	{
        m_bSrcType = UID_2_TYPE(uidEntity);
	}
};
