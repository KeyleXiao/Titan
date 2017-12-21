/*
 * Copyright (c) 2011 Stephen A. Pratt
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef CAI_DETOUREX_H
#define CAI_DETOUREX_H

#include "DetourExport.h"
#include "DetourNavMesh.h"

static const int MAX_RCN_PATH_CORRIDOR_SIZE = 256;
struct rcnPathCorridorData
{
    float position[3];
    float target[3];

    dtPolyRef path[MAX_RCN_PATH_CORRIDOR_SIZE];
    int pathCount;
};

struct rcnNavmeshPoint
{
	dtPolyRef polyRef;
	float point[3];
};

class dtTileCache;
class dtNavMeshQuery;
class dtQueryFilter;
class dtProximityGrid;

DT_API dtNavMesh* dtmAllocNavMesh();

DT_API void dtmFreeNavMesh(dtNavMesh* navmesh);

DT_API dtProximityGrid* dtmAllocProximityGrid();

DT_API void dtmFreeProximityGrid(dtProximityGrid* ptr);

// 动态添加多边形
DT_API dtStatus dtnmAddDynamicPolys(dtNavMesh* pNavMesh, 
    float* verts, 
    int nverts, 
    dtPolyRef& polyref,
    int nFlag);

// 动态删除多边形
DT_API dtStatus dtnmRemoveDynamicPolys(dtNavMesh* pNavMesh,
    dtPolyRef& polyref);

// 动态障碍技能判断
DT_API dtStatus dtnmCheckObstacle4Spell(dtNavMesh* pNavMesh,
    const float *fStartPos,
    const float *fDestPos,
    float *fInterPos,
    dtPolyRef obsRef,
    int nFlag);

// 检测指定地点是否有障碍多边形
DT_API dtStatus dtnmCheckObstacle(dtNavMesh* pNavMesh,
    const float *pos,
    const float *verts,
    const int nVerts,
    dtPolyRef obsRef,
    int nFlag,
    int nType);

DT_API dtStatus dtnmBuildDTNavMeshFromRaw(const unsigned char* data
	, int dataSize
	, bool safeStorage
	, dtNavMesh** ppNavMesh);

DT_API dtStatus dtnmBuildDTNavMeshFromRaw(const unsigned char* data
	, int dataSize
	, bool safeStorage
	, dtNavMesh** ppNavMesh
	, dtTileCache*& pTileCache 
	);

DT_API dtNavMeshQuery* dtqAllocNavMeshQuery();

DT_API void dtqFreeNavMeshQuery(dtNavMeshQuery* query);

DT_API dtStatus dtnqBuildDTNavQuery(dtNavMesh* pNavMesh
	, const int maxNodes
	, dtNavMeshQuery** ppNavQuery);

DT_API dtStatus dtqFindNearestPoly(dtNavMeshQuery* query
	, const float* center
	, const float* extents
	, const dtQueryFilter* filter
	, rcnNavmeshPoint* nearest);

DT_API dtStatus dtqFindStraightPath(dtNavMeshQuery* query
	, const float* startPos
	, const float* endPos
	, const dtPolyRef* path
	, const int pathStart
	, const int pathSize
	, float* straightPath
	, unsigned char* straightPathFlags
	, dtPolyRef* straightPathRefs
	, int* straightPathCount
	, const int maxStraightPath);

DT_API dtStatus dtqFindPathExt(dtNavMeshQuery* query 
	, rcnNavmeshPoint* startPos
	, rcnNavmeshPoint* endPos
	, const float* extents
	, const dtQueryFilter* filter
	, dtPolyRef* path
	, int* pathCount
	, const int maxPath
    , int nFlag);

DT_API dtStatus dtqMoveAlongSurface(dtNavMeshQuery* query
	, rcnNavmeshPoint startPos
	, const float* endPos
	, const dtQueryFilter* filter
	, float* resultPos
	, dtPolyRef* visited
	, int* visitedCount
	, const int maxVisitedSize);

DT_API dtStatus dtqRaycast(dtNavMeshQuery* query
    , rcnNavmeshPoint startPos
    , const float* endPos
    , const dtQueryFilter* filter
    , float* t
    , float* hitNormal
    , dtPolyRef* path
    , int* pathCount
    , const int maxPath);

#endif
