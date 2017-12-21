/*******************************************************************
** 文件名:	UIWindowDebugPlane.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	谭强均
** 日  期:	2016-3-24
** 版  本:	1.0
** 描  述:	用于调试窗口
		
********************************************************************/

using System;
using UnityEngine;
using USpeedUI;
using System.Collections.Generic;
using UnityEngine.UI;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using UIWidgets;
public sealed partial class UIWindowDebugPlane : MonoBehaviour
{

    private static ObservableList<UListItemUIWindowDebug> m_UIWindowStatusList = new ObservableList<UListItemUIWindowDebug>();
    private static UIWindowDebugPlane m_Instance = null;

    private static ResNode DebugPlaneRes = null;
    static string PrefabGUID = "06975e23d20ac2f4b93b0acb72e40305";
    public static void CloseUIWindowDebugPlane()
    {
        if(m_Instance)
        {
            m_Instance.gameObject.SetActive(false);
        }
    }

    public static void OpenUIWindowDebugPlane()
    {
        if(null == DebugPlaneRes)
        {
            DebugPlaneRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, PrefabGUID, true);
            GameObject go = null;
            if (!ResNode.isNullOrEmpty(DebugPlaneRes))
            {
                go = DebugPlaneRes.InstanceMainRes();
                if (go)
                {
                    go.transform.SetParent(UISystem.Instance.transform);
                    go.transform.localPosition = Vector3.zero;
                    go.transform.localScale = Vector3.one;
                }
            }
        }

        if(m_Instance)
        {
            m_Instance.gameObject.SetActive(true);
            m_Instance.transform.SetParent(UISystem.Instance.transform);
            m_Instance.transform.localPosition = Vector3.zero;
            m_Instance.transform.localScale = Vector3.one;
        }
    }

    public UIWindowDebugNode listview = null;

    public Button RefershButton = null;
    public Button CloseButton = null;
    public Button DisableAllButton = null;
    public Button EnableAllButton = null;
    public void Awake()
    {
        if (!m_Instance)
        {
            m_Instance = this;
        }
        else
        {
            Debug.LogError("只能有一个UIWindowDebugPlane:" + m_Instance.name);
        }


    }

    private void OnRefershClick()
    {
        RefershListView();
    }

    private void OnCloseClick()
    {
        CloseUIWindowDebugPlane();
    }
    private void OnDisableAllButtonClick()
    {
        m_UIWindowStatusList.Clear();

        UIBaseWndView[] viewList = GameObject.FindObjectsOfType<UIBaseWndView>();

        foreach (UIBaseWndView view in viewList)
        {
            UListItemUIWindowDebug data = new UListItemUIWindowDebug();
            data.uiWindow = view;
            GameUtil.SetLayer(LayerMask.NameToLayer(Config.LayerIngoreRayCast), view.gameObject, true);
            m_UIWindowStatusList.Add(data);
        }

        listview.DataSource.BeginUpdate();
        listview.DataSource = m_UIWindowStatusList;
        listview.DataSource.EndUpdate();
    }
    private void OnEnableAllButtonClick()
    {
        m_UIWindowStatusList.Clear();

        UIBaseWndView[] viewList = GameObject.FindObjectsOfType<UIBaseWndView>();

        foreach (UIBaseWndView view in viewList)
        {
            UListItemUIWindowDebug data = new UListItemUIWindowDebug();
            data.uiWindow = view;
            GameUtil.SetLayer(LayerMask.NameToLayer(Config.LayerUI), view.gameObject, true);
            m_UIWindowStatusList.Add(data);
        }

        listview.DataSource.BeginUpdate();
        listview.DataSource = m_UIWindowStatusList;
        listview.DataSource.EndUpdate();
    }
    private void RefershListView()
    {
        m_UIWindowStatusList.Clear();

        UIBaseWndView[] viewList = GameObject.FindObjectsOfType<UIBaseWndView>();

        foreach(UIBaseWndView view in viewList)
        {
            UListItemUIWindowDebug data = new UListItemUIWindowDebug();
            data.uiWindow = view;
            m_UIWindowStatusList.Add(data);
        }
      
        listview.DataSource.BeginUpdate();
        listview.DataSource = m_UIWindowStatusList;
        listview.DataSource.EndUpdate();
    }

    public void Start()
    {
        if(RefershButton)
        {
            RefershButton.onClick.AddListener(OnRefershClick);
        }

        if(CloseButton)
        {
            CloseButton.onClick.AddListener(OnCloseClick);
        }

        if (DisableAllButton)
        {
            DisableAllButton.onClick.AddListener(OnDisableAllButtonClick);
        }


        if (EnableAllButton)
        {
            EnableAllButton.onClick.AddListener(OnEnableAllButtonClick);
        }
        
    }


    public void OnDestroy()
    {
        m_UIWindowStatusList.Clear();
        m_Instance = null;
    }
}

