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
public partial class AssetBundleManagerEditor : EditorWindow
{
    enum BuildFlow
    {

        /// <summary>
        /// 没有任何动作
        /// </summary>
        BuilFlow_None,

        /// <summary>
        /// 发布开始
        /// </summary>
        BuilFlow_Begin,

        /// <summary>
        /// 正在处于资源打包
        /// </summary>
        BuildFlow_PackingAssetPackage,

        /// <summary>
        /// 正在发布exe
        /// </summary>
        BuildFlow_BuildingEXE,
    }

    static BuildFlow currBuildFlow = BuildFlow.BuilFlow_Begin;
    static BuildFlow LastBuildFlow = BuildFlow.BuilFlow_None;
    public static bool NeedToExitEditor = true;
    
    void StartBuildFlow()
    {
        currBuildFlow = BuildFlow.BuilFlow_Begin;
        LastBuildFlow = BuildFlow.BuilFlow_None;
    }

    bool IsCommandOnPackingAssetPackage()
    {
        return File.Exists(GetCommand_PackingAssetsPath()) || File.Exists(GetCommand_PackingAssetsPath64());
    }

    bool IsCommandOnBuildEXE()
    {
        return File.Exists(GetCommand_BuildingDebugProjectPath()) || File.Exists(GetCommand_BuildingDebugProjectPath64()) ||
               File.Exists(GetCommand_BuildingReleaseProjectPath()) || File.Exists(GetCommand_BuildingReleaseProjectPath64());
    }


    bool CheckPackingAssetPackignFlow()
    {
         /* 1.首先检查是否处于打包流程
         * 如果处于打包流程，看看是否打包失败,如果打包失败的，要还原对应的文件
         * 然后重新开始打包
         */
        if (IsCommandOnPackingAssetPackage())
        {
            if (File.Exists(GetBuildFlowPackingAssetPackageFaildPath()))
            {
                CopyFileHelper(GetBuildFlowPackageListPath(), GetEditorAssetPackagePatchRoot() + AssetBundleManager.AssetPackageListPatch);
                currBuildFlow = BuildFlow.BuildFlow_PackingAssetPackage;
                DeleteFileHelper(GetBuildFlowPackingAssetPackageFaildPath());
                DeleteFileHelper(GetBuildFlowPackageListPath());
                return true;
            }
        }

        /**
         * 2.检查是否完成了打包，但是没有进行buildexe，就又来打包一次
         * 这个时候就要还原文件
         * 然后重新开始打包
         */
        if (IsCommandOnPackingAssetPackage())
        {
            if (File.Exists(GetBuildFlowPackingAssetPackageSuccessdPath()))
            {
                CopyFileHelper(GetBuildFlowPackageListPath(), GetEditorAssetPackagePatchRoot() + AssetBundleManager.AssetPackageListPatch);
                currBuildFlow = BuildFlow.BuildFlow_PackingAssetPackage;
                DeleteFileHelper(GetBuildFlowPackingAssetPackageSuccessdPath());
                DeleteFileHelper(GetBuildFlowPackageListPath());
                return true;
            }
        }

        /**
         * 3.检查打包的时候是否carsh了
         * 这个时候就要还原文件
        * 然后重新开始打包
        */
        if (IsCommandOnPackingAssetPackage())
        {
            if (File.Exists(GetBuildFlowPackingAssetPackageCarshFlagsPath()))
            {
                CopyFileHelper(GetBuildFlowPackageListPath(), GetEditorAssetPackagePatchRoot() + AssetBundleManager.AssetPackageListPatch);
                currBuildFlow = BuildFlow.BuildFlow_PackingAssetPackage;
                DeleteFileHelper(GetBuildFlowPackingAssetPackageCarshFlagsPath());
                DeleteFileHelper(GetBuildFlowPackageListPath());
                return true;
            }
        }

        /**
         * 4.需要打包，但是没有成功，没有失败，那就是一个全新的。
         * 直接开始打包
         */
        if (IsCommandOnPackingAssetPackage())
        {
            currBuildFlow = BuildFlow.BuildFlow_PackingAssetPackage;
            return true;
        }

        return false;
    }


