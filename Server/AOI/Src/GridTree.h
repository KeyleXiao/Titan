/*******************************************************************
** 文件名:	GridTree.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			
********************************************************************/

#pragma once

#include "Simple_Atom_Lock.h"

// 是否支持多线程模式
#define  GRIDTREE_MULTITHREAD

#ifdef   GRIDTREE_MULTITHREAD
#	
#endif

/** 
	格子树,多叉树的一种，使用格子树来实现服务期AOI的原因:
	1.格子算法是最直接的空间索引算法
	2.相对R树来说,频繁更新对象坐标,格子算法相对比较稳定,不需要频繁的分裂节点,性能也比较稳定
	3.格子算法的主要问题: 当玩家大量聚集在同一个格子时效率较低,解决方法是当单一格子内对象过多,则对格子进行分裂
	4.格子算法的另外一个问题,无法做到视野的动态变化: 
	  解决方法时,视野管理和对象搜索逻辑分开,这样视野变化也无需对格子的大小进行处理,事实上本身也是2个逻辑,无须偶合
	5.本算法最核心的功能是提供空间对象的近邻搜索,没有使用4叉树或8叉树的原因是减少树的高度

	算法第2版优化：
	1.实际测试，发现当玩家大量聚集时，分裂格子，并不能提高太多效率，有可能效率还会降低
	2.最影响效率的因素在以下两个环节：
	    1).对格子中的对象逐个做距离判断（查询需尽量以格子为单位，避免逐个比较距离）
		2).对格子中的对象逐个遍历，并填充到返回的数组里（把逐个赋值改成memcpy批量拷贝，特别是当1000个对象聚集在同一格子时，
		  可提升效率100倍，前提是不需要逐个比较对象的距离)
	3.基于2的优化得出的性能已经足够满意，唯一的问题是无法做到动态视野，以及精确距离的搜索会比较成问题
	  解决思路有：
	    1).应用中尽量避免,比如服务器尽量不做精确搜索，客户端搜，服务器进行校验
		2).服务器压力不大时精确搜索，压力大时就不对单元格中的点做精确距离比对
		3).关于动态缩小视野减少广播量的问题，可在主线程进行基于单元格的快速搜索，在广播线程再精确判断距离，以减少广播量
		4).服务器视野广播时，初始不是9宫广播，可以是25宫广播，人数太密集时，缩小为9宫广播
	**/

///////////////////////////////////////////////////////////////////////////////////////////////////////
/// 多维格子树实现,因实测证明，动态分裂格子效率上无法提高，所以严格上这个不是一个树结构
//
// @param OBJECT_TYPE: 树中存储的数据类型
// @param COORD_TYPE : 坐标数据类型
// @param DIM        : 几维,默认是2维的
// @param grid_size  : 单个格子的边长
///////////////////////////////////////////////////////////////////////////////////////////////////////
template< typename OBJECT_TYPE,typename COORD_TYPE,int DIM,int grid_size >
class GridTree
{
public:
	class GridNode;  				// 单个格子对象，实现在后面

	COORD_TYPE  origin[DIM];		// 格子原点位置
	COORD_TYPE  grid_num[DIM];      // 每维度格子切分数量,对应的x,y,z就是长宽高方向上的格子数量

	GridNode ** pGrids;	     		// 所有格子节点的指针

    int nLayerNum;
	
	GridTree(int nLayerCount) : nLayerNum(nLayerCount)
	{
        memset( origin,0,sizeof(origin) );
        memset( grid_num,0,sizeof(grid_num) );
        pGrids = 0;
	}

