/*******************************************************************
** 文件名:	AssetBundleManagerEditor.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	ab资源系统
** 应  用:  	ab资源管理器编辑器

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using System.IO;
using System;
using LZ4ps;
using UnityEditor.Callbacks;
using System.Linq;
/*
 * 1.资源整理的目的:
 * 1.删除不用的资源包
 * 2.统一meta
 */
[InitializeOnLoad]
public partial class AssetBundleManagerEditor : EditorWindow
{

    public static void CheckComplineError()
    {
        UnityEditor.SceneManagement.EditorSceneManager.NewScene(UnityEditor.SceneManagement.NewSceneSetup.EmptyScene);
        EditorApplication.isPlaying = true;
        
        EditorApplication.update += CheckComplineErrorUpdate;
    }

    private static void CheckComplineErrorUpdate()
    {
        EditorApplication.Exit(0);
    }

    #region 发布EXE

    public static void BuildProject_Release()
    {
        CreateFileHelper(GetCommand_BuildingReleaseProjectPath());
        AssetBundleManagerEditor uv = EditorWindow.GetWindow<AssetBundleManagerEditor>(false, "AssetBundle管理理器", true);
        uv.Show();
        uv.autoRepaintOnSceneChange = true;
    }

    /// <summary>
    /// 构建项目，给命令行使用
    /// </summary>
    public static void BuildProject_Release_CommandLine()
    {
        EditorRunTypeWindow.RunFromCommandLine();
        CreateFileHelper(GetCommand_BuildingReleaseProjectPath());
        AssetBundleManagerEditor uv = EditorWindow.GetWindow<AssetBundleManagerEditor>(false, "AssetBundle管理理器", true);
        uv.Show();
        uv.autoRepaintOnSceneChange = true;
    }

    public static void BuildProject_Release64()
    {
        CreateFileHelper(GetCommand_BuildingReleaseProjectPath64());
        AssetBundleManagerEditor uv = EditorWindow.GetWindow<AssetBundleManagerEditor>(false, "AssetBundle管理理器", true);
        uv.Show();
        uv.autoRepaintOnSceneChange = true;
    }

    public static void BuildProject_Release64_CommandLine()
    {
        EditorRunTypeWindow.RunFromCommandLine();
        CreateFileHelper(GetCommand_BuildingReleaseProjectPath64());
        AssetBundleManagerEditor uv = EditorWindow.GetWindow<AssetBundleManagerEditor>(false, "AssetBundle管理理器", true);
        uv.Show();
        uv.autoRepaintOnSceneChange = true;
    }

    public static void BuildProject_Debug()
    {
        CreateFileHelper(GetCommand_BuildingDebugProjectPath());
        AssetBundleManagerEditor uv = EditorWindow.GetWindow<AssetBundleManagerEditor>(false, "AssetBundle管理理器", true);
        uv.Show();
        uv.autoRepaintOnSceneChange = true;
    }

    /// <summary>
    /// 构建项目，给命令行使用
    /// </summary>
    public static void BuildProject_Debug_CommandLine()
    {
        EditorRunTypeWindow.RunFromCommandLine();
        CreateFileHelper(GetCommand_BuildingDebugProjectPath());
        AssetBundleManagerEditor uv = EditorWindow.GetWindow<AssetBundleManagerEditor>(false, "AssetBundle管理理器", true);
        uv.Show();
        uv.autoRepaintOnSceneChange = true;
    }

    public static void BuildProject_Debug64()
    {
        CreateFileHelper(GetCommand_BuildingDebugProjectPath64());
        AssetBundleManagerEditor uv = EditorWindow.GetWindow<AssetBundleManagerEditor>(false, "AssetBundle管理理器", true);
        uv.Show();
        uv.autoRepaintOnSceneChange = true;
    }

    public static void BuildProject_Debug64_CommandLine()
    {
        EditorRunTypeWindow.RunFromCommandLine();
        CreateFileHelper(GetCommand_BuildingDebugProjectPath64());
        AssetBundleManagerEditor uv = EditorWindow.GetWindow<AssetBundleManagerEditor>(false, "AssetBundle管理理器", true);
        uv.Show();
        uv.autoRepaintOnSceneChange = true;
    }

    private static void OnBuildProjectStart()
    {

    }

    private static void OnBuildProjectSuccess()
    {

    }

    private static void OnBuildProjectFaild()
    {

    }