    bool CheckBuildProjectFlow()
    {
        /* 1.首先检查是否打资源包的时候是否carsh了，如果carsh了，弹框提示重新打包资源，退出
        */
        if (IsCommandOnBuildEXE())
        {
            if (File.Exists(GetBuildFlowPackingAssetPackageCarshFlagsPath()))
            {
                ReleaseBuildingEXECommand();
                EditorUtility.DisplayDialog("发布错误", "资源打包时Unity崩溃，请重新进行资源打包后再发布,如果想强制发布请删除文件夹:" + GetBuildFlowFloderPathRoot() + ",但会出现很多问题", "确定");
                return false;
            }
        }

        /* 资源打包失败，先询问一下
        */
        if (IsCommandOnBuildEXE())
        {
            if (File.Exists(GetBuildFlowPackingAssetPackageFaildPath()))
            {
                //外部层序的话，由外部判断
                if(File.Exists( GetRunFromOutSideProgramPath()))
                {
                    CopyFileHelper(GetBuildFlowFloderPathRoot() + ResDetailFileName, GetClient_GamePathRoot() + ResDetailFileName);
                    currBuildFlow = BuildFlow.BuildFlow_BuildingEXE;
                    return true;
                }

                if(EditorUtility.DisplayDialog("发布警告", "资源发布的时候发生了一些错误，如果要强制发布项目的话，可能会造成一些资源丢失或者崩溃,是否需要发布?", "是","否"))
                {
                    CopyFileHelper(GetBuildFlowFloderPathRoot() + ResDetailFileName, GetClient_GamePathRoot() + ResDetailFileName);
                    currBuildFlow = BuildFlow.BuildFlow_BuildingEXE;
                    return true;
                }
                return false;
            }
        }


         /* 2.首先检查是否处于发布流程
         * 如果处于发布流程，看看是否发布失败,如果发布失败的，要还原对应的文件
         * 然后重新开始发布
         */
        if (IsCommandOnBuildEXE())
        {
            if (File.Exists(GetBuildFlowBuildProjectFaildPath()))
            {
                CopyFileHelper(GetBuildFlowFloderPathRoot() + ResDetailFileName, GetClient_GamePathRoot() + ResDetailFileName);
                currBuildFlow = BuildFlow.BuildFlow_BuildingEXE;
                DeleteFileHelper(GetBuildFlowBuildProjectFaildPath());
                DeleteFileHelper(GetBuildFlowFloderPathRoot() + ResDetailFileName);
                return true;
            }
        }

        /* 2.再检查发布的时候是不是崩溃了要还原对应的文件
            * 然后重新开始发布
        */
        if (IsCommandOnBuildEXE())
        {
            if (File.Exists(GetBuildProjectCarshFlagsPath()))
            {
                CopyFileHelper(GetBuildFlowFloderPathRoot() + ResDetailFileName, GetClient_GamePathRoot() + ResDetailFileName);
                currBuildFlow = BuildFlow.BuildFlow_BuildingEXE;
                DeleteFileHelper(GetBuildFlowBuildProjectFaildPath());
                DeleteFileHelper(GetBuildFlowFloderPathRoot() + ResDetailFileName);
                return true;
            }
        }

        /**
         * 4.需要发布，但是没有失败(对发布来说，发布成功就结束了整个流程了)，那就是一个全新的。
         * 直接开始打包
         */
        if (IsCommandOnBuildEXE())
        {
            currBuildFlow = BuildFlow.BuildFlow_BuildingEXE;
            return true;
        }

        return false;
    }


    /// <summary>
    /// 开始构建流
    /// </summary>
    void OnFlow_Begin()
    {
        //开始的时候，检查状态

        if(CheckPackingAssetPackignFlow())
        {
            return;
        }
        else if(!CheckBuildProjectFlow())
        {
            currBuildFlow = BuildFlow.BuilFlow_None;
            LastBuildFlow = BuildFlow.BuilFlow_None;
            if (NeedToExitEditor)
            {
                EditorApplication.Exit(0);
            }
        }
    }

    void ReleasePackignAssetPackageCommand()
    {
        DeleteFileHelper(GetCommand_PackingAssetsPath());
        DeleteFileHelper(GetCommand_PackingAssetsPath64());
    }

