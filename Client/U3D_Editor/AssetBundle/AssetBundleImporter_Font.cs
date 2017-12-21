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
    /// 字体资源AB
    /// </summary>
    /// <param name="assetImporter"></param>
    private static void MakeFontBundleData(AssetImporter assetImporter)
    {
         /*字体只依赖贴图和Mat，把他们连都打包在字体的ab里面，不用单独做成一个AB包
         * 因为字体之间的资源是不会公用的,只需要检查一下合法性就可以了
          * 1.资源必须在Fonts目录
          * 2.资源必须跟fonts文件在同级目录
          * 3.只能引用材质、贴图、shader资源
         */
        string patch = assetImporter.assetPath;
        List<string> dependeciesList = new List<string>(AssetDatabase.GetDependencies(new string[] { patch }));
        dependeciesList.Remove(patch);

        string patchFolder = System.IO.Path.GetDirectoryName(patch);
        bool isOk = true;
        foreach (string s in dependeciesList)
        {
            string lowerString = s;
            if (IsTextureAsset(s))
            {
                if (!lowerString.Contains(FontPatch))
                {
                    Debug.LogError("字体:" + patch + "引用了Fonts以外的资源，请修！:" + s);
                    isOk = false;
                    continue;
                }

                string folder = System.IO.Path.GetDirectoryName(s);
                if(!folder.Equals(patchFolder))
                {
                    Debug.LogError("字体引用的资源:" + s + "与字体不在同一目录，字体目录:" + patchFolder + ",引用目录:" + folder);
                    isOk = false;
                    continue;
                }
            }
            else if(IsMaterialAsset(s))
            {
                if (!lowerString.Contains(FontPatch))
                {
                    Debug.LogError("字体:" + patch + "引用了Fonts以外的资源，请修！:" + s);
                    isOk = false;
                    continue;
                }

                string folder = System.IO.Path.GetDirectoryName(s);
                if (!folder.Equals(patchFolder))
                {
                    Debug.LogError("字体引用的资源:" + s + "与字体不在同一目录，字体目录:" + patchFolder + ",引用目录:" + folder);
                    isOk = false;
                    continue;
                }
            }
            else if(s.Contains(".shader") && s.Contains(".cginc"))
            {

            }
            else
            {
                Debug.LogError("字体:" + patch + "引用了非材质、贴图、shader资源，请修:" + s);
                isOk = false;
            }
        }

        if(isOk)
        {
            MakeFontAssetBundleData(assetImporter);
        }
        
    }

    private static void MakeFontAssetBundleData(AssetImporter assetImporter)
    {
        if (!assetImporter)
        {
            return;
        }

        string asPatch = assetImporter.assetPath;

        //不是在这个路径下的，不处理
        if (!asPatch.Contains(FontPatch))
        {
            return;
        }
        //移除头部
        asPatch = asPatch.Replace(FontPatch, "");

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

        string abName = "Font/" + asPatch;
        string variant = AssetBundleManager.FontAssetVariant;
        SaveAssetBundleData(assetImporter, abName, variant);
    }


    private static bool IsFontAsset(string patch)
    {
        AssetImporter ap = AssetImporter.GetAtPath(patch);
        if (!ap)
        {
            return false;
        }

        if(!patch.Contains(FontPatch))
        {
            return false;
        }

        if (ap.GetType() == typeof(TrueTypeFontImporter) ||
            patch.EndsWith(".fontsettings"))
        {
            return true;
        }

        return false;
    }
}
