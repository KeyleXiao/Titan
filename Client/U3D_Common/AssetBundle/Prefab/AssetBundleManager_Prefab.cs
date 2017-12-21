/*******************************************************************
** 文件名:	AssetBundleManager_Prefab.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	ab资源管理器
** 应  用:  	ab资源管理器，预制体部分

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
     * 预制体跟其他的资源不一样
     * 所以单独开出来，重新搞一搞。
     */


    /// <summary>
    /// 取得某个路径下的所有Prefab，包括子文件夹
    /// </summary>
    /// <param name="FolderPatch"></param>
    /// <returns></returns>
    private static List<ResNode> GetPrefabsFromPatch(string FolderPatch)
    {
        List<ResNode> reslut = new List<ResNode>();

        string loewrPatch = FolderPatch.ToLower();
        foreach (string s in m_RespackageTable.Keys)
        {
            if(s.Contains(loewrPatch))
            {
                ResNode nod = GetPrefab(FolderPatch);
                if(null != nod)
                {
                    reslut.Add(nod);
                }
            }
        }

        return reslut;
    }

    /// <summary>
    /// 取得预制体,相对于Prefabs目录
    /// </summary>
    /// <param name="strParam">[相对Prefabs路径,如:EntityPrefab/MonsterPrefab]</param>
    /// <param name="isGuid">参数是否是GUID</param>
    /// <returns></returns>
    private static ResNode GetPrefab(string strParam, bool isGuid = false)
    {
        if(null == strParam || strParam.Equals(string.Empty))
        {
            return null;
        }

        if(!isGuid)
        {
            string assetName = System.IO.Path.GetFileName(strParam);
            string assetBundleName = System.IO.Path.GetDirectoryName(strParam).ToLower();

            if (assetBundleName.Equals(string.Empty))
            {
                assetBundleName = "default";
            }

            assetBundleName = assetBundleName + "/" + assetName.ToLower() + "." + AssetBundleManager.PrefabVariant; ;
            string FullAssetBundleName = "gameobject/" + assetBundleName;

            return GetPrefab(FullAssetBundleName, assetName);
        }
        else
        {
            UnionResConfigData data = null;
            int InstanceID = GUIDToInstanceID(strParam);
            if (m_PrefabGUIDConfigTable.TryGetValue(InstanceID, out data))
            {
                return GetPrefab(data);
            }
            else
            {
                Debug.LogWarning("加载预制体失败，资源数据库中找不到GUID:" + strParam);
            }
            return null;
        }
        
    }

    /// <summary>
    /// 取得预制体
    /// </summary>
    /// <param name="assetBundeName">包名</param>
    /// <param name="assetName">资源名</param>
    /// <returns></returns>
    private static ResNode GetPrefab(string assetBundleName, string assetName)
    {
        Dictionary<string, UnionResConfigData> assetTable = null;
        if (m_PrefabConfigTable.TryGetValue(assetBundleName, out assetTable))
        {
            UnionResConfigData cof = null;
            if (assetTable.TryGetValue(assetName, out cof))
            {
                return GetPrefab(cof);
            }
        }
        Debug.LogWarning("在预制体配置中找不到:" + assetBundleName + "-" + assetName);
        return null;
    }

    private static ResNode GetPrefab(UnionResConfigData config)
    {
        if (!isVaild)
        {
            return null;
        }
        if(isOnRunTimeMode)
        {
            if( config.unionIndx < 0 || config.startIndex < 0 || config.dataLength <= 0)
            {
                Debug.LogWarning("confing is invalid:" + config.AssetName);
                return null;
            }
        }
        ResNode res = null;

        //看看是不是通用资源
        GetCommonRes(ref res,config);
        if(null != res)
        {
            return res;
        }

        res = GetCachePrefabResource(config);
        if (null != res)
        {
            res.AddRef();
            m_PrefabResTable.Add(res.instanceID, res);
            return res;
        }

        double loadbegain = DateTime.Now.Ticks;
        int id = GUIDToInstanceID(config.AssetGUID);
        //如果还在异步加载中,这里是依赖的资源异步还没有完成
        if (m_AsyncLoadingDependicensPrefabResNode.TryGetValue(id, out res))
        {
            BuildPrefabWhenInAsyncLoading(ref res, config);
            res.AddRef();
            return res;
        }

        res = AllocResNode(id);
        if (m_PrefabResTable.TryGetValue(res.instanceID, out res))
        {
            //如果是异步加载，这里是依赖资源加载完了，但是预制体本身还没有出来。
            if(res.isAsync)
            {
                if(!res.isLoadFinish)
                {
                    BuildResNodeWhenInAsyncLoading<GameObject>(ref res, config);
                }
            }
            res.AddRef();
            return res;
        }

        //非运行时模式，不需要去加载依
        if(!isOnRunTimeMode)
        {
            GetPrefabResource(ref res, config);
        }
        else
        {
            /*加载完依赖项之后，再去加载目标资源，要检测目标资源是否成功
            * 如果加载失败，要立刻释放这些资源。
            */
            if (LoadDependenices(config))
            {
                //依赖资源都加载完了，是时候加载这个真正的预制体了
                GetPrefabResource(ref res, config);

                if (null == res)
                {
                    UnLoadDependenices(id);
                }
            }
        }

        if (null != res)
        {
            res.assetType = AssetType.Asset_Prefab;
            //SaveMemory(res);
            ReleaseDependenicesAssetBundleIfNeed(res);
            ReleaseAssetBundleIfNeed(res);
            //if(!SceneManager.isLoadingScene && !SceneManager.Instance.isMainCity())
            //{
            //    loadbegain = GameUtil.TicksToSecond(DateTime.Now.Ticks - loadbegain);
            //    if (loadbegain > 0.15f)
            //    {
            //        loadbegain *= 1000;
            //        Debug.LogWarning("资源加载超过150ms,当前时间:" + loadbegain .ToString("F3") + ",资源名:" + res.AssetName + ",请预加载或者使用异步加载此资源");
            //    }
            //}

        }

        return res;
    }


    public static void DeletePrefab(ref ResNode res,bool Immediate = true)
    {
        if (null == res || !isVaild || res.GetRefCount() <= 0)
        {
            return;
        }


        //先看看是不是通用资源
        if(IsCommonAssets(res))
        {
            res.Release();
            return;
        }

        if(!res.UnLoadImmediate)
        {
            res.UnLoadImmediate = Immediate;
        }
        //有这个资源
        if (m_PrefabResTable.ContainsKey(res.instanceID))
        {
            int refCount = res.Release();

            //这个资源已经没有人引用了
            if (refCount <= 0)
            {
                DeletePrefabResource(ref res, Immediate);
                
            }
        }
        else
        {
            Debug.LogError("要卸载的资源不是预制体,忽略此资源，有可能会存在资源泄露!:" + res.AssetBundleName + "," + res.AssetName);
        }
        res = null;
    }



    /// <summary>
    /// 异步取得Prefab,返回值只是一个标示，没有资源，资源在回调中,相对于Prefabs路径
    /// </summary>
    /// <param name="patch">路径,如:EntityPrefab/MonsterPrefab ,Skin/Hero</param>
    /// <returns></returns>
    private static ResNode GetPrefabAsync(string strParam, bool isGuid,
                                       System.Action<ResConfigData, ResNode, System.Object> callBack,
                                      System.Object userDataObj = null, AssetLoadPriority priority = AssetLoadPriority.Priority_Normal)
    {

        if (null == strParam || strParam.Equals(string.Empty))
        {
            return null;
        }

        if (!isGuid)
        {
            string assetName = System.IO.Path.GetFileName(strParam);
            string assetBundleName = System.IO.Path.GetDirectoryName(strParam).ToLower();

            if (assetBundleName.Equals(string.Empty))
            {
                assetBundleName = "default";
            }

            assetBundleName = assetBundleName + "/" + assetName.ToLower() + "." + AssetBundleManager.PrefabVariant; ;
            string FullAssetBundleName = "gameobject/" + assetBundleName;

            return GetPrefabAsync(FullAssetBundleName, assetName, callBack, userDataObj, priority);
        }
        else
        {
            UnionResConfigData data = null;
            int InstanceID = GUIDToInstanceID(strParam);
            if (m_PrefabGUIDConfigTable.TryGetValue(InstanceID, out data))
            {
                return GetPrefabAsync(data, callBack, userDataObj, priority);
            }
            else
            {
                Debug.LogWarning("加载预制体失败，资源数据库中找不到GUID:" + strParam);
            }
            return null;
        }

    }

    /// <summary>
    /// 异步取得Prefab
    /// </summary>
    /// <param name="assetBundeName">包名</param>
    /// <param name="assetName">资源名</param>
    /// <returns></returns>
    private static ResNode GetPrefabAsync(string assetBundleName, string assetName,
                                        System.Action<ResConfigData, ResNode, System.Object> callBack,
                                       System.Object userDataObj = null, AssetLoadPriority priority = AssetLoadPriority.Priority_Normal)
    {
        Dictionary<string, UnionResConfigData> assetTable = null;
        if (m_PrefabConfigTable.TryGetValue(assetBundleName, out assetTable))
        {
            UnionResConfigData cof = null;
            if (assetTable.TryGetValue(assetName, out cof))
            {
                return GetPrefabAsync(cof, callBack, userDataObj, priority);
            }
        }
        Debug.LogWarning("在预制体配置中找不到:" + assetBundleName + "-" + assetName);
        return null;
    }

    /// <summary>
    /// 异步取得Prefab,返回值只是一个标示，没有资源，资源在回调中
    /// </summary>
    /// <param name="config">资源配置</param>
    /// <param name="callBack">回调</param>
    /// <param name="userDataObj">用户数据</param>
    private static ResNode GetPrefabAsync(UnionResConfigData config,
                                       System.Action<ResConfigData, ResNode, System.Object> callBack,
                                      System.Object userDataObj = null, AssetLoadPriority priority = AssetLoadPriority.Priority_Normal)
    {
        if (!isVaild)
        {
            return null;
        }
        ResNode reslut = null;
        config.priority = priority;
        //编辑器下无异步加载
        if(!isOnRunTimeMode)
        {
            reslut = GetPrefab(config);
            PushToResDelayCallBackList(reslut, config, callBack, userDataObj);
            return reslut;
        }

        if (config.unionIndx < 0 || config.startIndex < 0 || config.dataLength <= 0)
        {
            Debug.LogWarning("confing is invalid:" + config.AssetName);
            return null;
        }

        GetCommonRes(ref reslut, config);
        if(null != reslut)
        {
            PushToResDelayCallBackList(reslut, config, callBack, userDataObj);
            return reslut;
        }
        int InstanceID = GUIDToInstanceID(config.AssetGUID);

        reslut = GetResNodeFromNeedToUnLoadAsset(InstanceID);
        if (null != reslut)//表里面有这个预制体的话，说明依赖已经加载完毕
        {
            reslut.AddRef();
            m_PrefabResTable.Add(InstanceID, reslut);

            GeneratePrefabCallBack(reslut, config, callBack, userDataObj);
            reslut.config = config;
            return reslut;
        }


        reslut = GetCachePrefabResource(config);
        if (null != reslut)//表里面有这个预制体的话，说明依赖已经加载完毕
        {
            reslut.AddRef();
            m_PrefabResTable.Add(InstanceID, reslut);
            if (!reslut.isLoadFinish)
            {
                GeneratePrefabCallBack(reslut, config, callBack, userDataObj);
            }
            else
            {
                PushToResDelayCallBackList(reslut, config, callBack, userDataObj);
            }

            reslut.config = config;
            return reslut;
        }

        //先看看表中是否有这个配置
        m_PrefabResTable.TryGetValue(InstanceID, out reslut);
        if(null == reslut)
        {
            m_AsyncLoadingDependicensPrefabResNode.TryGetValue(InstanceID, out reslut);
            if(null == reslut)
            {
                reslut = AllocResNode(config.AssetGUID);
                reslut.config = config;
                reslut.assetType = AssetType.Asset_Prefab;
                reslut.status = ResNodeStatus.WatingForLoad;
                reslut.isAsync = true;
                bool b = LoadDependenicesAsync(config);
                //依赖加载失败
                if(!b)
                {
                    reslut = null;
                    return null;
                }
                m_AsyncLoadingDependicensPrefabResNode.Add(InstanceID, reslut);
            }
            reslut.AddRef();
            GeneratePrefabCallBack(reslut, config, callBack, userDataObj);
            reslut.config = config;
        }
        else
        {
            //再看看加载列表中是否有这个配置,有就等待回调
            if (m_PrefabRequestTable.ContainsKey(InstanceID))
            {
                reslut.AddRef();
                GeneratePrefabCallBack(reslut, config, callBack, userDataObj);
            }
            else
            {
                PushToResDelayCallBackList(reslut, config, callBack, userDataObj);
            }
            reslut.AddRef();
            reslut.config = config;
        }
        return reslut;
    }

    private static void DeletePrefabAsync(ref ResNode res, System.Action<ResConfigData, ResNode, System.Object> callBack,
                                        bool Immediate = true)
    {

        if (null == res || !isVaild || res.GetRefCount() <= 0)
        {
            return;
        }
        //先看看是不是通用资源
        if (IsCommonAssets(res))
        {
            res.Release();
            RemoveFromDelayCallBackList(res, callBack);
            return;
        }
        int guid = res.instanceID;
        //表里面有这个预制体的话，说明依赖是加载成功了的。但是预制体本体可能加载完成，也可能是在加载中
        if (m_PrefabResTable.ContainsKey(guid))
        {
            int refCount = res.Release();

            //这个资源已经没有人引用了
            if (refCount <= 0)
            {
                RemoveAllDelayCallBackList(res);
                RemoveAllResCallBack(m_PrefabRequestTable, res);
                DeletePrefabResourceAsync(ref res, PrefabResourceCallBack, Immediate);
               
            }
            else
            {
                RemoveFromDelayCallBackList(res, callBack);
                RemoveResCallBack(m_PrefabRequestTable, res, callBack);
            }
        }
        else
        {
            //如果没有在预制体表里的，说明还处于依赖加载中
            if (m_AsyncLoadingDependicensPrefabResNode.ContainsKey(guid))
            {
                int refCount = res.Release();

                //这个资源已经没有人引用了
                if (refCount <= 0)
                {

                    UnLoadDependenicesAsync(guid);
                    //m_NeedToDeletePrefab.Add(guid);
                    m_AsyncLoadingDependicensPrefabResNode.Remove(guid);

                    RemoveAllDelayCallBackList(res);
                    RemoveAllResCallBack(m_PrefabRequestTable, res);
                    res.Clear();
                }
                else
                {
                    RemoveFromDelayCallBackList(res, callBack);
                    RemoveResCallBack(m_PrefabRequestTable, res, callBack);
                }
            }
            else
            {
                Debug.LogError("要卸载的资源不是预制体,忽略此资源，有可能会存在资源泄露!:" + res.AssetBundleName + "," + res.AssetName);
            }
        }

        res = null;
    }
}
