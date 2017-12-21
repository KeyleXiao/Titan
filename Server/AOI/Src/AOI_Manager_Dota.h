/*******************************************************************
** 文件名:	AOI_Manager_Dota.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	10/15/2014
** 版  本:	1.0
** 描  述:	

			
********************************************************************/

#pragma once

#include "Broadcast_Package.h"
#include "AOI_Define.h"
#include "GridTree.h"
#include "GridVisible.h"
#include "EntityHelper.h"
#include <set>

/**
	专为Dota类游戏定制的视野管理逻辑(Area Of Interest)
	具体规则：
	1.地图中有N个阵营,每个阵营有x个玩家
	2.某个实体产生行为需要广播时,判断周围有没有某阵营的实体，如果有，则对该阵营的所有玩家广播
	3.客户端管理移出视野
	*/

template< typename OBJECT_TYPE,		/*对象类型*/
	      typename COORD_TYPE,      /*坐标数据类型*/
		  int DIM,					/*几维空间*/
		  int GRID_SIZE,	        /*每个广播单元格的大小*/
  		  int SIGHT,                /*标准视野大小*/
		  typename FOREST_TEST      /*草丛检测系统*/
>
class AOI_Manager_Dota
{
private:
	typedef GridTree< OBJECT_TYPE,int,DIM,GRID_SIZE > AOI_TREE;
	AOI_TREE			 m_AOI_tree;         // 各阵营对象分布

	typedef AOI_Manager_Dota<OBJECT_TYPE,COORD_TYPE,DIM,GRID_SIZE,SIGHT,FOREST_TEST> MY_TYPE;
	typedef GridVisible< MY_TYPE,OBJECT_TYPE,COORD_TYPE,GRID_SIZE > VISIBLE_GRID;

	VISIBLE_GRID         m_visible_grid;				// 视野管理对象

	typedef std::set<OBJECT_TYPE>	PLAYER_SET;
    typedef std::map<int,PLAYER_SET> CAMP_PLAYER_SET;

	CAMP_PLAYER_SET           m_camp_player_set;       // 各阵营玩家列表

    int                  m_nCampCount;                   // 阵营总数
    AOI_Forest*          m_pForest;

	typedef ForestVisible< OBJECT_TYPE>  FOREST_VISIBLE;
	FOREST_VISIBLE       m_forest_visible;				// 草丛视野管理

public:
	AOI_Manager_Dota(int nCampCount) : m_AOI_tree(nCampCount), m_visible_grid(nCampCount), m_forest_visible(nCampCount), m_nCampCount(nCampCount), m_pForest(0)
	{
	}

	virtual ~AOI_Manager_Dota()
	{
	}

	// --------------------------------------------------------------------------------------------------- //
	// 初始化地图大小
	// --------------------------------------------------------------------------------------------------- //
	void initialize( COORD_TYPE origin_loc[DIM],COORD_TYPE grid_num[DIM], AOI_Forest* pForest )
	{
        m_pForest = pForest;
        m_AOI_tree.create(origin_loc,grid_num);

		m_visible_grid.create( origin_loc,grid_num,this );
	}

	// 某个格子中所有对象的可见性发生了变化
	void onUpdateGridVisible( COORD_TYPE loc[DIM],int nNormalVisibleFlag,int nRealVisibleFlag,int camp,bool bUpdateNormalVisible,bool bUpdateRealVisible )
	{
		// 格子中其他阵营的对象都需要更新视野
		OBJECT_TYPE pWatcherArray[MAX_INTEREST_OBJECT];
		int watcher_count = k_nearest(loc,GRID_SIZE/2,pWatcherArray,MAX_INTEREST_OBJECT,~CAMP_MASK(camp));
		
        for ( int i=0;i<watcher_count;++i )
		{
            // 只更新格子realvisible的话  只更新隐形单位的可见性
            if ( !bUpdateNormalVisible&&!pWatcherArray[i]->m_bIsStealing )
                continue;

			onUpdateObjVisible( pWatcherArray[i],nNormalVisibleFlag,nRealVisibleFlag );
		}
	}