	virtual~GridTree()
	{
		if ( pGrids )
		{
			int total_num = 1;					 // 所有维度总格子数
			for ( int i=0;i<DIM;++i )
			{
				total_num *= grid_num[i];
			}

			for ( int n=0;n<total_num;++n )
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
	}

	bool create( COORD_TYPE loc[DIM],COORD_TYPE g_num[DIM] )
	{
		memcpy( origin,loc,sizeof(origin) );
		memcpy( grid_num,g_num,sizeof(grid_num) );

		int total_num = 1;					 // 所有维度总格子数
		for ( int i=0;i<DIM;++i )
		{
			total_num *= grid_num[i];
		}

		pGrids = (GridNode **)malloc( sizeof(GridNode*)*total_num );
		memset(pGrids,0,sizeof(GridNode*)*total_num);
		return true;
	}

	int loc2index(COORD_TYPE loc[DIM])
	{
		int index = 0;
		int dim_span = 1;

		for ( int i=0;i<DIM;++i )
		{
			int offset = (loc[i]-origin[i])/grid_size;
			if ( offset>=grid_num[i] || offset<0 )				// 超出边界
				return -1;

			index += offset*dim_span;
			dim_span *= grid_num[i];
		}

		return index;
	}

	GridNode * index2node(int index)
	{
		if ( index<0 )
			return 0;

		if ( pGrids[index]==0 )
		{
			pGrids[index] = new GridNode(nLayerNum);
		}

		return pGrids[index];
	}

	// 添加一个对象
	bool insertObject( OBJECT_TYPE & obj,COORD_TYPE loc[DIM],int layer )
	{
		GridNode * pNode = index2node(loc2index(loc));
		if ( pNode )
		{
			pNode->insertObject(obj,layer);
			return true;
		}

		return false;
	}

	// 移除对象
	bool removeObject( OBJECT_TYPE & obj,COORD_TYPE loc[DIM],int layer )
	{
		GridNode * pNode = index2node(loc2index(loc));
		if ( pNode )
		{
			return pNode->removeObject(obj,layer);
		}

		return false;
	}

	// 移动对象
	// @param obj : 要移动的对象
	// 
	bool moveObject( OBJECT_TYPE & obj,COORD_TYPE loc[DIM],COORD_TYPE newloc[DIM],int layer )
	{
		int oldIndex = loc2index(loc);
		int newIndex = loc2index(newloc);

		if ( oldIndex<0 || newIndex<0 )
			return false;

		if ( oldIndex!=newIndex )
		{
			GridNode * pNode = index2node(oldIndex);
			if ( !pNode->removeObject(obj,layer) )
				return false;

			pNode = index2node(newIndex);
			pNode->insertObject(obj, layer);
			return true;
		}

		return true;
	}

	// 取出所有对象
	int getAllObject( OBJECT_TYPE * pReturnArray,int nArraySize, int nLayerMask )
	{
		int count = 0;
		if ( pGrids )
		{
			int total_num = 1;					 // 所有维度总格子数
			for ( int i=0;i<DIM;++i )
			{
				total_num *= grid_num[i];
			}

			for ( int n=0;n<total_num;++n )
			{
				if ( pGrids[n] )
				{
					count += pGrids[n]->getAll( pReturnArray+count,nArraySize-count,nLayerMask );
				}
			}
		}

		return count;
	}

	// 这个只能取出基于单元格的近似数据，如需要精确匹配对象的距离，则需要取出去之后应用层再自行比对
	int k_nearest( COORD_TYPE loc[DIM],COORD_TYPE dist,OBJECT_TYPE * pReturnArray,int nArraySize,int nLayerMask )
	{
		int range[DIM][2];
		grid_range( loc,dist,range );

		int nCount = __k_nearest_loop(loc,range,0,pReturnArray,nArraySize,nLayerMask);
        if(nCount == -1)
        {
            ErrorLn(__FUNCTION__": loc0="<<loc[0]<<",loc1="<<loc[1]<<",dist="<<dist);
            //__debugbreak();
        }
        return nCount;
	}

	int __k_nearest_loop( COORD_TYPE travel_loc[DIM],int range[DIM][2],int dim,OBJECT_TYPE * pReturnArray,int nArraySize,int nLayerMask )
	{
		int total     = 0;

		COORD_TYPE temp_loc[DIM];						// 用个临时变量效率还高点，直接用travel_loc效率还低，不明白原因
		memcpy( temp_loc,travel_loc,sizeof(temp_loc));

		int start = range[dim][0];
		int end = range[dim][1];

		// 参数错误，可能是位置没有初始化，会引起死循环
		if ( start-end > grid_num[dim] )
		{
            static DWORD s_out_tick = getTickCount();
            DWORD dwNowTick = getTickCount();
            if(dwNowTick - s_out_tick > 5000)
            {
                s_out_tick = dwNowTick;
			    ErrorLn("__k_nearest_loop param error! dim="<<dim<<",start="<<start<<",end="<<end<<",start-end = " << (start-end) );
            }
			return -1;
		}

		int index = -1;
		int dimspan = 1;

		for ( int i=start;i<=end;++i )
		{
			if ( i<0 || i>=grid_num[dim] )
				continue;

			temp_loc[dim] = origin[dim]+i*grid_size;
			int count = 0;

			if ( dim<DIM-1 )
			{
				count = __k_nearest_loop( temp_loc,range,dim+1,pReturnArray+total,nArraySize-total,nLayerMask );
			}else
			{
				// 快速取ID
				if ( index<0 ){
					index = loc2index(temp_loc);
					for ( int d=0;d<dim;++d ) dimspan *= grid_num[d];
				}
				else
					index+=dimspan; 	

				// 快速取出格子中的所有对象
				count = index2node(index)->getAll(pReturnArray+total,nArraySize-total,nLayerMask );			
			}

			if ( count>0 )
			{
				total += count;

				if ( total>=nArraySize )		// 满了
					return total;
			}
		}

		return total;
	}

	// 搜索出,a移动到b时,b相对a的增量
	int increment_search(COORD_TYPE oldLoc[DIM],COORD_TYPE newLoc[DIM],COORD_TYPE dist,OBJECT_TYPE * pReturnArray,int nArraySize,int nLayerMask)
	{
		int range_new[DIM][2];		
		int range_old[DIM][2];		
		
		grid_range( newLoc,dist,range_new );
		grid_range( oldLoc,dist,range_old );

		int total = 0;
		int temp  = 0;
		
		for ( int dim=0;dim<DIM;++dim )
		{
			if ( range_new[dim][0]<range_old[dim][0] )
			{
				temp = range_new[dim][1];
				range_new[dim][1] = min(temp,range_old[dim][0]-1);
				total += __k_nearest_loop(newLoc,range_new,0,pReturnArray+total,nArraySize-total,nLayerMask);
				if ( total>=nArraySize )		// 满了
					return total;
				range_new[dim][1] = temp;       // 恢复
			}

			if ( range_new[dim][1]>range_old[dim][1] )
			{
				temp = range_new[dim][0];
				range_new[dim][0]= max(temp,range_old[dim][1]+1);
				total += __k_nearest_loop(newLoc,range_new,0,pReturnArray+total,nArraySize-total,nLayerMask);
				if ( total>=nArraySize )		// 满了
					return total;
				range_new[dim][0] = temp;
			}
		}

		// 是同一个单元格
		if ( memcmp(range_new,range_old,sizeof(range_new))==0 )
			return -1;

		return total;
	}

	// 给出一个点，计算他的格子覆盖范围
	void grid_range( COORD_TYPE loc[DIM],COORD_TYPE dist,int range[DIM][2] )
	{
        /*
		for ( int dim=0;dim<DIM;++dim )
		{
			COORD_TYPE a = (loc[dim]-origin[dim]-dist);
			// 由于格子算法是[a,b)的关系，比如格子大小为10,则0-9为单元格0,10就是单元格1了
			COORD_TYPE b = (loc[dim]-origin[dim]+dist-1);
			range[dim][0] = a/grid_size;
			range[dim][1] = b/grid_size;
		}
        */

        // 暂时用以下代码测试：
        // 上面的计算覆盖范围好像有问题 不同的点计算的覆盖范围会有1个格子的误差。边界时，
        // 导致实体A移动时按照自己的覆盖范围看另外一个实体B是出视野了 会触发互相出视野消息。 
        // 但是另外一个实体B以自己的点计算的覆盖范围看A没出视野 由于B看来A没出视野，B走进时
        // 不会触发互相进视野 导致不再次出视野的话永远进不了视野。
        // 点落在哪个格子按照 loc.x/gird_size是一定的 不会出现同一个点落在不同格子的问题，所以这里不用考虑[a,b)问题?
        for ( int dim=0;dim<DIM;++dim )
        {
            range[dim][0] = (loc[dim] - origin[dim])/grid_size - dist/grid_size;
            range[dim][1] = (loc[dim] - origin[dim])/grid_size + dist/grid_size;  
        }
	}
public:
	/** 
	   格子树的单个格子实现,内部维护了一个数据对象的数组
	*/
	class GridNode
	{
        unsigned short nLayerNum;            // 阵营个数
		unsigned short* pCountArray;      // 对象个数
		unsigned short* pCapArray;		// 数组分配时的空间大小，不够时需要重新分配
		OBJECT_TYPE**  pDataArray;	        // 使用数组的原因，是在查询时可以用memcpy批量拷贝，这将极大提高查询效率

#		ifdef GRIDTREE_MULTITHREAD
		volatile	LONG m_MutexBlock;		// 互斥标志
#		endif

	public:
		GridNode(int nLayerCount) : nLayerNum(nLayerCount), pCountArray(0), pCapArray(0), pDataArray(0) 
		{
            int nSize = sizeof(unsigned short)*nLayerNum; 
            pCountArray = (unsigned short*)malloc( nSize );
            memset( pCountArray,0,nSize );
            pCapArray = (unsigned short*)malloc( nSize );
            memset( pCapArray, 0, nSize );
            nSize = sizeof(OBJECT_TYPE*)*nLayerNum; 
            pDataArray = (OBJECT_TYPE**)malloc( nSize );
            memset(pDataArray, 0, nSize);

#		ifdef GRIDTREE_MULTITHREAD
			m_MutexBlock = 0;
#		endif
		}

		~GridNode()
		{
            if ( pCountArray!=0 )
            {
                free(pCountArray);
                pCountArray = 0;
            }

            if ( pCapArray!=0 )
            {
                free(pCapArray);
                pCapArray = 0;
            }

            if ( pDataArray!= 0)
            {
                for( int i = 0;i<nLayerNum;++i )
                {
                    if ( pDataArray[i] )
                    {
                        delete []pDataArray[i];
                        pDataArray[i] = 0;
                    }
                }

                free(pDataArray);
                pDataArray = 0;
            }
		}

		void insertObject( OBJECT_TYPE & obj,int layer )
		{
            if ( layer<0 || layer>=nLayerNum )
                return;

#			ifdef GRIDTREE_MULTITHREAD
				Simple_Atom_Lock lock(&m_MutexBlock);
#			endif

			if ( pCountArray[layer]>=pCapArray[layer] )
			{
				int grow_num = pCountArray[layer]/4;			// 每次内存不够时递增25%,这个需要配合内存池才能有最高效率
				if ( grow_num<2 ) grow_num = 2;
	
				pCapArray[layer] += grow_num;							
				OBJECT_TYPE* pOld = pDataArray[layer];
				pDataArray[layer] = new OBJECT_TYPE[pCapArray[layer]];
				if ( pOld )
				{
					memcpy(pDataArray[layer],pOld,pCountArray[layer]*sizeof(OBJECT_TYPE));
					delete []pOld;
				}
			}

			pDataArray[layer][pCountArray[layer]++] = obj;
		}

		bool removeObject( OBJECT_TYPE & obj,int layer )
		{
            if ( layer<0 || layer>=nLayerNum )
                return false;

#			ifdef GRIDTREE_MULTITHREAD
			Simple_Atom_Lock lock(&m_MutexBlock);
#			endif

			for ( int i=0;i<pCountArray[layer];++i )
			{
				if ( pDataArray[layer][i]==obj )
				{
					--pCountArray[layer];

					if ( pCountArray[layer]<=pCapArray[layer]/2 )
					{
						pCapArray[layer] = pCountArray[layer];
						OBJECT_TYPE* pOld = pDataArray[layer];
						if ( pCountArray[layer]<=0)
							pDataArray[layer] = 0;
						else
						{
							// 重新分配内存就要重新复制一下
							pDataArray[layer] = new OBJECT_TYPE[pCapArray[layer]];
							if ( i>0 ) 
								memcpy(pDataArray[layer],pOld,i*sizeof(OBJECT_TYPE));
							if ( pCountArray[layer]>i)
								memcpy(pDataArray[layer]+i,pOld+i+1,(pCountArray[layer]-i)*sizeof(OBJECT_TYPE));
						}

						delete []pOld;
					}else
					{
						// 不重新分配内存只需要移动后面半截
						if ( pCountArray[layer]>i)
							memmove(pDataArray[layer]+i,pDataArray[layer]+i+1,(pCountArray[layer]-i)*sizeof(OBJECT_TYPE));
					}

					
					return true;
				}
			}
			return false;
		}

		inline int getAll( OBJECT_TYPE * pReturnArray,int nArraySize, int layerMask )
		{
#			ifdef GRIDTREE_MULTITHREAD
			Simple_Atom_Lock lock(&m_MutexBlock);
#			endif

            int count = 0;
            for ( int cmp=0;cmp<nLayerNum;++cmp )
            {
                if ( CAMP_MASK(cmp)&layerMask )
                {
                    if (pCountArray[cmp]==0)
                        continue;

                    int num = min(nArraySize-count,pCountArray[cmp]);
                    if ( num==0 )
                        break;
                    memcpy(pReturnArray+count,pDataArray[cmp],num*sizeof(OBJECT_TYPE));
                    count += num;
                }
            }
			
			return count;
		}
	};
};
