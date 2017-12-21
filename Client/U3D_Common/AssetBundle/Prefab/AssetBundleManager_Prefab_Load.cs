/*******************************************************************
** 文件名:	AssetBundleManager_FBX.cs
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

    /// <summary>
    /// 正在使用的资源节点,资源GUID，资源结点 与m_PrefabCacheTable是互斥的存在
    /// </summary>
    private static Dictionary<int, ResNode> m_PrefabResTable = new Dictionary<int, ResNode>();

    /// <summary>
    /// 缓存的资源结点，GUID-资源结点 与m_PrefabTable是互斥的存在
    /// </summary>
    private static Dictionary<int, ResNode> m_PrefabResCacheTable = new Dictionary<int, ResNode>();


    private static Type PrefabType = typeof(GameObject);

    /// <summary>
    /// 获取资源，资源内部使用引用计数来管理资源，切勿乱调用,一个get，一个delete
    /// </summary>
    /// <param name="config"></param>
    /// <returns></returns>
    private static void GetPrefabResource(ref ResNode result,UnionResConfigData config)
    {
        if (!isVaild)
        {
            return ;
        }
        
        if (string.IsNullOrEmpty(config.AssetBundleName))
        {
            return ;
        }
        //先看看是不是通用资源
        GetCommonRes(ref result, config);
        if (null != result)
        {
            return ;
        }


        //先看看缓存里是不是有这个资源
        result = GetCachePrefabResource(config);

        if (null != result)
        {
            m_PrefabResTable.Add(result.instanceID, result);
            //判断一下这个资源是否是异步加载的
            if (result.isAsync)
            {
                if (!result.isLoadFinish)
                {
                    BuildResNodeWhenInAsyncLoading<GameObject>(ref result, config);
                    //Debug.LogWarning(StringHelper.BuildString(config.AssetName, "正在异步加载中，请调用ResNode.isLoadFinish查看是否完成"));
                }
            }
            result.AddRef();
            return ;
        }

        int InstanceID = GUIDToInstanceID(config.AssetGUID);

        //所需的资源已经存在
        if (m_PrefabResTable.TryGetValue(InstanceID,out result))
        {
            ////判断一下这个资源是否是异步加载的,预制体的这个在上层判断
            //if (result.isAsync)
            //{
            //    if (!result.isLoadFinish)
            //    {
            //        BuildResNodeWhenInAsyncLoading<GameObject>(ref result, config);
            //        //Debug.LogWarning(StringHelper.BuildString(config.AssetName, "正在异步加载中，请调用ResNode.isLoadFinish查看是否完成"));
            //    }
            //}
            //同时增加这个资源的引用计数
            result.AddRef();
        }
        else
        {
            BuildPrefabResNode(ref result,config);
            if (result != null)
            {
                m_PrefabResTable.Add(result.instanceID, result);
            }
        }
        if (null != result)
        {
            result.assetType = AssetType.Asset_Prefab;
        }
        return;
    }


    /// <summary>
    /// 删除资源
    /// </summary>
    /// <param name="res"></param>
    private static void DeletePrefabResource(ref ResNode res, bool Immediate)
    {
        //预制体比较特殊，在上层判断引用计数
        if (null == res || !isVaild /*||
            res.GetRefCount() <= 0*/)
        {
            return;
        }

        //只要有一个需要立即卸载，则立即卸载
        if(!res.UnLoadImmediate)
        {
            res.UnLoadImmediate = Immediate; 
        }
        //有这个资源
        if (m_PrefabResTable.ContainsKey(res.instanceID))
        {
            m_PrefabResTable[res.instanceID] = null;
            m_PrefabResTable.Remove(res.instanceID);

            if (!res.UnLoadImmediate)
            {
                CachePrefabResource(res);
            }
            else
            {
                if (m_PrefabResCacheTable.ContainsKey(res.instanceID))
                {
                    m_PrefabResCacheTable.Remove(res.instanceID);
                }
                UnLoadDependenices(res.instanceID);
                UnLoadResource(ref res);
            }
        }
        else
        {
            Debug.LogError("要卸载的资源不是Prefab,忽略此资源，有可能会存在资源泄露!:" + res.AssetBundleName + "," + res.AssetName);
        }
        res = null;
    }

    private static void BuildPrefabWhenInAsyncLoading(ref ResNode res, UnionResConfigData config)
    {
        //先检查
        if (null == res)
        {
            return;
        }

        if (!isOnRunTimeMode)
        {
            return;
        }

        if (!res.isAsync)
        {
            return;
        }

        if (res.isLoadFinish)
        {
            return;

        }

        res.StartLoadedTime();
        //先从回调列表里移除
        if (m_AsyncLoadingDependicensPrefabResNode.ContainsKey(res.instanceID))
        {
            m_AsyncLoadingDependicensPrefabResNode.Remove(res.instanceID);
        }
        //卸载依赖
        UnLoadDependenicesAsync(res.instanceID);

        //开始加载依赖
        if(LoadDependenices(config))
        {
            ResourcePackage pkg = null;

            pkg = GetResourcePackge(config);

            if (null != pkg)
            {
                LoadAssetFromAssetBundle<GameObject>(ref res, config, pkg.AssetBundleInstance);
            }
            else
            {
                AssetBudleLog("加载AssetBundle失败:" + config.AssetBundleName);
            }

            if(!res.IsValid())
            {
                UnLoadDependenices(res.instanceID);
            }
        }
        else
        {
            UnLoadDependenices(res.instanceID);
        }

        res.status = ResNodeStatus.LoadFinish;
        m_PrefabResTable.Add(res.instanceID, res);

        //取出回调列表
        List<AsyncLoadingObject> list = null;
        if (m_PrefabRequestTable.TryGetValue(res.instanceID, out list))
        {
            ReleaseDependenicesAssetBundleIfNeed(res);
            ReleaseAssetBundleIfNeed(res);
            m_PrefabRequestTable.Remove(res.instanceID);

            foreach (AsyncLoadingObject obj in list)
            {
                PushToResDelayCallBackList(res, config, obj.callBack, obj.UserDataObj);
            }
        }


        return;
    }

    private static void BuildPrefabResNode(ref ResNode result,UnionResConfigData config)
    {

        if(!isOnRunTimeMode)
        {
            BuildResNodeOnEditor<GameObject>(ref result, config);
        }
        else
        {
            BuildResNodeOnRunTime<GameObject>(ref result, config);
        }
        return ;
    }

    /// <summary>
    /// 获取资源，资源内部使用引用计数来管理资源，切勿乱调用,一个get，一个delete
    /// 返回的值仅仅是一个标示，没有资源~
    /// </summary>
    /// <param name="config">资源配置</param>
    /// <param name="callBack">回调</param>
    /// <param name="userDataObj">用户数据，这个会出现在资源回调里,不需要可以填null</param>
    /// <param name="priority">加载优先级</param>
    /// <returns></returns>
    private static void GetPrefabResourceAsync(ref ResNode res,UnionResConfigData config, System.Action<ResConfigData, ResNode, System.Object> callBack,
                                                   System.Object userDataObj = null, AssetLoadPriority priority = AssetLoadPriority.Priority_Normal)
    {
        if (!isVaild)
        {

            return ;
        }

        if (string.IsNullOrEmpty(config.AssetBundleName))
        {
            return ;
        }

        config.priority = priority;
        BuildPrefabResNodeAsync(ref res,config, callBack, userDataObj);
        if (res != null)
        {
            res.assetType = AssetType.Asset_Prefab;
        }
        return;
    }

    /// <summary>
    /// 删除异步加载的资源，与GetPrefabResourceAsync对应
    /// </summary>
    /// <param name="res"></param>
    /// <param name="callBack"></param>
    private static void DeletePrefabResourceAsync(ref ResNode res, System.Action<ResConfigData, ResNode, System.Object> callBack,bool Immediate)
    {
        //已在上一级判断,不在relase
        if (null == res || !isVaild /*||
            res.GetRefCount() <= 0*/)
        {
            return;
        }

        if (!res.UnLoadImmediate)
        {
            res.UnLoadImmediate = Immediate;
        }
        
        //编辑器下没有异步
        if(!isOnRunTimeMode)
        {
            DeletePrefabResource(ref res, Immediate);
            RemoveFromDelayCallBackList(res, callBack);
            return;
        }
       
        int guid = res.instanceID;
        //有这个资源
        if (m_PrefabResTable.ContainsKey(guid))
        {
            
            RemoveAllResCallBack(m_ResCallBackTable, res);
            RemoveAllDelayCallBackList(res);

            m_PrefabResTable[guid] = null;
            m_PrefabResTable.Remove(guid);

            if(!res.UnLoadImmediate)
            {
                CachePrefabResource(res);
            }
            else
            {
                if (m_PrefabResCacheTable.ContainsKey(guid))
                {
                    m_PrefabResCacheTable.Remove(guid);
                }
                UnLoadDependenicesAsync(guid);
                UnLoadResource(ref res);
            }

        }
        else
        {
            Debug.LogError("要卸载的资源不是Prefab,忽略此资源，有可能会存在资源泄露!:" + res.AssetBundleName + "," + res.AssetName);
        }

        res = null;
    }



    /// <summary>
    /// 异步构建ResNode，会立即返回一个ResNode，这个ResNode只是一个标示，里面没有资源!
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="config"></param>
    /// <param name="callBack"></param>
    /// <param name="userDataObj"></param>
    /// <returns></returns>
    private static void BuildPrefabResNodeAsync(ref ResNode res,UnionResConfigData config, System.Action<ResConfigData, ResNode, System.Object> callBack,
                                                    System.Object userDataObj)
    {

        int assetGUID = GUIDToInstanceID(config.AssetGUID);

        //缓存相关，已在上层判断
        ////如果这个资源正在要卸载的列表里，移除掉,免得资源加载完就被卸载了
        //RemoveNeedToUnLoadAsset(assetGUID);

        //ResNode CacheRes = GetCachePrefabResource(config);

        ////如果缓存里有这个资源结点
        //if (CacheRes != null)
        //{
        //    //缓存中的Res应该要和传进来的Res一样！
        //    if (null != res && CacheRes != res)
        //    {
        //        Debug.LogError("预制体异步加载超级bug");
        //        return;
        //    }
        //    res = CacheRes;
        //    res.AddRef();
        //    m_PrefabResTable.Add(assetGUID, res);

        //    //看看资源是不是已经加载完成了。
        //    if (m_LoadingAssetTable.ContainsKey(assetGUID))
        //    {
        //        GenerateResCallBack(res, config, callBack, userDataObj);
        //    }
        //    else
        //    {
        //        //直接回调
        //        PushToResDelayCallBackList(res, config, callBack, userDataObj);
        //    }
        //    return;
        //}
        //else
        {
            //没有在缓存里，就看看是不是已经在使用了
            if (m_PrefabResTable.ContainsKey(assetGUID))
            {
                res = m_PrefabResTable[assetGUID];
                res.AddRef();

                //看看资源是不是已经加载完成了。
                if (!res.isLoadFinish)
                {
                    GenerateResCallBack(res, config, callBack, userDataObj);
                }
                else
                {
                    PushToResDelayCallBackList(res, config, callBack, userDataObj);
                }

                return;
            }
        }

        /*没有缓存，没有在使用，说明这是一个全新的请求，直接走下面，创建一个新的请求*/

        GetPrefabPackgeAsync(ref res,config, PrefabPackageCallBack, callBack, userDataObj);

    }

    private static void GetPrefabPackgeAsync(ref ResNode res,UnionResConfigData config, System.Action<ResourcePackageCallBackObj> resourcePackageCallBack,
                                           System.Action<ResConfigData, ResNode, System.Object> UserCallBack, System.Object userDataObj)
    {
        BuildPacakgeAsync(ref res,m_PrefabResTable, config, resourcePackageCallBack, UserCallBack, userDataObj,true);
    }

    private static float GetPrefabUpLoadClipTime(ResNode res)
    {
        return MaxUpLoad_Assets_AsyncRequestClip * 0.5f;
    }

    private static bool UpLoadPrefabAsyncRequest(ResourcePackageCallBackObj req)
    {
        return UpLoadAsyncRequest<GameObject>(req,true);
    }

    private static void PrefabPackageCallBack(ResourcePackageCallBackObj req)
    {
        EnQueueWaitForUpLoadAssetsAsyncRequestTable(PrefabType, req);
    }

    /// <summary>
    /// 缓存，避免频繁的加载
    /// </summary>
    /// <param name="res"></param>
    private static void CachePrefabResource(ResNode res)
    {
        res.cacheTime = 0.0f;

        //缓存起来
        m_PrefabResCacheTable.Add(res.instanceID, res);
    }


    private static ResNode GetCachePrefabResource(UnionResConfigData cof)
    {
        ResNode res = null;
        int InstanceID = GUIDToInstanceID(cof.AssetGUID);
        if (m_PrefabResCacheTable.TryGetValue(InstanceID, out res))
        {
            res.cacheTime = 0.0f;
            m_PrefabResCacheTable.Remove(InstanceID);
        }

        return res;
    }

    private static void ClearPrefabRes()
    {
        ClearResTable(ref m_PrefabResTable);
        ClearResTable(ref m_PrefabResCacheTable);

        m_AsyncLoadingDependicensPrefabResNode.Clear();
        AsyncDependenicesLoadCompleteList.Clear();
        AsyncDependenicesLoadInValidList.Clear();
        m_AsyncLoadingDependicensPrefabResNode.Clear();
        m_PrefabRequestTable.Clear();
        m_DependenicesRequestTable.Clear();
       // m_NeedToDeletePrefab.Clear();

    }

    /// <summary>
    /// 更新纹理缓存
    /// </summary>
    /// <param name="LoadCompleteList"></param>
    private static void UpdatePrefabCache(List<int> LoadCompleteList, float dealtTime)
    {
        if (m_PrefabResCacheTable.Count > 0)
        {
            LoadCompleteList.Clear();
            foreach (KeyValuePair<int, ResNode> r in m_PrefabResCacheTable)
            {
                ResNode res = r.Value;
                res.cacheTime += dealtTime;

                //到了缓存时间，卸载资源
                if (res.cacheTime > prefabCacheTime)
                {
                    LoadCompleteList.Add(r.Key);
                    UnLoadDependenices(res.instanceID);
                    UnLoadResource(ref res);

                }
            }

            //移除已经加载完成的
            foreach (int s in LoadCompleteList)
            {
                m_PrefabResCacheTable[s] = null;
                m_PrefabResCacheTable.Remove(s);
            }
        }
    }


    
}