    void UpdateObjVisibleInForest( COORD_TYPE loc[DIM],int nForestID,int nSight,int nCampMask )
    {
        if(nForestID < 0)
        {
            return;
        }

        OBJECT_TYPE pWatcherArray[MAX_INTEREST_OBJECT];
        // 当草丛很大时 可能有一点点bug- -
        int count = k_nearest(loc, nSight, pWatcherArray, MAX_INTEREST_OBJECT, nCampMask);
        for(int i = 0; i < count; ++i)
        {
            if(pWatcherArray[i]->m_nForestID == nForestID)
            {
                COORD_TYPE myLoc[DIM]; myLoc[0] = pWatcherArray[i]->m_Loc.x; myLoc[1] = pWatcherArray[i]->m_Loc.z;  
                UpdateObjVisible(myLoc, pWatcherArray[i]);
            }
        }
    }

    // 取得Ob的阵营
    int getObCamp()
    {
        // 多线程不安全
//         if( m_camp_player_set.find(m_nCampCount)!=m_camp_player_set.end() )
//         {
//             return m_nCampCount; 
//         }
// 
//         return -1;

        return m_nCampCount; 
    }

	// 某个对象的可见性发生了变化
	void onUpdateObjVisible( OBJECT_TYPE obj,int nVisibleFlag,int nGridRealVisibleFlag )
	{
        uint16 nOldSightFlag = obj->m_nVisibaleFlag; 
        uint16 & nSightFlag = obj->m_nVisibaleFlag;

        int nCampCount = (m_camp_player_set.find(m_nCampCount)!=m_camp_player_set.end()) ? (m_nCampCount+1) : m_nCampCount;    
		for ( int camp=0;camp<nCampCount;++camp )
		{
			int camp_mask   = CAMP_MASK(camp);
			bool bOldCanSee = nSightFlag & camp_mask;
			bool bCurCanSee = (nVisibleFlag|obj->m_nKeepInsightFlag|CAMP_MASK(m_nCampCount)) & camp_mask;

            // 观察者对所有阵营不可见
            if ( obj->m_bObserver )
                bCurCanSee = false;

			// 阵营规则可见后，再判断某个对象是否是隐身状态
			if ( bCurCanSee )
			{
				if ( isObjHide( obj,camp,nGridRealVisibleFlag ) )
					bCurCanSee = false;
			}

			// 通知进出视野
			if ( bOldCanSee!=bCurCanSee )
			{
				nSightFlag ^= camp_mask;
				obj->onInOutSight( camp,bCurCanSee );
			}
        }

        // 可见性标志变了 属性同步给其他客户端(观察者)
        if ( nOldSightFlag!=nSightFlag )
        {
            setProperty_Integer( obj->m_uid,PROPERTY_VISIBLE_FLAG,nSightFlag,MSG_FLAG_NO_BLOCK );
        }
	}

	// 更新某个对象的可见性
	void UpdateObjVisible( COORD_TYPE loc[DIM],OBJECT_TYPE obj )
	{
		onUpdateObjVisible( obj,m_visible_grid.getVisibleFlag(loc),m_visible_grid.getRealVisibleFlag(loc) );
	}

    // 某个对象的反隐标志发生了变化
    void onUpdateObjAntiStealth( OBJECT_TYPE obj )
    {
        COORD_TYPE myLoc[DIM];
        myLoc[0] = obj->m_Loc.x; myLoc[1] = obj->m_Loc.z;
        if( obj->m_bIsAntiStealth )
        {
            m_visible_grid.insertObject( obj,myLoc,obj->m_nCamp,MASK_OBJ_REAL_SIGHT );
        }
        else
        {
            m_visible_grid.removeObject( obj,myLoc,obj->m_nCamp,MASK_OBJ_REAL_SIGHT );
        }
    }

    // 设置某个对象的视野
    void SetObjSight( OBJECT_TYPE obj, int nSight )
    {
        int nOriSightInGrid = obj->m_nSightInGrid;

        obj->m_nSight = nSight;
        obj->m_nSightInGrid = nSight>0 ? nSight/AOI_GRID_SIZE : -1;

        m_visible_grid.onObjSightChange(obj, nOriSightInGrid, obj->m_nSightInGrid);
    }

