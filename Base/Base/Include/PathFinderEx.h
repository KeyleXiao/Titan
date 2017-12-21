/*******************************************************************
** 文件名:	PathFinderEx.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	2008/04/17
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __PATH_FINDER_EX_H__
#define __PATH_FINDER_EX_H__

#include "PathFinder.h"
#include <assert.h>
#include <functional>
#include <vector>
#include <map>
#include <hash_map>

/**
@name : 路径搜索模块
@brief: 功能:
			提供一份地图数据,该数据维护了地图中所有Tile的信息，例如哪些Tile是可通行的，哪些Tile是阻挡的。
			然后传入起点和终点坐标,该模块负责搜索从起点到终点的路径。
		算法:
			搜索路径最常用的算法是A*算法,但是出于性能考虑,这里采用一种更简单的方法实现,
			该方法的基本思路是,首先总是尝试直接向终点逼近，当遇到阻挡的Tile时沿着阻挡Tile绕转，直到重新回到直路上
			经过这个过程最终总是可以找到从起点到终点的有效路径，虽然这条路径不是最短的.
			最后一个步骤会对这条路径进行优化，使之看起来更自然
		修改:
		    由于使用环绕方式搜索到的路径常常效果不好，有些特殊的路径也很难搜索到，所以我们又额外实现一份以A*最短算法
			搜路的版本，当环绕方式搜索不到时启动A*搜索,保证搜索速度的同时兼顾搜索准确度
*/

#ifndef WIN32
	struct POINT    // 如果不是windows环境则定义一下POINT结构
	{ 
		long x; 
		long y;
	};
#endif

//// 搜路选项
//enum
//{
//	SEARCH_OPTION_EMPTY      =0x00   , // 最精简的搜路
//	SEARCH_OPTION_OPTIMIZE   =0x01   , // 优化路径
//	SEARCH_OPTION_NEAREST    =0x02   , // 如果到不了最终点则移动到最近的点
//	SEARCH_OPTION_STRAIGHTWAY=0x04   , // 启动直路判断
//	SEARCH_OPTION_ASTAR      =0x08   , // 当精简搜路搜索不到时启用A*搜路
//	SEARCH_OPTION_ONLY_ASTAR =0x10   , // 只启用A*搜路
//	SEARCH_OPTION_LONGFIND	 =0x20   , // 对于一些特殊的地图，做些特别的搜索
//};

//////////////////////////////////////////////////////////////////////////
/**
@name : 搜路模板
@brief: 该搜路模板基于地图Tile实现,依赖一个判断地图Tile是否阻挡的函数
        使用方法:
		class CMap
		{
		public:
		    bool IsBlock(POINT pt);   // 必须实现IsBlock方法,如果名字不叫IsBlock可以在SetMapInfo时修改
		....
		};

		CMap m_Map;
		PathFinderEx<CMap>  m_PathFinder;
		m_PathFinder.SetMapInfo(m_Map.GetWidth(),m_Map.GetHeight(),&m_Map,mem_fun1(&CMap::IsBlock));
		POINT * pPath = 0;
		int nPathLen  = 0;
		if ( m_PathFinder.FindPath(ptStart,ptEnd,pPath,nPathLen) )
*/
//////////////////////////////////////////////////////////////////////////

template
< 
	class    _Map,                                                  // 地图 
	typename _BlockTestFunc = std::mem_fun1_t<bool,_Map,POINT> ,    // 阻挡判断函数对象,函数型如bool IsBlock(POINT pt)
	int      MAX_PATH_LEN   = 1024 ,                                // 最大路径长度
	int      MAX_SEARCH_AREA= 512                                   // 最大搜索面积
>
class PathFinderEx
{
public:
	struct TILE_TAG;

	enum
	{
		DIR_EAST    =0 ,  // 方向定义 : 东  (angle:0)
		DIR_EAST_NORTH ,  // 方向定义 : 东北(angle:45)
		DIR_NORTH      ,  // 方向定义 : 北  (angle:90)
		DIR_WEST_NORTH ,  // 方向定义 : 西北(angle:135)
		DIR_WEST       ,  // 方向定义 : 西  (angle:180)
		DIR_WEST_SOUTH ,  // 方向定义 : 西南(angle:225)
		DIR_SOUTH      ,  // 方向定义 : 南  (angle:270)
		DIR_EAST_SOUTH ,  // 方向定义 : 东南(angle:315)
		DIR_MAX,
	};

	enum
	{
		CORNER_LEFT_TOP = 0,           // 左上角拐点
		CORNER_LEFT_BOTTOM ,           // 左下角拐点
		CORNER_RIGHT_TOP   ,           // 右上角拐点
		CORNER_RIGHT_BOTTOM,           // 右下角拐点
		CORNER_NUM         ,           // 拐点个数

		INVALID_CORNER_FLAG=0xFFFFFFFF,// 无效拐点标志
	};

	enum
	{
		TURN_CW    = 0,   // 顺时针旋转
		TURN_CCW      ,   // 逆时针旋转
	};

	/**
	@name           : 设置地图信息 
	@brief          : 调用FindPath之前请设置地图信息,该函数可以重复调用,所以可以在多个地图上搜索路径
	@param nMapWidth: 地图宽度
	@param nMapHeigh: 地图高度
	@param pMap     : 地图对象指针
	@param isBlock  : 判断地表是否阻挡的函数,如果地图对象判断阻挡的函数是IsBlock则该参数不用传
	@return         : 
	*/
	bool SetMapInfo(unsigned long nMapWidth,unsigned long nMapHeight,_Map * pMap,_BlockTestFunc isBlock=std::mem_fun1(&_Map::IsBlock));

	/**
	@name			: 搜索路径
	@brief			: 
	@param ptStart  : 起点坐标
	@param ptEnd    : 终点坐标
	@param pPathRet : 返回的路径点
	@param nPathLen : 返回路径长度
	@param nOption  : 搜路选项,是否优化路径
	@param nMaxStep : 最多搜索的步数,如果太远就放弃 (这个参数有两个意义 1.有时候只需要搜出最近几步的路,并不一定非要到达目标点 2.默认设为1024,防止出现死循环)
	@return         : 返回搜路是否成功 
	*/
	bool FindPath(const POINT & ptStart,const POINT & ptEnd,POINT *& pPath,int & nPathLen,int nOption=SEARCH_OPTION_OPTIMIZE,int nMaxStep=MAX_PATH_LEN);

	// 构造函数
	PathFinderEx() : m_pMap(0),m_pNearestTile(0),m_nFind(0),m_BlockFunc(0),m_nSearchOption(0),m_nMaxStep(MAX_PATH_LEN),m_nStepCount(0)
	{
		m_TileTags=NULL;
		m_nMapWidth=0;
		m_nMapHeight=0;
		//m_TileTags = new TILE_TAG[MAX_SEARCH_AREA*MAX_SEARCH_AREA];
		//memset(m_TileTags,0,sizeof(TILE_TAG)*MAX_SEARCH_AREA*MAX_SEARCH_AREA);
	}

