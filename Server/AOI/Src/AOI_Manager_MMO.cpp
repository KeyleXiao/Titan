/*******************************************************************
** 文件名:	AOI_Manager_MMO.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	10/15/2014
** 版  本:	1.0
** 描  述:	

			
********************************************************************/

#include "stdafx.h"
#include "AOI_Manager_MMO.h"

// --------------------------------------------------------------------------------------------------- //
// 初始化地图大小
// --------------------------------------------------------------------------------------------------- //
bool AOI_Manager_MMO::initialize( int nMapID,int nWidth,int nHeight )
{
    int origin[AOI_GRID_DIM] = {0, 0};
    int grid_num[AOI_GRID_DIM];
    getGridNum(nWidth, nHeight, grid_num);

	for ( int i=0;i<LAYER_COUNT;++i )
	{
		m_AOI_tree[i].create( origin,grid_num );
	}

	return true;
}

// --------------------------------------------------------------------------------------------------- //
// 插入实体
// --------------------------------------------------------------------------------------------------- //
bool AOI_Manager_MMO::insertEntity( AOI_Proxy* obj,int loc[AOI_GRID_DIM],int layer )
{
	m_pBCStrategyMgr->InsertEntity(obj);

	int nMyRadii= obj->getAOISight();
	int radii   = ROUND_UP( nMyRadii,AOI_GRID_SIZE);

	// 搜索周围实体列表
	AOI_Proxy* pWatcherArray[MAX_INTEREST_OBJECT];
	int watcher_count = k_nearest(loc,radii,pWatcherArray,MAX_INTEREST_OBJECT,LAYER_WATCHER);

	AOI_Proxy* pBlindrArray[MAX_INTEREST_OBJECT];
	int blind_count = k_nearest(loc,radii,pBlindrArray,MAX_INTEREST_OBJECT,LAYER_BLIND);

	// 看到周围的人
	int nCheckDist = radii==nMyRadii ? 0 : -1;		// 如果当前的视野正好是标准视野，则不用再精确判断距离
	for ( int i=0;i<watcher_count;++i )
	{
		m_pBCStrategyMgr->Dispatch( obj,pWatcherArray[i],BROADCAST_CREATE );
	}

	for ( int i=0;i<blind_count;++i )
	{
		m_pBCStrategyMgr->Dispatch( obj,pBlindrArray[i],BROADCAST_CREATE );
	}

	// 更新一下最大视野
	if ( m_nMaxSignRadii<nMyRadii )
		m_nMaxSignRadii = nMyRadii;


	// 其他人的视野比我大，那就要重新搜索一下(大多情况下视野是一样大的）
	if ( nMyRadii<m_nMaxSignRadii )
	{
		radii = ROUND_UP(m_nMaxSignRadii,AOI_GRID_SIZE);
		watcher_count = k_nearest(loc,radii,pWatcherArray,MAX_INTEREST_OBJECT,LAYER_WATCHER);
	}

	// 让周围有视野的人看到我
	for ( int i=0;i<watcher_count;++i )
	{
		m_pBCStrategyMgr->Dispatch( pWatcherArray[i],obj,BROADCAST_CREATE );
	}

	// 插入到地图
	m_AOI_tree[LAYER_INDEX(layer)].insertObject(obj,loc);
	return true;
}

// --------------------------------------------------------------------------------------------------- //
// 移除实体
// --------------------------------------------------------------------------------------------------- //
bool AOI_Manager_MMO::removeEntity( AOI_Proxy* obj,int loc[AOI_GRID_DIM],int layer )
{
	m_pBCStrategyMgr->RemoveEntity(obj);

	// 从地图上移除
	m_AOI_tree[LAYER_INDEX(layer)].removeObject(obj,loc);


	int radii   = ROUND_UP(m_nMaxSignRadii,AOI_GRID_SIZE);

	// 只广播给有视野的对象
	AOI_Proxy* pWatcherArray[MAX_INTEREST_OBJECT];
	int watcher_count = k_nearest(loc,radii,pWatcherArray,MAX_INTEREST_OBJECT,LAYER_WATCHER);
	for ( int i=0;i<watcher_count;++i )
	{
		m_pBCStrategyMgr->Dispatch( pWatcherArray[i],obj,BROADCAST_ID(BROADCAST_DESTROY) );
	}

	return true;
}