    // 更新视野里的所有实体给客户端
    void updateObjsInSight( OBJECT_TYPE obj )
    {
        OBJECT_TYPE pObjectArray[MAX_INTEREST_OBJECT * 4];
        int count = m_AOI_tree.getAllObject(pObjectArray,MAX_INTEREST_OBJECT * 4,CAMP_MASK_ALL);
        for ( int i=0;i<count;++i )
        {
            // 就不要发给自己了
            if ( pObjectArray[i]!=obj )
            {
                pObjectArray[i]->onCreateNotify( obj->m_uid,-1 );

                int nCamp = obj->m_nCamp;
                if ( obj->m_bObserver )
                {
                    nCamp = m_nCampCount; 
                }

                if( pObjectArray[i]->m_nVisibaleFlag&CAMP_MASK(nCamp) )
                {
                    // 给obj发进视野
                    pObjectArray[i]->onInOutSight( -1,true,obj->m_uid );
                }
            }
        }
    }

	// --------------------------------------------------------------------------------------------------- //
	// 某个点是否在某个阵营的视野中
	// --------------------------------------------------------------------------------------------------- //
	bool isInSight( COORD_TYPE loc[DIM],int campFlag, int forestID )
	{
		if ( !m_visible_grid.isInSight( loc,campFlag ) )
			return false;

		if ( forestID>=0 )
		{
			// 判断某阵营能不能看到这个草丛
			if ( !m_forest_visible.isVisible( forestID,campFlag ) )
				return false;
		}

		return true;
	}

	// 对象是否在视野中
	bool isObjInSight( COORD_TYPE loc[DIM],int campFlag,OBJECT_TYPE obj )
	{
		if ( obj==0 )
		{
            // 所有点都在GM视野内
            if ( campFlag==m_nCampCount )
                return true;

            int forestID = m_pForest==0 ? -1 : m_pForest->GetForestID(loc[0], loc[1]);

			return isInSight( loc,campFlag,forestID );
		}

        return obj->m_nVisibaleFlag&CAMP_MASK(campFlag);
	}

	// 判断某个对象相对某个阵营是否处于隐身状态
    bool isObjHide( OBJECT_TYPE obj,int campFlag,int nGridRealVisibleFlag )
    {
		// 这个状态下任何人都看不到，包括自己
		if ( obj->m_bGhost )
			return true;

        // 对GM阵营不隐身
        if ( campFlag==m_nCampCount )
            return false;

		// 同一个阵营除了在GHOST状态，其他都可见
		if ( obj->m_nCamp==campFlag )
			return false;

		// 对某个阵营暴露视野
		if ( obj->m_nKeepInsightFlag & CAMP_MASK(campFlag) )
		{
			return false;
		}

		// 对某个阵营暴露视野(临时)
        if ( getTickCountEx()<obj->m_ExposedEndTick[campFlag] )
			return false;

		// 飞行状态不隐身
		if ( obj->m_bFlying )
			return false;

        // 在草丛中
        if ( obj->m_nForestID>=0 )
        {
            if ( !m_forest_visible.isVisible( obj->m_nForestID,campFlag ) )
                return true;
        }

		// 是不是隐形单位(隐形即使周围有反隐 可能也看不到 因为可能在草丛,所以先判断草丛可见性)
        // 没被反隐 则不可见
		if ( obj->m_bIsStealing )
            return !( nGridRealVisibleFlag&CAMP_MASK(campFlag) ); 
		
		return false;
    }

    // --------------------------------------------------------------------------------------------------- //
    // 设置观察者
    // @param obj : 对象指针或句柄
    // @param loc : 对象位置
    // @param campFlag : 对象阵营
    // @param isPlayer : 是否是玩家对象
    // --------------------------------------------------------------------------------------------------- //
    bool setObserver( OBJECT_TYPE obj,COORD_TYPE loc[DIM],bool isPlayer,bool bObserver )
    {
        if ( obj->m_bObserver==bObserver )
            return false;

        removeEntity( obj,loc,isPlayer );

        obj->m_bObserver = bObserver;

        insertEntity( obj,loc,isPlayer );

        return true;
    }

