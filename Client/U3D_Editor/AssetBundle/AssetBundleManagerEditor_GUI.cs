/*******************************************************************
** 文件名:	AssetBundleManagerEditor_GUI.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/11/17
** 版  本:	1.0
** 描  述:	场景管理器
** 应  用:   场景管理编辑器脚本
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

using UnityEngine;
using UnityEditor;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System;
using LZ4ps;

/// <summary>
/// Ab管理器
/// </summary>
public partial class AssetBundleManagerEditor : EditorWindow
{

    [MenuItem("Tools/AssetBundleEditor")]
    public static void Init()
    {
        AssetBundleManagerEditor uv = EditorWindow.GetWindow<AssetBundleManagerEditor>(false, "AssetBundle管理理器", true);
        uv.Show();
        uv.autoRepaintOnSceneChange = true;
    }

    static string BuildPackageLogPath = string.Empty;
    static long currDelayToPackageTicks = 0;

    public void OnEnable()
    {
        if(Is64bitEditor)
        {
            Profiler.maxNumberOfSamplesPerFrame = 7 * 1024 * 1024;
        }
        if (!Application.isPlaying)
        {
            AssetBundleManager.InitAssetBundleThread();
        }
        currDelayToPackageTicks = DateTime.Now.Ticks;
    }

    public void OnDisable()
    {
        if(!Application.isPlaying)
        {
            AssetBundleManager.ClearAssetBundleThread();
        }
    }

    public void Update()
    {
        UpdateBuildFlow();
    }


    public void OnGUI()
    {
        EditorGUILayout.LabelField("所有功能都不能在playe模式下进行");

        if (GUILayout.Button("测试东西不要点-camera1"))
        {
            FindMissingScripts();
            //DotfuscatorScripts_Editor();
            //AssetImporter imp = AssetImporter.GetAtPath(AssetDatabase.GetAssetPath(Selection.activeObject));
            //if(imp)
            //{
            //    Debug.LogError(imp.assetBundleName);
            //}
        }

        if(!Is64bitEditor)
        {
            EditorGUILayout.LabelField("只能用于64bit编辑器");
            return;
        }

        if (Application.isPlaying)
        {
            return;
        }

        DrawButton();
        DrawOther();

        Repaint();
    }


    private static bool BuildAll = true;
    private static bool BuildPostPrefabConfig = true;
    private static bool BuildPacakage = true;
    private static bool BuildUIConfig = true;
    private static bool BuildSceneConfig = true;
    private static bool BuildScheme = true;
    private static bool BuildAssetsInfo = true;
    private static bool BuildPrePrefabConfig = true;
    public static bool isCleaningAssets = false;

    static void FindMissingScripts()
    {
        Transform []tr =  GameObject.FindObjectsOfType<Transform>();
        foreach (Transform gameObject in tr)
        {
            FindMissingScripts(gameObject.gameObject);
        }
    }

    static void FindMissingScripts(GameObject gameObject)
    {
        Component []components = gameObject.GetComponents<Component>();

        for (int j = 0; j < components.Length; j++)
        {
            if (components[j] == null)
            {
                Debug.Log("有脚本丢失:" + gameObject.name);
                break;
            }
        }
    }

    void DrawButton()
    {
        if(GUILayout.Button("黑科技（不要点）"))
        {
           // CleanupMissingScripts();
           
            //DotfuscatorScripts_RunTime();
            //SaveAndRefershAssets();
        }

        GUILayout.Label("客户端生成:");
        if (GUILayout.Button("生成客户端(Debug)"))
        {
            NeedToExitEditor = false;
            StartBuildFlow();
            BuildProject_Debug();
        }
        if(GUILayout.Button("生成客户端(Release)"))
        {
            NeedToExitEditor = false;
            StartBuildFlow();
            BuildProject_Release();
        }

        if (GUILayout.Button("生成客户端(Debug)_64"))
        {
            NeedToExitEditor = false;
            StartBuildFlow();
            BuildProject_Debug64();
        }
        if (GUILayout.Button("生成客户端(Release)_64"))
        {
            NeedToExitEditor = false;
            StartBuildFlow();
            BuildProject_Release64();
        }

        if(GUILayout.Button("导出文件(非资源包)到RunTime(Debug)"))
        {
            NeedToExitEditor = false;
            ExportOhterFilesToRunTime(true,false);
        }

        if (GUILayout.Button("导出文件(非资源包)到RunTime(Release)"))
        {
            NeedToExitEditor = false;
            ExportOhterFilesToRunTime(false,false);
        }

        if (GUILayout.Button("导出文件(非资源包)到RunTime64(Debug)"))
        {
            NeedToExitEditor = false;
            ExportOhterFilesToRunTime(true, true);
        }

        if (GUILayout.Button("导出文件(非资源包)到RunTime64(Release)"))
        {
            NeedToExitEditor = false;
            ExportOhterFilesToRunTime(false,true);
        }

        if (GUILayout.Button("混淆代码(Editor)"))
        {
            DotfuscatorScripts_Editor();
        }

        if (GUILayout.Button("生成资源包Key"))
        {
            GenerateAssetPackageKey();
        }

        GUILayout.Space(15.0f);
        GUILayout.Label("资源包生成:");
        
        BuildAll = GUILayout.Toggle(BuildAll,"构建完整包");
        if(!BuildAll)
        {
            GUILayout.BeginHorizontal("box");
            GUILayout.Label("预计算预制体配置:");
            BuildPrePrefabConfig = GUILayout.Toggle(BuildPrePrefabConfig, "");
            GUILayout.EndHorizontal();

            GUILayout.BeginHorizontal("box");
            GUILayout.Label("后计算预制体配置:");
            BuildPostPrefabConfig = GUILayout.Toggle(BuildPostPrefabConfig, "");
            GUILayout.EndHorizontal();


            GUILayout.BeginHorizontal("box");
            GUILayout.Label("资源包:");
            BuildPacakage = GUILayout.Toggle(BuildPacakage, "");
            GUILayout.EndHorizontal();

            GUILayout.BeginHorizontal("box");
            GUILayout.Label("UI配置:");
            BuildUIConfig = GUILayout.Toggle(BuildUIConfig, "");
            GUILayout.EndHorizontal();

            GUILayout.BeginHorizontal("box");
            GUILayout.Label("场景配置:");
            BuildSceneConfig = GUILayout.Toggle(BuildSceneConfig, "");
            GUILayout.EndHorizontal();

            GUILayout.BeginHorizontal("box");
            GUILayout.Label("资源信息:");
            BuildAssetsInfo = GUILayout.Toggle(BuildAssetsInfo, "");
            GUILayout.EndHorizontal();

            GUILayout.BeginHorizontal("box");
            GUILayout.Label("Scheme:");
            BuildScheme = GUILayout.Toggle(BuildScheme, "");
            GUILayout.EndHorizontal();
            
        }
        else
        {
            BuildPacakage = true;
            BuildPrePrefabConfig = true;
            BuildPostPrefabConfig = true;
            BuildUIConfig = true;
            BuildSceneConfig = true;
        }
        if (GUILayout.Button("生成"))
        {
            NeedToExitEditor = false;
            StartBuildFlow();
            BuildFullPackage();
        }

        if (GUILayout.Button("生成_64"))
        {
            NeedToExitEditor = false;
            StartBuildFlow();
            BuildFullPackage64();
        }
    }

    void DrawOther()
    {
        GUILayout.Space(15.0f);
        GUILayout.Label("其他:");

        if(GUILayout.Button("黑科技"))
        {
            //DeleteMainfainstFile(GetEditorAssetPackagePatchRoot());
        }
    }
}

