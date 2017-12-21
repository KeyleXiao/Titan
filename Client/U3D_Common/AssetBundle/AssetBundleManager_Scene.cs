/*******************************************************************
** 文件名:	AssetBundleManager_Scene.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	ab资源管理器
** 应  用:  	ab资源管理器,场景部分

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;
using System.IO;
//using LZ4ps;


public sealed partial class AssetBundleManager : MonoBehaviour
{

    enum SceneConfig
    {
        COL_SCENE_NAME,
        COL_GUID,
        COL_ASSETBUNDLENAME
    }

    /// <summary>
    /// 场景配置表，场景名-场景路径
    /// </summary>
    private static Dictionary<string, ResConfigData> SceneConfigTable = new Dictionary<string, ResConfigData>();

    /// <summary>
    /// 已经加载好的场景，场景名-场景对应的ab
    /// </summary>
    private static Dictionary<string, AssetBundle> LoadedScene_AbTable = new Dictionary<string, AssetBundle>();
    public static string SceneConfigPatch = "SceneConfg." + unityCSVExe;

    /// <summary>
    /// 通过additive加载的场景
    /// </summary>
    private static HashSet<string> LoadedAdditiveLevel = new HashSet<string>();

    /// <summary>
    /// 最后一个加载的场景
    /// </summary>
    private static string LastLoadedSceneName = string.Empty;

    private static bool isCallisCanLoadLevel = false;

    private static string debugScenePath = "scene/debugscene";

    /// <summary>
    /// 
    /// </summary>
    /// <param name="sceneName">区分大小写</param>
    /// <returns></returns>
    public static bool isCanLoadLevel(string sceneName)
    {
        if(LastLoadedSceneName.Equals(sceneName))
        {
            return false;
        }
        isCallisCanLoadLevel = true;
        bool re = SceneConfigTable.ContainsKey(sceneName);
        if(!re)
        {
            Debug.LogError("找不到场景:" + sceneName);
        }
        
        return re;
    }

    public static void ConfirmLoadedScene(string sceneName)
    {
        LastLoadedSceneName = sceneName;
    }

    /// <summary>
    /// 再调用之前必须调用isCanLoadLvel判断是否可以加载场景
    /// </summary>
    /// <param name="sceneName">场景名，区分大小写</param>
    /// <returns></returns>
    public static void LoadLevel(string sceneName)
    {
        /*
         * 用回旧的流程，走runtime，因为不走runtime会读取到材质球，导致每次都需要svn更新
         */

        if (!isCallisCanLoadLevel)
        {
            Debug.LogError("必须先调用isCanLoadLevel");
            return ;
        }

        //场景已经被加载
        if (LoadedScene_AbTable.ContainsKey(sceneName))
        {
            return;
        }
        LoadSceneAssetBundle(sceneName);
        isCallisCanLoadLevel = false;

        if (isOnRunTimeMode)
        {
            UnityEngine.SceneManagement.SceneManager.LoadScene(sceneName);
        }
        else
        {
            ResConfigData scenePath = GetScenePath(sceneName);
            if (null != scenePath)
            {
                LoadSceneOnEditorInstance(scenePath.AssetGUID, false, false);
            }
        }

        
    }


    /// <summary>
    /// 再调用之前必须调用isCanLoadLvel判断是否可以加载场景
    /// </summary>
    /// <param name="sceneName">场景名，区分大小写</param>
    /// <returns></returns>
    public static AsyncOperation LoadLevelAsync(string sceneName)
    {
        /*
         * 用回旧的流程，走runtime，因为不走runtime会读取到材质球，导致每次都需要svn更新
         */

        if (!isCallisCanLoadLevel)
        {
            Debug.LogError("必须先调用isCanLoadLevel");
            return null;
        }
        if (Debug.isDebugBuild)
        {
            Debug.Log("加载场景:" + sceneName);
        }
       
        //场景已经被加载
        if (LoadedScene_AbTable.ContainsKey(sceneName))
        {
            return null;
        }
        LoadSceneAssetBundle(sceneName);
        isCallisCanLoadLevel = false;

        AsyncOperation reslut = null;
        if(isOnRunTimeMode)
        {
            reslut = UnityEngine.SceneManagement.SceneManager.LoadSceneAsync(sceneName);
        }
        else
        {
            ResConfigData scenePath = GetScenePath(sceneName);
            if (null != scenePath)
            {
                reslut = LoadSceneOnEditorInstance(scenePath.AssetGUID, true, false);
            }
        }
        return reslut;
    }


   /// <summary>
   /// 叠加加载场景,
   /// </summary>
    /// <param name="sceneName">场景名，区分大小写</param>
    public static void LoadLevelAdditive(string sceneName)
    {
        /*
         * 用回旧的流程，走runtime，因为不走runtime会读取到材质球，导致每次都需要svn更新
         */

        if (!isCallisCanLoadLevel)
        {
            Debug.LogError("必须先调用CallisCanLoadLevel");
            return;
        }
        if (LoadedScene_AbTable.ContainsKey(sceneName))
        {
            return;
        }


        LoadSceneAssetBundle(sceneName);
        isCallisCanLoadLevel = false;
        if (!LoadedAdditiveLevel.Contains(sceneName))
        {
            LoadedAdditiveLevel.Add(sceneName);
        }
        if(isOnRunTimeMode)
        {
            UnityEngine.SceneManagement.SceneManager.LoadScene(sceneName, UnityEngine.SceneManagement.LoadSceneMode.Additive);
        }
        else
        {
            ResConfigData scenePath = GetScenePath(sceneName);
            if(null != scenePath)
            {
                LoadSceneOnEditorInstance(scenePath.AssetGUID, false, true);
            }
        }
    }


    public static bool isBundleScene(string sceneName)
    {
        return LoadedScene_AbTable.ContainsKey(sceneName);
    }

    public static void UnLoadSceneBundle(string sceneName)
    {
        AssetBundle pAssetBundles = null;
        if (LoadedScene_AbTable.TryGetValue(sceneName, out pAssetBundles))
        {
            if (LoadedAdditiveLevel.Contains(sceneName))
            {
                UnityEngine.SceneManagement.SceneManager.UnloadScene(sceneName);
                LoadedAdditiveLevel.Remove(sceneName);
            }

            if(isOnRunTimeMode)
            {
                pAssetBundles.Unload(true);
            }
            LoadedScene_AbTable.Remove(sceneName);
        }

        if(LastLoadedSceneName.Equals(sceneName))
        {
            LastLoadedSceneName = string.Empty;
        }
    }

    #region 呵呵哒

    public static void UnLoadAllSceneBundle()
    {

        if(isOnRunTimeMode)
        {
            foreach (AssetBundle pa in LoadedScene_AbTable.Values)
            {
                pa.Unload(true);
            }
        }

        LoadedScene_AbTable.Clear();
        LastLoadedSceneName = string.Empty;
    }

    private static void LoadSceneConfig()
    {
        string patch = FullAssetPackagePathRoot + SceneConfigPatch;
        string stSchemeAllText = DeCodeAssetFile(patch);

        if(stSchemeAllText.Equals(string.Empty))
        {
            Debug.LogError("加载场景配置失败！");
            return;
        }

        //去掉\n,因为ScpReader以"\r"分割
        stSchemeAllText = stSchemeAllText.Replace("\n", "");
        //再干掉最后的"\r"
        stSchemeAllText = stSchemeAllText.Remove(stSchemeAllText.Length - 1, 1);
        AssetBundleConfigReader SceneConfigReader = new AssetBundleConfigReader(stSchemeAllText, "SceneConfig", 2);

        // 遍歷整個表并存儲起來
        for (int i = 0; i < SceneConfigReader.GetRecordCount(); ++i)
        {
            ResConfigData cof = new ResConfigData();
            cof.AssetName = SceneConfigReader.GetString(i, (int)SceneConfig.COL_SCENE_NAME, "");
            cof.AssetGUID = SceneConfigReader.GetString(i, (int)SceneConfig.COL_GUID, "");
            cof.AssetBundleName = SceneConfigReader.GetString(i, (int)SceneConfig.COL_ASSETBUNDLENAME, "");
            SceneConfigTable.Add(cof.AssetName, cof);
        }

        SceneConfigReader.Dispose();
        SceneConfigReader = null;
    }

    private static void InitScene()
    {
        LoadSceneConfig();
    }

    private static void ClearSceneConfig()
    {
        SceneConfigTable.Clear();
    }

    private static void ClearScene()
    {
        ClearSceneConfig();
        UnLoadAllSceneBundle();
    }

    /// <summary>
    /// 加载场景ab，如果这个函数返回true，则可以加载场景
    /// </summary>
    /// <param name="sceneName"></param>
    /// <returns></returns>
    private static bool LoadSceneAssetBundle(string sceneName)
    {

        ResConfigData scenePath = GetScenePath(sceneName);
        if(null == scenePath)
        {
            return false;
        }
        AssetBundle pAssetBundles = null;

        

        //if(!Application.isEditor)
        //{
        //    if (scenePath.StartsWith(debugScenePath))
        //    {
        //        if (Debug.isDebugBuild)
        //        {
        //            Debug.LogWarning(StringHelper.BuildString("当前加载的场景为调试场景，调试场景[-不会-]在Release版本存在，请注意!,场景名:", sceneName));
        //        }
        //        else
        //        {
        //            Debug.LogError(StringHelper.BuildString("当前加载的场景为调试场景，调试场景不会在Release版本中加载,场景名:", sceneName));
        //            return false;
        //        }
        //    }

        //}

        if(isOnRunTimeMode)
        {
            string patch = FullAssetPackagePathRoot + scenePath.AssetBundleName;
            if (!m_RespackageTable.ContainsKey(scenePath.AssetBundleName))
            {
                AssetBudleLog("AssetBundle文件不存在!,路径:" + patch);
                return pAssetBundles;
            }

            pAssetBundles = LoadAssetBundle(patch, null);
        }
        LoadedScene_AbTable.Add(sceneName, pAssetBundles);
        
        return true;
    }

    private static ResConfigData GetScenePath(string sceneName)
    {
        ResConfigData data;
        SceneConfigTable.TryGetValue(sceneName, out data);

        if (data == null)
        {
            AssetBudleLog("加载场景错误,找不到场景:" + sceneName);
        }
        return data;
    }
    #endregion
}
