/*******************************************************************
** 文件名:	AssetBundleManager_UI.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	ab资源管理器
** 应  用:  	ab资源管理器，UI部分

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

public interface IResNodeHandle
{
    
}

public class ResNodeHandleParamBase
{
    public AssetType AssetType;
    public bool IsGuid ;

    public ResNodeHandleParamBase(AssetType _eAssetsType)
    {
        AssetType = _eAssetsType;
    }
}

public class ResNodeHandleParam_Path : ResNodeHandleParamBase
{
    public string StrResPath;
    public ResNodeHandleParam_Path(AssetType _eAssetsType, string _strPath): base(_eAssetsType)
    {
        StrResPath = _strPath;
    }
}

public class ResNodeHandleParam_Bundle : ResNodeHandleParamBase
{
    public string StrBundleName;
    public string StrAssetName;

    public ResNodeHandleParam_Bundle(AssetType _eAssetsType,string _strBundleName,string _strAssetsName):base(_eAssetsType)
    {
        StrBundleName = _strBundleName;
        StrAssetName = _strAssetsName;
    }
}

public class ResNodeHandleParam_ResConfigData : ResNodeHandleParamBase
{
    public ResConfigData ConfigData = null;
    public ResNodeHandleParam_ResConfigData(AssetType _eAssetsType, ResConfigData _resConfigData) :base(_eAssetsType)
    {
        ConfigData = _resConfigData;
    }
}



public sealed partial class AssetBundleManager : MonoBehaviour
{
    /// <summary>
    /// GUID-已经在使用的资源
    /// </summary>
    static Dictionary<int, ResNode> m_SpriteTable = new Dictionary<int, ResNode>();


    /// <summary>
    /// GUID-缓存的资源
    /// </summary>
    static Dictionary<int, ResNode> m_UISpriteCacheTable = new Dictionary<int, ResNode>();


    /// <summary>
    /// 包ID-包名
    /// </summary>
    //static Dictionary<int, string> m_UIPackageTable = new Dictionary<int, string>();

    /// <summary>
    /// 包名-资源名-ResConfigData
    /// </summary>
    static Dictionary<string, Dictionary<string, ResConfigData>> m_UIConfigTable = new Dictionary<string, Dictionary<string, ResConfigData>>();

    //static HashSet<string> LoadedUIAltas = new HashSet<string>();

    public static string UIConfigFileName_Csv = "UIConfig." + unityCSVExe;
    public static string UIConfigFileName_Bin = "UIConfig." + unityBinExe;


    private static Type SpriteType = typeof(Sprite);

    static bool IsUIAssetBundle(string abName)
    {
        return m_UIAssetBundleTable.Contains(abName);
    }

    //static void PushLoadedUIAltas(ResNode res)
    //{
    //    if(null == res)
    //    {
    //        return;
    //    }

    //    if(!res.IsValid())
    //    {
    //        return;
    //    }
    //}

    static void InitUIRes()
    {
        LoadConfig();
    }

    static void ClearUIRes()
    {

        ClearResTable(ref m_SpriteTable);
        ClearResTable(ref m_UISpriteCacheTable);

        //LoadedUIAltasID.Clear();
        //m_UIPackageTable.Clear();
        m_UIConfigTable.Clear();
    }


    #region 同步加载

    /// <summary>
    /// 取得sprite
    /// </summary>
    /// <param name="assetbudleName">包名</param>
    /// <param name="assetName">资源名</param>
    /// <returns></returns>
    private static ResNode GetSprite(string assetbudleName,string assetName)
    {
        if (!isVaild)
        {
            return null;
        }
        if (m_UIConfigTable.ContainsKey(assetbudleName))
        {
            Dictionary<string, ResConfigData> co = m_UIConfigTable[assetbudleName];
            ResConfigData da;
            if (co.TryGetValue(assetName, out da))
            {
                return GetSpriteResource(da);
            }
        }
        return null;
    }


    private static ResNode GetSpriteResource(ResConfigData config)
    {
        if (string.IsNullOrEmpty(config.AssetBundleName))
        {
            return null;
        }
        ResNode res = null;
        //先看看是不是通用资源
        GetCommonRes(ref res,config);
        if (null != res)
        {
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

        res = GetCacheSpriteResource(config); ;
        if (null != res)
        {
            m_SpriteTable.Add(res.instanceID, res);

            //判断一下这个资源是否是异步加载的
            if (res.isAsync)
            {
                if (!res.isLoadFinish)
                {
                    BuildResNodeWhenInAsyncLoading<Sprite>(ref res, config);
                    //Debug.LogWarning(StringHelper.BuildString(config.AssetName, "正在异步加载中，请调用ResNode.isLoadFinish查看是否完成"));
                }
            }

            res.AddRef();
            return res;
        }

        int InstanceID = GUIDToInstanceID(config.AssetGUID);
        //所需的资源已经存在
        if (m_SpriteTable.TryGetValue(InstanceID,out res))
        {

            //判断一下这个资源是否是异步加载的
            if (res.isAsync)
            {
                if (!res.isLoadFinish)
                {
                    BuildResNodeWhenInAsyncLoading<Sprite>(ref res, config);
                    //Debug.LogWarning(StringHelper.BuildString(config.AssetName, "正在异步加载中，请调用ResNode.isLoadFinish查看是否完成"));
                }
            }

            //同时增加这个资源的引用计数
            res.AddRef();
        }
        else
        {
            if(!isOnRunTimeMode)
            {
                BuildResNodeOnEditor<Sprite>(ref res,config);
            }
            else
            {
                BuildResNodeOnRunTime<Sprite>(ref res, config);
            }
            if (res != null)
            {
                m_SpriteTable.Add(res.instanceID, res);
            }
        }
        if (null != res)
        {
            res.assetType = AssetType.Asset_Sprite;
        }
        return res;
    }


    /// <summary>
    /// 删除资源
    /// </summary>
    /// <param name="res"></param>
    private static void DeleteSpriteResource(ref ResNode res, bool Immediate)
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

        //只要有一个需要理解卸载，那么就得立刻卸载
        if (!res.UnLoadImmediate)
        {
            res.UnLoadImmediate = Immediate;
        }
        int guid = res.instanceID;
        //有这个资源
        if (m_SpriteTable.ContainsKey(guid))
        {
            int refCount = res.Release();

            //这个资源已经没有人引用了
            if (refCount <= 0)
            {
                m_SpriteTable[guid] = null;
                m_SpriteTable.Remove(guid);


                if (!res.UnLoadImmediate)
                {
                    CacheSpriteResource(res);
                }
                else
                {
                    if (m_UISpriteCacheTable.ContainsKey(guid))
                    {
                        m_UISpriteCacheTable.Remove(guid);
                    }
                    UnLoadResource(ref res);
                }
            }
        }
        else
        {
            Debug.LogError("要卸载的资源不是Sprite,忽略此资源，有可能会存在资源泄露!:" + res.AssetBundleName + "," + res.AssetName);
        }
        res = null;
    }

    #endregion


    #region 异步加载


    /// <summary>
    /// 取得sprite
    /// </summary>
    /// <param name="assetbudleName">包名</param>
    /// <param name="assetName">资源名</param>
    /// <param name="callBack">回调</param>
    /// <param name="userDataObj">用户数据，这个会出现在资源回调里,不需要可以填null</param>
    /// <param name="priority">加载优先级</param>
    /// <returns></returns>
    private static ResNode GetSpriteAsync(string assetbudleName, string assetName,
                                         System.Action<ResConfigData, ResNode, System.Object> callBack,
                                         System.Object userDataObj, AssetLoadPriority priority = AssetLoadPriority.Priority_Normal)
    {
        if (!isVaild)
        {
            return null;
        }
        if (m_UIConfigTable.ContainsKey(assetbudleName))
        {
            Dictionary<string, ResConfigData> co = m_UIConfigTable[assetbudleName];
            ResConfigData da;
            if (co.TryGetValue(assetName, out da))
            {
                return GetSpriteResourceAsync(da, callBack, userDataObj, priority);
            }
        }
        return null;
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
    private static ResNode GetSpriteResourceAsync(ResConfigData config, System.Action<ResConfigData, ResNode, System.Object> callBack,
                                                   System.Object userDataObj, AssetLoadPriority priority = AssetLoadPriority.Priority_Normal)
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
        if (null != res)
        {
            PushToResDelayCallBackList(res, config, callBack, userDataObj);
            return res;
        }

        //编辑器下无异步加载
        if (Application.isEditor && !OpenRunTime )
        {
            res = GetSpriteResource(config);
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
        res = BuildSpriteResNodeAsync(config, callBack, userDataObj);
        if(null != res)
        {
            res.assetType = AssetType.Asset_Sprite;
        }
        return res;
    }


    /// <summary>
    /// 删除异步加载的资源，与GetSpriteResourceAsync对应
    /// </summary>
    /// <param name="res"></param>
    /// <param name="callBack"></param>
    private static void DeleteSpriteResourceAsync(ref ResNode res, System.Action<ResConfigData, ResNode, System.Object> callBack, bool Immediate)
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
            DeleteSpriteResource(ref res, Immediate);
            return;
        }
        int guid = res.instanceID;
        //有这个资源
        if (m_SpriteTable.ContainsKey(guid))
        {
            int refCount = res.Release();

            //这个资源已经没有人引用了
            if (refCount <= 0)
            {
                RemoveAllResCallBack(m_ResCallBackTable, res);
                RemoveAllDelayCallBackList(res);

                m_SpriteTable[guid] = null;
                m_SpriteTable.Remove(guid);

                if (!res.UnLoadImmediate)
                {
                    CacheSpriteResource(res);
                }
                else
                {
                    if (m_UISpriteCacheTable.ContainsKey(guid))
                    {
                        m_UISpriteCacheTable.Remove(guid);
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
            Debug.LogError("要卸载的资源不是Sprite,忽略此资源，有可能会存在资源泄露!:" + res.AssetBundleName + "," + res.AssetName);
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
    private static ResNode BuildSpriteResNodeAsync(ResConfigData config, System.Action<ResConfigData, ResNode, System.Object> callBack,
                                                    System.Object userDataObj)
    {
        ResNode res = null;

        int assetGUID = GUIDToInstanceID(config.AssetGUID);

        //如果这个资源正在要卸载的列表里，移除掉,免得资源加载完就被卸载了
        res = GetResNodeFromNeedToUnLoadAsset(assetGUID);
        if (res != null)
        {
            res.AddRef();
            m_SpriteTable.Add(assetGUID, res);

            GenerateResCallBack(res, config, callBack, userDataObj);
            res.config = config;
            return res;
        }

        res = GetCacheSpriteResource(config);

        //如果缓存里有这个资源结点
        if (res != null)
        {
            res.AddRef();
            m_SpriteTable.Add(assetGUID, res);

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
        if (m_SpriteTable.ContainsKey(assetGUID))
        {
            res = m_SpriteTable[assetGUID];
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

        res = GetSpritePackgeAsync(config, SpriteResourcePackageCallBack, callBack, userDataObj);

        //res = BuildResNodeOnRunTimeAsync<Sprite>(config, callBack, userDataObj);
        //if (res != null)
        //{
        //    m_SpriteTable.Add(config.AssetGUID, res);
        //}

        return res;
    }

    private static ResNode GetSpritePackgeAsync(ResConfigData config, System.Action<ResourcePackageCallBackObj> resourcePackageCallBack,
                                       System.Action<ResConfigData, ResNode, System.Object> UserCallBack, System.Object userDataObj)
    {

        ResNode res = null;
        BuildPacakgeAsync(ref res, m_SpriteTable, config, resourcePackageCallBack, UserCallBack, userDataObj);
        return res;
    }
 
    private static bool UpLoadSpriteAsyncRequest(ResourcePackageCallBackObj req)
    {
        return UpLoadAsyncRequest<Sprite>(req);
    }

    private static void SpriteResourcePackageCallBack(ResourcePackageCallBackObj req)
    {
        EnQueueWaitForUpLoadAssetsAsyncRequestTable(SpriteType, req);
    }

    #endregion

    #region 其他
    /// <summary>
    /// 缓存，避免频繁的加载
    /// </summary>
    /// <param name="res"></param>
    private static void CacheSpriteResource(ResNode res)
    {
        res.cacheTime = 0.0f;

        //缓存起来
        m_UISpriteCacheTable.Add(res.instanceID, res);
    }


    static ResNode GetCacheSpriteResource(ResConfigData cofing)
    {
        ResNode res = null;
        int InstanceID = GUIDToInstanceID(cofing.AssetGUID);
        if (m_UISpriteCacheTable.TryGetValue(InstanceID, out res))
        {
            res.cacheTime = 0.0f;
            m_UISpriteCacheTable.Remove(InstanceID);
        }

        return res;
    }
   

    static void LoadConfig()
    {
        LoadBinConfig();
    }

    static void LoadCSVConfig()
    {
        string patch = AssetBundleManager.GetUICSVConfigPatch();
        if (!File.Exists(patch))
        {
            return;
        }
        m_UIConfigTable.Clear();
        //m_UIPackageTable.Clear();
        string stSchemeAllText = DeCodeAssetFile(patch);

        //去掉\n,因为ScpReader以"\r"分割
        stSchemeAllText = stSchemeAllText.Replace("\n", "");
        //再干掉最后的"\r"
        stSchemeAllText = stSchemeAllText.Remove(stSchemeAllText.Length - 1, 1);
        AssetBundleConfigReader UIReader = new AssetBundleConfigReader(stSchemeAllText, "UIConfig", 2);

        // 遍歷整個表并存儲起來
        for (int i = 0; i < UIReader.GetRecordCount(); ++i)
        {
            ResConfigData data = new ResConfigData();
            //int id = UIReader.GetInt(i, (int)UIConfigCol.COL_ID, -1);
            data.AssetBundleName = UIReader.GetString(i, (int)AssetConfigCol.COL_AssetBundleName, string.Empty);
            data.AssetName = UIReader.GetString(i, (int)AssetConfigCol.COL_AssetName, string.Empty);
            data.AssetGUID = UIReader.GetString(i, (int)AssetConfigCol.COL_AssetGUID, string.Empty);
            data.AssetSize_X = UIReader.GetFloat(i, (int)AssetConfigCol.COL_AssetSize_X, -1.0f);
            data.AssetSize_Y = UIReader.GetFloat(i, (int)AssetConfigCol.COL_AssetSize_Y, -1.0f);
            if (data.AssetName.Equals(string.Empty))
            {
                Debug.LogError("包名为空!");
                continue;
            }
            ////ID与包名对应表
            //if (!m_UIPackageTable.ContainsKey(id))
            //{
            //    m_UIPackageTable.Add(id, data.AssetBundleName);
            //}

            //包名已经存在了
            if (m_UIConfigTable.ContainsKey(data.AssetBundleName))
            {
                Dictionary<string, ResConfigData> ta = m_UIConfigTable[data.AssetBundleName];
                if (ta.ContainsKey(data.AssetName))
                {
                    Debug.LogError("重复的资源名:" + data.AssetName + ",包:" + data.AssetBundleName);
                    continue;
                }
                ta.Add(data.AssetName, data);
            }
            else
            {
                Dictionary<string, ResConfigData> ta = new Dictionary<string, ResConfigData>();
                ta.Add(data.AssetName, data);
                m_UIConfigTable.Add(data.AssetBundleName, ta);
            }
        }
        UIReader.Dispose();
        UIReader = null;
    }

    static void LoadBinConfig()
    {
        string patch = AssetBundleManager.GetUIBinConfigPatch();
        if (!File.Exists(patch))
        {
            return;
        }

        m_UIConfigTable.Clear();
        FileStream _readObj = new FileStream(patch, FileMode.Open, FileAccess.Read);
        if(null == _readObj)
        {
            Debug.LogError("读取UI配置失败:" + patch);
            return;
        }
        int cout = 0;
        AssetBundleManager.ReadStruct(ref cout, _readObj);

        if (cout <= 0)
        {
            return;
        }
        
        // 遍歷整個表并存儲起來
        for (int i = 0; i < cout; ++i)
        {
            ResConfigData data = new ResConfigData();
            data.AssetBundleName = AssetBundleManager.ReadString(_readObj);
            data.AssetName = AssetBundleManager.ReadString(_readObj);
            data.AssetGUID = AssetBundleManager.ReadString(_readObj);
            data.AssetSize_X = float.TryParse(AssetBundleManager.ReadString(_readObj), out data.AssetSize_X) ? data.AssetSize_X : -1;
            data.AssetSize_Y = float.TryParse(AssetBundleManager.ReadString(_readObj), out data.AssetSize_Y) ? data.AssetSize_Y : -1;
            if (data.AssetName.Equals(string.Empty))
            {
                Debug.LogError("包名为空!");
                continue;
            }

            //包名已经存在了
            if (m_UIConfigTable.ContainsKey(data.AssetBundleName))
            {
                Dictionary<string, ResConfigData> ta = m_UIConfigTable[data.AssetBundleName];
                if (ta.ContainsKey(data.AssetName))
                {
                    Debug.LogError("重复的资源名:" + data.AssetName + ",包:" + data.AssetBundleName);
                    continue;
                }
                ta.Add(data.AssetName, data);
            }
            else
            {
                Dictionary<string, ResConfigData> ta = new Dictionary<string, ResConfigData>();
                ta.Add(data.AssetName, data);
                m_UIConfigTable.Add(data.AssetBundleName, ta);
            }
        }

        _readObj.Flush();
        _readObj.Close();
        _readObj = null;
    }

    public static string GetUICSVConfigPatch()
    {
        //int i = Application.dataPath.LastIndexOf("/");
        //int l = Application.dataPath.Length;

        //string dataPatch = Application.dataPath.Substring(0, i);
        //dataPatch = dataPatch + "/Scp/" + AssetBundleManager.UIConfigFileName;

        string fullPatch = FullAssetPackagePathRoot + AssetBundleManager.UIConfigFileName_Csv;


        return fullPatch;
    }

    public static string GetUIBinConfigPatch()
    {
        //int i = Application.dataPath.LastIndexOf("/");
        //int l = Application.dataPath.Length;

        //string dataPatch = Application.dataPath.Substring(0, i);
        //dataPatch = dataPatch + "/Scp/" + AssetBundleManager.UIConfigFileName;

        string fullPatch = FullAssetPackagePathRoot + AssetBundleManager.UIConfigFileName_Bin;


        return fullPatch;
    }

    /// <summary>
    /// 更新Sprite缓存
    /// </summary>
    /// <param name="LoadCompleteList"></param>
    private static void UpdateSpriteCache(List<int> LoadCompleteList, float dealtTime)
    {
        if (m_UISpriteCacheTable.Count > 0)
        {
            LoadCompleteList.Clear();
            foreach (KeyValuePair<int, ResNode> r in m_UISpriteCacheTable)
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
                m_UISpriteCacheTable[s] = null;
                m_UISpriteCacheTable.Remove(s);
            }
        }
    }
    #endregion


    static Dictionary<IResNodeHandle, List<ResNode>> m_dicResNodeHandle = new Dictionary<IResNodeHandle, List<ResNode>>();

    public static ResNode GetResNode(IResNodeHandle _handle, ResNodeHandleParamBase _param)
    {
        if (_handle == null || _param == null)
            return null;

        ResNode returnNode = null;

        #region 分类型
        if (_param is ResNodeHandleParam_Path)
        {
            ResNodeHandleParam_Path pathParam = _param as ResNodeHandleParam_Path;

            returnNode = GetAssets(_param.AssetType, pathParam.StrResPath);
        }
        else if(_param is ResNodeHandleParam_Bundle)
        {
            ResNodeHandleParam_Bundle bundleParam = _param as ResNodeHandleParam_Bundle;


            returnNode = GetAssets(_param.AssetType,bundleParam.StrBundleName, bundleParam.StrAssetName);
        }
        else if(_param is ResNodeHandleParam_ResConfigData)
        {
            ResNodeHandleParam_ResConfigData resConfigData = _param as ResNodeHandleParam_ResConfigData;

            returnNode = GetAssets(_param.AssetType, resConfigData.ConfigData);
        }
        #endregion

        if (returnNode != null)
        {
            if (m_dicResNodeHandle.ContainsKey(_handle))
                m_dicResNodeHandle[_handle].Add(returnNode);
            else
                m_dicResNodeHandle.Add(_handle, new List<ResNode>() { returnNode });
        }

        return returnNode;
    }


    public static ResNode GetResNodeAsync(IResNodeHandle _handle, ResNodeHandleParamBase _param, IAsyncResLoadHandler callBack, System.Object userDataObj = null, AssetLoadPriority priority = AssetLoadPriority.Priority_Normal)
    {
        if (_handle == null || _param == null)
            return null;

        ResNode returnNode = null;

        #region 分类型
        if (_param is ResNodeHandleParam_Path)
        {
            ResNodeHandleParam_Path pathParam = _param as ResNodeHandleParam_Path;

            returnNode = GetAssetsAsync(_param.AssetType, callBack, pathParam.StrResPath, pathParam.IsGuid, userDataObj, priority);
        }
        else if (_param is ResNodeHandleParam_Bundle)
        {
            ResNodeHandleParam_Bundle bundleParam = _param as ResNodeHandleParam_Bundle;

            returnNode = GetAssetsAsync(_param.AssetType, bundleParam.StrBundleName, bundleParam.StrAssetName, callBack, userDataObj, priority);
        }
        else if (_param is ResNodeHandleParam_ResConfigData)
        {
            ResNodeHandleParam_ResConfigData resConfigData = _param as ResNodeHandleParam_ResConfigData;

            returnNode = GetAssetsAsync(_param.AssetType, resConfigData.ConfigData, callBack, userDataObj, priority);
        }
        #endregion

        if (returnNode != null)
        {
            if (m_dicResNodeHandle.ContainsKey(_handle))
                m_dicResNodeHandle[_handle].Add(returnNode);
            else
                m_dicResNodeHandle.Add(_handle, new List<ResNode>() { returnNode });
        }

        return returnNode;
    }

    

    public static bool ReleaseResNodeHandle(IResNodeHandle _handle,bool _bImmediate)
    {
        bool result = false;

        if (m_dicResNodeHandle.ContainsKey(_handle))
        {
            List<ResNode> listValue = m_dicResNodeHandle[_handle];
            ResNode tmpResNode = null;

            for (int i = listValue.Count - 1; i >= 0; --i) 
            {
                tmpResNode = listValue[i];
                if (tmpResNode.isAsync)
                    ReleaseResNodeAsync(_handle, tmpResNode, _handle as IAsyncResLoadHandler, _bImmediate);
                else
                    ReleaseResNode(_handle, tmpResNode, _bImmediate);               
            }

            listValue.Clear();

            result = true;
        }

        return result;
    }

    public static bool ReleaseResNode(IResNodeHandle _handle,ResNode _resNode,bool _bImmediate)
    {
        bool result = false;
        
        if(_handle != null && _resNode != null && _resNode.IsValid())
        {
            if (m_dicResNodeHandle.ContainsKey(_handle))
            {
                List<ResNode> listValue = m_dicResNodeHandle[_handle];

                ResNode FindedResNode = null;

                foreach (ResNode tmpResNode in listValue)
                {
                    if (tmpResNode.instanceID == _resNode.instanceID)
                    {
                        FindedResNode = tmpResNode;
                        break;
                    }
                }

                if(FindedResNode != null)
                {
                    listValue.Remove(FindedResNode);
                    DeleteAssets(ref FindedResNode, _bImmediate);

                    result = true;
                }
            }
        }           


        return result;
    }

    public static bool ReleaseResNodeAsync(IResNodeHandle _handle, ResNode _resNode, IAsyncResLoadHandler callBack, bool _bImmediate)
    {
        bool result = false;

        if (_handle != null && _resNode != null)
        {
            if (m_dicResNodeHandle.ContainsKey(_handle))
            {
                List<ResNode> listValue = m_dicResNodeHandle[_handle];

                ResNode FindedResNode = null;

                foreach (ResNode tmpResNode in listValue)
                {
                    if (tmpResNode.instanceID == _resNode.instanceID)
                    {
                        FindedResNode = tmpResNode;
                        break;
                    }
                }

                if (FindedResNode != null)
                {
                    listValue.Remove(FindedResNode);
                    DeleteAssetsAsync(ref _resNode, callBack, _bImmediate);
                    result = true;
                }
            }
        }


        return result;
    }
}
