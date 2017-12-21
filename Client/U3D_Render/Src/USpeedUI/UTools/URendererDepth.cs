/*******************************************************************
** 文件名:	URendererDepth.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2016/6/7
** 版  本:	1.0
** 描  述:	将非UI的对象提前到UI前面显示
            粒子Renderer里面有SortingLayer和OrderInLayer，粒子可以不用挂这个脚本了。
** 应  用:  

**************************** 修改记录 ******************************
** 修改人: 
** 日  期:
** 描  述: 
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI.UExtensions;

public class URendererDepth : MonoBehaviour
{
    [SerializeField]
    protected int m_orderInLayer;
    [SerializeField]
    protected string m_sortingLayerName = "Default";
    [SerializeField]
    protected bool m_bIsUIElememt = false;
    [SerializeField]
    protected bool m_bIsRequirRaycaster = false;

    /// <summary>
    /// 该脚本作用的是否是UI控件
    /// </summary>
    public bool IsUIElememt
    {
        get { return m_bIsUIElememt; }
        set
        {
            if(value != m_bIsUIElememt)
            {
                m_bIsUIElememt = value;
            }
        }
    }
    /// <summary>
    /// 该UI控件是否需要UI射线检测
    /// </summary>
    public bool IsRequirRaycaster
    {
        get { return m_bIsRequirRaycaster; }
        set
        {
            if(value != m_bIsRequirRaycaster)
            {
                m_bIsRequirRaycaster = value;
                GraphicRaycaster raycaster = this.gameObject.GetComponent<GraphicRaycaster>();
                if (m_bIsRequirRaycaster && raycaster == null)
                {
                    gameObject.AddComponent<GraphicRaycaster>(); 
                }
                else if(!m_bIsRequirRaycaster && raycaster != null)
                {
                    gameObject.RemoveComponent<GraphicRaycaster>();
                }                        
            }
        }
    }

    [NonSerialized]
    private Renderer[] m_Renderers;
    private bool m_bHadFindRenderers;


    public int OrderInLayer
    {
        get { return m_orderInLayer; }
        set
        {
            if(value != m_orderInLayer)
            {
                m_orderInLayer = value;
                if(!IsUIElememt)
                {
                    SetNonUIElememtOrder();
                }
                else
                {
                    SetUIElememtOrder();
                }

            }
        }
    }

    public string SortingLayerName
    {
        get { return m_sortingLayerName; }
        set
        {
            if (value != m_sortingLayerName)
            {
                m_sortingLayerName = value;
                if (!IsUIElememt)
                {
                    SetNonUIElementSortingLayer();
                }
                else
                {
                    SetUIElementSortingLayer();
                }

            }

        }
    }

    public void Awake()
    {
        GerRenderer();
    }

    private void SetNonUIElementSortingLayer()
    {
        if (GerRenderer() == null)
            return;

        foreach (Renderer renderer in GerRenderer())
        {
            renderer.sortingLayerName = SortingLayerName;
        }
    }

    private void SetUIElementSortingLayer()
    {
        Canvas canvas = GetComponent<Canvas>();
        if (canvas == null)
        {
            canvas = gameObject.AddComponent<Canvas>();
        }
        canvas.sortingLayerName = SortingLayerName;
    }

    private void SetNonUIElememtOrder()
    {
        if (GerRenderer() == null)
            return;

        foreach (Renderer renderer in GerRenderer())
        {
            renderer.sortingOrder = m_orderInLayer;
        }
    }

    private void SetUIElememtOrder()
    {
        Canvas canvas = GetComponent<Canvas>();
        if (canvas == null)
        {
            canvas = gameObject.AddComponent<Canvas>();
        }
        canvas.overrideSorting = true;
        canvas.sortingOrder = m_orderInLayer;
        Debug.Log("sortingLayerID:" + canvas.sortingLayerID + ",sortingLayerName:" + canvas.sortingLayerName);
        
    }

    private Renderer[] GerRenderer()
    {
        if(!m_bHadFindRenderers)
        {
            m_Renderers = this.GetComponentsInChildren<Renderer>();
            m_bHadFindRenderers = true;
        }
        return m_Renderers;
    }
}

