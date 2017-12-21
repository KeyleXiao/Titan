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
/// 配置的导入
/// </summary>
public partial class AssetBundleImporter : AssetPostprocessor
{
    private static bool isLightingEffectSchemeAssets(string s)
    {
        //不是在这个路径下的,不处理
        if (!s.Contains(SchemePatch))
        {
            return false;
        }

        if (!s.Contains(LinghtingEffectConfigPatch))
        {
            return false;
        }
        if (!s.Contains("."))
        {
            return false;
        }
        return true;
    }

    private static void OnImportLightingEffectSceheme(string path)
    {
         AssetBundleManagerEditor.AssetBundleEditorInfo da =AssetBundleManagerEditor.GetAssetBundleEditorInfo (path);
    }

}
