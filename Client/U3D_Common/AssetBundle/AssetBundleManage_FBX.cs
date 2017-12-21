/*******************************************************************
** 文件名:	AssetBundleManager_FBX.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	ab资源管理器
** 应  用:  	ab资源管理器，FBX部分

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
    /// 正在使用的资源节点,资源GUID，资源结点 与m_FBXCacheResTable是互斥的存在
    /// </summary>
    private static Dictionary<int, ResNode> m_FBXResTable = new Dictionary<int, ResNode>();

    /// <summary>
    /// 缓存的资源结点，GUID-资源结点 与m_GameObjectResTable是互斥的存在
    /// </summary>
    private static Dictionary<int, ResNode> m_FBXCacheResTable = new Dictionary<int, ResNode>();


    private static Type FBXType = typeof(FBXAssets);
    private static Type MeshType = typeof(Mesh);
    private static Type AvatarType = typeof(Avatar);

    /// <summary>
    /// 获取资源，资源内部使用引用计数来管理资源，切勿乱调用,一个get，一个delete
    /// </summary>
    /// <param name="config"></param>
    /// <returns></returns>
    private static ResNode GetFBXResource(ResConfigData config)
    {
        if (!isVaild)
        {
            return null;
        }

        if (string.IsNullOrEmpty(config.AssetBundleName))
        {
            return null;
        }
        ResNode res = null;

        //先看看是不是通用资源
        GetCommonRes(ref res,config);
        if(null != res)
        {
            return res;
        }

        if(IsUnionDependenicesAssetBundle(config.AssetBundleName))
        {
            if(!config.fromReference)
            {
                AssetBudleLog(StringHelper.BuildString("资源错误--uniondepndenices不能单独加载!:",config.AssetName));
                return res;
            }
        }

        //先看看缓存里是不是有这个资源
        res = GetCacheFBXResource(config);

        if (null != res)
        {
            m_FBXResTable.Add(res.instanceID, res);
            //判断一下这个资源是否是异步加载的
            if (res.isAsync)
            {
                if (!res.isLoadFinish)
                {
                    BuildResNodeWhenInAsyncLoading<FBXAssets>(ref res, config);
                    //Debug.LogWarning(StringHelper.BuildString(config.AssetName, "正在异步加载中，请调用ResNode.isLoadFinish查看是否完成"));
                }
            }
            res.AddRef();
            return res;
        }

        int InstanceID = GUIDToInstanceID(config.AssetGUID);
        //所需的资源已经存在
        if (m_FBXResTable.TryGetValue(InstanceID,out res))
        {
            //判断一下这个资源是否是异步加载的
            if(res.isAsync)
            {
                if(!res.isLoadFinish)
                {
                    BuildResNodeWhenInAsyncLoading<FBXAssets>(ref res, config);
                    //Debug.LogWarning(StringHelper.BuildString(config.AssetName, "正在异步加载中，请调用ResNode.isLoadFinish查看是否完成"));
                }
            }
            //同时增加这个资源的引用计数
            res.AddRef();
        }
        else
        {
            res = BuildFBXResNode(config);
            if (res != null)
            {
                m_FBXResTable.Add(res.instanceID, res);
            }
        }
        if (null != res)
        {
            res.assetType = AssetType.Asset_FBX;
        }
        return res;
    }

    /// <summary>
    /// 删除资源
    /// </summary>
    /// <param name="res"></param>
    private static void DeleteFBXResource(ref ResNode res, bool Immediate)
    {
        if (null == res || !isVaild ||
            res.GetRefCount() <= 0)
        {
            return;
        }

        //先看看是不是通用资源
        if (IsCommonAssets(res))
        {
            res.Release();
            return;
        }
        //只要有一个需要立即卸载，则立即卸载
        if (!res.UnLoadImmediate)
        {
            res.UnLoadImmediate = Immediate;
        }
        int guid = res.instanceID;
        //有这个资源
        if (m_FBXResTable.ContainsKey(guid))
        {
            int refCount = res.Release();

            //这个资源已经没有人引用了
            if (refCount <= 0)
            {
                m_FBXResTable[guid] = null;
                m_FBXResTable.Remove(guid);

                if (!res.UnLoadImmediate)
                {
                    CacheFBXResource(res);
                }
                else
                {
                    if (m_FBXCacheResTable.ContainsKey(guid))
                    {
                        m_FBXCacheResTable.Remove(guid);
                    }
                    UnLoadResource(ref res);
                }
            }
        }
        else
        {
            Debug.LogError("要卸载的资源不是FBX,忽略此资源，有可能会存在资源泄露!:" + res.AssetBundleName + "," + res.AssetName);
        }

        res = null;
    }

    private static ResNode BuildFBXResNode(ResConfigData config)
    {
        ResNode res = null;

        if(!isOnRunTimeMode)
        {
            BuildResNodeOnEditor<FBXAssets>(ref res, config);
        }
        else
        {
            BuildResNodeOnRunTime<FBXAssets>(ref res, config);
        }

        return res;
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
    private static ResNode GetFBXResourceAsync(ResConfigData config, System.Action<ResConfigData, ResNode, System.Object> callBack,
                                                   System.Object userDataObj = null, AssetLoadPriority priority = AssetLoadPriority.Priority_Normal)
    {
        if (!isVaild || !isVaild)
        {

            return null;
        }
        if (string.IsNullOrEmpty(config.AssetBundleName))
        {
            return null;
        }
        ResNode res = null;

        //先看看是不是通用资源
        GetCommonRes(ref res,config);
        if (null != res)
        {
            PushToResDelayCallBackList(res, config, callBack, userDataObj);
            return res;
        }


        //编辑器下无异步加载
        if(!isOnRunTimeMode)
        {
            res = GetFBXResource(config);
            PushToResDelayCallBackList(res, config, callBack, userDataObj);
            return res;
        }

        if (IsUnionDependenicesAssetBundle(config.AssetBundleName))
        {
            if (!config.fromReference)
            {
                AssetBudleLog(StringHelper.BuildString("资源错误--uniondepndenices不能单独加载!:", config.AssetName));
                return res;
            }
        }

        config.priority = priority;
        res = BuildFBXResNodeAsync(config, callBack, userDataObj);
        if(res != null)
        {
            res.assetType = AssetType.Asset_FBX;
        }
        return res;
    }


    /// <summary>
    /// 删除异步加载的资源，与GetFBXResourceAsync对应
    /// </summary>
    /// <param name="res"></param>
    /// <param name="callBack"></param>
    private static void DeleteFBXResourceAsync(ref ResNode res, System.Action<ResConfigData, ResNode, System.Object> callBack, bool Immediate)
    {
        if (null == res || !isVaild)
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

        //只要有一个需要理解卸载，那么就得立刻卸载
        if (!res.UnLoadImmediate)
        {
            res.UnLoadImmediate = Immediate;
        }


        //编辑器下没有异步
        if(!isOnRunTimeMode)
        {
            RemoveFromDelayCallBackList(res, callBack);
            DeleteFBXResource(ref res, Immediate);
            return;
        }
        int InstanceID = res.instanceID;
        //有这个资源
        if (m_FBXResTable.ContainsKey(InstanceID))
        {
            int refCount = res.Release();
            //这个资源已经没有人引用了
            if (refCount <= 0)
            {
                RemoveAllResCallBack(m_ResCallBackTable, res);
                RemoveAllDelayCallBackList(res);

                m_FBXResTable[InstanceID] = null;
                m_FBXResTable.Remove(InstanceID);

                if (!res.UnLoadImmediate)
                {
                    CacheFBXResource(res);
                }
                else
                {
                    if (m_FBXCacheResTable.ContainsKey(InstanceID))
                    {
                        m_FBXCacheResTable.Remove(InstanceID);
                    }
                    UnLoadResource(ref res);
                }

            }
            else
            {
                RemoveResCallBack(m_ResCallBackTable, res, callBack);
                RemoveFromDelayCallBackList(res, callBack);
            }
        }
        else
        {
           
            Debug.LogError("要卸载的资源不是FBX,忽略此资源，有可能会存在资源泄露!:" + res.AssetBundleName + "," + res.AssetName);
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
    private static ResNode BuildFBXResNodeAsync(ResConfigData config, System.Action<ResConfigData, ResNode, System.Object> callBack,
                                                    System.Object userDataObj)
    {
        ResNode res = null;

        int assetGUID = GUIDToInstanceID(config.AssetGUID);

        //如果这个资源正在要卸载的列表里，移除掉,免得资源加载完就被卸载了
        res = GetResNodeFromNeedToUnLoadAsset(assetGUID);

        if (res != null)
        {
            res.AddRef();
            m_FBXResTable.Add(assetGUID, res);

            GenerateResCallBack(res, config, callBack, userDataObj);
            res.config = config;
            return res;
        }

        res = GetCacheFBXResource(config);

        //如果缓存里有这个资源结点
        if (res != null)
        {
            res.AddRef();
            m_FBXResTable.Add(assetGUID, res);

            //看看资源是不是已经加载完成了。
            if (!res.isLoadFinish)
            {
                GenerateResCallBack(res, config, callBack, userDataObj);
            }
            else
            {
                PushToResDelayCallBackList(res, config, callBack, userDataObj);
            }
            res.config = config;
            return res;
        }

        //没有在缓存里，就看看是不是已经在使用了
        if (m_FBXResTable.TryGetValue(assetGUID,out res))
        {
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
            res.config = config;
            return res;
        }

        /*没有缓存，没有在使用，说明这是一个全新的请求，直接走下面，创建一个新的请求*/
        res = GetFBXPackgeAsync(config, FBXPackageCallBack, callBack, userDataObj);


        return res;
    }


    private static ResNode GetFBXPackgeAsync(ResConfigData config, System.Action<ResourcePackageCallBackObj> resourcePackageCallBack,
                                       System.Action<ResConfigData, ResNode, System.Object> UserCallBack, System.Object userDataObj)
    {

        ResNode res = null;
        BuildPacakgeAsync(ref res, m_FBXResTable, config, resourcePackageCallBack, UserCallBack, userDataObj);
        return res;
    }

    private static float GetFBXUpLoadClipTime(ResNode res)
    {
        return MaxUpLoad_Assets_AsyncRequestClip;
    }


    private static bool UpLoadFBXAsyncRequest(ResourcePackageCallBackObj req)
    {
        return UpLoadAsyncRequest<FBXAssets>(req);
    }

    private static void FBXPackageCallBack(ResourcePackageCallBackObj req)
    {
        EnQueueWaitForUpLoadAssetsAsyncRequestTable(FBXType, req);
    }

    /// <summary>
    /// 缓存，避免频繁的加载
    /// </summary>
    /// <param name="res"></param>
    private static void CacheFBXResource(ResNode res)
    {
        res.cacheTime = 0.0f;

        //缓存起来
        m_FBXCacheResTable.Add(res.instanceID, res);
    }

    private static ResNode GetCacheFBXResource(ResConfigData cof)
    {
        ResNode res = null;
        int InstanceID = GUIDToInstanceID(cof.AssetGUID);
        if (m_FBXCacheResTable.TryGetValue(InstanceID, out res))
        {
            res.cacheTime = 0.0f;
            m_FBXCacheResTable.Remove(InstanceID);
        }

        return res;
    }

    private static void ClearFBXRes()
    {

        ClearResTable(ref m_FBXResTable);
        ClearResTable(ref m_FBXCacheResTable);
    }

    /// <summary>
    /// 更新缓存
    /// </summary>
    /// <param name="LoadCompleteList"></param>
    private static void UpdateFBXCache(List<int> LoadCompleteList, float dealtTime)
    {
        if (m_FBXCacheResTable.Count > 0)
        {
            LoadCompleteList.Clear();
            foreach (KeyValuePair<int, ResNode> r in m_FBXCacheResTable)
            {
                ResNode res = r.Value;
                res.cacheTime += dealtTime;

                //到了缓存时间，卸载资源
                if (res.cacheTime > AssetCacheTime)
                {
                    LoadCompleteList.Add(r.Key);
                    UnLoadResource(ref res);

                }
            }

            //移除已经加载完成的
            foreach (int s in LoadCompleteList)
            {
                m_FBXCacheResTable[s] = null;
                m_FBXCacheResTable.Remove(s);
            }
        }
    }
}
