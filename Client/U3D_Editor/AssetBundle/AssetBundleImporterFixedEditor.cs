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
using System.IO;
using System;

/// <summary>
/// 资源导入导入
/// </summary>

public partial class AssetBundleImporterFixedEditor : EditorWindow
{
    public class FixedPathNode
    {
        public string sourcePath = string.Empty;
        public string fixedType = string.Empty;
        public string FixedPath = string.Empty;

        public string getDesc()
        {
            return "\r\n" + 
                    "sourcePath:" + sourcePath + "\r\n" +
                    "FixedPath:" + FixedPath + "\r\n"+
                    "FixedType:" + fixedType;
        }
    }

    public static List<FixedPathNode> FixedFilePath
    {
        get
        {
            return needToFixedFilePath;
        }
    }

    static List<FixedPathNode> needToFixedFilePath = new List<FixedPathNode>();

    [MenuItem("Tools/AssetBundleImporterFixedEditor")]
    public static void OpenAssetBundleImporterFixedEditor()
    {
        AssetBundleImporterFixedEditor uv = EditorWindow.GetWindow<AssetBundleImporterFixedEditor>(false, "导入修复管理理器", true);
        uv.Show();
        uv.autoRepaintOnSceneChange = true;
    }

    public static void OpenAssetBundleImporterFixedEditorFromCommandLine()
    {
        EditorRunTypeWindow.RunFromCommandLine();

        AssetBundleManagerEditor uv1 = EditorWindow.GetWindow<AssetBundleManagerEditor>(false, "AssetBundle管理理器", false);
        uv1.Close();

        AssetBundleImporterFixedEditor uv = EditorWindow.GetWindow<AssetBundleImporterFixedEditor>(false, "导入修复管理理器", true);
        uv.Show();
        uv.autoRepaintOnSceneChange = true;
       
    }

    static double currTicks = 0.0f;
    static float delayTime = 1.5f;
    static bool needtoFixDll = false;
    static string CopyLogPath ="CopyEditorLog.log";
    public void OnEnable()
    {
        currTicks = DateTime.Now.Ticks;
    }

    public void OnGUI()
    {
        if(GUILayout.Button("修复"))
        {
            DoImporterFixed();
        }
    }

    private static void GenerateErrorFile(string overrideCopyEditorLogPath)
    {
        
        string LogPath = Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData) + "\\Unity\\Editor\\Editor.log";
        if (EditorRunTypeWindow.IsRunFromCommandLine)
        {
            string[] allCommandLine = Environment.GetCommandLineArgs();
            for(int i = 0;i < allCommandLine.Length;i++)
            {
                if (allCommandLine[i].Contains("-logFile"))
                {
                    LogPath = allCommandLine[i+1];
                    EditorLog.LogInfo("findCommandLineLogPath:" + LogPath, true);
                    break;
                }
            }
        }
        string reladyCopyPath = CopyLogPath;
        if (!string.IsNullOrEmpty(overrideCopyEditorLogPath))
        {
            reladyCopyPath = overrideCopyEditorLogPath;
        }
        File.Copy(LogPath, reladyCopyPath, true);

