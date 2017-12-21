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
public partial class AssetDebugEditor : EditorWindow
{

    [MenuItem("Tools/AssetDebugEditor")]
    public static void Init()
    {
        AssetDebugEditor uv = EditorWindow.GetWindow<AssetDebugEditor>(false, "资源调试器", true);
        uv.Show();
        uv.autoRepaintOnSceneChange = true;
    }


    public void OnGUI()
    {
        AssetBundleManager.DrawDebugInfo();
    }
}

