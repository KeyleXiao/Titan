#include "TileRebuildHelper.h"

extern "C"
{

    //总接口--细分多边形
	NMGEN_API int trbRebuildTile(
        //build region params:
        const int borderSize, const int minRegionArea, const int mergeRegionArea
        //build contours params：
        , const float maxError, const int maxEdgeLen, const int flags
        //build polys params：
        , const int maxVertsPerPoly, int* maxVerts
        //build detail mesh params：
        , const float sampleDist, const float sampleMaxError
        //output data params：
        , rcCompactHeightfield* chf
        , rcContourSet* cset
        , rcPolyMesh* mesh
        , nmgPolyMeshDetail_Self* dmesh);

	//深度复制CHF
	NMGEN_API void trbDeepCopyCHF(const rcCompactHeightfield* chf
		, rcCompactHeightfield* result
		, int* dataSize);

	//接口--修改CHF-方体
    NMGEN_API int trbMarkBoxArea(const float* bmin
        , const float* bmax
		, float angle
        , unsigned int areaId
        , rcCompactHeightfield* chf, unsigned int* flagList);

	//接口--修改CHF-圆柱体
	NMGEN_API int trbMarkCylinderArea(const float* pos
		, const float radius, const float height
		, unsigned int areaId
		, rcCompactHeightfield* chf, unsigned int* flagList);


	//接口--划分区域
    NMGEN_API int trbBuildRegions(rcCompactHeightfield* chf, const int borderSize
        , const int minRegionArea, const int mergeRegionArea);

	//接口--构造轮廓线
    NMGEN_API int trbBuildSet(rcCompactHeightfield* chf
        , const float maxError, const int maxEdgeLen, rcContourSet* cset, const int flags);

	//接口--构造多边形
    NMGEN_API int trbBuildFromContourSet(rcContourSet* cset
        , const int nvp, rcPolyMesh* mesh, int* maxVerts);

	//接口--细分多边形
    NMGEN_API int trbBuildPolyMeshDetail(const rcPolyMesh* mesh
        , rcCompactHeightfield* chf
        , const float sampleDist
        , const float sampleMaxError
        , nmgPolyMeshDetail_Self* dmesh);

	NMGEN_API void trbFreeNMGPolyMeshDetail(nmgPolyMeshDetail_Self* dmesh);

	NMGEN_API void* trbAlloc(int size, rcAllocHint hint);

	NMGEN_API void trbFree(void* ptr);
}

