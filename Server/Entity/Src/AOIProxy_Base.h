/*******************************************************************
** �ļ���:	AOIProxy_Base.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	���� (Carl Chen)
** ��  ��:	12/24/2014
** ��  ��:	1.0
** ��  ��:	

			����/NPC��AOI Proxy
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
   ��Ұ������� �����Ҫ�Ǹ�AOI�߳�ʹ�õģ�ע���̰߳�ȫ
   �������Ұ������Ҫ�Ǹ�AIģ���õ�
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

	// �յ��㲥����(ע�⣺������ڹ㲥�̷߳��ʵģ�
	virtual void onBroadcast( BROADCAST_ID id,AOI_Proxy * pSrc,PACKAGE_PTR package ) 
	{
		// ���ﲻ��Ҫ֪���㲥��Ϣ
	}

	// �յ�������Ұ�¼�
	virtual void onInOutSight( int camp,bool bInSight,UID uidTarget )
	{
		// ��Ϣͷ
		SNetMsgHead head;
		head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
		head.byDestEndPoint= MSG_ENDPOINT_SCENE;	
		head.byKeyModule   = MSG_MODULEID_ENTITY;	
		head.byKeyAction   = bInSight ? SS_MSG_ENTITY_IN_SIGHT : SS_MSG_ENTITY_OUT_OF_SIGHT;

		// ʵ����ͳһ����ͷ������ת�򲿼�
		SMsgEntityHead entityHead;
		entityHead.uidMaster = m_uid;			// ʵ��UID
		entityHead.byPartID = PART_AOI;

		obuf256 data;
		data << head << entityHead << camp << uidTarget;

		handleMessage(m_uid,data.data(),data.size(), MSG_FLAG_NO_BLOCK);
	}

    // �յ������ݴ��¼�(�ͻ���͸������)
    virtual void onInOutForest( int camp, int nForestID )
    {
        // ��Ϣͷ
        SNetMsgHead head;
        head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
        head.byDestEndPoint= MSG_ENDPOINT_SCENE;	
        head.byKeyModule   = MSG_MODULEID_ENTITY;	
        head.byKeyAction   = SS_MSG_ENTITY_FOREST_CHANGE;

        // ʵ����ͳһ����ͷ������ת�򲿼�
        SMsgEntityHead entityHead;
        entityHead.uidMaster = m_uid;			// ʵ��UID
        entityHead.byPartID = PART_AOI;

        obuf256 data;
        data << head << entityHead << camp << nForestID;

        handleMessage(m_uid,data.data(),data.size(), MSG_FLAG_NO_BLOCK);
    }

	// ��ĳ����һ���ĳ����Ӫ֪ͨ�Լ���������Ϣ
	virtual void onCreateNotify( UID uidAcceptor,int nAcceptCamp )
	{
        if(nAcceptCamp < 0 && UID_2_TYPE(uidAcceptor) != TYPE_PLAYER_ROLE)
        {
            return;
        }
		// ��Ϣͷ
		SNetMsgHead head;
		head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
		head.byDestEndPoint= MSG_ENDPOINT_SCENE;	
		head.byKeyModule   = MSG_MODULEID_ENTITY;	
		head.byKeyAction   = SS_MSG_ENTITY_CREATE_NOTIFY;

		// ʵ����ͳһ����ͷ������ת�򲿼�
		SMsgEntityHead entityHead;
		entityHead.uidMaster = m_uid;			// ʵ��UID
		entityHead.byPartID = PART_AOI;

		obuf256 data;
		data << head << entityHead << uidAcceptor << nAcceptCamp;

		handleMessage(m_uid,data.data(),data.size(),MSG_FLAG_NO_BLOCK);
	}

	// ��ĳ����һ���ĳ����Ӫ֪ͨ�Լ���ʧ����Ϣ
	virtual void onDestroyNotify( UID uidAcceptor,int nAcceptCamp )
	{
        if(nAcceptCamp < 0 && UID_2_TYPE(uidAcceptor) != TYPE_PLAYER_ROLE)
        {
            return;
        }

		// ��Ϣͷ
		SNetMsgHead head;
		head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
		head.byDestEndPoint= MSG_ENDPOINT_SCENE;	
		head.byKeyModule   = MSG_MODULEID_ENTITY;	
		head.byKeyAction   = SS_MSG_ENTITY_DESTROY_NOTIFY;

		// ʵ����ͳһ����ͷ������ת�򲿼�
		SMsgEntityHead entityHead;
		entityHead.uidMaster = m_uid;			// ʵ��UID
		entityHead.byPartID = PART_AOI;

		obuf256 data;
		data << head << entityHead << uidAcceptor << nAcceptCamp;

		handleMessage(m_uid,data.data(),data.size(),MSG_FLAG_NO_BLOCK);
	}
};