    private static void BuildProject_Impl(bool Version_Debug, bool Version_64Bit)
    {
        if (Version_64Bit && !Is64bitEditor)
        {
            Debug.LogError("发布只能使用64位编辑器");
            return;
        }
        if (!Is64BitNativeDll("game.dll") && Version_64Bit)
        {
            Debug.LogError("NativeDLL不是64位版本");
            return;
        }

        if (Is64BitNativeDll("game.dll") && !Version_64Bit)
        {
            Debug.LogError("NativeDLL不是32位版本");
            return;
        }


        AssetBundleManager.InitAssetBundleThread();

        if(Version_Debug)
        {
            GenurateMDB();
        }

        SaveAndRefershAssets();
        List<string> scenes = new List<string>();

        BuildOptions options = Version_Debug ? BuildOptions.Development | BuildOptions.ConnectWithProfiler | BuildOptions.AllowDebugging : BuildOptions.None;
        BuildTarget target = Version_64Bit ? BuildTarget.StandaloneWindows64 : BuildTarget.StandaloneWindows;


        //登录场景
        scenes.Add("Assets/UnityScene/PerformanceCheck.unity");
       // scenes.Add("Assets/UnityScene/Login.unity");
        BuildPipeline.BuildPlayer(scenes.ToArray(), GetRunTimeBinPatchRoot() + GameName + "." + GameExeName, target, options);

        DeleteMainfainstFile(GetRunTimeAssetPackagePatchRoot());
        DeleteOtherFiles(GetRunTimeBinPatchRoot(), Version_Debug);

        OnPostBuildProject(Version_Debug, fromTaiTanProgrma && Version_Debug, Version_64Bit);
    }

    #endregion


    #region 资源打包

    public static bool isBuildingPackage = false;

    public static void BuildFullPackage_CommandLine()
    {
        EditorRunTypeWindow.RunFromCommandLine();

        CreateFileHelper(GetCommand_PackingAssetsPath());
        AssetBundleManagerEditor uv = EditorWindow.GetWindow<AssetBundleManagerEditor>(false, "AssetBundle管理理器", true);
        uv.Show();
        uv.autoRepaintOnSceneChange = true;

    }
    public static void BuildFullPackage64_CommandLine()
    {
        EditorRunTypeWindow.RunFromCommandLine();

        CreateFileHelper(GetCommand_PackingAssetsPath64());
        AssetBundleManagerEditor uv = EditorWindow.GetWindow<AssetBundleManagerEditor>(false, "AssetBundle管理理器", true);
        uv.Show();
        uv.autoRepaintOnSceneChange = true;

    }

    public static void BuildFullPackage()
    {
        CreateFileHelper(GetCommand_PackingAssetsPath());
        AssetBundleManagerEditor uv = EditorWindow.GetWindow<AssetBundleManagerEditor>(false, "AssetBundle管理理器", true);
        uv.Show();
        uv.autoRepaintOnSceneChange = true;

    }

    public static void BuildFullPackage64()
    {
        CreateFileHelper(GetCommand_PackingAssetsPath64());
        AssetBundleManagerEditor uv = EditorWindow.GetWindow<AssetBundleManagerEditor>(false, "AssetBundle管理理器", true);
        uv.Show();
        uv.autoRepaintOnSceneChange = true;

    }

    private static void OnBuildFullPackageStart()
    {

    }

    private static void OnBuildFullPackageSuccess()
    {

    }

    private static void OnBuildFullPackageFaild()
    {

    }
    public static void BuildFullPackage_Impl(bool Version_64Bit)
    {
        if (!Is64bitEditor)
        {
            Debug.LogError("发布只能使用64位编辑器");
            return;
        }
        //if (!Is64BitNativeDll("game.dll") && Version_64Bit)
        //{
        //    Debug.LogError("NativeDLL不是64位版本");
        //    return;
        //}
        //if (Is64BitNativeDll("game.dll") && !Version_64Bit)
        //{
        //    Debug.LogError("NativeDLL不是32位版本");
        //    return;
        //}

        //DestroyAllBuiltInMaterial();
        //DestroyAllBuiltInShader();
        isBuildingPackage = true;
        Console.Clear();

        SaveAndRefershAssets();

        Dictionary<string, AssetBundleManager.PackageNode> OldResFileList = new Dictionary<string, AssetBundleManager.PackageNode>();
        Dictionary<string, AssetBundleManager.PackageNode> OldOtherFileList = new Dictionary<string, AssetBundleManager.PackageNode>();

        //打包之前，先吧资源列表加载进来,后面用于做更新对比
        AssetBundleManager.LoadPackageList(GetEditorAssetPackagePatchRoot() + AssetBundleManager.AssetPackageListPatch, OldResFileList, OldOtherFileList);

        /*
         * 先生成预制体配置，因为有的资源再导入之前是没有assetbundle的
         */
        EditorLog.LogInfo("正在计算预制体信息....", true);
        if (BuildPrePrefabConfig || BuildAll)
        {
            PreGeneratePrefabConfig();
            SaveAndRefershAssets();
        }

        if (BuildPacakage || BuildAll)
        {
            EditorLog.LogInfo("正在打包资源文件....", true);

            if (!System.IO.Directory.Exists(AssetBundleManager.FullAssetPackagePathRoot) && !File.Exists(AssetBundleManager.FullAssetPackagePathRoot))
            {
                System.IO.Directory.CreateDirectory(AssetBundleManager.FullAssetPackagePathRoot);
            }

            BuildPipeline.BuildAssetBundles(AssetBundleManager.FullAssetPackagePathRoot, BuildAssetBundleOptions.UncompressedAssetBundle | BuildAssetBundleOptions.DeterministicAssetBundle, Version_64Bit ? BuildTarget.StandaloneWindows64 : BuildTarget.StandaloneWindows);
            SaveAndRefershAssets();
        }



        BuildCurrAssetBundleList();

        if (BuildUIConfig || BuildAll)
        {
            EditorLog.LogInfo("正在计算UI配置....", true);
            GenerateUIConfig();
        }
        if (BuildSceneConfig || BuildAll)
        {
            EditorLog.LogInfo("正在计算场景配置....", true);
            GenerateScenConfig();
        }

        if (BuildScheme || BuildAll)
        {
            EditorLog.LogInfo("正在计算Scheme....", true);
            GenerateScheme();
            ExportLightingEffectConfig();
        }

        if (BuildPostPrefabConfig || BuildAll)
        {
            EditorLog.LogInfo("正在合并资源文件....", true);
            PostGeneratePrefabConfig();
        }

        if (BuildAssetsInfo || BuildAll)
        {
            EditorLog.LogInfo("正在计算资源信息....", true);
            GenerateEditorAssetInfo(OldResFileList, OldOtherFileList);
        }

        SaveAndRefershAssets();
        //DeleteMainfainstFile(GetEditorAssetPackagePatchRoot());
        isBuildingPackage = false;

        EditorLog.LogInfo("正在修复打包错误文件...",true);

        if (AssetBundleImporterFixedEditor.DoImporterFixed("CopyEditorLog_BuildPackage.log", "FixedFillPathInfo_BuildPackage.txt"))
        {
            foreach (AssetBundleImporterFixedEditor.FixedPathNode path in AssetBundleImporterFixedEditor.FixedFilePath)
            {
                EditorLog.LogInfo("修复文件成功:" + path.getDesc() + ",可重新打包或者下次打包...", true);
            }

            CreateFileHelper(GetCommand_NeedToRePackingAssets());
        }
        else
        {
            EditorLog.LogInfo("没有发现需要修复的文件...", true);
        }
    }

