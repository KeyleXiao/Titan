#pragma once
#include "DetourEx.h"
#include "DetourNavMeshBuilder.h"

struct rcnNavMeshCreateParams;
struct rcnNavMeshSetHeader;
struct rcnNavMeshTileHeader;
struct rcnTileData;


	EXPORT_API bool dtnmBuildTileData(rcnNavMeshCreateParams* params, rcnTileData* resultData);


	EXPORT_API void dtnmFreeBytes(unsigned char** data);