	// 释构函数
	virtual ~PathFinderEx()
	{
		if ( m_TileTags ) { delete []m_TileTags;m_TileTags = 0;}
	}

protected:
	//////////////////////////////////////////////////////////////////////////
	// 真正搜索路径的函数
	bool FindPathCore(const POINT & ptStart,const POINT & ptEnd);

	// 采用A*算法搜索路径
	bool FindAStar(const POINT & ptStart,const POINT & ptEnd );

	// 沿直路靠近目标点
	bool Approach(const POINT & ptStart,const POINT & ptEnd);

	// 从某点开始绕路
	bool Around(const POINT & ptTile,int nMoveDir,int nTurnDir,const POINT & ptStart,const POINT & ptEnd);

	// 将两个绕来绕去的点尝试直接连起来
	bool ConnectDirect(const POINT & ptStart,const POINT & ptEnd);

	// 获取Tile信息
	inline TILE_TAG * GetTileTag(const POINT & ptTile);

	// 从TAG的指针获得Tile的坐标
	inline void TileToPoint(TILE_TAG * pTag,POINT &ptTile);

	// 判断某个Tile是否被访问过
	inline bool IsTileVisited(TILE_TAG * pTag,int nDir);

	// 标记某个Tile已经被访问过
	inline void MarkTileVisited(TILE_TAG * pTag,int nDir,TILE_TAG * pParent);

	// 取得某方向上下一个点的坐标
	inline void MoveNext(const POINT & ptTile,int dir,POINT & ptNext);

	// 取得两个点之间的方向 
	inline int GetDir(const POINT & ptCur,const POINT & ptNext);

	// 旋转方向
	inline int  TurnDir(int nMoveDir,int nTurnDir);

	// 记录拐点
	inline void RecordCorner(const POINT & ptTile);

	// 连接所有拐点
	inline void ConnectCorners(const POINT & ptStart);

	// 是否是主线上的点
	inline bool IsOnStraightWay(const POINT & ptTile,const POINT & ptStart,const POINT & ptEnd);

	// 是否可以直接逼近
	bool IsCanApproach(const POINT & ptTile,const POINT & ptEnd);

	// 根据走路的Tile,回朔完整路径
	int BuildPath(POINT * pPathBuffer,int nBufSize,bool nReverse,bool optmize=true);

	// 长距离搜路
	bool FindPathForLongDistance(const POINT & ptStart,const POINT & ptEnd, POINT *& pPath,int & nPathLen, int nMaxStep);
protected:
	//////////////////////////////////////////////////////////////////////////
	struct TILE_TAG
	{
		TILE_TAG  *      pParent;		// 上一个Tile指针
		unsigned short   nFindID;       // 当前搜路记号,每次搜路自动加1,这样可以避免每次都清一遍内存,当搜路频繁时每次都memset一下是非常低效的事情

		union
		{
			struct	// 环绕算法使用的数据
			{
				unsigned char    nMoveDir;      // 经过该点时的方向
				bool             bAround;       // 标识绕转的路线,这段路线后面需要进行优化
			};
			struct  // A*算法使用的数据
			{  
				unsigned short   nStep;			// 从起始点到该点的步数	
			};
		};
	};

	struct AROUND_POINT
	{
		POINT            ptTile;
		int              nMoveDir;
		int              nTurnDir;
	};

	TILE_TAG       *     m_TileTags;								  // 用来记录搜路过程中走过的Tile的信息 
	POINT				 m_PathTemp[5][MAX_PATH_LEN];                 // 用来存储返回的路径
	POINT                m_ptStart;                                   // 本次搜路的起始点
	POINT                m_ptEnd;                                     // 本次搜路的结束点
	TILE_TAG  *          m_pNearestTile;                              // 最接近的点s
	unsigned long        m_nMapWidth;                                 // 地图宽度
	unsigned long        m_nMapHeight;                                // 地图高度

	_Map      *          m_pMap;                                      // 地图指针
	_BlockTestFunc       m_BlockFunc;                                 // 阻挡判断函数
	unsigned short       m_nFind;                                     // 当前搜路的标识
	int                  m_nSearchOption;                             // 搜路选项
	int                  m_nMaxStep;                                  // 最大搜路步数
	int                  m_nStepCount;                                // 当前搜索的步数

	std::vector<AROUND_POINT>   m_AroundQueue;                        // 绕转队列
	POINT                       m_Corners[CORNER_NUM];                // 拐点列表
	DWORD                       m_nCornerFlag[CORNER_NUM];            // 拐点标志
};


// 沿直路靠近目标点
#define  TEMPLATE_DECLARE		template<class _Map,typename _BlockTestFunc,int MAX_PATH_LEN,int MAX_SEARCH_AREA>
#define  PATH_FINDER_DECLARE	PathFinderEx<_Map,_BlockTestFunc,MAX_PATH_LEN,MAX_SEARCH_AREA>

//////////////////////////////////////////////////////////////////////////
/**
@name           : 设置地图信息 
@brief          : 调用FindPath之前请设置地图信息,该函数可以重复调用,所以可以在多个地图上搜索路径
@param nMapWidth: 地图宽度
@param nMapHeigh: 地图高度
@param pMap     : 地图对象指针
@param isBlock  : 判断地表是否阻挡的函数,如果地图对象判断阻挡的函数是IsBlock则该参数不用传
@return         : 
*/
//////////////////////////////////////////////////////////////////////////
TEMPLATE_DECLARE
bool PATH_FINDER_DECLARE::SetMapInfo(unsigned long nMapWidth,unsigned long nMapHeight,_Map * pMap,_BlockTestFunc isBlock)
{
	if (m_nMapWidth!=nMapWidth || m_nMapHeight!=nMapHeight)
	{
		if ( m_TileTags ) { delete []m_TileTags;m_TileTags = 0;}
		m_TileTags = new TILE_TAG[nMapWidth*nMapHeight];
		if (!m_TileTags)
			MessageBox(0,"内存不足无法创建寻路模块将暂停寻路功能，请重启客户端","内存不足",0);
		if (m_TileTags)
			memset(m_TileTags,0,sizeof(TILE_TAG)*nMapWidth*nMapHeight);
	}

	m_nMapWidth = nMapWidth;
	m_nMapHeight= nMapHeight;



	m_pMap = pMap;
	m_BlockFunc = isBlock;
	
	return true;
}

