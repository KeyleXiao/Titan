/*******************************************************************
** 文件名:	EntityFactory.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			实体工厂
********************************************************************/

#pragma once

#include "IClientEntity.h"
#include "PlayerRole.h"
#include "Monster.h"
#include "Tank.h"
#include "buffer.h"
#include "fast_id_map.h"
#include "EntityFactory.h"
#include <map>
#include "EntityViewDef.h"
#include "IRenderView.h"
#include "Minimap_ManageDef.h"
#include "ISchemeCenter.h"
#include "AOI_Define.h"
#include "IHandleCommand.h"

#include "ConfigIni.h"
#include "PerformanceStat.h"
#include "GameViewDef.h"
#include "EventDef.h"

#define ENTITY_TYPE_MAX   (1<<6)

class EntityFactory : public IEntityFactory
{
    typedef  hash_map<PDBID,IClientEntity*>		    PDBID_MAP;
	typedef  hash_map<UID,IClientEntity*>		    UID_MAP;
	typedef  hash_map<ENTITY_ID,UID>		        VIEW_MAP;

    PDBID_MAP       m_PDBIDMap;         // PDBID索引表
	UID_MAP         m_uidMap;			// UID索引表
	UID_MAP         m_uidPlayerMap;	// UID人物索引表
    UID_MAP         m_uidMonsterMap;	// UID怪物索引表
    UID_MAP         m_uidTankMap;	    // UID载具索引表

	VIEW_MAP     m_viewMap;         // 显示ID索引表

#ifdef PERFORMANCE_CLIENT_TEST
    CPerformanceStat    m_entityCmdStat;
    int                 m_nCmdFlag;
    bool                m_isOpenPropStat;
#endif


private:
    void add_map_entity(UID uid, IClientEntity * p)
    {
        m_uidMap[uid] = p;

        if(isActor(p))
        {
            PDBID PlayerID = p->getIntProperty(PROPERTY_ID);
            if (PlayerID != INVALID_PDBID)
            {
                m_PDBIDMap[PlayerID] = p;
            }
            
            m_uidPlayerMap[uid] = p;
        }
        else if( isMonster(p))
        {
            m_uidMonsterMap[uid] = p;
        }
        else if( isTank(p) )
        {
            m_uidTankMap[uid] = p;
        }
    }

    void remove_map_entity(UID uid, IClientEntity* p)
    {
        
        
        m_uidMap.erase(uid);

        if(isActor(p))
        {
            m_PDBIDMap.erase(p->getIntProperty(PROPERTY_ID));
            m_uidPlayerMap.erase(uid);
        }
        else if( isMonster(p))
        {
            m_uidMonsterMap.erase(uid);
        }
        else if( isTank(p) )
        {
            m_uidTankMap.erase(uid);
        }
    }

public:

#ifdef PERFORMANCE_CLIENT_TEST
    EntityFactory()
    {
        m_entityCmdStat.create("[prop]entityid", ConfigIni::getInstance().isAccumulate(), ConfigIni::getInstance().getCmdInterVal(), ConfigIni::getInstance().getCmdMinVal());
        m_nCmdFlag = ConfigIni::getInstance().getCmdFlag();
        m_isOpenPropStat = ConfigIni::getInstance().isOpenPropStat();
    }
#endif
	// 创建实体
	// @param type   : 实体类型
	// @param context: 创建现场
	// @param len    : 长度
	// @param flag   : 现场序列化标识，比如从数据库现场创建，从服务器发给客户端的现场创建
	virtual IClientEntity * createEntity( UID uid, SEntiyExportShadowContext &shadowData,void * context,int len)
	{
		IClientEntity * pEntity = 0;
		
		BYTE type = shadowData.byType;
		BYTE flag = shadowData.byFlag;
		

		switch(type)
		{
		case TYPE_PLAYER_ROLE:
			pEntity = new PlayerRole();
			break;
		case TYPE_MONSTER:
			pEntity = new Monster();
			break;
		case TYPE_TANK:
			pEntity = new Tank();
			break;
		default:
			break;
		}

		if ( pEntity==0 )
		{
			ErrorLn("entity type is unrecognized!"<<type);
			return 0;
		}

		add_map_entity(uid, pEntity); //m_uidMap[uid] = pEntity;

		if ( !pEntity->create(*(UID*)&uid,shadowData,context,len) )
		{
			destroyEntity(uid);
			ErrorLn("entity create error!");
			return 0;
		}

        bool bMobaMode = (UID_2_SCENE(uid) >= (MAP_MAXCOUNT + 1));
        bool bDefaultEnterView = !bMobaMode;
		if ( flag & TO_CLIENT_SELF )
		{
			gClientGlobal->setHero(pEntity);
            bDefaultEnterView = true;
		}

		if ( !pEntity->createView(flag) )
		{
			destroyEntity(uid);
			ErrorLn("entity createview failed! flag="<<flag << " type=" << type);
			return 0;
		}

        // 显示层更新(创建默认不显示)
        pEntity->updateView( bDefaultEnterView, true );

        if (bDefaultEnterView)
        {
            IClientEntity *pClientEntity = gClientGlobal->getHero();
            UID uidMe = INVALID_UID;
            if(pClientEntity != NULL)
            {
                uidMe = pClientEntity->getUID();
            }
            event_entity_enter_viewport enterViewPort;
            enterViewPort.uidMe = uidMe;
            enterViewPort.uidEnter = uid;
            // 进入视野事件
            pEntity->fireEvent(EVENT_ENTITY_ENTER_VIEWPORT, &enterViewPort, sizeof(enterViewPort));
        }

		m_viewMap[pEntity->getViewID()] = uid;
		return pEntity;
	}

