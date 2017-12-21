/*******************************************************************
** 文件名:	GridVisible.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			
********************************************************************/

#pragma once

enum OBJ_SIGHT_MASK
{
    MASK_OBJ_NORMAL_SIGHT = 0x01,
    MASK_OBJ_REAL_SIGHT = 0x02,
};

#define MASK_OBJ_SIGHT_ALL (MASK_OBJ_NORMAL_SIGHT|MASK_OBJ_REAL_SIGHT)


/**
	九宫格阵营视野可见性判断算法
	1.每个对象将周围的九个格子点亮，表示可见
	2.格子变亮时，把格子中的对象都发给这个阵营
	3.格子变暗时,把格子中的对象从这个阵营取消
	4.对象跨越格子时，如果新格子对某阵营可见，但是对象对某阵营不可见，则更新视野
*/
template< typename HANDLER_TYPE,typename OBJECT_TYPE,typename COORD_TYPE,int GRID_SIZE,int DIM=2 >
class GridVisible
{
public:
	//struct Handler
	//{
	//	// 某个格子中所有对象的可见性发生了变化
	//	virtual void onUpdateGridVisible( COORD_TYPE loc[DIM],int nVisibleFlag ) = 0;

	//	// 某个对象的可见性发生了变化
	//	virtual void onUpdateObjVisible( OBJECT_TYPE obj,int nVisibleFlag ) = 0;
	//};

	// 单个格子对象
	struct GridNode
	{
        int nCampNum;
		int* pVisibleCount;       
        int* pRealVisibleCount;

		GridNode(int nCampCount) : nCampNum(nCampCount), pVisibleCount(0), pRealVisibleCount(0) 
        { 
            pVisibleCount = (int*)malloc( sizeof(int)*nCampCount );
            memset( pVisibleCount,0,sizeof(int)*nCampCount );
            pRealVisibleCount = (int*)malloc( sizeof(int)*nCampCount );
            memset( pRealVisibleCount,0,sizeof(int)*nCampCount );
        }

        ~GridNode()
        {
            if ( pVisibleCount )
            {
                free(pVisibleCount);
                pVisibleCount = 0;
            }

            if ( pRealVisibleCount )
            {
                free(pRealVisibleCount);
                pRealVisibleCount = 0;
            }

            nCampNum = 0;
        }

		int getVisibleFlag()	// 取得视野标志
		{
			int flag = 0;
			for ( int camp=0;camp<nCampNum;++camp )
			{
				if ( pVisibleCount[camp]>0 )
					flag |= CAMP_MASK(camp);
			}

			return flag;
		}

        int getRealVisibleFlag()	// 取得真眼视野标志
        {
            int flag = 0;
            for ( int camp=0;camp<nCampNum;++camp )
            {
                if ( pRealVisibleCount[camp]>0 )
                    flag |= CAMP_MASK(camp);
            }

            return flag;
        }
	};

	COORD_TYPE  origin[2];		// 格子原点位置
	COORD_TYPE  grid_num[2];    // 每维度格子切分数量,对应的x,y,z就是长宽高方向上的格子数量
	int  grid_total;			// 所有维度总格子数
    int camp_cout;

	HANDLER_TYPE * pHandler;    // 处理器指针

	GridNode ** pGrids;	     		// 所有格子节点的指针
	
	GridVisible(int nCampCount) : camp_cout(nCampCount)
	{
        memset( origin,0,sizeof(origin) );
        memset( grid_num,0,sizeof(grid_num) );
        pGrids = 0;
	}

	virtual~GridVisible()
	{
		if ( pGrids )
		{
			for ( int n=0;n<grid_total;++n )
			{
				if ( pGrids[n] )
				{
					delete pGrids[n];
					pGrids[n] = 0;
				}
			}

			free(pGrids);
			pGrids = 0;
		}
		
		pHandler = 0;
	}

	bool create( COORD_TYPE loc[DIM],COORD_TYPE g_num[DIM],HANDLER_TYPE * handler )
	{
		pHandler = handler;

		memcpy( origin,loc,sizeof(origin) );
		memcpy( grid_num,g_num,sizeof(grid_num) );

		grid_total = grid_num[0]*grid_num[1];					 // 所有维度总格子数

		pGrids = (GridNode **)malloc( sizeof(GridNode*)*grid_total );
		memset(pGrids,0,sizeof(GridNode*)*grid_total);
		return true;
	}

