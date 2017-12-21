/*******************************************************************
** 文件名:	AOI_Manager.h
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

/**
	管理游戏中玩家的视野逻辑(Area Of Interest)
	具体规则：
	1.玩家上线，向周围九宫格内的所有玩家广播自己出现的消息(Appear)
	2.同时向该玩家发送周围所以玩家的信息(Nearest)
	3.玩家下线, 向周围九宫格内的所有玩家广播自己离开的消息(Disappear)
	4.玩家移动, 计算玩家之前所在位置的九宫格列表，再计算玩家新位置的九宫格列表
	  a).对于其中新增的单元格，广播玩家出现的消息(Appear),并把该单元格中的所有玩家列表发给此玩家(Nearest)
	  b).对于其中失效的单元格，广播玩家离开的消息(Disappear)，同时将该单元格内的所有玩家发给此玩家，告知离开
	5.玩家做出动作或者外观变化，则对周围九宫格内所有玩家进行广播
	*/

template< typename OBJECT_TYPE,		/*对象类型*/
	      typename COORD_TYPE,      /*坐标数据类型*/
		  int DIM,					/*几维空间*/
		  int GRID_SIZE,		    /*每个广播单元格的大小*/
		  int LAYER_COUNT			/*有几层AOI对象*/
>
class AOI_Manager
{
private:
	typedef GridTree< OBJECT_TYPE,int,DIM,GRID_SIZE > AOI_TREE;
	AOI_TREE    m_AOI_tree;
	int         m_nMaxSignRadii;			// 最大视野对象

public:
	AOI_Manager() : m_AOI_tree(LAYER_COUNT), m_nMaxSignRadii(0)
	{
		// memset( m_AOI_tree,0,sizeof(m_AOI_tree));
	}

	virtual ~AOI_Manager()
	{
	}

	// --------------------------------------------------------------------------------------------------- //
	// 初始化地图大小
	// --------------------------------------------------------------------------------------------------- //
	void initialize( COORD_TYPE loc[DIM],COORD_TYPE grid_num[DIM] )
	{
        m_AOI_tree.create(loc,grid_num);
	}

	// --------------------------------------------------------------------------------------------------- //
	// 插入实体
	// --------------------------------------------------------------------------------------------------- //
	bool insertEntity( OBJECT_TYPE obj,COORD_TYPE loc[DIM],int layer )
	{
        // 视野大小先弄成一样 目前假如不同实体视野覆盖范围不一样大应该还有bug。TODO.
        obj->m_nSight = 32;
		int nMyRadii= obj->m_nSight;
		int radii   = ROUND_UP( nMyRadii,GRID_SIZE);

		// 搜索周围实体列表
		OBJECT_TYPE pWatcherArray[MAX_INTEREST_OBJECT];
		int watcher_count = k_nearest(loc,radii,pWatcherArray,MAX_INTEREST_OBJECT,LAYER_WATCHER);

		OBJECT_TYPE pBlindrArray[MAX_INTEREST_OBJECT];
		int blind_count = k_nearest(loc,radii,pBlindrArray,MAX_INTEREST_OBJECT,LAYER_BLIND);

		// 看到周围的人
		int nCheckDist = radii==nMyRadii ? 0 : -1;		// 如果当前的视野正好是标准视野，则不用再精确判断距离
		for ( int i=0;i<watcher_count;++i )
		{
            OBJECT_TYPE & currentObj = pWatcherArray[i];
            // 就不要发给自己了
            if (currentObj && currentObj != obj)
            {   
                currentObj->onCreateNotify(obj->m_uid, -1);

                if( currentObj->m_bGhost||currentObj->m_bIsStealing )
                {
                    currentObj->onInOutSight( -1,false,obj->m_uid );
                }
            }
		}
		
		for ( int i=0;i<blind_count;++i )
		{
            OBJECT_TYPE & currentObj = pBlindrArray[i];
            if (currentObj && currentObj != obj)
            {
                currentObj->onCreateNotify(obj->m_uid, -1);

                if( currentObj->m_bGhost||currentObj->m_bIsStealing )
                {
                    currentObj->onInOutSight( -1,false,obj->m_uid );
                }
            }
		}

		// 更新一下最大视野
		if ( m_nMaxSignRadii<nMyRadii )
			m_nMaxSignRadii = nMyRadii;


		// 其他人的视野比我大，那就要重新搜索一下(大多情况下视野是一样大的）
		if ( nMyRadii<m_nMaxSignRadii )
		{
			radii = ROUND_UP(m_nMaxSignRadii,GRID_SIZE);
			watcher_count = k_nearest(loc,radii,pWatcherArray,MAX_INTEREST_OBJECT,LAYER_WATCHER);
		}

		// 让周围有视野的人看到我
		for ( int i=0;i<watcher_count;++i )
		{
            OBJECT_TYPE & currentObj = pWatcherArray[i];
            if( currentObj && currentObj!=obj )
            {
                obj->onCreateNotify(currentObj->m_uid, -1);

                if( obj->m_bGhost||obj->m_bIsStealing )
                {
                    obj->onInOutSight( -1,false,currentObj->m_uid );
                }
            }
		}

		// 插入到地图
		m_AOI_tree.insertObject(obj,loc,LAYER_INDEX(layer));
		return true;
	}

