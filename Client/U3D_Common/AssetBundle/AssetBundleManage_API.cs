/*******************************************************************
** 文件名:	AssetBundleManager_FBX.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	ab资源管理器
** 应  用:  	ab资源管理器，API部分

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
    

    #region 同步

    /// <summary>
    /// 不能获取预制体
    /// </summary>
    /// <param name="t"></param>
    /// <param name="conf"></param>
    /// <returns></returns>
    public static ResNode GetAssets(AssetType t, ResConfigData conf)
    {
        
        ResNode res = null;

        if (null == conf)
        {
            return res;
        }

        switch(t)
        {
            case AssetType.Asset_Prefab:
                UnionResConfigData unionCof = conf as UnionResConfigData;
                if(null == unionCof)
                {
                      Debug.LogWarning("预制体参数转换失败，请确认参数2是否是UnionResConfigData类型");
                    return null;
                }
                res = GetPrefab(unionCof);
                break;
            case AssetType.Asset_FBX:
                res = GetFBXResource(conf);
                break;
            case AssetType.Asset_AnimationClip:
                res = GetAnimationClipResource(conf);
                break;
            case AssetType.Asset_AnimatorController:
                res = GetAnimatorControllerResource(conf);
                break;
            case AssetType.Asset_Audio:
                res = GetAudioResource(conf);
                break;
            case AssetType.Asset_Font:
                res = GetFont(conf.AssetBundleName, conf.AssetName);
                break;
            case AssetType.Asset_Material:
                res = GetMaterialResource(conf);
                break;
            case AssetType.Asset_Texture:
                res = GetTextureResource(conf);
                break;
            case AssetType.Asset_Sprite:
                res = GetSprite(conf.AssetBundleName,conf.AssetName);
                break;
            default:
                Debug.LogWarning(StringHelper.BuildString("无效的资源类型:", t));
                break;
        }
        return res;
    }

    public static List<ResNode> GetAllAssetsFromPath(AssetType t,string path)
    {
        List<ResNode> reslut = null;
        if (null == path)
        {
            return null;
        }

        if (path.Equals(string.Empty))
        {
            return null;
        }

        switch (t)
        {
            case AssetType.Asset_Prefab:
                reslut = GetPrefabsFromPatch(path);
                break;
            default:
                Debug.LogWarning(StringHelper.BuildString("类型：" , t.ToString() , "不支持通过GetAllAssetsFromPath来获取"));
                break;
        }
        return null;
    }

    public static ResNode GetAssets(AssetType t, string assetBundleName, string assetName)
    {
        ResNode res = null;

        if (assetBundleName.Equals(string.Empty) || assetName.Equals(string.Empty))
        {
            return res;
        }

        switch (t)
        {
            case AssetType.Asset_Font:
                res = GetFont(assetBundleName, assetName);
                break;
            case AssetType.Asset_Sprite:
                res = GetSprite(assetBundleName, assetName);
                break;
            case AssetType.Asset_Prefab:
                res = GetPrefab(assetBundleName,assetName);
                break;
            default:
                Debug.LogWarning(StringHelper.BuildString("类型：" ,t.ToString() , "不支持通过assetbundlename和assetName来获取"));
                break;
        }
        return res;
    }

    public static ResNode GetAssets(AssetType t, string strParam,bool isGuid =false)
    {
        ResNode res = null;

        if(null == strParam)
        {
            return res;
        }

        if (strParam.Equals(string.Empty))
        {
            return res;
        }

        if(isGuid && t != AssetType.Asset_Prefab)
        {
            Debug.LogWarning(StringHelper.BuildString( "类型：" , t.ToString() , "不支持通过guid来获取"));
            return null;
        }
        switch (t)
        {
            case AssetType.Asset_Font:
                res = GetFont(strParam);
                break;
            case AssetType.Asset_Cursour:
                res = GetCursorResource(strParam);
                break;
            case AssetType.Asset_Material:
                res = GetMaterialResource(strParam);
                break;
            case AssetType.Asset_Prefab:
                res = GetPrefab(strParam, isGuid);
                break;
            default:
                Debug.LogWarning(StringHelper.BuildString( "类型：", t.ToString() , "不支持通过strParam来获取"));
                break;
        }
        return res;
    }

    public static void DeleteAssets(ref ResNode conf, bool Immediate)
    {
        if (null == conf || conf.assetType == AssetType.Asset_NotUse)
        {
            return;
        }
        //if(!conf.ResObj)
        //{
        //    return;
        //}
        switch (conf.assetType)
        {
            case AssetType.Asset_Prefab:
                DeletePrefab(ref conf, Immediate);
                return;
            case AssetType.Asset_FBX:
                DeleteFBXResource(ref conf, Immediate);
                return;
            case AssetType.Asset_AnimationClip:
                DeleteAnimationClipResource(ref conf, Immediate);
                return;
            case AssetType.Asset_AnimatorController:
                DeleteAnimatorControllerResource(ref conf, Immediate);
                return;
            case AssetType.Asset_Audio:
                DeleteAudioResource(ref conf, Immediate);
                return;
            case AssetType.Asset_Font:

                Debug.LogWarning("字体是通用资源，不能通过deleteasset删除");
                return;
            case AssetType.Asset_Material:
                DeleteMaterialResource(ref conf, Immediate);
                return;
            case AssetType.Asset_Texture:
                DeleteTextureResource(ref conf, Immediate);
                return;
            case AssetType.Asset_Sprite:
                DeleteSpriteResource(ref conf, Immediate);
                return;
            case AssetType.Asset_CommonAssets:
                return;
        }

        Debug.LogWarning("不支持资源类型的删除:" + conf.assetType.ToString());
    }
    #endregion

    #region 异步

    /// <summary>
    /// 本函数不能获取预制体和字体
    /// </summary>
    /// <param name="t"></param>
    /// <param name="conf"></param>
    /// <param name="callBack"></param>
    /// <param name="userDataObj"></param>
    /// <returns></returns>
    public static ResNode GetAssetsAsync(AssetType t, ResConfigData conf,
                                System.Action<ResConfigData, ResNode, System.Object> callBack,
                                System.Object userDataObj = null,
                                AssetLoadPriority priority = AssetLoadPriority.Priority_Normal)
    {
        ResNode res = null;

        if (null == conf)
        {
            return res;
        }

        switch (t)
        {
            case AssetType.Asset_Prefab:
                UnionResConfigData unionCof = conf as UnionResConfigData;
                if(null == unionCof)
                {
                      Debug.LogWarning("预制体参数转换失败，请确认参数2是否是UnionResConfigData类型");
                    return null;
                }
                return GetPrefabAsync(unionCof, callBack, userDataObj, priority);
            case AssetType.Asset_FBX:
                return GetFBXResourceAsync(conf, callBack, userDataObj, priority);
            case AssetType.Asset_AnimationClip:
                return GetAnimationClipResourceAsync(conf, callBack, userDataObj, priority);
            case AssetType.Asset_AnimatorController:
                return GetAnimatorControllerResourceAsync(conf, callBack, userDataObj, priority);
            case AssetType.Asset_Audio:
                return GetAudioResourceAsync(conf, callBack, userDataObj, priority);
            case AssetType.Asset_Font:
                Debug.LogWarning("字体属于通用资源，不支持异步加载");
                return res;
            case AssetType.Asset_Material:
                return GetMaterialResourceAsync(conf, callBack, userDataObj, priority);
            case AssetType.Asset_Texture:
                return GetTextureResourceAsync(conf, callBack, userDataObj, priority);
            case AssetType.Asset_Sprite:
                return GetSpriteAsync(conf.AssetBundleName, conf.AssetName, callBack, userDataObj, priority);
        }
        Debug.LogWarning("无效的资源类型:" + t.ToString());
        return res;
    }

   public static ResNode GetAssetsAsync(AssetType t, ResConfigData conf,
                                IAsyncResLoadHandler callBack,
                                System.Object userDataObj = null,
                                AssetLoadPriority priority = AssetLoadPriority.Priority_Normal)
    {
        return GetAssetsAsync(t, conf, callBack.ResLoadCallBack, userDataObj, priority);
    }
    public static ResNode GetAssetsAsync(AssetType t, string assetBundleName, string assetName, 
                             System.Action<ResConfigData, ResNode, System.Object> callBack,
                             System.Object userDataObj = null,
                             AssetLoadPriority priority = AssetLoadPriority.Priority_Normal)
    {
        ResNode res = null;

        if (assetBundleName.Equals(string.Empty) || assetName.Equals(string.Empty))
        {
            return res;
        }

        switch (t)
        {
            case AssetType.Asset_Sprite:
                return GetSpriteAsync(assetBundleName, assetName, callBack, userDataObj, priority);
            case AssetType.Asset_Prefab:
                return GetPrefabAsync(assetBundleName, assetName, callBack, userDataObj, priority);
        }
        Debug.LogWarning("类型：" + t.ToString() + "不支持通过assetbundlename和assetName来获取");
        return res;
    }
   public static ResNode GetAssetsAsync(AssetType t, string assetBundleName, string assetName,
                             IAsyncResLoadHandler callBack,
                             System.Object userDataObj = null,
                             AssetLoadPriority priority = AssetLoadPriority.Priority_Normal)
    {
        return GetAssetsAsync(t, assetBundleName, assetName, callBack.ResLoadCallBack, userDataObj, priority);
    }
   public static ResNode GetAssetsAsync(AssetType t, System.Action<ResConfigData, ResNode, System.Object> callBack,
                                        string strParam, bool isGuid = false, System.Object userDataObj = null, AssetLoadPriority priority = AssetLoadPriority.Priority_Normal)
    {
        ResNode res = null;

        if (null == strParam)
        {
            return res;
        }

        if (strParam.Equals(string.Empty))
        {
            return res;
        }

        if (isGuid && t != AssetType.Asset_Prefab)
        {
            Debug.LogWarning("类型：" + t.ToString() + "不支持通过guid来获取");
            return null;
        }

        
        switch (t)
        {
            case AssetType.Asset_Prefab:
                res = GetPrefabAsync(strParam, isGuid, callBack, userDataObj, priority);
                return res;
        }
        Debug.LogWarning("类型：" + t.ToString() + "不支持通过assetbundlename和assetName来获取");
        return res;
    }

    public static ResNode GetAssetsAsync(AssetType t, IAsyncResLoadHandler callBack,
                                        string strParam, bool isGuid = false, System.Object userDataObj = null, AssetLoadPriority priority = AssetLoadPriority.Priority_Normal)
    {
        return GetAssetsAsync(t, callBack.ResLoadCallBack, strParam, isGuid, userDataObj, priority);
    }
    public static void DeleteAssetsAsync(ref ResNode conf,
                                System.Action<ResConfigData, ResNode, System.Object> callBack, bool Immediate)
    {
        if (null == conf || conf.assetType == AssetType.Asset_NotUse)
        {
            return;
        }
        //if (!conf.ResObj)
        //{
        //    return;
        //}
        switch (conf.assetType)
        {
            case AssetType.Asset_Prefab:
                DeletePrefabAsync(ref conf, callBack, Immediate);
                return;
            case AssetType.Asset_FBX:
                DeleteFBXResourceAsync(ref conf, callBack, Immediate);
                return;
            case AssetType.Asset_AnimationClip:
                DeleteAnimationClipResourceAsync(ref conf, callBack, Immediate);
                return;
            case AssetType.Asset_AnimatorController:
                DeleteAnimatorControllerResourceAsync(ref conf, callBack, Immediate);
                return;
            case AssetType.Asset_Audio:
                DeleteAudioResourceAsync(ref conf, callBack, Immediate);
                return;
            case AssetType.Asset_Font:

                Debug.LogWarning("字体是通用资源，不能通过deleteassetAsync删除");
                return;
            case AssetType.Asset_Material:
                DeleteMaterialResourceAsync(ref conf, callBack, Immediate);
                return;
            case AssetType.Asset_Texture:
                DeleteTextureResourceAsync(ref conf, callBack, Immediate);
                return;
            case AssetType.Asset_Sprite:
                DeleteSpriteResourceAsync(ref conf, callBack, Immediate);
                return;
            case AssetType.Asset_CommonAssets:
                return;
        }

        Debug.LogWarning("不支持资源类型的删除:" + conf.assetType.ToString());
    }

    public static void DeleteAssetsAsync(ref ResNode conf,
                            IAsyncResLoadHandler callBack, bool Immediate)
    {
        DeleteAssetsAsync(ref conf, callBack.ResLoadCallBack, Immediate);
    }
    #endregion
}
