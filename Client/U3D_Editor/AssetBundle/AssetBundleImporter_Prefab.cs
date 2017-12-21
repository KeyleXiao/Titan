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
    /// 预制体后缀
    /// </summary>
    public const string PrefabExetion = ".prefab";

    public const string GameManagerPrefab = "_GameManager";

    public const string extractUnityDefaultPathRoot = ArtistCommonAssetsPatch + "UnityDefaultResources/";

    private static string[] invalidMaterialName = new string[] { "No Name", "01 - Default"};


    /// <summary>
    /// unity默认资源
    /// </summary>
    static string []DefaultResourcesGUID = new string[]{"0000000000000000e000000000000000"/*unity default resources*/,"0000000000000000f000000000000000"/*unity_builtin_extra*/};



    /// <summary>
    /// 联合资源的互斥，在这个表里面，说明已经被引用了
    /// </summary>
    static HashSet<string> UnionResMutexPath = new HashSet<string>();

    private static Dictionary<AssetImporter, AssetBundleManagerEditor.AssetBundleEditorInfo> allGeneratePrefabAssetBundleInfoTable = new Dictionary<AssetImporter, AssetBundleManagerEditor.AssetBundleEditorInfo>();

    static bool CanUnioResMutex = false;

    /// <summary>
    /// 是否是无效的材质球，比如01 material,No Name等等
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    public static bool isInValidMaterial(string path)
    {
        foreach(string s in invalidMaterialName)
        {
            if(path.Contains(s))
            {
                return true;
            }
        }
        return false;
    }

    /// <summary>
    /// 修正材质，把引用unity的shader换成自己的。
    /// </summary>
    /// <param name="path"></param>
    /// <returns></returns>
    public static bool CorrectMaterial(string path)
    {
        //只在非打包机上做材质修正
        if (EditorRunTypeWindow.currentEditorRunType == EditorRunType.PackingMachine)
        {
            return true;
        }
        if (IsMaterialAsset(path))
        {
            //无效的材质球
            if (isInValidMaterial(path))
            {
                return true;
            }

            Material mat = AssetDatabase.LoadAssetAtPath<Material>(path);
            string shaderName = mat.shader.name;

            bool bSuccess = isValidShader(shaderName);

            if(!bSuccess)
            {
                Shader sh = Shader.Find(unityBuildInShaderHead + shaderName);
                if(!sh)
                {
                    sh = Shader.Find(CharacterStandradShaderHead + shaderName);
                }
                if (!sh)
                {
                    Debug.LogError("修正Shader失败:" + path + ",shader:" + shaderName + ",请替换,不要使用此shader,如需使用，请联系程序");
                    return false;
                }
                else
                {
                    mat.shader = sh;
                    Debug.LogWarning("修正Shader成功:" + path + ",shader:" + shaderName + ",请不要使用unity内置shader，如需引用，请到此文件夹下寻找:" + UnityBuildInShaderPath);
                }
            }

        }

        return true;
    }

    public static bool CheckPrefabAssets(string patch,bool isSkin,bool heroSkin)
    {
        GameObject go = AssetDatabase.LoadAssetAtPath<GameObject>(patch);
        
        if(!go)
        {
            Debug.LogError( "资源错误:" + patch + "不是预制体或者是空预制体");
            
            return false;
        }
        bool Reslut = true;
        //先检查skinnedmeshrender,对于animator来说，如果skinnedmeshrender丢失的话，打包的时候会崩溃
        //所以要检查每一个预制体的skinned是否正常
        SkinnedMeshRenderer[] allSkinneds = go.GetComponentsInChildren<SkinnedMeshRenderer>(true);
        foreach(SkinnedMeshRenderer sk in allSkinneds)
        {
            if (!sk.sharedMesh)
            {
                Debug.LogError("资源错误:" + patch + "中 " + sk.name + "丢失mesh，请检查资源");
                Reslut = false;
            }
            else
            {
                if(sk.sharedMesh.name.Contains("Missing"))
                {
                    Debug.LogError("资源错误:" + patch + "中 " + sk.name + "丢失mesh，请检查资源");
                    Reslut = false;
                }

                if (sk.sharedMesh.boneWeights.Length <= 0 || sk.sharedMesh.bindposes.Length <= 0)
                {
                    Debug.LogError("资源错误:" + patch + "中 " + sk.name + "与FBX文件不匹配，请更新.");
                    Reslut = false;
                }
            }
        }

        MeshFilter[] allMeshFilter= go.GetComponentsInChildren<MeshFilter>(true);
        foreach (MeshFilter sk in allMeshFilter)
        {
            if (!sk.sharedMesh)
            {
                Debug.LogError("资源错误:" + patch + "中 " + sk.name + "丢失mesh，请检查资源");
                Reslut = false;
            }
            else
            {
                if (sk.sharedMesh.name.Contains("Missing"))
                {
                    Debug.LogError("资源错误:" + patch + "中 " + sk.name + "丢失mesh，请检查资源");
                    Reslut = false;
                }
            }
        }

        if (isSkin)
        {
            Animator[] anis = go.GetComponents<Animator>();

            foreach (Animator ani in anis)
            {
                if (!ani.runtimeAnimatorController)
                {
                    Debug.LogError("资源错误:" + patch + "丢失状态机，请检查资源");
                    Reslut = false;
                }
                else
                {
                    if (ani.runtimeAnimatorController.name.Contains("Missing"))
                    {
                        Debug.LogError("资源错误:" + patch + "丢失状态机，请检查资源");
                        Reslut = false;
                    }
                }


                if (!ani.avatar)
                {
                    Debug.LogError("资源错误:" + patch + "丢失Avatar，请检查资源");
                    Reslut = false;
                }
                else
                {
                    if (ani.avatar.name.Contains("Missing"))
                    {
                        Debug.LogError("资源错误:" + patch + "丢失Avatar，请检查资源");
                        Reslut = false;
                    }
                }
            }

            if(heroSkin)
            {
                EntitySkinConfig skinconfig = go.GetComponentInChildren<EntitySkinConfig>();
                if (skinconfig)
                {
                    if (!skinconfig.BodyRender)
                    {
                        Debug.LogError("资源错误:" + patch + "丢失皮肤配置上的[-身体-]配置为空，请[策划]重新配置");
                        Reslut = false;
                    }
                }
                else
                {
                    Debug.LogError("资源错误:" + patch + "皮肤配置上没有对应的SkinConfig，请[策划]重新配置");
                    Reslut = false;
                }
            }

        }
        return Reslut;
    }

    /// <summary>
    /// 是否引用了Unity内置资源
    /// </summary>
    /// <param name="prefabPatch"></param>
    /// <returns></returns>
    public static bool IsReferenceUnityDefaultAssets(string prefabPatch)
    {
        if(!IsPrefabAssets(prefabPatch))
        {
            return false;
        }
        if (prefabPatch.Contains(sceneTerrainPrefabPatch))
        {
            return false;
        }
        string allFileText = File.ReadAllText(prefabPatch, System.Text.Encoding.Default);


        bool findDefaultAssets = false;
        foreach (string guid in DefaultResourcesGUID)
        {
            if (allFileText.Contains(guid))
            {
                findDefaultAssets = true;
                break;
            }
        }
        return findDefaultAssets;
    }


    /// <summary>
    /// 导入单个预制体,会刷新资源数据库，生成新的配置资源，并且降单个预制体配置导入到
    /// 配置表
    /// </summary>
    /// <param name="patch"></param>
    public static void ImportSinglePrefab(string patch,bool isImportingTime)
    {
        string lowerPatch = patch;

        if(!IsPrefabAssets(lowerPatch))
        {
            return;
        }


        string skinPatch = "Prefabs/Skin";
        string skinHeroPatch = "Prefabs/Skin/Hero";
        List<string> dependeciesList = new List<string>(AssetDatabase.GetDependencies(new string[] { patch }));

        if(lowerPatch.Contains(skinPatch))
        {
            if(dependeciesList.Count > 90)
            {
                if (EditorRunTypeWindow.currentEditorRunType == EditorRunType.DevelopMachine || EditorRunTypeWindow.currentEditorRunType == EditorRunType.ResourcesCleanUpMachine)
                {
                    Debug.LogError(patch + ",引用超过90,当前引用:" + dependeciesList.Count + ",请检查是否有不正确的引用");
                    return;
                }

            }
        }


        if (EditorRunTypeWindow.currentEditorRunType == EditorRunType.DevelopMachine || EditorRunTypeWindow.currentEditorRunType == EditorRunType.ResourcesCleanUpMachine)
        {
            //等待下次导入
            if (IsReferenceUnityDefaultAssets(patch))
            {
                Debug.LogError(patch + "引用了Unity内置资源(如系统自带的UI资源[UIMask、UISprite等]，材质，Cube等)，请移除，如需引用内置资源请引用:" + extractUnityDefaultPathRoot + "下的资源");
                //ClearAssetBundleData(AssetImporter.GetAtPath(patch));
                //return;
            }
        }

        //只在非导入的时候检查资源，因为导入的时候有可能其他资源没有导进来。
        if (!isImportingTime || EditorRunTypeWindow.currentEditorRunType == EditorRunType.DevelopMachine)
        {
            if (!CheckPrefabAssets(patch, lowerPatch.Contains(skinPatch), lowerPatch.Contains(skinHeroPatch)))
            {
                return;
            }
        }

        AssetBundleManagerEditor.AssetBundleEditorInfo info = null;
        //if (EditorRunTypeWindow.currentEditorRunType == EditorRunType.DevelopMachine || EditorRunTypeWindow.currentEditorRunType == EditorRunType.ResourcesCleanUpMachine) //开发机去构建这个
        {
            info = MakePrefabAssetBundleInfo(patch);
            if (info == null)
            {
                Debug.LogError("预制体取得AssetBundleEditorInfo失败，路径:" + patch);
                return;
            }
        }
        //else if (EditorRunTypeWindow.currentEditorRunType == EditorRunType.PackingMachine)//而打包机，只需要去取
        //{
        //    info = AssetBundleManagerEditor.GetAssetBundleEditorInfo(patch);
        //    if(info == null)
        //    {
        //        Debug.LogWarning("读取资源包配置失败:" + patch);
        //        return;
        //    }
        //}


        List<string> texConfig = new List<string>();
        List<string> FBXConfig = new List<string>();
        List<string> ClipConfig = new List<string>();
        List<string> ControllerConfig = new List<string>();
        List<string> ShaderConfig = new List<string>();
        List<string> MaterialConfig = new List<string>();

        //引用的资源，无非就几种:代码，贴图，材质球，动画控制器，动画Mask,动画clip，fbx
        //我们只将贴图、fbx、动画控制器、动画clip打包，其他的直接整合到prefab的包里面
        foreach (string s in dependeciesList)
        {
            if (IsCanImportDependicens_FBX(patch, s, isImportingTime))
            {
                string referenceAbName = info.AssetBundleName.Replace("gameobject", "model") + AssetBundleManager.PrefabUnionResKey;
                ResConfigData cof = MakeFBXAssetBundleFromReference(s, referenceAbName);
                if (cof == null)
                {
                    Debug.LogError("取得资源配置失败:" + s);
                    continue;
                }
                AssetBundleManager.PushDependenicesRes(cof);
                FBXConfig.Add(cof.AssetGUID);
            }
            else if (IsCanImportDependicens_Texture(patch, s, isImportingTime))
            {
                string referenceAbName = info.AssetBundleName.Replace("gameobject", "texture") + AssetBundleManager.PrefabUnionResKey;
                ResConfigData cof = MakeTextureAssetBundleFromReference(s, referenceAbName);
                if (cof == null)
                {
                    Debug.LogError("取得资源配置失败:" + s + ",请及时修复,否则会导致预制体加载失败");
                    continue;
                }
                AssetBundleManager.PushDependenicesRes(cof);
                texConfig.Add(cof.AssetGUID);
            }
            else if (IsCanImportDependicens_AniController(patch, s, isImportingTime))
            {
                string referenceAbName = info.AssetBundleName.Replace("gameobject", "animcontroller") + AssetBundleManager.PrefabUnionResKey;
                ResConfigData cof = MakeAnimatorControllerAssetBundleFromReference(s, referenceAbName);
                if (cof == null)
                {
                    Debug.LogError("取得资源配置失败:" + s + ",请及时修复,否则会导致预制体加载失败");
                    continue;
                }
                AssetBundleManager.PushDependenicesRes(cof);
                ControllerConfig.Add(cof.AssetGUID);
            }
            else if (IsCanImportDependicens_AniClip(patch, s, isImportingTime))
            {
                string referenceAbName = info.AssetBundleName.Replace("gameobject", "animclip") + AssetBundleManager.PrefabUnionResKey;
                ResConfigData cof = MakeAnimatorClipAssetBundleFromReference(s, referenceAbName);
                if (cof == null)
                {
                    Debug.LogError("取得资源配置失败:" + s + ",请及时修复,否则会导致预制体加载失败");
                    continue;
                }
                AssetBundleManager.PushDependenicesRes(cof);
                ClipConfig.Add(cof.AssetGUID);
            }
            else if (IsCanImportDependicens_Material(patch, s, isImportingTime))
            {
                string referenceAbName = info.AssetBundleName.Replace("gameobject", "material") + AssetBundleManager.PrefabUnionResKey;
                ResConfigData cof = MakeMaterialAssetBundleFromReference(s, referenceAbName);
                if (cof == null)
                {
                    Debug.LogError("取得资源配置失败:" + s + ",请及时修复,否则会导致预制体加载失败");
                    continue;
                }
                AssetBundleManager.PushDependenicesRes(cof);
                MaterialConfig.Add(cof.AssetGUID);
            }
            else if (IsPrefabAssets(s) && !s.Equals(lowerPatch))
            {
                if (!s.Contains(AssetBundleManager.PrefabCommonPatch))
                {
                    Debug.LogError("不能引用预制体,如果要引用，请使用动态加载!" + lowerPatch + ",引用路径:" + s);
                    continue;
                }

            }
        }
       AssetBundleManager.PushPrefabConfig(info.ConvertToUnionResConfigData(),
                                            texConfig,FBXConfig,
                                            ClipConfig,ControllerConfig,
                                            ShaderConfig, MaterialConfig);
    }

    private static bool IsCanImportDependicens_FBX(string prefabPath,string path,bool importingTime)
    {
        if (IsFBXAsset(path))
        {

            if (!path.Contains(ArtitsPatch) && !path.Contains(SceneOptimitzedConfigPatch))
            {
                Debug.LogError("预制体:" + prefabPath + "引用了Artist以外的资源，请修！:" + path);
                return false;
            }

            if (path.Contains(SceneAssetPatch))
            {
                if (!path.Contains(sceneTerrainDataPatch))
                {
                    Debug.LogError("预制体请不要引用场景资源下的东西,如果要引用,请将资源放到Artist/CommonArtist文件夹下!:" + prefabPath + ",引用路径:" + path + ",如果是地形资源请放到:" + sceneTerrainDataPatch);
                    return false;
                }
            }

            if (path.Contains(ArtistDebugPatch))
            {
                Debug.LogError("预制体请不要引用资源调试目录下的东西:" + prefabPath + ",引用路径:" + path);
                return false;
            }
            string animaClipPath = path.ToLower();
            string needReferencePath = path.ToLower();
            needReferencePath = needReferencePath.Replace(".fbx", "_model.fbx");
            animaClipPath = animaClipPath.Replace(".fbx", "_AnimationClip");
            if (System.IO.Directory.Exists(animaClipPath))
            {
                Debug.LogError("预制体:" + prefabPath + ",不能用源动作文件:" + path + ",请引用如下路径文件:" + needReferencePath);
                return false;
            }

            //在导入的时候，并且是开发机,直接返回flase，不写入ab名字
            if (importingTime && EditorRunTypeWindow.currentEditorRunType == EditorRunType.DevelopMachine)
            {
                if(EditorRunTypeWindow.currentEditorRunType != EditorRunType.ResourcesCleanUpMachine)
                {
                    return false;
                }

            }

            return true;
        }
        return false;
    }

    private static bool IsCanImportDependicens_Texture(string prefabPath, string path, bool importingTime)
    {
        if (IsTextureAsset(path))
        {
            //如果引用了字体资源，跳过，字体资源不需要加载，常驻内存的
            if (path.Contains(FontPatch))
            {
                return false;
            }

            if (!path.Contains(ArtitsPatch) && !path.Contains(SceneOptimitzedConfigPatch))
            {
                Debug.LogError("预制体:" + prefabPath + "引用了Artist以外的资源，请修！:" + path);
                return false;
            }
            //可以应用场景资源下的贴图
            //if (lowerString.Contains(SceneAssetPatch))
            //{
            //      Debug.LogError("预制体请不要引用场景资源下的东西,如果要引用,请将资源放到Artist/CommonArtist文件夹下!:" + patch + ",引用路径:" + s);
            //      continue;
            //}

            if (path.Contains(ArtistDebugPatch))
            {
                Debug.LogError("预制体请不要引用资源调试目录下的东西:" + prefabPath + ",引用路径:" + path);
                return false;
            }
            //在导入的时候，并且是开发机,直接返回flase，不写入ab名字
            if (importingTime && EditorRunTypeWindow.currentEditorRunType == EditorRunType.DevelopMachine)
            {
                if (EditorRunTypeWindow.currentEditorRunType != EditorRunType.ResourcesCleanUpMachine)
                {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    private static bool IsCanImportDependicens_AniController(string prefabPath, string path, bool importingTime)
    {
        if (IsControllerAsset(path))
        {
            if (path.Contains(SceneAssetPatch))
            {
                Debug.LogError("预制体请不要引用场景资源下的东西,如果要引用,请将资源放到Artist/CommonArtist文件夹下!:" + prefabPath + ",引用路径:" + path);
                return false;

            }
            if (path.Contains(ArtistDebugPatch))
            {
                Debug.LogError("预制体请不要引用资源调试目录下的东西:" + prefabPath + ",引用路径:" + path);
                return false;
            }

            //在导入的时候，并且是开发机,直接返回flase，不写入ab名字
            if (importingTime && EditorRunTypeWindow.currentEditorRunType == EditorRunType.DevelopMachine)
            {
                if (EditorRunTypeWindow.currentEditorRunType != EditorRunType.ResourcesCleanUpMachine)
                {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    private static bool IsCanImportDependicens_AniClip(string prefabPath, string path, bool importingTime)
    {
        if (IsAniClipAsset(path))
        {
            if (path.Contains(SceneAssetPatch))
            {
                Debug.LogError("预制体请不要引用场景资源下的东西,如果要引用,请将资源放到Artist/CommonArtist文件夹下!:" + prefabPath + ",引用路径:" + path);
                return false;
            }
            if (path.Contains(ArtistDebugPatch))
            {
                Debug.LogError("预制体请不要引用资源调试目录下的东西:" + prefabPath + ",引用路径:" + path);
                return false;
            }



            //在导入的时候，并且是开发机,直接返回flase，不写入ab名字
            if (importingTime && EditorRunTypeWindow.currentEditorRunType == EditorRunType.DevelopMachine)
            {
                if (EditorRunTypeWindow.currentEditorRunType != EditorRunType.ResourcesCleanUpMachine)
                {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    private static bool IsCanImportDependicens_Material(string prefabPath, string path, bool importingTime)
    {
        if (IsMaterialAsset(path))
        {
            //无效的材质球
            if (isInValidMaterial(path))
            {
                return false;
            }
            //如果引用了字体资源，跳过，字体资源不需要加载，常驻内存的
            if (path.Contains(FontPatch))
            {
                return false;
            }

            if (!path.Contains(ArtitsPatch) && !path.Contains(SceneOptimitzedConfigPatch))
            {
                Debug.LogError("预制体:" + prefabPath + "引用了Artist的资源，请修！:" + path);
                return false;
            }

            if (path.Contains(SceneAssetPatch))
            {
                if (!path.Contains(sceneTerrainDataPatch))
                {
                    Debug.LogError("预制体请不要引用场景资源下的东西,如果要引用,请将资源放到Artist/CommonArtist文件夹下!:" + prefabPath + ",引用路径:" + path + ",如果是地形资源请放到:" + sceneTerrainDataPatch);
                    return false;
                }
            }

            if (path.Contains(ArtistDebugPatch))
            {
                Debug.LogError("预制体请不要引用资源调试目录下的东西:" + prefabPath + ",引用路径:" + path);
                return false;
            }
            if (!CorrectMaterial(path))
            {
                return false;
            }
            //在导入的时候，并且是开发机,直接返回flase，不写入ab名字
            if (importingTime && EditorRunTypeWindow.currentEditorRunType == EditorRunType.DevelopMachine)
            {
                if (EditorRunTypeWindow.currentEditorRunType != EditorRunType.ResourcesCleanUpMachine)
                {
                    return false;
                }
            }

            return true;
        }
        return false;
    }

    /// <summary>
    /// 清楚单个预制体,会刷新资源数据库
    /// 配置表
    /// </summary>
    /// <param name="patch"></param>
    public static void ClearSinglePrefab(string patch)
    {
        string lowerPatch = patch;
        List<string> dependeciesList = new List<string>(AssetDatabase.GetDependencies(new string[] { patch }));

        //引用的资源，无非就几种:代码，贴图，材质球，动画控制器，动画Mask,动画clip，fbx
        //我们只将贴图、fbx、动画控制器、动画clip打包，其他的直接整合到prefab的包里面
        foreach (string s in dependeciesList)
        {
            string lowerString = s;

            if (IsFBXAsset(s))
            {
                if (!lowerString.Contains(ArtitsPatch) && !lowerString.Contains(SceneOptimitzedConfigPatch))
                {
                    Debug.LogError("预制体:" + patch + "引用了Artist以外的资源，请修！:" + s);
                    continue;
                }
                if (lowerString.Contains(SceneAssetPatch))
                {
                    if (!lowerString.Contains(AssetBundleManager.ArtistCommonPatch))
                    {
                        Debug.LogError("预制体请不要引用场景资源下的东西,如果要引用,请放到Artist/CommonArtist文件夹下!:" + patch + ",引用路径:" + s);
                        continue;
                    }

                }
                if (lowerString.Contains(ArtistDebugPatch))
                {
                    Debug.LogError("预制体请不要引用资源调试目录下的东西:" + patch + ",引用路径:" + s);
                    continue;
                }
                ClearAssetBundleData(AssetImporter.GetAtPath(s));
            }
            else if (IsTextureAsset(s))
            {
                //如果引用了字体资源，跳过，字体资源不需要加载，常驻内存的
                if (lowerString.Contains(FontPatch))
                {
                    continue;
                }

                if (!lowerString.Contains(ArtitsPatch) && !lowerString.Contains(SceneOptimitzedConfigPatch))
                {
                    Debug.LogError("预制体:" + patch + "引用了Artist以外的资源，请修！:" + s);
                    continue;
                }
                if (lowerString.Contains(SceneAssetPatch))
                {
                    if (!lowerString.Contains(AssetBundleManager.ArtistCommonPatch))
                    {
                        Debug.LogError("预制体请不要引用场景资源下的东西,如果要引用,请放到Artist/CommonArtist文件夹下!:" + patch + ",引用路径:" + s);
                        continue;
                    }

                }

                if (lowerString.Contains(ArtistDebugPatch))
                {
                    Debug.LogError("预制体请不要引用资源调试目录下的东西:" + patch + ",引用路径:" + s);
                    continue;
                }

                ClearAssetBundleData(AssetImporter.GetAtPath(s));
            }
            else if (IsControllerAsset(s))
            {
                if (lowerString.Contains(SceneAssetPatch))
                {
                    if (!lowerString.Contains(AssetBundleManager.ArtistCommonPatch))
                    {
                        Debug.LogError("预制体请不要引用场景资源下的东西,如果要引用,请放到Artist/CommonArtist文件夹下!:" + patch + ",引用路径:" + s);
                        continue;
                    }

                }
                if (lowerString.Contains(ArtistDebugPatch))
                {
                    Debug.LogError("预制体请不要引用资源调试目录下的东西:" + patch + ",引用路径:" + s);
                    continue;
                }
                ClearAssetBundleData(AssetImporter.GetAtPath(s));
            }
            //只有特效会用这个clip
            else if (IsAniClipAsset(s))
            {
                if (lowerString.Contains(SceneAssetPatch))
                {
                    if (!lowerString.Contains(AssetBundleManager.ArtistCommonPatch))
                    {
                        Debug.LogError("预制体请不要引用场景资源下的东西,如果要引用,请放到Artist/CommonArtist文件夹下!:" + patch + ",引用路径:" + s);
                        continue;
                    }

                }
                if (lowerString.Contains(ArtistDebugPatch))
                {
                    Debug.LogError("预制体请不要引用资源调试目录下的东西:" + patch + ",引用路径:" + s);
                    continue;
                }
                ClearAssetBundleData(AssetImporter.GetAtPath(s));
            }
            else if (IsMaterialAsset(s))
            {
                //如果引用了字体资源，跳过，字体资源不需要加载，常驻内存的
                if (lowerString.Contains(FontPatch))
                {
                    continue;
                }

                if (!lowerString.Contains(ArtitsPatch) && !lowerString.Contains(SceneOptimitzedConfigPatch))
                {
                    Debug.LogError("预制体:" + patch + "引用了Artist的资源，请修！:" + s);
                    continue;
                }
                if (lowerString.Contains(SceneAssetPatch))
                {
                    if (!lowerString.Contains(AssetBundleManager.ArtistCommonPatch))
                    {
                        Debug.LogError("预制体请不要引用场景资源下的东西,如果要引用,请放到Artist/CommonArtist文件夹下!:" + patch + ",引用路径:" + s);
                        continue;
                    }

                }
                if (lowerString.Contains(ArtistDebugPatch))
                {
                    Debug.LogError("预制体请不要引用资源调试目录下的东西:" + patch + ",引用路径:" + s);
                    continue;
                }
                ClearAssetBundleData(AssetImporter.GetAtPath(s));
            }
            else if (IsPrefabAssets(s) && !lowerString.Equals(lowerPatch))
            {
                if (!lowerString.Contains(AssetBundleManager.PrefabCommonPatch))
                {
                    Debug.LogError("不能引用预制体,如果要引用，请使用动态加载!" + lowerPatch + ",引用路径:" + lowerString);
                    continue;
                }

            }
        }

        AssetBundleManager.DeletePrefabConfig(AssetDatabase.AssetPathToGUID(patch));
    }

    public static void BeginGenerateUnionRes()
    {
        UnionResMutexPath.Clear();
        CanUnioResMutex = true;
        
    }

    public static void EndGenerateUnionRes()
    {
        UnionResMutexPath.Clear();
        CanUnioResMutex = false;
        
    }

    private static bool IsCanGeneratePrefabUnionRes(string path)
    {
        if (!CanUnioResMutex)
        {
            return false;
        }
        if (path.Contains(SceneAssetPatch))
        {
            //if(!path.Contains(sceneTerrainDataPatch))
            {
                return false;
            }
        }

        if (IsUIDynamicAssets(path))
        {
            return false;
        }

        if (IsCommonArtistAssets(path))
        {
            return false;
        }

        if (IsCommonPrefabAssets(path))
        {
            return false;
        }
        bool Result = !UnionResMutexPath.Contains(path);
        if(Result)
        {
            UnionResMutexPath.Add(path);
        }
        return Result;
    }

    public static AssetBundleManagerEditor.AssetBundleEditorInfo MakePrefabAssetBundleInfo(string patch)
    {
        AssetBundleManagerEditor.AssetBundleEditorInfo info = null;
        string lowerPatch = patch;

        if(!IsPrefabAssets(lowerPatch))
        {
            return info;
        }

        AssetImporter assetImporter = AssetImporter.GetAtPath(patch);
        if (!assetImporter)
        {
            return info;
        }

        string asPatch = assetImporter.assetPath;
        //移除头部
        asPatch = asPatch.Replace(PrefabPatch, "");
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

        string assetName = Path.GetFileNameWithoutExtension(assetImporter.assetPath);
        string guid = AssetBundleManagerEditor.GetAssetGUIDFromMeta(assetImporter.assetPath);

        if(guid.Equals(string.Empty))
        {
            return info;
        }

        string abName = "gameobject/" + asPatch + "/" + assetName;
        string variant = AssetBundleManager.PrefabVariant;
        SaveAssetBundleData(assetImporter, abName, variant);

        info = new AssetBundleManagerEditor.AssetBundleEditorInfo();
        info.AssetBundleName = abName;
        info.AssetBundleVarian = variant;
        info.guid = guid;
        info.assetName = assetName;
        return info;
    }


    public static bool IsPrefabAssets(string s)
    {
        string patch = s;
        //不是prefab,是GameManagerPrefab这个预制没有引用，不做打包处理
        if (patch.Equals(string.Empty) || !patch.EndsWith(PrefabExetion)
            || patch.Contains(GameManagerPrefab))
        {
            return false;
        }

        //不在预制体路径或者通用路径下
        if (!patch.Contains(PrefabPatch) && !IsCommonPrefabAssets (s) && !IsCommonArtistAssets(s))
        {
            return false;
        }

        return true;
    }
}
