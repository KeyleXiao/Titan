/*******************************************************************
** 文件名:	AOI_MapForest.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - All Rights Reserved
** 创建人:	彭璐
** 日  期:	3/24/2015
** 版  本:	1.0
** 描  述:	地图的草丛类
********************************************************************/

#pragma once

/*
*	1. 草丛系统不一定都是草丛，也可能是树木，或者其他3D可遮挡的东西
*	2. 由策划配置哪些区域为草丛、客户端和服务端分别读取此配置文件
*	3. 玩家未进入草丛之前，是看不到草丛区域内的任何物件的
*	4. 玩家进入草丛之后，可看到视野内的草丛里的物件（也可看到视野内草丛外的物件）
*	5. 同阵营的玩家视野是共享的
*   注意：一个地图可以创建N个战场，因此地图中草丛的一些静态数据只保留一份即可
*/

#include "AOI_Forest.h"
#include <map>
using std::map;


/*  AOI_MapForestFactory 工厂采用以下方式来封装:
*   1. 外界只可以通过工厂类提供的函数createMapForest来创建AOI_MapForest对象
*   2. 通过对象指针访问接口函数
*   3. 对象指针->release() 即可释放
*   这样封装的好处:
*       1> 接口简单
*       2> 外界一旦出现非法调用，编译就会报错
*/
class AOI_MapForestFactory;

class AOI_MapForest
{
    friend class AOI_MapForestFactory;

public:
    // 根据坐标获取草丛ID
    int getForestID( int nX,int nY );

    void release();

private:
	AOI_MapForest() 
		: m_nMapID(0)
		, m_nWidth(0)
		, m_nHeight(0)
		, m_bitmap(NULL)
        , m_nRef(0)
	{
	}

    ~AOI_MapForest()
    {
        if (m_bitmap) delete [] m_bitmap;
    }

    /** 从文件里面加载，直接生成位图
    @name				: 
    @param	nMapID		: 静态地图ID
    @param	nWidth		: 静态地图宽度
    @param	nHeight		: 静态地图高度
    @param	filename    : bitmap文件名
    @return
    */
	bool load( int nMapID,int nWidth,int nHeight,const char * filename="MapForest.bmp" );

private:
    enum
    {
        FOREST_NULL = 0,
        FOREST_RED,
        FOREST_GREEN,
        FOREST_BLUE,
    };

    enum
    {
        COLORREF_RED	= RGB(255,0,0),
        COLORREF_GREEN	= RGB(0,255,0),
        COLORREF_BLUE	= RGB(0,0,255),
    };

    // 根据颜色的RGB值返回对应的草丛ID号
    inline BYTE getForestID( DWORD dwColor )
    {
        switch (dwColor)
        {
        case COLORREF_RED:		return FOREST_RED;
        case COLORREF_GREEN:	return FOREST_GREEN;
        case COLORREF_BLUE:		return FOREST_BLUE;
        default:				return FOREST_NULL;
        }
    }

private:
	int						m_nMapID;
	int						m_nWidth;
	int						m_nHeight;
	BYTE*					m_bitmap;		// 草丛的位图信息,可以直接通过坐标检索到哪个草丛
    int                     m_nRef;         // 静态地图草丛对象的引用计数
};

class AOI_MapForestFactory : public Singleton<AOI_MapForestFactory>
{
    friend class AOI_MapForest;

public:
    /** 根据静态地图ID创建地图草丛对象
    @name				: 
    @param				: 
    @return
    */
    AOI_MapForest* createMapForest( int nMapID,int nWidth,int nHeight );

private:
    /** 根据静态地图ID释放地图草丛对象
    @name				: 
    @param				: 
    @return
    */
    void releaseMapForest( int mapid );

private:
    typedef map< int, AOI_MapForest* > T_FORESTS ;
    T_FORESTS m_forests;
};