	// --------------------------------------------------------------------------------------------------- //
	// 插入实体
	// @param obj : 对象指针或句柄
	// @param loc : 对象位置
	// @param campFlag : 对象阵营
	// @param isPlayer : 是否是玩家对象
	// --------------------------------------------------------------------------------------------------- //
	bool insertEntity( OBJECT_TYPE obj,COORD_TYPE loc[DIM],bool isPlayer )
	{
        // 观察者
        if ( obj->m_bObserver )
        {
            // 存到最后一个阵营里
            m_camp_player_set[m_nCampCount].insert(obj);

            // 更新自己的可见性
            UpdateObjVisible( loc,obj );
          
            // 看到战场内所有实体
            OBJECT_TYPE pObjectArray[MAX_INTEREST_OBJECT * 4];
            int count = m_AOI_tree.getAllObject(pObjectArray,MAX_INTEREST_OBJECT * 4,CAMP_MASK_ALL);
            for ( int i=0;i<count;++i )
            {
                pObjectArray[i]->onCreateNotify( obj->m_uid,-1 );
                pObjectArray[i]->onInOutSight( -1,true,obj->m_uid );
            }

            return true;
        }

		int campFlag = obj->m_nCamp;
		if ( campFlag<0 || campFlag>=m_nCampCount )
			return false;
		
#ifdef OPEN_BVTTEST
		PP_BY_NAME( "AOI_Manager_Dota::insertEntity" );
#endif

		// 加入到阵营列表
		if ( isPlayer )
		{
			m_camp_player_set[campFlag].insert( obj );
		}

		// 插入到地图
		if ( m_AOI_tree.insertObject(obj,loc,campFlag) )
		{
            int obj_mask = MASK_OBJ_NORMAL_SIGHT;
            if ( obj->m_bIsAntiStealth ) obj_mask = MASK_OBJ_SIGHT_ALL; 
			m_visible_grid.insertObject( obj,loc,campFlag,obj_mask );
		}

		// 广播创建
        for ( CAMP_PLAYER_SET::iterator it=m_camp_player_set.begin();it!=m_camp_player_set.end();++it )
        {
            obj->onCreateNotify( /*INVALID_UID*/0xFFFFFFFF,it->first );
        }

        // 如果正好是草丛
        int nForestID = m_pForest==0 ? -1 : m_pForest->GetForestID(loc[0],loc[1]);
        if(nForestID >= 0)
        {
            bool bOldVisible = m_forest_visible.isVisible(nForestID, obj->m_nCamp);

            m_forest_visible.OnEnterForest( obj,nForestID );

            // 更新自己可见性
            UpdateObjVisible( loc,obj );

            // 更新草丛中其他阵营实体可见性
            if( !bOldVisible )
            {
                UpdateObjVisibleInForest( loc,nForestID,obj->m_nSight*2,~CAMP_MASK(obj->m_nCamp) );
            }

            // 通知EntityAOI模块广播(客户端需要做透明处理.)
            obj->onInOutForest(obj->m_nCamp, nForestID);
        }
        else
        {
            // 更新自己的可见性
            UpdateObjVisible(loc, obj);
        }

		if ( isPlayer )
		{
			// 把当前服务器的其他人的信息发给自己
            updateObjsInSight( obj );
		}

		return true;
	}

