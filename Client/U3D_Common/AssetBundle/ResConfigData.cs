/*******************************************************************
** 文件名:	ResConfigData.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	资源配置
** 应  用:  	资源配置类，通过配置去找资源

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;


[System.Serializable]
public  class ResConfigData
{
    /// <summary>
    /// 依赖的AssetBundle包
    /// </summary>
    public string AssetBundleName = string.Empty;

    /// <summary>
    /// 包里的资源名
    /// </summary>
    public string AssetName = string.Empty;

    /// <summary>
    /// 资源GUID,只在编辑器模式下使用
    /// </summary>
    public string AssetGUID = string.Empty;

    /// <summary>
    /// 资源尺寸
    /// </summary>
    public float AssetSize_X = -1.0f;

    public float AssetSize_Y = -1.0f;

    /// <summary>
    /// 是否通过引用加载的，目前是预制体
    /// </summary>
    [System.NonSerialized]
    public bool fromReference = false;

    /// <summary>
    /// 资源优先级
    /// </summary>
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
    private AssetLoadPriority m_Priority = AssetLoadPriority.Priority_None;

    public void ResetPriority()
    {
        m_Priority = AssetLoadPriority.Priority_None;
        
    }

}

[System.Serializable]
public class UnionResConfigData : ResConfigData
{
    public int unionIndx = -1;
    public int startIndex = -1;
    public int dataLength = -1;

    /// <summary>
    /// 用来判断是否存在
    /// </summary>
    public string ShortUnionFilePath
    {
        get
        {
            return AssetBundleManager.PrefabUnionVariant + "/" + unionIndx + "." + AssetBundleManager.PrefabUnionVariant;
        }
    }

    public string UnionFilePath
    {
        get
        {
            return AssetBundleManager.FullAssetPackagePathRoot + AssetBundleManager.PrefabUnionVariant + "/" + unionIndx + "." + AssetBundleManager.PrefabUnionVariant;
        }
    }
}