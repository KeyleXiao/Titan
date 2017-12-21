
#pragma once
#include"stdlib.h"
#include<string.h>

class NavBroadcaster
{
public:
	NavBroadcaster();
	//���navչʾ����
	void getNewNavMeshShow(float* verts,int tileID) {

		if(m_meshVertsShow[tileID] && verts){
			memcpy(verts, m_meshVertsShow[tileID],m_meshVertsShowSize[tileID]);
		}
	}
	//���tile������
	int getNavMeshMaxTile(){ return maxTiles;}
	//���navmeshTile�Ĵ�С
	int getNavMeshTileSize(int tileID){ return m_meshVertsShowSize[tileID];}
	
	//��ʼ��navmesh�㲥����������
	void InitNavBroadcastData(int maxTiles);
	//����navmesh�㲥������
	void SetNavBroadcastData(void* pData,int size);
	//��ÿͻ�����������
	void GetNavMeshBroadcast();

private:
	float**                     m_meshVertsShow;
	int*						m_meshVertsShowSize;
	int maxTiles;
};