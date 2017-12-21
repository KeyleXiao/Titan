#pragma once


#ifdef MAPPATHFIND_EXPORTS
#define PATH_EXPORT		__declspec(dllexport)
#define PATH_API		extern "C" __declspec(dllexport)
#else
#define PATH_IMPORT		__declspec(dllimport)
#define PATH_API		extern "C" __declspec(dllimport)
#endif

struct PathFindManager;
 
PATH_API void getNavMesh(float* verts,int tileID);

PATH_API int getNavMeshMaxTile();

PATH_API int getNavMeshVertSize( int tileID);