    private static void GenerateUIConfig()
    {
        PostprocessUITextureToCSV.nEditorInstance.UIAssetImport();

    }

    private static void ExportLightingEffectConfig()
    {
        LightingEffectExportEditor.ExportLightEffect();
    }

    private static void GenerateScheme()
    {

        string[] allScheme = AssetDatabase.FindAssets("t:Object", new string[] { AssetBundleImporter.SchemePatch.Remove(AssetBundleImporter.SchemePatch.Length - 1, 1) });

        foreach (string str in allScheme)
        {
            AssetBundleImporter.MakeSchemeAssetBundleInfo(AssetDatabase.GUIDToAssetPath(str));
        }
        AssetBundleImporter.OnPostProcessScheme();

    }

    /// <summary>
    /// 计算场景配置
    /// </summary>
    private static void GenerateScenConfig()
    {

        List<string> folders = new List<string>();


        //取得场景资源目录下的所有scene
        folders.AddRange(AssetDatabase.FindAssets("t:Object", new string[] { AssetBundleImporter.scenePatch.Remove(AssetBundleImporter.scenePatch.Length - 1, 1) }));

        string haderLine1 = "场景名,guid,场景AssetBundle";
        string haderLine2 = "string,string,string";

        StreamWriter writer = new StreamWriter(AssetBundleManager.FullAssetPackagePathRoot + AssetBundleManager.SceneConfigPatch, false, new System.Text.UTF8Encoding(true));

        writer.WriteLine(haderLine1);
        writer.WriteLine(haderLine2);
        foreach (string s in folders)
        {
            string patch = AssetDatabase.GUIDToAssetPath(s);
            if (!patch.Contains(".unity"))
            {
                continue;
            }
            patch = patch.Replace("\\", "/");
            AssetBundleEditorInfo pdata = AssetBundleImporter.MakeSceneAssetBundleInfoFromReference(patch);
            if (null == pdata)
            {
                Debug.LogWarning("场景取得资源配置失败，不会导出:" + patch);
                continue;
            }
            writer.WriteLine(pdata.assetName + "," + pdata.guid + "," + pdata.FullAssetBundleName);

        }
        writer.Flush();
        writer.Close();

    }

    /// <summary>
    /// 修正运行时资源文件，删除应该删除东西
    /// </summary>
    private static void CorrectRunTimeAssetFile(Dictionary<string, AssetBundleManager.PackageNode> NewResFileList)
    {
        //计算运行时多余的包，然后删除
        List<string> folders = new List<string>();

        /*除了meta、资源列表、mainifest，全都要更新
        * 去掉对PrefabVariant的版本记载,改用合并的
        */

        if (!System.IO.Directory.Exists(GetRunTimeAssetPackagePatchRoot()))
        {
            return;
        }

        folders.AddRange(System.IO.Directory.GetFiles(GetRunTimeAssetPackagePatchRoot(), "*.*", SearchOption.AllDirectories).Where(
                        str =>
                        {
                            foreach (string excPath in ResExcPathList)
                            {
                                if (str.Contains(excPath))
                                {
                                    return false;
                                }
                            }
                            return true;
                        })
                    );

        foreach (string deletePath in folders)
        {
            string packageName = deletePath.Replace(GetRunTimeAssetPackagePatchRoot(), string.Empty);
            packageName = packageName.Replace("\\", "/");
            if (!NewResFileList.ContainsKey(packageName))
            {
                DeleteFileHelper(deletePath);
            }
        }
    }

