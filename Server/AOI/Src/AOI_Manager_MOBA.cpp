/*******************************************************************
** 文件名:	AOI_Manager_MOBA.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - All Rights Reserved
** 创建人:	彭璐
** 日  期:	3/16/2015
** 版  本:	1.0
** 描  述:	MOBA类AOI管理
            以前写的AOI_Manager.h主要是针对MMO的，对MOBA类战场需要重写一份AOI
********************************************************************/

#include "stdafx.h"
#include "AOI_Manager_MOBA.h"
#include <cassert>

bool AOI_Manager_MOBA::initialize( int nMapID,int nWidth,int nHeight )
{
    int origin[AOI_GRID_DIM] = {0, 0};
    int grid_num[AOI_GRID_DIM];
    getGridNum(nWidth, nHeight, grid_num);

	for ( int i=0;i<CAMP_MAX;++i )
	{
		if ( !m_AOI_tree[i].create(origin, grid_num) )
			return false;
	}

    m_pMapForest = AOI_MapForestFactory::getInstance().createMapForest( nMapID,nWidth,nHeight );

	return true;
}

bool AOI_Manager_MOBA::insertEntity( AOI_Proxy* obj,int loc[AOI_GRID_DIM],int layer )
{
	int camp = obj->getCamp();
	if ( camp>=CAMP_MAX )
		return false;

	bool isPlayer = isObjPlayer(obj);
	// 向看得见的人广播
	broadcast( BROADCAST_CREATE,loc,obj,obj->getPackage(BROADCAST_CREATE),camp );

	int sight = obj->getAOISight();

	if ( isPlayer )
	{
		AOI_Proxy* pObjectArray[MAX_INTEREST_OBJECT];
		// 搜索出在所有在我视野内的实体并通知我更新
		for ( int cmp=0;cmp<CAMP_MAX;++cmp )
		{
			int count = m_AOI_tree[cmp].k_nearest( loc,sight,pObjectArray,MAX_INTEREST_OBJECT );
			for ( int i=0;i<count;++i )
			{
                Vector3 posObj = pObjectArray[i]->getLocation();
                int locObj[2]; 
                normalizeLoc( posObj,locObj );
				if ( canWatch(obj,locObj) )
					obj->sendBCMessage( pObjectArray[i]->getPackage(BROADCAST_CREATE) );
			}
		}
	}

	// 统计一个最大视野
	if ( sight>m_max_sight[camp] )
	{
		m_max_sight[camp] = sight;
	}

	// 加入到阵营列表
	if ( isPlayer )
	{
		m_player_set[camp].insert( obj );
	}
	
	// 插入到地图
    m_AOI_tree[camp].insertObject( obj,loc );

    m_mapEntity[obj->getID()] = obj;

	return true;
}

bool AOI_Manager_MOBA::removeEntity( AOI_Proxy* obj,int loc[AOI_GRID_DIM],int layer )
{
	int camp = obj->getCamp();
	if ( camp>=CAMP_MAX )
		return false;

	if ( isObjPlayer(obj) )
		m_player_set[camp].erase( obj );

	// 删除实体
	m_AOI_tree[camp].removeObject( obj,loc );

    m_mapEntity.erase( obj->getID() );

	// 向看得见的人广播
	broadcast( BROADCAST_DESTROY,loc,obj,obj->getPackage(BROADCAST_DESTROY),camp );

	return true;
}

bool AOI_Manager_MOBA::moveEntity( AOI_Proxy* obj,int old_loc[AOI_GRID_DIM],int new_loc[AOI_GRID_DIM],int layer )
{
	int camp = obj->getCamp();
	if ( camp>=CAMP_MAX )
		return false;

	// 移动在地图容器中的位置
	if ( !m_AOI_tree[camp].moveObject(obj,old_loc,new_loc) )
		return false;

    if ( !isObjPlayer(obj) )
    {
        return true;
    }

    // 搜索增量区域中实体，并通告obj这些实体进入视野了
    int sight = obj->getAOISight();
    AOI_Proxy* pObjectArray[MAX_INTEREST_OBJECT];
    for ( int cmp=0;cmp<CAMP_MAX;++cmp )
    {
        if ( cmp==camp )
            continue;

        int nCount = m_AOI_tree[cmp].increment_search( old_loc,new_loc,sight,pObjectArray,_countof(pObjectArray) );
        for ( int i=0;i<nCount;++i )
        {
            // 通知obj新实体进入视野
            obj->sendBCMessage( pObjectArray[i]->getPackage(BROADCAST_ENTER_VIEWPORT) );
        }
    }

	return true;
}