	// --------------------------------------------------------------------------------------------------- //
	// 移除实体
	// @param obj : 对象指针或句柄
	// @param loc : 对象位置
	// @param campFlag : 对象阵营
	// @param isPlayer : 是否是玩家对象
	// --------------------------------------------------------------------------------------------------- //
	bool removeEntity( OBJECT_TYPE obj,COORD_TYPE loc[DIM],bool isPlayer )
	{
        if ( obj->m_bObserver )
        {
            CAMP_PLAYER_SET::iterator it = m_camp_player_set.find( m_nCampCount );
            if ( it!=m_camp_player_set.end() )
            {
                it->second.erase( obj );
                if ( it->second.empty() )
                {
                    m_camp_player_set.erase( it );
                }
            }

            return true;
        }

		int campFlag = obj->m_nCamp;
		if ( campFlag<0 || campFlag>=m_nCampCount )
			return false;

#ifdef OPEN_BVTTEST
		PP_BY_NAME( "AOI_Manager_Dota::removeEntity" );
#endif

		// 向周围人广播消失
		//for ( int cmp=0;cmp<m_nBroadcastCampCount;++cmp )
		//{
		//	obj->onDestroyNotify( /*INVALID_UID*/0xFFFFFFFF,cmp );
		//}

		// 从地图上移除
		if ( m_AOI_tree.removeObject(obj,loc,campFlag) )
		{
            int obj_mask = MASK_OBJ_NORMAL_SIGHT;
            if ( obj->m_bIsAntiStealth ) obj_mask = MASK_OBJ_SIGHT_ALL; 
			m_visible_grid.removeObject( obj,loc,campFlag,obj_mask );
		}

		if ( isPlayer )
		{
            CAMP_PLAYER_SET::iterator it = m_camp_player_set.find( campFlag );
            if ( it!= m_camp_player_set.end() )
            {
                it->second.erase( obj );
                if (it->second.empty() )
                {
                    m_camp_player_set.erase( it );
                }
            }
		}

        // 如果当前在草丛中 更新草丛中其他阵营实体的可见性
        int nForestID = obj->m_nForestID; 
        if(nForestID >= 0)
        {
            m_forest_visible.OnLeaveForest( obj );

            bool bCurVisible = m_forest_visible.isVisible(nForestID, obj->m_nCamp); 

            // 如果因为自己离开 导致草丛对本阵营不可见 更新草丛中其他阵营实体可见性
            if(!bCurVisible)
            {
                UpdateObjVisibleInForest( loc,nForestID,obj->m_nSight*2,~CAMP_MASK(obj->m_nCamp) );
            }
        }

		return true;
	}

	// --------------------------------------------------------------------------------------------------- //
	// 移动实体
	// --------------------------------------------------------------------------------------------------- //
	bool moveEntity( OBJECT_TYPE obj,COORD_TYPE old_loc[DIM],COORD_TYPE new_loc[DIM] )
	{
#ifdef OPEN_BVTTEST
		PP_BY_NAME( "AOI_Manager_Dota::moveEntity" );
#endif
        // 观察者
        if ( obj->m_bObserver )
        {
            return true;
        }

		int campFlag = obj->m_nCamp;
		if ( campFlag<0 || campFlag>=m_nCampCount )
			return false;

		// 移动在地图容器中的位置
		if ( m_AOI_tree.moveObject(obj,old_loc,new_loc,campFlag) ==false )
			return false;

		m_visible_grid.moveObject(obj,old_loc,new_loc,campFlag );

       // 更新草丛ID
        int nForestID = (m_pForest==0 ? -1 : m_pForest->GetForestID(new_loc[0],new_loc[1]));
        if( obj->m_nForestID != nForestID )
        {
            // 草丛变了，取消暴露(这个逻辑感觉有点问题，暂不实现)
            //ZeroMemory(proxy->m_ExposedEndTick, sizeof(proxy->m_ExposedEndTick));

            int nOldForest = obj->m_nForestID;
            bool bForestVisible = nForestID>=0 ? m_forest_visible.isVisible( nForestID,obj->m_nCamp ) : false; 

            m_forest_visible.OnEnterForest( obj,nForestID );

            // 草丛变化刷新自己可见性
            UpdateObjVisible(new_loc, obj);

            // 旧的草丛已经对本阵营不可见 更新旧草丛中其他阵营实体可见性
            if( nOldForest>=0 && !m_forest_visible.isVisible(nOldForest, obj->m_nCamp) )
            {
                UpdateObjVisibleInForest( old_loc,nOldForest,obj->m_nSight*2 ,~CAMP_MASK(obj->m_nCamp));
            }

            // 新的草丛原来不可见 更新新草丛中其他阵营实体的可见性
            if( nForestID>=0 && !bForestVisible)
            {
                UpdateObjVisibleInForest( new_loc,nForestID,obj->m_nSight*2,~CAMP_MASK(obj->m_nCamp));
            }

            // 通知EntityAOI模块广播(客户端需要做透明处理.)
            obj->onInOutForest(obj->m_nCamp, nForestID);
        }

		return true;
	}

