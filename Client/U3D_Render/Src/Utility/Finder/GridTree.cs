/*******************************************************************
** 文件名:	GridTree.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-3-10
** 版  本:	1.0
** 描  述:	格子树
            依据服务器的改造成C#版
		
********************************************************************/
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
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;


using UID = System.Int32;

public class GridTree
{
    #region GridNode
    internal class GridNode
    {
        enum INNER_CAMP
        {
            CAMP_SELF = 0,
            CAMP_ENEMY,
            CAMP_NEUTRAL,
            CAMP_MAX,
        }

        // 对象个数
        ushort nChildCount;
        // 所有阵营的对象列表
        Dictionary<UID, U3D_Render.EntityView> m_allObjList;
        // 每个阵营中的对象列表(0.自己方，1.敌方，2.中立)
        Dictionary<INNER_CAMP, Dictionary<UID, U3D_Render.EntityView>> m_campObjList;


        // 获取阵营序号
        private INNER_CAMP getCampIndex(SPELL.CampFlag flag)
        {
            if (flag == SPELL.CampFlag.CampFlag_Friend || flag == SPELL.CampFlag.CampFlag_Self)
                return INNER_CAMP.CAMP_SELF;

            if (flag == SPELL.CampFlag.CampFlag_Enemy)
                return INNER_CAMP.CAMP_ENEMY;

            return INNER_CAMP.CAMP_NEUTRAL;
        }

        internal GridNode()
        {
            nChildCount = 0;
            m_allObjList = new Dictionary<UID, U3D_Render.EntityView>();

            m_campObjList = new Dictionary<INNER_CAMP, Dictionary<UID,U3D_Render.EntityView>>();

            for (INNER_CAMP camp = INNER_CAMP.CAMP_SELF; camp < INNER_CAMP.CAMP_MAX; camp++)
            {
                m_campObjList[camp] = new Dictionary<UID, U3D_Render.EntityView>();
            }
        }
        


        internal void insertObject(ref U3D_Render.EntityView ev)
        {
            UID id = ev.ID;
            m_allObjList[id] = ev;
            m_campObjList[getCampIndex(ev.CampFlag)][id] = ev;

            ++nChildCount;
        }

        internal bool removeObject(ref U3D_Render.EntityView ev)
        {
            UID id = ev.ID;

            if (!m_allObjList.ContainsKey(id))
                return false;

            m_allObjList.Remove(id);
            m_campObjList[getCampIndex(ev.CampFlag)].Remove(id);

            --nChildCount;
            return true;
        }

        internal Dictionary<UID, U3D_Render.EntityView> getCampEntities(SPELL.CampFlag flag)
        {
            return m_campObjList[getCampIndex(flag)];
        }

        internal Dictionary<UID, U3D_Render.EntityView> getAll()
        {
            return m_allObjList;
        }
    }
    #endregion

    const int DIM = 2;
    int[]           m_arrOrigin;		    // 格子原点位置
    int[]           m_arrGridNum;          // 每维度格子切分数量,对应的x,y,z就是长宽高方向上的格子数量
    GridNode[]      m_arrGridNode;
    int m_gridSize;


    public bool create(UnityEngine.Vector3 loc, int[] grid_num, int gridSize)
	{
        m_arrOrigin = new int[DIM];		        // 格子原点位置
        m_arrGridNum = new int[DIM];            // 每维度格子切分数量,对应的x,y,z就是长宽高方向上的格子数量

        m_arrOrigin[0] = (int)loc.x; m_arrOrigin[1] = (int)loc.y;
        m_arrGridNum = grid_num;

        m_gridSize = gridSize;

		int total_num = 1;					 // 所有维度总格子数
        for (int i = 0; i < DIM; ++i)
		{
            total_num *= grid_num[i];
		}

        m_arrGridNode = new GridNode[total_num];
           
        return true;
	}

    public void Release()
    {
        m_arrOrigin = null;         // 格子原点位置
        m_arrGridNum = null;        // 每维度格子切分数量,对应的x,y,z就是长宽高方向上的格子数量
        m_arrGridNode = null;
        m_gridSize = 0;
    }

    int loc2index(UnityEngine.Vector3 v)
	{
		int index = 0;
		int dim_span = 1;

        int[] loc = new int[2];
        loc[0] = (int)v.x;
        loc[1] = (int)v.z;
        for (int i = 0; i < DIM; ++i)
		{
            int offset = (loc[i] - m_arrOrigin[i]) / m_gridSize;
            if (offset >= m_arrGridNum[i] || offset < 0)				// 超出边界
				return -1;

			index += offset*dim_span;
            dim_span *= m_arrGridNum[i];
		}

		return index;
	}

