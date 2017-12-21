/*******************************************************************
** 文件名:	SceneResInfoManager_Debug.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/7/20
** 版  本:	1.0
** 描  述:	场景资源信息 管理器,调试部分
** 应  用:   场景资源信息，用于做场景资源管理

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public partial class SceneResInfoManager : MonoBehaviour
{
    public static void DrawRect(Rect r, float y, Color c, float padding = 0.0f)
    {
        Debug.DrawLine(new Vector3(r.xMin + padding, y, r.yMin + padding), new Vector3(r.xMin + padding, y, r.yMax - padding), c);
        Debug.DrawLine(new Vector3(r.xMin + padding, y, r.yMin + padding), new Vector3(r.xMax - padding, y, r.yMin + padding), c);
        Debug.DrawLine(new Vector3(r.xMax - padding, y, r.yMax - padding), new Vector3(r.xMin + padding, y, r.yMax - padding), c);
        Debug.DrawLine(new Vector3(r.xMax - padding, y, r.yMax - padding), new Vector3(r.xMax - padding, y, r.yMin + padding), c);
    }


    void DrawBounds(QuadTreeNode p, List<QuadTreeNode> ActiveList)
    {

        DrawAll(p);
        DrawActive(ActiveList);
       
    }

    void DrawAll(QuadTreeNode p)
    {
        if(null == p)
        {
            return;
        }
        DrawRect(p.Bounds, 5.0f, Color.white);
        if (p.ChildCount > 0)
        {
            //继续遍历剩下的结点
            foreach (QuadTreeNode p1 in p.ChildNode)
            {
                DrawAll(p1);
            }
        }
    }

    void DrawActive(List<QuadTreeNode> ActiveList)
    {
        foreach (QuadTreeNode p in ActiveList)
        {
            DrawRect(p.Bounds, 8.0f, Color.red);
        }
       
    }

    public int showType = -1;
    //public void Update()
    //{
    //    if (!Application.isEditor)
    //    {
    //        return;
    //    }
    //    if (showType == 0)
    //    {
    //        DrawBounds(ColliderRootParent, ActiveColliderParent);
    //    }

    //    if (showType == 1)
    //    {
    //        DrawBounds(ResRootParent, ActiveResParent);
    //    }

    //    if (showType == 2)
    //    {
    //        DrawBounds(LightRootParent, ActiveLightParent);
    //    }

    //    if (showType == 3)
    //    {
    //        DrawBounds(EffectRootParent, ActiveEffectParent);
    //    }

    //    if (showType == 4)
    //    {
    //        DrawBounds(GrassResRootParent, ActiveGrassResParent);
    //    }
    //}
}
