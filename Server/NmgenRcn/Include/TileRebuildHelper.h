#pragma once
#include <string.h>
#include "NMGen.h"
#include "RecastAlloc.h"

struct nmgPolyMeshDetail_Self
	: rcPolyMeshDetail
{
	int maxmeshes;
	int maxverts;
	int maxtris;
	unsigned char resourcetype;
};

bool  checkCellInCHF(const rcCompactHeightfield* chf)
{
	if (chf->cells != nullptr)
	{
		return true;
	}
	return false;
}

bool  checkContoursInCSET(const rcContourSet* cset)
{
	if (cset->conts != nullptr)
	{
		return true;
	}
	return false;
}

bool  checkPolysInMESH(const rcPolyMesh* mesh)
{
	if (mesh->polys != nullptr)
	{
		return true;
	}
	return false;
}

bool  checkMeshInDM(const nmgPolyMeshDetail_Self* mesh)
{
	if (mesh->meshes != nullptr)
	{
		return true;
	}
	return false;
}


bool  checkCHF(const rcCompactHeightfield* chf)
{
	if (chf == nullptr || !checkCellInCHF(chf))
		return false;
	else if (chf->spanCount < 1)
		return false;
	return true;
}

bool  checkContours(const rcContourSet* cset)
{
	if (cset == nullptr || !checkContoursInCSET(cset))
		return false;
	else if (cset->nconts < 1)
		return false;
	return true;
}

bool  checkPolyMesh(const rcPolyMesh* polyMesh)
{
	if (polyMesh == nullptr || !checkPolysInMESH(polyMesh))
		return false;
	else if (polyMesh->npolys < 1)
		return false;
	return true;
}

bool checkDetailPolyMesh(const rcPolyMesh* polyMesh,const nmgPolyMeshDetail_Self* detailMesh)
{
	if (detailMesh == nullptr || !checkMeshInDM(detailMesh))
		return false;
	else if (polyMesh->npolys != detailMesh->nmeshes)
		return false;
	return true;
}


int getMaxVerts_Self(rcPolyMesh& mesh)
{
	int maxIndex = 0;
	for (int i = 0; i < mesh.npolys; i++)
	{
		int p = i * 2 * mesh.nvp;
		for (int j = 0; j < mesh.nvp; j++)
		{
			int index = mesh.polys[p + j];
			if (index == RC_MESH_NULL_IDX)
				break;
			maxIndex = rcMax(index, maxIndex);
		}
	}

	return maxIndex + 1;
}