    GridNode index2node(int index)
	{
		if ( index<0 )
			return null;

		if ( m_arrGridNode[index]==null )
		{
            m_arrGridNode[index] = new GridNode();
		}

		return m_arrGridNode[index];
	}

    	// 添加一个对象
    public bool insertObject(ref U3D_Render.EntityView ev, UnityEngine.Vector3 loc)
	{
		GridNode pNode = index2node(loc2index(loc));
		if ( pNode != null )
		{
			pNode.insertObject(ref ev);

			return true;
		}

		return false;
	}

	// 移除对象
    public bool removeObject(ref U3D_Render.EntityView ev, UnityEngine.Vector3 loc)
	{
		GridNode pNode = index2node(loc2index(loc));
		if ( pNode != null)
            return pNode.removeObject(ref ev);

		return false;
	}

	// 移动对象
    public bool moveObject(ref U3D_Render.EntityView ev, UnityEngine.Vector3 loc, UnityEngine.Vector3 newloc)
	{
		int oldIndex = loc2index(loc);
		int newIndex = loc2index(newloc);

		if ( oldIndex<0 || newIndex<0 )
			return false;

		if ( oldIndex!=newIndex )
		{
            if (!index2node(oldIndex).removeObject(ref ev))
				return false;

            GridNode pNode = index2node(newIndex);
            if (pNode == null)
                return false;

            pNode.insertObject(ref ev);

			return true;
		}

		return true;
	}

    // 给出一个点，计算他的格子覆盖范围
	void grid_range( int[] loc,int dist,int[,] range )
	{
		for ( int dim=0; dim<DIM; ++dim )
		{
			int a = (loc[dim]-m_arrOrigin[dim]-dist);
			// 由于格子算法是[a,b)的关系，比如格子大小为10,则0-9为单元格0,10就是单元格1了
			int b = (loc[dim]-m_arrOrigin[dim]+dist-1);

			range[dim,0] = a/m_gridSize;
			range[dim,1] = b/m_gridSize;
		}
	}

	// 这个只能取出基于单元格的近似数据，如需要精确匹配对象的距离，则需要取出去之后应用层再自行比对
    public int k_nearest(SPELL.CampFlag flag, UnityEngine.Vector3 v, int dist, ref List<U3D_Render.EntityView> list)
	{
		int[,] range = new int[DIM,2];
        int[] loc = new int[2];
        loc[0] = (int)v.x;
        loc[1] = (int)v.z;
		grid_range( loc,dist,range );

        return __k_nearest_loop(flag, loc, range, 0, ref list);
	}

    int __k_nearest_loop(SPELL.CampFlag flag, int[] travel_loc, int[,] range, int dim, ref List<U3D_Render.EntityView> list)
	{
		int[] temp_loc = new int[DIM];						// 用个临时变量效率还高点，直接用travel_loc效率还低，不明白原因
        temp_loc = travel_loc;

		int start = range[dim,0];
		int end = range[dim,1];

		int index = -1;
		int dimspan = 1;

		for ( int i=start; i<=end; ++i )
		{
			if ( i<0 || i>= m_arrGridNum[dim] )
				continue;

			temp_loc[dim] = m_arrOrigin[dim] + i* m_gridSize;
			int count = 0;

			if ( dim<DIM-1 )
			{
                count = __k_nearest_loop(flag, temp_loc, range, dim + 1, ref list);
			}
            else
			{
                if (index < 0)
                {
                    index = loc2index(new UnityEngine.Vector3(temp_loc[0], 0, temp_loc[1]));

                    for (int d = 0; d < dim; ++d)
                        dimspan *= m_arrGridNum[d];
                }
                else
                {
                    index += dimspan;
                }
                // 快速取出格子中的所有对象
                if (flag == SPELL.CampFlag.CampFlag_Unknow)
                {
                    Dictionary<UID, U3D_Render.EntityView> result = index2node(index).getAll();
                    if (result.Count > 0)
                    {
                        list.AddRange(result.Values.ToList<U3D_Render.EntityView>());
                    }
                }
                else
                {
                    Dictionary<UID, U3D_Render.EntityView> result = index2node(index).getCampEntities(flag);
                    if (result.Count > 0)
                    {
                        list.AddRange(result.Values.ToList<U3D_Render.EntityView>());
                    }
                }

			}
		}

        return list.Count;
	}



}
