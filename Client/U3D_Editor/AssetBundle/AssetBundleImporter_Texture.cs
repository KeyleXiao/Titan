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

    void OnPreprocessTexture()
    {
        TextureImporter timp = assetImporter as TextureImporter;
        if(timp.assetPath.Contains(SceneSkyBoxPatch))
        {
            timp.maxTextureSize = 1024;
        }
    }

    static string TextureInfoBegin = "[TEX-";
    static string TextureInfoEnd = "-TEX]";

    void OnPostprocessTexture(Texture2D texture)
    {
        //TextureImporter importer = assetImporter as TextureImporter;
        //if(texture.width >= 1024 || texture.height >= 1024)
        //{
        //    importer.textureType = TextureImporterType.Advanced;
        //    importer.mipmapEnabled = false;
        //    importer.isReadable = false;
        //}

        string texturtInfo;
        int w, h;
        string textureInfoOld;
        texturtInfo = ConvertTextureInfo(texture.width, texture.height);

        if (GetTextureInfo(assetImporter, out w, out h))
        {
            textureInfoOld = ConvertTextureInfo(w, h);
            if (!textureInfoOld.Equals(texturtInfo))
            {
                assetImporter.userData = assetImporter.userData.Replace(textureInfoOld, string.Empty);
            }
        }

        if (!assetImporter.userData.Contains(texturtInfo))
        {
            assetImporter.userData += texturtInfo;
        }
        //SaveAssetBundleUserData(assetImporter, assetImporter.userData);
    }

    private static string ConvertTextureInfo(int width,int height)
    {
        return TextureInfoBegin + width + "," + height + TextureInfoEnd;
    }

    private static bool GetTextureInfo(AssetImporter assetImporter, out int width, out int height)
    {
        width = 0;
        height = 0;
        string filePatch = AssetBundleManagerEditor.ConverRelativePatchToObslotePatch(assetImporter.assetPath);
        filePatch += ".meta";

        if (!File.Exists(filePatch))
        {
            return false;
        }

        //CubeMap特殊处理一下
        if(IsCubeMap(assetImporter.assetPath))
        {
            width = 2048;
            height = width;
            return true;
        }

        string userData = assetImporter.userData;
        //if (!userData.Contains(TextureInfoBegin))
        //{
            //string text = System.IO.File.ReadAllText(filePatch);
            //List<string> alltext = new List<string>();
            //alltext.AddRange(text.Split('\n'));
            //string strAbVar = "  userData: ";
            //for (int i = alltext.Count - 1; i >= 0; i--)
            //{
            //    string s = alltext[i];
            //    if (s.Contains(strAbVar))
            //    {
            //        userData = s.Replace(strAbVar, string.Empty);
            //    }
            //}
        //}

        if (!userData.Contains(TextureInfoBegin))
        {
            return false;
        }
        int begin = userData.IndexOf(TextureInfoBegin);
        int end = userData.IndexOf(TextureInfoEnd);

        string reslut = userData.Substring(begin, end - begin);
        reslut = reslut.Replace(TextureInfoBegin, string.Empty);
        reslut = reslut.Replace(TextureInfoEnd, string.Empty);
        string[] str = reslut.Split(',');
        bool re = int.TryParse(str[0], out width);
        re = re && int.TryParse(str[1], out height);
        return re;
    }


    /// <summary>
    /// 构建ab，原因是被需要的东西引用了
    /// </summary>
    /// <param name="path"></param>
    /// <param name="referenceAssetbundleName">参考名，如果为空，则会使用默认的生成方式</param>
    /// <param name="referenceVariant">参考名，如果为空，则会使用默认的生成方式<</param>
    /// <returns></returns>
    public static ResConfigData MakeTextureAssetBundleFromReference(string path,string referenceAssetbundleName)
    {
        ResConfigData conf = null;
        if(path.Equals(string.Empty))
        {
            return conf;
        }
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
        AssetBundleManagerEditor.AssetBundleEditorInfo info = BuildTextureAssetBundleInfo(assetImporter,referenceAssetbundleName);
        //如果还是取不到，就说明不是贴图了。
        if (info == null)
        {
            return conf;
        }
        if (!assetImporter.userData.Contains(FromReferencebKey))
        {
            assetImporter.userData += FromReferencebKey;
        }
        //没有在打包的时候，不需要写入ab包，因为编辑器不走ab包
        if (AssetBundleManagerEditor.isBuildingPackage)
        {
            PushPrefabAssetImport(assetImporter, info);
            //SaveAssetBundleData(assetImporter, info);
        }

        conf = info.ConvertToResConfigData();
        return conf;
    }

    private static AssetBundleManagerEditor.AssetBundleEditorInfo BuildTextureAssetBundleInfo(AssetImporter assetImporter,string referenceAssetbundleName)
    {
        string AbName = string.Empty;
        string AbVirtant = string.Empty;
        string asPatch = assetImporter.assetPath;
        bool isSprite = IsSpriteAssets(asPatch);
        bool isHaveSpritePacking = false;
        if(isSprite)
        {
            TextureImporter tp = assetImporter as TextureImporter;
            isHaveSpritePacking = IsHaveSpritePacking(tp);

            if (isHaveSpritePacking)
            {
                BuildTextureAssetBundleName_SpritePacking(tp.spritePackingTag, out AbName, out AbVirtant, true);
            }
            else
            {
                BuildTextureAssetBundleName_Normal(asPatch, out AbName, out AbVirtant, true, referenceAssetbundleName);
            }
        }
        else
        {
            BuildTextureAssetBundleName_Normal(asPatch, out AbName, out AbVirtant, false, referenceAssetbundleName);
        }

        if (AbName.Equals(string.Empty) || AbVirtant.Equals(string.Empty))
        {
            return null;
        }

        string assetName = Path.GetFileNameWithoutExtension(asPatch);
        string assetGUID = AssetBundleManagerEditor.GetAssetGUIDFromMeta(asPatch);

        if (assetName.Equals(string.Empty) || assetGUID.Equals(string.Empty))
        {
            return null;
        }
        AssetBundleManagerEditor.AssetBundleEditorInfo info = new AssetBundleManagerEditor.AssetBundleEditorInfo();
        info.AssetBundleName = AbName;
        info.AssetBundleVarian = AbVirtant;
        info.guid = assetGUID;
        info.assetName = assetName;
        info.Asset_X = -1;
        info.Asset_Y = -1;
        if (!isHaveSpritePacking)
        {
            int w, h;
            if (!GetTextureInfo(assetImporter, out w, out h))
            {
                Debug.LogWarning("贴图信息读取失败，请重新导入资源:" + assetImporter.assetPath);
                info.Asset_X = 2048;
                info.Asset_Y = 2048;
            }
            else
            {
                info.Asset_X = w;
                info.Asset_Y = h;
            }
        }
        else
        {
            //info.Asset_X = 3535;//特殊含义，表示这个贴图有图集,因为图集会分多个group，在加载之前是不知道属于哪一个group的
            //info.Asset_Y = 5353;

            info.Asset_X = 2048;
            info.Asset_Y = 2048;
        }
        return info;
    }

    private static void BuildTextureAssetBundleName_SpritePacking(string path, out string AbName, out string AbVirtant,bool UIAssets)
    {
        AbName = string.Empty;
        AbVirtant = string.Empty;

        string asPatch = path.Replace("@","/");

        //最后就是包名
        AbName = "Texture/UI/" + asPatch;

        AbVirtant = AssetBundleManager.textureVariant;
        if (UIAssets)
        {
            AbVirtant = AssetBundleManager.UIVariant;
        }
    }

    private static void BuildTextureAssetBundleName_Normal(string path, out string AbName, out string AbVirtant, bool UIAssets, string referenceAssetbundleName)
    {
        AbName = string.Empty;
        AbVirtant = string.Empty;

        string asPatch = path;
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
        AbName = "Texture/" + asPatch;
        if (!referenceAssetbundleName.Equals(string.Empty))
        {
            AbName = referenceAssetbundleName;
        }
        AbVirtant = AssetBundleManager.textureVariant;
        if (UIAssets)
        {
            AbVirtant = AssetBundleManager.UIVariant;
        }
    }

    private static void MakeTextureAssetBundle(string path)
    {
        AssetImporter assetImporter = AssetImporter.GetAtPath(path);
        if (!assetImporter)
        {
            return;
        }

        string asPatch = assetImporter.assetPath;

        //不是在这个路径下的，不处理
        if (!IsTextureAsset(asPatch))
        {
            return;
        }
        bool bFromPrefab = assetImporter.userData.Contains(FromReferencebKey);
        assetImporter.userData = assetImporter.userData.Replace(FromReferencebKey, "");

        if(bFromPrefab)
        {
            SaveAssetBundleUserData(assetImporter, assetImporter.userData);
            return;
        }

        /*
            * 不是来自于Prefab的，自动生成ab包的，只有UI动态目录，通用资源目录
            * 不是这两个目录的，只能通过prefab来生成ab包
        */
        if (!IsUIDynamicAssets (asPatch) && !IsCommonArtistAssets(asPatch))
        {
            return;
        }


        AssetBundleManagerEditor.AssetBundleEditorInfo info = BuildTextureAssetBundleInfo(assetImporter, string.Empty);
        if (info == null)
        {
            return;
        }
        SaveAssetBundleData(assetImporter, info);
    }




    private static bool IsTextureAsset(string patch)
    {
        string asPatch = patch;
        /*cubemap特殊处理一下*/
        if(patch.Contains(".cubemap"))
        {
            if (!asPatch.Contains(ArtitsPatch) && !asPatch.Contains(FontPatch))
            {
                return false;
            }
            return true;
        }

        TextureImporter ap = AssetImporter.GetAtPath(patch) as TextureImporter;
        if (!ap)
        {
            return false;
        }
        asPatch = ap.assetPath;
        //不是在这个路径下的，不处理
        if (!asPatch.Contains(ArtitsPatch) && !asPatch.Contains(FontPatch))
        {
            return false;
        }
        
        return true;
    }

    private static bool IsCubeMap(string path)
    {
        /*cubemap特殊处理一下*/
        if (path.Contains(".cubemap"))
        {
            if (!path.Contains(ArtitsPatch) && !path.Contains(FontPatch))
            {
                return false;
            }
            return true;
        }
        return false;

    }

    private static bool IsSpriteAssets(string path)
    {
        return path.Contains(UIAssetPatch) || path.Contains(BuiltInUIAssetPatch);
    }

    private static bool IsHaveSpritePacking(TextureImporter tp)
    {
        if(!tp)
        {
            return false;
        }
        return !tp.spritePackingTag.Equals(string.Empty);
    }
}
