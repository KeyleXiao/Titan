/*******************************************************************
** 文件名:	MapFinder.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-3-15
** 版  本:	1.0
** 描  述:	地图实体搜索

		
********************************************************************/
using System;
using System.Collections;
using System.Collections.Generic;
using U3D_Render;


public class MapFinder : IFinder
{
    GridTree m_gridTree;       // 各阵营对象分布
    UnityEngine.Vector3 m_Offset;


    // 让浮点坐标系标准化，乘系数转成整型
    private UnityEngine.Vector3 AdjustLoc(UnityEngine.Vector3 fRunLoc)
    {
        return (fRunLoc + m_Offset);
    }

    
    public bool Create(GameLogic.game_LoadSceneInfo info)
    {
        m_Offset = new UnityEngine.Vector3(info.fOffx, info.fOffy, info.fOffz);

        int[] grid = new int[2];
        grid[0] = info.nXAxisLen / MapFinderManager.DEFAULT_GRID_SIZE + 1;
        grid[1] = info.nZAxisLen / MapFinderManager.DEFAULT_GRID_SIZE + 1;

        m_gridTree = new GridTree();
        m_gridTree.create(UnityEngine.Vector3.zero, grid, MapFinderManager.DEFAULT_GRID_SIZE);

        return true;
    }

    // 释放
    public void Release()
    {
        m_gridTree.Release();
        m_gridTree = null;
    }

    // 添加一个对象
    public bool InsertObject(ref U3D_Render.EntityView ev,  UnityEngine.Vector3  loc)
    {
        // 暂时移除一次 看是否有重复插入
        bool bResult = m_gridTree.removeObject(ref ev, ev.LastGridLoc);
        if (bResult)
        {
            Trace.LogWarning("repeate InsertObject, id=" + ev.ID);
        }

        UnityEngine.Vector3 newLoc = AdjustLoc(loc);

        // 第一次初始化上一次坐标值
        ev.LastGridLoc = newLoc;

        return m_gridTree.insertObject(ref ev, newLoc);
    }

	// 移除对象
    public bool RemoveObject(ref U3D_Render.EntityView ev)
    {
        bool bResult = m_gridTree.removeObject(ref ev, ev.LastGridLoc);

        if (bResult)
        {
            ev.LastGridLoc = UnityEngine.Vector3.zero;
        }
        else if (ev.StateMachine.m_OnViewVisible || ev.LastGridLoc != UnityEngine.Vector3.zero)
        {
            Trace.LogError("RemoveObject failed, id=" + ev.ID);
        }
        return bResult;
    }

	// 移动对象
    public bool MoveObject(ref U3D_Render.EntityView ev,  UnityEngine.Vector3 newloc)
    {
        UnityEngine.Vector3 normalLoc = AdjustLoc(newloc);

        bool bResult = m_gridTree.moveObject(ref ev, ev.LastGridLoc, normalLoc);

        if (!bResult)
        {
            if (ev.StateMachine.m_OnViewVisible || ev.LastGridLoc != UnityEngine.Vector3.zero)
            {
                Trace.LogError("MoveObject failed, id=" + ev.ID);
            }
          
            return bResult;
        }
        
        // 更新上一次坐标值
        ev.LastGridLoc = normalLoc;

        return bResult;
    }

	// 取某点周围一定范围内的格子内的所有实体 
    public IList<U3D_Render.EntityView> Nearest(UnityEngine.Vector3 loc, int dist, SPELL.CampFlag flag)
    {
        UnityEngine.Vector3 normalLoc = AdjustLoc(loc);

        List<U3D_Render.EntityView> result = new List<EntityView>();
        m_gridTree.k_nearest(flag, normalLoc, dist, ref result);

        return result;
    }


}