    /// <summary>
    /// 指资源包信息
    /// </summary>
    private static void GenerateEditorAssetInfo(Dictionary<string, AssetBundleManager.PackageNode> OldResFileList, Dictionary<string, AssetBundleManager.PackageNode> OldOtherFileList)
    {
        //编辑器下的res表
        Dictionary<string, AssetBundleManager.PackageNode> EditorNewResFileList = new Dictionary<string, AssetBundleManager.PackageNode>();

        EditorNewResFileList = null;
        GenerateAssetPackageList_Res(AssetBundleManager.FullAssetPackagePathRoot, ref EditorNewResFileList, false);

        //将改变的资源名字保存出去,因为加密前后的MD5不一样，区别新增的只能在这里,用于后面做加密
        List<string> ResModifList = null;
        List<string> ResDeleteList = null;
        GenerateDetail(EditorNewResFileList, OldResFileList, ref ResModifList, ref ResDeleteList);
        WriteDetailFile(GetResDetailFileNamePath(), ResModifList, ResDeleteList);

        //本次打包应该删除的包，运行时的资源包和editor下的资源包一起干掉
        foreach (string DeletePath in ResDeleteList)
        {
            string RunTimePath = GetRunTimeAssetPackagePatchRoot() + DeletePath;
            string EditorPath = GetEditorAssetPackagePatchRoot() + DeletePath;
            DeleteFileHelper(RunTimePath);
            DeleteFileHelper(EditorPath);

        }

        ExportPackageListToEditor(EditorNewResFileList, OldOtherFileList);
        OldResFileList = null;
        OldOtherFileList = null;

    }

    private static void CopyMapPtchFolder()
    {
        CopyDirectory(GetEditorMapPathFolder(), GetRunTimeMapPathFolder());
    }

    private static void CopyMSDll()
    {
        string cp100Name = "msvcp100.dll";
        string cr100Name = "msvcr100.dll";
        string avcodecName = "avcodec-57.dll";
        string avdeviceName = "avdevice-57.dll";
        string avfilterName = "avfilter-6.dll";
        string avformatName = "avformat-57.dll";
        string avutilName = "avutil-55.dll";
        string postprocName = "postproc-54.dll";
        string swresampleName = "swresample-2.dll";
        string swscaleName = "swscale-4.dll";

        string msvcp100 = GetClient_GamePathRoot() + cp100Name;
        string msvcr100 = GetClient_GamePathRoot() + cr100Name;
        string ffmpegavcodec = GetClient_GamePathRoot() + avcodecName;
        string ffmpegavdevice = GetClient_GamePathRoot() + avdeviceName;
        string ffmpegavfilter = GetClient_GamePathRoot() + avfilterName;
        string ffmpegavformat = GetClient_GamePathRoot() + avformatName;
        string ffmpegavutil = GetClient_GamePathRoot() + avutilName;
        string ffmpegpostproc = GetClient_GamePathRoot() + postprocName;
        string ffmpegswresample = GetClient_GamePathRoot() + swresampleName;
        string ffmpegswscale = GetClient_GamePathRoot() + swscaleName;


        if (!File.Exists(msvcp100))
        {
            Debug.LogError("找不到文件:" + msvcp100 + ",请联系程序");
        }

        if (!File.Exists(msvcr100))
        {
            Debug.LogError("找不到文件:" + msvcr100 + ",请联系程序");
        }

        if (!File.Exists(ffmpegavcodec))
        {
            Debug.LogError("找不到文件:" + ffmpegavcodec + ",请联系程序");
        }

        if (!File.Exists(ffmpegavdevice))
        {
            Debug.LogError("找不到文件:" + ffmpegavdevice + ",请联系程序");
        }

        if (!File.Exists(ffmpegavfilter))
        {
            Debug.LogError("找不到文件:" + ffmpegavfilter + ",请联系程序");
        }

        if (!File.Exists(ffmpegavformat))
        {
            Debug.LogError("找不到文件:" + ffmpegavformat + ",请联系程序");
        }

        if (!File.Exists(ffmpegavutil))
        {
            Debug.LogError("找不到文件:" + ffmpegavutil + ",请联系程序");
        }

        if (!File.Exists(ffmpegpostproc))
        {
            Debug.LogError("找不到文件:" + ffmpegpostproc + ",请联系程序");
        }

        if (!File.Exists(ffmpegswresample))
        {
            Debug.LogError("找不到文件:" + ffmpegswresample + ",请联系程序");
        }

        if (!File.Exists(ffmpegswscale))
        {
            Debug.LogError("找不到文件:" + ffmpegswscale + ",请联系程序");
        }

        CopyFileHelper(msvcp100, GetRunTimeBinPatchRoot() + cp100Name);
        CopyFileHelper(msvcr100, GetRunTimeBinPatchRoot() + cr100Name);
        CopyFileHelper(ffmpegavcodec, GetRunTimeBinPatchRoot() + avcodecName);
        CopyFileHelper(ffmpegavdevice, GetRunTimeBinPatchRoot() + avdeviceName);
        CopyFileHelper(ffmpegavfilter, GetRunTimeBinPatchRoot() + avfilterName);
        CopyFileHelper(ffmpegavformat, GetRunTimeBinPatchRoot() + avformatName);
        CopyFileHelper(ffmpegavutil, GetRunTimeBinPatchRoot() + avutilName);
        CopyFileHelper(ffmpegpostproc, GetRunTimeBinPatchRoot() + postprocName);
        CopyFileHelper(ffmpegswresample, GetRunTimeBinPatchRoot() + swresampleName);
        CopyFileHelper(ffmpegswscale, GetRunTimeBinPatchRoot() + swscaleName);
    }
    /// <summary>
    /// 指除了资源包意外的信息，如代码，配置等等
    /// </summary>
    private static void GenerateOtherInfo(bool debugVersion, bool is64Bit)
    {
        PackingScp();
        CopyMono(is64Bit);
        CopyUIDll();
        CopyMapPtchFolder();
        CopyMSDll();

        Dictionary<string, AssetBundleManager.PackageNode> OldOtherFileList = new Dictionary<string, AssetBundleManager.PackageNode>();
        Dictionary<string, AssetBundleManager.PackageNode> NewOtherFileList = new Dictionary<string, AssetBundleManager.PackageNode>();
        Dictionary<string, AssetBundleManager.PackageNode> NewResFileList = new Dictionary<string, AssetBundleManager.PackageNode>();
        //把信息加载进来.
        AssetBundleManager.LoadPackageList(GetEditorAssetPackagePatchRoot() + AssetBundleManager.AssetPackageListPatch, NewResFileList, OldOtherFileList);

        NewOtherFileList = null;
        GenerateAssetPackageList_Other(ref NewOtherFileList, debugVersion);

        List<string> OtherModifList = null;
        List<string> OtherDeleteList = null;
        GenerateDetail(NewOtherFileList, OldOtherFileList, ref OtherModifList, ref OtherDeleteList);
        WriteDetailFile(GetOtherDetailFileNamePath(), OtherModifList, OtherDeleteList);

        foreach (string str in OtherDeleteList)
        {
            DeleteFileHelper(GetRunTimeBinPatchRoot() + str);
        }
        //复制unity的dll到runtime下，只有真正发布的时候，unity才会复制，其他重新取的时候，unity不会复制
        foreach (string s in OtherModifList)
        {
            string fileName = Path.GetFileName(s);
            CopyUnityDll(fileName, debugVersion);
        }

        if (!fromTaiTanProgrma)
        {
            DotfuscatorScripts_RunTime();
        }

        OldOtherFileList = null;
        //将配置导出给editor
        ExportPackageListToEditor(NewResFileList, NewOtherFileList);
    }

