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
 

    private static bool IsMaterialAsset(string patch)
    {
        AssetImporter ap = AssetImporter.GetAtPath(patch);
        if (!ap)
        {
            return false;
        }
        string asPatch = patch;
        if(asPatch.Contains("xiaoqianghenshuai"))
        {
            return false;
        }
        if (!asPatch.Contains(ArtitsPatch) && !asPatch.Contains(FontPatch))
        {
            return false;
        }
        if (!asPatch.EndsWith(".mat"))
        {
            return false;
        }
        return true;
    }

    private static AssetBundleManagerEditor.AssetBundleEditorInfo BuildMaterialAssetBundleInfo(AssetImporter assetImporter,string referenceAssetbundleName)
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
        if (!IsMaterialAsset(asPatch))
        {
            return null;
        }

        //移除头部
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


        //最后就是包名
        AbName = "Material/" + asPatch;
        AbVirtant = AssetBundleManager.MatVariant;

        if (AbName.Equals(string.Empty) || AbVirtant.Equals(string.Empty))
        {
            return null;
        }

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


    /// <summary>
    /// 构建ab，原因是被需要的东西引用了
    /// </summary>
    /// <param name="path"></param>
    /// <param name="referenceAssetbundleName">参考名，如果为空，则会使用默认的生成方式</param>
    /// <param name="referenceVariant">参考名，如果为空，则会使用默认的生成方式<</param>
    /// <returns></returns>
    public static ResConfigData MakeMaterialAssetBundleFromReference(string path,string referenceAssetbundleName)
    {
        ResConfigData conf = null;
        if (path.Equals(string.Empty))
        {
            return conf;
        }

        AssetBundleManagerEditor.AssetBundleEditorInfo info = null;
        AssetImporter assetImporter = AssetImporter.GetAtPath(path);
        if (!assetImporter)
        {
            return conf;
        }
        if (!IsCanGeneratePrefabUnionRes(path))
        {
            referenceAssetbundleName = string.Empty;
        }
        //然后刷一下名字
        info = BuildMaterialAssetBundleInfo(assetImporter, referenceAssetbundleName);
        //如果还是取不到，就说明不是贴图了。
        if (info == null)
        {
            return conf;
        }
        if (!assetImporter.userData.Contains(FromReferencebKey))
        {
            assetImporter.userData += FromReferencebKey;
        }
        assetImporter.userData += FromReferencebKey;
        //没有在打包的时候，不需要写入ab包，因为编辑器不走ab包
        if (AssetBundleManagerEditor.isBuildingPackage)
        {
            PushPrefabAssetImport(assetImporter, info);
            //SaveAssetBundleData(assetImporter, info);
        }
        return info.ConvertToResConfigData();
    }

    static void MakeMaterialAssetBundle(string patch)
    {
        AssetImporter assetImporter = AssetImporter.GetAtPath(patch);
        if (!assetImporter)
        {
            return;
        }

        string asPatch = patch;
        if (!IsMaterialAsset(asPatch))
        {
            return;
        }

        /*
        * 只有Artist目录下的Mat才会被打包
        * 一般情况下,FBX的包是通过预制体来生成包的，但是也有一种例外
        * 就是在通用资源(Artist/CommonArtist)下的fbx，这些是必须要打包的。
        */

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

        AssetBundleManagerEditor.AssetBundleEditorInfo info = BuildMaterialAssetBundleInfo(assetImporter, string.Empty);
        if (info == null)
        {
            return;
        }
        SaveAssetBundleData(assetImporter, info);
    }
}
