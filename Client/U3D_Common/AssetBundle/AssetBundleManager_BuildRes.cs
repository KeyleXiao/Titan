/*******************************************************************
** 文件名:	AssetBundleManager_BuildRes.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	ab资源管理器
** 应  用:  	ab资源管理器,负责资源构建部分

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;

public sealed partial class AssetBundleManager : MonoBehaviour
{
    #region 数据类

    public class PrefabDependeicesNode
    {
        private HashSet<int> depIDList = new HashSet<int>();
        private List<int> tempList = new List<int>(100);

        private bool isNoReference = false;
        public void PushDep(ICollection<int> dependenicesList)
        {
            if (isNoReference)
            {
                return;
            }

            if(depIDList.Count > 0)
            {
                depIDList.IntersectWith(dependenicesList);
                //如果求交集，是空的话，说明这个依赖节点是没有依赖资源需要加载
                if(depIDList.Count <= 0)
                {
                    isNoReference = true;
                }
            }
            else
            {
                depIDList.UnionWith(dependenicesList);
            }
        }

        public void RemoveDep(ICollection<int> dependenicesList)
        {
            foreach (int res in dependenicesList)
            {
                depIDList.Remove(res);
            }
        }


        public bool isLoadFinish()
        {
            if(depIDList.Count <= 0)
            {
                return true;
            }

            foreach (int id in depIDList)
            {
                ResNode curr = AllocResNode(id);
                if (curr.status != ResNodeStatus.Empty)
                {
                     if (curr.status != ResNodeStatus.LoadFinish)
                    {
                        return false;
                    }
                }
            }
            return true;
        }
    }

    /// <summary>
    /// 资源请求物体
    /// </summary>
    private class AsyncLoadingObject
    {
        public int loadingResInstanceID ;
        public ResConfigData configData;
        public System.Action<ResConfigData, ResNode, System.Object> callBack;
        public System.Object UserDataObj ;
    }

    /// <summary>
    /// 资源请求
    /// </summary>
    private class AssetRequest
    {
        public AssetBundleRequest Request = null;

        /// <summary>
        /// 是否已经上载到加载线程
        /// </summary>
        public bool isUpLoadToLoadingThread = false;

        public float curTimeOut = 0.0f;

        /// <summary>
        /// 超出这个时间则跳过这个ab，同步加载
        /// </summary>
        public const float TimeOut = 30.0f;

        public bool forceSynchronous = false;
        public UnityEngine.Object []forceSynchronous_assets = null;

        public void Clear()
        {
            Request = null;
            isUpLoadToLoadingThread = false;
            curTimeOut = 0.0f;
            forceSynchronous = false;
            forceSynchronous_assets = null;
        }
    }


    #endregion

    private static Type TransformType = typeof(Transform);
    private static Type SkinnedMeshRenderType = typeof(SkinnedMeshRenderer);
    private static Type MeshRenderType = typeof(MeshRenderer);
    private static Type MeshFilterType = typeof(MeshFilter);
    private static Type AnimatorType = typeof(Animator);
    private static Type AnimatonType = typeof(Animation);
                              
    #region 异步加载

    /// <summary>
    /// 正在等待卸载的异步加载的资源
    /// 存在在这个列表里的，是引用为0，并且是缓存时间超过了指定的时间，又没有加载完成的，等加载完成之后直接卸载掉
    /// [InstanceID]
    /// </summary>
    private static HashSet<int> m_NeedToUnLoadList = new HashSet<int>();

    /// <summary>
    /// 正在加载的ab资源请求,AssetGUID
    /// [InstanceID]
    /// </summary>
    private static List<int> m_LoadingAssetList = new List<int>();

    /// <summary>
    /// 资源请求的回调列表,InstanceID - 对应的回调列表 
    /// </summary>
    private static Dictionary<int, List<AsyncLoadingObject>> m_ResCallBackTable = new Dictionary<int, List<AsyncLoadingObject>>();

    /// <summary>
    /// 同时处理的最大异步加载时间片
    /// </summary>
    public static float MaxUpLoad_Assets_AsyncRequestClip = 0.0f;
    private static float usedUpLoad_Assets_AsyncRequestClip = 0.0f;

    /// <summary>
    /// 最大的上载时间，一次上载的时间片
    /// </summary>
    private static double MaxUpLoad_Assets_AsyncRequestTime = 0.0f;
    private static double usedUpLoad_Assets_AsyncRequestTime = 0.0f;
    private static double MinFrameCostTime = 0.045;

    /// <summary>
    /// 已经准备好异步加载的资源
    /// [InstanceID,ResourcePackageCallBackObj]
    /// </summary>
    private static Dictionary<int, ResourcePackageCallBackObj> m_ReadyForUpLoadAssetsAsyncRequestTable = new Dictionary<int, ResourcePackageCallBackObj>();

    /// <summary>
    /// 等待回调的，这些是异步加载的时候已经加载完成的，不能立刻回调，调用函数要早与回调函数返回
    /// </summary>
    private static Dictionary<ResNode, List<AsyncLoadingObject>> m_ResWaitingForCallBack = new Dictionary<ResNode, List<AsyncLoadingObject>>();


    //private static Dictionary<string, float> currLoadingResNodeList = new Dictionary<string, float>();
    static List<int> Temp_LoadCompleteList_int = new List<int>(100);
    static List<int> TempList_int = new List<int>(100);

    static List<string> Temp_LoadCompleteList_Str = new List<string>(100);
    static List<AsyncLoadingObject> Temp_LoadCompleteList_AsyncLoadingObject = new List<AsyncLoadingObject>(100);

    static List<System.Action<ResourcePackageCallBackObj>> Temp_resourcePackageCallBackList = new List<Action<ResourcePackageCallBackObj>>(20);
    static List<ResourcePackageCallBackObj> Temp_callBackDataList = new List<ResourcePackageCallBackObj>(20);

    /// <summary>
    /// 所有创建的ResNode结点
    /// [instanceID,ResNode]
    /// </summary>
    private static Dictionary<int, ResNode> allCreatedResNode = new Dictionary<int, ResNode>();

    /// <summary>
    /// 所有创建的ResNode结点
    /// [instanceID,AssetRequest]
    /// </summary>
    private static Dictionary<int, AssetRequest> allCreatedAssetsRequest = new Dictionary<int, AssetRequest>();

    /// <summary>
    /// GUID和实例ID表
    /// </summary>
    private static Dictionary<string, int> guid_InstanceIDTable = new Dictionary<string, int>();

    /// <summary>
    ///实例ID
    /// </summary>
    private static HashSet<int> ResNodeInstanceIDList = new HashSet<int>();

    static int currInstanceID = 1;

    /// <summary>
    /// 一个资源的依赖列表
    /// instanceID-List<PrefabDependeicesNode>
    /// </summary>
    private static Dictionary<int, PrefabDependeicesNode> dependenceIDTable = new Dictionary<int, PrefabDependeicesNode>();

    static int CanExclusiveModeLoadCout = -1;

    private static bool IsDependenceLoadFinish(int sourceInstanceID)
    {
        PrefabDependeicesNode depNode = null;
        if (dependenceIDTable.TryGetValue(sourceInstanceID, out depNode))
        {
            return depNode.isLoadFinish();
        }
        return true;
    }

    /// <summary>
    /// 多次push同一个，取他们的共有集合
    /// </summary>
    /// <param name="sourceInstanceID"></param>
    /// <param name="dependenicesList"></param>
    private static void PushDependence(int sourceInstanceID, ICollection<ResNode> dependenicesList)
    {
        TempList_int.Clear();
        foreach (ResNode res in dependenicesList)
        {
            if (null != res)
            {
               TempList_int.Add(res.instanceID);
            }
        }

        PushDependence(sourceInstanceID, TempList_int);
        TempList_int.Clear();
        
    }

    private static void PushDependence(int sourceInstanceID, ICollection<int> dependenicesList)
    {

        PrefabDependeicesNode depNode = null; ;
        ResNode res = AllocResNode(sourceInstanceID);
        if (!dependenceIDTable.TryGetValue(sourceInstanceID, out depNode))
        {
            depNode = new PrefabDependeicesNode();
            dependenceIDTable.Add(sourceInstanceID, depNode);

        }
        depNode.PushDep(dependenicesList);

    }

    private static void ClearResNodeTempData()
    {
        currInstanceID = 1;
        dependenceIDTable.Clear();
        guid_InstanceIDTable.Clear();
        allCreatedResNode.Clear();
        allCreatedAssetsRequest.Clear();
    }


    private static AssetRequest AllocAssetRequest(int InstanceID)
    {
        AssetRequest reslut = null;
        if (allCreatedAssetsRequest.TryGetValue(InstanceID, out reslut))
        {
            return reslut;
        }

        reslut = new AssetRequest();
        reslut.Clear();
        allCreatedAssetsRequest.Add(InstanceID, reslut);
        return reslut;
    }

    private static int GUIDToInstanceID(string guid)
    {
        int reslut = -1;
        if (!guid_InstanceIDTable.TryGetValue(guid, out reslut))
        {
            reslut = currInstanceID;
            guid_InstanceIDTable.Add(guid, currInstanceID);
            ResNodeInstanceIDList.Add(currInstanceID);
            currInstanceID++;
        }
        return reslut;
    }


    private static ResNode AllocResNode(int instanceID)
    {
        ResNode reslut = null;
        if (ResNodeInstanceIDList.Contains(instanceID))
        {
            if (!allCreatedResNode.TryGetValue(instanceID, out reslut))
            {
                reslut = new ResNode();
                reslut.Clear();
                reslut.instanceID = instanceID;
                allCreatedResNode.Add(instanceID, reslut);
            }
        }


        return reslut;

    }

    private static bool IsResNodeOnLoadStatus(int instanceID)
    {
        return IsResNodeOnLoadStatus(AllocResNode(instanceID));
    }

    private static bool IsResNodeOnLoadStatus(ResNode res)
    {
        if(res.status == ResNodeStatus.WatingForLoad || res.status == ResNodeStatus.Loading)
        {
            return true;
        }
        return false;
    }

    /// <summary>
    /// 避免重复创建ResNode
    /// </summary>
    /// <param name="guid"></param>
    /// <returns></returns>
    private static ResNode AllocResNode(string guid)
    {
        ResNode reslut = null;

        int instanceID = GUIDToInstanceID(guid);
        if(instanceID <= 0)
        {
            return reslut;
        }
        return AllocResNode(instanceID);
    }

    public void Update()
    {
        float dealtTime = Time.deltaTime * 2;
        if(Time.frameCount % 2 == 0)
        {
            ProcessNeedToUnLoadPackage(Temp_LoadCompleteList_int);

            ProcessNeedToUnLoadAsset(Temp_LoadCompleteList_int);

            //处理回调
            ProcessCallBack();

            //更新异步解压加密资源文件进度
            UpdateLoadingAssetFileProgress(Temp_LoadCompleteList_int);

            //处理异步请求
            ProcessAsyncRequest(dealtTime);

            //更新异步加载的资源加载进度
            UpdateLoadingAssetProgress(Temp_LoadCompleteList_int, dealtTime);

            UpdatePrefabAsyncLoadingProgress();
        }
        else
        {
            
            //GameObject更新
            UpdatePrefabCache(Temp_LoadCompleteList_int, dealtTime);

            //Controller缓存更新
            UpdateAnimatorControllerCache(Temp_LoadCompleteList_int, dealtTime);

            //AinationClip更新
            UpdateAnimationClipCache(Temp_LoadCompleteList_int, dealtTime);

            //FBX更新
            UpdateFBXCache(Temp_LoadCompleteList_int, dealtTime);

            //音频缓存更新
            UpdateAudioCache(Temp_LoadCompleteList_int, dealtTime);

            //材质缓存更新
            UpdateMaterialCache(Temp_LoadCompleteList_int, dealtTime);

            //Sprite缓存更新
            UpdateSpriteCache(Temp_LoadCompleteList_int, dealtTime);

            //纹理缓存更新
            UpdateTextureCache(Temp_LoadCompleteList_int, dealtTime);
        }


    }

    private static void EnQueueWaitForUpLoadAssetsAsyncRequestTable(Type t,ResourcePackageCallBackObj req)
    {
        req.assetType = t;
        //ResourcePackageCallBackObj temp = null;
        //if (m_ReadyForUpLoadAssetsAsyncRequestTable.TryGetValue(req.resInstanceID, out temp))
        //{
        //    m_ReadyForUpLoadAssetsAsyncRequestTable[temp.resInstanceID] = req;
        //}
        //else
        //{
            m_ReadyForUpLoadAssetsAsyncRequestTable.Add(req.resInstanceID, req);
        //}

    }

    /// <summary>
    /// 处理回调
    /// </summary>
    private static void ProcessCallBack()
    {
        ProcessResourcePackageCallBack();
        ProcessResCallBack();
    }

    private static void ProcessResCallBack()
    {
        if (m_ResWaitingForCallBack.Count <= 0)
        {
            return;
        }
        Temp_LoadCompleteList_AsyncLoadingObject.Clear();
        foreach (List<AsyncLoadingObject> objList in m_ResWaitingForCallBack.Values)
        {
            Temp_LoadCompleteList_AsyncLoadingObject.AddRange(objList);
        }
        m_ResWaitingForCallBack.Clear();

        foreach (AsyncLoadingObject obj in Temp_LoadCompleteList_AsyncLoadingObject)
        {
            ResNode loadingResNode = AllocResNode(obj.loadingResInstanceID);
            if (obj.callBack != null)
            {
                obj.callBack(obj.configData, loadingResNode, obj.UserDataObj);
            }
        }
    }

    /// <summary>
    /// 添加到延迟回调列表，这个是指调用异步加载，而已经加载完成的延迟回调，确保回调函数是慢于调用函数的
    /// </summary>
    private static void PushToResDelayCallBackList(ResNode res,ResConfigData config,System.Action<ResConfigData, ResNode, System.Object> UserCallBack, System.Object userDataObj)
    {
        if(null == res)
        {
            return;
        }
        List<AsyncLoadingObject> list = null;
        if(m_ResWaitingForCallBack.TryGetValue(res,out list))
        {
            list.Add(BuildCallBackObj(res, config,UserCallBack, userDataObj));
        }
        else
        {
            list = new List<AsyncLoadingObject>();
            list.Add(BuildCallBackObj(res, config,UserCallBack, userDataObj));
            m_ResWaitingForCallBack.Add(res,list);
        }
        
    }

    /// <summary>
    /// 移除延迟回调列表，这个是指调用异步加载，而已经加载完成的延迟回调，确保回调函数是慢于调用函数的
    /// </summary>
    private static void RemoveFromDelayCallBackList(ResNode res,System.Action<ResConfigData, ResNode, System.Object> UserCallBack)
    {
        if (null == res)
        {
            return;
        }
        List<AsyncLoadingObject> list = null;
        if (m_ResWaitingForCallBack.TryGetValue(res, out list))
        {
            for(int i = 0;i < list.Count;i++)
            {
                if(list[i].callBack == UserCallBack)
                {
                    list.RemoveAt(i);
                    break;
                }
            }
        }
    }

    private static void RemoveAllDelayCallBackList(ResNode res)
    {
        if (null == res)
        {
            return;
        }
        List<AsyncLoadingObject> list = null;
        if (m_ResWaitingForCallBack.TryGetValue(res, out list))
        {
            m_ResWaitingForCallBack.Remove(res);
        }
    }

    private static void RemoveFromResUpLoadQueue(int InstanceID)
    {
        if (m_ReadyForUpLoadAssetsAsyncRequestTable.ContainsKey(InstanceID))
        {
            m_ReadyForUpLoadAssetsAsyncRequestTable.Remove(InstanceID);
        }
    }

    private static bool RemoveAssetsAsyncProcessingList(ResNode res)
    {
        if(null == res)
        {
            return false;
        }

        usedUpLoad_Assets_AsyncRequestTime -= res.UpLoadCostTime;
        usedUpLoad_Assets_AsyncRequestClip -= res.UpLoadClipTime;
        res.UpLoadClipTime = 0;
        res.UpLoadCostTime = 0;

        return true;
    }

    /// <summary>
    /// 已经卸载的资源，但是还没创建出来，保存起来,等待加载完成再删除
    /// </summary>
    /// <param name="request"></param>
    private static void PushNeedToUnLoadAsset(ResNode res)
    {
        if(null == res)
        {
            return;
        }
        int assetInstnceID = res.instanceID;
        //如果这个ab，还有资源在加载
        if (IsResNodeOnLoadStatus(res))
        {
            res.status = ResNodeStatus.Empty;
            m_LoadingAssetList.Remove(assetInstnceID);
            RemoveFromResUpLoadQueue(assetInstnceID);
            RemoveFromResourcePackageCallBackList(res.AssetBundleName, res);
            
            AssetRequest req = AllocAssetRequest(assetInstnceID);
            //是否已经上载的加载线程
            if (!req.isUpLoadToLoadingThread)
            {
                RemoveAssetsAsyncProcessingList(res);
                ReleaseAssetBundle(res.AssetBundleName);
                res.Clear();
            }
            else
            {
                m_NeedToUnLoadList.Add(assetInstnceID);
            }
        }
    }

    /// <summary>
    /// 取得卸载列表中的资源
    /// </summary>
    /// <param name="InstanceID"></param>
    /// <returns></returns>
    private static ResNode GetResNodeFromNeedToUnLoadAsset(int InstanceID)
    {
        //如果这个ab，还有资源在加载
        if (m_NeedToUnLoadList.Contains(InstanceID))
        {
            ResNode curr = AllocResNode(InstanceID);
            curr.status = ResNodeStatus.Loading;
            m_LoadingAssetList.Add(InstanceID);
            m_NeedToUnLoadList.Remove(InstanceID);
            return curr;
        }
        return null;
    }


    private static void ProcessNeedToUnLoadAsset(List<int> LoadCompleteList)
    {
        if (m_NeedToUnLoadList.Count > 0)
        {
            LoadCompleteList.Clear();

            foreach (int ins in m_NeedToUnLoadList)
            {
                AssetRequest assetRequest = AllocAssetRequest( ins);
                ResNode res = AllocResNode(ins);
                //检查一遍请求列表，是否都加载完成了
                if (!assetRequest.Request.isDone)
                {
                    continue;
                }

                LoadCompleteList.Add(ins);

                if(res.assetType != AssetType.Asset_FBX)//对于FBX模型来说，直接释放ab,因为GameObject不能直接卸载
                {
                    UnLoadAssets(assetRequest.Request.allAssets, true);
                }
                    
                ReleaseAssetBundle(res.AssetBundleName);
                

            }

            //移除已经加载完成的
            foreach (int s in LoadCompleteList)
            {
                ResNode res = AllocResNode(s);
                RemoveAssetsAsyncProcessingList(res);
                m_NeedToUnLoadList.Remove(s);
                res.Clear();
            }
        }
    }

    /// <summary>
    /// 处理异步请求
    /// </summary>
    private static void ProcessAsyncRequest(float dealtTime)
    {
        ProcessAssetsFileAsyncRequest(dealtTime);
        ProcessAssetsAsyncRequest(dealtTime);
    }

    private static void ProcessAssetsAsyncRequest(float dealtTime)
    {
        if (isLoadingScene)
        {
            ProcessAssetsAsyncRequestWithLoadingScene();
            usedUpLoad_Assets_AsyncRequestClip = 0;
            usedUpLoad_Assets_AsyncRequestTime = 0.0f;
        }
        else
        {
            ProcessAssetsAsyncRequestWithOutLoadingScene(dealtTime);
        }
    }

    private static int AssetsAsyncRequestSortFunc(int left,int right)
    {
        ResNode req_left = AllocResNode(left);

        ResNode req_right = AllocResNode(right);
        if(null == req_left || null == req_right)
        {
            Debug.LogError("AssetsAsyncRequestSort failed,req_left:" + req_left + ",req_right:" + req_right);
            return 0;
        }

        if (null == req_left.config || null == req_right.config)
        {
            Debug.LogError("AssetsAsyncRequestSort failed,req_left.configData:" + req_left.config + ",status:" + req_left.status + ",req_right.configData:" + req_right.config + ",status:" +req_right.status );
            return 0;
        }

        int leftPro = (int)req_left.config.priority;
        int rightPro = (int)req_right.config.priority;
        return -leftPro.CompareTo(rightPro);
    }

    /// <summary>
    /// 加载场景时候的异步请求处理
    /// </summary>
    private static void ProcessAssetsAsyncRequestWithLoadingScene()
    {
        if (m_ReadyForUpLoadAssetsAsyncRequestTable.Count <= 0)
        {
            return;
        }

        Temp_LoadCompleteList_int.Clear();
        TempList_int.Clear();
        TempList_int.AddRange(m_ReadyForUpLoadAssetsAsyncRequestTable.Keys);
        foreach (int keys in TempList_int)
        {
            
            if (!IsDependenceLoadFinish(keys))
            {
                continue;
            }
            ResourcePackageCallBackObj req = null;
            //如果资源没有依赖，就可以继续上载
            m_ReadyForUpLoadAssetsAsyncRequestTable.TryGetValue(keys, out req);

            RemoveFromResUpLoadQueue(keys);
            ResNode curr = AllocResNode(keys);
            curr.status = ResNodeStatus.Loading;
            req.resInstanceID = keys;

            
            UpLoadRequest(req);
        }

    }

    private static void CaclUpLoadClipTime()
    {
        //保证能最少加载一个
        MaxUpLoad_Assets_AsyncRequestTime = (float)(MinFrameCostTime - Time.unscaledDeltaTime) + 0.001f;
    }


    /// <summary>
    /// 没有加载场景时候的异步请求处理
    /// </summary>
    private static void ProcessAssetsAsyncRequestWithOutLoadingScene(float dealtTime)
    {
        if ((usedUpLoad_Assets_AsyncRequestClip - 0.001f) >= MaxUpLoad_Assets_AsyncRequestClip)
        {
            return;
        }

        CaclUpLoadClipTime();

        
        if (usedUpLoad_Assets_AsyncRequestTime >= MaxUpLoad_Assets_AsyncRequestTime)
        {
            return;
        }


        if(m_LoadingAssetList.Count <= 0)
        {
            return;
        }
        if (m_ReadyForUpLoadAssetsAsyncRequestTable.Count <= 0)
        {
            return;
        }
        Temp_LoadCompleteList_int.Clear();
        m_LoadingAssetList.Sort(AssetsAsyncRequestSortFunc);
        ResourcePackageCallBackObj req = null;

        //按照优先级排序，第一个正在加载的是优先级最高的
        ResNode FirstLoadNode = AllocResNode(m_LoadingAssetList[0]);
        int currCanExclusiveModeLoadCout = 0;
       // usedUpLoad_Assets_AsyncRequestTime = 0.0f;
        foreach (int keys in m_ReadyForUpLoadAssetsAsyncRequestTable.Keys)
        {
            if (usedUpLoad_Assets_AsyncRequestTime >= MaxUpLoad_Assets_AsyncRequestTime)
            {
                //AssetBudleLog(StringHelper.BuildString("资源上载超过最大时间:" + usedUpLoad_Assets_AsyncRequestTime, ",ClipCount:", usedUpLoad_Assets_AsyncRequestClip));
                break;
            }

            //一次最多上传MinUpLoadAsyncRequestCount请求
            if ((usedUpLoad_Assets_AsyncRequestClip - 0.001f) >= MaxUpLoad_Assets_AsyncRequestClip)
            {
                break;
            }
            
            if(!IsDependenceLoadFinish(keys))
            { 
                continue;
            }

            //如果资源没有依赖，就可以继续上载
            m_ReadyForUpLoadAssetsAsyncRequestTable.TryGetValue(keys, out req);

            ResNode curr = AllocResNode(keys);
            ResourcePackage pakg = AllocResourcePackage(curr.AssetBundleName);

            if(null != FirstLoadNode.config)
            {            
                //如果第一个是独占优先级，那么只能其他优先级，只能加载一个。
                if (FirstLoadNode.config.priority == AssetLoadPriority.Priority_Exclusive)
                {
                    if (curr.config.priority != AssetLoadPriority.Priority_Exclusive)
                    {
                        if (currCanExclusiveModeLoadCout > CanExclusiveModeLoadCout)
                        {
                            continue;
                        }
                        currCanExclusiveModeLoadCout++;
                    }
                }
            }


            Temp_LoadCompleteList_int.Add(keys);
            //if (!m_LoadedResourcePackage.Contains(pakg.instanceID))
            //{
            //    continue;
            //}
            //if (!m_LoadingAssetList.Contains(keys))
            //{
            //    continue;
            //}
            
            req.resInstanceID = keys;
            float clip = GetUpLoadClipTime(req);

            usedUpLoad_Assets_AsyncRequestClip += clip;
            curr.UpLoadClipTime = clip;
            curr.status = ResNodeStatus.Loading;
            double currentTime = DateTime.Now.Ticks;
            if(UpLoadRequest(req))
            {
                currentTime = GameUtil.TicksToSecond(DateTime.Now.Ticks - currentTime);
                if (!isLoadingScene)
                {
                    curr.UpLoadCostTime = (float)currentTime;
                    usedUpLoad_Assets_AsyncRequestTime += currentTime;
                    //if(currentTime > 0.031f)
                    //{
                    //    AssetBudleLog(StringHelper.BuildString("资源上载时间过长:", currentTime.ToString("F4"), ",资源信息:", curr.AssetBundleName, ",", curr.AssetName, ",AssetX:", curr.config.AssetSize_X, ",AssetY:", curr.config.AssetSize_Y));
                    //}
                }
            }
        }

        foreach (int id in Temp_LoadCompleteList_int)
        {
            RemoveFromResUpLoadQueue(id);
        }
    }

    /// <summary>
    /// 是否需要占据一整个异步加载片段
    /// </summary>
    /// <param name="req"></param>
    /// <returns></returns>
    private static float GetUpLoadClipTime(ResourcePackageCallBackObj req)
    {
        float Reslut = 0;
        ResNode curr = AllocResNode(req.resInstanceID);
        if (null == curr)
        {
            Debug.LogError("资源异步加载重大bug -- GetUpLoadClipTime-> req.resNode");
            return 0;
        }
        if (IsUnionDependenicesAssetBundle(curr.AssetBundleName))
        {
            Reslut = MaxUpLoad_Assets_AsyncRequestClip;
        }
        else if (req.assetType == TextureType)
        {
            Reslut = GetTextureUpLoadClipTime(curr);
        }
        else if (req.assetType == AudioType)
        {
            Reslut = GetAudioUpLoadClipTime(curr);
        }
        else if (req.assetType == SpriteType)//sprite归到贴图里
        {
            Reslut = GetTextureUpLoadClipTime(curr);
        }
        else if (req.assetType == AnimatorControllerType)
        {
            Reslut = GetAnimatorControllerUpLoadClipTime(curr);
        }
        else if (req.assetType == AnimationClipType)
        {
            Reslut = GetAnimationClipUpLoadClipTime(curr);
        }
        else if (req.assetType == MaterialType)
        {
            Reslut = GetMaterialUpLoadClipTime(curr);
        }
        else if (req.assetType == FBXType)
        {
            Reslut = GetFBXUpLoadClipTime(curr);
        }
        else if (req.assetType == PrefabType)
        {
            Reslut = GetPrefabUpLoadClipTime(curr);
        }
        else
        {
            Reslut = MaxUpLoad_Assets_AsyncRequestClip;
        }
        return Reslut;
    }

    /// <summary>
    /// 上载请求
    /// </summary>
    /// <param name="req"></param>
    private static bool UpLoadRequest(ResourcePackageCallBackObj req)
    {
        if (req.assetType == TextureType)
        {
            return UpLoadTextureAsyncRequest(req);
        }
        else if (req.assetType == AudioType)
        {
            return UpLoadAudioAsyncRequest(req);
        }
        else if (req.assetType == SpriteType)
        {
            return UpLoadSpriteAsyncRequest(req);
        }
        else if (req.assetType == AnimatorControllerType)
        {
            return UpLoadAnimatorControllerAsyncRequest(req);
        }
        else if (req.assetType == AnimationClipType)
        {
            return UpLoadAnimationClipAsyncRequest(req);
        }
        else if (req.assetType == MaterialType)
        {
            return UpLoadMaterialAsyncRequest(req);
        }
        else if (req.assetType == FBXType)
        {
            return UpLoadFBXAsyncRequest(req);
        }
        else if (req.assetType == PrefabType)
        {
            return UpLoadPrefabAsyncRequest(req);
        }
        else
        {
            Debug.LogWarning("不支持的异步类型:" + req.assetType);
            return false;
        }
    }
    
    private static void UpLoadRequestFail(int InstanceID)
    {
        m_LoadingAssetList.Remove(InstanceID);
        List<AsyncLoadingObject> RequestList = null;
        ResNode loadingResNode = AllocResNode(InstanceID);
        loadingResNode.status = ResNodeStatus.LoadFinish;
        if (m_ResCallBackTable.TryGetValue(InstanceID, out RequestList))
        {
            m_ResCallBackTable.Remove(InstanceID);
            foreach (AsyncLoadingObject obj in RequestList)
            {
                if (obj.callBack != null)
                {
                    obj.callBack(obj.configData, loadingResNode, obj.UserDataObj);
                }
            }
        }
        RemoveAssetsAsyncProcessingList(loadingResNode);
    }

    /// <summary>
    /// 更新异步加载资源的加载进度
    /// </summary>
    /// <param name="LoadCompleteList"></param>
    private static void UpdateLoadingAssetProgress(List<int> LoadCompleteList,float delatTime)
    {
        if (m_LoadingAssetList.Count > 0)
        {
            LoadCompleteList.Clear();

            foreach (int  ins in m_LoadingAssetList)
            {
                AssetRequest assetRequest = AllocAssetRequest( ins);
                
                //如果资源还没上载的加载线程
                if (!assetRequest.isUpLoadToLoadingThread)
                {
                    continue;
                }

                if (assetRequest.forceSynchronous || assetRequest.Request.isDone)
                {
                    LoadCompleteList.Add(ins);
                    ResNode loadingResNode = AllocResNode(ins);
                    loadingResNode.status = ResNodeStatus.LoadFinish;
                    loadingResNode.FillAssets(assetRequest.forceSynchronous ? assetRequest.forceSynchronous_assets : assetRequest.Request.allAssets);
                }
            }
                   

            //移除已经加载完成的
            foreach (int s in LoadCompleteList)
            {
                //加载完一个异步，从正在处理中移除
                if (!RemoveAssetsAsyncProcessingList(AllocResNode(s)))
                {
                    Debug.LogError("AssetBundle重大错误1111!");
                }

                m_LoadingAssetList.Remove(s);
                ResNode loadingResNode = AllocResNode(s);
                //如果资源请求列表里有这个，直接进行回调
                if (m_ResCallBackTable.ContainsKey(s))
                {
                    List<AsyncLoadingObject> RequestList = m_ResCallBackTable[s];
                    m_ResCallBackTable.Remove(s);

                    foreach (AsyncLoadingObject obj in RequestList)
                    {
                        if (obj.callBack != null)
                        {
                            obj.callBack(obj.configData, loadingResNode, obj.UserDataObj);
                        }
                    }
                    RequestList.Clear();
                }
            }
        }
    }




    /// <summary>
    /// 计算资源回调，他们的res和cofig是一样的，只是userdataobj与回调不一样
    /// </summary>
    /// <param name="res"></param>
    /// <param name="config"></param>
    /// <param name="callBack"></param>
    /// <param name="userDataObj"></param>
    private static void GenerateResCallBack(ResNode res, ResConfigData config, System.Action<ResConfigData, ResNode, System.Object> callBack, System.Object userDataObj)
    {
        List<AsyncLoadingObject> requestObjList = null;

        AsyncLoadingObject AnsycObj = BuildCallBackObj(res, config,callBack, userDataObj);

        if (m_ResCallBackTable.TryGetValue(res.instanceID, out requestObjList))
        {
            requestObjList.Add(AnsycObj);
        }
        else
        {
            requestObjList = new List<AsyncLoadingObject>();
            requestObjList.Add(AnsycObj);
            m_ResCallBackTable.Add(res.instanceID, requestObjList);
        }

    }

    /// <summary>
    /// 移除资源回调
    /// </summary>
    /// <param name="res"></param>
    /// <param name="callBack"></param>
    private static void RemoveResCallBack(Dictionary<int,List<AsyncLoadingObject>> requestTable,ResNode res, System.Action<ResConfigData, ResNode, System.Object> callBack)
    {
        //如果这个资源还在请求列表里
        if (requestTable.ContainsKey(res.instanceID))
        {

            //移除掉对应的请求
            List<AsyncLoadingObject> RequestList = requestTable[res.instanceID];
            for (int i = 0; i < RequestList.Count; i++)
            {
               
                AsyncLoadingObject obj = RequestList[i];
                ResNode loadingResNode = AllocResNode(obj.loadingResInstanceID);
                if (loadingResNode == res &&
                    obj.callBack == callBack)
                {
                    RequestList.RemoveAt(i);
                    break;
                }
            }
        }
    }

    /// <summary>
    /// 移除资源回调
    /// </summary>
    /// <param name="res"></param>
    /// <param name="callBack"></param>
    private static void RemoveAllResCallBack(Dictionary<int,List<AsyncLoadingObject>> requestTable,ResNode res)
    {
        if (null == requestTable)
        {
            return;
        }

        //如果这个资源还在请求列表里
        if (requestTable.ContainsKey(res.instanceID))
        {
            //移除掉对应的请求
            List<AsyncLoadingObject> RequestList = requestTable[res.instanceID];
            RequestList.Clear();
            requestTable.Remove(res.instanceID);
        }
    }

    /// <summary>
    /// 构建回调物体
    /// </summary>
    /// <param name="res"></param>
    /// <param name="config"></param>
    /// <param name="callBack"></param>
    /// <param name="userDataObj"></param>
    /// <returns></returns>
    private static AsyncLoadingObject BuildCallBackObj(ResNode res,ResConfigData data,System.Action<ResConfigData, ResNode, System.Object> callBack, System.Object userDataObj)
    {
        AsyncLoadingObject AnsycObj = new AsyncLoadingObject();
        AnsycObj.loadingResInstanceID = res.instanceID;
        AnsycObj.UserDataObj = userDataObj;
        AnsycObj.callBack = callBack;
        AnsycObj.configData = data;
        return AnsycObj;
    }


    private static bool UpLoadAsyncRequest<T>(ResourcePackageCallBackObj req, bool forceSynchronous = false) where T : UnityEngine.Object
    {
        if(forceSynchronous || isLoadingScene)
        {
            return UpLoadAsyncRequest_Synchronous<T>(req);
        }
        else
        {
            return UpLoadAsyncRequest_Normal<T>(req);
        }
    }

    private static bool UpLoadAsyncRequest_Synchronous<T>(ResourcePackageCallBackObj req) where T : UnityEngine.Object
    {
        ResourcePackage pkg = AllocResourcePackage(req.packageID);
        ResNode curr = AllocResNode(req.resInstanceID);
        if (null != pkg)
        {
            if (!pkg.AssetBundleInstance)
            {
                AssetBudleLog("AssetBundle读取失败:" + curr.AssetBundleName);
                UpLoadRequestFail(curr.instanceID);
                return false;
            }
            if (typeof(T) == FBXType)
            {
                ResNode res = curr;

                UnityEngine.Object []AllAssets = null;
                if (IsUnionDependenicesAssetBundle(res.AssetBundleName))
                {
                    AllAssets = pkg.AssetBundleInstance.LoadAllAssets<GameObject>();
                }
                else
                {
                    AllAssets = new UnityEngine.Object[] { pkg.AssetBundleInstance.LoadAsset<GameObject>(res.AssetName) };
                }
                //先判断isdone是否true，然后在判断有没有资源
                if (AllAssets.Length > 0 && AllAssets[0])
                {
                    AssetRequest reques = AllocAssetRequest(res.instanceID);
                    reques.isUpLoadToLoadingThread = true;
                    reques.forceSynchronous = true;
                    reques.forceSynchronous_assets = AllAssets;
                }
                else
                {
                    UpLoadRequestFail(res.instanceID);
                    ReleaseResourcePackageWhenLoadFiald(pkg);
                    AssetBudleLog("在AssetBundle:" + res.AssetBundleName + ",中找不到资源:" + res.AssetName + "--SynchronousFBX");
                    return false;
                }
            }
            else
            {
                ResNode res = curr;

                UnityEngine.Object[] AllAssets = null;
                if (IsUnionDependenicesAssetBundle(res.AssetBundleName))
                {
                    AllAssets = pkg.AssetBundleInstance.LoadAllAssets<T>();
                }
                else
                {
                    AllAssets = new UnityEngine.Object[] { pkg.AssetBundleInstance.LoadAsset<T>(res.AssetName) };
                }
                //先判断isdone是否true，然后在判断有没有资源
                if (AllAssets.Length > 0 && AllAssets[0])
                {
                    AssetRequest reques = AllocAssetRequest(res.instanceID);
                    reques.isUpLoadToLoadingThread = true;
                    reques.forceSynchronous = true;
                    reques.forceSynchronous_assets = AllAssets;
                }
                else
                {
                    UpLoadRequestFail(res.instanceID);
                    ReleaseResourcePackageWhenLoadFiald(pkg);
                    AssetBudleLog("在AssetBundle:" + res.AssetBundleName + ",中找不到资源:" + res.AssetName + "--Synchronous");
                    return false;
                }
            }
        }
        else
        {
            UpLoadRequestFail(req.resInstanceID);
            ReleaseResourcePackageWhenLoadFiald(pkg);
            AssetBudleLog("加载AssetBundle失败:" + curr.AssetBundleName);
            return false;
        }

        return true;
    }

    private static bool UpLoadAsyncRequest_Normal<T>(ResourcePackageCallBackObj req) where T : UnityEngine.Object
    {
        ResourcePackage pkg = AllocResourcePackage(req.packageID);
        ResNode curr = AllocResNode(req.resInstanceID);
        if (null != pkg)
        {
            if (!pkg.AssetBundleInstance)
            {
                AssetBudleLog("AssetBundle读取失败:" + curr.AssetBundleName);
                UpLoadRequestFail(curr.instanceID);
                return false;
            }
            if (typeof(T) == FBXType)
            {
                ResNode res = curr;


                AssetBundleRequest RequestFBX = null;
                if (IsUnionDependenicesAssetBundle(res.AssetBundleName))
                {
                    RequestFBX = pkg.AssetBundleInstance.LoadAllAssetsAsync<GameObject>();
                }
                else
                {
                    RequestFBX = pkg.AssetBundleInstance.LoadAssetAsync<GameObject>(res.AssetName);
                }
                //先判断isdone是否true，然后在判断有没有资源
                if (!(RequestFBX.isDone && RequestFBX.asset == null))
                {
                    AssetRequest reques = AllocAssetRequest(res.instanceID);
                    reques.isUpLoadToLoadingThread = true;
                    reques.Request = RequestFBX;
                    reques.forceSynchronous = false;
                    reques.forceSynchronous_assets = null;

                }
                else
                {
                    UpLoadRequestFail(res.instanceID);
                    ReleaseResourcePackageWhenLoadFiald(pkg);
                    AssetBudleLog("在AssetBundle:" + res.AssetBundleName + ",中找不到资源:" + res.AssetName + "--NormalFBX");
                    return false;
                }
            }
            else
            {
                ResNode res = curr;
                AssetBundleRequest Request = null;


                if (IsUnionDependenicesAssetBundle(res.AssetBundleName))
                {
                    Request = pkg.AssetBundleInstance.LoadAllAssetsAsync<T>();
                }
                else
                {
                    Request = pkg.AssetBundleInstance.LoadAssetAsync<T>(res.AssetName);
                }

                //先判断isdone是否true，然后在判断有没有资源
                if (!(Request.isDone && Request.asset == null))
                {
                    AssetRequest reques = AllocAssetRequest(res.instanceID);
                    reques.isUpLoadToLoadingThread = true;
                    reques.Request = Request;
                    reques.forceSynchronous = false;

                }
                else
                {
                    UpLoadRequestFail(res.instanceID);
                    ReleaseResourcePackageWhenLoadFiald(pkg);
                    AssetBudleLog("在AssetBundle:" + res.AssetBundleName + ",中找不到资源:" + res.AssetName + "--Normal");
                    return false;
                }
            }
        }
        else
        {
            UpLoadRequestFail(req.resInstanceID);
            ReleaseResourcePackageWhenLoadFiald(pkg);
            AssetBudleLog("加载AssetBundle失败:" + curr.AssetBundleName);
            return false;
        }
        return true;
    }
    #endregion

    #region 同步加载

    /// <summary>
    /// 构建资源结点，当他在异步加载中的时候
    /// </summary>
    /// <param name="res"></param>
    private static void BuildResNodeWhenInAsyncLoading<T>(ref ResNode res, ResConfigData config) where T : UnityEngine.Object
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

        GetResNodeFromNeedToUnLoadAsset(res.instanceID);
        RemoveFromResUpLoadQueue(res.instanceID);
        //先从队列中移除这个资源结点
        RemoveAssetsAsyncProcessingList(res);



        bool needToLoadFromPackage = true;
        //先看看是不是ab文件已经加载完了
        if (IsResNodeOnLoadStatus(res))
        {
            //移除他
            m_LoadingAssetList.Remove(res.instanceID);

            AssetRequest arq = AllocAssetRequest(res.instanceID);
            //然后再看看是否已经上载的加载线程
            if (arq.isUpLoadToLoadingThread)
            {
                res.FillAssets(arq.Request.allAssets);
                needToLoadFromPackage = false;
            }
        }

        if(needToLoadFromPackage)
        {
            //先检查，这个资源包是否加载完成了。
            ResourcePackage pkg = GetResourcePackge(config);
            RemoveFromResourcePackageCallBackList(res.AssetBundleName, res);
            if (pkg.AssetBundleInstance)
            {
                LoadAssetFromAssetBundle<T>(ref res, config, pkg.AssetBundleInstance);
            }
        }

        //最后将这个资源结点弄成加载完成
        res.status = ResNodeStatus.LoadFinish;
        //取出他的回调信息,加入到延迟回调列表里
        List<AsyncLoadingObject> RequestList = null;
        if (m_ResCallBackTable.TryGetValue(res.instanceID, out RequestList))
        {
            m_ResCallBackTable.Remove(res.instanceID);
            
            foreach (AsyncLoadingObject obj in RequestList)
            {
                PushToResDelayCallBackList(res, obj.configData, obj.callBack, obj.UserDataObj);
            }
        }


    }

    private static void BuildResNodeOnEditor<T>(ref ResNode reslut,ResConfigData config) where T : UnityEngine.Object
    {
        if (LoadAssetOnEditorInstance != null)
        {
            reslut = AllocResNode(config.AssetGUID);
            reslut.StartLoadedTime();
            reslut.Clear();
            if (LoadAssetFromEditor<T>(ref reslut, config))
            {
                reslut.config = config; 
                reslut.isAsync = false;
                reslut.AddRef();
            }
            else
            {
                reslut = null;
            }

        }
        return ;
    }
   
    private static bool LoadAssetFromEditor<T>(ref ResNode reslut, ResConfigData config) where T : UnityEngine.Object
    {
        if (null == reslut)
        {
            return false;
        }

        UnityEngine.Object[] clip = LoadAssetOnEditorInstance(config, typeof(T));
        if (clip.Length > 0 && clip[0])
        {
            reslut.status = ResNodeStatus.LoadFinish;
            reslut.FillAssets(clip);
            
            clip = null;
            return true;
        }
        else
        {
            AssetBudleLog(StringHelper.BuildString("资源数据库中找不到对应的资源:" + config.AssetName, ",请重新导入资源"));
        }
        return false;
    }

    private static bool LoadAssetFromAssetBundle<T>(ref ResNode reslut, ResConfigData config, AssetBundle instance) where T : UnityEngine.Object
    {
        if (null == reslut)
        {
            return false;
        }

        if(!instance)
        {
            return false;
        }
        List<UnityEngine.Object> objList = new List<UnityEngine.Object>();

        if (typeof(T) != FBXType)
        {
            if(IsUnionDependenicesAssetBundle(config.AssetBundleName))
            {
                objList.AddRange(instance.LoadAllAssets());
            }
            else
            {
                objList.Add(instance.LoadAsset<T>(config.AssetName));
            }

            if (objList.Count > 0 && objList[0])
            {
                reslut.status = ResNodeStatus.LoadFinish;
                reslut.FillAssets(objList);
                return true;
            }
            else
            {
                AssetBudleLog("在AssetBundle:" + config.AssetBundleName + ",中找不到资源:" + config.AssetName + "--LoadAssetFromAssetBundle-NotFBX");
            }
        }
        else
        {

            //objList.AddRange(pkg.AssetBundleInstance.LoadAllAssets<Mesh>());
            //objList.AddRange(pkg.AssetBundleInstance.LoadAllAssets<AnimationClip>());
            //objList.AddRange(pkg.AssetBundleInstance.LoadAllAssets<Avatar>());

            if(IsUnionDependenicesAssetBundle(config.AssetBundleName))
            {
                objList.AddRange(instance.LoadAllAssets());
            }
            else
            {
                objList.Add(instance.LoadAsset<GameObject>(config.AssetName));
            }
            if (objList.Count > 0 && objList[0])
            {
                reslut.status = ResNodeStatus.LoadFinish;
                reslut.FillAssets(objList);
                return true;
            }
            else
            {
                AssetBudleLog("在AssetBundle:" + config.AssetBundleName + ",中找不到资源:" + config.AssetName + "--LoadAssetFromAssetBundle-FBX");
            }
        }

        return false;
    }

    private static void BuildResNodeOnRunTime<T>(ref ResNode reslut, ResConfigData config) where T : UnityEngine.Object
    {
        ResourcePackage pkg = null;
        reslut = AllocResNode(config.AssetGUID);

        pkg = GetResourcePackge(config);

        if (null != pkg)
        {
            reslut.Clear();
            reslut.StartLoadedTime();
            if (LoadAssetFromAssetBundle<T>(ref reslut, config, pkg.AssetBundleInstance))
            {
                reslut.config = config;
                reslut.isAsync = false;
                reslut.AddRef();
            }
            else
            {
                reslut = null;
                ReleaseResourcePackageWhenLoadFiald(pkg);
            }

        }
        else
        {
            reslut = null;
            ReleaseResourcePackageWhenLoadFiald(pkg);
            AssetBudleLog("加载AssetBundle失败:" + config.AssetBundleName);
        }
    }
    #endregion


    /// <summary>
    /// 卸载assets
    /// </summary>
    /// <param name="assetToUnLoad"></param>
    private static void UnLoadAssets(ResNode res)
    {
        if(res.Cleared)
        {
            return;
        }
        bool neeToForceDelete = false;
        if (res.assetType == AssetType.Asset_Prefab)
        {
            neeToForceDelete = !res.bSavedMemory;
            neeToForceDelete = neeToForceDelete && isOnRunTimeMode;
        }
        UnLoadAssets(res.AllAssets, neeToForceDelete);
    }

    /// <summary>
    /// 卸载assets
    /// </summary>
    /// <param name="assets"></param>
    /// <param name="MustDelete">必须要删除</param>
    private static void UnLoadAssets(UnityEngine.Object []assets, bool MustDelete = false)
    {
        if (null == assets || assets.Length <= 0)
        {
            return;
        }
        //编辑器中不释放资源，防止编辑器崩溃,有被编辑器引用的地方，强制卸载的话，会导致编辑器崩溃
        if (!isOnRunTimeMode)
        {
            assets = null;
            return;
        }
        foreach (UnityEngine.Object o in assets)
        {

            if (IsAsset(o))
            {
                Resources.UnloadAsset(o);
            }
            else
            {
                /*
                 * 不需要去释放GameObject的资源，GameObjec的资源有两种类型
                 * 1.是FBX文件，FBX文件是放在assetbundle里的，直接unload(true)即可,异步加载的则通过MustDelete来控制删除
                 * 2.是Prefab文件，prefab文件不会保存资源，只会保存实例，而实例是在ResNode.Clear中删除
                 */
                if (MustDelete)
                {
                    GameObject.DestroyImmediate(o, true);
                }
            }
        }
    }

    /// <summary>
    /// 卸载assets
    /// </summary>
    /// <param name="assets"></param>
    /// <param name="MustDelete">必须要删除</param>
    private static void UnLoadAssets(List<UnityEngine.Object> assets,bool MustDelete = false)
    {
        if (null == assets || assets.Count <= 0)
        {
            return;
        }
        //编辑器中不释放资源，防止编辑器崩溃,有被编辑器引用的地方，强制卸载的话，会导致编辑器崩溃
        if (!isOnRunTimeMode)
        {
            assets = null;
            return;
        }
        foreach (UnityEngine.Object o in assets)
        {

            if (IsAsset(o))
            {
                Resources.UnloadAsset(o);
            }
            else
            {
                /*
                 * 不需要去释放GameObject的资源，GameObjec的资源有两种类型
                 * 1.是FBX文件，FBX文件是放在assetbundle里的，直接unload(true)即可,异步加载的则通过MustDelete来控制删除
                 * 2.是Prefab文件，prefab文件不会保存资源，只会保存实例，而实例是在ResNode.Clear中删除
                 */
                if (MustDelete)
                {
                    GameObject.DestroyImmediate(o, true);
                }
            }
        }
        assets.Clear();
    }


    /// <summary>
    /// 将资源从内存中卸载
    /// </summary>
    /// <param name="res"></param>
    /// <param name="isGameObject">是不是GameObject,像预制体，FBX这种是GameObject的,Resoures不能卸载</param>
    private static void UnLoadResource(ref ResNode res)
    {
        //这个资源是异步加载的
        if (res.isAsync)
        {
            //资源加载完成，直接卸载
            if (res.isLoadFinish)
            {
                UnLoadAssets(res);
                ReleaseAssetBundle(res.AssetBundleName);
                res.Clear();
            }
            else
            {
                //没有加载完成，压入等待卸载
                PushNeedToUnLoadAsset(res);
            }
        }
        else
        {
            UnLoadAssets(res);
            ReleaseAssetBundle(res.AssetBundleName);
            res.Clear();
        }
       
        res = null;
    }
    

    private static bool IsAsset(UnityEngine.Object asset)
    {
        if (!asset || asset.GetType() == GameObjectType )
        {
            return false;
        }
        return true;
    }

    private static bool CheckAssetPatch(string assetBundleName,string assetName)
    {
        if (isOnRunTimeMode)
        {
            string fullPatch = AssetBundleManager.FullAssetPackagePathRoot + assetBundleName;
            return System.IO.File.Exists(fullPatch);
        }
        else
        {
            return CheckAssetPatchOnEditorInstance(assetBundleName, assetName);
        }
    }
}