	int loc2index(COORD_TYPE loc[DIM])
	{
		int index = 0;
		int dim_span = 1;

		for ( int i=0;i<DIM;++i )
		{
			int offset = (loc[i]-origin[i])/GRID_SIZE;
			if ( offset>=grid_num[i] || offset<0 )				// 超出边界
				return -1;

			index += offset*dim_span;
			dim_span *= grid_num[i];
		}

		return index;
	}

	GridNode * index2node(int index)
	{
		if ( index<0 || index>=grid_total )
			return 0;

		if ( pGrids[index]==0 )
		{
			pGrids[index] = new GridNode(camp_cout);
		}

		return pGrids[index];
	}

	// 添加一个对象,将周围9格占位
	void insertObject( OBJECT_TYPE obj,COORD_TYPE loc[DIM],int camp,int objMask)
	{
		if ( camp<0||camp>=camp_cout||obj->m_nSightInGrid<0 )
			return;

		COORD_TYPE temp_loc[DIM];
		int index = loc2index( loc );
		for ( int x=-obj->m_nSightInGrid;x<=obj->m_nSightInGrid;x++ )
		{
			for ( int y=-obj->m_nSightInGrid;y<=obj->m_nSightInGrid;y++ )
			{
				int current = index + y*grid_num[0]+x;
				GridNode * pNode= index2node(current);
				if ( pNode )
				{
                    bool bUpdateGridNormalVisible = (objMask&MASK_OBJ_NORMAL_SIGHT && ++(pNode->pVisibleCount[camp])==1);
                    bool bUpdateGridRealVisible = (objMask&MASK_OBJ_REAL_SIGHT && ++(pNode->pRealVisibleCount[camp])==1);

                    if ( bUpdateGridNormalVisible||bUpdateGridRealVisible )
                    {
                        // 取格子中心点
                        temp_loc[0] = origin[0] + (current%grid_num[0])*GRID_SIZE + GRID_SIZE/2;
                        temp_loc[1] = origin[1] + ((int)(current/grid_num[0]))*GRID_SIZE + GRID_SIZE/2;
                        pHandler->onUpdateGridVisible(temp_loc,pNode->getVisibleFlag(),pNode->getRealVisibleFlag(),camp,bUpdateGridNormalVisible,bUpdateGridRealVisible);
                    }
				}
			}
		}
	}

	// 移除对象
	void removeObject( OBJECT_TYPE obj,COORD_TYPE loc[DIM],int camp,int objMask )
	{
		if ( camp<0||camp>=camp_cout||obj->m_nSightInGrid<0 )
			return;

		COORD_TYPE temp_loc[DIM];
		int index = loc2index( loc );
		for ( int x=-obj->m_nSightInGrid;x<=obj->m_nSightInGrid;x++ )
		{
			for ( int y=-obj->m_nSightInGrid;y<=obj->m_nSightInGrid;y++ )
			{
				int current = index + y*grid_num[0]+x;
				GridNode * pNode= index2node(current);
				if ( pNode )
				{
                    bool bUpdateGridNormalVisible = (objMask&MASK_OBJ_NORMAL_SIGHT && --(pNode->pVisibleCount[camp])==0);
                    bool bUpdateGridRealVisible = (objMask&MASK_OBJ_REAL_SIGHT && --(pNode->pRealVisibleCount[camp])==0);

                    if ( bUpdateGridNormalVisible||bUpdateGridRealVisible )
                    {
                        // 取格子中心点
                        temp_loc[0] = origin[0] + (current%grid_num[0])*GRID_SIZE + GRID_SIZE/2;
                        temp_loc[1] = origin[1] + ((int)(current/grid_num[0]))*GRID_SIZE + GRID_SIZE/2;
                        pHandler->onUpdateGridVisible(temp_loc,pNode->getVisibleFlag(),pNode->getRealVisibleFlag(),camp,bUpdateGridNormalVisible,bUpdateGridRealVisible);
                    }
				}
			}
		}
	} 

