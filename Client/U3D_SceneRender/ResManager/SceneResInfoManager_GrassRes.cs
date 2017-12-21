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
    public List<SceneResInfo> GrassResInfoList = new List<SceneResInfo>();

    /// <summary>
    /// 活动的资源结点
    /// </summary>
    private List<QuadTreeNode> ActiveGrassResParent = new List<QuadTreeNode>();
    private List<QuadTreeLeaves> ActiveGrassResLeafList = new List<QuadTreeLeaves>();

    /// <summary>
    /// 两次检测之间，从活动转成未活动的结点
    /// </summary>
    private List<QuadTreeLeaves> unUsedGrassResLeafList = new List<QuadTreeLeaves>();
    private List<QuadTreeNode> unUsedGrassResParent = new List<QuadTreeNode>();
    private QuadTreeNode GrassResRootParent = null;

    bool isVaildGrassRes(SceneResInfo res)
    {
        if (!res || res.name.Contains("Missing Prefab") ||
            !res.hostRender)
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
        foreach (Material mat in res.hostRender.sharedMaterials)
        {
            if (!mat)
            {
                isNeedToReBakeScene = true;
                return false;
            }
        }
        return true;
    }

    /// <summary>
    /// 构建资源的四叉树
    /// </summary>
    void BuildGrassResQuadTree()
    {
        if (GrassResInfoList.Count > 0)
        {

            GrassResRootParent = new QuadTreeNode();
            GrassResRootParent.Bounds = new Rect(-SceneSize * 0.5f, -SceneSize * 0.5f, SceneSize, SceneSize);

            BuildQuadTree(GrassResRootParent, currGrassResInfoCellSize, currGrassResInfoCellSize);
            InsetGrassResQuadTree();
            BatchesGrassRes();
        }


    }

    /// <summary>
    /// 插入资源四叉树
    /// </summary>
    void InsetGrassResQuadTree()
    {
        foreach (SceneResInfo res in GrassResInfoList)
        {
            if (!isVaildGrassRes(res))
            {
                continue;
            }
            Vector2 pos2D = Vector2.zero;
            pos2D.x = res.RefPos.x;
            pos2D.y = res.RefPos.z;
            QuadTreeNode p = GetQuadTreeParent(GrassResRootParent, pos2D);
            if (null != p)
            {
                QuadTreeLeaves leaf = new QuadTreeLeaves();
                leaf.m_ResInfo = res;
                leaf.m_Parent = p;
                p.m_LeavesList.Add(leaf);
                res.hostTreeNode = p;
                if (!Batches_AllGrassResParentNode.Contains(p))
                {
                    Batches_AllGrassResParentNode.Add(p);
                }
            }
        }
    }


    void UpdateGrassResInfo(int start, int end, List<QuadTreeNode> list)
    {
        if (list.Count > 0)
        {
            for (int i = start; i < end; i++)
            {
                QuadTreeNode parent = list[i];
                UnOptimitzedBaatchesRendering(parent);
            }
        }
    }


    void ProcessUnUsedGrassResInfo(int start, int end, List<QuadTreeNode> list)
    {
        if (list.Count > 0)
        {
            for (int i = start; i < end; i++)
            {
                QuadTreeNode parent = list[i];
                OptimitzedBaatchesRendering(parent);

            }
        }
    }
}