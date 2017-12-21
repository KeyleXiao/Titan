/*******************************************************************
** �ļ���:	SceneLogicEvent.h 
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	��绪
** ��  ��:	2015-04-07
** ��  ��:	1.0
** ��  ��:	

			�����߼��¼�����
********************************************************************/

#pragma once

#include "IEntity.h"
#include "IEntityEvent.h"
#include "EventEngineT.h"
#include "IServerGlobal.h"

// ʵ���¼�������
class EntityEvent : public __IEntityEvent
{
private:
	TEventEngine<IEventVoteSink,EntityEvent,EventKey>       m_Voter;   // ͶƱ��
	TEventEngine<IEventExecuteSink,EntityEvent,EventKey>    m_Executer;// ͶƱ��

	UID    m_uidEntity;


public:
	virtual bool		FireVote(EventKey eventID, LPCSTR pszContext, int nLen)
	{
		if ( !m_Voter.Fire( eventID,pszContext,nLen ) )
			return false;

		// �����߳����ж��ģ�������Ƿ�����Ϣ(Ŀǰ��û��һ�����̶߳��ĵ�����)
		//if ( !gServerGlobal->getEventEngine()->FireVote(eventID,UID_2_TYPE(m_uidEntity), m_uidEntity, pszContext, nLen) )
		//	return false;

		return true;
	}

	virtual void		FireExecute(EventKey eventID, LPCSTR pszContext, int nLen)
	{
		m_Executer.Fire( eventID,pszContext,nLen );

		// �����߳����ж��ģ�������Ƿ�����Ϣ(Ŀǰ��û��һ�����̶߳��ĵ�����)
		//gServerGlobal->getEventEngine()->FireExecute(eventID,UID_2_TYPE(m_uidEntity), m_uidEntity, pszContext, nLen);
	}

	virtual bool		FireVoteExecute(EventKey eventID, LPCSTR pszContext, int nLen)
	{
		// �Ƚ���ͶƱ
		if (FireVote( eventID,pszContext,nLen ))
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
	bool operator() (IEventExecuteSink * pSink, EventKey wEventID, LPCSTR pszContext, int nLen)
	{
		if(pSink == NULL)
		{
			return false;
		}

		pSink->OnExecute(wEventID, UID_2_TYPE(m_uidEntity), m_uidEntity, pszContext, nLen);

		return true;
	}

	// �ص�ͶƱ�¼�
	bool operator() (IEventVoteSink * pSink, EventKey wEventID,LPCSTR pszContext, int nLen)
	{
		if(pSink == NULL)
		{
			return false;
		}

		return pSink->OnVote(wEventID, UID_2_TYPE(m_uidEntity), m_uidEntity, pszContext, nLen);		
	}

	EntityEvent(UID uidEntity) : m_uidEntity(uidEntity),m_Voter(this),m_Executer(this)
	{
	}
};
