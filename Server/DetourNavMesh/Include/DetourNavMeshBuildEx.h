#pragma once
#include "DetourEx.h"
#include "DetourNavMeshBuilder.h"

struct rcnNavMeshCreateParams;
struct rcnNavMeshSetHeader;
struct rcnNavMeshTileHeader;
struct rcnTileData;


DT_API bool dtnmBuildTileData(rcnNavMeshCreateParams* params, rcnTileData* resultData);

