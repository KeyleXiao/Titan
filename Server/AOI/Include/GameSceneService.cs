using System;
using System.Reflection;
using System.IO;
using System.Text;

struct AOI_PROXY_PTR
{
};

struct Vector3
{
};

struct BROADCAST_ID
{
};

interface GameSceneService
{
	// 创建场景,传入ID和场景配置信息
	bool onCreate( int mapID,int sceneID );

	// 获取唯一场景ID
	int getSceneID();

	// 获取地图ID，同一张地图可以创建多个副本
	int getMapID();

	// 插入实体
	bool insertEntity( AOI_PROXY_PTR proxy,Vector3 loc,int layer );

	// 移除实体;
	bool removeEntity( AOI_PROXY_PTR proxy,Vector3 loc,int layer );

	// 移动实体
	bool moveEntity( AOI_PROXY_PTR proxy,Vector3 oldLoc,Vector3 newLoc,int layer );

	// 取某点周围一定范围内的格子内的所有实体 
	// @param dist : 距离半径(注意，这里不是精确匹配的，是把一个单元格内的所有对象都返回,误差为单元格大小)
	unsafe int k_nearest( Vector3 loc,int dist,AOI_PROXY_PTR* pReturnArray,int nArraySize,int layerMask );

	// 范围广播 
	int broadcast(Vector3 loc,int dist,BROADCAST_ID id,string msg,int layerMask);

	// 全体玩家广播
	int broadcast_all( BROADCAST_ID id,string msg );

	// 获取玩家总数
	int getPlayerCount();

	// 获取所有玩家的对象列表
	// @param return : 返回真正返回的玩家数目
	unsafe int getAllPlayers( AOI_PROXY_PTR * pReturnArray,int nArraySize );

	// 销毁地图
	void release();
};
