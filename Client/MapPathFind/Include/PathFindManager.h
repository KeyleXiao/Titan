/*****************************************************************************
** 文件名:	PathFindManager.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	彭璐
** 日  期:	2015/07/14
** 版  本:	1.0
** 描  述:	寻路管理头
** 应  用:
**************************** 修改记录 ****************************************
** 修改人:  
** 日  期:  
** 描  述:  
******************************************************************************/
#pragma once

#include "IPathFindManager.h"
#include "IMessageDispatch.h"
#include "NavBroadcaster.h"
#include "HeightFieldMgr.h"

struct msgTileObstacle
{
	union {
		dtObstacleCylinder cylinder;
		dtObstacleBox box;
	};
	int uid;
	ObstacleType type;
	int nFlag;
	float angle;
	bool isPreload;
};

class PathFindManager : public IPathFindManager, public IMessageHandler
{
public:
    virtual bool                load();

    virtual void                release();

    virtual IPathFindService*   createServiceInterface( int nMapID,int nSceneID );

    virtual void                releaseServiceInterface( IPathFindService *& pInterface );

    virtual IPathFindService*  getPathFindService();

    ////////////////////////////////////////IMessageHandler////////////////////////////////////////////////////////////
    // 模块消息处理
    virtual void onMessage( SNetMsgHead* head, void* data, size_t len);

public:
    PathFindManager();
	void addPreLoadObstacle(dtTileObstacle ob);
	bool onPathFindMessageID(int msgId, void *data, size_t len);

	void getNewNavMeshShow(float* verts,int tileID);
	//获得tile的数量
	int getNavMeshMaxTile();
	//获得navmeshTile的大小
	int getNavMeshTileSize(int tileID);

private:
	void						realeaseService();
private:
    // 
    IPathFindService*           m_pPathFindService;
	NavBroadcaster				m_navBroadcaster; 
	std::vector<dtTileObstacle> m_obstacleArray;	//该地图在开始前就预先存在的碰撞体（服务端会提前发送，先用list缓存起来）
	int							m_nSceneID;
	int							m_nMapID;
};
