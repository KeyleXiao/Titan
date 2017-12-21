#include "TileRebuildHelper.h"

extern "C"
{

    //�ܽӿ�--ϸ�ֶ����
	NMGEN_API int trbRebuildTile(
        //build region params:
        const int borderSize, const int minRegionArea, const int mergeRegionArea
        //build contours params��
        , const float maxError, const int maxEdgeLen, const int flags
        //build polys params��
        , const int maxVertsPerPoly, int* maxVerts
        //build detail mesh params��
        , const float sampleDist, const float sampleMaxError
        //output data params��
        , rcCompactHeightfield* chf
        , rcContourSet* cset
        , rcPolyMesh* mesh
        , nmgPolyMeshDetail_Self* dmesh);

	//��ȸ���CHF
	NMGEN_API void trbDeepCopyCHF(const rcCompactHeightfield* chf
		, rcCompactHeightfield* result
		, int* dataSize);

	//�ӿ�--�޸�CHF-����
    NMGEN_API int trbMarkBoxArea(const float* bmin
        , const float* bmax
		, float angle
        , unsigned int areaId
        , rcCompactHeightfield* chf, unsigned int* flagList);

	//�ӿ�--�޸�CHF-Բ����
	NMGEN_API int trbMarkCylinderArea(const float* pos
		, const float radius, const float height
		, unsigned int areaId
		, rcCompactHeightfield* chf, unsigned int* flagList);


	//�ӿ�--��������
    NMGEN_API int trbBuildRegions(rcCompactHeightfield* chf, const int borderSize
        , const int minRegionArea, const int mergeRegionArea);

	//�ӿ�--����������
    NMGEN_API int trbBuildSet(rcCompactHeightfield* chf
        , const float maxError, const int maxEdgeLen, rcContourSet* cset, const int flags);

	//�ӿ�--��������
    NMGEN_API int trbBuildFromContourSet(rcContourSet* cset
        , const int nvp, rcPolyMesh* mesh, int* maxVerts);

	//�ӿ�--ϸ�ֶ����
    NMGEN_API int trbBuildPolyMeshDetail(const rcPolyMesh* mesh
        , rcCompactHeightfield* chf
        , const float sampleDist
        , const float sampleMaxError
        , nmgPolyMeshDetail_Self* dmesh);

	NMGEN_API void trbFreeNMGPolyMeshDetail(nmgPolyMeshDetail_Self* dmesh);

	NMGEN_API void* trbAlloc(int size, rcAllocHint hint);

	NMGEN_API void trbFree(void* ptr);
}