    /// <summary>
    /// 打包流程
    /// </summary>
    void OnFlow_PackingAssetPackage(bool is64bit)
    {
        /*
         * 1.开始流程要创建文件，需要备份资源文件列表
         */

        ReleasePackignAssetPackageCommand();

        //备份资源列表文件
        CopyFileHelper(GetEditorAssetPackagePatchRoot() + AssetBundleManager.AssetPackageListPatch, GetBuildFlowPackageListPath());

        //告知流程--告知崩溃，如果没有崩溃，后面会删除
        CreateFileHelper(GetBuildFlowPackingAssetPackageCarshFlagsPath());

        EditorLog.StartLog("BuildPacakgeLog",EditorLog.LogLevel.ERROR);
        OnBuildFullPackageStart();


        BuildFullPackage_Impl(is64bit);

        if(EditorLog.isHaveFatleErrorLog)
        {
            //打包失败
            OnBuildFullPackageFaild();

            //告知流程--资源发布失败
            CreateFileHelper(GetBuildFlowPackingAssetPackageFaildPath());
            //告知流程--资源发布没有崩溃
            DeleteFileHelper(GetBuildFlowPackingAssetPackageCarshFlagsPath());

            EditorLog.EndLog();
            if (!File.Exists(GetRunFromOutSideProgramPath()))
            {
                EditorUtility.DisplayDialog("致命错误", "!!!!资源有致命错误，不可进行后续从左!!!!稍后会打开对应的日志路径，请将:" + EditorLog.LogPath + "发给程序.", "确定");
                System.Diagnostics.Process.Start(Path.GetDirectoryName(EditorLog.LogPath));
            }
        }
        else if (EditorLog.isHaveErroLog)
        {
            //打包失败
            OnBuildFullPackageFaild();
            
            //告知流程--资源发布失败
            CreateFileHelper(GetBuildFlowPackingAssetPackageFaildPath());
            //告知流程--资源发布没有崩溃
            DeleteFileHelper(GetBuildFlowPackingAssetPackageCarshFlagsPath());

            EditorLog.EndLog();
            if (!File.Exists(GetRunFromOutSideProgramPath()))
            {
                EditorUtility.DisplayDialog("资源错误", "资源打包错误，稍后会打开对应的日志路径，请将:" + EditorLog.LogPath + "发给程序.", "确定");
                System.Diagnostics.Process.Start(Path.GetDirectoryName(EditorLog.LogPath));
            }

        }
        else
        {
            //打包成功
            OnBuildFullPackageSuccess();
            //告知流程--资源发布成功
            CreateFileHelper(GetBuildFlowPackingAssetPackageSuccessdPath());
            //告知流程--资源发布没有崩溃
            DeleteFileHelper(GetBuildFlowPackingAssetPackageCarshFlagsPath());
        }

        if(NeedToExitEditor)
        {
            EditorApplication.Exit(0);
        }
    }

    void ReleaseBuildingEXECommand()
    {
        DeleteFileHelper(GetCommand_BuildingDebugProjectPath());
        DeleteFileHelper(GetCommand_BuildingDebugProjectPath64());
        DeleteFileHelper(GetCommand_BuildingReleaseProjectPath());
        DeleteFileHelper(GetCommand_BuildingReleaseProjectPath64());
    }

    void OnFlow_OnBuildingEXE(bool isDebug,bool is64bit)
    {

        ReleaseBuildingEXECommand();

        if(isDebug)
        {
            EditorLog.StartLog("BuildProjectLog", EditorLog.LogLevel.ERROR);
            //备份文件
            CopyFileHelper(GetClient_GamePathRoot() + ResDetailFileName, GetBuildFlowFloderPathRoot() + ResDetailFileName);
           
            //告知流程--告知崩溃，如果没有崩溃，后面会删除
            CreateFileHelper(GetBuildProjectCarshFlagsPath());

            OnBuildProjectStart();

            BuildProject_Impl(isDebug,is64bit);

            if(EditorLog.isHaveFatleErrorLog)
            {
                OnBuildProjectFaild();
                EditorLog.EndLog();
                if (!File.Exists(GetRunFromOutSideProgramPath()))
                {
                    EditorUtility.DisplayDialog("致命错误", "!!!!!!Debug版本有致命错误，不能进行后续操作!!!!!!稍后会打开对应的日志路径，请将:" + EditorLog.LogPath + "发给程序.", "确定");
                    System.Diagnostics.Process.Start(Path.GetDirectoryName(EditorLog.LogPath));
                }
                OnFlow_BuildingExeFailed(isDebug, is64bit);

            }
            else if (EditorLog.isHaveErroLog)
            {
                OnBuildProjectFaild();
                EditorLog.EndLog();
                if (!File.Exists(GetRunFromOutSideProgramPath()))
                {
                    EditorUtility.DisplayDialog("发布错误", "Debug版本发布错误，稍后会打开对应的日志路径，请将:" + EditorLog.LogPath + "发给程序.", "确定");
                    System.Diagnostics.Process.Start(Path.GetDirectoryName(EditorLog.LogPath));
                }
                OnFlow_BuildingExeFailed(isDebug, is64bit);
            }
            else
            {
                OnBuildProjectSuccess();
                OnFlow_BuildingExeSuccess(isDebug, is64bit);
                EditorLog.LogInfo("工程发布成功!!!!!!!!!!!!!!!!", true);
                
            }
            EditorLog.EndLog();
        }
        else
        {
            EditorLog.StartLog("BuildProjectLog",EditorLog.LogLevel.ERROR);
            OnBuildProjectStart();
            CopyFileHelper(GetClient_GamePathRoot() + ResDetailFileName, GetBuildFlowFloderPathRoot() + ResDetailFileName);

            BuildProject_Impl(isDebug,is64bit);

            if(EditorLog.isHaveFatleErrorLog)
            {
                OnBuildProjectFaild();

                EditorLog.EndLog();
                if (!File.Exists(GetRunFromOutSideProgramPath()))
                {
                    EditorUtility.DisplayDialog("致命错误", "!!!Release版本有致命错误，不能进行后续操作!!!稍后会打开对应的日志路径，请将:" + EditorLog.LogPath + "发给程序.", "确定");
                    System.Diagnostics.Process.Start(Path.GetDirectoryName(EditorLog.LogPath));
                }
                OnFlow_BuildingExeFailed(isDebug, is64bit);

            }
            else if (EditorLog.isHaveErroLog)
            {
                OnBuildProjectFaild();

                EditorLog.EndLog();
                if (!File.Exists(GetRunFromOutSideProgramPath()))
                {
                    EditorUtility.DisplayDialog("发布错误", "Release版本发布错误，稍后会打开对应的日志路径，请将:" + EditorLog.LogPath + "发给程序.", "确定");
                    System.Diagnostics.Process.Start(Path.GetDirectoryName(EditorLog.LogPath));
                }              
                OnFlow_BuildingExeFailed(isDebug, is64bit);
            }
            else
            {
                OnBuildProjectSuccess();
                
                OnFlow_BuildingExeSuccess(isDebug,is64bit);
                EditorLog.LogInfo("工程发布成功!!!!!!!!!!!!!!!!", true);
            }
            EditorLog.EndLog();
        }
    }

