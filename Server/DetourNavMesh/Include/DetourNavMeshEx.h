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
#ifndef CAI_DETOURNAVMESHEX_H
#define CAI_DETOURNAVMESHEX_H

#include "DetourEx.h"
#include "DetourNavMeshBuilder.h"

struct rcnTileData
{
    unsigned char* data;
    int dataSize;
    bool isOwned;
};

extern "C"
{
	DT_API dtTileRef dtnmGetTileRefAt(const dtNavMesh* navMesh
		, int x
		, int y
		, int layer);

	DT_API dtStatus dtnmAddTile(dtNavMesh* navMesh
		, rcnTileData* tileData
		, dtTileRef lastRef
		, dtTileRef* resultRef);

	DT_API dtStatus dtnmRemoveTile(dtNavMesh* navMesh
		, dtTileRef ref
		, unsigned char** data
		, int* dataSize);

	DT_API int dtnmGetMaxTiles(const dtNavMesh* pNavMesh);

	DT_API const dtMeshTile* dtnmGetTile(const dtNavMesh* navmesh
		, int index);

	DT_API const dtMeshHeader* dtnmGetTileHeader(const dtMeshTile* tile);

	DT_API int dtnmGetTileVerts(const dtMeshTile* tile
		, float* verts
		, const int vertsCount);

	DT_API int dtnmGetTilePolys(const dtMeshTile* tile
		, dtPoly* polys
		, const int polysSize);

	DT_API int dtnmGetTileDetailVerts(const dtMeshTile* tile
		, float* verts
		, const int vertsCount);

	DT_API int dtnmGetTileDetailTris(const dtMeshTile* tile
		, unsigned char* tris
		, const int trisSize);

	DT_API int dtnmGetTileDetailMeshes(const dtMeshTile* tile
		, dtPolyDetail* detailMeshes
		, const int meshesSize);

}
#endif