	virtual IClientEntity * getEntity( UID uid)
	{
		UID_MAP::iterator it = m_uidMap.find(uid);
		if ( it!=m_uidMap.end() )
		{
			return it->second;
		}

		return 0;
	}

    // 根据PDBID查找实体
    virtual IClientEntity * getEntityByPDBID( PDBID playerID)
    {
        PDBID_MAP::iterator it = m_PDBIDMap.find(playerID);
        if ( it!=m_PDBIDMap.end() )
        {
            return it->second;
        }

        return 0;
    }

	// 根据显示ID查找实体
	virtual IClientEntity * getEntityByView( ENTITY_ID viewID)
	{
        VIEW_MAP::iterator it = m_viewMap.find(viewID);
        if ( it!=m_viewMap.end() )
        {
            return getEntity(it->second);
        }

		return 0;
	}

	// 删除UID对应的实体
	virtual void destroyEntity( UID uid, EntityDestroyReason eReason = DESTROY_REASON_NORMAL, void* pParam = NULL)
	{
		IClientEntity * pEntity = getEntity(uid);
		if ( pEntity )
		{
            if (pEntity == gClientGlobal->getHero())
            {
                gClientGlobal->setHero(0);
                // 主角释放时强制清空下命令队列
                IHandleCommandManager *pHandleCommandManager = gClientGlobal->getHandleCommandManager();
                if (pHandleCommandManager)
                {
                    pHandleCommandManager->clear();
                }
            }

			if ( pEntity->getViewID()!=INVALID_ENTITY_ID) 
				m_viewMap.erase(pEntity->getViewID());
			
			remove_map_entity(uid, pEntity); //m_uidMap.erase(uid);

			pEntity->destroy(eReason, pParam);
		}
        pEntity = NULL;
	}

    /** 获取当前场景中的所有玩家
    @name				: 
    @param				: 
    @return
    */
    virtual size_t getAllPlayer( IClientEntity* entityArray[], size_t nArraySize )
    {
        if ( nArraySize==0 )
            return 0;

        size_t n = 0;
        for ( UID_MAP::iterator it = m_uidPlayerMap.begin(); it!=m_uidPlayerMap.end(); ++it )
        {
            IClientEntity* pEntity = it->second;
            if ( pEntity )
            {
                entityArray[n] = pEntity;
                if ( ++n==nArraySize )
                    break;
            }
        }

        return n;
    }