    /// <summary>
    /// 预计算预制体配置，最后还要进行合并
    /// </summary>
    private static void PreGeneratePrefabConfig()
    {
        //先刷新一遍资源数据库，把所有的资源都丢进来
        SaveAndRefershAssets();

 
        DeleteFileHelper(AssetBundleManager.GetPrefabConfigPath());
        DeleteFileHelper(AssetBundleManager.GetDependenicesResConfigPath());

        AssetBundleManager.ClearPrefabConfig();

        AssetBundleImporter.BeginPrefabAssetImport();
        AssetBundleImporter.BeginGenerateUnionRes();
        string[] assetss = AssetDatabase.FindAssets("t:Object", new string[] { AssetBundleImporter.PrefabPatch.Remove(AssetBundleImporter.PrefabPatch.Length - 1, 1),
                                                                             AssetBundleImporter.ArtistCommonAssetsPatch.Remove(AssetBundleImporter.ArtistCommonAssetsPatch.Length - 1, 1)});
        
        foreach (string patch1 in assetss)
        {
            string patch = AssetDatabase.GUIDToAssetPath(patch1);
            AssetBundleImporter.ImportSinglePrefab(patch, false);
        }

        AssetBundleImporter.EndGenerateUnionRes();
        AssetBundleImporter.EndPrefabAssetImport();
        SaveAndRefershAssets();
    }

