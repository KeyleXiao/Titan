/*******************************************************************
** 文件名:	AOI_Forest.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - All Rights Reserved
** 创建人:	彭璐
** 日  期:	3/24/2015
** 版  本:	1.0
** 描  述:	DOTA类AOI之丛林类
********************************************************************/

#pragma once

#include <map>

/*
*   通过一张BITMAP位图，描述一个地图的丛林掩码(位图格式为8*width*height)
*	1. 草丛系统不一定都是草丛，也可能是树木，或者其他3D可遮挡的东西
*	2. 由策划配置哪些区域为草丛、客户端和服务端分别读取此配置文件
*	3. 玩家未进入草丛之前，是看不到草丛区域内的任何物件的
*	4. 玩家进入草丛之后，可看到视野内的草丛里的物件（也可看到视野内草丛外的物件）
*	5. 同阵营的玩家视野是共享的
*   注意：一个地图可以创建N个战场，因此地图中草丛的一些静态数据只保留一份即可
*/

class SceneManager;
struct ISingleMapArea;

class AOI_Forest
{
    friend class SceneManager;

    int m_nMapID;
    ISingleMapArea * m_pMapArea;

    AOI_Forest() : m_nMapID( 0 ), m_pMapArea( NULL )
    {
        
    }

    ~AOI_Forest()
    {

    }

public:
	bool Load( int mapID );

	int GetForestID( float fLocX, float fLocY );
};

// 草丛阵营可见性管理
template<typename OBJECT_TYPE>
class ForestVisible
{
	struct ForestCampData
	{
		int* pObjCount;   // 每个阵营在草丛中的实体个数

        ForestCampData() : pObjCount(NULL){}
	};

	std::map<int,ForestCampData>   m_ForestVisible;
    int                            m_nCampCount;
public:
    ForestVisible( int nCampCount ) : m_nCampCount(nCampCount){}

    ~ForestVisible()
    {
        for ( std::map<int,ForestCampData>::iterator it=m_ForestVisible.begin();it!=m_ForestVisible.end();++it )
        {
            if ( it->second.pObjCount!=NULL )
            {
                free(it->second.pObjCount);
            }
        }
        m_ForestVisible.clear();
    }

	// 有对象进入草丛
	void OnEnterForest( OBJECT_TYPE obj,int nForestID )
	{
		int camp = obj->m_nCamp;
		if ( camp<0 || camp>=m_nCampCount )
			return;

		int oldForest = obj->m_nForestID;
		if ( oldForest==nForestID )
			return;

		if ( oldForest>=0 )
		{
			OnLeaveForest( obj );
			OnEnterForest( obj,nForestID );
			return;
		}

		obj->m_nForestID = nForestID;

		if ( nForestID>=0 )
		{
			ForestCampData & data = m_ForestVisible[nForestID];
            if ( data.pObjCount==NULL )
            {
                data.pObjCount = (int*)malloc( sizeof(int)*m_nCampCount );
                memset( data.pObjCount,0,sizeof(int)*m_nCampCount );
            }
			++data.pObjCount[camp];
		}
	}

	// 有对象进入草丛
	void OnLeaveForest( OBJECT_TYPE obj )
	{
		int camp = obj->m_nCamp;
		if ( camp<0 || camp>=m_nCampCount )
			return;

		int nForestID = obj->m_nForestID;
		if ( nForestID>=0 )
		{
			obj->m_nForestID = -1;
			ForestCampData & data = m_ForestVisible[nForestID];

            if ( data.pObjCount==NULL )
                return;

			--data.pObjCount[camp];
		}
	}

	// 某个草丛是否对某个阵营可见
	bool isVisible( int nForestID,int nCamp )
	{
		if ( nCamp<0 || nCamp>=m_nCampCount )
			return false;

		ForestCampData & data = m_ForestVisible[nForestID];

        if ( data.pObjCount==NULL )
            return false;

		return data.pObjCount[nCamp]>0;
	}
};
