/*******************************************************************
** 文件名:	AssetBundleManager_CommonAsset.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	ab资源管理器
** 应  用:  	ab资源管理器，通用资源部分

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System;
using System.Runtime.InteropServices;

public sealed partial class AssetBundleManager : MonoBehaviour
{
    /*
     * 通用资源是常驻内存的，不会卸载，所以通用资源
     * 能少就少
     * 
     */

    /// <summary>
    /// 预制体通用路径
    /// </summary>
    public static string PrefabCommonPatch = "CommonPrefab";

    /// <summary>
    /// 美术资源通用路径
    /// </summary>
    public static string ArtistCommonPatch = "CommonArtist";

    /// <summary>
    /// 通用资源表，包名-[资源名,资源]
    /// </summary>
    static Dictionary<string, Dictionary<string,UnityEngine.Object>> m_CommonAssetsTable = new Dictionary<string,Dictionary<string,UnityEngine.Object>>();

    /// <summary>
    /// 预制体依赖资源，单独保存起来，因为要卸载
    /// </summary>
    static List<PrefabDependenciesResNode> m_PrefabDependenciesAssetList = new List<PrefabDependenciesResNode>();

    /// <summary>
    /// 通用资源表，资源GUID-资源名
    /// </summary>
    static Dictionary<string, ResNode> m_CommonResNodeTable = new Dictionary<string, ResNode>();

    /// <summary>
    /// 通用资源表，包名-[资源名,资源]
    /// </summary>
    static Dictionary<string, AssetBundle> m_CommonAssetsBundleInstanceTable = new Dictionary<string, AssetBundle>();

    /// <summary>
    /// 美术通用资源包
    /// </summary>
    static List<string> ArtistCommonAssetBundleNames = new List<string>();

    /// <summary>
    /// 预制体通用资源包
    /// </summary>
    static List<string> PrefabCommonAssetBundleNames = new List<string>();

    private static void LoadCommonAssets()
    {
        if(!isOnRunTimeMode)
        {
            return;
        }
        
        List<string> textureList = new List<string>();
        List<string> matList = new List<string>();
        List<string> meshList = new List<string>();
        List<string> fbxList = new List<string>();
        List<string> aniclipList = new List<string>();
        List<string> anicontrollerList = new List<string>();
       
        List<string> sortedList = new List<string>();


        //先加载美术的资源
        foreach (string abPatch in ArtistCommonAssetBundleNames)
        {
            if(abPatch.EndsWith(textureVariant))
            {
                textureList.Add(abPatch);
            }
            else if (abPatch.EndsWith(MatVariant))
            {
                matList.Add(abPatch);
            }
            else if (abPatch.EndsWith(FBXVariant))
            {
                fbxList.Add(abPatch);
            }
            else if (abPatch.EndsWith(AnimatorClipVariant))
            {
                aniclipList.Add(abPatch);
            }
            else if (abPatch.EndsWith(AnimatorControllerVariant))
            {
                anicontrollerList.Add(abPatch);
            }
            else if (abPatch.EndsWith(MeshAssetVariant))
            {
                meshList.Add(abPatch);
            }
        }

        sortedList.AddRange(textureList);
        sortedList.AddRange(matList);
        sortedList.AddRange(fbxList);
        sortedList.AddRange(aniclipList);
        sortedList.AddRange(anicontrollerList);
        sortedList.AddRange(meshList);
        foreach (string abPatch in sortedList)
        {
            GetArtistAssets(abPatch);
        }
        sortedList.Clear();
        //再加载美术的资源
        foreach (string abPatch in PrefabCommonAssetBundleNames)
        {
            GetPrefabAssets(abPatch);
        }
    }


    /// <summary>
    /// 加载美术资源
    /// </summary>
    /// <param name="pAssetBundle"></param>
    /// <returns></returns>
    private static void GetArtistAssets(string abPatch)
    {
        if(!m_RespackageTable.ContainsKey(abPatch))
        {
            AssetBudleLog("加载通用资源失败，找不到资源包:" + abPatch);
            return;
        }
        AssetBundle pAssetBundle = LoadAssetBundle(AssetBundleManager.FullAssetPackagePathRoot + abPatch, null);
        if (!pAssetBundle)
        {
            AssetBudleLog("加载通用资源失败，找不到资源包:" + abPatch);
            return;
        }
        
        List<UnityEngine.Object> allAssets = new List<UnityEngine.Object>();
        UnityEngine.Object[] assets = null;

        /*根据后缀来决定要加载什么*/

        if (abPatch.EndsWith(AssetBundleManager.textureVariant))
        {
            assets = pAssetBundle.LoadAllAssets<Texture>();
            allAssets.AddRange(assets);
        }
        else if (abPatch.EndsWith(AssetBundleManager.UIVariant))
        {
            assets = pAssetBundle.LoadAllAssets<Sprite>();
            allAssets.AddRange(assets);
        }
        else if (abPatch.EndsWith(AssetBundleManager.AnimatorClipVariant))
        {
            assets = pAssetBundle.LoadAllAssets<AnimationClip>();
            allAssets.AddRange(assets);
        }
        else if (abPatch.EndsWith(AssetBundleManager.soundVariant))
        {
            assets = pAssetBundle.LoadAllAssets<AudioClip>();
            allAssets.AddRange(assets);
        }
        else if (abPatch.EndsWith(AssetBundleManager.MatVariant))
        {
            assets = pAssetBundle.LoadAllAssets<Material>();
            allAssets.AddRange(assets);
        }
        else if (abPatch.EndsWith(AssetBundleManager.UIVariant))
        {
            assets = pAssetBundle.LoadAllAssets<Sprite>();
            allAssets.AddRange(assets);
        }
        else if (abPatch.EndsWith(AssetBundleManager.FBXVariant))
        {
            assets = pAssetBundle.LoadAllAssets<GameObject>();
            allAssets.AddRange(assets);
        }
        else if (abPatch.EndsWith(AssetBundleManager.AnimatorControllerVariant))
        {
            assets = pAssetBundle.LoadAllAssets<RuntimeAnimatorController>();
            allAssets.AddRange(assets);
        }
        else if (abPatch.EndsWith(AssetBundleManager.MeshAssetVariant))
        {
            assets = pAssetBundle.LoadAllAssets<Mesh>();
            allAssets.AddRange(assets);
        }
        if(allAssets.Count > 0)
        {
            Dictionary<string, UnityEngine.Object> assetTable = new Dictionary<string, UnityEngine.Object>();
            m_CommonAssetsTable.Add(abPatch, assetTable);
            foreach (UnityEngine.Object asset in allAssets)
            {
                assetTable.Add(asset.name, asset);
            }
            m_CommonAssetsBundleInstanceTable.Add(abPatch, pAssetBundle);
        }


    
    }

    /// <summary>
    /// 加载预制体
    /// </summary>
    /// <param name="pAssetBundle"></param>
    /// <returns></returns>
    private static List<UnityEngine.Object> GetPrefabAssets(string PrefabPatch)
    {


        /*
         * 这个预制体加载与AssetBundleManager_Prefab里的预制体加载不一样,
         * 因为预制体无法释放资源的缘故，只能采取摧毁资源的方式,所以那里的预制体不会保存AssetBundle
         * 而这里的预制体因为是常驻内存的缘故，所以需要保存AssetBundle,要不然其他引用就找不到了
         */
        List<UnityEngine.Object> allAssets = new List<UnityEngine.Object>();
        Dictionary<string, UnionResConfigData> PrefabAssetTable = null;


        if (m_PrefabConfigTable.TryGetValue(PrefabPatch, out PrefabAssetTable))
        {
            Dictionary<string, UnityEngine.Object> assetTable = new Dictionary<string, UnityEngine.Object>();
            AssetBundle pAssetBudle = null;
            foreach (UnionResConfigData pefabConfig in PrefabAssetTable.Values)
            {
                if (!m_RespackageTable.ContainsKey(pefabConfig.ShortUnionFilePath))
                {
                    AssetBudleLog("加载通用资源失败，找不到资源包:" + PrefabPatch);
                    continue;
                }

                if(pAssetBudle == null)
                {
                    pAssetBudle = LoadAssetBundle(AssetBundleManager.FullAssetPackagePathRoot + PrefabPatch, pefabConfig);
                }
                PrefabDependenciesResNode dpNode ;
                if(LoadDependenicesResource(pefabConfig,out dpNode))
                {
                    UnityEngine.Object asst = pAssetBudle.LoadAsset<GameObject>(pefabConfig.AssetName);
                    if (asst)
                    {
                        assetTable.Add(pefabConfig.AssetName, asst);
                        m_PrefabDependenciesAssetList.Add(dpNode);
                    }
                    else
                    {
                        UnLoadDependenicesResource(dpNode);
                        Debug.LogWarning("找不到预制体:" + pefabConfig.AssetName + "在资源包:" + PrefabPatch);
                    }
               
                }
                else
                {
                    UnLoadDependenicesResource(dpNode);
                }

            }

            m_CommonAssetsBundleInstanceTable.Add(PrefabPatch, pAssetBudle);
            m_CommonAssetsTable.Add(PrefabPatch, assetTable);
        }
        else
        {
            AssetBudleLog("预制体通用包里没有资源:" + PrefabPatch);
        }
        return allAssets;
    }

    /// <summary>
    /// 删除资源
    /// </summary>
    /// <param name="res"></param>
    private static void UnLoadCommonAssets()
    {
        PrefabCommonAssetBundleNames.Clear();
        ArtistCommonAssetBundleNames.Clear();

        if(isOnRunTimeMode)
        {
            /*
             * 按照加载顺序来释放
             */
            foreach (PrefabDependenciesResNode dp in m_PrefabDependenciesAssetList)
            {
                UnLoadDependenicesResource(dp);
            }
            foreach (AssetBundle ab in m_CommonAssetsBundleInstanceTable.Values)
            {
                ab.Unload(true);
            }
        }

        m_PrefabDependenciesAssetList.Clear();
        m_CommonResNodeTable.Clear();
        m_CommonAssetsTable.Clear();
        m_CommonAssetsBundleInstanceTable.Clear();
    }

    /// <summary>
    /// 是否是通用资源，通过ab名来判断
    /// </summary>
    /// <param name="abName"></param>
    /// <returns></returns>
    private static bool IsCommonAssets(ResNode res)
    {
        return m_CommonAssetsTable.ContainsKey(res.AssetBundleName);
    }


    private static void GetCommonRes(ref ResNode reslut,ResConfigData config)
    {
        if(!isOnRunTimeMode)
        {
            return;
        }

        if (m_CommonResNodeTable.TryGetValue(config.AssetGUID, out reslut))
        {
            reslut.AddRef();
            return ;
        }
        Dictionary<string, UnityEngine.Object> assetTable = null;
        if (m_CommonAssetsTable.TryGetValue(config.AssetBundleName, out assetTable))
        {
            UnityEngine.Object asset = null;
            if (assetTable.TryGetValue(config.AssetName, out asset))
            {

                reslut = AllocResNode(config.AssetGUID);
                reslut.Clear();
                
                reslut.config = config;
                reslut.assetType = config.AssetBundleName.EndsWith(AssetBundleManager.PrefabVariant) ? AssetType.Asset_Prefab : AssetType.Asset_CommonAssets;
                reslut.isAsync = false;
                reslut.FillAssets(asset);
                reslut.status = ResNodeStatus.LoadFinish;
                reslut.AddRef();
                m_CommonResNodeTable.Add(config.AssetGUID, reslut);
            }
            else
            {
                AssetBudleLog("在通用资源包:" + config.AssetBundleName + "找不到资源:" + config.AssetName);
            }
        }
        return ;
    }

    private static void GetCommonRes(ref ResNode reslut,string AssetBundleName, string AssetName,System.Type t)
    {
        if(isOnRunTimeMode)
        {
            GetCommonRes_OnRunTime(ref reslut,AssetBundleName, AssetName, t);
        }
        else
        {
            GetCommonRes_OnEditor(ref reslut, AssetBundleName, AssetName, t);
        }
      
    }

    private static void GetCommonRes_OnRunTime(ref ResNode reslut, string AssetBundleName, string AssetName, System.Type t)
    {
        if (!isOnRunTimeMode)
        {
            return;
        }
        string str = AssetBundleName + AssetName;
        string hashCode = str.GetHashCode().ToString();
        if(m_CommonResNodeTable.TryGetValue(hashCode,out reslut))
        {
            reslut.AddRef();
            return;
        }
        Dictionary<string, UnityEngine.Object> assetTable = null;
        if (m_CommonAssetsTable.TryGetValue(AssetBundleName, out assetTable))
        {
            UnityEngine.Object asset = null;
            if (assetTable.TryGetValue(AssetName, out asset))
            {
                reslut = AllocResNode(hashCode.ToString());
                reslut.Clear();
                if(null == reslut.config)
                {
                    reslut.config = new ResConfigData();
                }
                reslut.config.AssetName = AssetName;
                reslut.config.AssetBundleName = AssetBundleName;
                reslut.config.AssetGUID = hashCode;
                reslut.isAsync = false;
                reslut.FillAssets(asset);
                reslut.status = ResNodeStatus.LoadFinish;
                reslut.AddRef();
             
                reslut.assetType = AssetBundleName.EndsWith(AssetBundleManager.PrefabVariant) ? AssetType.Asset_Prefab : AssetType.Asset_CommonAssets;
                m_CommonResNodeTable.Add(hashCode, reslut);
            }
            else
            {
                AssetBudleLog("在通用资源包:" + AssetBundleName + "找不到资源:" + AssetName);
            }
        }
        return;
    }

    private static void GetCommonRes_OnEditor(ref ResNode reslut, string AssetBundleName, string AssetName, System.Type t)
    {
        if (isOnRunTimeMode)
        {
            return;
        }

        string str = AssetBundleName + AssetName;
        string hashCode = str.GetHashCode().ToString();
        if (m_CommonResNodeTable.TryGetValue(hashCode, out reslut))
        {
            reslut.AddRef();
            return;
        }

        Dictionary<string, UnityEngine.Object> assetTable = null;
        UnityEngine.Object asset = null ;
        /*编辑器下找不到资源，就去加载*/
        if (m_CommonAssetsTable.TryGetValue(AssetBundleName, out assetTable))
        {
            if (assetTable.TryGetValue(AssetName, out asset))
            {

                reslut = AllocResNode(hashCode.ToString());
                reslut.Clear();
           
                if (null == reslut.config)
                {
                    reslut.config = new ResConfigData();
                }
                reslut.config.AssetName = AssetName;
                reslut.config.AssetBundleName = AssetBundleName;
                reslut.config.AssetGUID = hashCode;
                reslut.isAsync = false;
                reslut.FillAssets(asset);
                reslut.assetType = AssetBundleName.EndsWith(AssetBundleManager.PrefabVariant) ? AssetType.Asset_Prefab : AssetType.Asset_CommonAssets;
                reslut.AddRef();
                m_CommonResNodeTable.Add(hashCode, reslut);
            }
            else
            {
                UnityEngine.Object[] assets = LoadAssetOnEditorFromAbInstance(AssetBundleName, AssetName, t);
                if (null != assets && assets.Length > 0)
                {
                    assetTable = new Dictionary<string, UnityEngine.Object>();
                    foreach (UnityEngine.Object ob in assets)
                    {
                        assetTable.Add(ob.name, ob);
                    }
                    //再将东西取出来
                    asset = assetTable[AssetName];
                    reslut = AllocResNode(hashCode.ToString());
                    reslut.Clear();
                    reslut.FillAssets(asset);
                    reslut.status = ResNodeStatus.LoadFinish;
                    if(reslut.config == null)
                    {
                        reslut.config = new ResConfigData();
                    }
                    reslut.config.AssetName = AssetName;
                    reslut.config.AssetBundleName = AssetBundleName;
                    reslut.config.AssetGUID = hashCode;
                    reslut.isAsync = false;
                    reslut.assetType = AssetBundleName.EndsWith(AssetBundleManager.PrefabVariant) ? AssetType.Asset_Prefab : AssetType.Asset_CommonAssets;
                    reslut.AddRef();
                    m_CommonResNodeTable.Add(hashCode, reslut);
                }
                else
                {
                    AssetBudleLog("在通用资源包:" + AssetBundleName + "找不到资源:" + AssetName);
                }

            }
        }
        else
        {
            UnityEngine.Object[] assets = LoadAssetOnEditorFromAbInstance(AssetBundleName, AssetName, t);
            if(null != assets && assets.Length > 0)
            {
                assetTable = new Dictionary<string, UnityEngine.Object>();
                foreach(UnityEngine.Object ob in assets)
                {
                    assetTable.Add(ob.name, ob);
                }
                m_CommonAssetsTable.Add(AssetBundleName, assetTable);

                //再将东西取出来
                asset = assetTable[AssetName];
                reslut = AllocResNode(hashCode.ToString());
                reslut.Clear();
                
                if (reslut.config == null)
                {
                    reslut.config = new ResConfigData();
                }
                reslut.config.AssetName = AssetName;
                reslut.config.AssetBundleName = AssetBundleName;
                reslut.config.AssetGUID = hashCode;
                reslut.isAsync = false;
                reslut.FillAssets(asset);
                reslut.status = ResNodeStatus.LoadFinish;
                reslut.assetType = AssetBundleName.EndsWith(AssetBundleManager.PrefabVariant) ? AssetType.Asset_Prefab : AssetType.Asset_CommonAssets;
                reslut.AddRef();
                m_CommonResNodeTable.Add(hashCode, reslut);
            }
            else
            {
                AssetBudleLog("在通用资源包:" + AssetBundleName + "找不到资源:" + AssetName);
            }
        }
        return ;
    }
}