	// --------------------------------------------------------------------------------------------------- //
	// 移除实体
	// --------------------------------------------------------------------------------------------------- //
	bool removeEntity( OBJECT_TYPE obj,COORD_TYPE loc[DIM],int layer )
	{
		// 从地图上移除
		m_AOI_tree.removeObject(obj,loc,LAYER_INDEX(layer));


		int radii   = ROUND_UP(m_nMaxSignRadii,GRID_SIZE);

		// 只广播给有视野的对象
		OBJECT_TYPE pWatcherArray[MAX_INTEREST_OBJECT];
		int watcher_count = k_nearest(loc,radii,pWatcherArray,MAX_INTEREST_OBJECT,LAYER_WATCHER);
		for ( int i=0;i<watcher_count;++i )
		{
            OBJECT_TYPE & currentObj = pWatcherArray[i];
            if (currentObj)
            {
                obj->onDestroyNotify(currentObj->m_uid, -1);
            }
			//pWatcherArray[i]->onBroadcast( BROADCAST_OUT_OF_SIGHT,obj,PACKAGE_PTR(0) );
		}

		return true;
	}

	// --------------------------------------------------------------------------------------------------- //
	// 移动实体
	// --------------------------------------------------------------------------------------------------- //
	bool moveEntity( OBJECT_TYPE obj,COORD_TYPE old_loc[DIM],COORD_TYPE new_loc[DIM],int layer )
	{
		// 移动在地图容器中的位置
		if ( m_AOI_tree.moveObject(obj,old_loc,new_loc,LAYER_INDEX(layer)) ==false )
			return false;
	
		int nMyRadii= obj->m_nSight;
		int radii   = ROUND_UP(nMyRadii,GRID_SIZE);

		// 搜索周围实体列表
		OBJECT_TYPE pWatcherArray[MAX_INTEREST_OBJECT];
		int watcher_count = increment_search(old_loc,new_loc,radii,pWatcherArray,MAX_INTEREST_OBJECT,LAYER_WATCHER);

		// 是同一个单元格
		if ( watcher_count==-1 )
			return true;

		OBJECT_TYPE pBlindrArray[MAX_INTEREST_OBJECT];
		int blind_count = increment_search(old_loc,new_loc,radii,pBlindrArray,MAX_INTEREST_OBJECT,LAYER_BLIND);

		// 看到增量区域的对象
		int nCheckDist = radii==nMyRadii ? 0 : -1;		// 如果当前的视野正好是标准视野，则不用再精确判断距离
		for ( int i=0;i<watcher_count;++i )
		{
            OBJECT_TYPE & currentObj = pWatcherArray[i];
            // 就不要发给自己了
            if (currentObj && currentObj != obj)
            {
                currentObj->onCreateNotify(obj->m_uid, -1);

                if( currentObj->m_bGhost||currentObj->m_bIsStealing )
                {
                    currentObj->onInOutSight( -1,false,obj->m_uid );
                }
            }
		}

		for ( int i=0;i<blind_count;++i )
		{
			//obj->onBroadcast( BROADCAST_IN_SIGHT,pBlindrArray[i],PACKAGE_PTR(0) );
            OBJECT_TYPE & currentObj = pBlindrArray[i];
            // 就不要发给自己了
            if (currentObj && currentObj != obj)
            {
                currentObj->onCreateNotify(obj->m_uid, -1);

                if( currentObj->m_bGhost||currentObj->m_bIsStealing )
                {
                    currentObj->onInOutSight( -1,false,obj->m_uid );
                }
            }
		}

		// 其他人的视野比我大，那就要重新搜索一下(大多情况下视野是一样大的）
		if ( nMyRadii<m_nMaxSignRadii )
		{
			radii = ROUND_UP(m_nMaxSignRadii,GRID_SIZE);
			watcher_count = increment_search(old_loc,new_loc,radii,pWatcherArray,MAX_INTEREST_OBJECT,LAYER_WATCHER);
		}

		// 让周围有视野的人看到我
		for ( int i=0;i<watcher_count;++i )
		{
            //pWatcherArray[i]->onBroadcast( BROADCAST_IN_SIGHT,obj,PACKAGE_PTR(0) );
            OBJECT_TYPE & currentObj = pWatcherArray[i];
            // 就不要发给自己了
            if ( currentObj && currentObj!=obj )
            {
                obj->onCreateNotify(currentObj->m_uid, -1);

                if( obj->m_bGhost||obj->m_bIsStealing )
                {
                    obj->onInOutSight( -1,false,currentObj->m_uid );
                }
            }
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 让老视野的人消失
		radii   = ROUND_UP(nMyRadii,GRID_SIZE);

		// 搜索周围实体列表
		watcher_count = increment_search(new_loc,old_loc,radii,pWatcherArray,MAX_INTEREST_OBJECT,LAYER_WATCHER);
		blind_count = increment_search(new_loc,old_loc,radii,pBlindrArray,MAX_INTEREST_OBJECT,LAYER_BLIND);

		nCheckDist = radii==nMyRadii ? 0 : -1;		// 如果当前的视野正好是标准视野，则不用再精确判断距离
		for ( int i=0;i<watcher_count;++i )
		{
			//obj->onBroadcast( BROADCAST_OUT_OF_SIGHT,pWatcherArray[i],PACKAGE_PTR(0) );
            OBJECT_TYPE & currentObj = pWatcherArray[i];
            if (currentObj && currentObj != obj)
            {
                currentObj->onDestroyNotify(obj->m_uid, -1);
            }            
		}

		for ( int i=0;i<blind_count;++i )
		{
			//obj->onBroadcast( BROADCAST_OUT_OF_SIGHT,pBlindrArray[i],PACKAGE_PTR(0) );
            OBJECT_TYPE & currentObj = pBlindrArray[i];
            if (currentObj && currentObj != obj)
            {
                currentObj->onDestroyNotify(obj->m_uid, -1);
            }  
		}

		// 其他人的视野比我大，那就要重新搜索一下(大多情况下视野是一样大的）
		if ( nMyRadii<m_nMaxSignRadii )
		{
			radii = ROUND_UP(m_nMaxSignRadii,GRID_SIZE);
			watcher_count = increment_search(new_loc,old_loc,radii,pWatcherArray,MAX_INTEREST_OBJECT,LAYER_WATCHER);
		}

		// 让周围有视野的人不要看到我
		for ( int i=0;i<watcher_count;++i )
		{
			//pWatcherArray[i]->onBroadcast( BROADCAST_OUT_OF_SIGHT,obj,PACKAGE_PTR(0) );
            OBJECT_TYPE & currentObj = pWatcherArray[i];
            if (currentObj && currentObj != obj)
            {
                obj->onDestroyNotify(currentObj->m_uid, -1);
            }
		}

		return true;
	}

    // --------------------------------------------------------------------------------------------------- //
    // 更新实体的可见性
    // @param obj : 实体
    // --------------------------------------------------------------------------------------------------- //
    void onUpdateObjVisible( OBJECT_TYPE obj )
    {
        int radii   = ROUND_UP(m_nMaxSignRadii,GRID_SIZE);

        OBJECT_TYPE pWatcherArray[MAX_INTEREST_OBJECT];
        COORD_TYPE myLoc[DIM]; myLoc[0] = obj->m_Loc.x; myLoc[1] = obj->m_Loc.z;
        int watcher_count = k_nearest(myLoc,radii,pWatcherArray,MAX_INTEREST_OBJECT,LAYER_WATCHER);
        for ( int i=0;i<watcher_count;++i )
        {
            OBJECT_TYPE & currentObj = pWatcherArray[i];

            if( currentObj->m_uid!=obj->m_uid )
            {
                bool bVisible2Other = ((!obj->m_bGhost) && (!obj->m_bIsStealing));
                if( bVisible2Other )
                {
                    obj->onInOutSight( -1,true,currentObj->m_uid );
                }
                else
                {
                    obj->onInOutSight( -1,false,currentObj->m_uid );
                }
            }
            else
            {
                bool bVisible2SelfOld = !obj->m_bGhostOld;
                bool bVisible2Self = !obj->m_bGhost;

                // 对自己的的可见性没发生变化 不能给自己发进视野 会引起客户端摄像机视角变化
                if( bVisible2Self==bVisible2SelfOld )
                {
                    continue;
                }

                if( bVisible2Self )
                {
                    obj->onInOutSight( -1,true,currentObj->m_uid );
                }
                else
                {
                    obj->onInOutSight( -1,false,currentObj->m_uid );
                }
            }
        }
    }

	// --------------------------------------------------------------------------------------------------- //
	// 取某点周围一定范围内的格子内的所有实体 
	// @param dist : 距离半径(注意，这里不是精确匹配的，是把一个单元格内的所有对象都返回,误差为单元格大小)
	// --------------------------------------------------------------------------------------------------- //
	int k_nearest( COORD_TYPE loc[DIM],int dist,OBJECT_TYPE* pReturnArray,int nArraySize,int layerMask )
	{
		return m_AOI_tree.k_nearest(loc,dist,pReturnArray,nArraySize,layerMask);
	}

	// --------------------------------------------------------------------------------------------------- //
	// 搜索出,a移动到b时,b相对a的增量
	// --------------------------------------------------------------------------------------------------- //
	int increment_search(COORD_TYPE loc[DIM],COORD_TYPE new_loc[DIM],int dist,OBJECT_TYPE* pReturnArray,int nArraySize,int layerMask)
	{
		return m_AOI_tree.increment_search(loc,new_loc,dist,pReturnArray,nArraySize,layerMask);
	}

	// --------------------------------------------------------------------------------------------------- //
	// 范围广播
	// --------------------------------------------------------------------------------------------------- //
	void broadcast( COORD_TYPE loc[DIM],int dist,BROADCAST_ID id,OBJECT_TYPE obj,void *pContext,int len,int layerMask=LAYER_WATCHER,bool ignoreSelf=false)
	{
		Broadcast_Package * package = new Broadcast_Package;
		package->setContext( pContext,len );

		broadcast( loc,dist,id,obj,package,layerMask, ignoreSelf);

		package->Release(false);
	}

	void broadcast( COORD_TYPE loc[DIM],int dist,BROADCAST_ID id,OBJECT_TYPE obj,PACKAGE_PTR package,int layerMask=LAYER_WATCHER,bool ignoreSelf=false)
	{
        if( obj!=0 )
        {
            if( obj->m_bGhost )
                return;

            if( obj->m_bIsStealing )
            {
                obj->onBroadcast( id,obj,package );
                return;
            }
        }

		if ( dist==0 )
			dist = m_nMaxSignRadii;

		int radii   = ROUND_UP(dist,GRID_SIZE);

		// 只广播给有视野的对象
		OBJECT_TYPE pWatcherArray[MAX_INTEREST_OBJECT];
		int watcher_count = k_nearest(loc,radii,pWatcherArray,MAX_INTEREST_OBJECT,layerMask);
		for ( int i=0;i<watcher_count;++i )
		{
            if( obj!=0 && ignoreSelf && obj->m_uid==pWatcherArray[i]->m_uid)
            {
                continue;
            }

			pWatcherArray[i]->onBroadcast( id,obj,package );
		}
	}

    // 向看得见的多个点广播
    void broadcastMultiple( BROADCAST_ID id, int myLoc[AOI_LOC_COUNT][AOI_GRID_DIM],int count,OBJECT_TYPE obj,PACKAGE_PTR package )
    {
        int dist = m_nMaxSignRadii;

        int radii = ROUND_UP(dist,GRID_SIZE);

        // 使用map,重复不插入
        map<UID, OBJECT_TYPE> watcherMap;
        for (int i=0; i<count; ++i)
        {
            // 只广播给有视野的对象
            OBJECT_TYPE pWatcherArray[MAX_INTEREST_OBJECT];
            int watcher_count = k_nearest(myLoc[i],radii,pWatcherArray,MAX_INTEREST_OBJECT,LAYER_WATCHER);
            for ( int j=0;j<watcher_count;++j )
            {
                watcherMap.insert(map<UID, OBJECT_TYPE>::value_type(pWatcherArray[j]->m_uid,pWatcherArray[j]));
            }
        }
        
        for (map<UID, OBJECT_TYPE>::iterator Iter=watcherMap.begin(); Iter!=watcherMap.end(); ++Iter)
        {
            Iter->second->onBroadcast( id,obj,package );
        }
    }
};