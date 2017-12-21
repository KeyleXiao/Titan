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
using UnityEngine.Events;
public class ActiveSceneChangedEvent : UnityEvent<SceneResInfoManager>
{
}

public partial class SceneResInfoManager : MonoBehaviour
{
    public static Camera currentSceneCamera
    {
        get
        {
            return m_currentSceneCamera;
        }
    }
    private static Camera m_currentSceneCamera = null;

    private static ActiveSceneChangedEvent m_ActiveSceneChangedCallBack = new ActiveSceneChangedEvent();

    /// <summary>
    /// 加载的场景,[场景名,场景管理器]
    /// </summary>
    private static Dictionary<string, SceneResInfoManager> LoadedScene_ResTable = new Dictionary<string, SceneResInfoManager>();

    #region ActiveSceneChanged
    public static void RegisterActiveSceneChangedEvent(UnityAction<SceneResInfoManager> ev)
    {
        if (null != m_ActiveSceneChangedCallBack)
        {
            if (null != ev)
            {
                m_ActiveSceneChangedCallBack.AddListener(ev);
            }
        }
    }

    public static void UnRegisterActiveSceneChangedEvent(UnityAction<SceneResInfoManager> ev)
    {
        if (null != m_ActiveSceneChangedCallBack)
        {
            if (null != ev)
            {
                m_ActiveSceneChangedCallBack.RemoveListener(ev);
            }
        }
    }

    public static void InvokeActiveSceneChangedEvent(SceneResInfoManager prog)
    {

        if (null != m_ActiveSceneChangedCallBack)
        {
            m_ActiveSceneChangedCallBack.Invoke(prog);
        }
    }

    #endregion

    #region ImageSetting
    public static ImageSettingData API_GetImageSettingData()
    {
        if (null != GetImageSettingDataInstance)
        {
            return GetImageSettingDataInstance();
        }
        Debug.LogError("GetImageSettingDataInstance is null");
        return null;
    }

    public static void RegsiterGetImageSettignData(GetImageDataDelegate func)
    {
        GetImageSettingDataInstance = func;
    }

    public delegate ImageSettingData GetImageDataDelegate();
    private static GetImageDataDelegate GetImageSettingDataInstance = null;
    #endregion


    public static void API_OnResolutionChanged(int w, int h)
    {
        foreach(SceneResInfoManager ins in SceneResInfoManager.AllInstance)
        {
            ins.OnResolutionChanged();
        }
    }

    public static SceneResInfoManager API_GetSceneResManager(string sceneName)
    {
        SceneResInfoManager result = null;
        if (!LoadedScene_ResTable.TryGetValue(sceneName, out result))
        {
            Debug.LogWarning("找不到场景管理器:" + sceneName);
        }
        return result;

    }

    private static void API_ConfirmLoadedSceneRes(SceneResInfoManager Res)
    {
        if (!AssetBundleManager.isBundleScene(Res.sceneName))
        {
            Debug.LogError("发现一类场景资源超级BUG!!!!!");
            return;
        }

        if (LoadedScene_ResTable.ContainsKey(Res.sceneName))
        {
            Debug.LogError("发现二类场景资源超级BUG!!!!!");
            return;
        }
        LoadedScene_ResTable.Add(Res.sceneName, Res);
        AssetBundleManager.ConfirmLoadedScene(Res.sceneName);
    }

    public static void API_UnLoadLevel(string sceneName)
    {
        SceneResInfoManager mr = null;
        if (LoadedScene_ResTable.TryGetValue(sceneName, out mr))
        {
            ResNode.DestroyRes(ref mr.SceneRoot);
            LoadedScene_ResTable.Remove(sceneName);
        }

    }

    public static void API_UnLoadAllLevel()
    {
        foreach (SceneResInfoManager re in LoadedScene_ResTable.Values)
        {
            ResNode.DestroyRes(ref re.SceneRoot);
        }

        LoadedScene_ResTable.Clear();
    }
}
