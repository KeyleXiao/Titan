/*******************************************************************
** 文件名:	AssetBundleManager.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	ab资源管理器
** 应  用:  	ab资源管理器

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

public sealed partial class AssetBundleManager : MonoBehaviour
{
 
    /// <summary>
    /// 节约内存,别乱调用。
    /// </summary>
    /// <param name="res"></param>
    public static void SaveMemory(ResNode res)
    {
        if(!isOnRunTimeMode)
        {
            return;
        }
        if(ResNode.isNullOrEmpty(res))
        {
            return;
        }
        if (res.assetType != AssetType.Asset_Prefab)
        {
            return;
        }

        res.SaveMemory();
    }

    private static void ReleaseAssetBundleIfNeed(ResNode res)
    {
        if(!isOnRunTimeMode)
        {
            return;
        }
        if(res.assetType == AssetType.Asset_FBX)
        {
            return;
        }
        if (res.assetType != AssetType.Asset_Prefab  && !IsUnionDependenicesAssetBundle(res.AssetBundleName))
        {
            return;
        }

        if(res.isBeReleaseAssetBundleIfNeed)
        {
            return;
        }
        res.isBeReleaseAssetBundleIfNeed = true;
        ResourcePackage pkg = AllocResourcePackage(res.AssetBundleName);
        if (m_LoadedResourcePackage.Contains(pkg.instanceID))
        {
            int refcout = pkg.Release();
            if (refcout <= 0)
            {
                UnLoadAssetBundle(pkg.instanceID, false);
            }
        }
    }

}