//////////////////////////////////////////////////////////////////////////
/**
@name           : 长距搜索距离 
@brief          : 
@param nMapWidth: 地图宽度
@param nMapHeigh: 地图高度
@param pMap     : 地图对象指针
@param isBlock  : 判断地表是否阻挡的函数,如果地图对象判断阻挡的函数是IsBlock则该参数不用传
@return         : 
*/
//////////////////////////////////////////////////////////////////////////
TEMPLATE_DECLARE
bool PATH_FINDER_DECLARE::FindPathForLongDistance(const POINT & ptStart,const POINT & ptEnd, POINT *& pPath,int & nPathLen, int nMaxStep)
{
	POINT ptCenter;
	ptCenter.x = abs((ptStart.x + ptEnd.x)/2);
	ptCenter.y = abs((ptStart.y + ptEnd.y)/2);

	int  dx = abs(ptEnd.x-ptStart.x);
	int  dy = abs(ptEnd.y-ptStart.y);

	pPath = NULL;
	nPathLen = 0;
	
	//ostrbuf strCenter;
	//strCenter <<"开始：("<< ptStart.x << "," << ptStart.y << ")";
	//strCenter <<"结束：("<< ptEnd.x << "," << ptEnd.y << ")";

	bool bXFirst = true;
	int nMax = m_nMapWidth;
	if (dy>dx)
	{
		bXFirst = false;
		nMax = m_nMapHeight;
	}

	for (int nStep=0; nStep<nMax; nStep=nStep+5)
	{
		int nTempStep = nStep;
		for (int n=0; n<=1; n++)
		{
			POINT pt = ptCenter;
			/*if (bXFirst)
			{
				pt.y= 0.0 + (pt.x-ptStart.x)/(ptEnd.x-ptStart.x)*(ptEnd.y-ptStart.y) + ptStart.y;
			}
			else
			{
				pt.x = 0.0 + (pt.y-ptStart.y)/(ptEnd.y-ptStart.y)*(ptEnd.x-ptStart.x) + ptStart.x;
			}*/	

			nTempStep = 0 - nTempStep;
			if (bXFirst)
			{
				pt.y = pt.y+nTempStep;
			}
			else
			{
				pt.x = pt.x+nTempStep;;
			}
			//strCenter <<"("<< pt.x << "," << pt.y << ")";
			if (pt.y<0 || pt.y>m_nMapHeight || pt.x<0 || pt.x>m_nMapWidth || m_BlockFunc(m_pMap,pt) )
			{
			//	strCenter <<"*,";
				continue;
			}
			//strCenter <<"#,";			
			
			POINT* pTempPath =NULL;
			int nOnePathLen = MAX_PATH_LEN;
			int nTwoPathLen = MAX_PATH_LEN;
			int nOption= SEARCH_OPTION_OPTIMIZE;//SEARCH_OPTION_ONLY_ASTAR;

			int nTick = getTickCountEx();

			if (FindPath(ptStart, pt, pTempPath, nOnePathLen, nOption, nMaxStep))
			{
				//if (nOnePathLen<=0 || nOnePathLen>MAX_PATH_LEN || pTempPath[nOnePathLen-1].x!=pt.x || pTempPath[nOnePathLen-1].y!=pt.y)
				if (nOnePathLen<=0 || nOnePathLen>MAX_PATH_LEN )
				{
					continue;
				}

				memcpy(m_PathTemp[4], pTempPath, nOnePathLen*sizeof(POINT));

				if (FindPath(pt, ptEnd, pTempPath, nTwoPathLen, nOption, nMaxStep))
				{
					//if (nTwoPathLen<=0 || nTwoPathLen>MAX_PATH_LEN || nTwoPathLen+nOnePathLen>MAX_PATH_LEN || pTempPath[nTwoPathLen-1].x!=ptEnd.x || pTempPath[nTwoPathLen-1].y!=ptEnd.y)
					if (nTwoPathLen<=0 || nTwoPathLen>MAX_PATH_LEN || nTwoPathLen+nOnePathLen>MAX_PATH_LEN)
					{
						continue;
					}
					else
					{
						memcpy(m_PathTemp[4]+nOnePathLen-1, pTempPath, nTwoPathLen*sizeof(POINT));

						//ostrbuf buf;
						//int nCount = 0;
						//for (int x=0; x<nOnePathLen+nTwoPathLen-1; x++)
						//{
						//	buf <<"("<< m_PathTemp[4][x].x << "," << m_PathTemp[4][x].y << "),";
						//	nCount++;
						//}
						//EmphasisLn("中点数据："<<strCenter.c_str());
						//EmphasisLn("路径" << (getTickCountEx()-nTick) << " 长度="<< nCount << "中点("<< pt.x << "," << pt.y << "),起点" << "("<< ptStart.x << "," << ptStart.y << "),终点" << "("<< ptEnd.x << "," << ptEnd.y << ")," <<"：" << buf.c_str());
						nPathLen = nOnePathLen+nTwoPathLen-1;
						pPath = m_PathTemp[4];
						return true;
					}					
				}
			}	
			
		}
	}

	//EmphasisLn("中点数据："<<strCenter.c_str());
	
	return false;
}

