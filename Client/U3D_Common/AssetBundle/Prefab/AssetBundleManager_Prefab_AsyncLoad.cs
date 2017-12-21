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
    /// 预制体GUID-他的请求列表
    /// </summary>
    static Dictionary<int, List<AsyncLoadingObject>> m_PrefabRequestTable = new Dictionary<int, List<AsyncLoadingObject>>();

    /// <summary>
    /// 预制体GUID-他的依赖资源
    /// </summary>
    static Dictionary<int, List<ResNode>> m_DependenicesRequestTable = new Dictionary<int, List<ResNode>>();

    ///// <summary>
    ///// 仅仅用于异步加载的时候
    ///// </summary>
    //static HashSet<int> m_NeedToDeletePrefab = new HashSet<int>();

    /// <summary>
    /// 预制体ResNode检测，因为预制体的ResNode是外部创建的
    /// 预制体需要等待依赖加载完成之后，才能去加载本身，这个时候才会去实际填充这个ResNode，
    /// 在这之前，ResNode只有一个GUID
    /// 这个在依赖加载完成后删除
    /// </summary>
    static Dictionary<int, ResNode> m_AsyncLoadingDependicensPrefabResNode = new Dictionary<int, ResNode>();

    /// <summary>
    /// 加载完成的依赖资源
    /// </summary>
    static List<int> AsyncDependenicesLoadCompleteList = new List<int>();

    /// <summary>
    /// 异步加载依赖资源中的无效结点
    /// </summary>
    static HashSet<int> AsyncDependenicesLoadInValidList = new HashSet<int>();


    /// <summary>
    /// 更新预制体的加载进度
    /// </summary>
    private static void UpdatePrefabAsyncLoadingProgress()
    {
        AsyncDependenicesLoadCompleteList.Clear();
        AsyncDependenicesLoadInValidList.Clear();
        foreach (KeyValuePair<int, List<ResNode>> pair in m_DependenicesRequestTable)
        {
            List<ResNode> nodList = pair.Value;
            bool isLoadFinish = true;
            bool isHaveInvalidNode = false;
            foreach(ResNode nod in nodList)
            {
                if(!nod.isLoadFinish)
                {
                    isLoadFinish = false;
                    break;
                }
                else
                {
                    if(!nod.IsValid())
                    {
                        isHaveInvalidNode = true;
                    }
                }
            }
            
            if(isLoadFinish)
            {
                AsyncDependenicesLoadCompleteList.Add(pair.Key);
                if(isHaveInvalidNode)
                {
                    AsyncDependenicesLoadInValidList.Add(pair.Key);
                }
            }
        }

        foreach(int s in AsyncDependenicesLoadCompleteList)
        {
            m_DependenicesRequestTable[s].Clear();
            m_DependenicesRequestTable[s] = null;
            m_DependenicesRequestTable.Remove(s);

            ////依赖加载完了之后，发现预制已经不需要了，直接卸载依赖
            //if (m_NeedToDeletePrefab.Contains(s))
            //{
            //    UnLoadDependenicesAsync(s);
            //    m_NeedToDeletePrefab.Remove(s);
            //    continue;
            //}

            List<AsyncLoadingObject> list = m_PrefabRequestTable[s];

            if (m_AsyncLoadingDependicensPrefabResNode.ContainsKey(s))
            {
                m_AsyncLoadingDependicensPrefabResNode.Remove(s);
            }

            //发现加载的依赖项有无效的资源节点，那么不进行下面的预制体本体加载
            //但是，还是要进行回调，因为这个是属于加载失败.
            if (AsyncDependenicesLoadInValidList.Contains(s))
            {             
                m_PrefabRequestTable.Remove(s);
                UnLoadDependenicesAsync(s);
            
                ResNode res = AllocResNode(list[0].loadingResInstanceID);
                m_PrefabResTable.Add(s, res);
                res.status = ResNodeStatus.LoadFinish;
                foreach (AsyncLoadingObject obj in list)
                {
                    obj.callBack(obj.configData,  res, obj.UserDataObj);
                }
                list.Clear();

                continue;
            }
            
            AsyncLoadingObject alo = list[0];
            ResNode loadingResNode = AllocResNode(alo.loadingResInstanceID);
            UnionResConfigData config = alo.configData as UnionResConfigData;
            GetPrefabResourceAsync(ref loadingResNode, config, PrefabResourceCallBack, config.priority);
            list[0] = alo;

        }
    }

     /// <summary>
    /// 计算资源回调，他们的res和cofig是一样的，只是userdataobj与回调不一样
    /// </summary>
    /// <param name="res"></param>
    /// <param name="config"></param>
    /// <param name="callBack"></param>
    /// <param name="userDataObj"></param>
    private static void GeneratePrefabCallBack(ResNode res, UnionResConfigData config, System.Action<ResConfigData, ResNode, System.Object> callBack, System.Object userDataObj)
    {
        List<AsyncLoadingObject> requestObjList = null;

        AsyncLoadingObject AnsycObj = BuildCallBackObj(res, config,callBack, userDataObj);

        if (m_PrefabRequestTable.TryGetValue(res.instanceID, out requestObjList))
        {
            requestObjList.Add(AnsycObj);
        }
        else
        {
            requestObjList = new List<AsyncLoadingObject>();
            requestObjList.Add(AnsycObj);
            m_PrefabRequestTable.Add(res.instanceID, requestObjList);
        }

    }

    private static void PrefabResourceCallBack(ResConfigData cof,ResNode nod,System.Object userDataObj)
    {
        List<AsyncLoadingObject> list = null;

        ////预制体依赖加载完了，预制体本身也加载完了，这时候发现，已经不需要这个预制体了
        //if (m_NeedToDeletePrefab.Contains(nod.instanceID))
        //{
        //    m_NeedToDeletePrefab.Remove(nod.instanceID);
        //    DeletePrefabResourceAsync(ref nod, PrefabResourceCallBack, true);
        //    return;
        //}



        //预制体加载失败的
        if (nod != null && !nod.IsValid())
        {
            UnLoadDependenicesAsync(nod.instanceID);
        }
        else
        {
            ReleaseDependenicesAssetBundleIfNeed(nod);
        }

        ReleaseAssetBundleIfNeed(nod);
        PushLoadedAssetInfo(nod, GetPrefabDependenciesResNode(nod.instanceID));
        if (m_PrefabRequestTable.TryGetValue(nod.instanceID, out list))
        {
            m_PrefabRequestTable.Remove(nod.instanceID);
            foreach (AsyncLoadingObject obj in list)
            {
                obj.callBack(cof, nod, obj.UserDataObj);
            }
        }


        
    }
}








