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

    public const string scenePatch = "Assets/Artist/Scene/";
    public const string SchemePatch = "Assets/Scheme/";
    public const string LinghtingEffectConfigPatch = "Assets/Scheme/LightingEffect";
    public const string DebugscenePatch = "Assets/Artist/Scene/DebugScene";
    public const string MonsterAssetPatch = "Assets/Artist/Model/Monster/";
    public const string HeroAssetPatch = "Assets/Artist/Model/Hero/";
    public const string BossAssetPatch = "Assets/Artist/Model/Boss/";
    public const string NpcAssetPatch = "Assets/Artist/Model/NPC/";
    public const string wingAssetPatch = "Assets/Artist/Model/wing/";
    public const string effectAssetPatch = "Assets/Artist/Effect/model/";
    public const string effectParticleModelAssetPatch = "Assets/Artist/Effect/model/ParticleModel";

    public const string SceneAssetPatch = "Assets/Artist/Model/Scene/";
    public const string SceneMeshColliderPatch = "Assets/Artist/Model/Scene/MeshCollider";
    public const string sceneTerrainDataPatch = "Assets/Artist/Model/Scene/TerrainData";
    public const string sceneTerrainPrefabPatch = "Assets/Prefabs/Scene/Terrain";
    public const string XPjingtouAssetPatch = "Assets/Artist/Model/XPjingtou/";

    public const string SceneSkyBoxPatch = "Assets/Artist/Model/Scene/SkyBox/";
    public const string UIDynamicAssetPatch = "Assets/Artist/UI/Dynamic/";
    public const string UIAssetPatch = "Assets/Artist/UI/";
    public const string BuiltInUIAssetPatch = "Assets/Artist/CommonArtist/UnityDefaultResources/Sprite";
    public const string soundAssetPatch = "Assets/Sounds/";
    public const string soundEnvironmentAssetPatch = "Assets/Sounds/Environment";
    public const string ArtitsPatch = "Assets/Artist/";
    public const string FontPatch = "Assets/Fonts/";
    public const string FromReferencebKey = "FromReference";

    public const string ModelFBXKey = "_model";
    /// <summary>
    /// 调试路径
    /// </summary>
    public const string ArtistDebugPatch = "Assets/Artist/Effect/Hero_animation/";

    public const string SceneOptimitzedConfigPatch = "Assets/Resources/SceneOptimitzedConfig/";

    /// <summary>
    /// 模型的Controller
    /// </summary>
    public const string ModelAnimatorControllerPatch = "Assets/AnimatorController/";

    public const string ShaderPatch = "Assets/Shaders/";
    public const string EditorOnlyShaderPatch = "Assets/Shaders/EditorOnly/";
    public const string PrefabPatch = "Assets/Prefabs/";
    /// <summary>
    /// Unity内置shader路径
    /// </summary>
    const string UnityBuildInShaderPath = "Asset/Shadets/UnityBuildIn/";

    public const string PrefabCommonAssetsPatch = "Assets/Prefabs/CommonPrefab/";
    public const string ArtistCommonAssetsPatch = "Assets/Artist/CommonArtist/";
    public const string MeshPath = "Assets/Artist/CommonArtist/UnityDefaultResources/Mesh/";

    public static void ClearAssetBundleData(AssetImporter assetImporter)
    {
        if (!assetImporter)
        {
            return;
        }

        SaveAssetBundleData(assetImporter,string.Empty,string.Empty);
    }

    public static void ClearUserData(AssetImporter assetImporter)
    {
        if (!assetImporter)
        {
            return;
        }

        SaveAssetBundleUserData(assetImporter, string.Empty);
    }

    public static ResConfigData BuildResConfigData(string path,string referenceAssetbundleName,string referenceVariant)
    {
        if (!referenceAssetbundleName.Equals(string.Empty) && !referenceVariant.Equals(string.Empty))
        {
            string guid = AssetBundleManagerEditor.GetAssetGUIDFromMeta(path);
            string assetName = Path.GetFileNameWithoutExtension(path);

            ResConfigData cof = new ResConfigData();
            cof.AssetBundleName = referenceAssetbundleName + "." + referenceVariant;
            cof.AssetGUID = guid;
            cof.AssetName = assetName;
            return cof;
        }
        return null;
    }

    private static void SaveAssetBundleData(AssetImporter assetImporter,string AssetbundleName,string AssetVarian)
    {
        if (!assetImporter)
        {
            return;
        }
        AssetbundleName = AssetbundleName.ToLower();
        AssetVarian = AssetVarian.ToLower();
        if (assetImporter.assetBundleName.Equals(AssetbundleName) &&
            assetImporter.assetBundleVariant.Equals(AssetVarian))
        {
            return;
        }

        string filePatch = AssetBundleManagerEditor.ConverRelativePatchToObslotePatch(assetImporter.assetPath);
        filePatch += ".meta";

        if (!File.Exists(filePatch))
        {
            return;
        }

        string text = System.IO.File.ReadAllText(filePatch);
        List<string> alltext = new List<string>();
        alltext.AddRange(text.Split('\n'));
        string strAbVar = "  assetBundleVariant: ";
        string StrAbNa = "  assetBundleName: ";
        for (int i = alltext.Count - 1; i >= 0; i--)
        {
            string s = alltext[i];
            if (s.Contains(StrAbNa))
            {
                alltext[i] = StrAbNa + AssetbundleName;
            }
            if (s.Contains(strAbVar))
            {
                alltext[i] = strAbVar + AssetVarian;
            }
        }

        string result = string.Empty;
        result = alltext[0];
        for (int i = 1; i < alltext.Count; i++)
        {
            result += '\n';
            result += alltext[i];
        }
        System.IO.File.WriteAllText(filePatch, result);
        assetImporter.assetBundleName = AssetbundleName;
        if (!AssetbundleName.Equals(string.Empty))
        {
            assetImporter.assetBundleVariant = AssetVarian;
        }
      
        

    }

    /// <summary>
    /// 保存AssetBundle,版本升级后，再onpostallassets阶段，ab名不会写入到meta文件了，需要手动写入
    /// </summary>
    /// <param name="assetImporter"></param>
    private static void SaveAssetBundleData(AssetImporter assetImporter,AssetBundleManagerEditor.AssetBundleEditorInfo info)
    {
        if (!assetImporter || null == info)
        {
            return;
        }

        SaveAssetBundleData(assetImporter, info.AssetBundleName, info.AssetBundleVarian);

    }


    /// <summary>
    /// 保存AssetBundle,版本升级后，再onpostallassets阶段，ab名不会写入到meta文件了，需要手动写入
    /// </summary>
    /// <param name="assetImporter"></param>
    private static void SaveAssetBundleUserData(AssetImporter assetImporter,string userData)
    {
        if (!assetImporter)
        {
            return;
        }

        string filePatch = AssetBundleManagerEditor.ConverRelativePatchToObslotePatch(assetImporter.assetPath);
        filePatch += ".meta";

        if (!File.Exists(filePatch))
        {
            return;
        }
        //if (assetImporter.userData.Equals(userData))
        //{
        //    return;
        //}
        assetImporter.userData = userData;
        string text = System.IO.File.ReadAllText(filePatch);
        List<string> alltext = new List<string>();
        alltext.AddRange(text.Split('\n'));
        string StrUSD = "  userData: ";
        for (int i = alltext.Count - 1; i >= 0 ; i--)
        {
            string s = alltext[i];
            if (s.Contains(StrUSD))
            {
                alltext[i] = StrUSD + userData;
            }
        }
        
        string result = string.Empty;
        result = alltext[0];
        for (int i = 1; i < alltext.Count; i++)
        {
            result += '\n';
            result += alltext[i];
        }
        System.IO.File.WriteAllText(filePatch, result);
    }

    private static bool IsCommonArtistAssets(string patch)
    {
        return patch.Contains(ArtistCommonAssetsPatch);
    }

    private static bool IsCommonPrefabAssets(string patch)
    {
        return patch.Contains(PrefabCommonAssetsPatch);
    }

    private static bool IsUIDynamicAssets(string patch)
    {
        return patch.Contains(UIDynamicAssetPatch);
    }
    /// <summary>
    /// 检查资源的依赖，因为通用资源只能引用通用目录下的东西
    /// </summary>
    /// <param name="path"></param>
    /// <returns></returns>
    private static bool CheckAssetDependices(string path)
    {
        bool bOk = true;
        //如果是通用资源，要检测一下他引用的资源是不是在通用目录下，如果不是，不予以生成assetbundle
        if (IsCommonArtistAssets(path))
        {
            string[] dependence = AssetDatabase.GetDependencies(new string[] { path });
            foreach (string s in dependence)
            {
                if (!IsCommonArtistAssets(s) && !s.Contains("Resources") && !isShaderAssets(s))
                {
                    Debug.LogError("通用资源:" + path + ",引用了非通用目录下的资源:" + s);
                    bOk = false;
                }
            }
        }
        return bOk;
    }

    static void OnPostprocessAllAssets(string[] importedAssets, string[] deletedAssets, string[] movedAssets, string[] movedFromAssetPaths)
    {
        bool bMovedSound = false;
        bool bImportSound = false;

        //处理的是否是UI文件
        bool bImportUI = false;


        OnPreProcessScheme();
       
        for (int i = 0; i < importedAssets.Length; i++)
        {
            //看看是不是场景文件
            string s = importedAssets[i];
            AssetBundleManagerEditor.CheckAssetName(s);
            if(isSceneAssets(s))
            {
                MakeSceneAssetBundleInfo(s);
            }
            else if (isShaderAssets(s))
            {
                MakeShaderAssetBundle(s);
            }
            else if (isMeshAssets(s))
            {
                MakeMeshAssetBundle(s);
            }
            else if(isAudioAssets(s))
            {
               bImportSound = true;
               MakeSoundAssetBundleData(s);
            }
            else if(IsFontAsset(s))
            {
                MakeFontBundleData(AssetImporter.GetAtPath(s));
            }
            else if(IsFBXAsset(s))
            {
                MakeFBXAssetBundle(AssetImporter.GetAtPath(s) as ModelImporter);
            }
            else if(IsMaterialAsset(s))
            {
                MakeMaterialAssetBundle(s);
            }
            else if (IsAniClipAsset(s))
            {
                MakeAnimatorClipAssetBundleData(s);
            }
            else if(IsControllerAsset(s))
            {
                MakeAnimatorControllerAssetBundleData(s);
            }
            else if(IsTextureAsset(s))
            {
                MakeTextureAssetBundle(s);
            }
            else if (isSchemeAssets(s))
            {
                MakeSchemeAssetBundleInfo(s);
            }

            string path = importedAssets[i];
            //载入的话弄到表里
            if (path.Contains(UIDynamicAssetPatch))
            {
                if (!IsTextureAsset(path))
                {
                    continue;
                }

                bImportUI = true;
                UFileData data = new UFileData();

                data.configData = MakeTextureAssetBundleFromReference(path, string.Empty);
                if (null != data.configData)
                {
                    PostprocessUITextureToCSV.nEditorInstance.InsetConfig(data);
                }
                else
                {
                    Debug.LogError("取得UI资源失败:" + path);
                }
            }
        }

        //资源移动了
        List<string> movedGUID = new List<string>();
        for (int i = 0; i < movedAssets.Length; i++)
        {
            string str = movedAssets[i];
            AssetBundleManagerEditor.CheckAssetName(movedAssets[i]);
            //先清掉ab
            ClearAssetBundleData(AssetImporter.GetAtPath(str));

            //再根据资源类型来重新生成ab
            //是音效的
            if (isAudioAssets(str))
            {
                MakeSoundAssetBundleData(str);
                movedGUID.Add(AssetBundleManagerEditor.GetAssetGUIDFromMeta(str));
                bMovedSound = true;
            }
            //是贴图
            else if (IsTextureAsset(str))
            {
                MakeTextureAssetBundle(str);
            }
            //是场景
            else if (isSceneAssets(str))
            {
                MakeSceneAssetBundleInfo(str);
            }
            else if(IsFontAsset(str))
            {
                MakeFontBundleData(AssetImporter.GetAtPath(str));
            }
            else if (IsFBXAsset(str))
            {
                CheckFBXInImportTime(str);
                MakeFBXAssetBundle(AssetImporter.GetAtPath(str) as ModelImporter);
            }
            else if (IsMaterialAsset(str))
            {
                MakeMaterialAssetBundle(str);
            }
            else if (IsAniClipAsset(str))
            {
                MakeAnimatorClipAssetBundleData(str);
            }
            else if (IsControllerAsset(str))
            {
                MakeAnimatorControllerAssetBundleData(str);
            }
            else if (isSchemeAssets(str))
            {
                MakeSchemeAssetBundleInfo(str);
            }
            else if (isShaderAssets(str))
            {
                MakeShaderAssetBundle(str);
            }
            else if (isMeshAssets(str))
            {
                MakeMeshAssetBundle(str);
            }
            string path = movedAssets[i];
            if (path.Contains(UIDynamicAssetPatch))
            {
                bImportUI = true;
                AssetImporter assetImporter = AssetImporter.GetAtPath(path);

                PostprocessUITextureToCSV.nEditorInstance.RemoveConfig(PostprocessUITextureToCSV.GetGUID(assetImporter.assetPath));
            }
        }

        if (SoundSystem.SoundSystemEditorWindow.EditorInstance)
        {
            if (bMovedSound || bImportSound)
            {
                if (bMovedSound)
                {
                    SoundSystem.SoundSystemEditorWindow.RefrshAudioClipSource(movedGUID);
                }
                else
                {
                    SoundSystem.SoundSystemEditorWindow.RefrshAudioClipSource();
                }

            }
        }

        //UI的处理完保存配置
        if (bImportUI)
        {
            PostprocessUITextureToCSV.nEditorInstance.ExportConfig();
        }

        OnPostProcessScheme();

        if (!AssetBundleManagerEditor.isBuildingPackage && !AssetBundleManagerEditor.isCleaningAssets)
        {
            AssetBundleManager.LoadPrefabConfig();

            for (int i = 0; i < movedAssets.Length; i++)
            {
                string str = movedAssets[i];
                if (IsPrefabAssets(movedFromAssetPaths[i]))
                {
                    ClearSinglePrefab(str);
                }

                if (IsPrefabAssets(str))
                {
                    ImportSinglePrefab(str, true);
                }
            }


            for (int i = 0; i < importedAssets.Length; i++)
            {
                ImportSinglePrefab(importedAssets[i],true);
            }

            SchemeExport.Export_PrefabScheme();
            AssetBundleManager.ClearPrefabConfig();
        }


        //进入播放模式的时候会调用一次这个postallasets，所以要加载一次配置
        if(Application.isPlaying)
        {
            AssetBundleManager.LoadPrefabConfig();
        }

        if (movedAssets.Length > 0)
        {
            AssetDatabase.Refresh();
        }
        //移除无用的ab名
        AssetDatabase.RemoveUnusedAssetBundleNames();
    }


    public static void BeginPrefabAssetImport()
    {
        allGeneratePrefabAssetBundleInfoTable.Clear();
    }

    private static void PushPrefabAssetImport(AssetImporter assetImporter,AssetBundleManagerEditor.AssetBundleEditorInfo info)
    {
        if(!assetImporter || info == null)
        {
            return;
        }
        if (allGeneratePrefabAssetBundleInfoTable.ContainsKey(assetImporter))
        {
            allGeneratePrefabAssetBundleInfoTable[assetImporter] = info;
        }
        else
        {
            allGeneratePrefabAssetBundleInfoTable.Add(assetImporter, info);
        }
    }
    public static void EndPrefabAssetImport()
    {
        foreach (KeyValuePair<AssetImporter, AssetBundleManagerEditor.AssetBundleEditorInfo> var in allGeneratePrefabAssetBundleInfoTable)
        {
            SaveAssetBundleData(var.Key, var.Value);
        }
        allGeneratePrefabAssetBundleInfoTable.Clear();
    }
}
