/*******************************************************************
** 文件名:	MapFinderManager.cs 
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


public class MapFinderManager 
{
    public const int DEFAULT_GRID_SIZE = 4;
    static IFinder m_finder = null;

    static MapFinderManager()
    {
        
    }

    // 创建
    public static bool Create(GameLogic.game_LoadSceneInfo info)
    {
        // TODO 临时用来处理
        //if(info.nMapID != 5)
        //{
        //    return false;
        //}
        if (m_finder != null )
        {
            Release();
        }
        m_finder = new MapFinder();
        return m_finder.Create(info);
    }

    // 释放
    public static void Release()
    {
        m_finder.Release();
        m_finder = null;
    }

    // 是否排除外实体
    private static bool CheckException(ref U3D_Render.EntityView ev)
    {
        //if(ev.Type == ENTITY_TYPE.TYPE_MONSTER && ev.Property != null)
        //{
        //    // 怪物脚本中的怪物类型
        //    MONSTER_TYPE subType = (MONSTER_TYPE)ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
        //    // 战场细化怪物类型
        //    //War.EWarMonsterExec warSubType = (War.EWarMonsterExec)ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SEX);
        //    // 野怪监视器,动态障碍怪
        //    return (subType == MONSTER_TYPE.MONSTER_TYPE_WILD_MONITOR || subType == MONSTER_TYPE.MONSTER_TYPE_OBSTACLE);
        //}

        return false;
    }

    // 添加一个对象
    public static bool InsertObject(ref U3D_Render.EntityView ev, UnityEngine.Vector3 loc)
	{
        if (m_finder == null)
            return false;

        if (CheckException(ref ev))
            return false;
    
        return m_finder.InsertObject(ref ev, loc);
	}

	// 移除对象
    public static bool RemoveObject(ref U3D_Render.EntityView ev)
	{
        if (m_finder == null)
            return false;

        bool bFlag = m_finder.RemoveObject(ref ev);
        return bFlag;
	}

	// 移动对象
    public static bool MoveObject(ref U3D_Render.EntityView ev, UnityEngine.Vector3 newloc)
    {
        if (m_finder == null)
            return false;

        return m_finder.MoveObject(ref ev, newloc);
    }

    // --------------------------------------------------------------------------------------------------- //
	// 取某点周围一定范围内的格子内的所有实体 
	// @param dist : 距离半径(注意，这里不是精确匹配的，是把一个单元格内的所有对象都返回,误差为单元格大小)
	// --------------------------------------------------------------------------------------------------- //
    public static IList<EntityView> Nearest(UnityEngine.Vector3 loc, int dist, SPELL.CampFlag flag)
	{
        if (m_finder == null)
            return null;

        return m_finder.Nearest(loc, dist, flag);
	}

}