using UnityEngine;
using System.Collections;
using UnityEditor;

/// <summary>
/// 特效纹理导入
/// </summary>
public class EffectTextureImport : AssetPostprocessor
{
    private const string EffectTexturePatch = "Artist/Effect/textures";
    private const string GemStoneCubeMapTexturePatch = "Artist/Effect/textures/Huanjing";
    private const string ProjectorPatchKey = "tiehua";
    private const string CookiePatchKey = "cookie";
    private const string NormalPatchKey = "normalmap";
    private const string XuliePatchKey = "xulie";
    private const string UIPatchKey = "ui";
    private TextureImporter textureImporter = null;
    void OnPreprocessTexture()
    {
        textureImporter = assetImporter as TextureImporter;
        string patch = textureImporter.assetPath;
        
        if (patch.Contains(GemStoneCubeMapTexturePatch))
        {
            textureImporter.maxTextureSize = 256;
            return;
        }
       
        if (patch.Contains(EffectTexturePatch))
        {
            if (textureImporter.textureType != TextureImporterType.Image)
            {
                return;
            }

            if (patch.Contains(ProjectorPatchKey)) //贴花
            {
                textureImporter.textureType = TextureImporterType.Advanced;
                textureImporter.isReadable = false;
                textureImporter.mipmapEnabled = false;
                //textureImporter.borderMipmap = true;
                //textureImporter.textureFormat = TextureImporterFormat.ARGB32;
                textureImporter.wrapMode = TextureWrapMode.Clamp;
                textureImporter.maxTextureSize = 256;
            }
            else if (patch.Contains(CookiePatchKey))//cookie贴图
            {
                textureImporter.textureType = TextureImporterType.Advanced;
                textureImporter.isReadable = false;
                textureImporter.mipmapEnabled = false;
                textureImporter.textureFormat = TextureImporterFormat.Alpha8;
                textureImporter.wrapMode = TextureWrapMode.Clamp;
                textureImporter.grayscaleToAlpha = true;
                textureImporter.maxTextureSize = 256;
            }
            else if (patch.Contains(UIPatchKey))//ui贴图
            {
                textureImporter.textureType = TextureImporterType.Advanced;
                textureImporter.isReadable = false;
                textureImporter.mipmapEnabled = false;
                textureImporter.maxTextureSize = 2048;
            }
            else if (patch.Contains(XuliePatchKey))
            {
                textureImporter.textureType = TextureImporterType.Advanced;
                textureImporter.isReadable = false;
                textureImporter.mipmapEnabled = false;
                textureImporter.maxTextureSize = 512;
            }
            else //普通贴图
            {
                textureImporter.textureType = TextureImporterType.Advanced;
                textureImporter.isReadable = false;
                textureImporter.mipmapEnabled = false;
                textureImporter.maxTextureSize = 256;
            }

        }
    }
}
