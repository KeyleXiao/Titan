/*******************************************************************
** 文件名:	SceneLogicEvent.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-04-07
** 版  本:	1.0
** 描  述:	

			场景逻辑事件中心
********************************************************************/

#pragma once

#include "IEntity.h"
#include "IEntityEvent.h"
#include "EventEngineT.h"
#include "IServerGlobal.h"

// 实体事件管理器
class EntityEvent : public __IEntityEvent
{
private:
	TEventEngine<IEventVoteSink,EntityEvent,EventKey>       m_Voter;   // 投票器
	TEventEngine<IEventExecuteSink,EntityEvent,EventKey>    m_Executer;// 投票器

	UID    m_uidEntity;


public:
	virtual bool		FireVote(EventKey eventID, LPCSTR pszContext, int nLen)
	{
		if ( !m_Voter.Fire( eventID,pszContext,nLen ) )
			return false;

		// 其他线程如有订阅，则给他们发给消息(目前还没有一个跨线程订阅的先例)
		//if ( !gServerGlobal->getEventEngine()->FireVote(eventID,UID_2_TYPE(m_uidEntity), m_uidEntity, pszContext, nLen) )
		//	return false;

		return true;
	}

	virtual void		FireExecute(EventKey eventID, LPCSTR pszContext, int nLen)
	{
		m_Executer.Fire( eventID,pszContext,nLen );

		// 其他线程如有订阅，则给他们发给消息(目前还没有一个跨线程订阅的先例)
		//gServerGlobal->getEventEngine()->FireExecute(eventID,UID_2_TYPE(m_uidEntity), m_uidEntity, pszContext, nLen);
	}

	virtual bool		FireVoteExecute(EventKey eventID, LPCSTR pszContext, int nLen)
	{
		// 先进行投票
		if (FireVote( eventID,pszContext,nLen ))
			return false;

		// 再执行事件
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

	// 回调执行事件
	bool operator() (IEventExecuteSink * pSink, EventKey wEventID, LPCSTR pszContext, int nLen)
	{
		if(pSink == NULL)
		{
			return false;
		}

		pSink->OnExecute(wEventID, UID_2_TYPE(m_uidEntity), m_uidEntity, pszContext, nLen);

		return true;
	}

	// 回调投票事件
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
