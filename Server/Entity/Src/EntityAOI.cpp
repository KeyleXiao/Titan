/*******************************************************************
** 文件名:	EntityAOI.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/29/2014
** 版  本:	1.0
** 描  述:	

********************************************************************/

#include "stdafx.h"
#include "EntityAOI.h"
#include "ISceneManager.h"
#include "IServerGlobal.h"
#include "Vector3.h"
#include "IEntityProperty.h"
#include "AOIProxy_Monster.h"
#include "AOIProxy_Tank.h"
#include "AOIProxy_PlayerRole.h"
#include "IEntityEvent.h"
#include "ServiceHelper.h"
#include "ITimeSyncService.h"
#include "ITimerService.h"
#include "Monster.h"
#include "Property.h"
#include "IPlayerRole.h"
#include "IAIPart.h"
#include "MovePath.h"
#include "IPlayerKillPart.h"
#include "PlayerKillMessageDef.h"
#include "IMovePart.h"

// 最多只关注4096个对象
#define	  ROUND_UP(x, align) (((int) (x) + (align - 1)) & ~(align - 1))


EntityAOI::EntityAOI()
    : m_pMaster(0)
    , m_fSightRadii(AOI_DEFAULT_SIGHT)
    , m_bHide(false)
    , m_isGodMode(false)
    , m_nEventFlag(0)
    , m_bStealthInitialValue(false)
    , m_bAntiStealthInitialValue(false)
    , m_vLookLoc(0,0,0)
{
	m_dwBroadcastCD[0] = 0;
	m_dwBroadcastCD[1] = 0;

    m_StealthList.clear();
}


short EntityAOI::getPartID()
{
	return PART_AOI;
}

bool EntityAOI::onLoad( __IEntity* pEntity, int nLoginMode)
{
	if (!pEntity)
	{
		return false;
	}
	m_pMaster = pEntity;

	__IEntityEvent * pEntityEvent = m_pMaster->getEntityEvent();
	if (pEntityEvent)
	{
		pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this),EVENT_ENTITY_CREATE,__FUNCTION__);
        pEntityEvent->Subscibe(static_cast<IEventVoteSink*>(this), EVENT_ENTITY_DESTROY, __FUNCTION__);
        pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this),EVENT_ENTITY_MOVE_BEHAVIOUR_CHANGE,__FUNCTION__);
		pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this),EVENT_ENTITY_PROPERTY_BROADCAST,__FUNCTION__);
		pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this),EVENT_ENTITY_TRANSPORT,__FUNCTION__);
        pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_FLUSH_PROPERTY_2_ClIENT_IMMEDIATELY, __FUNCTION__);
        pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_ATTACK, __FUNCTION__);
        pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this),EVENT_ENTITY_CHANGE_STATE,__FUNCTION__);
        pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this),EVENT_ENTITY_FLY_STATE_CHANGE,__FUNCTION__);
        
        pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_LOCATION), __FUNCTION__);
        pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_LEVEL), __FUNCTION__);
        pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_HP), __FUNCTION__);
        pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_MAX_HP), __FUNCTION__);
        pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_MP), __FUNCTION__);
        pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_MAX_MP), __FUNCTION__);
        pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_STAMINA), __FUNCTION__);
        pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_MASK), __FUNCTION__);
        pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_FORMTYPE), __FUNCTION__);
        pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_OBSERVER), __FUNCTION__);
        

        pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ACTOR_PK_TARGET_CHANGE, __FUNCTION__);
        pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CONTINUE_PLAY, __FUNCTION__);
    }

	// 注册获得金币事件
	UID uidSelf = m_pMaster->getUID();
	if (UID_2_TYPE(uidSelf) == TYPE_PLAYER_ROLE)
	{
		SceneLogicEventHelper eventHelper(UID_2_SCENE((uidSelf)));
		if (eventHelper.m_ptr != NULL)
		{
			eventHelper.m_ptr->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ACTOR_TALENT_CHANGE, TYPE_PLAYER_ROLE, 0, __FUNCTION__);
		}
	}

    loadSight();
	
	return true;
}

__IEntity * EntityAOI::getHost()
{
	return m_pMaster;
}

void EntityAOI::release()
{
	if ( m_pAOIProxy!=0 )
	{
		m_pAOIProxy = AOI_PROXY_PTR(0);
	}

    // 关草丛暴露定时器
    for ( int camp=0; camp<3; ++camp )
    {
        gServerGlobal->getTimerService()->KillTimer(camp, this);
    }

	if ( m_pMaster )
	{
		// 退定系统消息
		__IEntityEvent * pEntityEvent = m_pMaster->getEntityEvent();
		if (pEntityEvent)
		{
			pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this),EVENT_ENTITY_CREATE);
			pEntityEvent->UnSubscibe(static_cast<IEventVoteSink*>(this),EVENT_ENTITY_DESTROY);
            pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this),EVENT_ENTITY_MOVE_BEHAVIOUR_CHANGE);
			pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this),EVENT_ENTITY_PROPERTY_BROADCAST);
			pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this),EVENT_ENTITY_TRANSPORT);
            pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this),EVENT_FLUSH_PROPERTY_2_ClIENT_IMMEDIATELY);
            pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_ATTACK);
            pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this),EVENT_ENTITY_CHANGE_STATE);
            pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this),EVENT_ENTITY_FLY_STATE_CHANGE);

            pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_LOCATION));
            pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_LEVEL));
            pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_HP));
            pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_MAX_HP));
            pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_MP));
            pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_MAX_MP));
            pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_STAMINA));
            pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_MASK));
            pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_FORMTYPE));
            pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), (EventKey)(EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_OBSERVER));
            

            pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ACTOR_PK_TARGET_CHANGE);
            pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CONTINUE_PLAY);

        }

		// 注册获得金币事件
		UID uidSelf = m_pMaster->getUID();
		if (UID_2_TYPE(uidSelf) == TYPE_PLAYER_ROLE)
		{
			SceneLogicEventHelper eventHelper(UID_2_SCENE((uidSelf)));
			if (eventHelper.m_ptr != NULL)
			{
				eventHelper.m_ptr->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ACTOR_TALENT_CHANGE, TYPE_PLAYER_ROLE, 0);
			}
		}
	}
	m_pMaster = 0;
	
	delete this;
}

bool EntityAOI::onSerialize( rkt::ibuffer & in,int flag )
{
    return true;
}

bool EntityAOI::deSerialize( rkt::obuf & out,int flag )
{
	return true;
}

float EntityAOI::getSight()
{
	return m_fSightRadii;
}

void EntityAOI::setSight( float radii )
{
	m_fSightRadii = radii;

    if(m_pMaster == 0 || m_pAOIProxy == 0)
    {
        return;
    }
    
    int nSceneID = m_pMaster->getSceneID();
    SceneServiceHelper helper(nSceneID);
    IGameSceneService* pScene = helper.m_ptr;
    if ( pScene==0 )
    {
        ErrorLn("EntityAOI::setSight can not found the scene!id=" << nSceneID);
        return;
    }
    pScene->setSight(m_pAOIProxy, (int)radii);

    // 发出视野改变事件
    __IEntityEvent* pEntityEvent = m_pMaster->getEntityEvent();
    if(pEntityEvent)
    {
        pEntityEvent->FireExecute(EVENT_ENTITY_SIGHT_DISTANCE_CHANGE, NULL, 0);
    }
}

bool EntityAOI::isHide()
{
	return m_bHide;
}

void EntityAOI::setHide( bool state )
{
	m_bHide = state;
}

void EntityAOI::setEventFlag( int flag )
{
	m_nEventFlag = flag;
}

bool EntityAOI::OnVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    switch (wEventID)
    {
    case EVENT_ENTITY_DESTROY:
        {
            if (pszContext != NULL)
            {
                onEvent_EntityDestroy(*(event_entity_destroy*)pszContext);
                return true;
            }
        }
        break;
    }

    return true;
}

