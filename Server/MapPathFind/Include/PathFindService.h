/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\MapPathFind\Src\PathFindService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	3/12/2015	16:26
** 版  本:	1.0
** 描  述:	
寻路服务
********************************************************************/

#pragma once
#include <map>

#include "IPathFindService.h"

#include "EntityDef.h"
#include "AOI_Define.h"
#include "SchemeDef.h"
#include "PathHelper.h"
struct rcnNavmeshPoint;
class dtNavMesh;
class dtNavMeshQuery;
struct rcCompactHeightfield;
struct rcCompactSpan;
class HeightFieldMgr;
struct NvBuildConfig;
class QueryFilter;


#define PATHFIND_MSC_POLYREF_MAX			1000
const static int OBS_SPELL_CAN_PASS = 0x1;

class PathFindService : public IPathFindService
{
///////Self(Init&&Free)/////////////////////////////////////////////////
public:
    PathFindService();
    virtual			~PathFindService();
	bool			create(int nMapID, int nSceneID, int uPathFindServiceID, bool bSurface, float fExtent, PathFindServiceScheme * pSchemeInfo);
	// _Stub::release() call
	void			release();
	void			on_stop();
private:
	// 加载navmesh文件
	bool			loadNavMeshFile(char const * pcszFileName);
	// 加载mesh数据
	unsigned int	loadMeshData(unsigned char const * pbyData, int nSize);
	//获得navMesh数据 
	dtNavMesh*		getNavMesh() { return m_pNavMesh; }
//数据
public:
	// MapID和场景ID
	int				m_nMapID;
	int				m_nSceneID;
    NvBuildConfig*  m_chfConfig;
    rcCompactHeightfield* m_chfList;
private:
	// 基于凸多边形块的导航网格
	// DetourNavMesh.h
	dtNavMesh*		m_pNavMesh;

	// 为导航网格提供寻路特征
	// DetourNavMesh.h
	dtNavMeshQuery*	m_pNavMeshQuery;

	// 多边形过滤以及通过某些导航格子所需要花费的消耗
	QueryFilter*	m_pQueryFilter;

	//nav文件名
	string          m_strNavFileName;
	//chf文件名
	string          m_strCHFFileName;

	///////IPathFindService/////////////////////////////////////////////////
public:

    virtual bool    GetPath(Vector3 const& v3Start, Vector3 const& v3End, vector<Vector3>& vecRet, int nFlag = 0x0, int nCamp = 0x0, float fWidthMin = 0);
	bool	        GetPath(Vector3 const& v3Start, Vector3 const& v3End,	int nFlag = 0x0, int nCamp = 0x0, float fWidthMin = 0);

    virtual bool	getAroundPos(Vector3* v3Pos, float fRadius,vector<Vector3>& vecRet);
    
	virtual bool	isObstacle(Vector3* v3Pos);

	virtual bool	getHeight(Vector3* v3Pos, float& fHeight);
    
	virtual Vector3 getRaycast(const Vector3& startPos, const Vector3& rayDir, float dist);
    
	virtual Vector3 checkObstacle4Spell(const Vector3& startPos, const Vector3& rayDir, float dist, int nCamp = 0x0, int nFlag = 0x0);
    virtual bool	checkObstacle(const float *pos, const float *verts, const int &nVerts, int nFlag = 0x0, int nType = 0x0);
    virtual bool	startDynamicObstacleProxy();
	virtual bool	getAroundPos(Vector3* v3Pos, float fRadius, bool bObstacle[], int nCount, int nMaxCount);

	virtual int     addDynamicObstacle(UID uid, void* oInfo, unsigned char camp);

	virtual int	    removeDynamicObstacle(UID uid);

	virtual bool	addObstacle(UID uid, void* oInfo, unsigned char camp);
	virtual bool	removeObstacle(UID uid);

	virtual bool	hasCHF();

///////PathFind Tools/////////////////////////////////////////////////
public:
	// 最终路径 终点可能会变(终点最终只会在非障碍区域)
	// 关联此寻路服务对应PathAgent的行走路径,PathAgent中也有一个对应的m_vec3StraightPath
	Vector3			m_vec3StraightPath[PATHFIND_MSC_STRAIGHTPATH_MAX];
	int				m_nStraightPathCount;
private:
	// 是否2D寻路
	bool			m_bSurface;
	// 查找点最近的polygon的查找范围 must<128
	Vector3			m_v3Extents;
	// 寻路实际 0起始点 1终点,对输入进行最近查找获得的
	rcnNavmeshPoint	* m_pNPPath2Find;

	// 找到的寻路路径多边形引用集合
	unsigned int	m_PolyRef[PATHFIND_MSC_POLYREF_MAX];
	// 找到的寻路路径多边形引用总数
	int				m_nPolyRefCount;

public:
    // 获取pVec3Point最近的有效点pVec3Point
    bool			getNearPoint(Vector3* pVec3Point);
    
    // 查找点最近的polygon 填充到 rcnNavmeshPoint
    bool			FindNearestNavmeshPoint(Vector3 const* pVec3Point,Vector3 const* extent, rcnNavmeshPoint& rcnNP);

    // v3End->v3Start的直线 挨着v3End最近距离fMinDis放入stCount个半径fRadius的obj的坐标
    void			getLinePos(Vector3& v3Start, Vector3 const& v3End, size_t stCount, float fRadius, float fMinDis, vector<Vector3>& vecRet);
    // 获取v3End->v3Start的扇形忽略y
    void			getSectorPos(Vector3 v3Start, Vector3 v3End, size_t stCount, float fRadius, float fMinDis, vector<Vector3>& vecRet);

    // bSurface 是否2D
    // fExtent	查找点最近的polygon的查找范围
    void*			getStraightPathBuffer()
    {
        return (void*)m_vec3StraightPath;
    };