    private static void PostGeneratePrefabConfig()
    {
        List<string> fileList = new List<string>();

        fileList.AddRange(System.IO.Directory.GetFiles(AssetBundleManager.FullAssetPackagePathRoot, "*." + AssetBundleManager.PrefabVariant, SearchOption.AllDirectories));

        int maxUnionCount = 50;
        int currentUnionCount = 0;
        int curUnionIndex = 0;
        string dstDir = AssetBundleManager.FullAssetPackagePathRoot + AssetBundleManager.PrefabUnionVariant + "/";

        if (System.IO.Directory.Exists(dstDir))
        {
            System.IO.Directory.Delete(dstDir, true);
        }

        System.IO.Directory.CreateDirectory(dstDir);
        string currFileName = dstDir + curUnionIndex + "." + AssetBundleManager.PrefabUnionVariant;
        FileStream unionFile = File.Open(currFileName, FileMode.Append, FileAccess.Write);
        if (null == unionFile)
        {
            Debug.LogError("资源打包重大错误!--union");
            return;
        }
        int currDataOffset = 0;
        foreach (string filePath in fileList)
        {
            Byte[] srcData = File.ReadAllBytes(filePath);
            if (srcData.Length <= 0)
            {
                Debug.LogWarning("union + 读取文件失败:" + filePath);
                continue;
            }
            string abName;
            GetAssetBundNameFromAssetFilePath(filePath, out abName);
            if(!IsOnAssetBundle(abName))
            {
                Debug.LogWarning("IsOnAssetBundle失败:" + filePath);
                continue;
            }
            if (AssetBundleManager.SupplmentConfig(abName, curUnionIndex, currDataOffset, srcData.Length))
            {
                unionFile.Write(srcData, 0, srcData.Length);
                currDataOffset += srcData.Length;
                currentUnionCount++;
            }


            if (currentUnionCount >= maxUnionCount)
            {
                unionFile.Flush();
                unionFile.Close();

                curUnionIndex++;
                currentUnionCount = 0;
                currDataOffset = 0;

                currFileName = dstDir + curUnionIndex + "." + AssetBundleManager.PrefabUnionVariant;
                unionFile = File.Open(currFileName, FileMode.Append, FileAccess.Write);
            }

        }

        unionFile.Flush();
        unionFile.Close();

        SchemeExport.Export_PrefabScheme();
    }

    #endregion


    #region 程序发布版
    //[MenuItem("Tools/BuildProject(Debug)")]
    //public static void BuildProject_Full()
    //{
    //    if (Application.isPlaying)
    //    {
    //        Debug.LogError("不能在播放模式下生成客户端");
    //        return;
    //    }
    //    if (Is64BitNativeDll("game.dll"))
    //    {
    //        Debug.LogError("NativeDLL不是32位版本");
    //        return;
    //    }
    //    fromTaiTanProgrma = true;
    //    AssetBundleManager.InitAssetBundleThread();

    //    isBuildingPackage = true;
    //   // PreGeneratePrefabConfig();
    //   // SaveAndRefershAssets();

    //   // BuildCurrAssetBundleList();

    //   // PostGeneratePrefabConfig();

    //    isBuildingPackage = false;
    //    BuildProject_Impl(true,false);
    //    System.Diagnostics.Process.Start(GetRunTimeBinPatchRoot());
    //    fromTaiTanProgrma = false;
    //}

    //[MenuItem("Tools/BuildProject64(Debug)")]
    //public static void BuildProject64_Full()
    //{
    //    if (Application.isPlaying)
    //    {
    //        Debug.LogError("不能在播放模式下生成客户端");
    //        return;
    //    }
    //    if (!Is64bitEditor)
    //    {
    //        Debug.LogError("只能使用64位编辑器发布");
    //        return;
    //    }
    //    if (!Is64BitNativeDll("game.dll"))
    //    {
    //        Debug.LogError("NativeDLL不是64位版本");
    //        return;
    //    }
    //    fromTaiTanProgrma = true;
    //    AssetBundleManager.InitAssetBundleThread();

    //    isBuildingPackage = true;
    //   // PreGeneratePrefabConfig();
    //   // SaveAndRefershAssets();

    //   // BuildCurrAssetBundleList();

    //   // PostGeneratePrefabConfig();
    //    isBuildingPackage = false;
    //    BuildProject_Impl(true,true);
    //    System.Diagnostics.Process.Start(GetRunTimeBinPatchRoot());
    //    fromTaiTanProgrma = false;
    //}


    //public static void BuildProject_Release_Full()
    //{
    //    if (Application.isPlaying)
    //    {
    //        Debug.LogError("不能在播放模式下生成客户端");
    //        return;
    //    }
    //    if (Is64BitNativeDll("game.dll"))
    //    {
    //        Debug.LogError("NativeDLL不是32位版本");
    //        return;
    //    }
    //    fromTaiTanProgrma = true;
    //    AssetBundleManager.InitAssetBundleThread();

    //    isBuildingPackage = true;
    //    // PreGeneratePrefabConfig();
    //    // SaveAndRefershAssets();

    //    // BuildCurrAssetBundleList();

    //    // PostGeneratePrefabConfig();

    //    isBuildingPackage = false;
    //    BuildProject_Impl(false, false);
    //    System.Diagnostics.Process.Start(GetRunTimeBinPatchRoot());
    //    fromTaiTanProgrma = false;
    //}


    //public static void BuildProject64_Release_Full()
    //{
    //    if (Application.isPlaying)
    //    {
    //        Debug.LogError("不能在播放模式下生成客户端");
    //        return;
    //    }
    //    if (!Is64bitEditor)
    //    {
    //        Debug.LogError("只能使用64位编辑器发布");
    //        return;
    //    }
    //    if (!Is64BitNativeDll("game.dll"))
    //    {
    //        Debug.LogError("NativeDLL不是64位版本");
    //        return;
    //    }
    //    fromTaiTanProgrma = true;
    //    AssetBundleManager.InitAssetBundleThread();

