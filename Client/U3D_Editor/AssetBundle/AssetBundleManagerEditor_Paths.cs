/*******************************************************************
** 文件名:	AssetBundleManagerEditor_Extract.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	ab资源系统
** 应  用:  	ab资源管理器编辑器,提取资源更新

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
using System.Text;
using System.Linq;

public partial class AssetBundleManagerEditor : EditorWindow
{

    static string GameName = "TTClient";
    static string GameDataName = GameName + "_Data";
    static string GameExeName = "exe";
    static string GameResPatch = AssetBundleManager.AssetBundlePathName + "/";


    static string backFileExe = ".copy";
    /// <summary>
    /// 其他文件排除列表,在收集其他文件的时候，会排除这里面的列表，使用string.Contains来排除
    /// 这个目录是相对Client/Bin目录,只包括里面的文件夹，不包括文件
    /// </summary>
    static string[] OtherFloderExcPathList = new string[] { "Data/User", "Data/Scp", backFileExe,GameResPatch,".txt",
                                                      ".html",".dmp",".log",".rar","副本"
                                                    };

    /// <summary>
    /// 其他文件排除列表,在收集其他文件的时候，会排除这里面的列表，使用string.Contains来排除
    /// 这个目录是相对Client/Bin目录,只包括里面的文件，不包含文件夹
    /// </summary>
    static string[] OtherTopFolderExcPathList = new string[] { backFileExe,".txt",".html",".dmp",".log","ICSharpCode.NRefactory","Mono","U3D_","Unity",
                                                      "GameAPI_",".rar",".csv",".xml","副本"
                                                    };

    /// <summary>
    /// 资源文件的排除列表
    /// </summary>
    static string[] ResExcPathList = new string[] { backFileExe,".pkl" ,".meta",".manifest"};

    /// <summary>
    ///  C# DLL路径，相对于Client\Game目录
    /// </summary>
    static string[] UnityDllsPath = new string[] { "Assets/Scripts/U3D_ImageEffect.dll","Assets/Scripts/U3D_Render.dll","Assets/Standard Assets/U3D_StandardAssets.dll",
                                                   "Assets/Scripts/U3D_UIFramework.dll","Assets/Scripts/GameAPI_Wrapper.dll"};


    /// <summary>
    ///  忽略打包的文件路径，相对于 GetScpPathRoot目录
    /// </summary>
    static string[] NotPackingScpFile = new string[] { "Condition.csv", "Effect.csv", "Spell.csv", };

    private static string ResDetailFileName = "ResPackageDetaillist.pkl";
    private static string OtherDetailFileName = "OtherPackageDetaillist.pkl";


    /// <summary>
    /// 取得editor目录下的mappatch路径
    /// </summary>
    /// <returns></returns>
    private static string GetEditorMapPathFolder()
    {
        return GetClient_GamePathRoot() + "MapPath";
    }

    /// <summary>
    /// 取得runtime目录下的mappatch路径
    /// </summary>
    /// <retur
    private static string GetRunTimeMapPathFolder()
    {
        return GetRunTimeBinPatchRoot();
    }

    /// <summary>
    /// 获取资源修改的文件列表
    /// </summary>
    /// <returns></returns>
    private static string GetResDetailFileNamePath()
    {
        return GetClient_GamePathRoot() + ResDetailFileName;
    }

    /// <summary>
    /// 获取资源修改的文件列表
    /// </summary>
    /// <returns></returns>
    private static string GetOtherDetailFileNamePath()
    {
        return GetClient_GamePathRoot() + OtherDetailFileName;;
    }

    private static void GenerateVersionAndPatchRoot(out string version, out string FolderPatch)
    {
        version = DateTime.Now.Year.ToString();
        version += "-";
        version += DateTime.Now.Month;
        version += "-";
        version += DateTime.Now.Day;
        version += "-";
        int index = 1;

        string relayVersion = version;
        relayVersion += index.ToString("x2");

        //先创建一个文件夹，用于存放更新的文件
        FolderPatch = GetUpdateInfoPatchRoot();

        FolderPatch = FolderPatch + AssetBundleManager.UpdateInfo_UpdateFolderName + relayVersion;

        while (System.IO.Directory.Exists(FolderPatch) && index < 100)
        {
            index++;
            relayVersion = version;
            relayVersion += index.ToString("x2");
            FolderPatch = GetUpdateInfoPatchRoot();
            FolderPatch = FolderPatch + AssetBundleManager.UpdateInfo_UpdateFolderName + relayVersion;
        }
        if (index >= 100)
        {
            Debug.LogWarning("文件夹子版本超出上限，取上限文件夹，将会删除原来的文件夹.");
            System.IO.Directory.Delete(FolderPatch, true);
        }
        version = relayVersion;
    }

    static string UpdateInfoPatch = string.Empty;
    private static string GetUpdateInfoPatchRoot()
    {
        if(UpdateInfoPatch.Equals(string.Empty))
        {
            string resPatch = Application.streamingAssetsPath;
            resPatch = resPatch.Replace("Game/Assets/StreamingAssets", "");
            UpdateInfoPatch = resPatch + "UpdatePackage/";
        }

        return UpdateInfoPatch;
    }

    static string RunTimeBinPatch = string.Empty;
    private static string GetRunTimeBinPatchRoot()
    {
        if(RunTimeBinPatch.Equals(string.Empty))
        {
            string resPatch = Application.streamingAssetsPath;
            resPatch = resPatch.Replace("Game/Assets/StreamingAssets", "");
            RunTimeBinPatch = resPatch + "Bin/";
        }
        return RunTimeBinPatch;
    }

    static string EditorAssetPackagePatch = string.Empty;
    public static string GetEditorAssetPackagePatchRoot()
    {
        if (EditorAssetPackagePatch.Equals(string.Empty))
        {
            EditorAssetPackagePatch = GetClient_GamePathRoot() + GameResPatch;
        }
        return EditorAssetPackagePatch;
    }

    static string RunTimeAssetPackagePatch = string.Empty;
    private static string GetRunTimeAssetPackagePatchRoot()
    {
        if(RunTimeAssetPackagePatch.Equals(string.Empty))
        {
            RunTimeAssetPackagePatch = GetRunTimeBinPatchRoot() + GameResPatch;
        }
        return RunTimeAssetPackagePatch;
    }

    static string Client_GamePath = string.Empty;
    /// <summary>
    /// 去的client/Game目录
    /// </summary>
    /// <returns></returns>
    private static string GetClient_GamePathRoot()
    {
        if(Client_GamePath.Equals(string.Empty))
        {
            Client_GamePath = Application.streamingAssetsPath;
            Client_GamePath = Client_GamePath.Replace("/Assets/StreamingAssets", string.Empty);
            Client_GamePath += "/";
        }
        return Client_GamePath;
    }

    static string RunTimeManagedScriptPath = string.Empty;
    /// <summary>
    /// 取得runtime的C#脚本目录
    /// </summary>
    /// <returns></returns>
    private static string GetRunTimeManagedScriptPathRoot()
    {
        if(RunTimeManagedScriptPath.Equals(string.Empty))
        {
            RunTimeManagedScriptPath = GetRunTimeBinPatchRoot() + GameDataName + "/Managed/";
        }
        return RunTimeManagedScriptPath;
    }

    static string DotRootPath = string.Empty;
    /// <summary>
    /// 取得混淆的父目录
    /// </summary>
    /// <returns></returns>
    private static string GetDotPathRoot()
    {
        if(DotRootPath.Equals(string.Empty))
        {
            DotRootPath = GetClient_GamePathRoot();
            DotRootPath += "ScriptDAssmbly/";
        }
        return DotRootPath;

    }


    static string RunTimeDotPath = string.Empty;
    private static string GetRunTimeDotPath()
    {
        if(RunTimeDotPath.Equals(string.Empty))
        {
            RunTimeDotPath = GetDotPathRoot() + "RunTimeSpeedConfigurations.postbuild";
        }
        return RunTimeDotPath;
    }

    static string EditorTimeDotPath = string.Empty;
    private static string GetEditorTimeDotPath()
    {
        if (EditorTimeDotPath.Equals(string.Empty))
        {
            EditorTimeDotPath = GetDotPathRoot() + "EditorSpeedConfigurations.postbuild";
        }
        return EditorTimeDotPath;
    }

    static string DotExePath = string.Empty;
    /// <summary>
    /// 取得混淆的EXE目录
    /// </summary>
    /// <returns></returns>
    private static string GetDotExePath()
    {
        if (DotExePath.Equals(string.Empty))
        {
            DotExePath = GetDotPathRoot();
            DotExePath += "XBuild.exe";
        }
        return DotExePath;

    }


    static string MPKPackerPath = string.Empty;
    /// <summary>
    /// 取得MPKPacker打包工具目录
    /// </summary>
    /// <returns></returns>
    private static string GetMPKPackerPath()
    {
        if (MPKPackerPath.Equals(string.Empty))
        {
            MPKPackerPath = GetClient_GamePathRoot();
            MPKPackerPath += "MPacker.exe";
        }

        return MPKPackerPath;

    }


    static string RunTimeScpPath = string.Empty;
    /// <summary>
    /// 取得需要打包的scp目录
    /// </summary>
    /// <returns></returns>
    private static string GetEdtiorScpPathRootNotLine()
    {
        if (RunTimeScpPath.Equals(string.Empty))
        {
             RunTimeScpPath = GetClient_GamePathRoot();
             RunTimeScpPath += "Data/Scp";
         }
        return RunTimeScpPath;

    }

    /// <summary>
    /// 取得Runtime下的Scp目录
    /// </summary>
    /// <returns></returns>
    private static string GetRunTimeScpPatchNotLine()
    {
        return GetRunTimeBinPatchRoot() + "Data/Scp";
    }

    static string RunScpMPKPath = string.Empty;
    /// <summary>
    /// 取得打包后SCP的MPK路径
    /// </summary>
    /// <returns></returns>
    private static string GetRunTimeScpMPKPath()
    {
        if (RunScpMPKPath.Equals(string.Empty))
        {
            RunScpMPKPath = GetRunTimeBinPatchRoot();
            RunScpMPKPath += "Data/Data.mpk";
        }
        return RunScpMPKPath;

    }


    private static string RunTimeAssetPackageListPathRoot = string.Empty;
    private static string GeEditorTimeAssetPackageListPathRoot()
    {
        if (RunTimeAssetPackageListPathRoot.Equals(string.Empty))
        {
            RunTimeAssetPackageListPathRoot = GetClient_GamePathRoot();
            RunTimeAssetPackageListPathRoot += AssetBundleManager.AssetBundlePathName + "/";
        }
        return RunTimeAssetPackageListPathRoot;
    }

    private static string GetMDBGenuratorPathRoot()
    {
        return GetClient_GamePathRoot() + "GenurateMDB/";
    }

    #region 发布流程
    private static string GetBuildFlowFloderPathRoot()
    {
        return GetClient_GamePathRoot() + "/BuildFlow/";
    }

    private static string GetBuildFlowPackageListPath()
    {
        return GetBuildFlowFloderPathRoot() + AssetBundleManager.AssetPackageListPatch;
    }

    private static string GetBuildFlowResDetalPath()
    {
        return GetBuildFlowFloderPathRoot() + ResDetailFileName;
    }

    /// <summary>
    /// 资源包构建失败
    /// </summary>
    /// <returns></returns>
    private static string GetBuildFlowPackingAssetPackageCarshFlagsPath()
    {
        return GetBuildFlowFloderPathRoot() + "Flow_PackingAssetPackageCarsh.txt";
    }

    /// <summary>
    /// 是否由外部调用
    /// </summary>
    /// <returns></returns>
    private static string GetRunFromOutSideProgramPath()
    {
        return GetBuildFlowFloderPathRoot() + "RunFromOutSideProgram.txt";
    }

    /// <summary>
    /// 资源包构建失败
    /// </summary>
    /// <returns></returns>
    private static string GetBuildFlowPackingAssetPackageFaildPath()
    {
        return GetBuildFlowFloderPathRoot() + "Flow_PackingAssetPackageFaild.txt";
    }

    /// <summary>
    /// 资源包构建成功
    /// </summary>
    /// <returns></returns>
    private static string GetBuildFlowPackingAssetPackageSuccessdPath()
    {
        return GetBuildFlowFloderPathRoot() + "Flow_PackingAssetPackageSuccess.txt";
    }

    /// <summary>
    /// 资源包构建失败
    /// </summary>
    /// <returns></returns>
    private static string GetBuildProjectCarshFlagsPath()
    {
        return GetBuildFlowFloderPathRoot() + "Flow_BuildProjectCarsh.txt";
    }

    /// <summary>
    /// BuildProject构建失败
    /// </summary>
    /// <returns></returns>
    private static string GetBuildFlowBuildProjectFaildPath()
    {
        return GetBuildFlowFloderPathRoot() + "Flow_BuildProjectFaild.txt";
    }


    /// <summary>
    /// 发出命令，构建资源包
    /// </summary>
    /// <returns></returns>
    private static string GetCommand_PackingAssetsPath()
    {
        return GetBuildFlowFloderPathRoot() + "Command_PackingAssets.txt";
    }

    /// <summary>
    /// 发出命令，构建64位资源包
    /// </summary>
    /// <returns></returns>
    private static string GetCommand_PackingAssetsPath64()
    {
        return GetBuildFlowFloderPathRoot() + "Command_PackingAssets64.txt";
    }

    /// <summary>
    /// 发出命令，是否需要重新打包
    /// </summary>
    /// <returns></returns>
    private static string GetCommand_NeedToRePackingAssets()
    {
        return GetBuildFlowFloderPathRoot() + "Command_NeedToRePackingAssets.txt";
    }

    /// <summary>
    /// 发出命令，构建调试exe
    /// </summary>
    /// <returns></returns>
    private static string GetCommand_BuildingDebugProjectPath()
    {
        return GetBuildFlowFloderPathRoot() + "Command_BuildDebugProject.txt";
    }

    /// <summary>
    /// 发出命令，构建调试64位exe
    /// </summary>
    /// <returns></returns>
    private static string GetCommand_BuildingDebugProjectPath64()
    {
        return GetBuildFlowFloderPathRoot() + "Command_BuildDebugProject64.txt";
    }

    /// <summary>
    /// 发出命令，构建发布版exe
    /// </summary>
    /// <returns></returns>
    private static string GetCommand_BuildingReleaseProjectPath()
    {
        return GetBuildFlowFloderPathRoot() + "Command_BuildReleaseProject.txt";
    }

    /// <summary>
    /// 发出命令，构建发布版64位exe
    /// </summary>
    /// <returns></returns>
    private static string GetCommand_BuildingReleaseProjectPath64()
    {
        return GetBuildFlowFloderPathRoot() + "Command_BuildReleaseProject64.txt";
    }


#endregion
}
