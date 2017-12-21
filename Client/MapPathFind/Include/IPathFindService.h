/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\MapPathFind\Include\IPathFindService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	3/12/2015	16:22
** 版  本:	1.0
** 描  述:	
            寻路服务提供的接口
********************************************************************/

#pragma once

// 导入导出标志
#ifdef MAPPATHFIND_EXPORTS
#	define CLASS_API	__declspec(dllexport)
#else
#	define CLASS_API	__declspec(dllimport)
#endif

// for addObstacle
//const unsigned int INVALID_POLYGON_REF = 0;
//struct __IEntity;
#include "PathFindDef.h"
class Vector3;

// 障碍形状类型
enum Obstacle_Shape
{
    Cube,           /* 方体 */ 
    Cylinder,       /* 圆柱体 */
};
typedef unsigned int ObstacleRef;


struct  IPathFindService
{
	// 获取 v3Start->v3End 的路径拐点
	// fWidthMin 路径最小宽度
	// ret vecRet
	virtual bool	            GetPath(Vector3 const& v3Start, Vector3 const& v3End, vector<Vector3>& vecRet, int nFlag = 0x0, int nCamp = 0x00, float fWidthMin = 0) = 0;

	// v3Pos为中心到边距离fRadius的正方形区域内 步进1 vecRet非障碍点
	virtual bool	            getAroundPos(Vector3* v3Pos, float fRadius,vector<Vector3>& vecRet) = 0;

	/** 判断指定点是否有障碍物
    @name				: 
    @param				: 坐标点
    @return
    */
	virtual bool	            isObstacle(Vector3* v3Pos) = 0;

	/** 求指定坐标点的贴地高度
    @name				: 
    @param				: 坐标点
    @return
    */
	virtual bool	            getHeight(Vector3* v3Pos, float& fHeight) = 0;

    /** 求得射线击中的点
    @name				: 
    @param				: 起始点
    @param				: 射线向量
    @param				: 距离
    @return             : 返回所能到达的点，如果射线击中某个障碍，则返回击中点
    */
    virtual Vector3             getRaycast(const Vector3& startPos, const Vector3& rayDir, float dist) = 0;

    ///////////////动态障碍物///////////////////////////////////////////////////////////
    //* 添加障碍物
    /*
    @name				: 
    @param		pos	    : 障碍物添加动作执行坐标位
    @param		verts	: 障碍多边形顶点坐标
    @param		nVerts	: 障碍多边形顶点数
    @return     flag    : 标志位
    @demonstration      :

    float pos[3] = {0.0f};
    float verts*[4*3] = {0.0f}; // 障碍物多边形的顶点，假设为一个四边形
    int nVerts = 4;             // 障碍物多边形顶点数，4个顶点
    unsinged int flag = 0x01;            // 障碍物作用标志位,假设此时仅对对面阵营有效

    if (!addObstacle(pos, verts, nVerts, flag))
    {
        Errorln("addObstacle failed !!! "); // 当添加障碍物失败打印信息
    }
    */

	/** 创建障碍
	@param uid: 实体UID
	@param oInfo:碰撞体信息
	@param camp: 阵营
	@return             : 返回所能到达的点，如果射线击中某个障碍，则返回击中点
	*/
    virtual bool                addObstacle(UID uid, void* oInfo, unsigned char camp) = 0;

	/** 移除指定障碍物
    @param uid : 实体UID
    @return
    */
    virtual bool                removeObstacle(UID uid) = 0;

    /// 检测指定地点是否有障碍多边形
    ///  @param[in]	    verts	输入的清除障碍多边形清除物的顶点
    ///  @param[in]	    nverts	输入的清除障碍多边形清除物的顶点数量
    ///  @param[in]	    nFlag	添加的障碍多边形的标识
    ///  @param[in]	    nType	检测类型，CheckSinglePt为检测单点，CheckMulitPt为检测多边形
    ///  @return The status	flags for the operation.
    ///  const int CheckSinglePt = 1;
    ///  const int CheckMulitPt = 2;
    virtual Vector3             checkObstacle4Spell(const Vector3& startPos, const Vector3& rayDir, float dist, int nCamp = 0x0, int nFlag = 0x0) = 0;
    virtual bool                checkObstacle(const float *pos, const float *verts, const int &nVerts, int nFlag = 0x0, int nType = 0x0) = 0;
    
    /* 开始注册怪物死亡事件，添加障碍物
    @name				: 
    @return             : 开启动态障碍代理成功或者失败
    */
    virtual bool                startDynamicObstacleProxy() = 0;

    /** 创建动态障碍
    @param uid: 实体UID
	@param oInfo:碰撞体信息 
    @param camp: 阵营
    @return             : 返回所能到达的点，如果射线击中某个障碍，则返回击中点
    */
	virtual int                 addDynamicObstacle(UID uid, void* oInfo, unsigned char camp) = 0;

    /** 移除动态障碍
    @param obstacleRef: 实体UID
    */
	virtual int	                removeDynamicObstacle(UID uid) = 0;

	bool isInit;
	virtual bool hasCHF() = 0;
    //////////////////////////////////////////////////////////////////////////

};
