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
/// 配置的导入
/// </summary>
public partial class AssetBundleImporter : AssetPostprocessor
{

    public static AssetBundleManagerEditor.AssetBundleEditorInfo MakeSchemeAssetBundleInfo(string patch)
    {
        AssetBundleManagerEditor.AssetBundleEditorInfo info = null;
        AssetImporter assetImporter = AssetImporter.GetAtPath(patch);
        if (!assetImporter)
        {
            return info;
        }

        string asPatch = assetImporter.assetPath;

        //不是在这个路径下的,不是场景文件，不处理
        if (!isSchemeAssets(asPatch))
        {
            return info;
        }

        //移除头部
        asPatch = asPatch.Replace(SchemePatch, "");

        string[] strs = asPatch.Split('/');

        //没有放到其他文件夹下，则使用默认的包
        if (strs.Length <= 1)
        {
            asPatch = "Default";
        }
        else
        {
            //移除名字
            asPatch = asPatch.Replace(strs[strs.Length - 1], "");

            //再移除最后的斜杠
            asPatch = asPatch.Remove(asPatch.Length - 1);
        }

        string assetName = Path.GetFileNameWithoutExtension(assetImporter.assetPath);
        string guid = AssetBundleManagerEditor.GetAssetGUIDFromMeta(assetImporter.assetPath);

        if (guid.Equals(string.Empty))
        {
            return info;
        }

        //最后就是包名
        string abName = AssetBundleManager.SchemeVariant + "/" + asPatch;
        string variant = AssetBundleManager.SchemeVariant;

        SaveAssetBundleData(assetImporter, abName, variant);

        info = new AssetBundleManagerEditor.AssetBundleEditorInfo();
        info.AssetBundleName = abName;
        info.AssetBundleVarian = variant;
        info.guid = guid;
        info.assetName = assetName;

        DispatchScheme(patch);
        return info;


    }

    public static void DispatchScheme(string path)
    {
        if(isLightingEffectSchemeAssets(path))
        {
            OnImportLightingEffectSceheme(path);
        }
        
    }

    static void OnPreProcessScheme()
    {
        if(Application.isPlaying)
        {
            return;
        }
    }

    public static void OnPostProcessScheme()
    {
        if (Application.isPlaying)
        {
            return;
        }
    }

    private static bool isSchemeAssets(string s)
    {
        //不是在这个路径下的,不处理
        if (!s.Contains(SchemePatch))
        {
            return false;
        }

        if (!s.Contains("."))
        {
            return false;
        }
        return true;
    }
}
