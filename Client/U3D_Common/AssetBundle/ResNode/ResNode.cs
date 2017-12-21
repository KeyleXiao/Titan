/*******************************************************************
** 文件名:	ResNode.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	资源结点
** 应  用:  	资源结点类，所有的资源加载都是这个

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;
public enum AssetType
{
    Asset_NotUse,


    Asset_CommonAssets,
    Asset_Prefab,
    Asset_FBX,
    Asset_AnimationClip,
    Asset_AnimatorController,
    Asset_Audio,
    Asset_Font,
    Asset_Material,
    Asset_Texture,
    Asset_Sprite,
    Asset_Cursour,

    Asset_Sheme_LightingEffect,
}
public enum AssetLoadPriority
{

    Priority_None = 0,

    /// <summary>
    /// 普通
    /// </summary>
    Priority_Normal = 1,

    /// <summary>
    /// 中
    /// </summary>
    Priority_Medium = 2,

    /// <summary>
    /// 高
    /// </summary>
    Priority_High = 3,

    /// <summary>
    /// 独占，指独占加载线程，最快速的加载
    /// </summary>
    Priority_Exclusive = 4,
}

public enum ResNodeStatus
{
    Empty = 0,
    WatingForLoad = 1,
    Loading,
    LoadFinish,
}

public interface IAsyncResLoadHandler
{
    /// <summary>
    /// 资源加载回调，就算是加载失败也会回调
    /// </summary>
    /// <param name="cof"></param>
    /// <param name="res"></param>
    /// <param name="UserDataObj"></param>
    void ResLoadCallBack(ResConfigData cof, ResNode res,System.Object UserDataObj);
}

public interface ISaveSerializedData
{
    System.Object SaveSerializedData();

    void SetSerializedData(System.Object data);
}

public sealed class FBXAssets : UnityEngine.Object
{

}


public sealed partial class ResNode 
{
    public int instanceID = -1;

    public ResNodeStatus status = ResNodeStatus.Empty;

    public bool Cleared
    {
        get
        {
            return m_bCleared;
        }
    }

    private bool m_bCleared = false;

    public float UpLoadClipTime = 0.0f;

    public float UpLoadCostTime = 0.0f;

    public List<UnityEngine.Object> AllAssets
    {
        get
        {
            return assetObjList;
        }
    }

    /// <summary>
    /// 是否加载完成,就算加载失败，这个也是true
    /// </summary>
    public bool isLoadFinish
    {
        get
        {
            return status == ResNodeStatus.LoadFinish;
        }
    }

    /// <summary>
    /// 请不要调用此成员!!!!!! 资源物体，实际上是assetObjList[0]
    /// </summary>
    private UnityEngine.Object assetObj = null;

    /// <summary>
    /// 请不要调用此成员!!!!!! 所有资源，比如FBX类型的资源，里面会包括mesh，animationclip，avatar等
    /// </summary>
    private List<UnityEngine.Object> assetObjList = new List<UnityEngine.Object>();

    public string AssetBundleName
    {
        get
        {
            if (null == config)
            {
                return string.Empty;
            }
            return config.AssetBundleName;
        }
    }

    public string AssetName
    {
        get
        {
            if (null == config)
            {
                return string.Empty;
            }
            return config.AssetName;
        }
    }

    public string AssetGUID
    {
        get
        {
            if (null == config)
            {
                return string.Empty;
            }
            return config.AssetGUID;
        }
    }

    /// <summary>
    /// AB包是否被unload(false);
    /// </summary>
    public bool isBeReleaseAssetBundleIfNeed = false;

    /// <summary>
    /// 有写集成的子类，并不是一对一的关系，如SceneTexutreResConfigData，一个资源会对应多个纹理属性
    /// 所以，它只有ResConfigData这个基类部分是一一对应的。
    /// </summary>
    public ResConfigData config
    {
        get
        {
            return m_Config;
        }
        set
        {
            if (null == value)
            {
                Debug.LogError("ResConfig is null");
                return;
            }

            m_Config = value;
        }
    }

    public ResConfigData m_Config = null;

    /// <summary>
    /// 是否是异步加载的资源
    /// </summary>
    public bool isAsync = false;

    /// <summary>
    /// 已经缓存的时间，缓存时间到达一定得值，会删除这个资源
    /// </summary>
    public float cacheTime = 0.0f;

    public AssetType assetType = AssetType.Asset_NotUse;

    public bool UnLoadImmediate = false;

    private int m_ReferenceCount = 0;

    private List<System.Object> serialzedDataList = null;

    /// <summary>
    /// 判断是否为null
    /// </summary>
    /// <param name="res"></param>
    /// <returns></returns>
    public static bool isNullOrEmpty(ResNode res)
    {
        if(null == res)
        {
            return true;
        }

        if(res.isAsync)
        {
            if(res.isLoadFinish)
            {
                return !res.IsValid();
            }
            return false;
        }

        return !res.IsValid();
    }

    private void ResetTransform(GameObject go)
    {
        if(!go)
        {
            return;
        }
        RectTransform rtf = go.transform as RectTransform;
        if (rtf)
        {
            rtf.anchoredPosition = anchoredPosition;
            rtf.sizeDelta = sizeDelta;
        }

        go.transform.localPosition = localPosition;
        go.transform.localRotation = localRoation;
        go.transform.localScale = localScale;
    }

    private void ClearAssetObj()
    {
        if(bSavedMemory || !AssetBundleManager.isOnRunTimeMode)
        {
            return;
        }

        if(assetType == AssetType.Asset_Prefab)
        {
            foreach (UnityEngine.Object o in assetObjList)
            {
                GameObject.DestroyImmediate(o, true);
            }
        }

    }

    public void FillAssets(List<UnityEngine.Object> assets)
    {
        if(null == assets || assets.Count <= 0)
        {
            return;
        }


        if(assetObj)
        {
            Debug.LogWarning("资源没有卸载，填充资源失败!");
            return;
        }
        assetObj = assets[0];
        assetObjList.Clear();
        assetObjList.AddRange(assets);
        SaveAssetInfo();
        m_bCleared = false;
        EndLoadedTime();
        LoadingTickes = (DateTime.Now.Second) * 1000 + DateTime.Now.Millisecond;
    }

    public void FillAssets(UnityEngine.Object []assets)
    {
        if (null == assets || assets.Length <= 0)
        {
            return;
        }


        if (assetObj)
        {
            Debug.LogWarning("资源没有卸载，填充资源失败!");
            return;
        }
        assetObj = assets[0];
        assetObjList.Clear();
        assetObjList.AddRange(assets);
        SaveAssetInfo();
        m_bCleared = false;
        EndLoadedTime();
        LoadingTickes = (DateTime.Now.Second) * 1000 + DateTime.Now.Millisecond;
    }


    public void FillAssets(UnityEngine.Object asset)
    {
        if (!asset)
        {
            return;
        }


        if (assetObj)
        {
            Debug.LogWarning("资源没有卸载，填充资源失败!");
            return;
        }
        assetObj = asset;
        assetObjList.Clear();
        assetObjList.Add(asset);

        SaveAssetInfo();
        m_bCleared = false;
        EndLoadedTime();
        LoadingTickes = (DateTime.Now.Second) * 1000 + DateTime.Now.Millisecond ;
    }

    /// <summary>
    /// 这个节点是否有效,应调用此函数确认加载是否成功。
    /// 对于异步加载来说，谨当isLoadFinish=true，这个函数才会返回正确的结果。或者在回调函数中调用此函数
    /// </summary>
    /// <returns></returns>
    public bool IsValid()
    {
        return assetObj;
    }

    public int AddRef()
    {
        m_ReferenceCount++;
        return m_ReferenceCount;
    }

    public int Release()
    {
        m_ReferenceCount--;
        if(m_ReferenceCount <= 0)
        {
            m_ReferenceCount = 0;
            DestroyAllInstanceRes();
        }

        return m_ReferenceCount;
    }
    
    
    public void ResetPrioriy()
    {
        if(config != null)
        {
            config.ResetPriority();
        }
    }

    public void Clear()
    {   
        ClearAssetObj();
        ClearInstanceRes();
        ResetPrioriy();
        //ClearInstanceConfig();
        isBeReleaseAssetBundleIfNeed = false;
        m_Config = null;
        assetObj = null;
        assetObjList.Clear();
        isAsync = false;
        m_ReferenceCount = 0;
        bSavedMemory = false;
        assetType = AssetType.Asset_NotUse;
        status = ResNodeStatus.Empty;
      
        m_bCleared = true;

    }

    public int GetRefCount()
    {
        return m_ReferenceCount;
    }

    #region 预制体原始数据
    private Rect RectTransformRect;
    private Vector3 localPosition;
    private Quaternion localRoation;
    private Vector3 localScale;
    private Vector2 anchoredPosition;
    private Vector2 sizeDelta;

    /// <summary>
    /// 检查资源上的脚本
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <returns></returns>
    public bool CheckComponent<T>() where T : MonoBehaviour
    {
        if (assetType != AssetType.Asset_Prefab)
        {
            Debug.LogWarning(assetType + "不支持取脚本!");
            return false;
        }
        if (!IsValid())
        {
            return false;
        }
        GameObject res = (GameObject)assetObj;
        if (!res)
        {
            Debug.LogWarning(AssetName + "资源转换失败!");
            return false;
        }

        return res.GetComponent<T>();
    }

    /// <summary>
    /// 取得资源的位置
    /// </summary>
    /// <returns></returns>
    public Vector3 GetResPosition()
    {
        if (!IsValid())
        {
            return Vector3.zero;
        }

        if (assetType != AssetType.Asset_Prefab)
        {
            Debug.LogWarning(assetType + "不支持本函数-GetResPosition!");
            return Vector3.zero;
        }
        return localPosition;
    }

    /// <summary>
    /// 取得资源的角度
    /// </summary>
    /// <returns></returns>
    public Quaternion GetResRotation()
    {
        if (!IsValid())
        {
            return Quaternion.identity;
        }

        if (assetType != AssetType.Asset_Prefab)
        {
            Debug.LogWarning(assetType + "不支持本函数-GetResRotation!");
            return Quaternion.identity;
        }
        return localRoation;
    }

    /// <summary>
    /// 取得资源的缩放
    /// </summary>
    /// <returns></returns>
    public Vector3 GetResScale()
    {
        if (!IsValid())
        {
            return Vector3.zero;
        }

        if (assetType != AssetType.Asset_Prefab)
        {
            Debug.LogWarning(assetType + "不支持本函数-GetResScale!");
            return Vector3.zero;
        }
        return localScale;
    }

    /// <summary>
    /// 取得资源的AnchoredPosition
    /// </summary>
    /// <returns></returns>
    public Vector2 GetResAnchoredPosition()
    {
        if (!IsValid())
        {
            return Vector2.zero;
        }

        if (assetType != AssetType.Asset_Prefab)
        {
            Debug.LogWarning(assetType + "不支持本函数-GetResAnchoredPosition!");
            return Vector2.zero;
        }
        return anchoredPosition;
    }

    public Rect GetRectTransformRect()
    {
        if (!IsValid())
        {
            return default(Rect);
        }

        if (assetType != AssetType.Asset_Prefab)
        {
            Debug.LogWarning(assetType + "不支持本函数-GetRectTransformRect!");
            return default(Rect);
        }
        return RectTransformRect;
    }

    /// <summary>
    /// 取得资源的sizeDelta
    /// </summary>
    /// <returns></returns>
    public Vector2 GetResSizeDelta()
    {
        if (!IsValid())
        {
            return Vector2.zero;
        }

        if (assetType != AssetType.Asset_Prefab)
        {
            Debug.LogWarning(assetType + "不支持本函数-GetResSizeDelta!");
            return Vector2.zero;
        }
        return sizeDelta;
    }

    public T ConvertRes<T>() where T : UnityEngine.Object
    {
        if (!IsValid())
        {
            return null;
        }
        Type destonType = typeof(T);
        Type ComponentType = typeof(Component);
        if (destonType == typeof(GameObject))
        {
            Debug.LogWarning("不能将资源转换为GameObject,如果要实例化，请调用实例化函数，如果要取信息，请GetResXXX系列");
            return null;
        }

        if (destonType.IsSubclassOf(ComponentType))
        {
            Debug.LogWarning("要转换的类型是组件，如果需要获取资源的组件，请调用GetResComponent");
            return null;
        }

        T res = (T)assetObj;
        if (!res)
        {
            Debug.LogWarning(AssetName + "资源转换失败!");
            return null;
        }

        return res;
    }
    #endregion


    #region Debug

    /// <summary>
    /// 花了多长时间
    /// </summary>
    public double costLoadedTime = 0;

    /// <summary>
    /// 加载时刻，可用来查看资源的先后顺序
    /// </summary>
    public double LoadingTickes = 0;
    public void StartLoadedTime()
    {
        costLoadedTime = DateTime.Now.Ticks;
    }

    private void EndLoadedTime()
    {
        costLoadedTime = DateTime.Now.Ticks - costLoadedTime;
        costLoadedTime = GameUtil.TicksToSecond(costLoadedTime);
    }
    #endregion
}