    void OnFlow_BuildingExeSuccess(bool isDebug,bool is64bit)
    {
        /*
         * 当EXE完成的时候，整个发布流程已经完成，这个时候删除所有的发布文件就可以了。
         */

        if(System.IO.Directory.Exists(GetBuildFlowFloderPathRoot()))
        {
            System.IO.Directory.Delete(GetBuildFlowFloderPathRoot(), true);
        }

        System.Diagnostics.Process.Start(GetRunTimeBinPatchRoot());
        if (NeedToExitEditor)
        {
            EditorApplication.Exit(0);
        }
    }


    void OnFlow_BuildingExeFailed(bool isDebug,bool is64bit)
    {
        /*
         * 当EXE发布失败，需要还原对应的文件
        */
        DeleteFileHelper(GetBuildProjectCarshFlagsPath());

        CopyFileHelper(GetBuildFlowFloderPathRoot() + ResDetailFileName, GetClient_GamePathRoot() + ResDetailFileName);
        CreateFileHelper(GetBuildFlowBuildProjectFaildPath());

        if (NeedToExitEditor)
        {
            EditorApplication.Exit(0);
        }
    }

    void UpdateBuildFlow()
    {
        TimeSpan s = new TimeSpan(DateTime.Now.Ticks - currDelayToPackageTicks);
        if (s.TotalSeconds < 2.0f)
        {
            return;
        }

        //没有任何东西，不需要打包
        if (!File.Exists(GetCommand_BuildingReleaseProjectPath64()) && !File.Exists(GetCommand_BuildingReleaseProjectPath())
            && !File.Exists(GetCommand_BuildingDebugProjectPath64()) && !File.Exists(GetCommand_BuildingDebugProjectPath())
            && !File.Exists(GetCommand_PackingAssetsPath64()) && !File.Exists(GetCommand_PackingAssetsPath())
            )
        {
            return;
        }

        if(currBuildFlow == LastBuildFlow)
        {
            return;
        }
        LastBuildFlow = currBuildFlow;

        

        bool is64Bit = File.Exists(GetCommand_BuildingDebugProjectPath64()) || File.Exists(GetCommand_BuildingReleaseProjectPath64()) || File.Exists(GetCommand_PackingAssetsPath64());
        bool isDebug = File.Exists(GetCommand_BuildingDebugProjectPath()) || File.Exists(GetCommand_BuildingDebugProjectPath64());

        switch(currBuildFlow)
        {
            case BuildFlow.BuilFlow_Begin:
                OnFlow_Begin();
                break;
            case BuildFlow.BuildFlow_BuildingEXE:
                OnFlow_OnBuildingEXE(isDebug, is64Bit);
                break;
            case BuildFlow.BuildFlow_PackingAssetPackage:
                OnFlow_PackingAssetPackage(is64Bit);
                break;
        }

    }
}
