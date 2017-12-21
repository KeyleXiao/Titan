/*******************************************************************
** 文件名:	EffectEditor.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/9/7
** 版  本:	1.0
** 描  述:	特效编辑器
** 应  用:   特效编辑器，别问我干什么的
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

using UnityEngine;
using UnityEditor;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using Effect;
using USpeedUI;

/// <summary>
/// 特效编辑器
/// </summary>
public partial class UIWindowDebugEditor : EditorWindow
{
    [MenuItem("PrivateTools/UIWindowDebugEditor")]
    public static void Init()
    {

        UIWindowDebugEditor uv = EditorWindow.GetWindow<UIWindowDebugEditor>(false, "UI窗口调试器", true);
        uv.Show();
        uv.autoRepaintOnSceneChange = true;
    }

    List<UIBaseWndView> m_UIBaseWndView = new List<UIBaseWndView>();
    Dictionary<UIBaseWndView, List<Canvas>> m_UIBaseWndView_Canvas = new Dictionary<UIBaseWndView, List<Canvas>>();
    Camera BloodCamera = null;
    Camera TopNameCamera = null;

    GameObject ButtonLeftPartObj = null;

    public void OnEnable()
    {
        GetUIWindowData();
    }

    private void ShowAllUI()
    {
        GetUIWindowData();
        foreach (UIBaseWndView view in m_UIBaseWndView)
        {
            ShowUI(view);

        }

        ShowBlood();
        ShowTopName();
        ShowButtonLeftPart();
        //if(UISystem.Instance)
        //{
        //    Camera cam = UISystem.Instance.GetCamera();
        //    if(cam)
        //    {
        //        cam.cullingMask = 1 << LayerMask.NameToLayer(Config.LayerUI); 
        //    }
        //}
    }
    private void HiddeInAllUI()
    {
        GetUIWindowData();

        foreach (UIBaseWndView view in m_UIBaseWndView)
        {
            HiddeInUI(view);
        }

        HiddeInBlood();
        HiddeInTopName();
        HiddeInButtonLeftPart();
        //if (UISystem.Instance)
        //{
        //    Camera cam = UISystem.Instance.GetCamera();
        //    if (cam)
        //    {
        //        cam.cullingMask = 0;
        //    }
        //}
    }

    private void GetUIWindowData()
    {
        m_UIBaseWndView.Clear();
        m_UIBaseWndView_Canvas.Clear();
        UIBaseWndView[] viewList = GameObject.FindObjectsOfType<UIBaseWndView>();
        m_UIBaseWndView.AddRange(viewList);

        foreach(UIBaseWndView vie in m_UIBaseWndView)
        {
            List<Canvas> canvanList = new List<Canvas>();
            canvanList.AddRange(vie.GetComponentsInChildren<Canvas>());
            m_UIBaseWndView_Canvas.Add(vie,canvanList);
        }

        GameObject goBloodCamera = GameObject.Find("BloodCamera");
        GameObject goTopNameCamera = GameObject.Find("TopNameCamera");
        if(!ButtonLeftPartObj)
        {
            ButtonLeftPartObj = GameObject.Find("ButtonLeftPart");
        }
        if(goBloodCamera)
        {
            BloodCamera = goBloodCamera.GetComponent<Camera>();
        }

        if (goTopNameCamera)
        {
            TopNameCamera = goTopNameCamera.GetComponent<Camera>();
        }
       
    }

    private void ShowUI(UIBaseWndView view)
    {
        if (!view)
        {
            return;
        }
        List<Canvas> canvasList = null;
        if (m_UIBaseWndView_Canvas.TryGetValue(view,out canvasList))
        {
            foreach(Canvas can in canvasList)
            {
                if(!can)
                {
                    continue;
                }
                can.enabled = true;
            }
        }
        GameUtil.SetLayer(LayerMask.NameToLayer(Config.LayerUI), view.gameObject, true);
    }
    private void HiddeInUI(UIBaseWndView view)
    {
        if(!view)
        {
            return;
        }
        List<Canvas> canvasList = null;
        if (m_UIBaseWndView_Canvas.TryGetValue(view, out canvasList))
        {
            foreach (Canvas can in canvasList)
            {
                if (!can)
                {
                    continue;
                }
                can.enabled = false;
            }
        }
        GameUtil.SetLayer(LayerMask.NameToLayer(Config.LayerIngoreRayCast), view.gameObject, true);
    }

    private void ShowBlood()
    {
        if (BloodCamera)
        {
             BloodCamera.cullingMask = 1 << LayerMask.NameToLayer(Config.LayerUI);
        }
    }

    private void HiddeInBlood()
    {
        if (BloodCamera)
        {
            BloodCamera.cullingMask = 0;
        }
    }

    private void ShowButtonLeftPart()
    {
        if (ButtonLeftPartObj)
        {
            ButtonLeftPartObj.SetActive(true);
            //GameUtil.SetLayer(LayerMask.NameToLayer(Config.LayerUI), ButtonLeftPartObj, true);
        }
    }

    private void HiddeInButtonLeftPart()
    {
        if (ButtonLeftPartObj)
        {
            ButtonLeftPartObj.SetActive(false);
            //GameUtil.SetLayer(LayerMask.NameToLayer(Config.LayerIngoreRayCast), ButtonLeftPartObj, true);
        }
    }

    private void ShowTopName()
    {
        if (TopNameCamera)
        {
            TopNameCamera.cullingMask = 1 << LayerMask.NameToLayer(Config.LayerUI);
        }
    }

    private void HiddeInTopName()
    {
        if (TopNameCamera)
        {
            TopNameCamera.cullingMask = 0;
        }
    }

    Vector2 scrollPos;
    public void OnGUI()
    {
        GUILayout.BeginHorizontal();

        if(GUILayout.Button("获取UI数据"))
        {
            GetUIWindowData(); 
        }
        if (GUILayout.Button("启用所有UI"))
        {
            ShowAllUI();
            
        }
        if (GUILayout.Button("关闭所有UI"))
        {
            HiddeInAllUI();
        }
        GUILayout.EndHorizontal();

        scrollPos = GUILayout.BeginScrollView(scrollPos);
        bool bShow = false;

        if (BloodCamera)
        {
            bShow = BloodCamera.cullingMask == 1 << LayerMask.NameToLayer(Config.LayerUI);
            bShow = GUILayout.Toggle(bShow, "显示血条");
            if(bShow)
            {
                ShowBlood();
            }
            else
            {
                HiddeInBlood();
            }
        }

        if (TopNameCamera)
        {
            bShow = TopNameCamera.cullingMask == 1 << LayerMask.NameToLayer(Config.LayerUI);
            bShow = GUILayout.Toggle(bShow, "头顶信息");
            if (bShow)
            {
                ShowTopName();
            }
            else
            {
                HiddeInTopName();
            }
        }

        if (ButtonLeftPartObj)
        {
           // bShow = LayerMask.NameToLayer(Config.LayerUI) == ButtonLeftPartObj.layer;
            bShow = GUILayout.Toggle(ButtonLeftPartObj.activeSelf, "ButtonLeftPart");
            if (bShow)
            {
                ShowButtonLeftPart();
            }
            else
            {
                HiddeInButtonLeftPart();
            }
        }

        foreach (UIBaseWndView view in m_UIBaseWndView)
        {
            if (!view)
            {
                continue;
            }
            bShow = LayerMask.NameToLayer(Config.LayerUI) == view.gameObject.layer;

            bShow = GUILayout.Toggle(bShow, "显示" + view.gameObject.name);
            if (bShow)
            {
                ShowUI(view);
            }
            else
            {
                HiddeInUI(view);
            }
        }

        GUILayout.EndScrollView();
    }

}
