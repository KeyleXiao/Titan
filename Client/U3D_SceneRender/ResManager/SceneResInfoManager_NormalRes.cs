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
    public List<SceneResInfo> ResInfoList = new List<SceneResInfo>();

    /// <summary>
    /// 活动的资源结点
    /// </summary>
    private List<QuadTreeNode> ActiveResParent = new List<QuadTreeNode>();
    private List<QuadTreeLeaves> ActiveResLeafList = new List<QuadTreeLeaves>();

    /// <summary>
    /// 两次检测之间，从活动转成未活动的结点
    /// </summary>
    private List<QuadTreeLeaves> unUsedResLeafList = new List<QuadTreeLeaves>();
    private List<QuadTreeNode> unUsedResParent = new List<QuadTreeNode>();
    private QuadTreeNode ResRootParent = null;

    /// <summary>
    /// 总是可见的Res，与非总是可见的分开来管理
    /// </summary>
    private QuadTreeNode AlwaysVisibleResRootParent = null;

    private List<QuadTreeNode> ActiveAlwaysVisibleResParent = new List<QuadTreeNode>();
    private List<QuadTreeLeaves> ActiveAlwaysVisibleResLeafList = new List<QuadTreeLeaves>();

    /// <summary>
    /// 两次检测之间，从活动转成未活动的结点
    /// </summary>
    private List<QuadTreeLeaves> unUsedAlwaysVisibleResLeafList = new List<QuadTreeLeaves>();
    private List<QuadTreeNode> unUsedAlwaysVisibleResParent = new List<QuadTreeNode>();


    bool isVaildRes(SceneResInfo res)
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

    bool isVaildNormalRes(SceneResInfo res)
    {
        if (isVaildRes(res))
        {
            if (!res.AlwaysVisible)
            {
                return true;
            }
        }
        return false;
    }

    bool isVaildAlwaysVisibleRes(SceneResInfo res)
    {
        if (isVaildRes(res))
        {
            if (res.AlwaysVisible)
            {
                return true;
            }
        }
        return false;
    }

    /// <summary>
    /// 构建资源的四叉树
    /// </summary>
    void BuildResQuadTree()
    {
        if (ResInfoList.Count > 0)
        {

            ResRootParent = new QuadTreeNode();
            ResRootParent.Bounds = new Rect(-SceneSize * 0.5f, -SceneSize * 0.5f, SceneSize, SceneSize);

            BuildQuadTree(ResRootParent, currResInfoCellSize, currResInfoCellSize);
            InsetResQuadTree();
            BatchesRes();

            AlwaysVisibleResRootParent = new QuadTreeNode();
            AlwaysVisibleResRootParent.Bounds = new Rect(-SceneSize * 0.5f, -SceneSize * 0.5f, SceneSize, SceneSize);
            BuildQuadTree(AlwaysVisibleResRootParent, currAlwaysVisibleResInfoCellSize, currAlwaysVisibleResInfoCellSize);
            InsetAlwaysVisibleResQuadTree();
        }


    }

    /// <summary>
    /// 插入资源四叉树
    /// </summary>
    void InsetResQuadTree()
    {
        foreach (SceneResInfo res in ResInfoList)
        {
            if (!isVaildNormalRes(res))
            {
                continue;
            }
            Vector2 pos2D = Vector2.zero;
            pos2D.x = res.RefPos.x;
            pos2D.y = res.RefPos.z;
            QuadTreeNode p = GetQuadTreeParent(ResRootParent, pos2D);
            if (null != p)
            {
                QuadTreeLeaves leaf = new QuadTreeLeaves();
                leaf.m_ResInfo = res;
                leaf.m_Parent = p;
                p.m_LeavesList.Add(leaf);
                res.hostTreeNode = p;
                if (!Batches_AllResParentNode.Contains(p))
                {
                    Batches_AllResParentNode.Add(p);
                }
            }
        }
    }


    void UpdateResInfo(int start, int end, List<QuadTreeNode> list)
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


    void ProcessUnUsedResInfo(int start, int end, List<QuadTreeNode> list)
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


    /// <summary>
    /// 插入总是可见的资源四叉树，与普通的分开管理
    /// </summary>
    void InsetAlwaysVisibleResQuadTree()
    {
        foreach (SceneResInfo res in ResInfoList)
        {
            if (!isVaildAlwaysVisibleRes(res))
            {
                continue;
            }
            Vector2 pos2D = Vector2.zero;
            pos2D.x = res.RefPos.x;
            pos2D.y = res.RefPos.z;

            QuadTreeNode p = GetQuadTreeParent(AlwaysVisibleResRootParent, pos2D);
            if (null != p)
            {
                QuadTreeLeaves leaf = new QuadTreeLeaves();
                res.Init(null, SceneCamera);
                leaf.m_ResInfo = res;
                leaf.m_Parent = p;
                res.hostTreeNode = p;
                p.m_LeavesList.Add(leaf);
            }
        }
    }

    void UpdateAlwaysVisibleResInfo(int start, int end, List<QuadTreeLeaves> list)
    {
        if (list.Count > 0)
        {
            for (int i = start; i < end; i++)
            {
                QuadTreeLeaves leaf = list[i];
                leaf.m_ResInfo.UnOptimitzedRendering(null);
                leaf.m_ResInfo.OnUpdate(SceneCamera);
            }
        }
    }


    void ProcessUnUsedAlwaysVisibleResInfo(int start, int end, List<QuadTreeLeaves> list)
    {
        if (list.Count > 0)
        {
            for (int i = start; i < end; i++)
            {
                QuadTreeLeaves leaf = list[i];
                leaf.m_ResInfo.OptimitzedRendering(null);
                leaf.m_ResInfo.OnUpdate(SceneCamera);
            }
        }
    }


}