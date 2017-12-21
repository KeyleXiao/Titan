/*******************************************************************
** 文件名:	AOI_Manager_MMO.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	10/15/2014
** 版  本:	1.0
** 描  述:	

			
********************************************************************/

#pragma once

#include "AOI_Manager.h"
#include "BCStrategyMgr.h"
#include "GridTree.h"

/**
	针对MMO N多人同屏PK的战场：
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
class AOI_Manager_MMO : public AOI_Manager
{
public:
	AOI_Manager_MMO() : m_nMaxSignRadii(0)
	{
		memset( m_AOI_tree,0,sizeof(m_AOI_tree));
		m_pBCStrategyMgr = new BCStrategyMgr();
	}

	virtual ~AOI_Manager_MMO()
	{
	}

	// --------------------------------------------------------------------------------------------------- //
	// 初始化地图大小
	// --------------------------------------------------------------------------------------------------- //
	virtual bool initialize( int nMapID,int nWidth,int nHeight );

	// --------------------------------------------------------------------------------------------------- //
	// 插入实体
	// --------------------------------------------------------------------------------------------------- //
	virtual bool insertEntity( AOI_Proxy* obj,int loc[AOI_GRID_DIM],int layer );

	// --------------------------------------------------------------------------------------------------- //
	// 移除实体
	// --------------------------------------------------------------------------------------------------- //
	virtual bool removeEntity( AOI_Proxy* obj,int loc[AOI_GRID_DIM],int layer );

	// --------------------------------------------------------------------------------------------------- //
	// 移动实体
	// --------------------------------------------------------------------------------------------------- //
	virtual bool moveEntity( AOI_Proxy* obj,int old_loc[AOI_GRID_DIM],int new_loc[AOI_GRID_DIM],int layer );

	// --------------------------------------------------------------------------------------------------- //
	// 取某点周围一定范围内的格子内的所有实体 
	// @param dist : 距离半径(注意，这里不是精确匹配的，是把一个单元格内的所有对象都返回,误差为单元格大小)
	// --------------------------------------------------------------------------------------------------- //
	virtual int k_nearest( int loc[AOI_GRID_DIM],int dist,AOI_Proxy** pReturnArray,int nArraySize,int layerMask );

	// --------------------------------------------------------------------------------------------------- //
	// 范围广播
	// --------------------------------------------------------------------------------------------------- //
	virtual void broadcast( int loc[AOI_GRID_DIM],int dist,BROADCAST_ID id,AOI_Proxy* obj,PACKAGE_PTR package,int layerMask=LAYER_WATCHER);

	virtual void Dispatch(AOI_Proxy* pClientObj, AOI_Proxy* pSrcObj, BROADCAST_ID id, PACKAGE_PTR package = PACKAGE_PTR(NULL));

    // 客户端代理对象请求显示指定实体
    virtual void requestDisplayEntity( AOI_Proxy* pClientObj,UID uidEntity ) { }

	virtual void Run();

protected:
	// --------------------------------------------------------------------------------------------------- //
	// 搜索出,a移动到b时,b相对a的增量
	// --------------------------------------------------------------------------------------------------- //
	int increment_search(int loc[AOI_GRID_DIM],int new_loc[AOI_GRID_DIM],int dist,AOI_Proxy** pReturnArray,int nArraySize,int layerMask);

private:
	typedef GridTree< AOI_Proxy*,int,AOI_GRID_DIM,AOI_GRID_SIZE > AOI_TREE;
	AOI_TREE			m_AOI_tree[LAYER_COUNT];

	int					m_nMaxSignRadii;			// 当前实体中的最大视野
	BCStrategyMgr*		m_pBCStrategyMgr;			// 广播策略管理器
};
