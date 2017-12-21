/*******************************************************************
** 文件名:	AssetBundleManager_AssetFile.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	ab资源管理器
** 应  用:  	ab资源管理器,资源文件部分

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
using LZ4ps;
using System.Runtime.InteropServices;

public sealed partial class AssetBundleManager : MonoBehaviour
{
    public static bool enabledAsyncLoadAssetFile = true;

    /// <summary>
    /// 是否需要强制解密
    /// </summary>
    public static bool ForceOpenDeCode = false;

    public class ResourcePackageCallBackObj
    {
        public int resInstanceID = -1;
        public int packageID = -1;
        public System.Action<ResConfigData, ResNode, System.Object> userCallBck = null;
        public System.Object userDataObj = null;
        public ResConfigData configData = null;
        public Type assetType = null;

        public void Clear()
        {
            userCallBck = null;
            userDataObj = null;
            assetType = null;
            configData = null;
        }
    }

    /// <summary>
    /// 资源包
    /// </summary>
    public class ResourcePackage
    {
        public bool isAsync = false;

        public string filePatch = string.Empty;

        public bool isDone
        {
            get
            {
                if(!isAsync)
                {
                    return true;
                }
                if (fileRequest == null)
                {
                    return true;
                }

                return fileRequest.isDone;
            }
        }

        public bool isUpLoadToLoadingThread = false;

        public AssetBundleCreateRequest fileRequest = null;

        public AssetLoadPriority priority
        {
            get
            {
                return m_Priority;
            }
            set
            {
                int val = (int)m_Priority;
                int newVal = (int)value;
                if (val < newVal)
                {
                    m_Priority = value;
                }
            }
        }
        private AssetLoadPriority m_Priority = AssetLoadPriority.Priority_Normal;

        #region 数据区域

        public int unionIndx = -1;
        public int startIndex = -1;
        public int dataLength = -1;

        
        public int instanceID = 0;
        public AssetBundle AssetBundleInstance;

        public List<System.Action<ResourcePackageCallBackObj>> resourcePackageCallBackList
        {
            get
            {
                return m_resourcePackageCallBackList;

            }
        }
        public List<ResourcePackageCallBackObj> callBackDataList
        {
            get
            {
                return m_callBackDataList;

            }
        }

        private List<System.Action<ResourcePackageCallBackObj>> m_resourcePackageCallBackList = new List<Action<ResourcePackageCallBackObj>>(20);
        private List<ResourcePackageCallBackObj> m_callBackDataList = new List<ResourcePackageCallBackObj>(20);
        private int RefCount = 0;
        #endregion

        public void ResetPriority()
        {
            m_Priority = AssetLoadPriority.Priority_Normal;
        }

        public void ClearAsyncCallBackData()
        {
            m_resourcePackageCallBackList.Clear();
            m_callBackDataList.Clear();
        }

        public void AddAsyncCallBackData(System.Action<ResourcePackageCallBackObj> resourceCallBack,ResourcePackageCallBackObj dataObj)
        {
            m_resourcePackageCallBackList.Add(resourceCallBack);
            m_callBackDataList.Add(dataObj);
        }

        public void Clear()
        {
            ClearAsyncCallBackData();
            AssetBundleInstance = null;
            RefCount = 0;
            fileRequest = null;
            isUpLoadToLoadingThread = false;
            filePatch = string.Empty;
            isAsync = false;
            ResetPriority();
        }



        public int Release()
        {
            RefCount--;
            if(RefCount < 0)
            {
                RefCount = 0;
            }

            return RefCount;
        }
        public int AddRef()
        {
            RefCount++;
            return RefCount;
        }
      
    }

    /// <summary>
    /// 已经加载的资源包，资源包名-ResourcePackage
    /// </summary>
    private static HashSet<int> m_LoadedResourcePackage = new HashSet<int>();

    private static Dictionary<int, ResourcePackage> allCreatedResourcePackage = new Dictionary<int, ResourcePackage>();

    private static Dictionary<int, ResourcePackageCallBackObj> allCreatedResourcePackageCallBackObj = new Dictionary<int, ResourcePackageCallBackObj>();

    private static Dictionary<string, int> assetBundleName_InstanceIDTable = new Dictionary<string, int>();
    private static HashSet<int> AssetBundleNameInstanceIDList = new HashSet<int>();

    static int assetFileIndex = 1;

    private static int AssetBundleNameToInstanceID(string assetBundleName)
    {
        int reslut = -1;
        if(!assetBundleName_InstanceIDTable.TryGetValue(assetBundleName,out reslut))
        {
            assetBundleName_InstanceIDTable.Add(assetBundleName, assetFileIndex);
            AssetBundleNameInstanceIDList.Add(assetFileIndex);

            reslut = assetFileIndex;
            assetFileIndex++;
        }
        return reslut;
    }

    private static void ClearAssetFileTempData()
    {
        assetFileIndex = 1;
        assetBundleName_InstanceIDTable.Clear();
        allCreatedResourcePackage.Clear();
        allCreatedResourcePackageCallBackObj.Clear();
    }


    private static ResourcePackage AllocResourcePackage(int instanceID)
    {
        ResourcePackage reslut = null;
        if (AssetBundleNameInstanceIDList.Contains(instanceID))
        {
            if(!allCreatedResourcePackage.TryGetValue(instanceID,out reslut))
            {
                reslut = new ResourcePackage();
                reslut.Clear();
                reslut.instanceID = instanceID;
                allCreatedResourcePackage.Add(instanceID, reslut);
            }
        }

       return reslut;
    }

    private static ResourcePackage AllocResourcePackage(string assetBundleName)
    {
        int id = AssetBundleNameToInstanceID(assetBundleName);
        if(id <= 0)
        {
            return null;
        }
        return AllocResourcePackage(id);
    }

    private static void FillUnioFileInfo(UnionResConfigData cf,ResourcePackage pkg)
    {
        if(cf == null || pkg == null)
        {
            return;
        }
        pkg.unionIndx = cf.unionIndx; ;
        pkg.startIndex = cf.startIndex;
        pkg.dataLength = cf.dataLength;
    }

    private static ResourcePackageCallBackObj AllocResourcePackageCallBackObj(int instanceID,int PackageID)
    {
        ResourcePackageCallBackObj reslut = null;

        if (!allCreatedResourcePackageCallBackObj.TryGetValue(instanceID,out reslut))
        {
            reslut = new ResourcePackageCallBackObj();
            reslut.Clear();
            reslut.resInstanceID = instanceID;
            reslut.packageID = PackageID;
            allCreatedResourcePackageCallBackObj.Add(instanceID, reslut);
        }
        return reslut;
    }

    /// <summary>
    /// 释放资源包
    /// </summary>
    /// <param name="assetBundleName"></param>
    private static void ReleaseAssetBundle(string assetBundleName)
    {
        //非编辑器下，还有AssetBundle
        if (isOnRunTimeMode)
        {
            ResourcePackage pkg = AllocResourcePackage(assetBundleName);
            //如果这个ab没有被加载出来.
            if (!m_LoadedResourcePackage.Contains(pkg.instanceID))
            {
                PushToNeedToUnLoadPakage(pkg.instanceID);
            }
            else
            {
                //baundle的引用减少
                int assetBundleRefCount = pkg.Release();
                //如果这个bundle已经没有引用了,卸载bundle
                if (assetBundleRefCount <= 0)
                {
                    RemoveAllResourcePackageCallBackList(assetBundleName);
                    //移除加载
                    UnLoadAssetBundle(pkg.instanceID);
                }
            }

        }
    }


    private static void UnLoadAssetBundle(int bundleID, bool toUnLoadAllAssets = true)
    {
        ResourcePackage pkg = AllocResourcePackage(bundleID);
        if (m_LoadedResourcePackage.Contains(bundleID))
        {
            if (pkg.AssetBundleInstance)
            {
                pkg.AssetBundleInstance.Unload(toUnLoadAllAssets);
            }
            m_LoadedResourcePackage.Remove(bundleID);
            pkg.Clear();
        }
    }

    private static void UnLoadAllAssetBundle()
    {
        ClearPrefabRes();
        ClearAnimatorControllerRes();
        ClearFBXRes();
        ClearAnimationClipRes();
        ClearMaterialRes();
        ClearUIRes();
        ClearTexutreRes();
        ClearAudioRes();

        foreach (int id in m_LoadedResourcePackage)
        {
            ResourcePackage pkg = AllocResourcePackage(id);
            if(pkg.AssetBundleInstance)
            {
                pkg.AssetBundleInstance.Unload(true);
                pkg.AssetBundleInstance = null;
            }

        }

        ResNode.ClearWhenGameEnd();
        m_ResourcePackageRequestTable.Clear();
        m_NeedToUnLoadResourcePackageTable.Clear();
        m_ResWaitingForCallBack.Clear();

        m_LoadedResourcePackage.Clear();
        m_NeedToUnLoadList.Clear();
        m_LoadingAssetList.Clear();
        m_ReadyForUpLoadAssetsAsyncRequestTable.Clear();
        m_ResourcePackageWatingForCallBack.Clear();
        m_ProcessingAssetsFileAsyncRequestList.Clear();
        m_WaitForUpLoadAssetsFileAsyncRequestSet.Clear();
        currUpLoad_AssetsFile_AsyncRequestClipTime = 0.0f;
        ClearAssetFile();

    }


    #region 同步加载

    private static ResourcePackage GetResourcePackge(ResConfigData config)
    {
        ResourcePackage pkg = AllocResourcePackage(config.AssetBundleName);
        if (m_LoadedResourcePackage.Contains(pkg.instanceID))
        {
            pkg.AddRef();
        }
        else
        {

            string abName = config.AssetBundleName;
            string patch = AssetBundleManager.FullAssetPackagePathRoot + config.AssetBundleName;

            UnionResConfigData unionRes = config as UnionResConfigData;
            if (unionRes != null)
            {
                patch = unionRes.UnionFilePath;
                abName = unionRes.ShortUnionFilePath;
            }

            if (!m_RespackageTable.ContainsKey(abName))
            {
                AssetBudleLog("AssetBundle文件不存在!,路径:" + patch);
                return pkg;
            }
            RemoveNeedToUnLoadPackage(pkg.instanceID);
            //异步加载的列表里有他，那么要强制同步加载了
            if (m_ResourcePackageRequestTable.Contains(pkg.instanceID))
            {
                m_ResourcePackageRequestTable.Remove(pkg.instanceID);
                if (pkg.isUpLoadToLoadingThread)
                {
                    pkg.AssetBundleInstance = pkg.fileRequest.assetBundle;
                }
                else
                {
                    pkg.isUpLoadToLoadingThread = true;
                    pkg.AssetBundleInstance = LoadAssetBundle(patch, config);
                }
                RemoveFromAssetsFileUpLoadQueue(pkg.instanceID);
                RemoveAssetsFileProcessingList(pkg.instanceID);
                PushToResourcePackageCallBackList(pkg.instanceID);
            } 
            else
            {
                
                AssetBundle pAssetBundle = LoadAssetBundle(patch, config);
                pkg.AssetBundleInstance = pAssetBundle;
            }
            pkg.filePatch = patch;
            pkg.AddRef();
            m_LoadedResourcePackage.Add(pkg.instanceID);
        }

        return pkg;
    }

    private static AssetBundle LoadAssetBundle(string bundleName, ResConfigData data)
    {
        if (null != data && data.GetType() == typeof(UnionResConfigData))
        {
            UnionResConfigData unionData = data as UnionResConfigData;
            return LoadAssetBundleFromFile(unionData.UnionFilePath, unionData.startIndex);

        }
        else
        {
            return LoadAssetBundleFromFile(bundleName);
        }
    }

    private static AssetBundle LoadAssetBundleFromFile(string bundleName, int offset = 0)
    {
        AssetBundle pAssetBundles = null;
        int reloffset = offset;
        //ab包全部走加密
        //if (!Application.isEditor)
        {
            reloffset += AssetFileClassfiyKeyBytes.Length;
        }
        pAssetBundles = AssetBundle.LoadFromFile(bundleName, 0, (ulong)reloffset);
        return pAssetBundles;
    }
    #endregion

    #region 异步加载


    /// <summary>
    /// 正在加载的资源包请求,资源包名-Request
    /// </summary>
    private static HashSet<int> m_ResourcePackageRequestTable = new HashSet<int>();

    /// <summary>
    /// 正在等待卸载的异步加载的资源文件,AssetGUID
    /// 存在在这个列表里的，是引用为0，卸载了ab包，但是还没有加载出来的
    /// </summary>
    private static HashSet<int> m_NeedToUnLoadResourcePackageTable = new HashSet<int>();

    /// <summary>
    /// 等待回调的资源包，[资源包名-资源包请求]
    /// </summary>
    private static HashSet<int> m_ResourcePackageWatingForCallBack = new HashSet<int>();


    /// <summary>
    /// 最小的异步请求上载数目，小于这个数目不会上载异步请求
    /// </summary>
    private static int MinUpLoadAssetsFileAsyncRequestCount = 0;

    /// <summary>
    /// 资源异步请求时间片段。只有异步请求的数量累计达到了MinUpLoadAssetsFileAsyncRequestCount或者超过这个时间片段
    /// 才去上载异步
    /// </summary>
    private static float MaxUpLoad_AssetsFile_AsyncRequestClipTime = 0.05f;
    private static float currUpLoad_AssetsFile_AsyncRequestClipTime = 0.0f;

    /// <summary>
    /// 是否在处理异步请求，如果有异步请求在处理，则等待请求
    /// </summary>
    private static bool isProcessingAssetsFileAsyncRequest
    {
        get
        {
            return m_ProcessingAssetsFileAsyncRequestList.Count > 0;
        }
    }

    private static List<int> m_ProcessingAssetsFileAsyncRequestList = new List<int>(5);

    /// <summary>
    /// 将要处理的异步资源请求
    /// </summary>
    private static HashSet<int> m_WaitForUpLoadAssetsFileAsyncRequestSet = new HashSet<int>();

    private static void EnQueueWaitForUpLoadAssetsFileAsyncRequestTable(int assetBundleID)
    {
        m_WaitForUpLoadAssetsFileAsyncRequestSet.Add(assetBundleID);
    }

    private static int AssetsFileAsyncRequestSortFunc(int left, int right)
    {
        ResourcePackage req_left = AllocResourcePackage(left);
        ResourcePackage req_right = AllocResourcePackage(right); 
        if (null == req_left || null == req_right)
        {
            Debug.LogError("AssetsFileAsyncRequestSort failed,req_left:" + req_left + ",req_right:" + req_right);
            return 0;
        }
        return req_left.priority.CompareTo(req_right.priority);
    }


    private static void ProcessAssetsFileAsyncRequest(float dealtTime)
    {
        Temp_LoadCompleteList_int.Clear();
        //已经在处理请求或者没有请求返回
        if (isProcessingAssetsFileAsyncRequest ||
            m_WaitForUpLoadAssetsFileAsyncRequestSet.Count <= 0)
        {
            return;
        }

        currUpLoad_AssetsFile_AsyncRequestClipTime += dealtTime;
        int cout = m_WaitForUpLoadAssetsFileAsyncRequestSet.Count;

        //数量足够，或者达到了一个异步时间片段
        if (cout < MinUpLoadAssetsFileAsyncRequestCount && currUpLoad_AssetsFile_AsyncRequestClipTime < MaxUpLoad_AssetsFile_AsyncRequestClipTime)
        {
            return;
        }
        currUpLoad_AssetsFile_AsyncRequestClipTime = 0.0f;
        int UpLoadCount = 0;
        TempList_int.Clear();
        TempList_int.AddRange(m_WaitForUpLoadAssetsFileAsyncRequestSet);
        TempList_int.Sort(AssetsFileAsyncRequestSortFunc);

        ResourcePackage req = AllocResourcePackage(TempList_int[0]);

        foreach (int id in TempList_int)
        {
            //一次最多上传MinUpLoadAsyncRequestCount请求
            if (UpLoadCount >= MinUpLoadAssetsFileAsyncRequestCount)
            {
                break;
            }

            req = AllocResourcePackage(id);

            RemoveFromAssetsFileUpLoadQueue(id);
            UpLoadCount++;
            req.fileRequest = LoadAssetBundleAsync(req.filePatch, req.startIndex);
            req.isUpLoadToLoadingThread = true;
            m_ProcessingAssetsFileAsyncRequestList.Add(id);
        }
    }

    private static void ReleaseResourcePackageWhenLoadFiald(ResourcePackage pkg)
    {
        if (null == pkg)
        {
            return;
        }
        //不用管了，加载失败之后啥都没的
        //pkg.RefCount--;
        //if (pkg.RefCount <= 0)
        //{
        //    UnLoadAssetBundle(pkg.AssetBundleName);
        //}
    }

    private static void RemoveFromAssetsFileUpLoadQueue(int instanceID)
    {
        if (m_WaitForUpLoadAssetsFileAsyncRequestSet.Contains(instanceID))
        {
            m_WaitForUpLoadAssetsFileAsyncRequestSet.Remove(instanceID);
        }
    }

    /// <summary>
    /// 从异步等待队列中移除
    /// </summary>
    /// <param name="req"></param>
    /// <returns></returns>
    private static bool RemoveAssetsFileProcessingList(int packageID)
    {
        if (m_ProcessingAssetsFileAsyncRequestList.Contains(packageID))
        {
            m_ProcessingAssetsFileAsyncRequestList.Remove(packageID);
            return true;
        }
        return false;
    }


    private static void RemoveNeedToUnLoadPackage(int instanceID)
    {
        if (m_NeedToUnLoadResourcePackageTable.Contains(instanceID))
        {
            ResourcePackage req = AllocResourcePackage(instanceID);
            m_NeedToUnLoadResourcePackageTable.Remove(instanceID);
            m_ResourcePackageRequestTable.Add(instanceID);
        }

    }

    private static void PushToNeedToUnLoadPakage(int instanceID)
    {
        ResourcePackage req = AllocResourcePackage(instanceID);
        //如果加载列表里有他
        if (m_ResourcePackageRequestTable.Contains(instanceID))
        {
            int refcout = req.Release();
            if (refcout <= 0)
            {
                m_ResourcePackageRequestTable.Remove(instanceID);
                RemoveFromAssetsFileUpLoadQueue(instanceID);
                req.ResetPriority();
                if (req.isUpLoadToLoadingThread)
                {
                    //加入到将要卸载的列表
                    m_NeedToUnLoadResourcePackageTable.Add(instanceID);
                }
                else
                {
                    RemoveAssetsFileProcessingList(req.instanceID);
                }
            }

        }
    }

    private static void ProcessNeedToUnLoadPackage(List<int> LoadCompleteList)
    {
        if (m_NeedToUnLoadResourcePackageTable.Count > 0)
        {
            LoadCompleteList.Clear();

            foreach (int ins in m_NeedToUnLoadResourcePackageTable)
            {
                ResourcePackage package = AllocResourcePackage( ins);

                if (!package.isUpLoadToLoadingThread)
                {
                    continue;
                }

                /*为了保证资源的加载顺序，只要有一个包没有加载完成，
                 * 就直接跳出本次检测，防止有的包加载完了，但是他依赖的包还没有加载完成
                 */
                if (package.isDone)
                {
                    LoadCompleteList.Add(ins);
                    AssetBundle pAssetBundle = package.fileRequest.assetBundle;
                    pAssetBundle.Unload(true);
                }
                else
                {
                    break;
                }
            }

            //移除已经加载完成的
            foreach (int s in LoadCompleteList)
            {
                RemoveAssetsFileProcessingList(s);
                m_NeedToUnLoadResourcePackageTable.Remove(s);
            }
        }
    }


    /// <summary>
    /// 更新异步加载资源文件的加载进度
    /// </summary>
    /// <param name="LoadCompleteList"></param>
    private static void UpdateLoadingAssetFileProgress(List<int> LoadCompleteList)
    {
        if (m_ResourcePackageRequestTable.Count > 0)
        {
            LoadCompleteList.Clear();

            foreach (int ins in m_ResourcePackageRequestTable)
            {
                ResourcePackage package = AllocResourcePackage(ins);

                
                if(!package.isUpLoadToLoadingThread)
                {
                    continue;
                }

                /*为了保证资源的加载顺序，只要有一个包没有加载完成，
                 * 就直接跳出本次检测，防止有的包加载完了，但是他依赖的包还没有加载完成
                 */
                if (package.isDone)
                {
                    LoadCompleteList.Add(ins);
                    AssetBundle pAssetBundle  = package.fileRequest.assetBundle;

                    package.AssetBundleInstance = pAssetBundle;
                    m_LoadedResourcePackage.Add(package.instanceID);
                                

                    //加载完一个异步，从正在处理中移除
                    if (!RemoveAssetsFileProcessingList(ins))
                    {
                        Debug.LogError("AssetBundle重大错误222!");
                    }
                }
                //else
                //{
                //    break;
                //}
            }

            //移除已经加载完成的
            foreach (int s in LoadCompleteList)
            {
                m_ResourcePackageRequestTable.Remove(s);
                ResourcePackage package = AllocResourcePackage(s);
                PushToResourcePackageCallBackList(s);
            }
        }
    }

    private static ResourcePackageCallBackObj BuildResourcePackageCallBackObj(ResourcePackage pack,ResNode res,ResConfigData data ,System.Action<ResConfigData, ResNode, System.Object> usercallBack,System.Object userdataObj)
    {
        ResourcePackageCallBackObj reslut = AllocResourcePackageCallBackObj(res.instanceID, pack.instanceID);

        reslut.userCallBck = usercallBack;
        reslut.userDataObj = userdataObj;
        reslut.configData = data;
        return reslut;
    }

    /// <summary>
    /// 异步构建package
    /// </summary>
    /// <param name="reslutTable"></param>
    /// <param name="config"></param>
    /// <param name="resourcePackageCallBack"></param>
    /// <param name="UserCallBack"></param>
    /// <param name="userDataObj"></param>
    /// <returns></returns>
    private static void BuildPacakgeAsync(ref ResNode result, Dictionary<int, ResNode> reslutTable, ResConfigData config,System.Action<ResourcePackageCallBackObj> resourcePackageCallBack,
                                       System.Action<ResConfigData, ResNode, System.Object> UserCallBack, System.Object userDataObj,bool isPrefabResNode = false)
    {
        string patch = AssetBundleManager.FullAssetPackagePathRoot + config.AssetBundleName;
        
        UnionResConfigData unionRes = config as UnionResConfigData;
        string abPath = config.AssetBundleName;
        if (unionRes != null)
        {
            patch = unionRes.UnionFilePath;
            abPath = unionRes.ShortUnionFilePath;
        }
        if (!m_RespackageTable.ContainsKey(abPath))
        {
            AssetBudleLog("AssetBundle文件不存在!,路径:" + patch);
            return;
        }

        result = AllocResNode(config.AssetGUID);

        result.StartLoadedTime();

        //预制体是预先创建好ResNode的，所以不能清除和增加引用，直接使用外部的即可
        if (!isPrefabResNode)
        {
            result.Clear();
            result.AddRef();
        }

        result.config = config;
        result.isAsync = true;
        result.status = ResNodeStatus.WatingForLoad;

        AssetRequest are = null;
        are = AllocAssetRequest(result.instanceID);
        m_LoadingAssetList.Add(result.instanceID);
        are.Clear();

        reslutTable.Add(result.instanceID, result);

        GenerateResCallBack(result, config, UserCallBack, userDataObj);

        ResourcePackage pakg = AllocResourcePackage(config.AssetBundleName);

        pakg.priority = config.priority;
        //已经有了
        if (m_LoadedResourcePackage.Contains(pakg.instanceID))
        {
            pakg.AddRef();
            pakg.AddAsyncCallBackData(resourcePackageCallBack, BuildResourcePackageCallBackObj(pakg, result, config, UserCallBack, userDataObj));

            PushToResourcePackageCallBackList(pakg.instanceID);
        }
        else
        {
            //如果卸载列表里，先移除掉，避免加载出来就被卸载了
            RemoveNeedToUnLoadPackage(pakg.instanceID);
            //没有开启异步加载
            if (!enabledAsyncLoadAssetFile)
            {
                pakg = GetResourcePackge(config);
                pakg.AddAsyncCallBackData(resourcePackageCallBack, BuildResourcePackageCallBackObj(pakg, result, config, UserCallBack, userDataObj));
                PushToResourcePackageCallBackList(pakg.instanceID);
                return;
            }
            
            //全新的包
            if (!m_ResourcePackageRequestTable.Contains(pakg.instanceID))
            {
                pakg.Clear();
                pakg.isAsync = true;
                pakg.filePatch = patch;

                FillUnioFileInfo(config as UnionResConfigData,pakg);
                EnQueueWaitForUpLoadAssetsFileAsyncRequestTable(pakg.instanceID);
                m_ResourcePackageRequestTable.Add(pakg.instanceID);
            }

            pakg.AddAsyncCallBackData(resourcePackageCallBack, BuildResourcePackageCallBackObj(pakg, result, config, UserCallBack, userDataObj));
            pakg.AddRef();
        }
        return;
    }

    private static void ProcessResourcePackageCallBack()
    {
        Temp_LoadCompleteList_int.Clear();
        Temp_LoadCompleteList_int.AddRange(m_ResourcePackageWatingForCallBack);
        m_ResourcePackageWatingForCallBack.Clear();

        foreach (int id in Temp_LoadCompleteList_int)
        {
            ResourcePackage packageTemp = AllocResourcePackage(id);
            Temp_resourcePackageCallBackList.Clear();
            Temp_callBackDataList.Clear();

            Temp_resourcePackageCallBackList.AddRange(packageTemp.resourcePackageCallBackList);
            Temp_callBackDataList.AddRange(packageTemp.callBackDataList);
            packageTemp.ClearAsyncCallBackData();

            for (int i = 0; i < Temp_resourcePackageCallBackList.Count; i++)
            {
                System.Action<ResourcePackageCallBackObj> cb = Temp_resourcePackageCallBackList[i];
                ResNode result = AllocResNode(Temp_callBackDataList[i].resInstanceID);

                cb(Temp_callBackDataList[i]);
            }
            
        }
        
    }


    /// <summary>
    /// 移除延迟回调列表，这个是指调用异步加载，而已经加载完成的延迟回调，确保回调函数是慢于调用函数的
    /// </summary>
    private static void RemoveFromResourcePackageCallBackList(string AssetBundleName, ResNode result)
    {
        if (AssetBundleName.Equals(string.Empty))
        {
            return;
        }

        ResourcePackage package = AllocResourcePackage(AssetBundleName);
        for (int i = 0; i < package.resourcePackageCallBackList.Count; i++)
        {
            if (package.callBackDataList[i].resInstanceID.Equals(result.instanceID))
            {
                package.resourcePackageCallBackList.RemoveAt(i);
                package.callBackDataList.RemoveAt(i);
                break;
            }
        }

    }

    private static void RemoveAllResourcePackageCallBackList(string AssetBundleName)
    {
        if (AssetBundleName.Equals(string.Empty))
        {
            return;
        }
        ResourcePackage package = AllocResourcePackage(AssetBundleName);
        package.ClearAsyncCallBackData();
        if (m_ResourcePackageWatingForCallBack.Contains(package.instanceID))
        {
            m_ResourcePackageWatingForCallBack.Remove(package.instanceID);
            
        }
    }

    private static void PushToResourcePackageCallBackList(int packageID)
    {
        if (!m_ResourcePackageWatingForCallBack.Contains(packageID))
        {
            m_ResourcePackageWatingForCallBack.Add(packageID);
        }
    }

    private static AssetBundleCreateRequest LoadAssetBundleAsync(string bundleName, int unionStartIndex)
    {
        if (unionStartIndex > 0)
        {
            return LoadAssetBundleFromFileAsync(bundleName, unionStartIndex);

        }
        else
        {
            return LoadAssetBundleFromFileAsync(bundleName);
        }
    }


    private static AssetBundleCreateRequest LoadAssetBundleAsync(string bundleName, ResConfigData data)
    {
        if (null != data && data.GetType() == typeof(UnionResConfigData))
        {

            //return LoadAssetBundleFromMenory(bundleName, data);
            UnionResConfigData unionData = data as UnionResConfigData;
            return LoadAssetBundleFromFileAsync(unionData.UnionFilePath, unionData.startIndex);

        }
        else
        {
            return LoadAssetBundleFromFileAsync(bundleName);
        }
    }

    private static AssetBundleCreateRequest LoadAssetBundleFromFileAsync(string bundleName, int offset = 0)
    {
        AssetBundleCreateRequest pAssetBundles = null;
        int reloffset = offset;
        //ab包全部走加密
        //if (!Application.isEditor)
        {
            reloffset += AssetFileClassfiyKeyBytes.Length;
        }
        pAssetBundles = AssetBundle.LoadFromFileAsync(bundleName, 0, (ulong)reloffset);
        return pAssetBundles;
    }

    #endregion




    #region util

    const int KB = 1024;
    const int MB = 1024 * 1024;
    const ulong GB = 1024 * 1024 * 1024;

    /// <summary>
    /// 需要的空余磁盘空间
    /// </summary>
    const ulong TotalDiskFreeSpace = 2 * GB;

    /// <summary>
    /// 每次读取多少内存数据块,并不是越大越好，因为硬盘的写入速度限制
    /// </summary>
    public const int blockSize = 4 * MB;

    private static bool CheckDiskFreeSpace()
    {

        string str_HardDiskName = Application.dataPath;
        string[] tempstrs = str_HardDiskName.Split(':');
        str_HardDiskName = tempstrs[0];


        ulong FreeBytesAvailable = 0;
        ulong TotalNumberOfBytes = 0;
        ulong TotalNumberOfFreeBytes = 0;
        str_HardDiskName = str_HardDiskName + ":\\";
        try
        {
            GetDiskFreeSpaceEx(str_HardDiskName, out FreeBytesAvailable, out TotalNumberOfBytes, out TotalNumberOfFreeBytes);
        }
        catch (Exception e)
        {
            AssetBudleLog(e.Message, true);
        }

        if (TotalNumberOfFreeBytes >= TotalDiskFreeSpace)
        {
            return true;
        }
        GameUtil.MsgBox(IntPtr.Zero, "磁盘" + str_HardDiskName + "剩余空间不足,需要空间:" + TotalDiskFreeSpace / GB + "GB，当前空间:" + TotalNumberOfFreeBytes / GB + "GB!", "错误", 0x00000000 | 0x00000010);
        return false;
    }

    private static void ClearAssetFile()
    {

    }

    /// <summary>
    /// 将struct写入到流中得通用方法
    /// </summary>
    /// <param name="_struct">struct类型</param>
    /// <param name="stream">流</param>
    /// <returns>字节读取数</returns>
    public static int WriteStruct<T>(T _struct, Stream stream)
    {
        if (null == stream)
        {
            return 0;
        }
        byte[] buffer = StructToByte(_struct);
        stream.Write(buffer, 0, buffer.Length);
        return buffer.Length;
    }

    /// <summary>
    /// 从流中读取struct的通用方法
    /// </summary>
    /// <param name="_struct">struct类型</param>
    /// <param name="stream">流</param>
    /// <returns>字节读取数</returns>
    public static int ReadStruct<T>(ref T _struct, Stream stream)
    {
        int size = Marshal.SizeOf(_struct);
        byte[] buffer = new byte[size];
        size = stream.Read(buffer, 0, size);
        if (!Application.isEditor && !ForceOpenDeCode)
        {
            buffer = DeCodeAssetConfigByte(buffer);
        }
        _struct = ByteToStruct<T>(buffer);
        return size;
    }

    public static byte[] StructToByte<T>(T _struct)
    {
        int size = Marshal.SizeOf(_struct);
        byte[] buffer = new byte[size];

        IntPtr bufferIntPtr = Marshal.AllocHGlobal(size);

        try
        {
            Marshal.StructureToPtr(_struct, bufferIntPtr, true);
            Marshal.Copy(bufferIntPtr, buffer, 0, size);
        }
        finally
        {
            Marshal.FreeHGlobal(bufferIntPtr);
        }

        return buffer;
    }

    public static T ByteToStruct<T>(byte[] byteArray)
    {
        T t = default(T);
        if (byteArray == null || byteArray.Length <= 0)
        {
            return t;
        }
        GCHandle pinned = GCHandle.Alloc(byteArray, GCHandleType.Pinned);
        t = (T)Marshal.PtrToStructure(pinned.AddrOfPinnedObject(), typeof(T));
        pinned.Free();

        return t;
    }


    /// <summary>
    /// 写入字符串，会首先写入字符串的大小(int)，再写入字符串的内容
    /// </summary>
    /// <param name="str"></param>
    /// <param name="stream"></param>
    /// <returns>写入的字符串大小</returns>
    public static int WriteString(string str, Stream stream)
    {
        if (null == stream || str.Equals(string.Empty))
        {
            return 0;
        }
        byte[] buffer = StringToByte(str);
        int size = buffer.Length;
 
        //写入大小
        WriteStruct(size, stream);

        //写入数据
        stream.Write(buffer, 0, size);
        buffer = null;
        return size;
    }


    /// <summary>
    /// 读取字符串，先读一个int大小的字节，再度一个字符串
    /// </summary>
    /// <param name="stream"></param>
    /// <returns></returns>
    public static string ReadString(Stream stream)
    {
        string result = string.Empty;
        if (null == stream)
        {
            return result;
        }

        int length = 0;
        ReadStruct(ref length, stream);

        if (length > 0)
        {
            byte[] buffer = new byte[length];

            stream.Read(buffer, 0, length);
            if (!Application.isEditor && !ForceOpenDeCode)
            {
                buffer = DeCodeAssetConfigByte(buffer);
            }
            result = ByteToString(buffer);
            buffer = null;
        }

        return result;
    }

    public static string ByteToString(byte[] byteArray)
    {
        string result = string.Empty;
        if (byteArray == null || byteArray.Length <= 0)
        {
            return result;
        }

        result = System.Text.UTF8Encoding.UTF8.GetString(byteArray);
        return result;
    }

    public static byte[] StringToByte(string str)
    {
        byte[] buffer = System.Text.UTF8Encoding.UTF8.GetBytes(str);
        return buffer;
    }

    [DllImport("Kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
    private static extern bool GetDiskFreeSpaceEx(string lpDirectoryName, out ulong lpFreeBytesAvailable, out ulong lpTotalNumberOfBytes, out ulong lpTotalNumberOfFree);
    #endregion


    
}