    //    isBuildingPackage = true;
    //    // PreGeneratePrefabConfig();
    //    // SaveAndRefershAssets();

    //    // BuildCurrAssetBundleList();

    //    // PostGeneratePrefabConfig();
    //    isBuildingPackage = false;
    //    BuildProject_Impl(false, true);
    //    System.Diagnostics.Process.Start(GetRunTimeBinPatchRoot());
    //    fromTaiTanProgrma = false;
    //}

    #endregion


    //[MenuItem("Assets/ExportToPackage")]
    //private static void ExportSinglePackage()
    //{
    //    AssetBundleManager.InitAssetBundleThread();
    //    UnityEngine.Object[] ObjToPackage = Selection.GetFiltered(typeof(UnityEngine.Object), SelectionMode.DeepAssets);
    //    Dictionary<string, List<UnityEngine.Object>> assetTableToPackage = new Dictionary<string, List<UnityEngine.Object>>();

    //    bool bExported = false;
    //    foreach (UnityEngine.Object o in ObjToPackage)
    //    {
    //        string path = AssetDatabase.GetAssetPath(o);

    //        if (System.IO.Directory.Exists(ConverRelativePatchToObslotePatch(path)))
    //        {
    //            continue;
    //        }
    //        AssetImporter imp = AssetImporter.GetAtPath(path);
    //        if (imp.assetBundleName.Equals(string.Empty))
    //        {
    //            Debug.LogWarning(o.name + "没有设置ab名称，不是打包的资源,跳过");
    //            continue;
    //        }

    //        string assetbundleName = imp.assetBundleName + "." + imp.assetBundleVariant;
    //        List<UnityEngine.Object> list = null;
    //        if (assetTableToPackage.TryGetValue(assetbundleName, out list))
    //        {
    //            list.Add(o);
    //        }
    //        else
    //        {
    //            list = new List<UnityEngine.Object>();
    //            list.Add(o);
    //            assetTableToPackage.Add(assetbundleName, list);
    //        }
    //    }
    //    bool havePrefab = false;
    //    foreach (KeyValuePair<string, List<UnityEngine.Object>> packageNode in assetTableToPackage)
    //    {
    //        string targetpath = AssetBundleManager.FullAssetPackagePathRoot + packageNode.Key;

    //        string folderpath = System.IO.Path.GetDirectoryName(targetpath);
    //        if (!System.IO.Directory.Exists(folderpath))
    //        {
    //            System.IO.Directory.CreateDirectory(folderpath);
    //        }
    //        BuildPipeline.BuildAssetBundle(null, packageNode.Value.ToArray(), targetpath, BuildAssetBundleOptions.CompleteAssets | BuildAssetBundleOptions.CollectDependencies | BuildAssetBundleOptions.UncompressedAssetBundle, BuildTarget.StandaloneWindows);
    //        bExported = true;
    //        if (!havePrefab)
    //        {
    //            havePrefab = packageNode.Key.Contains(AssetBundleManager.PrefabVariant);
    //        }

    //    }

    //    if (!bExported)
    //    {
    //        Debug.Log("没有资源导出");
    //    }
    //    else
    //    {
    //        CopyExportSinglePackageFileToEXEFolder(havePrefab);
    //        Debug.Log("资源包导出完成");
    //    }
    //}

    //[MenuItem("Assets/ExportToPackage64")]
    //private static void ExportSinglePackage64()
    //{
    //    AssetBundleManager.InitAssetBundleThread();
    //    UnityEngine.Object[] ObjToPackage = Selection.GetFiltered(typeof(UnityEngine.Object), SelectionMode.DeepAssets);
    //    Dictionary<string, List<UnityEngine.Object>> assetTableToPackage = new Dictionary<string, List<UnityEngine.Object>>();

    //    bool bExported = false;
    //    foreach (UnityEngine.Object o in ObjToPackage)
    //    {
    //        string path = AssetDatabase.GetAssetPath(o);

    //        if (System.IO.Directory.Exists(ConverRelativePatchToObslotePatch(path)))
    //        {
    //            continue;
    //        }
    //        AssetImporter imp = AssetImporter.GetAtPath(path);
    //        if (imp.assetBundleName.Equals(string.Empty))
    //        {
    //            Debug.LogWarning(o.name + "没有设置ab名称，不是打包的资源,跳过");
    //            continue;
    //        }

    //        string assetbundleName = imp.assetBundleName + "." + imp.assetBundleVariant;
    //        List<UnityEngine.Object> list = null;
    //        if (assetTableToPackage.TryGetValue(assetbundleName, out list))
    //        {
    //            list.Add(o);
    //        }
    //        else
    //        {
    //            list = new List<UnityEngine.Object>();
    //            list.Add(o);
    //            assetTableToPackage.Add(assetbundleName, list);
    //        }
    //    }
    //    bool havePrefab = false;
    //    foreach (KeyValuePair<string, List<UnityEngine.Object>> packageNode in assetTableToPackage)
    //    {
    //        string targetpath = AssetBundleManager.FullAssetPackagePathRoot + packageNode.Key;

