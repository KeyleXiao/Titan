/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\MapPathFind\Src\PathFindService_Proxy.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	3/12/2015	16:26
** 版  本:	1.0
** 描  述:	
            寻路服务代理
********************************************************************/

#pragma once

#include "IPathFindService.h"
#include "PathFindService.h"
#include "IServiceContainer.h"
using namespace rkt;
using namespace std;
class PathFindService_Proxy : public IPathFindService
{
public:
	// IPathFindService
	//////////////////////////////////////////////////////////////////////////
    virtual bool    GetPath(Vector3 const& v3Start, Vector3 const& v3End, vector<Vector3>& vecRet, int nFlag = 0x0, int nCamp = 0x0, float fWidthMin = 0);

    virtual bool	getAroundPos(Vector3* v3Pos, float fRadius,vector<Vector3>& vecRet);

    virtual bool	isObstacle(Vector3* v3Pos);

    virtual bool	getHeight(Vector3* v3Pos, float& fHeight);

    virtual Vector3 getRaycast(const Vector3& startPos, const Vector3& rayDir, float dist);

    virtual bool    addObstacle(UID uid, void* oInfo, unsigned char camp);

    virtual bool    removeObstacle(UID uid);

    virtual bool    checkObstacle(const float *pos, const float *verts, const int &nVerts, int nFlag = 0x0, int nType = 0x0);
    virtual Vector3 checkObstacle4Spell(const Vector3& startPos, const Vector3& rayDir, float dist, int nCamp = 0x0, int nFlag = 0x0);
    virtual bool    startDynamicObstacleProxy();
	virtual bool    hasCHF() { return m_pRealService->hasCHF(); };

	virtual void release() {};
public:
	// 构造函数
	PathFindService_Proxy(PathFindService*  pRealService):m_nServiceType(GAMELOGIC_SERVICE_FINDPATH),m_pRealService(pRealService),m_pContainer(0){}
	// 析构函数
	~PathFindService_Proxy();

	// 取得服务类型
	BYTE getServiceType();

public:
	IServiceContainer*      m_pContainer;
	BYTE			    m_nServiceType;		// 游戏逻辑服务类型 GAMELOGIC_SERVICE_TYPE
    PathFindService*    m_pRealService;

//private:
//	void PostMsg(obuf256& oMsg);
//动态障碍
public:
	/** 创建动态障碍
	@param uid: 实体UID
	@param min: 最小点（左下方）
	@param max: 最大点（右上方）
	@param isPreload: 是否预加载
	@param camp: 阵营
	@param nFlag: 区域ID Area_Flag
	@param nShapeType: 形状 Obstacle_Shape
	@return             : 返回所能到达的点，如果射线击中某个障碍，则返回击中点
	*/
	virtual int		addDynamicObstacle(UID uid, void* oInfo, unsigned char camp);

	/** 移除动态障碍
	@param obstacleRef: 实体UID
	*/
	virtual int		removeDynamicObstacle(UID uid);
};