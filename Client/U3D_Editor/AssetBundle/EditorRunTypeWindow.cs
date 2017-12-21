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

public enum EditorRunType
{
    /// <summary>
    /// 发布机
    /// </summary>
    PackingMachine,

    /// <summary>
    /// 开发机
    /// </summary>
    DevelopMachine,

    /// <summary>
    /// 资源整理机
    /// </summary>
    ResourcesCleanUpMachine,
}

[InitializeOnLoad]
public partial class EditorRunTypeWindow 
{
    public static EditorRunType currentEditorRunType = EditorRunType.DevelopMachine;

    public static bool IsRunFromCommandLine
    {
        get
        {
            return m_IsRunFromCommandLine;
        }
    }
    private static bool m_IsRunFromCommandLine = false;

    public static string PackingMachineKey = "packingmachine.txt";
    public static string ResourcesCleanUpMachineKey = "resourcescleanupmachine.txt";

    static EditorRunTypeWindow()
    {
        UpdateRunType();
    }

    public static void UpdateRunType()
    {
        currentEditorRunType = EditorRunType.DevelopMachine;
        if (File.Exists(ResourcesCleanUpMachineKey))
        {
            currentEditorRunType = EditorRunType.ResourcesCleanUpMachine;
        }
        else if (File.Exists(PackingMachineKey))
        {
            currentEditorRunType = EditorRunType.PackingMachine;
        }
    }

    public static void RunFromCommandLine()
    {
        m_IsRunFromCommandLine = true;
    }
}

