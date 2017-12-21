/*******************************************************************
** 文件名:	AOI_Manager_MOBA.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - All Rights Reserved
** 创建人:	彭璐
** 日  期:	3/16/2015
** 版  本:	1.0
** 描  述:	MOBA类AOI管理
            以前写的AOI_Manager.h主要是针对MMO的，目前这对MOBA类战场需要重写一份AOI
********************************************************************/

#pragma once

#include "AOI_Manager.h"
#include "GridTree.h"
#include "AOI_MapForest.h"
#include "IGameSceneService.h"
#include "EntityDef.h"
#include <list>
#include <set>

// 包含ICamp.h N多编译问题搞不定，这里直接定义算了
#define CAMP_NEUTRAL	0
#define CAMP_MAX		3

/*	MOBA类AOI管理器隶属于场景（GameScene），主要完成:
*	1. 根据指定的场景进行初始化
*	2. 维护实体的进入、离开、移动，并对有视野的客户端（玩家、观察者）进行广播
*	3. 实体做动作，需要广播给有视野的客户端
*	4. 实体外观发生变化，也需要广播
*	
*	实现方式：
*	初始化时，根据地图大小生成一个二维的数组，数组元素值表示坐标的视野信息（红方|蓝方）
*	所以整个AOI实际上就是对这个数组进行维护，在实体进入、离开时修改此坐标的视野信息
*/
class AOI_Manager_MOBA : public AOI_Manager
{
	// 不允许拷贝构造和赋值构造
	AOI_Manager_MOBA(const AOI_Manager_MOBA&);
	AOI_Manager_MOBA& operator=(AOI_Manager_MOBA&);

public:
	AOI_Manager_MOBA()
	{
		m_pMapForest = NULL;

        memset( m_mapsize,0,sizeof(m_mapsize) );
        memset( m_gridsize,0,sizeof(m_gridsize) );
        memset( m_max_sight,0,sizeof(m_max_sight) );
	}

public:
	virtual bool initialize( int nMapID,int nWidth,int nHeight );

	virtual bool insertEntity( AOI_Proxy* obj,int loc[AOI_GRID_DIM],int layer );

	virtual bool removeEntity( AOI_Proxy* obj,int loc[AOI_GRID_DIM],int layer );

	virtual bool moveEntity( AOI_Proxy* obj,int old_loc[AOI_GRID_DIM],int new_loc[AOI_GRID_DIM],int layer );

	virtual int k_nearest( int loc[AOI_GRID_DIM],int dist, AOI_Proxy** pReturnArray,int nArraySize,int camp );

	virtual void broadcast( int loc[AOI_GRID_DIM],int dist,BROADCAST_ID id,AOI_Proxy* obj,PACKAGE_PTR package,int layerMask );

	virtual void Dispatch( AOI_Proxy* pClientObj,AOI_Proxy* pSrcObj,BROADCAST_ID id,PACKAGE_PTR package = PACKAGE_PTR(NULL) );

	virtual void Run();

    // 客户端代理对象请求显示指定实体
    virtual void requestDisplayEntity( AOI_Proxy * pClientObj,UID uidEntity );

	// 某个点是否在某个阵营的视野中
	bool isInSight( int loc[AOI_GRID_DIM],int campFlag );

	// 阵营广播
	void broadcast_camp( int campFlag,BROADCAST_ID id,AOI_Proxy* obj,PACKAGE_PTR package );

	// 向看得见的人广播
	void broadcast( BROADCAST_ID id,int loc[AOI_GRID_DIM],AOI_Proxy* obj,PACKAGE_PTR package,int selfCamp=0 );

	// 向所有人广播，不管视野
	void broadcast_all( BROADCAST_ID id,AOI_Proxy* obj,PACKAGE_PTR package );

	// 判断实体是否为玩家
	inline bool isObjPlayer( AOI_Proxy* obj )
	{
		UID uid = obj->getID();
		return ( UID_2_TYPE(uid)==TYPE_PLAYER_ROLE );
	}

    inline bool isInForest( int nX, int nY)
    {
        return ( m_pMapForest && m_pMapForest->getForestID(nX, nY)!=0 );
    }

    // 让浮点坐标系标准化，乘系数转成整型
    void normalizeLoc( Vector3 fLoc,int iLoc[AOI_GRID_DIM] )
    {
        iLoc[0] = (int)fLoc.x;
        iLoc[1] = (int)fLoc.z;
    }

    // 判断watcher是否可以看到指定位置
    bool canWatch( AOI_Proxy* watcher,int loc[AOI_GRID_DIM] );

private:
	int				m_mapsize[AOI_GRID_DIM];	// 地图大小
	int				m_gridsize[AOI_GRID_DIM];	// 一个的格子大小

	typedef std::set<AOI_Proxy*> PLAYER_SET;
	PLAYER_SET		m_player_set[CAMP_MAX];	    // 各阵营玩家列表

    int             m_max_sight[CAMP_MAX];      // 阵营最大视野

	typedef GridTree< AOI_Proxy*,int,AOI_GRID_DIM,AOI_GRID_SIZE > AOI_TREE;
	AOI_TREE		m_AOI_tree[CAMP_MAX];
	AOI_MapForest*	m_pMapForest;				// 地图的草丛对象

    typedef std::map< UID,AOI_Proxy* > MAP_PROXY;
    MAP_PROXY       m_mapEntity;                // uid->实体映射表
};
