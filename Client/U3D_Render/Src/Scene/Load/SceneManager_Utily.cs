using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using UnityEngine.Rendering;

public partial class SceneManager : MonoBehaviour
{
    private static string MainCitySceneName = "Cest01";



    /// <summary>
    /// 是否是主城场景
    /// </summary>
    /// <returns></returns>
    public bool isMainCity()
    {
        //主城ID是 1
        return m_curMapID == 1;
        //return m_SceneName.Contains(MainCitySceneName);
    }

    public static void ActiveScene(string sceneName)
    {
        if(sceneName.Equals(string.Empty))
        {
            return;
        }
        SceneResInfoManager manager = SceneResInfoManager.API_GetSceneResManager(sceneName); ;
        if(!manager)
        {
            return;
        }

        if(manager)
        {
            manager.Active();
        }
    }

    public static void DeActiveScene(string sceneName)
    {
        if (sceneName.Equals(string.Empty))
        {
            return;
        }
        SceneResInfoManager manager = SceneResInfoManager.API_GetSceneResManager(sceneName);
        if(!manager)
        {
            return;
        }

        if (manager)
        {
            manager.DeActive();
        }
    }

    public static void UnLoadLevel(string sceneName)
    {
        SceneResInfoManager.API_UnLoadLevel(sceneName);
        AssetBundleManager.UnLoadSceneBundle(sceneName);
    }

    public static void UnLoadAllLevel()
    {
        AssetBundleManager.UnLoadAllSceneBundle();
        SceneResInfoManager.API_UnLoadAllLevel();
    }
}