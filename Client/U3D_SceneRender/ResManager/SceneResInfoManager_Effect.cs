/*******************************************************************
** 文件名:	SceneResInfoManager.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/7/20
** 版  本:	1.0
** 描  述:	场景资源信息 管理器
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
    public List<SceneEffectInfo> EffectInfoList = new List<SceneEffectInfo>();

    /// 活动的碰撞结点
    /// </summary>
    private List<QuadTreeNode> ActiveEffectParent = new List<QuadTreeNode>();
    private List<QuadTreeLeaves> ActiveEffectLeafList = new List<QuadTreeLeaves>();

    /// <summary>
    /// 两次检测之间，从活动转成未活动的结点
    /// </summary>
    private List<QuadTreeLeaves> unUsedEffectLeafList = new List<QuadTreeLeaves>();
    private List<QuadTreeNode> unUsedEffectParent = new List<QuadTreeNode>();
    private QuadTreeNode EffectRootParent = null;

    bool isVaildEffect(SceneEffectInfo res)
    {
        if (!res || res.name.Contains("Missing Prefab"))
        {
            isNeedToReBakeScene = true;
            return false;
        }
        if (!res.enabled)
        {
            return false;
        }

        //如果层次视图不可用
        if (!res.gameObject.activeInHierarchy)
        {
            //看看root的active，如果active是可用的，那么这个是不可用的
            if (SceneRoot && SceneRoot.activeSelf)
            {
                return false;
            }

        }
        return true;
    }

    /// <summary>
    /// 构建Effect四叉树
    /// </summary>
    void BuildEffectQuadTree()
    {
        if (EffectInfoList.Count > 0)
        {

            EffectRootParent = new QuadTreeNode();
            EffectRootParent.Bounds = new Rect(-SceneSize * 0.5f, -SceneSize * 0.5f, SceneSize, SceneSize);


            BuildQuadTree(EffectRootParent, currEffectCellSize, currEffectCellSize);
            InsetEffectQuadTree();
        }


    }


    /// <summary>
    /// 插入碰撞盒四叉树
    /// </summary>
    void InsetEffectQuadTree()
    {
        foreach (SceneEffectInfo res in EffectInfoList)
        {
            if (!isVaildEffect(res))
            {
                continue;
            }

            Vector2 pos2D = Vector2.zero;

            pos2D.x = res.transform.position.x;
            pos2D.y = res.transform.position.z;

            QuadTreeNode p = GetQuadTreeParent(EffectRootParent, pos2D);
            if (null != p)
            {
                QuadTreeLeaves leaf = new QuadTreeLeaves();
                leaf.m_EffectInfo = res;
                leaf.m_Parent = p;
                p.m_LeavesList.Add(leaf);
            }

            if (res.isAlwaysNormal)
            {
                res.UnOptimitzed();
            }
        }
    }

    void UpdateEffectInfo(int start, int end, List<QuadTreeLeaves> list)
    {
        if (list.Count > 0)
        {
            for (int i = start; i < end; i++)
            {
                QuadTreeLeaves leaf = list[i];
                leaf.m_EffectInfo.OnUpdate(SceneCamera);
            }
        }
    }

    void ProcessUnUsedEffectInfo(int start, int end, List<QuadTreeLeaves> list)
    {
        if (list.Count > 0)
        {
            for (int i = start; i < end; i++)
            {
                QuadTreeLeaves leaf = list[i];
                leaf.m_EffectInfo.Optimitzed();
            }
        }
    }
}