    /** 获取指定阵营玩家
    @name				: 
    @param				: 
    @return
    */
    virtual size_t getCampPlayers(IClientEntity* entityArray[], size_t nArraySize, int campMask=CAMP_MASK_ALL)
    {
        if ( nArraySize==0 )
            return 0;

        size_t n = 0;

        for ( UID_MAP::iterator it = m_uidPlayerMap.begin();it!=m_uidPlayerMap.end();++it )
        {
            IClientEntity* pEntity = it->second;
            if ( !pEntity || !(CAMP_MASK(pEntity->getIntProperty(PROPERTY_CAMP) & campMask)) )
                continue;
            
            entityArray[n] = pEntity;

            if ( ++n==nArraySize )
                break;
        }

        return n;
    }

    /** 获取周围实体
    @name				: 
    @param				: 
    @return
    */
    virtual size_t getAroundEntities(UID uidMaster, IClientEntity* entityArray[], size_t nArraySize, int nDistance=32, int campMask=CAMP_MASK_ALL
        , int typeMask=(MASK_CREATURE|MASK_MONSTER|MASK_NPC|MASK_ACTOR|MASK_TANK) )
    {
        if ( uidMaster==INVALID_UID )
            return 0;

        if ( nArraySize==0 )
            return 0;

        IClientEntity* pMaster = getEntity(uidMaster);
        if(!pMaster)
            return 0;

        Vector3 myloc = pMaster->getLocation();

        ENTITY_MASK mask;
        size_t n = 0;
        for ( UID_MAP::iterator it = m_uidMap.begin();it!=m_uidMap.end();++it )
        {
            IClientEntity* pEntity = it->second;

            if ( !pEntity || !(CAMP_MASK(pEntity->getIntProperty(PROPERTY_CAMP) & campMask)) )
                continue;

            mask = pEntity->getTypeMask();
            if( !(mask & typeMask) )
                continue;

            if ( !myloc.checkDistance( pEntity->getLocation(),nDistance ) )
                continue;

            entityArray[n] = pEntity;
            if ( ++n==nArraySize )
                break;            
        }

        return n;
    }


    // 取得实体数量
    virtual DWORD getEntityCount(BYTE entity_type)
    {
        DWORD dwCount = 0;
        for ( UID_MAP::iterator it = m_uidMap.begin();it!=m_uidMap.end();++it )
        {
            IClientEntity* pEntity = it->second;
            if ( pEntity && (pEntity->getTypeMask()&entity_type) )
            {
                ++dwCount;
            }
        }

        return dwCount;
    }

    // 删除所有实体
    virtual void destroyAllEntity()
    {
        for(UID_MAP::iterator it = m_uidMap.begin(); it != m_uidMap.end(); )
        {
            UID uid = it->first;
            ++it;
            destroyEntity(uid);
        }
        m_uidMap.clear();

        m_uidPlayerMap.clear();
        m_uidMonsterMap.clear();
        m_uidTankMap.clear();
        m_viewMap.clear();
    }

    // 执行更新View后操作
    virtual void onUpdateView(IClientEntity * pEntity, int cmdID, int nParam, const char* strParam, void* ptrParam, int len)
    {
#ifdef PERFORMANCE_CLIENT_TEST
        if(!m_isOpenPropStat)
            return;

        if(!pEntity)
            return;

        entity_update_Numproperty * pCmdData = CAST_TYPE(entity_update_Numproperty*, ptrParam);
        if(!pCmdData)
            return;

        bool bCanAdd = false;

        UID uid = pEntity->getUID();
        
        bool bActor = isActor(pEntity);

        // 监控自己角色的命令
        if( bActor && (m_nCmdFlag & PERF_CHECK_ENTITY_CMD_SELF) )
        {
            IClientEntity * pHero = gClientGlobal->getHero();
            bCanAdd = (pHero && pHero->getUID() == uid) ;
        }
        else if( bActor && (m_nCmdFlag & PERF_CHECK_ENTITY_CMD_OTHERHERO) )
        {
            bCanAdd = true;
        }
        else if( (m_nCmdFlag & PERF_CHECK_ENTITY_CMD_MONSTER) && isMonster(pEntity) )
        {
            bCanAdd = true;
        }

        // 增加命令
        if(bCanAdd)
        {        
            m_entityCmdStat.push(uid, pCmdData->nPropID);
        }


#endif
    }


};
