/*******************************************************************
** 文件名:	AssetBundleManager_UI.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	ab资源管理器
** 应  用:  	ab资源管理器，UI部分

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System;
using System.Runtime.InteropServices;

public sealed partial class AssetBundleManager : MonoBehaviour
{
    /*
     * 字体与通用资源一样，属于常驻内存的存在
     */


    /// <summary>
    /// 字体资源包
    /// </summary>
    static List<string> FontAssetBundleNames = new List<string>();

    const string FontAssetBundlePatch = "font/";

    /// <summary>
    /// 资源表，包名-[资源名,资源]
    /// </summary>
    static Dictionary<string, Dictionary<string, UnityEngine.Object>> m_FontAssetsTable = new Dictionary<string, Dictionary<string, UnityEngine.Object>>();

    static Dictionary<string, AssetBundle> m_FontAssetBundleTable = new Dictionary<string, AssetBundle>();

    /// <summary>
    /// 资源表，hashCode-资源结点
    /// </summary>
    static Dictionary<int, ResNode> m_FontResNodeTable = new Dictionary<int, ResNode>();

    static void LoadFontRes()
    {
        /*
         * 只有runtime才需要提前加载，主要是应对那些再预制体上已经引用了的字体
         * 编辑器下，已经存在引用了，unity会自动查找
         */
        if(isOnRunTimeMode)
        {
            LoadOnRunTime();
        }
    }

    static void LoadOnRunTime()
    {
        foreach(string s in FontAssetBundleNames)
        {
            if(!m_RespackageTable.ContainsKey(s))
            {
                continue;
            }
            AssetBundle pAssetBundle = LoadAssetBundle(AssetBundleManager.FullAssetPackagePathRoot + s, null);
            if(pAssetBundle)
            {
                UnityEngine.Object[] assets = pAssetBundle.LoadAllAssets<Font>();
                Dictionary<string, UnityEngine.Object> assetTable = new Dictionary<string, UnityEngine.Object>();
                m_FontAssetsTable.Add(s, assetTable);
                foreach(UnityEngine.Object asset in assets)
                {
                    assetTable.Add(asset.name, asset);
                }
                m_FontAssetBundleTable.Add(s, pAssetBundle);
            }
        }
    }

    static void ClearFontRes()
    {
        if(isOnRunTimeMode)
        {
            ClearOnRunTime();
        }
        else
        {
            ClearOnEditor();
        }
    }

    static void ClearOnRunTime()
    {
        foreach(AssetBundle p in m_FontAssetBundleTable.Values)
        {
            p.Unload(true);
        }
        FontAssetBundleNames.Clear();
        m_FontAssetsTable.Clear();
        m_FontAssetBundleTable.Clear();
        m_FontResNodeTable.Clear();
    }

    static void ClearOnEditor()
    {
        FontAssetBundleNames.Clear();
        m_FontAssetsTable.Clear();
        m_FontAssetBundleTable.Clear();
        m_FontResNodeTable.Clear();
    }
    /// <summary>
    /// 取得字体
    /// </summary>
    /// <param name="fontPatch">路径，相对于Fonts目录，如:UI/ArtistFonts/ArtistFontPage0</param>
    /// <returns></returns>
    private static ResNode GetFont(string fontPatch)
    {
        if (!isVaild)
        {
            return null;
        }
    
        string assetName = System.IO.Path.GetFileName(fontPatch);
        string assetBundleName = System.IO.Path.GetDirectoryName(fontPatch).ToLower();
        if (assetBundleName.Equals(string.Empty))
        {
            assetBundleName = "default";
        }
        assetBundleName = "font/" + assetBundleName + ".fnt";

        return GetFont(assetBundleName,assetName);
    }


    static ResNode GetFont(string assetBundleName,string assetName)
    {
        if(isOnRunTimeMode)
        {
            return GetFontOnRunTime(assetBundleName, assetName);
        }
        else
        {
            return GetFontEditor(assetBundleName, assetName);
        }
    }

    static ResNode GetFontOnRunTime(string assetBundleName,string assetName)
    {
        ResNode res = null;
        int hashCode = (assetBundleName + assetName).GetHashCode();
        if (m_FontResNodeTable.TryGetValue(hashCode, out res))
        {
            return res;
        }

        Dictionary<string, UnityEngine.Object> assetTable = null;
        if (m_FontAssetsTable.TryGetValue(assetBundleName, out assetTable))
        {
            UnityEngine.Object asset = null;
            if (assetTable.TryGetValue(assetName, out asset))
            {
                res = AllocResNode(hashCode.ToString());
                
                if (res.config == null)
                {
                    res.config = new ResConfigData();
                }
                res.config.AssetBundleName = assetBundleName;
                res.config.AssetName = assetName;
                res.isAsync = false;
                res.FillAssets(asset);
                res.status = ResNodeStatus.LoadFinish;
                m_FontResNodeTable.Add(hashCode, res);
            }
            else
            {
                AssetBudleLog("找不到字体:" + assetName);
            }
        }
        else
        {
            AssetBudleLog("找不到字体包:" + assetBundleName);
        }

        if (null != res)
        {
            res.assetType = AssetType.Asset_Font;
        }
        return res;
    }

    static ResNode GetFontEditor(string assetBundleName, string assetName)
    {
        ResNode res = null;
        int hashCode = (assetBundleName + assetName).GetHashCode();
        if (m_FontResNodeTable.TryGetValue(hashCode, out res))
        {
            return res;
        }
        UnityEngine.Object []asset = LoadAssetOnEditorFromAbInstance(assetBundleName, assetName,typeof(Font));
        if(asset != null && asset.Length > 0)
        {
            res = AllocResNode(hashCode.ToString());
           
            if(res.config == null)
            {
                res.config = new ResConfigData();
            }

            res.config.AssetName = assetName;
            res.config.AssetBundleName = assetBundleName;
            res.isAsync = false;
            res.FillAssets(asset);
            res.status = ResNodeStatus.LoadFinish;
            m_FontResNodeTable.Add(hashCode, res);

        }
        else
        {
            AssetBudleLog("找不到字体包:" + assetBundleName + ",字体名:" + assetName);
        }

        if (null != res)
        {
            res.assetType = AssetType.Asset_Font;
        }
        return res;
    }
}
