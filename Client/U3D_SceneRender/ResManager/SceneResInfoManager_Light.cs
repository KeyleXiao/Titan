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
    public List<SceneLightInfo> LightInfoList = new List<SceneLightInfo>();

    /// <summary>
    /// 活动的灯光结点
    /// </summary>
    private List<QuadTreeNode> ActiveLightParent = new List<QuadTreeNode>();
    private List<QuadTreeLeaves> ActiveLightLeafList = new List<QuadTreeLeaves>();

    /// <summary>
    /// 两次检测之间，从活动转成未活动的结点
    /// </summary>
    private List<QuadTreeLeaves> unUsedLightLeafList = new List<QuadTreeLeaves>();
    private List<QuadTreeNode> unUsedLightParent = new List<QuadTreeNode>();
    private QuadTreeNode LightRootParent = null;


    bool isVaildLight(SceneLightInfo res)
    {
        if (!res || res.name.Contains("Missing Prefab") ||
            !res.HostLight)
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
    /// 构建资源的四叉树
    /// </summary>
    void BuildLightQuadTree()
    {
        if (LightInfoList.Count > 0)
        {

            LightRootParent = new QuadTreeNode();
            LightRootParent.Bounds = new Rect(-SceneSize * 0.5f, -SceneSize * 0.5f, SceneSize, SceneSize);

            BuildQuadTree(LightRootParent, currResLightCellSize, currResLightCellSize);
            InsetLightQuadTree();
        }


    }


    /// <summary>
    /// 插入资源四叉树
    /// </summary>
    void InsetLightQuadTree()
    {
        foreach (SceneLightInfo res in LightInfoList)
        {
            if (!isVaildLight(res))
            {
                continue;
            }

            Vector2 pos2D = Vector2.zero;
            pos2D.x = res.transform.position.x;
            pos2D.y = res.transform.position.z;
            QuadTreeNode p = GetQuadTreeParent(LightRootParent, pos2D);
            if (null != p)
            {
                QuadTreeLeaves leaf = new QuadTreeLeaves();
                leaf.m_LightInfo = res;
                leaf.m_Parent = p;
                p.m_LeavesList.Add(leaf);
            }
        }
    }

    void UpdateLightInfo(int start, int end, List<QuadTreeLeaves> list)
    {
        if (list.Count > 0)
        {
            for (int i = start; i < end; i++)
            {
                QuadTreeLeaves leaf = list[i];
                leaf.m_LightInfo.UnOptimitzed();
                //leaf.m_LightInfo.InfoUpdate();
            }
        }
    }

    void ProcessUnUsedLightInfo(int start, int end, List<QuadTreeLeaves> list)
    {
        if (list.Count > 0)
        {
            for (int i = start; i < end; i++)
            {
                QuadTreeLeaves leaf = list[i];
                leaf.m_LightInfo.Optimitzed();
            }
        }
    }

}