	// --------------------------------------------------------------------------------------------------- //
	// 返回所有玩家
	// --------------------------------------------------------------------------------------------------- //
	int getAllPlayers( OBJECT_TYPE* pReturnArray,int nArraySize,int campFlag )
	{
		if ( campFlag<0 || campFlag>=m_nCampCount )
			return false;

		int count = 0;
		PLAYER_SET::iterator it = m_camp_player_set[campFlag].begin();
		for ( ;it!=m_camp_player_set[campFlag].end() && count<nArraySize;++it )
		{
			pReturnArray[count++] = *it;
		}

		return count;
	}

	int getPlayerCount( int campFlag )
	{
		if ( campFlag<0 || campFlag>=m_nCampCount )
			return false;

		return m_camp_player_set[campFlag].size();
	}

	// --------------------------------------------------------------------------------------------------- //
	// 取某点周围一定范围内的格子内的所有实体 
	// @param dist : 距离半径(注意，这里不是精确匹配的，是把一个单元格内的所有对象都返回,误差为单元格大小)
	// --------------------------------------------------------------------------------------------------- //
	int k_nearest( COORD_TYPE loc[DIM],int dist,OBJECT_TYPE* pReturnArray,int nArraySize,int campMask=CAMP_MASK_ALL)
	{
#ifdef OPEN_BVTTEST
		PP_BY_NAME( "AOI_Manager_Dota::k_nearest" );
#endif
		return m_AOI_tree.k_nearest(loc,dist,pReturnArray,nArraySize,campMask);
	}
	
	// --------------------------------------------------------------------------------------------------- //
	// 阵营广播
	// --------------------------------------------------------------------------------------------------- //
	void broadcast_camp( int campFlag,BROADCAST_ID id,OBJECT_TYPE obj,PACKAGE_PTR package,bool ignoreSelf=false )
	{
#ifdef OPEN_BVTTEST
		PP_BY_NAME( "AOI_Manager_Dota::broadcast_camp" );
#endif
        CAMP_PLAYER_SET::iterator it_player_set = m_camp_player_set.find(campFlag);
        if ( it_player_set==m_camp_player_set.end() )
        {
            return;
        }

		PLAYER_SET::iterator it = it_player_set->second.begin();
		for ( ;it!=it_player_set->second.end();++it )
		{
            if( obj!=0 && ignoreSelf && obj->m_uid==(*it)->m_uid)
            {
                continue;
            }
			(*it)->onBroadcast( id,obj,package );
		}
	}

	// 向看得见的阵营广播
	void broadcast( BROADCAST_ID id,COORD_TYPE loc[DIM],OBJECT_TYPE obj,PACKAGE_PTR package,bool ignoreSelf)
	{
        for ( CAMP_PLAYER_SET::iterator it=m_camp_player_set.begin();it!=m_camp_player_set.end();++it )
		{
            if ( isObjInSight(loc,it->first,obj) )
            {
                broadcast_camp( it->first,id,obj,package,ignoreSelf );
            }
		}
	}

	// 向看得见的阵营广播 (用多点判断）
	// 使用情形： 释放飞行类技能时，起始点和结束点周围的人都应该看得见，所以需要用2个点进行判断
	void broadcastMultiple( BROADCAST_ID id, int myLoc[AOI_LOC_COUNT][AOI_GRID_DIM],int count,OBJECT_TYPE obj,PACKAGE_PTR package )
	{
        for ( CAMP_PLAYER_SET::iterator it=m_camp_player_set.begin();it!=m_camp_player_set.end();++it )
		{
			bool bSight = false;
			for (int i=0; i<count&&i<AOI_LOC_COUNT; ++i)
			{
				//if ( isObjInSight(myLoc[i],cmp,obj ))
                // obj为0才是判断点是否可见
                if ( isObjInSight(myLoc[i],it->first,0 ))
				{
					bSight = true;
					break;
				}
			}
			
			if (bSight)
			{
				broadcast_camp( it->first,id,obj,package, false );
			}
		}
	}