void EntityAOI::OnTimer( unsigned long dwTimerID )
{
    // 如果不在草丛了 不需要刷新可见性
    if(m_pMaster == 0 || m_pAOIProxy == 0 || m_pAOIProxy->m_nForestID < 0)
        return;

    // 对某个阵营的草丛暴露结束 刷新可见性 
    SceneServiceHelper helper(m_pMaster->getSceneID());
    IGameSceneService* pScene = helper.m_ptr;
    if ( pScene )
    {
        pScene->updateObjVisible(m_pAOIProxy);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EntityAOI::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	switch (wEventID)
	{
	case EVENT_ENTITY_CREATE:
        {
		    onEvent_EntityCreate();
        }
		break;

    case EVENT_ENTITY_MOVE_BEHAVIOUR_CHANGE:
        {
            if( pszContext!=0 && nLen>=sizeof(event_entity_move_behaviour_change) )
            {
                onEvent_EntityMoveBehaviourChange((event_entity_move_behaviour_change*)pszContext);
            }
        }
        break;

	case EVENT_ENTITY_TRANSPORT:
		{
			if ( pszContext!=0 && nLen==sizeof(event_entity_transport) )
			{
				onEvent_EntityTransport( *((event_entity_transport*)pszContext) );
			}
			break;
		}
		break;

	case EVENT_ENTITY_PROPERTY_BROADCAST:
		{
		    onEvent_PropertyBroadcast( (void*)pszContext, nLen);
		}
		break;

    case EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_LOCATION:
        {
            if ( pszContext!=0 && nLen==sizeof(event_entity_property_change) )
            {
                onEvent_PropLocationChange( (event_entity_property_change*)pszContext );
            }
            break;
        }
        break;
    case EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_LEVEL:
        {
            if ( pszContext!=0 && nLen==sizeof(event_entity_property_change) )
            {
                onEvent_PropLevelChange( (event_entity_property_change*)pszContext );
            }
            break;
        }
        break;
    case EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_HP:
        {
            if ( pszContext!=0 && nLen==sizeof(event_entity_property_change) )
            {
                onEvent_PropHpChange( (event_entity_property_change*)pszContext );
            }
            break;
        }
        break;
    case EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_MAX_HP:
        {
            if ( pszContext!=0 && nLen==sizeof(event_entity_property_change) )
            {
                onEvent_PropMaxHpChange( (event_entity_property_change*)pszContext );
            }
            break;
        }
        break;
    case EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_MP:
        {
            if ( pszContext!=0 && nLen==sizeof(event_entity_property_change) )
            {
                onEvent_PropMpChange( (event_entity_property_change*)pszContext );
            }
            break;
        }
        break;
    case EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_MAX_MP:
        {
            if ( pszContext!=0 && nLen==sizeof(event_entity_property_change) )
            {
                onEvent_PropMaxMpChange( (event_entity_property_change*)pszContext );
            }
            break;
        }
        break;
    case EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_STAMINA:
        {
            if ( pszContext!=0 && nLen==sizeof(event_entity_property_change) )
            {
                onEvent_PropStaminaChange( (event_entity_property_change*)pszContext );
            }
            break;
        }
        break;
    case EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_MASK:
        {
            if ( pszContext!=0 && nLen==sizeof(event_entity_property_change) )
            {
                onEvent_PropMaskChange( (event_entity_property_change*)pszContext );
            }
            break;
        }
        break;
    case EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_FORMTYPE:
        {
            if ( pszContext!=0 && nLen==sizeof(event_entity_property_change) )
            {
                onEvent_PropFromTypeChange( (event_entity_property_change*)pszContext );
            }
            break;
        }
        break;
    case EVENT_ENTITY_PROPERTY_CHANGE + PROPERTY_OBSERVER:
        {
            if ( pszContext!=0 && nLen==sizeof(event_entity_property_change) )
            {
                onEvent_PropObserverChange( (event_entity_property_change*)pszContext );
            }
            break;
        }
        break;

	case EVENT_GAME_ACTOR_TALENT_CHANGE:
		{
			if (pszContext == NULL || nLen < sizeof(event_entity_talent_change))
			{
				return;
			}

			// 记录金币总量
			event_entity_talent_change* pEvent = (event_entity_talent_change*)pszContext;
			if (pEvent->byChangeType != ENUM_TALENT_REVOKE && pEvent->byChangeType != ENUM_TALENT_SELL)
			{
				if (pEvent->nChangePoint > 0)
				{
					m_pAOIProxy->m_nEcm += pEvent->nChangePoint;
				}
			}
		}
		break;

    case EVENT_ENTITY_ATTACK:
        {
            if ( pszContext==0 || nLen!=sizeof(event_entity_attack) || m_pAOIProxy == 0)
            {
                return;
            }

			event_entity_attack* pAttack = (event_entity_attack *)pszContext;

			// 更新正在攻击目标
			int nIndex = -1;
			for (int i = 0; i < MAX_ATTACKING_TARGET_NUM; ++i)
			{
				if (nIndex < 0 && getTickCountEx() >= m_pAOIProxy->m_AttackingTargets[i].nAttackTick + 2000)
				{
					nIndex = i;
				}

				if (m_pAOIProxy->m_AttackingTargets[i].uidTarget == pAttack->uidTarget)
				{
					nIndex = i;
					break;
				}
			}
			if (nIndex >= 0)
			{
				m_pAOIProxy->m_AttackingTargets[nIndex].uidTarget = pAttack->uidTarget;
				m_pAOIProxy->m_AttackingTargets[nIndex].nAttackTick = getTickCountEx();
			}

            // 不在草丛中，不走暴露逻辑
            if(m_pAOIProxy->m_nForestID < 0)
            {
                return;
            }
            
            // 对目标所在阵营暴露5s钟
            if(pAttack->nSpellFlag & SPELL::SpellFlag_ExposeRole)
            {
                int nTargetCamp = getProperty_Integer(pAttack->uidTarget, PROPERTY_CAMP);
                if(nTargetCamp == INVALID_INT_VALUE)
                {
                    ErrorLn(__FUNCTION__ << " nTargetCamp == INVALID_INT_VALUE");
                    return;
                }
                forestExpose(nTargetCamp, 5000);
            }
        }
        break;
    case EVENT_ENTITY_CHANGE_STATE:
        {
            if(m_pAOIProxy == 0)
            {
                return;
            }

            if(pszContext == NULL || nLen < sizeof(event_entity_change_state))
            {
                ErrorLn(__FUNCTION__ << " EVENT_ENTITY_CHANGE_STATE format error");
                return;
            }

            event_entity_change_state* pEvent = (event_entity_change_state*)pszContext; 
            m_pAOIProxy->m_nState = pEvent->nState;
            m_pAOIProxy->m_dwCurStateStartTick = getTickCountEx();
        }
        break;
    case EVENT_ENTITY_FLY_STATE_CHANGE:
        {
            if(m_pAOIProxy == 0)
            {
                return;
            }

            if(pszContext == NULL || nLen < sizeof(event_entity_fly_state_change))
            {
                ErrorLn(__FUNCTION__ << " EVENT_ENTITY_FLY_STATE_CHANGE format error");
                return;
            }

            event_entity_fly_state_change* pEvent = (event_entity_fly_state_change*)pszContext;
            if(m_pAOIProxy->m_bFlying != pEvent->bFly)
            {
                m_pAOIProxy->m_bFlying = pEvent->bFly;
                if(m_pAOIProxy->m_nForestID >= 0)
                {
                    onStealthChange();
                }
            }
        }
        break;
    case EVENT_ACTOR_PK_TARGET_CHANGE:
        {
            onEvent_PKTargetChange((void*)pszContext, nLen);
        }
        break;
    case EVENT_ENTITY_CONTINUE_PLAY:
        {
            // 通知客户端创建自己
            updateSelfCreate();

            if(m_pAOIProxy == 0)
            {
                return;
            }

            int nSceneID = UID_2_SCENE(m_pAOIProxy->m_uid);
            SceneHelper scene(nSceneID);
            if(scene.m_ptr)
            {
                scene.m_ptr->updateObjsInSight(m_pAOIProxy);

                rkt::IEventEngine* pEventEngine = gServerGlobal->getEventEngine();
                if (pEventEngine)
                {
                    pEventEngine->FireExecute(EVENT_CLIENT_ENTER_SCENE, SOURCE_TYPE_PERSON, m_pMaster->getIntProperty(PROPERTY_ID), NULL, 0);
                }
            }
            else
            {
                ErrorLn(__FUNCTION__ << ", scene not found");
            }
        }
        break;
	default:
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EntityAOI::onEvent_EntityCreate()
{
	updateSelfCreate();

	// 广播给周围的人
    SceneServiceHelper helper(m_pMaster->getSceneID());
	IGameSceneService* pScene = helper.m_ptr;
	if ( pScene )
	{
		m_pAOIProxy = AOI_PROXY_PTR(CreateAOIProxy());
        if ( m_pAOIProxy==NULL )
        {
            ErrorLn( __FUNCTION__<<"m_pAOIProxy==NULL" );
            return;
        }

        m_pAOIProxy->m_nSight = m_fSightRadii;
        m_pAOIProxy->m_nSightInGrid = m_pAOIProxy->m_nSight>0 ? m_pAOIProxy->m_nSight/AOI_GRID_SIZE : -1;  
        m_pAOIProxy->m_bIsStealing = m_bStealthInitialValue;
        m_pAOIProxy->m_bIsAntiStealth = m_bAntiStealthInitialValue;
        
		pScene->insertEntity(m_pAOIProxy,m_pMaster->getLocation(), MSG_FLAG_NO_BLOCK);

        // 发出客户端进场景事件
        if ( m_pMaster->getType()==TYPE_PLAYER_ROLE )
        {
            if(((__IPlayerRole*)m_pMaster)->getClientID() != INVALID_CLIENT)
            {
                rkt::IEventEngine* pEventEngine = gServerGlobal->getEventEngine();
                if (pEventEngine)
                {
                    pEventEngine->FireExecute(EVENT_CLIENT_ENTER_SCENE, SOURCE_TYPE_PERSON, m_pMaster->getIntProperty(PROPERTY_ID), NULL, 0);
                }
            }
        }
    }
    else
    {
        ErrorLn( __FUNCTION__<<" not find sceneid="<<m_pMaster->getSceneID() );
    }
}

// 通知客户端创建主角
void EntityAOI::updateSelfCreate()
{
    if ( m_pMaster->getType()==TYPE_PLAYER_ROLE )
    {
        UID uid = m_pMaster->getUID();
        // 通知客户端创建自己
        obuf256 data;
        SNetMsgHead head;
        head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
        head.byDestEndPoint= MSG_ENDPOINT_CLIENT;	
        head.byKeyModule   = MSG_MODULEID_ENTITY;	
        head.byKeyAction   = SC_MSG_ENTITY_CREATE;

        // 实体类统一用子头来用于转向部件
        SMsgEntityHead entityHead;
        entityHead.uidMaster = uid;				// 实体UID
        entityHead.byPartID = ENTITY_PART_MAX;	// 部件ID，ENTITY_PART_MAX时将由实体来处理

        // 实体要导出的内容
        SEntiyExportShadowContext context;
        context.byType		= m_pMaster->getType();		// 实体类型
        context.byFlag		= TO_CLIENT_SELF;			// 实体标识
        context.byCurState	= m_pMaster->getState();	// 当前状态
        context.loc			= m_pMaster->getLocation();
        context.fAngleY		= m_pMaster->getAngle().y;
        context.byReserve  |= (gServerGlobal->isOpenSupperStone() ? 0x1 : 0x0);   // 扩展数据

        // 压入数据包
        data<< head<< entityHead;
        // 实体要导出的内容
        data.push_back(&context, sizeof(context));

        m_pMaster->deSerialize(data, context.byFlag);

        ((__IPlayerRole*)m_pMaster)->send2Client(data.data(),data.size());
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EntityAOI::onEvent_EntityDestroy(event_entity_destroy & context)
{
	int nSceneID = m_pMaster->getSceneID();
    SceneServiceHelper helper(nSceneID);
	IGameSceneService* pScene = helper.m_ptr;
	if ( pScene==0 )
	{
		ErrorLn("EntityAOI::onEvent_EntityDestroy can not found the scene!id=" << nSceneID);
		return;
	}

    if ( context.bBroadAOI )
    {
        onMessage_DestroyNotify( INVALID_UID,0 );
    }
	pScene->removeEntity(m_pAOIProxy,m_pMaster->getLocation(), MSG_FLAG_NO_BLOCK);
}

// 只广播一下
void EntityAOI::onEvent_EntityMoveBehaviourChange(event_entity_move_behaviour_change* context)
{
    if ( m_pMaster==0 )
    {
        return;
    }

    __IMovePart* pMovePart = (__IMovePart*)m_pMaster->getEntityPart(PART_MOVE);
    if ( pMovePart==0 )
    {
        return;
    }

    Behaviour* pCurBehaviour = pMovePart->getCurrentBehaviour();
    if ( (pCurBehaviour!=NULL && pCurBehaviour->getType()!=context->nBehaviour)
        || (pCurBehaviour==NULL && context->nBehaviour!=BEHAVIOUR_TYPE_NONE) )
    {
        ErrorLn(__FUNCTION__ << __LINE__);
        return;
    }

    if ( m_pAOIProxy==0 )
    {
        ErrorLn("onEvent_EntityMoveSyn failed!m_pAOIProxy=0!");
        return;
    }

    SceneServiceHelper helper(m_pMaster->getSceneID());
    IGameSceneService* pScene = helper.m_ptr;
    if ( pScene == 0 )
    {
        return;
    }

    obuf256 data;
    SNetMsgHead head;
    head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
    head.byDestEndPoint= MSG_ENDPOINT_CLIENT;	
    head.byKeyModule   = MSG_MODULEID_ENTITY;	
    head.byKeyAction   = SC_MSG_ENTITY_MOVE;

    data << head;

    // 实体类统一用子头来用于转向部件
    SMsgEntityHead entityHead;
    entityHead.uidMaster = m_pMaster->getUID();	// 实体UID
    entityHead.byPartID = PART_MOVE;			// 部件ID，ENTITY_PART_MAX时将由实体来处理

    data << entityHead;

    // 是否开启移动包优化
    bool bEnableOptimize = false;
    ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
    if(pSchemeCenter != NULL)
    {
        ISchemeEntity* pSchemeEntity = pSchemeCenter->getSchemeEntity();
        if(pSchemeEntity != NULL)
        {
            bEnableOptimize = (pSchemeEntity->getEntityXml()->nOpenMovePkgOptimize != 0);
        }
    }

    // 压入是否开启优化字段
    BYTE byOpenOptimize = bEnableOptimize ? 1 : 0;
    data << byOpenOptimize;

    // 开启移动包优化
    if(bEnableOptimize)
    {
        msg_loc_sync_pkg_optimize_head optimizeHead;

        WORD& wSendPkgSeq = pMovePart->getSendPkgSeq();
        if(wSendPkgSeq == INVLAID_PKG_SEQ)
        {
            wSendPkgSeq++;
        }

        WORD& wSendKeyPkgSeq = pMovePart->getSendKeyPkgSeq();
        if(!context->bNeedBroadcast)
        {
            // 设置序号 以及前置包序号
            optimizeHead.wPkgSeq = wSendPkgSeq++;
            optimizeHead.wPrecondPkgSeq = wSendKeyPkgSeq; 
        }
        else
        {
            // 设置序号 以及前置包序号(关键包之间不会乱序 不需要设置前置包)
            optimizeHead.wPkgSeq = wSendPkgSeq;
            optimizeHead.wPrecondPkgSeq = wSendPkgSeq; 

            // 设置当前关键包
            wSendKeyPkgSeq = wSendPkgSeq;
            ++wSendPkgSeq;
        }

        data << optimizeHead; 
    }

    msg_creature_move_sync msg;
    msg.uid = context->uid;
    msg.nPosX = context->move.vPosition.x * 10;
    msg.nPosY = context->move.vPosition.y * 10; 
    msg.nPosZ = context->move.vPosition.z * 10;
    msg.nRotX = context->move.vRotation.x * 10;
    msg.nRotY = context->move.vRotation.y * 10;
    msg.nRotZ = context->move.vRotation.z * 10;
    msg.nVelocityX = context->move.vVelocity.x * 10;
    msg.nVelocityY = context->move.vVelocity.y * 10; 
    msg.nVelocityZ = context->move.vVelocity.z * 10;
    msg.nBehaviour = context->nBehaviour; 
    msg.nCreaturState = -1; // 默认-1 忽略这个字段

    // 广播真人客户端状态
    if(m_pMaster->getType() == TYPE_PLAYER_ROLE)
    {
        __IPlayerRole* pRole = (__IPlayerRole*)m_pMaster;
        if(pRole->getClientID() != INVALID_CLIENT)
        {
            msg.nCreaturState = context->nCreatureState;
        }
    }

    data << msg;

    if ( pCurBehaviour!=NULL )
    {
         BehaviourCtx& ctx = pCurBehaviour->getCtx(); 
         data << ctx;
    }

	if ( !data.good() )
	{
		ErrorLn("output path data to data buffer failed!function onEvent_EntityMoveSyn");
		return;
	}

    // 如果是客户端同步过来的位置改变，不要再同步回去了，防止客户端表现不平滑
	bool bBroadcastIgnoreSelf = !(context->bNotifySelf);

	if(!context->bNeedBroadcast)
    {
        int nBroadcastID = BROADCAST_MOVE;
        if(bEnableOptimize)
        {
            nBroadcastID = BROADCAST_TYPE_COUNT;  // NO_ACK
        }

 		ulong now = getTickCount();
 		if ( now-m_dwBroadcastCD[1]>=500 )
 		{
 			// 500毫秒广播一次给视野中的玩家
 			m_dwBroadcastCD[1] = now;
 			m_dwBroadcastCD[0] = now;
			pScene->broadcast(context->move.vPosition,0,nBroadcastID,string(data.data(),data.size()),m_pAOIProxy, bBroadcastIgnoreSelf);
 		}
 		else if( now-m_dwBroadcastCD[0]>=50 )
 		{
 			 // 50毫秒广播一次给周围玩家
 			m_dwBroadcastCD[0] = now;
 			pScene->broadcast_nearLoc(context->move.vPosition,0,nBroadcastID,string(data.data(),data.size()),m_pAOIProxy, bBroadcastIgnoreSelf);
 		}
     }else
 	{
 		pScene->broadcast(context->move.vPosition,0,BROADCAST_MOVE,string(data.data(),data.size()),m_pAOIProxy, bBroadcastIgnoreSelf);
 	}

     // ghost状态 连自己都不广播 附身状态客户端位置会不同步
     if(m_pAOIProxy != 0 && m_pAOIProxy->m_bGhost && m_pMaster != NULL && m_pMaster->getType() == TYPE_PLAYER_ROLE && ((__IPlayerRole*)m_pMaster)->getClientID() != INVALID_UID)
     {
         ((__IPlayerRole*)m_pMaster)->send2Client(data.data(), data.size());
     }
}

// note: 先不考虑跨地图传送
void EntityAOI::onEvent_EntityTransport(event_entity_transport &event_msg)
{
	if ( m_pAOIProxy==0 )
	{
		ErrorLn(__FUNCTION__<<" failed!m_pAOIProxy=0!");
		return ;
	}

    if(event_msg.oldScene == 0)
    {
        event_msg.oldScene = m_pMaster->getSceneID();
    }
    if(event_msg.newScene == 0)
    {
        event_msg.newScene = m_pMaster->getSceneID();
    }

    if(event_msg.oldScene != event_msg.newScene)
    {
        // 迁移

        // todo. 跨场景传送时，不能只给场景ID 还需要知道目标场景的服务器ID 因为不同场景服的动态场景ID是复用的.
    }
    else
    {
        SceneServiceHelper oldSceneHelper(event_msg.oldScene);
        IGameSceneService* pOldScene = oldSceneHelper.m_ptr;
        if(pOldScene == NULL)
        {
            ErrorLn(__FUNCTION__ << __LINE__);
            return;
        }
        // 先通知客户端瞬移，客户端假如瞬移到了不该有视野的地方，后续的操作会触发视野消失。
        // 在旧的位置广播传送。
        // （因为旧的位置可能有一部分人在传送后还有这个角色的视野，如果不广播，这部分人就不会知道这个角色传送了）
        SNetMsgHead head;
        head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
        head.byDestEndPoint= MSG_ENDPOINT_CLIENT;	
        head.byKeyModule   = MSG_MODULEID_ENTITY;	
        head.byKeyAction   = SC_MSG_ENTITY_TRANSPORT;

        // 实体类统一用子头来用于转向部件
        SMsgEntityHead entityHead;
        entityHead.uidMaster = m_pMaster->getUID();	// 实体UID
        entityHead.byPartID = PART_MOVE;			// 部件ID，ENTITY_PART_MAX时将由实体来处理

        msg_creature_transport_sync msg;
        msg.nSceneSrc = event_msg.oldScene;
        msg.ptSrc = event_msg.oldLoc;
        msg.nSceneDst = event_msg.newScene;
        msg.nMapDst = pOldScene->getMapID();
        msg.ptDes = event_msg.newMove;
        msg.nReason = event_msg.nReason;

        obuf256 data;
        data << head << entityHead;

        // 是否开启移动包优化
        bool bEnableOptimize = false;
        ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
        if(pSchemeCenter != NULL)
        {
            ISchemeEntity* pSchemeEntity = pSchemeCenter->getSchemeEntity();
            if(pSchemeEntity != NULL)
            {
                bEnableOptimize = (pSchemeEntity->getEntityXml()->nOpenMovePkgOptimize != 0);
            }
        }

        // 移动包优化
        if(bEnableOptimize)
        {
            msg_loc_sync_pkg_optimize_head optimizeHead;

            __IMovePart* pMovePart = (__IMovePart*)m_pMaster->getEntityPart(PART_MOVE);
            if ( pMovePart==0 )
            {
                return;
            }

            WORD& wSendPkgSeq = pMovePart->getSendPkgSeq();
            if(wSendPkgSeq == INVLAID_PKG_SEQ)
            {
                wSendPkgSeq++;
            }

            WORD& wSendKeyPkgSeq = pMovePart->getSendKeyPkgSeq();

            // 设置序号 以及前置包序号(关键包之间不会乱序 不需要设置前置包)
            optimizeHead.wPkgSeq = wSendPkgSeq;
            optimizeHead.wPrecondPkgSeq = wSendPkgSeq; 

            // 设置当前关键包
            wSendKeyPkgSeq = wSendPkgSeq;
            ++wSendPkgSeq;

            data << optimizeHead; 
        }

        // 收到这个消息后，客户端只做瞬移就可以了
        data << msg;

        // ghost状态 连自己都不广播 附身状态客户端位置会不同步
        if(m_pAOIProxy != 0 && m_pAOIProxy->m_bGhost && m_pMaster != NULL && m_pMaster->getType() == TYPE_PLAYER_ROLE && ((__IPlayerRole*)m_pMaster)->getClientID() != INVALID_UID)
        {
            ((__IPlayerRole*)m_pMaster)->send2Client(data.data(), data.size());
        }
        else
        {
            pOldScene->broadcast(event_msg.oldLoc,0,BROADCAST_MOVE, string(data.data(),data.size()),m_pAOIProxy, false);
        }
    }
}

void EntityAOI::forestExpose( int nCamp,int nInterval )
{
    if(nCamp < 0 || nCamp >= sizeof(m_pAOIProxy->m_ExposedEndTick) / sizeof(DWORD))
    {
        return;
    }

    // 不在草丛 不处理暴露逻辑
    if(m_pAOIProxy->m_nForestID < 0)
    {
        return;
    }

    m_pAOIProxy->m_ExposedEndTick[nCamp] = getTickCountEx() + nInterval;

    // 立即更新可见性
    SceneServiceHelper helper(m_pMaster->getSceneID());
    IGameSceneService* pScene = helper.m_ptr;
    if ( pScene )
    {
        pScene->updateObjVisible(m_pAOIProxy);
    }

    // 启动对这个阵营暴露时间结束定时器 否则暴露结束客户端不知道
    gServerGlobal->getTimerService()->KillTimer(nCamp, this);
    // +32因为系统timer里用的gettickcount精度不高 可能会提前触发 
    // 假如还没到暴露结束时间更新可见性并不会有变化
    gServerGlobal->getTimerService()->SetTimer(nCamp, nInterval + 32, this, 1, __FUNCTION__);
}

// 设置隐形(对其他阵营隐形)
void EntityAOI::setStealth( bool bSteal, bool bAlpha2SelfCamp, int nEffectID )
{
    if (bSteal)
    {
        m_StealthList.push_back(nEffectID);
    }
    else
    {
        STEALTH_LIST::iterator Iter = find(m_StealthList.begin(), m_StealthList.end(), nEffectID);
        if (Iter != m_StealthList.end())
        {
            m_StealthList.erase(Iter);
        }
    }

    // 如果空了表示非隐身状态
    if (m_StealthList.empty())
    {
        bSteal = false;
    }
    else
    {
        bSteal = true;
    }

    if( m_pAOIProxy == 0)
    {
        m_bStealthInitialValue = bSteal;
        return;
    }

    if( m_pAOIProxy->m_bIsStealing == bSteal )
    {
        return;
    }

    m_pAOIProxy->m_bIsStealing = bSteal;

    // 立即更新可见性
    int nSceneID = m_pMaster->getSceneID();
    SceneServiceHelper helper(nSceneID);
    IGameSceneService* pScene = helper.m_ptr;
    if ( pScene )
    {
        pScene->updateObjVisible(m_pAOIProxy);
    }

    // 本阵营广播隐形(正式的情况是
    //  在deSerialize里也要有隐形信息，不然后来登录的角色会没有时机知道队友是隐形的)
    // 草丛ID也要放到deSerialize里 todo. 
    if(bAlpha2SelfCamp)
    {
        onStealthChange(); // 本阵营透明处理
    }

    m_pMaster->getEntityEvent()->FireExecute(EVENT_ENTITY_STEALTH_CHANGE, (LPCSTR)&bSteal, sizeof(bool));
}

// 反隐形
void EntityAOI::antiStealth( bool bEnable )
{ 
    if( m_pAOIProxy == 0)
    {
        m_bAntiStealthInitialValue = bEnable;
        return;
    }

    if(m_pAOIProxy->m_bIsAntiStealth == bEnable)
        return;

    m_pAOIProxy->m_bIsAntiStealth = bEnable;

    // 附近有非己方隐形单位 更新他们的可见性
    SceneServiceHelper helper(m_pMaster->getSceneID());
    IGameSceneService* pScene = helper.m_ptr;
    if ( pScene )
    {
        pScene->onUpdateObjAntiStealth(m_pAOIProxy);
    }

    // 通知本阵营的队友，头顶上有光圈。。表现 todo.
}

// 设置（取消）对某阵营一直可见
void EntityAOI::keepInsight( int camp, bool bEnable )
{
    if( m_pAOIProxy == 0)
    {
        ErrorLn("EntityAOI::keepInsight m_pAOIProxy == 0");
        return;
    }

    if( bEnable )
    {
        m_pAOIProxy->m_nKeepInsightFlag |= CAMP_MASK(camp); 
    }
    else
    {
        m_pAOIProxy->m_nKeepInsightFlag &= ~CAMP_MASK(camp);
    }

    // 立即更新可见性
    SceneServiceHelper helper(m_pMaster->getSceneID());
    IGameSceneService* pScene = helper.m_ptr;
    if ( pScene )
    {
        pScene->updateObjVisible(m_pAOIProxy);
    }

    // 通知本阵营的队友，头顶上有被点亮标志。。 表现 todo.
}

// 设置对有所阵营隐形（包括自己）
void EntityAOI::setGhostState( bool bGhost )
{
    if( m_pAOIProxy == 0)
    {
        ErrorLn("EntityAOI::setGhostState m_pAOIProxy == 0");
        return;
    }

    m_pAOIProxy->m_bGhostOld = m_pAOIProxy->m_bGhost; 
    m_pAOIProxy->m_bGhost = bGhost;

    // 立即更新可见性
    SceneServiceHelper helper(m_pMaster->getSceneID());
    IGameSceneService* pScene = helper.m_ptr;
    if ( pScene )
    {
        pScene->updateObjVisible(m_pAOIProxy);
    }
}

// 是否在某阵营的视野内
bool EntityAOI::isInsight( int camp )
{
    if( m_pAOIProxy == 0)
    {
        return false;
    }

    return m_pAOIProxy->m_nVisibaleFlag & CAMP_MASK(camp);
}

void EntityAOI::onEvent_PropertyBroadcast(void* pData, int nLen )
{
    if(pData == NULL || nLen < sizeof(event_entity_properties))
        return;

    event_entity_properties * pEvent = (event_entity_properties*) pData;
    if(pEvent->nPropCount <= 0)
        return;

    // 属性刷新到客户端
    std::set<int> properties;
    int *pItem = CAST_TYPE(int *,(pEvent+1));
    for(int i=0; i<pEvent->nPropCount; ++i)
    {
        int nPropID= (*pItem);
        properties.insert( nPropID);

        pItem++;
    }

    flushProperty2Client(properties, pEvent->isBroadcast);
}

void EntityAOI::onEvent_PropLocationChange(event_entity_property_change* context)
{
    if(context->nPropertyID != PROPERTY_LOCATION)
    {
        ErrorLn(__FUNCTION__ << __LINE__);
        return;
    }

    if(m_pAOIProxy == 0)
    {
        ErrorLn(__FUNCTION__ <<  ", update aoiproxy loc failed!m_pAOIProxy=0!");
        return;
    }

    SceneServiceHelper helper(m_pMaster->getSceneID());
    IGameSceneService* pScene = helper.m_ptr;
    if ( pScene == 0 )
    {
        return;
    }

    Vector3 loc;
    memcpy(&loc, context->data, context->nLen);
    pScene->moveEntity(m_pAOIProxy, loc);
}

void EntityAOI::onEvent_PropLevelChange(event_entity_property_change* context)
{
    if(context->nPropertyID != PROPERTY_LEVEL)
    {
        ErrorLn(__FUNCTION__ << __LINE__);
        return;
    }

    if(m_pAOIProxy == 0)
    {
        ErrorLn(__FUNCTION__ <<  ", update aoiproxy level failed!m_pAOIProxy=0!");
        return;
    }

    int nLevel = 0;
    switch(context->nLen)
    {
    case sizeof(BYTE):
        nLevel = *(BYTE *)&context->data;
    case  sizeof(short):
        nLevel = *(unsigned short *)&context->data;
    case sizeof(int):
        nLevel = *(int*)context->data; 
    }
    m_pAOIProxy->m_nLevel = nLevel;

    // 测试代码
//     if(m_pMaster->getType() == TYPE_PLAYER_ROLE && nLevel == 10)
//     {
//         ((__IPlayerRole*)m_pMaster)->changeVoc(14);
//         m_pMaster->setIntProperty(PROPERTY_SKIN, 22);
//     }
}

void EntityAOI::onEvent_PropHpChange(event_entity_property_change* context)
{
    if(context->nPropertyID != PROPERTY_HP)
    {
        ErrorLn(__FUNCTION__ << __LINE__);
        return;
    }

    if(m_pAOIProxy == 0)
    {
        ErrorLn(__FUNCTION__ <<  ", update aoiproxy hp failed!m_pAOIProxy=0!");
        return;
    }

    int nHP = 0;
    switch(context->nLen)
    {
    case sizeof(BYTE):
        nHP = *(BYTE *)&context->data;
    case  sizeof(short):
        nHP = *(unsigned short *)&context->data;
    case sizeof(int):
        nHP = *(int*)context->data; 
    }
    m_pAOIProxy->m_nCurHp = nHP;
}

void EntityAOI::onEvent_PropMaxHpChange(event_entity_property_change* context)
{
    if(context->nPropertyID != PROPERTY_MAX_HP)
    {
        ErrorLn(__FUNCTION__ << __LINE__);
        return;
    }

    if(m_pAOIProxy == 0)
    {
        ErrorLn(__FUNCTION__ <<  ", update aoiproxy max hp failed!m_pAOIProxy=0!");
        return;
    }

    int nMaxHp = 0;
    switch(context->nLen)
    {
    case sizeof(BYTE):
        nMaxHp = *(BYTE *)&context->data;
    case  sizeof(short):
        nMaxHp = *(unsigned short *)&context->data;
    case sizeof(int):
        nMaxHp = *(int*)context->data; 
    }
    m_pAOIProxy->m_nMaxHp = nMaxHp;
}

void EntityAOI::onEvent_PropMpChange(event_entity_property_change* context)
{
    if(context->nPropertyID != PROPERTY_MP)
    {
        ErrorLn(__FUNCTION__ << __LINE__);
        return;
    }

    if(m_pAOIProxy == 0)
    {
        ErrorLn(__FUNCTION__ <<  ", update aoiproxy mp failed!m_pAOIProxy=0!");
        return;
    }

    int nMp = 0;
    switch(context->nLen)
    {
    case sizeof(BYTE):
        nMp = *(BYTE *)&context->data;
    case  sizeof(short):
        nMp = *(unsigned short *)&context->data;
    case sizeof(int):
        nMp = *(int*)context->data; 
    }

    m_pAOIProxy->m_nCurMp = nMp;
}

void EntityAOI::onEvent_PropMaxMpChange(event_entity_property_change* context)
{
    if(context->nPropertyID != PROPERTY_MAX_MP)
    {
        ErrorLn(__FUNCTION__ << __LINE__);
        return;
    }

    if(m_pAOIProxy == 0)
    {
        ErrorLn(__FUNCTION__ <<  ", update aoiproxy max mp failed!m_pAOIProxy=0!");
        return;
    }

    int nMaxMp = 0;
    switch(context->nLen)
    {
    case sizeof(BYTE):
        nMaxMp = *(BYTE *)&context->data;
    case  sizeof(short):
        nMaxMp = *(unsigned short *)&context->data;
    case sizeof(int):
        nMaxMp = *(int*)context->data; 
    }

    m_pAOIProxy->m_nMaxMp = nMaxMp;
}

void EntityAOI::onEvent_PropStaminaChange(event_entity_property_change* context)
{
    if(context->nPropertyID != PROPERTY_STAMINA)
    {
        ErrorLn(__FUNCTION__ << __LINE__);
        return;
    }

    if(m_pAOIProxy == 0)
    {
        ErrorLn(__FUNCTION__ <<  ", update aoiproxy max mp failed!m_pAOIProxy=0!");
        return;
    }

    int nStamina = 0;
    switch(context->nLen)
    {
    case sizeof(BYTE):
        nStamina = *(BYTE *)&context->data;
    case  sizeof(short):
        nStamina = *(unsigned short *)&context->data;
    case sizeof(int):
        nStamina = *(int*)context->data; 
    }

    m_pAOIProxy->m_nCurStamina = nStamina;
}

void EntityAOI::onEvent_PropMaskChange(event_entity_property_change* context)
{
    if(context->nPropertyID != PROPERTY_MASK)
    {
        ErrorLn(__FUNCTION__ << __LINE__);
        return;
    }

    if(m_pAOIProxy == 0)
    {
        ErrorLn(__FUNCTION__ <<  ", update aoiproxy failed!m_pAOIProxy=0!");
        return;
    }

    int nMask = 0;
    switch(context->nLen)
    {
    case sizeof(BYTE):
        nMask = *(BYTE *)&context->data;
    case  sizeof(short):
        nMask = *(unsigned short *)&context->data;
    case sizeof(int):
        nMask = *(int*)context->data; 
    }

    m_pAOIProxy->m_bCanSelected = !(nMask & MASKFLAG_NOT_SELECT);
}

void EntityAOI::onEvent_PropFromTypeChange(event_entity_property_change* context)
{
    if(context->nPropertyID != PROPERTY_FORMTYPE)
    {
        ErrorLn(__FUNCTION__ << __LINE__);
        return;
    }

    if(m_pAOIProxy == 0)
    {
        ErrorLn(__FUNCTION__ <<  ", update aoiproxy failed!m_pAOIProxy=0!");
        return;
    }

    int nWhere = 0;
    switch(context->nLen)
    {
    case sizeof(BYTE):
        nWhere = *(BYTE *)&context->data;
    case  sizeof(short):
        nWhere = *(unsigned short *)&context->data;
    case sizeof(int):
        nWhere = *(int*)context->data; 
    }

    m_pAOIProxy->m_nWhere = nWhere;
}

 void EntityAOI::onEvent_PropObserverChange(event_entity_property_change* context)
 {
     if(context->nPropertyID != PROPERTY_OBSERVER)
     {
         ErrorLn(__FUNCTION__ << __LINE__);
         return;
     }

     if(m_pAOIProxy == 0)
     {
         return;
     }

     int nObserver = 0;
     switch(context->nLen)
     {
     case sizeof(BYTE):
         nObserver = *(BYTE *)&context->data;
     case  sizeof(short):
         nObserver = *(unsigned short *)&context->data;
     case sizeof(int):
         nObserver = *(int*)context->data; 
     }

     SceneServiceHelper helper(m_pMaster->getSceneID());
     IGameSceneService* pScene = helper.m_ptr;
     if ( pScene == 0 )
     {
         return;
     }

     pScene->setObserver(m_pAOIProxy, nObserver != 0);
 }

void EntityAOI::onEvent_PKTargetChange(void* pData, int nLen)
{
    if(pData == NULL || nLen < sizeof(event_actor_pk_target_change))
    {
        return;
    }

    if(m_pAOIProxy == 0)
    {
        return;
    }

    event_actor_pk_target_change* pEvent = (event_actor_pk_target_change*)pData;
    if(m_pAOIProxy->m_uidPKTarget == pEvent->uidTarget)
    {
        return;
    }

    UID uidOldTarget = m_pAOIProxy->m_uidPKTarget; 
    // 更新pk目标
    m_pAOIProxy->m_uidPKTarget = pEvent->uidTarget;

    // 自己隐身 才更新自己可见性
    if(!m_pAOIProxy->m_bIsStealing)
    {
        return;
    }

    // 进旧的pk目标的视野
    if(uidOldTarget != INVALID_UID)
    {
        m_pAOIProxy->onInOutSight(-1, true, uidOldTarget);
    }

    // 出新的pk目标的视野
    if(m_pAOIProxy->m_uidPKTarget != INVALID_UID)
    {
        m_pAOIProxy->onInOutSight(-1, false, m_pAOIProxy->m_uidPKTarget);
    }
}

//////////////////////////////////////////////////////////////////////////

bool EntityAOI::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
	if ( data==0 || len<sizeof(int) )
		return false;

	ibuffer buffer( data,len );

    switch ( msgId )
    {
	case SS_MSG_ENTITY_CREATE_NOTIFY:
		{
			UID uidPlayer;int camp;
			buffer >> uidPlayer >> camp;
			if ( !buffer )
				return false;
			onMessage_CreateNotify(uidPlayer,camp);
			return true;
		}
	case SS_MSG_ENTITY_DESTROY_NOTIFY:
        {
            UID uidPlayer;int camp;
            buffer >> uidPlayer >> camp;
            if ( !buffer )
                return false;
            onMessage_DestroyNotify(uidPlayer,camp);
            return true;
        }
	case SS_MSG_ENTITY_IN_SIGHT:
		{
			int camp; UID uidTarget;
			buffer >> camp >> uidTarget;
			if ( !buffer )
				return false;
			onMessage_InSight(camp, uidTarget);
			return true;
		}
    case SS_MSG_ENTITY_FOREST_CHANGE:
        {
            int camp; int nForestID;
            buffer >> camp >> nForestID;
            if ( !buffer )
                return false;
            onStealthChange();
            // 草丛事件(进入离开草丛)
            forestEvent(nForestID);
            return true;
        }
	case SS_MSG_ENTITY_OUT_OF_SIGHT:
		{
			int camp; UID uidTarget;
			buffer >> camp >> uidTarget;
			if ( !buffer )
				return false;
			onMessage_OutOfSight(camp, uidTarget);
			return true;
		}

    case CS_MSG_ENTITY_AOI_SYNC_INFO:       // 客户端同步AOI信息到服务器
        {
            if(len >= sizeof(msg_entity_aoi_sync_info))
            {
                msg_entity_aoi_sync_info msg;
                buffer >> msg;

                // 设置上帝模式
                m_isGodMode = msg.isGodMode;
                m_vLookLoc = msg.vLookLoc;

                m_pAOIProxy->m_LookLoc = m_isGodMode ? m_vLookLoc : INVALID_LOC;

                return true;
            }
        }
    default:
        return false;
    }

	return false;
}

// 当前实体进入某阵营视野
void EntityAOI::onMessage_InSight(int camp, UID uidTarget)
{
    SNetMsgHead head;
    head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
    head.byDestEndPoint= MSG_ENDPOINT_CLIENT;	
    head.byKeyModule   = MSG_MODULEID_ENTITY;	
    head.byKeyAction   = SC_MSG_ENTITY_APPEAR;

    // 实体类统一用子头来用于转向部件
    SMsgEntityHead entityHead;
	entityHead.uidMaster = m_pMaster->getUID();
    entityHead.byPartID = ENTITY_PART_MAX;

	///////////////////////////////////////////////////////////////////////////////////////////////
	// 这段代码是需要修改的，重新进入视野，到底要发哪些数据，以及buff等其他部件如何同步需要另行设计
    // 实体要导出的内容
    SEntiyExportShadowContext context;
    context.byType			= m_pMaster->getType();					// 实体类型
    context.byFlag			= TO_CLIENT_OTHER;						// 实体标识
    context.byCurState		= m_pMaster->getState();					// 当前状态
    context.loc				= m_pMaster->getLocation();

    obuf256 data;
    data << head << entityHead;

    // 实体要导出的内容
    data.push_back(&context, sizeof(context));

    m_pMaster->deSerialize(data,context.byFlag);
    
    if(uidTarget != INVALID_UID)
    {
        send2Client(uidTarget, PACKAGE_PTR(new std::string(data.data(),data.size())) );

        if(getProperty_Integer(uidTarget, PROPERTY_OBSERVER) != 0)
        {
            // 是否需要给ob加一个 TO_OB_FLAG ?  
            m_pMaster->getEntityEvent()->FireExecute(EVENT_ENTITY_ENTER_A_OB_SIGHT, (LPCSTR)&uidTarget, sizeof(UID));
        }
    }
    else
    {
        scene_broadcast_all( BROADCAST_IN_SIGHT,std::string(data.data(),data.size()),CAMP_MASK(camp) );

        // 发进入某个阵营视野事件
        m_pMaster->getEntityEvent()->FireExecute(EVENT_ENTITY_ENTER_A_CAMP_SIGHT, (LPCSTR)&camp, sizeof(int));

		if (camp != m_pAOIProxy->m_nCamp)
		{
			UID uidEntity = entityHead.uidMaster;
			// NPC只关心非中立阵营
			// 角色关心敌方与中立
			if ((UID_2_TYPE(uidEntity) == TYPE_MONSTER && camp != 0) || UID_2_TYPE(uidEntity) == TYPE_PLAYER_ROLE)
			{
				AIServiceHelper aiHelper(UID_2_SCENE(uidEntity));
				if (aiHelper.m_ptr)
				{
					AI_CONTEXT context;
					context.nParam1 = camp;
					aiHelper.m_ptr->onEvent(uidEntity, AI_EVENT_ENTER_ENEMY_SIGHT, context);
				}
			}
		}
    }

	// 如果是隐形广播实体，则触发检测当前实体所处野怪点的野怪生死，并进行对应操作
	if (m_pMaster->getType() == TYPE_MONSTER) 
		if ((m_pMaster->getIntProperty(PROPERTY_SEX)) == MONSTER_TYPE_WILD_MONITOR)
			reinterpret_cast<Monster *>(m_pMaster)->msgToSpecCampWildInfo(m_pMaster->getUID(), camp);
}

// 当前实体离开某阵营视野
void EntityAOI::onMessage_OutOfSight(int camp, UID uidTarget)
{
	SNetMsgHead head;
    head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
    head.byDestEndPoint= MSG_ENDPOINT_CLIENT;	
    head.byKeyModule   = MSG_MODULEID_ENTITY;	
    head.byKeyAction   = SC_MSG_ENTITY_DISAPPEAR;

    // 实体类统一用子头来用于转向部件
    SMsgEntityHead entityHead;
    entityHead.uidMaster = m_pMaster->getUID();
    entityHead.byPartID = ENTITY_PART_MAX;
	
    obuf256 data;
	data << head << entityHead << m_pMaster->getUID();

    if(uidTarget != INVALID_UID)
    {
        send2Client(uidTarget, PACKAGE_PTR(new std::string(data.data(),data.size())));
    }
    else
    {
        scene_broadcast_all( BROADCAST_IN_SIGHT,std::string(data.data(),data.size()),CAMP_MASK(camp) );

        // 发离开某个阵营视野事件
        m_pMaster->getEntityEvent()->FireExecute(EVENT_ENTITY_LEAVE_A_CAMP_SIGHT, (LPCSTR)&camp, sizeof(int));

		if (camp != m_pAOIProxy->m_nCamp)
		{
			UID uidEntity = entityHead.uidMaster;
			// NPC只关心非中立阵营
			// 角色关心敌方与中立
			if ((UID_2_TYPE(uidEntity) == TYPE_MONSTER && camp != 0) || UID_2_TYPE(uidEntity) == TYPE_PLAYER_ROLE)
			{
				AIServiceHelper aiHelper(UID_2_SCENE(uidEntity));
				if (aiHelper.m_ptr)
				{
					AI_CONTEXT context;
					context.nParam1 = camp;
					aiHelper.m_ptr->onEvent(uidEntity, AI_EVENT_LEAVE_ENEMY_SIGHT, context);
				}
			}
		}
    }
}

void EntityAOI::onStealthChange()
{
    if(m_pAOIProxy == 0)
    {
        return;
    }

    SNetMsgHead head;
    head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
    head.byDestEndPoint= MSG_ENDPOINT_CLIENT;	
    head.byKeyModule   = MSG_MODULEID_ENTITY;	
    head.byKeyAction   = SC_MSG_ENTITY_STEALTH_CHANGE;

    // 实体类统一用子头来用于转向部件
    SMsgEntityHead entityHead;
    entityHead.uidMaster = m_pMaster->getUID();
    entityHead.byPartID = ENTITY_PART_MAX;
    
    bool bIsStealing = ( m_pAOIProxy->m_bIsStealing 
        || (m_pAOIProxy->m_nForestID >= 0 && !m_pAOIProxy->m_bFlying) );

    obuf256 data;
    data << head << entityHead << m_pMaster->getUID() << bIsStealing;

    scene_broadcast_all( BROADCAST_SCENE, std::string(data.data(),data.size()), CAMP_MASK(m_pAOIProxy->m_nCamp) );
}

// 当前实体进入某个人视野(创建实体)
void EntityAOI::onMessage_CreateNotify(UID uidPlayer,int camp)
{
    SNetMsgHead head;
    head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
    head.byDestEndPoint= MSG_ENDPOINT_CLIENT;	
    head.byKeyModule   = MSG_MODULEID_ENTITY;	
    head.byKeyAction   = SC_MSG_ENTITY_CREATE;

    // 实体类统一用子头来用于转向部件
    SMsgEntityHead entityHead;
    entityHead.uidMaster = m_pMaster->getUID();
    entityHead.byPartID = ENTITY_PART_MAX;

    // 实体要导出的内容
    SEntiyExportShadowContext context;
    context.byType		= m_pMaster->getType();		// 实体类型
    context.byFlag		= TO_CLIENT_OTHER;			// 实体标识
    context.byCurState	= m_pMaster->getState();	// 当前状态
    context.loc			= m_pMaster->getLocation();
	context.fAngleY		= m_pMaster->getAngle().y;

    obuf256 data;
    data << head << entityHead;

    // 实体要导出的内容
    data.push_back(&context, sizeof(context));

    m_pMaster->deSerialize(data,context.byFlag);

	if ( uidPlayer!=INVALID_UID )
	{
        send2Client( uidPlayer, PACKAGE_PTR(new std::string(data.data(),data.size())) );
	}

	if ( camp>=0 )
	{
		scene_broadcast_all( BROADCAST_CREATE,std::string(data.data(),data.size()),CAMP_MASK(camp) );
	}
}

// 当前实体离开某个人视野(销毁实体)
void EntityAOI::onMessage_DestroyNotify(UID uidPlayer,int camp)
{
    SNetMsgHead head;
    head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
    head.byDestEndPoint= MSG_ENDPOINT_CLIENT;	
    head.byKeyModule   = MSG_MODULEID_ENTITY;	
    head.byKeyAction   = SC_MSG_ENTITY_DESTROY;

    // 实体类统一用子头来用于转向部件
    SMsgEntityHead entityHead;
    entityHead.uidMaster = m_pMaster->getUID();
    entityHead.byPartID = ENTITY_PART_MAX;
	
    obuf256 data;
	data << head << entityHead << m_pMaster->getUID();

    if ( uidPlayer!=INVALID_UID )
    {
        send2Client( uidPlayer, PACKAGE_PTR(new std::string(data.data(),data.size())) );
    }

	if ( camp>=0 )
	{
		scene_broadcast_all( BROADCAST_DESTROY,std::string(data.data(),data.size()),CAMP_MASK_ALL);
	}
}

// 是否是强制广播的属性
static bool isForceBroadcastProp(int nProp)
{
    // 可见标志
    return nProp == PROPERTY_VISIBLE_FLAG;
}

void EntityAOI::flushProperty2Client(const std::set<int>& refPropertys, bool isBroadcast/*=true*/)
{
    __IEntityProperty* pProperty = (__IEntityProperty*)m_pMaster->getEntityPart(PART_PROPERTY);
    if (pProperty == NULL)
        return;

    UID uid = m_pMaster->getUID();

    BYTE nEntityType = UID_2_TYPE(uid);
    // 是否机器人
    bool isRobot = false;
    if(nEntityType == TYPE_PLAYER_ROLE)
    {
        int nPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
        isRobot = IsComputerPDBID(nPDBID);
    }

    SNetMsgHead head;
    head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
    head.byDestEndPoint= MSG_ENDPOINT_CLIENT;
    head.byKeyModule   = MSG_MODULEID_ENTITY;
    head.byKeyAction   = SC_MSG_ENTITY_UPDATEPROPS;

    // 实体类统一用子头来用于转向部件
    SMsgEntityHead entityHead;
    entityHead.uidMaster = uid;		// 实体UID
    entityHead.byPartID = PART_AOI;	// 部件ID，ENTITY_PART_MAX时将由实体来处理

    // 更新
    obuf256 send2Self;
    obuf256 send2Other;

    send2Self << head << entityHead;
    send2Other << head << entityHead;

	// 只有人物的血量和等级需要全局广播，其他都是周围范围广播即可,范围广播已经包括了上帝模式的观察点
	bool bBroadcastAll = m_pMaster->getType() == TYPE_PLAYER_ROLE;
    bool bForceBroadcast = false;

    char property_data[MAX_PROPERTY_DATA_LEN];
    for(std::set<int>::const_iterator it = refPropertys.begin(); it != refPropertys.end(); ++it)
    {
        const int & nPropertyID = *it;

        int property_len = pProperty->getProperty_custom( nPropertyID,property_data,MAX_PROPERTY_DATA_LEN );

        msg_entity_updateprop msg;
        msg.nPropertyID = nPropertyID;
        msg.nDataLen = property_len;

        int nPropertyFlag = pProperty->getPropertyFlag(nPropertyID);
        // 机器人就不要发给自己，提高下性能
        if( (nPropertyFlag&TO_CLIENT_SELF) && nEntityType==TYPE_PLAYER_ROLE && !isRobot )
        {            
            do
            {
                // 广播状态下不更新血量属性，非广播给自己，否则很容易覆盖
                if (isBroadcast && nPropertyID == PROPERTY_HP)
                    break;

                send2Self << msg;
                send2Self.push_back(property_data, property_len);
            } while (false);
        }

        // 广播状态下可以进行属性广播
        if (isBroadcast)
        {
            // 只有血量和等级需要全局广播，其他都是周围范围广播即可,范围广播已经包括了上帝模式的观察点
            if (nPropertyID == PROPERTY_LEVEL || nPropertyID == PROPERTY_HP || nPropertyID == PROPERTY_MAX_HP)
            {
                bBroadcastAll = true;
            }

            if (isForceBroadcastProp(nPropertyID))
            {
                bForceBroadcast = true;
            }

            if (nPropertyFlag&TO_CLIENT_OTHER)
            {
                send2Other << msg;
                send2Other.push_back(property_data, property_len);
            }
        }
    }

    // 发消息给玩家(非机器人)
    if( !isRobot && send2Self.size() > sizeof(head) + sizeof(entityHead) )
    {
        ((__IPlayerRole*)m_pMaster)->send2Client(send2Self.data(), send2Self.size());
    }

    // 广播
    if (send2Other.size() > sizeof(head) + sizeof(entityHead))
    {
        int nSceneID = m_pMaster->getSceneID();
        SceneServiceHelper helper(nSceneID);
        IGameSceneService* pScene = helper.m_ptr;
        if ( pScene==0 )
        {
            ErrorLn("EntityAOI::onEvent_PropertyChange broadcast failed for not found the scene!id=" << nSceneID <<", name="<< m_pMaster->getName());
            return ;
        }

        Vector3 curLoc = m_pMaster->getLocation();
        if (bForceBroadcast)
        {
            pScene->broadcast(curLoc, 0, BROADCAST_CHANGE_PROPERTY, string(send2Other.data(), send2Other.size()), m_pAOIProxy, true);
        }
        else
        {
            if (bBroadcastAll)
            {
                // 怪物属性变化不广播给同阵营英雄
                if (nEntityType == TYPE_MONSTER && (m_pMaster->getIntProperty(PROPERTY_MASK) & MASKFLAG_PROPCHANGE_HIDE_SAME_CAMP))
                {
                    pScene->broadcast_nearLoc(curLoc, 0, BROADCAST_CHANGE_PROPERTY, string(send2Other.data(), send2Other.size()), m_pAOIProxy, false, CAMP_EXCEPT_MASK(m_pAOIProxy->m_nCamp));
                }
                else
                {
                    pScene->broadcast(curLoc, 0, BROADCAST_CHANGE_PROPERTY, string(send2Other.data(), send2Other.size()), m_pAOIProxy, true);
                }
            }
            else
            {
                pScene->broadcast_nearLoc(curLoc, 0, BROADCAST_CHANGE_PROPERTY, string(send2Other.data(), send2Other.size()), m_pAOIProxy, true);
            }
        }
    }


    // OB没有收到金币、体力值、经验值，在此广播给OB阵营
    int nSceneID = m_pMaster->getSceneID();
    SceneServiceHelper helper(nSceneID);
    IGameSceneService* pScene = helper.m_ptr;
    if (pScene->getObCampID() > 0)
    {
        obuf256 send2ob;

        send2ob << head << entityHead;
        for (std::set<int>::const_iterator it = refPropertys.begin(); it != refPropertys.end(); ++it)
        {
            const int & nPropertyID = *it;
            if (nPropertyID == PROPERTY_TALENT_POINT || nPropertyID == PROPERTY_EXP || nPropertyID == PROPERTY_STAMINA)
            {
                int property_len = pProperty->getProperty_custom(nPropertyID, property_data, MAX_PROPERTY_DATA_LEN);

                msg_entity_updateprop msg;
                msg.nPropertyID = nPropertyID;
                msg.nDataLen = property_len;

                send2ob << msg;
                send2ob.push_back(property_data, property_len);
            }
        }

        if (send2ob.size() > sizeof(head) + sizeof(entityHead))
        {
            pScene->broadcast_all(BROADCAST_CHANGE_PROPERTY, string(send2ob.data(), send2ob.size()), CAMP_MASK(pScene->getObCampID()));
        }
    }
}

void EntityAOI::scene_broadcast_all( BROADCAST_ID id,string & msg,int nCampMask )
{
    SceneHelper sceneHelper(m_pMaster->getSceneID());
	IGameSceneService * pScene = sceneHelper.m_ptr;
	if (pScene )
	{
		pScene->broadcast_all( id, msg, nCampMask);
	}
}

// 只对能看到自己的实体广播
int EntityAOI::broadcast(Vector3 loc,int dist,BROADCAST_ID id,std::string & msg, const char * pFileInfo, int nFileLine)
{
    SceneHelper sceneHelper(m_pMaster->getSceneID());
    IGameSceneService * pScene = sceneHelper.m_ptr;
    if (pScene )
    {
        pScene->broadcast(loc, dist, BROADCAST_DO_ACTION,msg, m_pAOIProxy, false);
    }

    return 1;
}

// 加载视野距离配置
void EntityAOI::loadSight()
{
    if( m_pMaster == 0 )
    {
        m_fSightRadii = AOI_DEFAULT_SIGHT;
        return;
    }

    __IEntityProperty* pProperty = (__IEntityProperty*)m_pMaster->getEntityPart(PART_PROPERTY);
    if( pProperty == 0 )
    {
        m_fSightRadii = AOI_DEFAULT_SIGHT;
        return;
    }

    if( m_pMaster->getType() == TYPE_PLAYER_ROLE )
    {
        int nHeroID = pProperty->getProperty_Integer(PROPERTY_VOCATION);
        const PersonSkinModelItem * pScheme = gServerGlobal->getSchemeCenter()->getSchemePersonModel()->getPersonSkinModelItem(nHeroID); 
        if (pScheme )
        {
            int nMapID = m_pMaster->getMapID();
            m_fSightRadii = pScheme->nSightArray[0]; // 默认值
            if(nMapID >= 0 && nMapID < MAP_MAXCOUNT && pScheme->nSightArray[nMapID] != 0)
            {
                m_fSightRadii = pScheme->nSightArray[nMapID];  // 配置值
            }
        }
        else
        {
            WarningLn("actor vocation id=" << nHeroID << ", cant find sight scheme, use default");
            m_fSightRadii = AOI_DEFAULT_SIGHT;
        }

        return;
    }

    if( m_pMaster->getType() == TYPE_MONSTER )
    {
        int nMonsterID = pProperty->getProperty_Integer(PROPERTY_ID);
        SMonsterScheme * pSchemeInfo = gServerGlobal->getSchemeCenter()->getSchemeMonster()->getMonsterShemeInfo(nMonsterID);
        if( pSchemeInfo != NULL )
        {
            m_fSightRadii = pSchemeInfo->fEyeshot;
        }
        else
        {
            WarningLn("monster id=" << nMonsterID << ", cant find sight scheme, use default");
            m_fSightRadii = AOI_DEFAULT_SIGHT;
        }

        return;
    }

    if( m_pMaster->getType() == TYPE_TANK )
    {
        m_fSightRadii = AOI_DEFAULT_SIGHT;
        return;
    }
}

// 草丛事件(进入离开草丛)
void EntityAOI::forestEvent(int nForestID)
{
    if (m_pMaster == NULL)
    {
        return;
    }

    __IEntityEvent *pEntityEvent = m_pMaster->getEntityEvent();
    if (pEntityEvent == NULL)
    {
        return;
    }

    event_entity_forest forest;
    forest.nType = nForestID>=0 ? 1 : 0;
    // 草丛事件
    pEntityEvent->FireExecute(EVENT_ENTITY_FOREST, (LPCSTR)&forest, sizeof(forest));
}

// 获取AOI代理
AOI_PROXY_PTR EntityAOI::getAOIProxy()
{
    return m_pAOIProxy;
}

////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" __declspec(dllexport) __IEntityAOI * CreateEntityAOI()
{
	return new EntityAOI;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
AOI_Proxy * EntityAOI::CreateAOIProxy()
{
	switch( m_pMaster->getType() )
	{
	case TYPE_PLAYER_ROLE:
            return new AOIProxy_PlayerRole((__IPlayerRole*)m_pMaster);
	case TYPE_MONSTER:
		return new AOIProxy_Monster(m_pMaster);
	case TYPE_TANK:
		return new AOIProxy_Tank(m_pMaster);
	default:
		return NULL;/*new AOIProxy_Base(m_pMaster)*/;
	}

	return 0;
}

void EntityAOI::onChangeHero(int nOldHeroID, int nNewHeroID, bool isBefore)
{
	if (m_pAOIProxy == 0)
		return;

	if (isBefore)
	{
		m_pAOIProxy->m_bGhost = false;
		m_pAOIProxy->m_bGhostOld = false;
	}
}

void EntityAOI::restore()
{

}
