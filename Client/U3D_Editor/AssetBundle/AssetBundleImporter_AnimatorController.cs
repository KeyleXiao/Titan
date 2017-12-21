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

    /// <summary>
    /// 构建ab，原因是被需要的东西引用了
    /// </summary>
    /// <param name="path"></param>
    /// <param name="referenceAssetbundleName">参考名，如果为空，则会使用默认的生成方式</param>
    /// <param name="referenceVariant">参考名，如果为空，则会使用默认的生成方式<</param>
    /// <returns></returns>
    public static ResConfigData MakeAnimatorControllerAssetBundleFromReference(string path,string referenceAssetbundleName)
    {
        AssetBundleManagerEditor.AssetBundleEditorInfo conf = null;
        if (path.Equals(string.Empty))
        {
            return null;
        }

        AssetImporter assetImporter = AssetImporter.GetAtPath(path);
        if (!assetImporter)
        {
            return null;
        }
        if (!IsCanGeneratePrefabUnionRes(path))
        {
            referenceAssetbundleName = string.Empty;
        }

        //然后刷一下名字
        conf = BuildAnimatorControllerAssetBundleInfo(assetImporter, referenceAssetbundleName);
        //如果还是取不到，就说明不是贴图了。
        if (conf == null)
        {
            return null;
        }
        if (!assetImporter.userData.Contains(FromReferencebKey))
        {
            assetImporter.userData += FromReferencebKey;
        }
        //没有在打包的时候，不需要写入ab包，因为编辑器不走ab包
        if (AssetBundleManagerEditor.isBuildingPackage)
        {
            PushPrefabAssetImport(assetImporter, conf);
            //SaveAssetBundleData(assetImporter, conf);
        }
        return conf.ConvertToResConfigData();
    }

    private static AssetBundleManagerEditor.AssetBundleEditorInfo BuildAnimatorControllerAssetBundleInfo(AssetImporter assetImporter, string referenceAssetbundleName)
    {
        string AbName = string.Empty;
        string AbVirtant = string.Empty;
        if (!assetImporter)
        {
            return null;
        }

        if (!CheckAssetDependices(assetImporter.assetPath))
        {
            return null;
        }

        string asPatch = assetImporter.assetPath;

        if (!IsControllerAsset(asPatch))
        {
            return null;
        }

        //区分一下是模型还是特效
        string patchHead = string.Empty;
        if (asPatch.Contains(ModelAnimatorControllerPatch))
        {
            patchHead = "model/";
        }

        //移除头部
        asPatch = asPatch.Replace(ModelAnimatorControllerPatch, "");
        asPatch = asPatch.Replace(ArtitsPatch, "");
        

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
        AbName = "animcontroller/" + patchHead + asPatch;
        AbVirtant = AssetBundleManager.AnimatorControllerVariant;

        string assetName = Path.GetFileNameWithoutExtension(assetImporter.assetPath);
        string assetGUID = AssetBundleManagerEditor.GetAssetGUIDFromMeta(assetImporter.assetPath);

        if (assetName.Equals(string.Empty) || assetGUID.Equals(string.Empty))
        {
            return null;
        }
        if (!referenceAssetbundleName.Equals(string.Empty))
        {
            AbName = referenceAssetbundleName;
        }
        AssetBundleManagerEditor.AssetBundleEditorInfo info = new AssetBundleManagerEditor.AssetBundleEditorInfo();
        info.AssetBundleName = AbName;
        info.AssetBundleVarian = AbVirtant;
        info.guid = assetGUID;
        info.assetName = assetName;
        return info;
    }

    private static void MakeAnimatorControllerAssetBundleData(string patch)
    {
        AssetImporter assetImporter = AssetImporter.GetAtPath(patch);
        if (!assetImporter)
        {
            return;
        }

        string asPatch = assetImporter.assetPath;

        //不是来自于prefab的，再commonartist目录下才可以生成AB名
        bool bFromPrefab = assetImporter.userData.Contains(FromReferencebKey);
        assetImporter.userData = assetImporter.userData.Replace(FromReferencebKey, "");

        if (bFromPrefab)
        {
            SaveAssetBundleUserData(assetImporter, assetImporter.userData);
            return;
        }

        if (!IsCommonArtistAssets(asPatch))
        {
            return;
        }

        AssetBundleManagerEditor.AssetBundleEditorInfo info = BuildAnimatorControllerAssetBundleInfo(assetImporter, string.Empty);
        if (info == null)
        {
            return;
        }
        SaveAssetBundleData(assetImporter, info);

    }

    private static bool IsControllerAsset(string patch)
    {
        string asPatch = patch;
        //不是在这个路径下的，不处理
        if (!asPatch.Contains(ModelAnimatorControllerPatch) && !asPatch.Contains(ArtitsPatch))
        {
            return false;
        }

        //不是controller
        if (!asPatch.EndsWith(".controller"))
        {
            return false ;
        }

        return true;
    }
}
