/*******************************************************************
** 文件名:	AssetBundleImporter.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	ab资源管理
** 应  用:  	ab资源导入

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using UnityEditor;
using System.Collections.Generic;
using SoundSystem;
using System.IO;

/// <summary>
/// 资源导入导入
/// </summary>
public partial class AssetBundleImporter : AssetPostprocessor
{

    public static AssetBundleManagerEditor.AssetBundleEditorInfo MakeSceneAssetBundleInfoFromReference(string patch)
    {
        AssetBundleManagerEditor.AssetBundleEditorInfo info = AssetBundleManagerEditor.GetAssetBundleEditorInfo(patch);
        
        if (null != info)
        {
            return info;
        }

        info = MakeSceneAssetBundleInfo(patch);
        return info;
    }

    private static AssetBundleManagerEditor.AssetBundleEditorInfo MakeSceneAssetBundleInfo(string patch)
    {
        AssetBundleManagerEditor.AssetBundleEditorInfo info = null;
        AssetImporter assetImporter = AssetImporter.GetAtPath(patch);
        if (!assetImporter)
        {
            return info;
        }

        string asPatch = assetImporter.assetPath;
        //不是在这个路径下的,不是场景文件，不处理
        if (!isSceneAssets(asPatch))
        {
            return info;
        }

        //场景文件是一个文件一个包，所以去掉头，然后去调后缀名就是包了
        //移除头部
        asPatch = asPatch.Replace(scenePatch, "");
        //就去掉后缀名
        asPatch = asPatch.Replace(".unity", "");

        string abName = "Scene/" + asPatch;
        string variant = AssetBundleManager.sceneVariant;

        string guid = AssetBundleManagerEditor.GetAssetGUIDFromMeta(assetImporter.assetPath);

        if (guid.Equals(string.Empty))
        {
            return info;
        }
        string assetName = Path.GetFileNameWithoutExtension(assetImporter.assetPath);
        SaveAssetBundleData(assetImporter, abName,variant);

        info = new AssetBundleManagerEditor.AssetBundleEditorInfo();
        info.AssetBundleName = abName;
        info.AssetBundleVarian = variant;
        info.guid = guid;
        info.assetName = assetName;
        return info;
    }

    private static bool isSceneAssets(string s)
    {
        //不是在这个路径下的,不是场景文件，不处理
        if (!s.Contains(scenePatch) || !s.EndsWith(".unity"))
        {
            return false;
        }

        //if (s.Contains(DebugscenePatch))
        //{
        //    return false;
        //}
        return true;
    }
}
