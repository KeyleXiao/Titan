/*******************************************************************
** 文件名:	AssetBundleImporter_Shader.cs
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

    private static void MakeMeshAssetBundle(string patch)
    {
        AssetImporter assetImporter = AssetImporter.GetAtPath(patch);
        if (!assetImporter)
        {
            return;
        }

        string asPatch = assetImporter.assetPath;

        if (!isMeshAssets(asPatch))
        {
            return;
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
        string abName = "Mesh/" + asPatch;
        string variant = AssetBundleManager.MeshAssetVariant;
        SaveAssetBundleData(assetImporter, abName, variant);
    }

    private static bool isMeshAssets(string s)
    {
        string asPatch = s;
        //不是在这个路径下的，不处理
        if (!asPatch.Contains(MeshPath))
        {
            return false;
        }

        if (!asPatch.EndsWith(".asset"))
        {
            return false;
        }

        return true;
    }
}