    unsigned int	getStraightPathBufferSize()
    {
        return (unsigned int)(m_nStraightPathCount * sizeof(m_vec3StraightPath[0]));
    };

    inline int		getStraightPathCount()
    {
        return min( m_nStraightPathCount,PATHFIND_MSC_STRAIGHTPATH_MAX );
    }
private:

    // 2D寻路,就是贴着导航网格的表面进行寻路
    bool			findPathPolygonsInSurface(Vector3 const* pvec3Start,Vector3 const* pvec3End);

    // 3D寻路，即通常意义上的寻路，寻找从路径起始点到终点的多边形路径
    bool			findPathPolygons( Vector3 const* pvec3Start, Vector3 const* pvec3End , unsigned int nFlag);

    // 将从起点到终点连接的多边形，转化为可直接行走的线段（路径）
    bool			findStraightPath();
//-------------------------动态障碍相关--2016-08-26-嘉文----------------//
private:
	static const int maxDynamicObstacles = 100;
	//空障碍节点链表头
	dtTileObstacle*	mNextFreeObstacleNode;
	//(非空)障碍节点链表头
	dtTileObstacle*	mNextObstacleNode;
	//记录所有动态障碍
	dtTileObstacle*	m_obstacle;	
	// 只影响物理检测的障碍物Map，用于管理战场中障碍怪物实体与对应障碍物多边形信息
	std::map<UID, dtTileObstacle> m_mapObsInfo;

	unsigned int*	flagList;		//存储具体的flag,将其index赋给areaID，每次增删都要更新这个数组【解决位数不足问题】

public:

#ifdef PATH_FIND_DEBUG
	void			getNavMeshBroadcastHeadDatas(obuf64& data, int& maxTiles);
	void			getNavMeshBroadcastDatas(obuf8192& data,int n);
#endif

private:
	//重构Tile
	bool			rebuildTile(int tileKey);
	void			addObstacleToFreeList(dtTileObstacle* free);
	void			addObstacleToList(dtTileObstacle* ob);
	void			chfSetCon(rcCompactSpan& s, int dir, int i);
	bool			checkInObstacle(dtTileObstacle* ob, const float* pos, const float *verts, const int &nVerts,int camp, int flag=0);
	bool			checkInObstacle(dtTileObstacle* ob, const float* pos, int camp, int flag = 0);
	bool			checkAcrossObstacle(dtTileObstacle* ob, const float* start, const float* dest, float* hitPos, int camp, int flag=0);
	inline unsigned int NextPow2(unsigned int v)
	{
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		v++;
		return v;
	}

	inline unsigned int Ilog2(unsigned int v)
	{
		unsigned int r;
		unsigned int shift;
		r = (v > 0xffff) << 4; v >>= r;
		shift = (v > 0xff) << 3; v >>= shift; r |= shift;
		shift = (v > 0xf) << 2; v >>= shift; r |= shift;
		shift = (v > 0x3) << 1; v >>= shift; r |= shift;
		r |= (v >> 1);
		return r;
	}

//--------------------------动态障碍相关--2016-08-26-嘉文---------------//
//---------------------------------END----------------------------------//
///废弃接口
public:
	//virtual int		addAgent(__IEntity* pEntity) { return 0; }
	//virtual void	removeAgent(int nAgentIndex) {}
	//virtual bool	getAgentPos(int nAgentIndex, Vector3* pv3Pos) { return true; }
	//virtual bool	setAgentTargetPos(int nAgentIndex, Vector3 const* pVec3Point) { return true; }
	//virtual bool	stopAgent(int nAgentIndex) { return true; }
	//// 加载导航瓷砖,目前没有使用
	//bool			loadTile(char const * pcFileName) { return true; }

	//// 手动即时更新Tile，主要用于动态障碍
	//void updateTile(const float dt, TileAction ta) {}
	//// 给客户端发添加障碍物消息
	//void sceneBroadcastAll(BROADCAST_ID id, string & msg, int nCampMask = CAMP_MASK_ALL) {}

	//// 其所属的寻路管理器
	//PathCrowd*					m_pCrowd;
	//struct LinearAllocator*		m_talloc;
	//struct FastLZCompressor*	m_tcomp;
	////struct MeshProcess*			m_tmproc;

	//// 导航瓷砖缓存，与动态障碍有关，Todo
	//class dtTileCache*			m_tileCache;

	////动态障碍服务引用
	//DynamicObstacleService_Proxy m_pDynamicObstacleProxy;

	// 障碍物map,每个阵营对应一个, pair<障碍物ID(使用时间障碍物创建时间戳)，dtPolyRef>
	// { typedef unsigned int dtPolyRef }
	// 障碍物Map,用于管理战场中的动态障碍物，key是PolRef，value是障碍物flag
	// std::map<unsigned int, int> m_mapObsRefFlag;
	//////////////////// IEventExecuteSink /////////////////////////////////////////////
	//virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	////////TimerHandler////////////////////////////////////////////////////
	//virtual void  OnTimer( unsigned long dwTimerID );

	// 实时动态更新路径
	//void update(const float dt);

	//void addDynamicObstacleBroadcast(UID uid, dtTileObstacle& obstacles) {}
	//void removeDynamicObstacleBroadcast(ObstacleRef obstacleRef) {}
	//bool			startDynamicObstacleProxyEX() {}
};
//#include "DynamicObstacleService_Proxy.h"
//#include <utility>
//#include "Event_ManagedDef.h"
//#include "IEventEngine.h"

//enum ContourBuildFlags {
//	/// Tessellate the wall (unconnected border) edges.
//	TessellateWallEdges = 0x01,
//
//	/// Tessellate the edges between areas.
//	TessellateAreaEdges = 0x02
//};