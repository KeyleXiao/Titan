/*******************************************************************
** 文件名:	EntityEvent.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	2/26/2015
** 版  本:	1.0
** 描  述:	

			实体事件中心
********************************************************************/

#pragma once

#include "IEntity.h"
#include "IEntityEvent.h"
#include "EventEngineT.h"
#include "IServerGlobal.h"
#include "EntityHelper.h"

// 实体事件管理器
class EntityEvent : public __IEntityEvent
{
private:
	TEventEngine<IEventVoteSink,
		         EntityEvent,
				 EventKey,
				 EVENT_ENTITY_CREATE,
				 EVENT_ENTITY_MAX-EVENT_ENTITY_CREATE>       m_Voter;   // 投票器

	TEventEngine<IEventExecuteSink,
		         EntityEvent,
				 EventKey,
				 EVENT_ENTITY_CREATE,
				 EVENT_ENTITY_MAX-EVENT_ENTITY_CREATE>    m_Executer;// 投票器

	UID    m_uidEntity;
    BYTE m_bSrcType;

public:
	virtual bool		FireVote(EventKey eventID, LPCSTR pszContext, int nLen)
	{
		if ( !m_Voter.Fire( eventID,m_bSrcType,m_uidEntity,pszContext,nLen ) )
			return false;

		// 其他线程如有订阅，则给他们发给消息(目前还没有一个跨线程订阅的先例)
		//if ( !gServerGlobal->getEventEngine()->FireVote(eventID,UID_2_TYPE(m_uidEntity), m_uidEntity, pszContext, nLen) )
		//	return false;		
		return true;
	}

	virtual void		FireExecute(EventKey eventID, LPCSTR pszContext, int nLen)
	{
		m_Executer.Fire( eventID,m_bSrcType,m_uidEntity,pszContext,nLen );
	}

	/** 发送全局执行事件
	@param   wEventID ：事件ID
	@param   pszContext : 上下文
	@param   nLen : 上下文长度	  
	*/
	virtual void		FireExecute(GAMEEVENT_KEY eventID, LPCSTR pszContext, int nLen)
    {
        int nSceneID = UID_2_SCENE(m_uidEntity);
        // 忽略主城里不需要触发的事件
        if (IsMainMap(nSceneID))
        {
            if (eventID == EVENT_GAME_ENTITY_LEARN_SPELL
             || eventID == EVENT_GAME_ENTITY_FREEZE_CHANGED
             || eventID == EVENT_GAME_ENTITY_PREPARE_CURE
             || eventID == EVENT_GAME_ACTOR_TALENT_CHANGE
            )
               return;
        }

        // 事件发到场景逻辑
        SceneLogicEventHelper eventHelper(nSceneID);
        rkt::IEventEngine * pEventEngine = eventHelper.m_ptr;
        if(pEventEngine)
        {
            pEventEngine->FireExecute(eventID,m_bSrcType,m_uidEntity, pszContext, nLen);
        }
    }

	virtual bool		FireVoteExecute(EventKey eventID, LPCSTR pszContext, int nLen)
	{
		// 先进行投票
		if (!FireVote( eventID,pszContext,nLen ))
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
	bool operator() (IEventExecuteSink * pSink, EventKey wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
		if(pSink == NULL)
		{
			return false;
		}

		pSink->OnExecute(wEventID,bSrcType,dwSrcID, pszContext, nLen);

		return true;
	}

	// 回调投票事件
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
