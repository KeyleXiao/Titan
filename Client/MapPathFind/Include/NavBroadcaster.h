
#pragma once
#include"stdlib.h"
#include<string.h>

class NavBroadcaster
{
public:
	NavBroadcaster();
	//获得nav展示数据
	void getNewNavMeshShow(float* verts,int tileID) {

		if(m_meshVertsShow[tileID] && verts){
			memcpy(verts, m_meshVertsShow[tileID],m_meshVertsShowSize[tileID]);
		}
	}
	//获得tile的数量
	int getNavMeshMaxTile(){ return maxTiles;}
	//获得navmeshTile的大小
	int getNavMeshTileSize(int tileID){ return m_meshVertsShowSize[tileID];}
	
	//初始化navmesh广播的数据容器
	void InitNavBroadcastData(int maxTiles);
	//设置navmesh广播的数据
	void SetNavBroadcastData(void* pData,int size);
	//获得客户端网格数据
	void GetNavMeshBroadcast();

private:
	float**                     m_meshVertsShow;
	int*						m_meshVertsShowSize;
	int maxTiles;
};