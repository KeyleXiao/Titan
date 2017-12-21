/*******************************************************************
** 文件名:	SkinManager.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/4/3
** 版  本:	1.0
** 描  述:	皮肤管理器
** 应  用:   用于管理皮肤的，简单的说，就是模型了
**************************** 修改记录 ******************************
** 修改人:  昔文博
** 日  期:  2016/1/19
** 描  述:  增加缓存皮肤函数
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;
using ASpeedGame.Data.PersonModelTransFormInfo;
using USpeedUI.UEffect;
using Effect;

/// <summary>
/// 用于管理皮肤的，简单的说，就是模型了
/// </summary>
public partial class SkinManager
{
    private class SkinAsyncLoadingObj
    {
        public int ReferenceCount = 0;
        public List<System.Action<Skin>> callbackList = new List<Action<Skin>>();
        public ResNode loadingResNode = null;
    }


    
    public static bool GetSkinAsync(int SkinID, System.Action<Skin> CallBack, AssetLoadPriority priority = AssetLoadPriority.Priority_Normal)
    {
        if(!currentActiveSkinList.Contains(SkinID))
        {
            currentActiveSkinList.Add(SkinID);
        }
        return GetSkinAsyncImpl(SkinID, CallBack, priority);
    }
    /// <summary>
    /// 异步取得Skin
    /// </summary>
    /// <param name="SkinID"></param>
    /// <param name="CallBack"></param>
    /// <returns></returns>
    private static bool GetSkinAsyncImpl(int SkinID, System.Action<Skin> CallBack, AssetLoadPriority priority = AssetLoadPriority.Priority_Normal)
    {
        SkinModelInfo info = null;
        if(!m_SkinConfigMap.TryGetValue(SkinID,out info))
        {
            Debug.LogWarning("找不到皮肤配置,ID:" + SkinID);
            return false;
        }
        Skin sk = null;
        SkinAsyncLoadingObj loadingObj = null;

        sk = GetSkinFromCache(SkinID);
        if (sk != null)
        {
            sk.AddRef();
            m_SkinMap.Add(sk.nSkinID, sk);

            if (m_AsyncLoadSkinRequestTable.TryGetValue(SkinID, out loadingObj))
            {
                loadingObj.callbackList.Add(CallBack);
            }
            else
            {
                loadingObj = new SkinAsyncLoadingObj();
                loadingObj.callbackList.Add(CallBack);
                m_AsyncLoadSkinRequestTable.Add(SkinID, loadingObj);
            }

            return true;
        }


        if(m_SkinMap.TryGetValue(SkinID,out sk))
        {
            if (m_AsyncLoadSkinRequestTable.TryGetValue(SkinID, out loadingObj))
            {
                sk.AddRef();
                loadingObj.callbackList.Add(CallBack);
            }
            else
            {
                loadingObj = new SkinAsyncLoadingObj();
                loadingObj.callbackList.Add(CallBack);
                sk.AddRef();
                m_AsyncLoadSkinRequestTable.Add(SkinID, loadingObj);
            }
            return true;
        }


        if (m_AsyncLoadSkinRequestTable.TryGetValue(SkinID, out loadingObj))
        {
            loadingObj.ReferenceCount++;
            loadingObj.callbackList.Add(CallBack);
        }
        else
        {
            ResNode obj = AssetBundleManager.GetAssetsAsync(AssetType.Asset_Prefab, ResLoadCallBack, info.strPatch, false, info, priority);
            if(obj != null)
            {
                loadingObj = new SkinAsyncLoadingObj();
                loadingObj.callbackList.Add(CallBack);
                loadingObj.ReferenceCount++;
                loadingObj.loadingResNode = obj;
                m_AsyncLoadSkinRequestTable.Add(SkinID, loadingObj);
            }
            else
            {
                Trace.Warning("加载皮肤模型失败,ID:" + info.nSkinID + ",路径:" + info.strPatch + ",请[--策划--]检查配置");
                return false;
            }
        }
        return true;
    }

    public static void DeleteSkinAsync(int SkinID, System.Action<Skin> CallBack, bool NotReleaseFromCache = false)
    {
        Skin sk = null;
        SkinAsyncLoadingObj obj = null;
        m_AsyncLoadSkinRequestTable.TryGetValue(SkinID, out obj);
        //首先看是不是已经加载出来了
        if (m_SkinMap.TryGetValue(SkinID, out sk))
        {
            DeleteSkinAsync(ref sk, CallBack, NotReleaseFromCache);
        }
        //没有加载出来
        else
        {
            if(null != obj)
            {
                int refCount = --obj.ReferenceCount;
                if(refCount <= 0)
                {
                    RemoveAllCallBack(SkinID);
                    AssetBundleManager.DeleteAssetsAsync(ref obj.loadingResNode, ResLoadCallBack, ImmdieateUnLoadSkin);
                }
                else
                {
                    RemoveCallBack(SkinID, CallBack);
                }
            }
        }
    }

    public static void DeleteSkinAsync(ref Skin sk, System.Action<Skin> CallBack, bool NotReleaseFromCache = false)
    {
        if (null == sk)
        {
            return;
        }
        SkinAsyncLoadingObj obj = null;
        int SkinID = sk.nSkinID;
        m_AsyncLoadSkinRequestTable.TryGetValue(SkinID, out obj);

        int refcount = sk.Release();
        if (refcount <= 0)
        {
            UnLoadSkin(ref sk, NotReleaseFromCache);
            RemoveAllCallBack(SkinID);
        }
        else
        {
            RemoveCallBack(SkinID, CallBack);

        }
    }

    static void RemoveAllCallBack(int nSkinID)
    {
        SkinAsyncLoadingObj obj = null;
        if (m_AsyncLoadSkinRequestTable.TryGetValue(nSkinID, out obj))
        {
            m_AsyncLoadSkinRequestTable.Remove(nSkinID);
        }
    }

    static void RemoveCallBack(int nSkinID, System.Action<Skin> CallBack)
    {
        SkinAsyncLoadingObj obj = null;
        if (m_AsyncLoadSkinRequestTable.TryGetValue(nSkinID, out obj))
        {
            for (int i = 0; i < obj.callbackList.Count; i++)
            {
                if (obj.callbackList[i] == CallBack)
                {
                    obj.callbackList.RemoveAt(i);
                    break;
                }
            }
        }
    }

    static void ResLoadCallBack(ResConfigData cof, ResNode res, System.Object UserDataObj)
    {
        SkinModelInfo info = UserDataObj as SkinModelInfo;
        if(null == info)
        {
            Debug.LogError("皮肤异步加载严重错误--ResLoadCallBack");
            return;
        }

        Skin sk = null;
        if (m_SkinMap.TryGetValue(info.nSkinID,out sk))
        {
            //已经有这个皮肤，不加载了,这种情况是被同步冲掉了
            if(sk.isValid)
            {
                return;
            }

        }

        //if(null == res || !res.IsValid())
        //{
        //    RemoveAllCallBack(info.nSkinID);
        //}
        BuildSkin(info, res,true);
    }

    private static void ClearAsyncLoadData()
    {
        m_AsyncLoadSkinRequestTable.Clear();
    }

    private static void ProcessSkinAsyncLoad()
    {
        UpdateLoadingSkin();
    }

    private static void UpdateLoadingSkin()
    {
        if (m_AsyncLoadSkinRequestTable.Count <= 0)
        {
            return;
        }
        LoadCompleteList.Clear();
        foreach (KeyValuePair<int, SkinAsyncLoadingObj> var in m_AsyncLoadSkinRequestTable)
        {
            Skin sk = null;
            if (!m_SkinMap.TryGetValue(var.Key, out sk))
            {
                continue;
            }
            LoadCompleteList.Add(var.Key);
        }

        foreach(int id in LoadCompleteList)
        {
            SkinAsyncLoadingObj var = null;
            m_AsyncLoadSkinRequestTable.TryGetValue(id, out var);
            m_AsyncLoadSkinRequestTable.Remove(id);

            Skin sk = null;
            m_SkinMap.TryGetValue(id, out sk);
            sk.AddRef(var.ReferenceCount);
            foreach (System.Action<Skin> callback in var.callbackList)
            {
                callback(sk);
            }
        }
    }

    private static List<int> LoadCompleteList = new List<int>();

    private static Dictionary<int, SkinAsyncLoadingObj> m_AsyncLoadSkinRequestTable = new Dictionary<int, SkinAsyncLoadingObj>();
}