    //        string folderpath = System.IO.Path.GetDirectoryName(targetpath);
    //        if (!System.IO.Directory.Exists(folderpath))
    //        {
    //            System.IO.Directory.CreateDirectory(folderpath);
    //        }
    //        BuildPipeline.BuildAssetBundle(null, packageNode.Value.ToArray(), targetpath, BuildAssetBundleOptions.CompleteAssets | BuildAssetBundleOptions.CollectDependencies | BuildAssetBundleOptions.UncompressedAssetBundle, BuildTarget.StandaloneWindows64);
    //        bExported = true;
    //        if (!havePrefab)
    //        {
    //            havePrefab = packageNode.Key.Contains(AssetBundleManager.PrefabVariant);
    //        }

    //    }

    //    if (!bExported)
    //    {
    //        Debug.Log("没有资源导出");
    //    }
    //    else
    //    {
    //        CopyExportSinglePackageFileToEXEFolder(havePrefab);
    //        Debug.Log("资源包导出完成");
    //    }
    //}

    [MenuItem("Assets/ExportManagedScriptToRunTime")]
    private static void ExportManagedScriptToRunTime()
    {
        AssetBundleManager.InitAssetBundleThread();
        UnityEngine.Object[] ObjToPackage = Selection.GetFiltered(typeof(UnityEngine.Object), SelectionMode.DeepAssets);
        foreach (UnityEngine.Object o in ObjToPackage)
        {
            string path = AssetDatabase.GetAssetPath(o);
            path = ConverRelativePatchToObslotePatch(path);

            //文件夹，跳过
            if (System.IO.Directory.Exists(path))
            {
                continue;
            }

            //不是脚本
            if (!o.name.Contains("U3D_") && !o.name.Contains("GameAPI_"))
            {
                continue;
            }

            if (!path.EndsWith(".dll"))
            {
                continue;
            }
            string destPath = GetRunTimeBinPatchRoot() + GameDataName + "/Managed/" + o.name + ".dll";
            EnCodeScriptHelper(path, destPath);
            string srcPdbFile = path.Replace(".dll", ".pdb");
            string DstPdbFile = destPath.Replace(".dll", ".pdb");

            CopyFileHelper(srcPdbFile, DstPdbFile);

            string srcMdbFile = path + ".mdb";
            string DstMdbFile = destPath + ".mdb";
            CopyFileHelper(srcMdbFile, DstMdbFile);
        }
        Debug.Log("脚本导出完成");
    }

    ///// <summary>
    ///// 将导出的资源包复制到EXE目录
    ///// </summary>
    //private static void CopyExportSinglePackageFileToEXEFolder(bool havePrefab)
    //{
    //    string exePath = GetRunTimeBinPatchRoot();
    //    if (!File.Exists(exePath + GameName + "." + GameExeName) ||
    //       !System.IO.Directory.Exists(exePath + GameDataName))
    //    {
    //        Debug.LogWarning("在:" + GetRunTimeBinPatchRoot() + "找不到TTClient.exe或者TTClient_Data,请将exe发布到这个目录下，否则要自己复制文件过去");
    //        return;
    //    }

    //    Dictionary<string, AssetBundleManager.PackageNode> OldResFileList = new Dictionary<string, AssetBundleManager.PackageNode>();
    //    Dictionary<string, AssetBundleManager.PackageNode> OldOtherFileList = new Dictionary<string, AssetBundleManager.PackageNode>();

    //    //打包之前，先吧资源列表加载进来,后面用于做更新对比
    //    AssetBundleManager.LoadPackageList(GetEditorAssetPackagePatchRoot() + AssetBundleManager.AssetPackageListPatch, OldResFileList, OldOtherFileList);

    //    BuildCurrAssetBundleList();

    //    if (havePrefab)
    //    {
    //        AssetBundleManager.LoadPrefabConfig();

    //        PostGeneratePrefabConfig();

    //        SchemeExport.Export_PrefabScheme();
    //    }

    //    GenerateEditorAssetInfo(OldResFileList, OldOtherFileList);



    //    List<string> ResModifList = null;
    //    List<string> ResDeleteList = null;
    //    ReadDetailFile(GetResDetailFileNamePath(), ref ResModifList, ref ResDeleteList);
    //    UpdateRunTime_Res(ResModifList, ResDeleteList,true);

    //    CopyFileHelper(GetResDetailFileNamePath(), GetResDetailFileNamePath() + backFileExe);
    //    DeleteFileHelper(GetResDetailFileNamePath());
    //}

    private static void BuildCurrAssetBundleList()
    {
        currAssetBundleNames.Clear();
        foreach (string s in AssetDatabase.GetAllAssetBundleNames())
        {

            currAssetBundleNames.Add(s);
        }

    }

    private static bool IsOnAssetBundle(string assetBundleName)
    {
        return currAssetBundleNames.Contains(assetBundleName);
    }


}
