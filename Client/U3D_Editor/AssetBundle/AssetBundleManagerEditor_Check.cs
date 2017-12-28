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

/*
 * 1.资源整理的目的:
 * 1.删除不用的资源包
 * 2.统一meta
 */
public partial class AssetBundleManagerEditor : EditorWindow
{

    public static void CheckCompileError()
    {
        UnityEditor.SceneManagement.EditorSceneManager.NewScene(UnityEditor.SceneManagement.NewSceneSetup.EmptyScene);
        EditorApplication.isPlaying = true;

        EditorApplication.update += CheckCompileErrorUpdate;
    }

    private static void CheckCompileErrorUpdate()
    {
        EditorApplication.Exit(0);
    }


    public static void CheckAssetName(string path)
    {
        if(AssetBundleImporter.isInValidMaterial(path))
        {
            return;
        }
        string FileName = Path.GetFileNameWithoutExtension(path);

        if (!path.Contains("Scripts") && !path.Contains("Editor") && !path.Contains("UnityScene") && !path.Contains("Standard Assets") && !path.Contains("UniStorm") && !path.Contains("Plugins"))
        {
            if (FileName.Contains(".") || FileName.Contains(" "))
            {
                Debug.LogError("资源错误:资源路径不能有.（点）符号或者空格!:" + path);
            }
            else
            {
                if (path.Contains(" "))
                {
                    Debug.LogError("资源错误:资源路径不能有空格!:" + path);
                }
            }

        }
    }
}
