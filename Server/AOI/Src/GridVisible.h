/*******************************************************************
** �ļ���:	GridVisible.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	���� (Carl Chen)
** ��  ��:	8/1/2014
** ��  ��:	1.0
** ��  ��:	

			
********************************************************************/

#pragma once

enum OBJ_SIGHT_MASK
{
    MASK_OBJ_NORMAL_SIGHT = 0x01,
    MASK_OBJ_REAL_SIGHT = 0x02,
};

#define MASK_OBJ_SIGHT_ALL (MASK_OBJ_NORMAL_SIGHT|MASK_OBJ_REAL_SIGHT)


/**
	�Ź�����Ӫ��Ұ�ɼ����ж��㷨
	1.ÿ��������Χ�ľŸ����ӵ�������ʾ�ɼ�
	2.���ӱ���ʱ���Ѹ����еĶ��󶼷��������Ӫ
	3.���ӱ䰵ʱ,�Ѹ����еĶ���������Ӫȡ��
	4.�����Խ����ʱ������¸��Ӷ�ĳ��Ӫ�ɼ������Ƕ����ĳ��Ӫ���ɼ����������Ұ
*/
template< typename HANDLER_TYPE,typename OBJECT_TYPE,typename COORD_TYPE,int GRID_SIZE,int DIM=2 >
class GridVisible
{
public:
	//struct Handler
	//{
	//	// ĳ�����������ж���Ŀɼ��Է����˱仯
	//	virtual void onUpdateGridVisible( COORD_TYPE loc[DIM],int nVisibleFlag ) = 0;

	//	// ĳ������Ŀɼ��Է����˱仯
	//	virtual void onUpdateObjVisible( OBJECT_TYPE obj,int nVisibleFlag ) = 0;
	//};

	// �������Ӷ���
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

		int getVisibleFlag()	// ȡ����Ұ��־
		{
			int flag = 0;
			for ( int camp=0;camp<nCampNum;++camp )
			{
				if ( pVisibleCount[camp]>0 )
					flag |= CAMP_MASK(camp);
			}

			return flag;
		}

        int getRealVisibleFlag()	// ȡ��������Ұ��־
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

	COORD_TYPE  origin[2];		// ����ԭ��λ��
	COORD_TYPE  grid_num[2];    // ÿά�ȸ����з�����,��Ӧ��x,y,z���ǳ���߷����ϵĸ�������
	int  grid_total;			// ����ά���ܸ�����
    int camp_cout;

	HANDLER_TYPE * pHandler;    // ������ָ��

	GridNode ** pGrids;	     		// ���и��ӽڵ��ָ��
	
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

		grid_total = grid_num[0]*grid_num[1];					 // ����ά���ܸ�����

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
			if ( offset>=grid_num[i] || offset<0 )				// �����߽�
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

	// ���һ������,����Χ9��ռλ
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
                        // ȡ�������ĵ�
                        temp_loc[0] = origin[0] + (current%grid_num[0])*GRID_SIZE + GRID_SIZE/2;
                        temp_loc[1] = origin[1] + ((int)(current/grid_num[0]))*GRID_SIZE + GRID_SIZE/2;
                        pHandler->onUpdateGridVisible(temp_loc,pNode->getVisibleFlag(),pNode->getRealVisibleFlag(),camp,bUpdateGridNormalVisible,bUpdateGridRealVisible);
                    }
				}
			}
		}
	}

	// �Ƴ�����
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
                        // ȡ�������ĵ�
                        temp_loc[0] = origin[0] + (current%grid_num[0])*GRID_SIZE + GRID_SIZE/2;
                        temp_loc[1] = origin[1] + ((int)(current/grid_num[0]))*GRID_SIZE + GRID_SIZE/2;
                        pHandler->onUpdateGridVisible(temp_loc,pNode->getVisibleFlag(),pNode->getRealVisibleFlag(),camp,bUpdateGridNormalVisible,bUpdateGridRealVisible);
                    }
				}
			}
		}
	} 

    // ������Ұ��С
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
                        // ȡ�������ĵ�
                        temp_loc[0] = origin[0] + (current%grid_num[0])*GRID_SIZE + GRID_SIZE/2;
                        temp_loc[1] = origin[1] + ((int)(current/grid_num[0]))*GRID_SIZE + GRID_SIZE/2;
                        pHandler->onUpdateGridVisible(temp_loc,pNode->getVisibleFlag(),pNode->getRealVisibleFlag(),obj->m_nCamp,bUpdateGridNormalVisible,bUpdateGridRealVisible);
                    }
                }
            }
        }
    }

	// �ƶ�����
	// @param obj : Ҫ�ƶ��Ķ���
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

	// ĳ�����Ƿ��ܱ�ĳ����Ӫ����
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

	// ȡ��ĳ��Ŀɼ���־
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

    // ȡ��ĳ���true�ɼ���־
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
