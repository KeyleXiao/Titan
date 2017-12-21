/*******************************************************************
** 文件名:	AOIProxy_Base.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	12/24/2014
** 版  本:	1.0
** 描  述:	

			怪物/NPC的AOI Proxy
********************************************************************/

#pragma once

#include "NetMessageDef.h"
#include "EntityHelper.h"
#include "IGameSceneService.h"
#include "IPlayerRole.h"
#include "buffer.h"
#include "Simple_Atom_Lock.h"
#include "IAIService.h"

/**
   视野代理对象： 这个主要是给AOI线程使用的，注意线程安全
   怪物的视野对象，主要是给AI模块用的
*/
class AOIProxy_Base : public AOI_Proxy 
{
public:
	AOIProxy_Base( __IEntity * pHost)
	{
		m_uid  = pHost->getUID();
		m_Loc  = pHost->getLocation();
        ZeroMemory(m_ExposedEndTick, sizeof(m_ExposedEndTick));
        m_bIsStealing = false;
        m_bIsAntiStealth = false;
        m_nForestID = -1;
		m_nCamp= getPropertyPart(pHost)->getProperty_Integer(PROPERTY_CAMP);
        m_nSight = AOI_DEFAULT_SIGHT; 
        m_nSightInGrid = m_nSight/AOI_GRID_SIZE;
		m_nVisibaleFlag  = 0;
        m_nKeepInsightFlag = 0;
        m_nState = ENTITY_STATE_STAND;
        m_bFlying = false;
        m_bGhostOld = false;
        m_bGhost = false;
        m_dwCurStateStartTick = 0;
        m_LookLoc = INVALID_LOC;
        m_uidPKTarget = INVALID_UID;
		m_pSharePtr = 0;
		m_MutexBlock= 0;

        __IEntityProperty* pPropertyPart = getPropertyPart(pHost);
        if(pPropertyPart != NULL)
        {
            m_nVocation = pPropertyPart->getProperty_Integer(PROPERTY_VOCATION);
            m_nCurHp = pPropertyPart->getProperty_Integer(PROPERTY_HP); 
            m_nMaxHp = pPropertyPart->getProperty_Integer(PROPERTY_MAX_HP);
            m_nCurMp = pPropertyPart->getProperty_Integer(PROPERTY_MP);
            m_nMaxMp = pPropertyPart->getProperty_Integer(PROPERTY_MAX_MP);
            m_nCurStamina = pPropertyPart->getProperty_Integer(PROPERTY_STAMINA);
            m_nLevel = pPropertyPart->getProperty_Integer(PROPERTY_LEVEL);
            m_bCanSelected = !(pPropertyPart->getProperty_Integer(PROPERTY_MASK) & MASKFLAG_NOT_SELECT);
            m_bObserver = (pPropertyPart->getProperty_Integer(PROPERTY_OBSERVER) != 0);

			if (UID_2_TYPE(m_uid) == TYPE_PLAYER_ROLE)
			{
				m_nEcm = pPropertyPart->getProperty_Integer(PROPERTY_TALENT_POINT);
				m_nVocationType = gServerGlobal->getSchemeCenter()->getSchemePersonModel()->GetHeroType(m_nVocation);
			}
        }
	}

	// 收到广播请求(注意：这个是在广播线程访问的）
	virtual void onBroadcast( BROADCAST_ID id,AOI_Proxy * pSrc,PACKAGE_PTR package ) 
	{
		// 怪物不需要知道广播消息
	}

	// 收到进出视野事件
	virtual void onInOutSight( int camp,bool bInSight,UID uidTarget )
	{
		// 消息头
		SNetMsgHead head;
		head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
		head.byDestEndPoint= MSG_ENDPOINT_SCENE;	
		head.byKeyModule   = MSG_MODULEID_ENTITY;	
		head.byKeyAction   = bInSight ? SS_MSG_ENTITY_IN_SIGHT : SS_MSG_ENTITY_OUT_OF_SIGHT;

		// 实体类统一用子头来用于转向部件
		SMsgEntityHead entityHead;
		entityHead.uidMaster = m_uid;			// 实体UID
		entityHead.byPartID = PART_AOI;

		obuf256 data;
		data << head << entityHead << camp << uidTarget;

		handleMessage(m_uid,data.data(),data.size(), MSG_FLAG_NO_BLOCK);
	}

    // 收到进出草丛事件(客户端透明处理)
    virtual void onInOutForest( int camp, int nForestID )
    {
        // 消息头
        SNetMsgHead head;
        head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
        head.byDestEndPoint= MSG_ENDPOINT_SCENE;	
        head.byKeyModule   = MSG_MODULEID_ENTITY;	
        head.byKeyAction   = SS_MSG_ENTITY_FOREST_CHANGE;

        // 实体类统一用子头来用于转向部件
        SMsgEntityHead entityHead;
        entityHead.uidMaster = m_uid;			// 实体UID
        entityHead.byPartID = PART_AOI;

        obuf256 data;
        data << head << entityHead << camp << nForestID;

        handleMessage(m_uid,data.data(),data.size(), MSG_FLAG_NO_BLOCK);
    }

	// 向某个玩家或者某个阵营通知自己创建的消息
	virtual void onCreateNotify( UID uidAcceptor,int nAcceptCamp )
	{
        if(nAcceptCamp < 0 && UID_2_TYPE(uidAcceptor) != TYPE_PLAYER_ROLE)
        {
            return;
        }
		// 消息头
		SNetMsgHead head;
		head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
		head.byDestEndPoint= MSG_ENDPOINT_SCENE;	
		head.byKeyModule   = MSG_MODULEID_ENTITY;	
		head.byKeyAction   = SS_MSG_ENTITY_CREATE_NOTIFY;

		// 实体类统一用子头来用于转向部件
		SMsgEntityHead entityHead;
		entityHead.uidMaster = m_uid;			// 实体UID
		entityHead.byPartID = PART_AOI;

		obuf256 data;
		data << head << entityHead << uidAcceptor << nAcceptCamp;

		handleMessage(m_uid,data.data(),data.size(),MSG_FLAG_NO_BLOCK);
	}

	// 向某个玩家或者某个阵营通知自己消失的消息
	virtual void onDestroyNotify( UID uidAcceptor,int nAcceptCamp )
	{
        if(nAcceptCamp < 0 && UID_2_TYPE(uidAcceptor) != TYPE_PLAYER_ROLE)
        {
            return;
        }

		// 消息头
		SNetMsgHead head;
		head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
		head.byDestEndPoint= MSG_ENDPOINT_SCENE;	
		head.byKeyModule   = MSG_MODULEID_ENTITY;	
		head.byKeyAction   = SS_MSG_ENTITY_DESTROY_NOTIFY;

		// 实体类统一用子头来用于转向部件
		SMsgEntityHead entityHead;
		entityHead.uidMaster = m_uid;			// 实体UID
		entityHead.byPartID = PART_AOI;

		obuf256 data;
		data << head << entityHead << uidAcceptor << nAcceptCamp;

		handleMessage(m_uid,data.data(),data.size(),MSG_FLAG_NO_BLOCK);
	}
};
