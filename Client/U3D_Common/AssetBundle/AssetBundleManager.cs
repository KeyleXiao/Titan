/*******************************************************************
** 文件名:	AssetBundleManager.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	ab资源管理器
** 应  用:  	ab资源管理器

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

public sealed partial class AssetBundleManager : MonoBehaviour
{
    //由于用了DLL形式，所以不能访问UnityEditor下的东西，只能用这种蛋疼的方式，在Editor下加载资源
    public delegate UnityEngine.Object[] LoadAllAssetOnEditorFromPath(string path,Type assetType);
    public static LoadAllAssetOnEditorFromPath LoadAllAssetOnEditorFromPathInstance = null;

    //由于用了DLL形式，所以不能访问UnityEditor下的东西，只能用这种蛋疼的方式，在Editor下加载资源
    public delegate UnityEngine.Object[] LoadAssetOnEditor(ResConfigData config,System.Type t);
    public static LoadAssetOnEditor LoadAssetOnEditorInstance = null;

    //由于用了DLL形式，所以不能访问UnityEditor下的东西，只能用这种蛋疼的方式，在Editor下加载资源
    public delegate AsyncOperation LoadSceneOnEditor(string scenePath, bool bAsync,bool bAdditive);
    public static LoadSceneOnEditor LoadSceneOnEditorInstance = null;

    //由于用了DLL形式，所以不能访问UnityEditor下的东西，只能用这种蛋疼的方式，在Editor下加载资源
    public delegate UnityEngine.Object[] LoadAssetOnEditorFromAb(string assetBundleName, string assetName,System.Type t);
    public static LoadAssetOnEditorFromAb LoadAssetOnEditorFromAbInstance = null;

    //由于用了DLL形式，所以不能访问UnityEditor下的东西，只能用这种蛋疼的方式，在Editor下加载资源
    public delegate bool CheckAssetPatchOnEditor(string assetBundleName, string assetName);
    public static CheckAssetPatchOnEditor CheckAssetPatchOnEditorInstance = null;
    public static bool isVaild
    {
        get
        {
            return m_bEnabled;
        }
    }

    public static bool isLoadingScene = false;

    private static AssetBundleManifest pManifest = null;

    private static bool m_bEnabled = false;

    /// <summary>
    /// 强制走AB
    /// </summary>
    private static bool OpenRunTime = false;

    public static GameObject HostGameObject = null;
    private static AssetBundleManager MonoInstance = null;

    /// <summary>
    /// 资源缓存时间
    /// </summary>
    private const float AssetCacheTime = 15.0f;

    /// <summary>
    /// 预制体缓存时间
    /// </summary>
    private const float prefabCacheTime = AssetCacheTime - 2.0f;

    /// <summary>
    /// 是否在运行时模式，运行时模式需要走ab的
    /// </summary>
    public static bool isOnRunTimeMode
    {
        get
        {
            return (!Application.isEditor || OpenRunTime) && Application.isPlaying;
        }
    }

    private static Type GameObjectType = typeof(GameObject);

    private static bool enabledUnionDependenices = true;


    public static void Init()
    {
        if (isVaild)
        {
            return;
        }
        //文件只能上一个，一样会阻塞加载线程
        MinUpLoadAssetsFileAsyncRequestCount = 1;
        MaxUpLoad_Assets_AsyncRequestClip = Mathf.Min(4, SystemInfo.processorCount);
        enabledUnionDependenices = enabledUnionDependenices && !File.Exists("disabledUnionDependenices.txt");
        MaxUpLoad_Assets_AsyncRequestClip = File.Exists("uploadrequestsingle.txt") ? 0.0001f : MaxUpLoad_Assets_AsyncRequestClip;
        OpenRunTime = OpenRunTime || File.Exists("openruntime.txt");
        enabledAsyncLoadAssetFile = enabledAsyncLoadAssetFile && !File.Exists("disableasyncloadassetfile.txt");
        m_bEnabled = CheckDiskFreeSpace();
        if (!m_bEnabled)
        {
            Clear();
            return;
        }
        ClearResNodeTempData();
        ClearAssetFileTempData();

        LoadAllAssetBundleConfigData();
        LoadShaders();

        InitAssetBundleThread();
        InitScene();
        LoadPrefabConfig();       
        InitUIRes();

        LoadFontRes();
        LoadCommonAssets();

        if (!HostGameObject)
        {
            HostGameObject = new GameObject();
            HostGameObject.name = "AssetBundleManager";
            HostGameObject.transform.position = Vector3.zero;
            HostGameObject.transform.rotation = Quaternion.identity;
            HostGameObject.transform.localScale = Vector3.one;
            MonoInstance = HostGameObject.AddComponent<AssetBundleManager>();
            UnityEngine.Object.DontDestroyOnLoad(HostGameObject);
        }
        ResNode res = null;
        CommonDissolveTextureConfigData = new ResConfigData();
        CommonDissolveTextureConfigData.AssetBundleName = "texture/commonartist/unitydefaultresources/texture.texture";
        CommonDissolveTextureConfigData.AssetGUID = "d5fb8ec7c6ae5334eabcbc04e73f44ee";
        CommonDissolveTextureConfigData.AssetName = "CommonDissolveMap";
        CommonDissolveTextureConfigData.AssetSize_X = 256;
        CommonDissolveTextureConfigData.AssetSize_Y = 256;
        CommonDissolveTextureConfigData.fromReference = false;
        res = GetTextureResource(CommonDissolveTextureConfigData);
        if (!ResNode.isNullOrEmpty(res))
        {
            CommonDissloveTexture = res.ConvertRes<Texture>();
        }

        CommonBlack_AlphaZeroTextureConfigData = new ResConfigData();
        CommonBlack_AlphaZeroTextureConfigData.AssetBundleName = "texture/commonartist/unitydefaultresources/texture.texture";
        CommonBlack_AlphaZeroTextureConfigData.AssetGUID = "2a8ce53528d8ac348af0de443cafb77c";
        CommonBlack_AlphaZeroTextureConfigData.AssetName = "BlackTexture_ZeroAlpha";
        CommonBlack_AlphaZeroTextureConfigData.AssetSize_X = 256;
        CommonBlack_AlphaZeroTextureConfigData.AssetSize_Y = 256;
        CommonBlack_AlphaZeroTextureConfigData.fromReference = false;
        res = GetTextureResource(CommonBlack_AlphaZeroTextureConfigData);
        if(!ResNode.isNullOrEmpty(res))
        {
            CommonBlack_AlphaZeroTexture = res.ConvertRes<Texture>();
        }

        if (!CommonDissloveTexture)
        {
            Debug.LogWarning("找不到CommonDisslove溶解贴图，使用白色贴图代替");
            CommonDissloveTexture = Texture2D.whiteTexture;
        }
        if (!CommonBlack_AlphaZeroTexture)
        {
            Debug.LogWarning("找不到CommonBlack_AlphaZeroTexture溶解贴图，使用黑色色贴图代替");
            CommonBlack_AlphaZeroTexture = Texture2D.blackTexture;
        }
    }


    public static void Clear()
    {
        ClearAllPackageConfig();
        UnLoadShaders();
        ClearAssetBundleThread();
        ClearScene();
        ClearPrefabConfig();
        UnLoadAssetBundleManifest();
        UnLoadAllAssetBundle();
        UnLoadCommonAssets();
        ClearFontRes();
        //非编辑器模式下，才走这个
        if (isOnRunTimeMode && HostGameObject)
        {
            MonoInstance.StopAllCoroutines();
            UnityEngine.Object.Destroy(HostGameObject);
        }

        if (CommonDissloveTexture != Texture2D.whiteTexture)
        {
            Resources.UnloadAsset(CommonDissloveTexture);
        }
        usedUpLoad_Assets_AsyncRequestClip = 0.0f;
        Resources.UnloadUnusedAssets();
        //GC.Collect();

        
        m_bEnabled = false;
    }

    private static void UnLoadAssetBundleManifest()
    {
        if (pManifest)
        {
            pManifest = null;
        }
    }

    private static bool LoadAssetBundleManifest()
    {
        //编辑器下不走AssetBundle，直接return,只要开了一个runtime，就要走ab
        if(!isOnRunTimeMode)
        {
            return true;
        }

        AssetBundle pAssetBundles = LoadAssetBundle(AssetBundleManager.FullAssetPackagePathRoot + AssetBundlePathName, null);

        if (!pAssetBundles)
        {
            return false;
        }

        pManifest = pAssetBundles.LoadAsset("AssetBundleManifest") as AssetBundleManifest;

        if (!pManifest)
        {
            return false;
        }
        
        pAssetBundles.Unload(false);
        pAssetBundles = null;

        return true;
    }

    private static void ClearResTable(ref Dictionary<int,ResNode> table)
    {
        if (null == table || table.Count <= 0 || !isOnRunTimeMode)
        {
            return;
        }

       List<int> Temp = new List<int>();
       Temp.AddRange(table.Keys);
       int cout = Temp.Count;
       for (int i = 0; i < cout;i++ )
       {
           ResNode res;
           if (table.TryGetValue(Temp[i], out res))
           {
                UnLoadAssets(res);
                res.Clear();
           }
        }

       table.Clear();
    }

    private static void AssetBudleLog(string msg,bool errorMsg = false)
    {
        if(!errorMsg)
        {
            Debug.LogWarning("AssetBundleManager--" + msg);
        }
        else
        {
            Debug.LogError("AssetBundleManager--" + msg);
        }

    }
}
