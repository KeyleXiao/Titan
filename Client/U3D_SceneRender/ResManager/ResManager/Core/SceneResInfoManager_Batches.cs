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

    /// <summary>
    /// 一个树节点，公用一个FadeOut
    /// </summary>
    public Dictionary<QuadTreeNode, Effect.SceneFadeInOutMaterialFloat> Batches_TreeNodeFadeTable = new Dictionary<QuadTreeNode, Effect.SceneFadeInOutMaterialFloat>();


    /// <summary>
    /// 一个树节点，公用一个FadeOut,反表
    /// </summary>
    public Dictionary<Effect.SceneFadeInOutMaterialFloat, QuadTreeNode> InvBatches_TreeNodeFadeTable = new Dictionary<Effect.SceneFadeInOutMaterialFloat, QuadTreeNode>();

    /// <summary>
    /// 所有有用树叶结点的树节点，不包括alwaysvisible的结点
    /// </summary>
    private HashSet<QuadTreeNode> Batches_AllResParentNode = new HashSet<QuadTreeNode>();

    /// <summary>
    /// 所有有用树叶结点的树节点，不包括alwaysvisible的结点
    /// </summary>
    private HashSet<QuadTreeNode> Batches_AllGrassResParentNode = new HashSet<QuadTreeNode>();

    private static float FadeTime = 1.0f;
    void ClearBatchesRes()
    {
        foreach (Effect.SceneFadeInOutMaterialFloat lis in Batches_TreeNodeFadeTable.Values)
        {
            lis.InvokeFinishEvent();
            lis.ClearMaterialInstance();
            lis.ClearFinishEvent();
            Effect.SceneFadeInOutMaterialFloat temp = lis;
            ResNode.DestroyRes(ref temp);
        }
        foreach (QuadTreeNode m_Parent in Batches_AllResParentNode)
        {

            foreach (Material mat in m_Parent.FadeMaterialList)
            {
                DestroyImmediate(mat);
            }
            m_Parent.FadeMaterialList.Clear();
            m_Parent.FadeMaterialList = null;

        }
        InvBatches_TreeNodeFadeTable.Clear();
        Batches_TreeNodeFadeTable.Clear();

        Batches_AllGrassResParentNode.Clear();
        Batches_AllResParentNode.Clear();
    }


    void BatchesRes()
    {
        foreach(QuadTreeNode m_Parent in Batches_AllResParentNode)
        {
            BuildBatches(m_Parent);
        }
    }

    void BatchesGrassRes()
    {
        foreach (QuadTreeNode m_Parent in Batches_AllGrassResParentNode)
        {
            BuildBatches(m_Parent);
        }
    }

    /// <summary>
    /// 插入到batches,batches是以一个QuadTreeNode为单位
    /// </summary>
    /// <param name="parent"></param>
    void BuildBatches(QuadTreeNode parent)
    {
        //如果没存在淡入淡出
        if(!Batches_TreeNodeFadeTable.ContainsKey(parent))
        {
            Effect.SceneFadeInOutMaterialFloat m_FadeFloat = gameObject.AddComponent<Effect.SceneFadeInOutMaterialFloat>();
            m_FadeFloat.PropertyName = SceneResInfo.CutOutKeyWorld;
            m_FadeFloat.RegisterFinishEvent(OnBatchesFadingFinish);
            m_FadeFloat.FadeOutDuartion = FadeTime;
            m_FadeFloat.FadeOutFrom = 1.0f;
            m_FadeFloat.FadeOutTo = 0.0f;
            m_FadeFloat.FadeTimer = m_FadeFloat.FadeOutFrom;
            m_FadeFloat.enabled = false;

            
            List<Material> FadeInstanceList = new List<Material>();
            Dictionary<Material, int> matIndexTable = new Dictionary<Material,int>();
            Dictionary<Material,Material> matFadeInstanceTable = new Dictionary<Material,Material>();
            foreach (QuadTreeLeaves m_Leaves in parent.m_LeavesList)
            {
                SceneResInfo res = m_Leaves.m_ResInfo;
                //回调，给到每个具体的类里,恢复原本的材质球
                m_FadeFloat.RegisterFinishEvent(res.OnFadeFinish);

                if (res.CheckSupportFading())
                {
                    Material[] shareds = res.hostRender.sharedMaterials;
                    int sharedCount = shareds.Length;
                    for (int i = 0; i < sharedCount; i++)
                    {
                        //如果这是一个全新的材质球,就新建一个，然后加入的实例链表里
                        if (!matIndexTable.ContainsKey(shareds[i]))
                        {
                            Material matFadeIn = SceneResInfo.CreateFadeingMaterial(shareds[i]);
                            FadeInstanceList.Add(matFadeIn);
                            matFadeInstanceTable.Add(shareds[i], matFadeIn);
                            matIndexTable.Add(shareds[i], FadeInstanceList.Count - 1);
                            res.BatchesIDs.Add(FadeInstanceList.Count - 1);

                        }
                        else
                        {

                            res.BatchesIDs.Add(matIndexTable[shareds[i]]);
                        }
                    }

                }

            }

            foreach (QuadTreeLeaves m_Leaves in parent.m_LeavesList)
            {
                SceneResInfo res = m_Leaves.m_ResInfo;
                res.Init(matFadeInstanceTable, SceneCamera);
            }

            parent.FadeMaterialList = FadeInstanceList;
            parent.isDistanceFadingIn = false;
            parent.isDistanceFadingIn = false;
            parent.bOpmitizedRendering = true;
            InvBatches_TreeNodeFadeTable.Add(m_FadeFloat, parent);
            Batches_TreeNodeFadeTable.Add(parent, m_FadeFloat);
        }
        
    }


    void OnBatchesFadingFinish(Effect.SceneFadeInOutMaterialFloat FadeInOutMaterialFloat)
    {
        FadeInOutMaterialFloat.enabled = false;
        InvBatches_TreeNodeFadeTable[FadeInOutMaterialFloat].isDistanceFadingIn = false;
        InvBatches_TreeNodeFadeTable[FadeInOutMaterialFloat].isDistanceFadingOut = false;
    }


    void OptimitzedBaatchesRendering(QuadTreeNode parent)
    {
        if(!parent.bOpmitizedRendering)
        {
            Effect.SceneFadeInOutMaterialFloat m_FadeFloat = null;

            parent.bOpmitizedRendering = true;

            int length = parent.m_LeavesList.Count;
            for (int j = 0; j < length; j++)
            {
                QuadTreeLeaves leaf = parent.m_LeavesList[j];
                leaf.m_ResInfo.OptimitzedMemory();
                leaf.m_ResInfo.OptimitzedRendering(parent.FadeMaterialList);
            }



            if (Batches_TreeNodeFadeTable.TryGetValue(parent, out m_FadeFloat))
            {
                m_FadeFloat.SetMaterialInstance(parent.FadeMaterialList);
                m_FadeFloat.enabled = true;

                parent.isDistanceFadingOut = true;

                if (!parent.isDistanceFadingIn)
                {
                    m_FadeFloat.FadeOutFrom = 0.0f;
                    m_FadeFloat.FadeOutTo = 1.0f;
                    m_FadeFloat.FadeOutDuartion = FadeTime;
                    m_FadeFloat.Play();
                }
                else
                {

                    m_FadeFloat.FadeOutDuartion = m_FadeFloat.FadeTimer;
                    m_FadeFloat.FadeOutFrom = m_FadeFloat.currentFloat;
                    m_FadeFloat.FadeOutTo = 1;
                    m_FadeFloat.FadeTimer = 0;
                    m_FadeFloat.Update();
                    m_FadeFloat.ReStartFadeOutFinish_Constant();

                }

                parent.isDistanceFadingIn = false;
            }


        }

    }



    void UnOptimitzedBaatchesRendering(QuadTreeNode parent)
    {
        if (parent.bOpmitizedRendering)
        {
            Effect.SceneFadeInOutMaterialFloat m_FadeFloat = null;
            parent.bOpmitizedRendering = false;

            int length = parent.m_LeavesList.Count;
            for (int j = 0; j < length; j++)
            {
                QuadTreeLeaves leaf = parent.m_LeavesList[j];
                leaf.m_ResInfo.UnOptimitzedRendering(parent.FadeMaterialList);
                //leaf.m_ResInfo.UnOptimitzedMemory();
                leaf.m_ResInfo.OnUpdate(SceneCamera);
            }

            if (Batches_TreeNodeFadeTable.TryGetValue(parent, out m_FadeFloat))
            {
                m_FadeFloat.SetMaterialInstance(parent.FadeMaterialList);
                m_FadeFloat.enabled = true;
                parent.isDistanceFadingIn = true;
                

                if (!parent.isDistanceFadingOut)
                {
                    m_FadeFloat.FadeOutFrom = 1.0f;
                    m_FadeFloat.FadeOutTo = 0.0f;
                    m_FadeFloat.FadeOutDuartion = FadeTime;
                    m_FadeFloat.Play();
                }
                else
                {
                    m_FadeFloat.FadeOutDuartion = m_FadeFloat.FadeTimer;
                    m_FadeFloat.FadeOutFrom = m_FadeFloat.currentFloat;
                    m_FadeFloat.FadeOutTo = 0;
                    m_FadeFloat.FadeTimer = 0;
                    m_FadeFloat.Update();
                    m_FadeFloat.ReStartFadeOutFinish_Constant();
                    
                }
                parent.isDistanceFadingOut = false;
            }


        }


    }


}
