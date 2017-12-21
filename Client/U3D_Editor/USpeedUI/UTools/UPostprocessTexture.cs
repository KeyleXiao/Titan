/*******************************************************************
** 文件名:	UPostprocessTexture.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	林建聪
** 日  期:	2016/3/28
** 版  本:	1.0
** 描  述:	更改导入到\Assets\Artist\UI 下资源图片的属性
** 应  用:  
 *          
**************************** 修改记录 ******************************
** 修改人: 林建聪
** 日  期: 2016.4.27
** 描  述: 图集规则：
            1、UI/Common 一个图集
            2、UI/Dynamic/Atlases/ 每个子文件夹一个图集
            3、UI/Dynamic/NonAtlases/ 不弄图集
            4、UI/View/ 每个子文件夹一个图集
********************************************************************/
using UnityEngine;
using UnityEditor;
using System.IO;
using System.Linq;
using System.Collections.Generic;

class UPostprocessTexture: AssetPostprocessor
{
    //UI图素资源父目录
    public  const string UIRootPath = "Assets/Artist/UI";
    public const string CommonPath = "Common";
    public const string DynamicPath = "Dynamic";
    public const string ViewPath = "View";
    public const string AtlasPath = "Atlases";
    public const string NonAtlasesPath = "NonAtlases";
    public const string RwPath = "RWUI";
    public const char PathConnectionSymbol = '/';
    public const string AtlasConnectionSymbol = "@";

    void OnPreprocessTexture()
    {
       // Debug.Log("OnPreprocessTexture");
    }

    void OnPostprocessTexture(Texture2D texture)
    {
        //非UI的不处理
        string path = Path.GetDirectoryName(assetPath);
        if (!path.Contains(UIRootPath))
            return;

        TextureImporter textureImporter = assetImporter as TextureImporter;
        textureImporter.textureType = TextureImporterType.Sprite;
        if (path.Contains(RwPath))
        {
            textureImporter.textureType = TextureImporterType.Advanced;
            textureImporter.isReadable = true;
        }
        
        textureImporter.spritePackingTag = GetAtlasName(path);
        textureImporter.mipmapEnabled = false;
        textureImporter.npotScale = TextureImporterNPOTScale.None;
    }

    string GetAtlasName(string strPath) 
    {
        if(strPath.Contains(CommonPath))
        {
            return CommonPath;
        }
        else if(strPath.Contains(DynamicPath+PathConnectionSymbol+AtlasPath))
        {
            List<string> name = strPath.Split(PathConnectionSymbol).ToList();
            //动态需要图集的资源以文件夹命名图集
            return GetAtlasName(name, nEndIndex: name.Count) ;//name[name.IndexOf(AtlasPath) + 1];
        }
        else if(strPath.Contains(ViewPath))
        {
            List<string> name = strPath.Split(PathConnectionSymbol).ToList();
            //View的以一个大的文件夹命名
            return GetAtlasName(name, 3, 5);

        }
        else if(strPath.Contains(DynamicPath+ PathConnectionSymbol + NonAtlasesPath))
        {
            return "";//name[name.IndexOf(ViewPath) + 1];
        }
        string LogWarning = string.Format("资源：{0} 是否放错地方？要放在UI/{1}/、 UI/{2}/ 、UI/{3}/ 、UI/{4}这些路径的子文件夹下"
                                            , strPath, CommonPath, DynamicPath + AtlasPath, DynamicPath + NonAtlasesPath, ViewPath);
        Debug.LogWarning(LogWarning);
        return "";
    }

    string GetAtlasName(List<string>pathList, int nBeginIndex=3,int nEndIndex = -1)
    {
        string resPath = string.Empty;
        nEndIndex = nEndIndex == -1 ? pathList.Count : nEndIndex;
        for (int i=nBeginIndex;i<nEndIndex;i++)
        {
            resPath += pathList[i];
            if (i < nEndIndex - 1)
                resPath += AtlasConnectionSymbol;
        }
        return resPath;
    }
}