// 取出loc视野dist内的所有实体，放入pReturnArray中
int AOI_Manager_MOBA::k_nearest(int loc[AOI_GRID_DIM], int dist, AOI_Proxy** pReturnArray, int nArraySize, int camp)
{
    int count = 0;

    for ( int cmp=0;cmp<CAMP_MAX;++cmp )
    {
        count += m_AOI_tree[cmp].k_nearest(loc,dist,pReturnArray+count,nArraySize-count);
    }

    return count;
}

void AOI_Manager_MOBA::broadcast(int loc[AOI_GRID_DIM], int dist, BROADCAST_ID id, AOI_Proxy* obj, PACKAGE_PTR package, int layerMask)
{
	// 向看得见的人广播
	broadcast( id,loc,obj,package );
}

void AOI_Manager_MOBA::Dispatch(AOI_Proxy* pClientObj, AOI_Proxy* pSrcObj, BROADCAST_ID id, PACKAGE_PTR package)
{
	// 直接发包
	pClientObj->sendBCMessage(package);
}

void AOI_Manager_MOBA::Run()
{

}

void AOI_Manager_MOBA::requestDisplayEntity( AOI_Proxy * pClientObj,UID uidEntity )
{
    AOI_Proxy * pDestObj = NULL;
    MAP_PROXY::iterator itr = m_mapEntity.find(uidEntity);
    if ( itr!=m_mapEntity.end() )
    {
        pDestObj = itr->second;
    }
    if ( pDestObj==NULL || pClientObj->getCamp()>=CAMP_MAX )
        return;

    // 校验一下，只有能看到的才发
    Vector3 posDest = pDestObj->getLocation();
    int locDest[2]; 
    normalizeLoc( posDest,locDest );

    if ( isInSight(locDest,pClientObj->getCamp()) )
    {
        pClientObj->sendBCMessage( pDestObj->getPackage(BROADCAST_CREATE) );
    }
}

bool AOI_Manager_MOBA::isInSight( int loc[AOI_GRID_DIM],int camp )
{
	AOI_Proxy* pWatcherArray[MAX_INTEREST_OBJECT];

    // 周围只要有一个该阵营的对象，则表示在视野内
    int watcher_count = m_AOI_tree[camp].k_nearest( loc,m_max_sight[camp],pWatcherArray,MAX_INTEREST_OBJECT );
	for ( int i=0;i<watcher_count;++i )
	{
        // 判断实体是否可以看到指定位置
        if ( canWatch(pWatcherArray[i],loc) )
        {
            return true;
        }
	}

	return false;
}

// 阵营广播
void AOI_Manager_MOBA::broadcast_camp( int campFlag,BROADCAST_ID id,AOI_Proxy* obj,PACKAGE_PTR package )
{
	if ( campFlag<0 || campFlag>=CAMP_MAX )
		return ;

	PLAYER_SET::iterator it = m_player_set[campFlag].begin();
	for ( ;it!=m_player_set[campFlag].end();++it )
	{
		(*it)->sendBCMessage( package );
	}
}

// 向看得见的人广播
void AOI_Manager_MOBA::broadcast( BROADCAST_ID id,int loc[AOI_GRID_DIM],AOI_Proxy* obj,PACKAGE_PTR package,int selfCamp )
{
	for ( int cmp=0;cmp<CAMP_MAX;++cmp )
	{
		if ( selfCamp==cmp || isInSight( loc,cmp ) )
		{
			broadcast_camp( cmp,id,obj,package );
		}
	}
}

// 向所有人广播，不管视野
void AOI_Manager_MOBA::broadcast_all( BROADCAST_ID id,AOI_Proxy* obj,PACKAGE_PTR package )
{
	for ( int cmp=0;cmp<CAMP_MAX;++cmp )
	{
		broadcast_camp( cmp,id,obj,package );
	}
}

// 判断watcher是否可以看到指定实体
bool AOI_Manager_MOBA::canWatch( AOI_Proxy* watcher,int loc[AOI_GRID_DIM] )
{
    // 这里要硬编码下
    Vector3 posWatcher = watcher->getLocation();
    int r = watcher->getAOISight();
    // 超过视距直接返回
    if ( (posWatcher.x-loc[0])*(posWatcher.x-loc[0]) + (posWatcher.z-loc[1])*(posWatcher.z-loc[1])> r*r )
        return false;
    // 我不在草丛，而它在草丛中，则我看不到它
    if ( !isInForest((int)posWatcher.x,(int)posWatcher.z) && isInForest((int)loc[0],(int)loc[1]) )
       return false;

    return true;
}