	// 向所有人广播，不管视野
	void broadcast_all( BROADCAST_ID id,OBJECT_TYPE obj,PACKAGE_PTR package,int campMask=CAMP_MASK_ALL )
	{
#ifdef OPEN_BVTTEST
		PP_BY_NAME( "AOI_Manager_Dota::broadcast_all" );
#endif
        for ( CAMP_PLAYER_SET::iterator it=m_camp_player_set.begin();it!=m_camp_player_set.end();++it )
		{
			if ( CAMP_MASK(it->first) & campMask )
			{
				broadcast_camp( it->first,id,obj,package, false );
			}
		}
	}


    // --------------------------------------------------------------------------------------------------- //
    // 阵营广播(在点附近才广播,用于怪物使用技能等广播)
    // --------------------------------------------------------------------------------------------------- //
    void broadcast_camp_nearLoc( int campFlag,BROADCAST_ID id,vector<Vector3> &vLoc,OBJECT_TYPE obj,PACKAGE_PTR package,bool ignoreSelf=false )
    {
#ifdef OPEN_BVTTEST
        PP_BY_NAME( "AOI_Manager_Dota::broadcast_camp_nearLoc" );
#endif
        CAMP_PLAYER_SET::iterator it_player_set = m_camp_player_set.find(campFlag);
        if( it_player_set==m_camp_player_set.end() )
        {
            return;
        }

        PLAYER_SET::iterator it = it_player_set->second.begin();
        for ( ;it!=it_player_set->second.end();++it )
        {
            if( obj!=0 && ignoreSelf && obj->m_uid==(*it)->m_uid)
            {
                continue;
            }

            // 看实体是不是在点附近,不在的不广播了
            bool bBroadcast = false;
            int nCount = vLoc.size();
            for (int i=0; i<nCount; ++i)
            {
                if ((*it)->m_Loc.checkDistance(vLoc[i], (*it)->m_nSight))
                {
                    bBroadcast = true;
                    break;
                }

                // 角色上帝视角时,在角色观察点的要广播
                if ((*it)->m_LookLoc != INVALID_LOC)
                {
                    if ((*it)->m_LookLoc.checkDistance(vLoc[i], (*it)->m_nSight))
                    {
                        bBroadcast = true;
                        break;
                    }
                }
            }
            
            if (bBroadcast)
            {
                (*it)->onBroadcast( id,obj,package );
            }
        }
    }

    // 向看得见的人广播(在点附近才广播,用于怪物使用技能等广播)
    void broadcast_nearLoc( BROADCAST_ID id,COORD_TYPE loc[DIM],vector<Vector3> &vLoc,OBJECT_TYPE obj,PACKAGE_PTR package,bool ignoreSelf, int campMask=CAMP_MASK_ALL  )
    {
        for ( CAMP_PLAYER_SET::iterator it=m_camp_player_set.begin();it!=m_camp_player_set.end();++it )
        {
            if ( (CAMP_MASK(it->first) & campMask) && isObjInSight(loc,it->first,obj ) )
            {
                broadcast_camp_nearLoc( it->first,id,vLoc,obj,package,ignoreSelf );
            }
        }
    }

    // 向看得见的多个点广播(在点附近才广播,用于怪物使用技能等广播)
    void broadcastMultiple_nearLoc( BROADCAST_ID id, COORD_TYPE myLoc[][DIM],int count,vector<Vector3> &vLoc,OBJECT_TYPE obj,PACKAGE_PTR package, int campMask=CAMP_MASK_ALL  )
    {
        for ( CAMP_PLAYER_SET::iterator it=m_camp_player_set.begin();it!=m_camp_player_set.end();++it )
        {
            if ( !(CAMP_MASK(it->first) & campMask) )
            {
                continue;
            }

            bool bSight = false;
            for (int i=0; i<count; ++i)
            {
                if ( isObjInSight(myLoc[i],it->first,obj ))
                {
                    bSight = true;
                    break;
                }
            }

            if (bSight)
            {
                broadcast_camp_nearLoc( it->first,id,vLoc,obj,package );
            }
        }
    }
};