//////////////////////////////////////////////////////////////////////////
/**
@name			: 搜索路径
@brief			: 
@param ptStart  : 起点坐标
@param ptEnd    : 终点坐标
@param pPathRet : 返回的路径点
@param nPathLen : 返回路径长度
@param nOption  : 搜路选项,是否优化路径
@return         : 返回搜路是否成功 
*/
//////////////////////////////////////////////////////////////////////////
TEMPLATE_DECLARE
bool PATH_FINDER_DECLARE::FindPath(const POINT & ptStart,const POINT & ptEnd,POINT *& pPath,int & nPathLen,int nOption,int nMaxStep)
{
	pPath = 0;
	nPathLen = 0;

	m_nSearchOption = nOption;
	m_nMaxStep      = nMaxStep;

	int  nPathLenTemp[4];
    POINT * pPathTemp[4];

	int  nShortest  = -1;
	bool bFindMore  = true;
	int  dx = abs(ptEnd.x-ptStart.x);
	int  dy = abs(ptEnd.y-ptStart.y);
    int  nPathLenLimit = min(dx,dy) + abs(dx-dy)+5;

	//////////////////////////////////////////////////////////////////////////
	// 启用常距离搜路
	if (SEARCH_OPTION_LONGFIND & nOption)
	{
		return FindPathForLongDistance(ptStart, ptEnd, pPath, nPathLen, nMaxStep);
	}

	//////////////////////////////////////////////////////////////////////////
	// 只启用A*
	if ( SEARCH_OPTION_ONLY_ASTAR & nOption )
	{
		FindAStar(ptStart,ptEnd);
		nPathLen = BuildPath(m_PathTemp[0],MAX_PATH_LEN,true,false);
		pPath = &(m_PathTemp[0][MAX_PATH_LEN-nPathLen]);
		return nPathLen > 0;		
	}

	//////////////////////////////////////////////////////////////////////////
	// 先正向搜索
	if ( bFindMore && FindPathCore(ptStart,ptEnd) )
	{
		if (( m_nStepCount>nPathLenLimit) &&( nOption & SEARCH_OPTION_OPTIMIZE )) {ConnectCorners(ptStart);}

		nPathLenTemp[0] = BuildPath(m_PathTemp[0],MAX_PATH_LEN,true);
		pPathTemp[0]    = &(m_PathTemp[0][MAX_PATH_LEN-nPathLenTemp[0]]);
		
		if ( nPathLenTemp[0]>0 && (nShortest<0 || nPathLenTemp[0]<nPathLenTemp[nShortest]) )
		{
			nShortest = 0;
			bFindMore = (nPathLenTemp[0] > nPathLenLimit) && ( nOption & SEARCH_OPTION_OPTIMIZE );
			m_nMaxStep      = nMaxStep;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 精简搜路搜索不到时启用A*
	if ( bFindMore && SEARCH_OPTION_ASTAR & nOption )
	{
		FindAStar(ptStart,ptEnd);
		nPathLen = BuildPath(m_PathTemp[0],MAX_PATH_LEN,true,false);
		pPath = &(m_PathTemp[0][MAX_PATH_LEN-nPathLen]);
		return nPathLen > 0;		
	}

	//////////////////////////////////////////////////////////////////////////
	// 正向搜索不到时,反向搜索一下,然后把结果反转
	if ( bFindMore && FindPathCore(ptEnd,ptStart) )
	{
		// 因为自己本身在占位块，所以可能路径开始不是自己
		TILE_TAG * pTag = GetTileTag(ptStart);
		if ( m_pNearestTile!=0 && m_pNearestTile!=pTag)
		{
			pTag->pParent = m_pNearestTile;
			m_pNearestTile= pTag;
		}

		if (( m_nStepCount>nPathLenLimit) &&( nOption & SEARCH_OPTION_OPTIMIZE )) {ConnectCorners(ptEnd);}
		nPathLenTemp[1] = BuildPath(m_PathTemp[1],MAX_PATH_LEN,false);
		pPathTemp[1]    = m_PathTemp[1];
		if ( nPathLenTemp[1]>0 && (nShortest<0 || nPathLenTemp[1]<nPathLenTemp[nShortest] ))
		{
			nShortest = 1;
			bFindMore = (nPathLenTemp[1] > nPathLenLimit) && ( nOption & SEARCH_OPTION_OPTIMIZE );
			m_nMaxStep      = nMaxStep;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 如果还搜不到则更换IsStraightway算法
	if ( bFindMore )
	{
		if ( m_nSearchOption & SEARCH_OPTION_STRAIGHTWAY )
		{
			m_nSearchOption &= ~SEARCH_OPTION_STRAIGHTWAY;
		}
		else
		{
			m_nSearchOption |= SEARCH_OPTION_STRAIGHTWAY;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	if ( bFindMore && FindPathCore(ptStart,ptEnd) )
	{
		if (( m_nStepCount>nPathLenLimit) &&( nOption & SEARCH_OPTION_OPTIMIZE )) {ConnectCorners(ptStart);}
		nPathLenTemp[2] = BuildPath(m_PathTemp[2],MAX_PATH_LEN,true);
		pPathTemp[2]    = &(m_PathTemp[2][MAX_PATH_LEN-nPathLenTemp[2]]);
		if ( nPathLenTemp[2]>0 && (nShortest<0 || nPathLenTemp[2]<nPathLenTemp[nShortest] ) )
		{
			nShortest = 2;
			bFindMore = (nPathLenTemp[2] > nPathLenLimit) && ( nOption & SEARCH_OPTION_OPTIMIZE );
			m_nMaxStep      = nMaxStep;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 正向搜索不到时,反向搜索一下,然后把结果反转
	if ( bFindMore && FindPathCore(ptEnd,ptStart) )
	{
		// 因为自己本身在占位块，所以可能路径开始不是自己
		TILE_TAG * pTag = GetTileTag(ptStart);
		if ( m_pNearestTile!=0 && m_pNearestTile!=pTag)
		{
			pTag->pParent = m_pNearestTile;
			m_pNearestTile= pTag;
		}

		if (( m_nStepCount>nPathLenLimit) &&( nOption & SEARCH_OPTION_OPTIMIZE )) {ConnectCorners(ptEnd);}
		nPathLenTemp[3] = BuildPath(m_PathTemp[3],MAX_PATH_LEN,false);
		pPathTemp[3]    = m_PathTemp[3];
		if ( nPathLenTemp[3]>0 && (nShortest<0 || nPathLenTemp[3]<nPathLenTemp[nShortest] ) )
		{
			nShortest = 3;
			bFindMore = (nPathLenTemp[3] > nPathLenLimit) && ( nOption & SEARCH_OPTION_OPTIMIZE );
			m_nMaxStep      = nMaxStep;
		}
	}

	if ( nShortest<0 )
	{
		// 如果没搜索成功,则尽量返回最近的点
		if ( m_nSearchOption & SEARCH_OPTION_NEAREST )
		{
			FindPathCore(ptStart,ptEnd);
			nPathLen = BuildPath(m_PathTemp[0],MAX_PATH_LEN,true);
			pPath = &(m_PathTemp[0][MAX_PATH_LEN-nPathLen]);
			return nPathLen > 0;
		}

		return false;
	}else
	{
		nPathLen = nPathLenTemp[nShortest];
		pPath = pPathTemp[nShortest];
		return nPathLen>0;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
/**
@name         : 核心搜路函数
@brief        : 
@param        : 
@return       : 
*/
//////////////////////////////////////////////////////////////////////////
TEMPLATE_DECLARE
bool PATH_FINDER_DECLARE::FindPathCore(const POINT & ptStart,const POINT & ptEnd)
{
	++m_nFind;

	m_ptStart = ptStart;
	m_ptEnd   = ptEnd;

	m_pNearestTile = 0;
	m_nStepCount   = 0;

	POINT ptTile = ptStart;

	m_AroundQueue.clear();
	
	if ( m_nSearchOption & SEARCH_OPTION_OPTIMIZE )
	{
		memset(m_Corners,0,sizeof(m_Corners));
		memset(m_nCornerFlag,INVALID_CORNER_FLAG,sizeof(m_nCornerFlag));
	}

	while(1)
	{
		// 直接逼近
		if ( Approach(ptTile,ptEnd))
		{
			return true;
		}

		// 直接逼近失败后取出绕转点进行绕转
		bool bAroundOK = false;
		while ( !m_AroundQueue.empty() )
		{
			AROUND_POINT around = m_AroundQueue.back();
			m_AroundQueue.pop_back();

			if ( Around(around.ptTile,around.nMoveDir,around.nTurnDir,m_ptStart,m_ptEnd))
			{
				assert ( m_pNearestTile );
		
				TileToPoint(m_pNearestTile,ptTile);
				bAroundOK = true;

				// 只有分布在外轮廓上的点才能加入拐点列表
				if ( m_nSearchOption & SEARCH_OPTION_OPTIMIZE )
				{
					RecordCorner(ptTile);
				}

				break;
			}
		}

		// 绕转全部失败了
		if( !bAroundOK )
		{
			return false;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
/**
@name         : 沿直路靠近目标点,如果遇到阻挡,则将拐点push到拐点队列中
@brief        : 由于对象只能沿八方向移动,所以如果起始点和目标点不在八方向上则实际走的是折线
@param ptStart: 起始点
@param ptEnd  : 目标点
@return       : 成功到达目标点则返回true,否则返回false
*/
//////////////////////////////////////////////////////////////////////////
TEMPLATE_DECLARE
bool PATH_FINDER_DECLARE::Approach(const POINT & ptStart,const POINT & ptEnd)
{
	if ( ptStart.x==ptEnd.x && ptStart.y==ptEnd.y )
	{
		return true;
	}

	POINT ptTile = ptStart;
	POINT ptNext;

	TILE_TAG  * pParent = GetTileTag(ptStart);
	if ( pParent==0 )
	{
		return false;
	}

	if ( pParent->nFindID!=m_nFind )
	{
		pParent->pParent = 0;
		pParent->nMoveDir= 0;
		pParent->nFindID = m_nFind;
	}

	TILE_TAG  * pCurrent= 0;

	int   nCurDir = DIR_EAST;

	while(++m_nStepCount<=m_nMaxStep)
	{
		// 直接向目标点逼近 approach to destination
		nCurDir = GetDir(ptTile,ptEnd);
		MoveNext(ptTile,nCurDir,ptNext);

		// 取得Tile信息
		pCurrent = GetTileTag(ptNext);

		if ( pCurrent==0 )
		{
			return false;
		}

		// 死循环了
		if ( IsTileVisited(pCurrent,nCurDir) )
		{
			return false;
		}

		// 碰到阻挡开始绕转
		if ( m_BlockFunc(m_pMap,ptNext) )
		{
			// 如果目标点是阻挡的则直接退出
			if (ptNext.x==ptEnd.x && ptNext.y==ptEnd.y )
			{
				return true;
			}

			// 将拐点坐标放进绕转队列
			AROUND_POINT around;
			around.ptTile = ptTile;
			around.nMoveDir = TurnDir(nCurDir,TURN_CW);
			around.nTurnDir = TURN_CCW;
			
			m_AroundQueue.push_back(around);

			around.ptTile = ptTile;
			around.nMoveDir = TurnDir(nCurDir,TURN_CCW);
			around.nTurnDir = TURN_CW;

			m_AroundQueue.push_back(around);

			return false;
		}

		// 记录路点
		MarkTileVisited(pCurrent,nCurDir,pParent);

		ptTile            = ptNext;
		m_pNearestTile    = pCurrent;
		pParent           = pCurrent;
		pCurrent->bAround = false;

		if (ptTile.x==ptEnd.x && ptTile.y==ptEnd.y )
		{
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
/**
@name         : 沿障碍物开始绕行
@brief        : 由于遇到阻挡,所以现在尝试沿阻挡块绕过去,这里实现的是一段沿占位块绕转的代码
@brief        : 特别要注意的是: 绕障碍的算法比较简单,麻烦的是判断何时结束绕行
@brief        :                 这里我们定义一个直路的概念,回到直路则结束绕行,这是本算法的核心
@param  ptLoc : 开始绕行的位置
@param nMoveDir:前进方向
@param nTurnDir:绕转的方向,是顺时针绕还是逆时针
@return       : 如果成功绕过障碍物重新回到直路则返回true,否则返回false
*/
//////////////////////////////////////////////////////////////////////////
TEMPLATE_DECLARE
bool PATH_FINDER_DECLARE::Around(const POINT & ptLoc,int nMoveDir,int nTurnDir,const POINT & ptStart,const POINT & ptEnd)
{
	POINT ptTile = ptLoc;
	POINT ptNext;
	POINT ptAround;

	TILE_TAG * pParent = GetTileTag(ptTile);
	TILE_TAG * pCurrent= 0;

	int   nContinueTurn = 0;

	while(++m_nStepCount<=m_nMaxStep)
	{
		MoveNext(ptTile,nMoveDir,ptNext);

		// 取得Tile信息
		pCurrent = GetTileTag(ptNext);

		if ( pCurrent==0 )
		{
			return false;
		}

		// 检查是否出现死循环路径
		if ( IsTileVisited(pCurrent,nMoveDir) )
		{
			return false;
		}

		bool bNextBlock   = m_BlockFunc(m_pMap,ptNext);

		// 如果下一步阻挡,则试图向反方向转向
		if ( bNextBlock==true )
		{
			nMoveDir = TurnDir(nMoveDir,!nTurnDir);

			// 记录转向点,防止出现死循环
			MarkTileVisited(pCurrent,nMoveDir,0);

			// 连续转了两次向
			if ( ++nContinueTurn > 2)
			{
				return false;
			}

			continue;
		}

		if ( nContinueTurn ) nContinueTurn = 0;

		// 行进到下一步
		MarkTileVisited(pCurrent,nMoveDir,pParent);

		ptTile         = ptNext;
		m_pNearestTile = pCurrent;
		pParent        = pCurrent;
        pCurrent->bAround = true;

		// 如果已经到了直路上就停止绕转
		bool bReturn = m_nSearchOption & SEARCH_OPTION_STRAIGHTWAY ? IsOnStraightWay(ptTile,ptLoc,ptEnd) : IsCanApproach(ptTile,ptEnd);
		if ( bReturn )
		{
			return true;
		}

		// 如果紧贴着的Tile不阻挡,则立即将方向绕回去
		MoveNext(ptTile,TurnDir(nMoveDir,nTurnDir),ptAround);
		if (!m_BlockFunc(m_pMap,ptAround))
		{
			nMoveDir = TurnDir(nMoveDir,nTurnDir);
		}
	};

	return false;
}

//////////////////////////////////////////////////////////////////////////
/**
@name         : 从A点到B点，中间有可能进行了绕转，这里尝试将这两个点直接连起来
@brief        : 路径优化策略之一
@param ptStart: 起始点
@param ptEnd  : 目标点
@return       : 成功到达目标点则返回true,否则返回false
*/
//////////////////////////////////////////////////////////////////////////
TEMPLATE_DECLARE
bool PATH_FINDER_DECLARE::ConnectDirect(const POINT & ptStart,const POINT & ptEnd)
{
	if ( ptStart.x==ptEnd.x && ptStart.y==ptEnd.y )
	{
		return true;
	}

	POINT ptTile = ptStart;
	POINT ptNext;

	TILE_TAG  * pParent = GetTileTag(ptStart);
	if ( pParent==0 )
	{
		return false;
	}

	if ( pParent->nFindID!=m_nFind )
	{
		pParent->pParent = 0;
		pParent->nMoveDir= 0;
		pParent->nFindID = m_nFind;
	}

	TILE_TAG  * pCurrent= 0;

	int   nCurDir = DIR_EAST;

	while(++m_nStepCount<=m_nMaxStep)
	{
		// 直接向目标点逼近 approach to destination
		nCurDir = GetDir(ptTile,ptEnd);
		MoveNext(ptTile,nCurDir,ptNext);

		// 取得Tile信息
		pCurrent = GetTileTag(ptNext);

		if ( pCurrent==0 )
		{
			return false;
		}

		// 发现已经存在的路径返回
		if ( pCurrent->nFindID==m_nFind )
		{
			pCurrent->pParent = pParent;
		//	return true;
		}

		// 碰到阻挡放弃
		if ( m_BlockFunc(m_pMap,ptNext) )
		{
			return false;
		}

		// 记录路点
		MarkTileVisited(pCurrent,nCurDir,pParent);

		ptTile            = ptNext;
		pParent           = pCurrent;
		pCurrent->bAround = false;
		
		if (ptTile.x==ptEnd.x && ptTile.y==ptEnd.y )
		{
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
/**
@name         : 记录拐点
@brief        : 用于优化路径,目前只记录上下左右四个点
*/
//////////////////////////////////////////////////////////////////////////
TEMPLATE_DECLARE
void PATH_FINDER_DECLARE::RecordCorner(const POINT & ptTile)
{
	if (( ptTile.x <= m_Corners[CORNER_LEFT_TOP].x && ptTile.y <= m_Corners[CORNER_LEFT_TOP].y ) || m_nCornerFlag[CORNER_LEFT_TOP] == INVALID_CORNER_FLAG)
	{
		m_Corners[CORNER_LEFT_TOP] = ptTile;
		m_nCornerFlag[CORNER_LEFT_TOP] = m_nStepCount;
	}

	if (( ptTile.x <= m_Corners[CORNER_LEFT_BOTTOM].x && ptTile.y >= m_Corners[CORNER_LEFT_BOTTOM].y )|| m_nCornerFlag[CORNER_LEFT_BOTTOM] == INVALID_CORNER_FLAG)
	{
		m_Corners[CORNER_LEFT_BOTTOM] = ptTile;
		m_nCornerFlag[CORNER_LEFT_BOTTOM] = m_nStepCount;
	}

	if (( ptTile.x >= m_Corners[CORNER_RIGHT_TOP].x && ptTile.y <= m_Corners[CORNER_RIGHT_TOP].y ) || m_nCornerFlag[CORNER_RIGHT_TOP] == INVALID_CORNER_FLAG)
	{
		m_Corners[CORNER_RIGHT_TOP] = ptTile;
		m_nCornerFlag[CORNER_RIGHT_TOP] = m_nStepCount;
	}

	if (( ptTile.x >= m_Corners[CORNER_RIGHT_BOTTOM].x && ptTile.y >= m_Corners[CORNER_RIGHT_BOTTOM].y ) || m_nCornerFlag[CORNER_RIGHT_BOTTOM] == INVALID_CORNER_FLAG)
	{
		m_Corners[CORNER_RIGHT_BOTTOM] = ptTile;
		m_nCornerFlag[CORNER_RIGHT_BOTTOM] = m_nStepCount;
	}
}

//////////////////////////////////////////////////////////////////////////
/**
@name         : 路径搜索完后尝试将所有拐点连接,可以得到更优化的路径
*/
//////////////////////////////////////////////////////////////////////////
TEMPLATE_DECLARE
void PATH_FINDER_DECLARE::ConnectCorners(const POINT & ptStart)
{
	DWORD dwMinCorner = INVALID_CORNER_FLAG;
	DWORD dwVisited   = 0;
	int   index = -1;
	POINT ptConnect = ptStart;

	while(1)
	{
		for ( int i=0;i<CORNER_NUM;++i )
		{
			if ( m_nCornerFlag[i]<dwMinCorner && m_nCornerFlag[i] > dwVisited )
			{
				dwMinCorner =  m_nCornerFlag[i];
				index = i;
			}
		}

		if ( index>=0 )
		{
			ConnectDirect(ptConnect,m_Corners[index]);
			ptConnect = m_Corners[index];
			index = -1;
			dwVisited = dwMinCorner;
			dwMinCorner = INVALID_CORNER_FLAG;
		}else
		{
			break;
		}
	};
}

//////////////////////////////////////////////////////////////////////////
/**
@name			: 采用A*算法搜索路径
@brief			: 
@param ptStart  : 起点坐标
@param ptEnd    : 终点坐标
@return         : 返回搜路是否成功 
*/
//////////////////////////////////////////////////////////////////////////
TEMPLATE_DECLARE
bool PATH_FINDER_DECLARE::FindAStar(const POINT & ptStart,const POINT & ptEnd)
{
	++m_nFind;
	m_ptStart = ptStart;
	m_ptEnd   = ptEnd;

	m_pNearestTile = 0;
	m_nStepCount   = 0;
	unsigned long nNearestDist = 10000000;

	POINT ptTile = ptStart;
	POINT ptNext;

	// 待搜索列表
	typedef  std::multimap<unsigned long,POINT>      OPEN_LIST; 
	static   OPEN_LIST								 m_openList;							  
	
	static   POINT          ptTileArray[DIR_MAX];
	static   int        	nDistArray[DIR_MAX];

	// 最好初始化工作
	m_openList.clear();
	m_openList.insert( OPEN_LIST::value_type(0,ptStart) );
	
	TILE_TAG * pStart = GetTileTag(ptStart);
	if ( pStart )
	{
		pStart->pParent = 0;
		pStart->nFindID = m_nFind;
		pStart->nStep   = 0;
	}

	while( ++m_nStepCount<=m_nMaxStep )
	{
		// 已经没有点可以尝试了
		if ( m_openList.empty() )
		{
			return false;
		}

		// 从OPEN_LIST中取出积分最小的点优先尝试
		OPEN_LIST::iterator it = m_openList.begin();
		ptTile = it->second;
		m_openList.erase( it );

		TILE_TAG * pCurrent = GetTileTag(ptTile);
		if ( pCurrent==0 )
			continue;

		// 到达目标点
		if ( ptTile.x == ptEnd.x && ptTile.y == ptEnd.y )
		{
			m_pNearestTile = pCurrent;
			return true;
		}

		int block_num  = 0;
		int visited_num= 0;
		int step       = pCurrent->nStep + 1;

		// 将周围8个方向的点压入OPEN_LIST
		for ( int dir=0;dir<DIR_MAX;++dir )
		{
			MoveNext( ptTile,dir,ptNext );
			nDistArray[dir] = -1;

			TILE_TAG * pTag = GetTileTag(ptNext);
			if ( pTag==0 || pTag==pCurrent->pParent )
				continue;

			// 已经搜索过的
			if ( pTag->nFindID==m_nFind )
			{
				if ( pTag->nStep > step )
				{
					pTag->pParent = pCurrent;
					pTag->nStep   = step;
				}else
				{
					++visited_num;
					continue;
				}
			}

			if ( m_BlockFunc(m_pMap,ptNext) )
			{
				++block_num;
				continue;
			}

			// 添加到OPEN列表
			pTag->nFindID = m_nFind;
			pTag->pParent = pCurrent;
			pTag->nStep   = step;			

			ptTileArray[dir] = ptNext;
			nDistArray[dir]  = abs( ptNext.x-ptEnd.x ) + abs( ptNext.y-ptEnd.y );
			if ( nDistArray[dir]<nNearestDist )
			{
				m_pNearestTile = pTag;
				nNearestDist = nDistArray[dir];
			}
		}

		for ( int i=0;i<DIR_MAX;++i )
		{
			if ( nDistArray[i]>=0 )
			{
				// 评估公式 = 离目标点的距离*a + 周围占位块数*b + 周围已搜索的点*c + 已经走过的步数*d
				unsigned score = nDistArray[i] + block_num + visited_num + step*2 + (i%2)*2;
				OPEN_LIST::value_type val(score,ptTileArray[i]);
				m_openList.insert( val );
			}
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
/**
@name         : 获取Tile信息
@brief        : 由于Tile的信息是临时保存在m_TileTags里,该段数据每次搜路都可能覆写一遍,所以取的时候要做个映射
@brief        : 设定一个最大搜路范围的好处是: 
@brief        :     1.当地图非常大时(例如1024*1024),如果维护一个和地图同样大小的数组会很消耗内存
@brief        :     2.m_TileTags的大小和地图大小无关,这样所有的地图可以共用一个搜路器,可以更多的节省内存
@return       : 如果是不合法的Tile或则已经超过了最大的搜路范围则返回0,否则返回Tile信息指针
*/
//////////////////////////////////////////////////////////////////////////
TEMPLATE_DECLARE
typename PATH_FINDER_DECLARE::TILE_TAG * PATH_FINDER_DECLARE::GetTileTag(const POINT & ptTile)
{
	if (!m_TileTags)
		return 0;

	// 是否超出边界
	if ( ptTile.x < 0 || (unsigned long)ptTile.x >= m_nMapWidth )
	{
		return 0;
	}

	if ( ptTile.y < 0 || (unsigned long)ptTile.y >= m_nMapHeight )
	{
		return 0;
	}

	// 映射到Tag数组
	int x = ptTile.x;//(ptTile.x - m_ptStart.x) + (MAX_SEARCH_AREA>>1);
	int y = ptTile.y;//(ptTile.y - m_ptStart.y) + (MAX_SEARCH_AREA>>1);

	if ( x<0 || y<0 || x>=MAX_SEARCH_AREA || y>=MAX_SEARCH_AREA )
	{
		return 0;
	}
	if ( x<0 || y<0 || x>=m_nMapWidth || y>=m_nMapHeight )
	{
		return 0;
	}
	return &(m_TileTags[y*m_nMapWidth+x]);
}

//////////////////////////////////////////////////////////////////////////
/**
@name         : 从TAG的指针获得Tile的坐标 (GetTileTag的逆操作)
@brief        : 从TAG的指针转换到Tile坐标可以直接通过指针到m_TileTags[0]的偏移折算出来
@param  pTag  : 传入TILE_TAG指针
@param ptTile : 返回该指针对应的Tile坐标
@return       : 
*/
//////////////////////////////////////////////////////////////////////////
TEMPLATE_DECLARE
void PATH_FINDER_DECLARE::TileToPoint(TILE_TAG * pTag,POINT &ptTile)
{
	int offset = pTag - m_TileTags;

	// 这里有除法运算,但如果MAX_SEARCH_AREA取值合适,编译器会进行优化
	ptTile.x = /*m_ptStart.x - (MAX_SEARCH_AREA>>1) +*/ (offset%m_nMapWidth);
	ptTile.y = /*m_ptStart.y - (MAX_SEARCH_AREA>>1) +*/ (offset/m_nMapWidth);
}

//////////////////////////////////////////////////////////////////////////
/**
@name         : 判断某个Tile是否被访问过
@brief        : 这个函数主要用于规避绕转时的死循环
@param  pTag  : Tile的信息指针 
@param  nDir  : 如果两次经过同一个Tile但是经过的方向不同则应被允许,所以这里需要传入待测试的方向
@return       : 
*/ 
//////////////////////////////////////////////////////////////////////////
TEMPLATE_DECLARE
bool PATH_FINDER_DECLARE::IsTileVisited(TILE_TAG * pTag,int nDir)
{
	return pTag->nFindID==m_nFind && (pTag->nMoveDir & (0x01 << nDir));
}

//////////////////////////////////////////////////////////////////////////
/**
@name         : 标记某个Tile已经被访问过
@brief        : 这个函数主要用于规避绕转时的死循环
@param  pTag  : Tile的信息指针 
@param  nDir  : 如果两次经过同一个Tile但是经过的方向不同则应被允许,所以这里需要传入待测试的方向
@param pParent: 记录父节点指针，用于回溯路径
@return       : 
*/
//////////////////////////////////////////////////////////////////////////
TEMPLATE_DECLARE
void PATH_FINDER_DECLARE::MarkTileVisited(TILE_TAG * pTag,int nDir,TILE_TAG * pParent)
{
	if ( pTag->nFindID!=m_nFind )
	{
		pTag->pParent = pParent;
		pTag->nMoveDir= 0x01 << nDir;
		pTag->nFindID = m_nFind;
	}else
	{
		pTag->nMoveDir |= 0x01<< nDir;
	}
}

//////////////////////////////////////////////////////////////////////////
/**
@name         : 沿给定方向移动一步
@brief        : 
@param ptTile : 当前坐标
@param dir    : 前进方向
@param ptNext : 返回下一步坐标
@return       : 
*/
//////////////////////////////////////////////////////////////////////////
TEMPLATE_DECLARE
void PATH_FINDER_DECLARE::MoveNext(const POINT & ptTile,int dir,POINT & ptNext)
{
	static int s_DirNextTable[8][2] =
	{
		{1,0},  // 方向定义 : 东  (angle:0)
		{1,-1}, // 方向定义 : 东北(angle:45)
		{0,-1}, // 方向定义 : 北  (angle:90)
		{-1,-1},// 方向定义 : 西北(angle:135)
		{-1,0}, // 方向定义 : 西  (angle:180)
		{-1,1}, // 方向定义 : 西南(angle:225)
		{0,1},  // 方向定义 : 南  (angle:270)
		{1,1},  // 方向定义 : 东南(angle:315)	
	};

	assert(dir>=0 && dir <8 );

	ptNext.x = ptTile.x + s_DirNextTable[dir][0];
	ptNext.y = ptTile.y + s_DirNextTable[dir][1];
}

//////////////////////////////////////////////////////////////////////////
/**
@name         : 取得两个点之间的方向
@brief        : 这里只返回45度整数倍的八个方向
@return       : 方向值定义见前面的枚举值
*/
//////////////////////////////////////////////////////////////////////////
TEMPLATE_DECLARE
int PATH_FINDER_DECLARE::GetDir(const POINT & ptCur,const POINT & ptNext)
{
	static int s_DirTable[3][3] = {
		{DIR_WEST_NORTH,DIR_NORTH,DIR_EAST_NORTH},
		{DIR_WEST,-1,DIR_EAST},
		{DIR_WEST_SOUTH,DIR_SOUTH,DIR_EAST_SOUTH}
	};
 
	int x = ptNext.x > ptCur.x ? 2 : (ptNext.x == ptCur.x ? 1 : 0);
	int y = ptNext.y > ptCur.y ? 2 : (ptNext.y == ptCur.y ? 1 : 0);

	return s_DirTable[y][x];
}

//////////////////////////////////////////////////////////////////////////
/**
@name         : 旋转方向
@brief        : 给定当前方向,返回沿顺时针或逆时针旋转后的方向
@param        : 
@return       : 返回旋转后的方向
*/
//////////////////////////////////////////////////////////////////////////
TEMPLATE_DECLARE
int  PATH_FINDER_DECLARE::TurnDir(int nMoveDir,int nTurnDir)
{
	// 斜方向只转45度,垂直水平方向转90度,这个规则是根据贴轮廓的需求得来的
	int turn_angle = (nMoveDir & 0x01) ? 1 : 2;

	if ( nTurnDir==TURN_CW)
	{
		nMoveDir -= turn_angle;
		return nMoveDir >=0 ? nMoveDir : nMoveDir+8;
	}else
	{
		nMoveDir += turn_angle;
		return nMoveDir >=8 ? nMoveDir-8 : nMoveDir;
	}
}

//////////////////////////////////////////////////////////////////////////
/**
@name         : 判断某点是否在直路上
@brief        : 用来结束绕行,这个函数的主要思路是判断给定点是否在起始点到目标点构成的直线（或折线）上
@brief        : 实际上判断是否在折线上时会有一点点小错误,但由于我们仅仅是用来判断何时结束绕行,这本来就是一个模糊概念,所以完全不影响搜路结果
@param ptTile : 当前点
@param ptStart: 搜路起始点
@param ptEnd  : 搜路结束点
@return       : 
*/
//////////////////////////////////////////////////////////////////////////
TEMPLATE_DECLARE
bool PATH_FINDER_DECLARE::IsOnStraightWay(const POINT & ptTile,const POINT & ptStart,const POINT & ptEnd)
{
	int dx = ptEnd.x - ptStart.x;
	int dy = ptEnd.y - ptStart.y;

	// 在90度射线上
	if ( dx==0 )
	{
		return (ptEnd.x == ptTile.x) && ((ptTile.y > ptStart.y) == (ptEnd.y>ptStart.y));
	}

	// 在0度射线上
	if ( dy==0 )
	{
		return (ptEnd.y == ptTile.y) && ((ptTile.x > ptStart.x) == (ptEnd.x>ptStart.x));
	}

	int abs_x = abs(dx);
	int abs_y = abs(dy);

	// 在45度射线上
	if ( abs_x==abs_y )
	{
		if ( abs(ptTile.x-ptStart.x)!=abs(ptTile.y-ptStart.y) )
			return false;
		return ((ptEnd.x>ptStart.x)==(ptTile.x>ptStart.x)) && ((ptEnd.y>ptStart.y)==(ptTile.y>ptStart.y));
	}
	
	// 折成两断再测
	POINT ptMiddle;

	if ( abs_x<abs_y )
	{
		ptMiddle.x = ptEnd.x;
		ptMiddle.y = ptStart.y + (dy > 0 ? abs_x : -abs_x);
	}else
	{
		ptMiddle.y = ptEnd.y;
		ptMiddle.x = ptStart.x + (dx > 0 ? abs_y : -abs_y);
	}
	
	if ( ptMiddle.x==ptTile.x && ptMiddle.y==ptTile.y )
	{
		return true;
	}

	return IsOnStraightWay(ptTile,ptStart,ptMiddle) || IsOnStraightWay(ptTile,ptMiddle,ptEnd);
}

//////////////////////////////////////////////////////////////////////////
/**
@name         : 不使用直路判断,而是判断可不可以Approach
@brief        : 即:只要朝目标的下一个点不被占位则表示可以结束绕转
@param        : 
@return       : 
*/
//////////////////////////////////////////////////////////////////////////
TEMPLATE_DECLARE
bool PATH_FINDER_DECLARE::IsCanApproach(const POINT & ptTile,const POINT & ptEnd)
{
	int nDir = GetDir(ptTile,ptEnd);
	POINT ptNext;
	MoveNext(ptTile,nDir,ptNext);

	if ( m_BlockFunc(m_pMap,ptNext) )
	{
		return false;
	}

	TILE_TAG * pTag = GetTileTag(ptNext);
	if ( pTag==0 || pTag->nFindID==m_nFind )
	{
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
/**
@name         : 根据走过的Tile,回朔完整路径
@brief        : 因为搜路时我们经过某些Tile,当时并不知道是否能到达目的地,无从记录,所以到达后要反过来回朔一遍才能把路径找出来
@param pPathBuffer : 用来存路径的缓冲区
@param nBufSize    : 缓冲区大小
@param nReverse    : 是否把路径反过来存,因为是回溯,所以默认应该反过来存
@return       : 
*/ 
//////////////////////////////////////////////////////////////////////////
TEMPLATE_DECLARE
int PATH_FINDER_DECLARE::BuildPath(POINT * pPathBuffer,int nBufSize,bool nReverse,bool optmize)
{
	int   nBarrier = 0;
	int   nPathLen = 0; 
	POINT ptTile;

	while(m_pNearestTile && nPathLen<nBufSize)
	{
		TileToPoint(m_pNearestTile,ptTile);

		// 绕转的Tile优化,不要频繁转向
		if ( optmize )
		{
			if ( m_pNearestTile->bAround && nPathLen>=2 )
			{
				POINT ptPreTile = nReverse ? pPathBuffer[nBufSize-1-nPathLen+2] : pPathBuffer[nPathLen-2];
				if ( abs(ptPreTile.x-ptTile.x)<=1 && abs(ptPreTile.y-ptTile.y)<=1 )
				{
					nPathLen-=1;
				}
			}
		}

		// 把路点放到缓冲区里
		if ( nReverse )
		{
			pPathBuffer[nBufSize-1-nPathLen] = ptTile;
		}
		else
		{
			pPathBuffer[nPathLen] = ptTile;
		}

		m_pNearestTile = m_pNearestTile->pParent;
		++nPathLen;

		// 检测死循环
		if ( ++nBarrier>=nBufSize)
		{
			assert("find path error! detected a closed-loop when buiding the path.");
			return 0;  
		}
	}

	return nPathLen;
}

#endif//__PATH_FINDER_H__