    // 更新视野大小
    void onObjSightChange( OBJECT_TYPE obj,int nOriSightInGrid,int nCurSightInGrid )
    {
        if( nOriSightInGrid==nCurSightInGrid )
        {
            return;
        }

        COORD_TYPE myLoc[DIM]; myLoc[0] = obj->m_Loc.x; myLoc[1] = obj->m_Loc.z;
        int index = loc2index( myLoc );

        COORD_TYPE temp_loc[DIM];
        bool bIncrease = nCurSightInGrid>nOriSightInGrid;
        int nOuterR = max(nOriSightInGrid, nCurSightInGrid);
        int nInnerR = min(nOriSightInGrid, nCurSightInGrid);
        for( int x=-nOuterR;x<=nOuterR;x++ )
        {
            for( int y=-nOuterR;y<=nOuterR;y++ )
            {
                if( x>=-nInnerR && x<=nInnerR && y>=-nInnerR && y<=nInnerR )
                {
                    continue;
                }

                int current = index + y*grid_num[0]+x;
                GridNode * pNode= index2node(current);
                if ( pNode )
                {
                    bool bUpdateGridNormalVisible = ((bIncrease&&(++(pNode->pVisibleCount[obj->m_nCamp])==1)) || ((!bIncrease)&&(--(pNode->pVisibleCount[obj->m_nCamp])==0)));
                    bool bUpdateGridRealVisible = obj->m_bIsAntiStealth 
                        && (((bIncrease&&(++(pNode->pRealVisibleCount[obj->m_nCamp])==1)) || ((!bIncrease)&&(--(pNode->pRealVisibleCount[obj->m_nCamp])==0))));

                    if ( bUpdateGridNormalVisible||bUpdateGridRealVisible )
                    {
                        // 取格子中心点
                        temp_loc[0] = origin[0] + (current%grid_num[0])*GRID_SIZE + GRID_SIZE/2;
                        temp_loc[1] = origin[1] + ((int)(current/grid_num[0]))*GRID_SIZE + GRID_SIZE/2;
                        pHandler->onUpdateGridVisible(temp_loc,pNode->getVisibleFlag(),pNode->getRealVisibleFlag(),obj->m_nCamp,bUpdateGridNormalVisible,bUpdateGridRealVisible);
                    }
                }
            }
        }
    }

	// 移动对象
	// @param obj : 要移动的对象
	// 
	void moveObject( OBJECT_TYPE obj,COORD_TYPE loc[DIM],COORD_TYPE newloc[DIM],int camp )
	{
        int obj_mask = MASK_OBJ_NORMAL_SIGHT;
        if( obj->m_bIsAntiStealth ) obj_mask = MASK_OBJ_SIGHT_ALL;

		insertObject( obj,newloc,camp,obj_mask );
		removeObject( obj,loc,camp,obj_mask );

		int index = loc2index( newloc );
		GridNode * pNew= index2node(index);
		
		index = loc2index( loc );
		GridNode * pOld= index2node(index);

		if ( pNew && pOld )
		{
			if ( pOld->getVisibleFlag()!=pNew->getVisibleFlag()
                || (obj->m_bIsStealing && pOld->getRealVisibleFlag()!=pNew->getRealVisibleFlag()) )
			{
				pHandler->onUpdateObjVisible( obj,pNew->getVisibleFlag(),pNew->getRealVisibleFlag() );
			}
		}
	}

	// 某个点是否能被某个阵营看到
	bool isInSight( COORD_TYPE loc[DIM],int camp )
	{
		if ( camp<0||camp>=camp_cout )
			return false;

		int index = loc2index( loc );
		GridNode * pNode= index2node(index);
		if ( pNode )
		{
			return pNode->pVisibleCount[camp]>0;
		}

		return false;
	}

	// 取得某点的可见标志
	int getVisibleFlag( COORD_TYPE loc[DIM] )
	{
		int index = loc2index( loc );
		GridNode * pNode= index2node(index);
		if ( pNode )
		{
			return pNode->getVisibleFlag();
		}

		return 0;
	}

    // 取得某点的true可见标志
    int getRealVisibleFlag( COORD_TYPE loc[DIM] )
    {
        int index = loc2index( loc );
        GridNode * pNode= index2node(index);
        if ( pNode )
        {
            return pNode->getRealVisibleFlag();
        }

        return 0;
    }
};
