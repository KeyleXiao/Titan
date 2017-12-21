/*******************************************************************
** 文件名:	IFinder.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-3-10
** 版  本:	1.0
** 描  述:	搜索接口

		
********************************************************************/
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/// <summary>
/// 搜索接口
/// </summary>
public interface IFinder
{
    // 创建
    bool Create(GameLogic.game_LoadSceneInfo info);

    // 释放
    void Release();

    // 添加一个对象
    bool InsertObject(ref U3D_Render.EntityView ev, UnityEngine.Vector3 loc);

	// 移除对象
    bool RemoveObject(ref U3D_Render.EntityView ev);

	// 移动对象
    bool MoveObject(ref U3D_Render.EntityView ev, UnityEngine.Vector3 newloc);

	// 取某点周围一定范围内的格子内的所有实体 
    IList<U3D_Render.EntityView> Nearest(UnityEngine.Vector3 loc, int dist, SPELL.CampFlag flag);
}