        string reader = File.ReadAllText(reladyCopyPath);
        reader = reader.Replace("\r", string.Empty);
        string[] lines = reader.Split('\n');
        foreach (string line in lines)
        {
            FixedPathNode reslut = GetNeedToFixedPath(line);
            if (null != reslut)
            {
                if (reslut.FixedPath.Contains(".dll"))
                {
                    needtoFixDll = true;
                }
                else
                {
                    needToFixedFilePath.Add(reslut);
                }
            }
        }
    }

    public static bool DoImporterFixed(string CopyeditorLogPath = null,string fixedLogPath = null)
    {
        needToFixedFilePath.Clear();
        GenerateErrorFile(CopyeditorLogPath);

        ApplyFixed(fixedLogPath);
        AssetBundleManagerEditor.SaveAndRefershAssets();

        return needToFixedFilePath.Count > 0;
        
    }

    public void Update()
    {
        if (EditorRunTypeWindow.IsRunFromCommandLine)
        {
            if (GameUtil.TicksToSecond(DateTime.Now.Ticks - currTicks) > delayTime)
            {
                DoImporterFixed();
                this.Close();
                EditorApplication.Exit(0);
            }
        }

    }

    private static void ApplyFixed(string overridefixedLogPath)
    {
        string fileText = string.Empty;
        string reladyPath = "FixedFillPathInfo.txt";
        if(!string.IsNullOrEmpty(overridefixedLogPath))
        {
            reladyPath = overridefixedLogPath;
        }
        if(needtoFixDll)
        {
            Debug.Log("正在修复DLL");
            ComponentUtily.ReimportUI();
            AssetBundleManagerEditor.SaveAndRefershAssets();
        }
        fileText = "needToFixDll:" + needtoFixDll +"\r\n";
        foreach (FixedPathNode str in needToFixedFilePath)
        {
            fileText += "FixedFileInfo:" + str.getDesc() + "\r\n";
            Debug.Log("资源导入时发生错误，正在重新导入:" + str.FixedPath);
            AssetDatabase.ImportAsset(str.FixedPath, ImportAssetOptions.ForceUpdate);
        }
        File.WriteAllText(reladyPath, fileText);
        if(needToFixedFilePath.Count > 0)
        {
            Debug.Log("资源重新导入完成..");
        }
        
        AssetBundleManagerEditor.SaveAndRefershAssets();

    }
    static string FixedKey_UnablePars = "Unable to parse file ";
    static string FixedKey_NotKnow_First =  "Asset '";
    static string FixedKey_NotKnow_Second = "not known";
    private static FixedPathNode GetNeedToFixedPath(string source)
    {
        FixedPathNode reslut = null;

        reslut = GetNeedToFixedPath_UnableParse(source);
        if(null == reslut)
        {
            reslut = GetNeedToFixedPath_UnableNotKnow(source);
        }
        return reslut;
    }

    static FixedPathNode GetNeedToFixedPath_UnableParse(string source)
    {
        FixedPathNode reslut = null;
        if (source.StartsWith(FixedKey_UnablePars))
        {
            reslut = new FixedPathNode();
            reslut.sourcePath = source;
            reslut.fixedType = "UnableParse";
            reslut.FixedPath = source.Replace(FixedKey_UnablePars, string.Empty);
            string[] temp = reslut.FixedPath.Split(':');
            if(temp.Length != 2)
            {
                Debug.Log("GetNeedToFixedPath_UnableParse 失败，temp.Length != 2 :" + source);
                reslut = null;
            }
            else
            {
                reslut.FixedPath = temp[0].Trim();
                if(!File.Exists(reslut.FixedPath))
                {
                    Debug.Log("GetNeedToFixedPath_UnableParse 失败，文件不存在:" + reslut);
                    reslut = null;
                }
            }
        }

        return reslut;
    }

    static FixedPathNode GetNeedToFixedPath_UnableNotKnow(string source)
    {
        FixedPathNode reslut = null;
        if (source.StartsWith(FixedKey_NotKnow_First) && source.Contains(FixedKey_NotKnow_Second))
        {
            reslut = new FixedPathNode();
            reslut.sourcePath = source;
            reslut.fixedType = "UnableNotKnown";
            reslut.FixedPath = source.Replace(FixedKey_NotKnow_First, string.Empty);
            string[] temp = reslut.FixedPath.Split('\'');
            if (temp.Length != 2)
            {
                Debug.Log("GetNeedToFixedPath_UnableNotKnow 失败，temp.Length != 2:" + source);
                reslut = null ;
            }
            else
            {
                reslut.FixedPath = temp[0].Trim();
                if (!File.Exists(reslut.FixedPath))
                {
                    Debug.Log("GetNeedToFixedPath_UnableNotKnow 失败，文件不存在:" + reslut);
                    reslut = null;
                }
            }
        }

        return reslut;
    }
}