// --------------------------------------------------------------------------------------------------- //
// 移动实体
// --------------------------------------------------------------------------------------------------- //
bool AOI_Manager_MMO::moveEntity( AOI_Proxy* obj,int old_loc[AOI_GRID_DIM],int new_loc[AOI_GRID_DIM],int layer )
{
	// 移动在地图容器中的位置
	if ( m_AOI_tree[LAYER_INDEX(layer)].moveObject(obj,old_loc,new_loc) ==false )
		return false;

	int nMyRadii= obj->getAOISight();
	int radii   = ROUND_UP(nMyRadii,AOI_GRID_SIZE);

	// 搜索周围实体列表
	AOI_Proxy* pWatcherArray[MAX_INTEREST_OBJECT];
	int watcher_count = increment_search(old_loc,new_loc,radii,pWatcherArray,MAX_INTEREST_OBJECT,LAYER_WATCHER);

	// 是同一个单元格
	if ( watcher_count==-1 )
		return true;

	AOI_Proxy* pBlindrArray[MAX_INTEREST_OBJECT];
	int blind_count = increment_search(old_loc,new_loc,radii,pBlindrArray,MAX_INTEREST_OBJECT,LAYER_BLIND);

	// 看到增量区域的对象
	int nCheckDist = radii==nMyRadii ? 0 : -1;		// 如果当前的视野正好是标准视野，则不用再精确判断距离
	for ( int i=0;i<watcher_count;++i )
	{
		m_pBCStrategyMgr->Dispatch( obj,pWatcherArray[i],BROADCAST_ENTER_VIEWPORT );
	}

	for ( int i=0;i<blind_count;++i )
	{
		m_pBCStrategyMgr->Dispatch( obj,pBlindrArray[i],BROADCAST_ENTER_VIEWPORT );
	}

	// 其他人的视野比我大，那就要重新搜索一下(大多情况下视野是一样大的）
	if ( nMyRadii<m_nMaxSignRadii )
	{
		radii = ROUND_UP(m_nMaxSignRadii,AOI_GRID_SIZE);
		watcher_count = increment_search(old_loc,new_loc,radii,pWatcherArray,MAX_INTEREST_OBJECT,LAYER_WATCHER);
	}

	// 让周围有视野的人看到我
	for ( int i=0;i<watcher_count;++i )
	{
		m_pBCStrategyMgr->Dispatch( pWatcherArray[i],obj,BROADCAST_ENTER_VIEWPORT );
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 让老视野的人消失
	radii   = ROUND_UP(nMyRadii,AOI_GRID_SIZE);

	// 搜索周围实体列表
	watcher_count = increment_search(new_loc,old_loc,radii,pWatcherArray,MAX_INTEREST_OBJECT,LAYER_WATCHER);
	blind_count = increment_search(new_loc,old_loc,radii,pBlindrArray,MAX_INTEREST_OBJECT,LAYER_BLIND);

	nCheckDist = radii==nMyRadii ? 0 : -1;		// 如果当前的视野正好是标准视野，则不用再精确判断距离
	for ( int i=0;i<watcher_count;++i )
	{
		m_pBCStrategyMgr->Dispatch( obj,pWatcherArray[i],BROADCAST_ENTER_VIEWPORT );
	}

	for ( int i=0;i<blind_count;++i )
	{
		m_pBCStrategyMgr->Dispatch( obj,pBlindrArray[i],BROADCAST_LEAVE_VIEWPORT );
	}

	// 其他人的视野比我大，那就要重新搜索一下(大多情况下视野是一样大的）
	if ( nMyRadii<m_nMaxSignRadii )
	{
		radii = ROUND_UP(m_nMaxSignRadii,AOI_GRID_SIZE);
		watcher_count = increment_search(new_loc,old_loc,radii,pWatcherArray,MAX_INTEREST_OBJECT,LAYER_WATCHER);
	}

	// 让周围有视野的人不要看到我
	for ( int i=0;i<watcher_count;++i )
	{
		m_pBCStrategyMgr->Dispatch( pWatcherArray[i],obj,BROADCAST_ENTER_VIEWPORT );
	}

	return true;
}

// --------------------------------------------------------------------------------------------------- //
// 取某点周围一定范围内的格子内的所有实体 
// @param dist : 距离半径(注意，这里不是精确匹配的，是把一个单元格内的所有对象都返回,误差为单元格大小)
// --------------------------------------------------------------------------------------------------- //
int AOI_Manager_MMO::k_nearest( int loc[AOI_GRID_DIM],int dist,AOI_Proxy** pReturnArray,int nArraySize,int layerMask )
{
	int count = 0;
	if ( layerMask & LAYER_WATCHER )
	{
		count = m_AOI_tree[0].k_nearest(loc,dist,pReturnArray,nArraySize);
	}

	if ( layerMask & LAYER_BLIND )
	{
		count += m_AOI_tree[1].k_nearest(loc,dist,pReturnArray+count,nArraySize-count);
	}

	return count;
}

// --------------------------------------------------------------------------------------------------- //
// 范围广播
// --------------------------------------------------------------------------------------------------- //
void AOI_Manager_MMO::broadcast( int loc[AOI_GRID_DIM],int dist,BROADCAST_ID id,AOI_Proxy* obj,PACKAGE_PTR package,int layerMask)
{
	if ( dist==0 )
		dist = m_nMaxSignRadii;

	int radii   = ROUND_UP(dist,AOI_GRID_SIZE);

	// 只广播给有视野的对象
	AOI_Proxy* pWatcherArray[MAX_INTEREST_OBJECT];
	int watcher_count = k_nearest(loc,radii,pWatcherArray,MAX_INTEREST_OBJECT,layerMask);
	for ( int i=0;i<watcher_count;++i )
	{
		m_pBCStrategyMgr->Dispatch( pWatcherArray[i],obj,id,package );
	}
}

void AOI_Manager_MMO::Dispatch( AOI_Proxy* pClientObj,AOI_Proxy* pSrcObj,BROADCAST_ID id,PACKAGE_PTR package )
{
	m_pBCStrategyMgr->Dispatch( pClientObj,pSrcObj,id,package );
}

void AOI_Manager_MMO::Run()
{
	m_pBCStrategyMgr->Run();
}

// --------------------------------------------------------------------------------------------------- //
// 搜索出,a移动到b时,b相对a的增量
// --------------------------------------------------------------------------------------------------- //
int AOI_Manager_MMO::increment_search(int loc[AOI_GRID_DIM],int new_loc[AOI_GRID_DIM],int dist,AOI_Proxy** pReturnArray,int nArraySize,int layerMask)
{
	int count = 0;
	if ( layerMask & LAYER_WATCHER )
	{
		count = m_AOI_tree[0].increment_search(loc,new_loc,dist,pReturnArray,nArraySize);
	}

	if ( layerMask & LAYER_BLIND )
	{
		count += m_AOI_tree[1].increment_search(loc,new_loc,dist,pReturnArray+count,nArraySize-count);
	}

	return count;
}
