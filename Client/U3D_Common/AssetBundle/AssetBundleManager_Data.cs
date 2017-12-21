/*******************************************************************
** 文件名:	AssetBundleManager_Data.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2016/6/13
** 版  本:	1.0
** 描  述:	ab资源管理器
** 应  用:  	ab资源管理器，数据部分

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

public sealed partial class AssetBundleManager : MonoBehaviour
{
    public static string soundVariant = "soundpkg";
    public const string  UIVariant = "ui";
    public static string textureVariant = "texture";
    public static string sceneVariant = "scene";
    public static string PrefabVariant = "gameobject";

    /// <summary>
    /// 预制体合并文件
    /// </summary>
    public static string PrefabUnionVariant = "uniongo";
    public static string PrefabUnionResKey = "_un_rs";
    public static string FBXVariant = "model";
    public static string MatVariant = "modelmat";
    public static string AnimatorControllerVariant = "anicontroller";
    public static string AnimatorClipVariant = "clip";
    public static string ShaderAssetVariant = "sdr";
    public static string MeshAssetVariant = "msh";
    public static string FontAssetVariant = "fnt";
    public static string SchemeVariant = "scheme";
    public static string LowPolyMeshKey = "_LowerMesh";
    public static string MediumPolyMeshKey = "_MediumMesh";
    
    /// <summary>
    /// 分隔符，分割资源，代码，和配置用的
    /// </summary>
    public static string SplitCSVKey = "#-@&";

    /// <summary>
    /// 更新文件夹名，后面会加上版本号
    /// </summary>
    public static string UpdateInfo_UpdateFolderName = "Update_";

    public static string AssetPackageListPatch = "PackageList.pkl";
    public static string AssetBundlePathName = "GameData";

    public const string unityCSVExe = "uncsv";
    public const string unityBinExe = "unbin";

    public enum PackageNodeCol
    {
        COL_MD5 = 0,
        COL_AB_NAME,
        COL_FILESIZE,
    }

    public enum AssetConfigCol
    {
        COL_AssetBundleName = 0,
        COL_AssetName,
        COL_AssetGUID,
        COL_AssetSize_X,
        COL_AssetSize_Y,
    }

    /// <summary>
    /// 包结点
    /// </summary>
    public class PackageNode
    {
        public string md5;

        public string assetBundlePatch;

        /// <summary>
        /// 文件大小，MB
        /// </summary>
        public float fileSize;

    }



    /// <summary>
    /// 资源包名，资源包结点,Res
    /// </summary>
    private static Dictionary<string, PackageNode> m_RespackageTable = new Dictionary<string, PackageNode>();

    /// <summary>
    /// 资源包名，资源包结点,除资源包意外的其他文件
    /// </summary>
    private static Dictionary<string, PackageNode> m_OtherpackageTable = new Dictionary<string, PackageNode>();

    /// <summary>
    /// 合并AB包名
    /// </summary>
    private static HashSet<string> m_UnionDependenicesAssetBundleTable = new HashSet<string>();

    /// <summary>
    /// UI AB包名
    /// </summary>
    private static HashSet<string> m_UIAssetBundleTable = new HashSet<string>();

    public static string FullAssetPackagePathRoot
    {
        get
        {
            if (m_FullAssetPackagePathRoot.Equals(string.Empty))
            {
                if(Application.isEditor)
                {
                    m_FullAssetPackagePathRoot = Application.streamingAssetsPath;
                    m_FullAssetPackagePathRoot = m_FullAssetPackagePathRoot.Replace("Assets/StreamingAssets", string.Empty);
                    m_FullAssetPackagePathRoot += AssetBundleManager.AssetBundlePathName + "/";
                }
                else
                {
                    m_FullAssetPackagePathRoot = Application.streamingAssetsPath;
                    m_FullAssetPackagePathRoot = m_FullAssetPackagePathRoot.Replace("/StreamingAssets", string.Empty);
                    string[] temp = m_FullAssetPackagePathRoot.Split('/');
                    m_FullAssetPackagePathRoot = m_FullAssetPackagePathRoot.Replace(temp[temp.Length - 1], string.Empty);
                    m_FullAssetPackagePathRoot += AssetBundleManager.AssetBundlePathName + "/";
                }
            }
            return m_FullAssetPackagePathRoot;
        }
    }

    /// <summary>
    /// 资源包绝对路径文件夹
    /// </summary>
    private static string m_FullAssetPackagePathRoot = string.Empty;

    /// <summary>
    /// EXE绝对路径文件夹
    /// </summary>
    public static string FullEXEPathRoot
    {
        get
        {
            if (m_FullEXEPathRoot.Equals(string.Empty))
            {
                if (Application.isEditor)
                {
                    m_FullEXEPathRoot = Application.streamingAssetsPath;
                    m_FullEXEPathRoot = m_FullEXEPathRoot.Replace("Assets/StreamingAssets", string.Empty);
                }
                else
                {
                    m_FullEXEPathRoot = Application.streamingAssetsPath;
                    m_FullEXEPathRoot = m_FullEXEPathRoot.Replace("/StreamingAssets", string.Empty);
                    string[] temp = m_FullEXEPathRoot.Split('/');
                    m_FullEXEPathRoot = m_FullEXEPathRoot.Replace(temp[temp.Length - 1], string.Empty);
                }
                m_FullEXEPathRoot = m_FullEXEPathRoot.Remove(m_FullEXEPathRoot.Length - 1, 1);
            }
            return m_FullEXEPathRoot;
        }
    }

    private static string m_FullEXEPathRoot = string.Empty;

    /// <summary>
    /// 加载所有ab包配置数据
    /// </summary>
    private static void LoadAllAssetBundleConfigData()
    {
       
        ClearAllPackageConfig();
        LoadPackageList(GetPackageListPath());
    }


    private static void ClearAllPackageConfig()
    {
        ClearScene();
        ClearPackageList();
    }

    private static bool IsUnionDependenicesAssetBundle(string abName)
    {
        if (!enabledUnionDependenices)
        {
            return false;
        }
        if (!isOnRunTimeMode)
        {
            return false;
        }
        return m_UnionDependenicesAssetBundleTable.Contains(abName);
    }

    private static void InsertUnionDependenicesAssetBundle(string abName)
    {
        if (!enabledUnionDependenices)
        {
            return ;
        }
        if (!isOnRunTimeMode)
        {
            return;
        }
        string extension = System.IO.Path.GetExtension(abName);
        if (extension.Equals(string.Empty))
        {
            return;
        }
        if(abName.Contains(PrefabUnionResKey))
        {
            if (!m_UnionDependenicesAssetBundleTable.Contains(abName))
            {
                m_UnionDependenicesAssetBundleTable.Add(abName);
            }
        }
    }

    private static string GetPackageListPath()
    {
        return FullAssetPackagePathRoot + AssetBundleManager.AssetPackageListPatch;;
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="path"></param>
    /// <param name="ResResult">为空，则加载到自身的表里</param>
    /// <param name="OtherResult">为空，则加载到自身的表里</param>
    public static void LoadPackageList(string path, Dictionary<string, PackageNode> ResResult = null,
                                       Dictionary<string, PackageNode> OtherResult = null)
    {
        if (!File.Exists(path))
        {
            return;
        }
        string stSchemeAllText = DeCodeAssetFile(path);

        if (stSchemeAllText.Equals(string.Empty))
        {
            return;
        }
        if (null == ResResult)
        {
            ResResult = m_RespackageTable;
        }
        if (null == OtherResult)
        {
            OtherResult = m_OtherpackageTable;
        }

        //去掉\n,因为ScpReader以"\r"分割
        stSchemeAllText = stSchemeAllText.Replace("\n", "");
        //再干掉最后的"\r"
        stSchemeAllText = stSchemeAllText.Remove(stSchemeAllText.Length - 1, 0);
        AssetBundleConfigReader packageReader = new AssetBundleConfigReader(stSchemeAllText, "packageList", 0);

        int currentIndex = 0;

        string lowerArtistCommonPatch = ArtistCommonPatch.ToLower();

        #region Res
        //先读取资源的
        for (int i = currentIndex; i < packageReader.GetRecordCount(); ++i)
        {
            string SplitKey = packageReader.GetString(i, 0, string.Empty);
            if (SplitKey.Contains(SplitCSVKey))
            {
                currentIndex = i + 1;
                break;
            }
            PackageNode data = new PackageNode();
            data.md5 = packageReader.GetString(i, (int)PackageNodeCol.COL_MD5, string.Empty);
            data.assetBundlePatch = packageReader.GetString(i, (int)PackageNodeCol.COL_AB_NAME, string.Empty);
            data.fileSize = packageReader.GetFloat(i, (int)PackageNodeCol.COL_FILESIZE, 0);
            string fullpath = FullAssetPackagePathRoot + data.assetBundlePatch;
            if(!File.Exists(fullpath))
            {
                continue;
            }
            if (!ResResult.ContainsKey(data.assetBundlePatch))
            {
                ResResult.Add(data.assetBundlePatch, data);
                InsertUnionDependenicesAssetBundle(data.assetBundlePatch);
                if (data.assetBundlePatch.Contains(lowerArtistCommonPatch))
                {
                    ArtistCommonAssetBundleNames.Add(data.assetBundlePatch);
                }
                else if (data.assetBundlePatch.Contains(FontAssetBundlePatch))
                {
                    FontAssetBundleNames.Add(data.assetBundlePatch);
                }

                if(data.assetBundlePatch.EndsWith(UIVariant))
                {
                    m_UIAssetBundleTable.Add(data.assetBundlePatch);
                }

                if (data.assetBundlePatch.EndsWith(ShaderAssetVariant) )
                {
                    
                    if(data.assetBundlePatch.Contains(FirstLoadPatchKey))
                    {
                        m_FirstLoadAssetBundlePath.Add(data.assetBundlePatch);
                    }
                    else
                    {
                        m_NormalShaderAssetBundlePath.Add(data.assetBundlePatch);
                    }
                }
            }
            else
            {
                Debug.LogWarning("资源列表里有重复的文件:" + data.assetBundlePatch);
            }
        }
        #endregion

        #region Other

        //先读取资源的
        for (int i = currentIndex; i < packageReader.GetRecordCount(); ++i)
        {
            string SplitKey = packageReader.GetString(i, 0, string.Empty);
            if (SplitKey.Contains(SplitCSVKey))
            {
                currentIndex = i + 1;
                break;
            }
            PackageNode data = new PackageNode();
            data.md5 = packageReader.GetString(i, (int)PackageNodeCol.COL_MD5, string.Empty);
            data.assetBundlePatch = packageReader.GetString(i, (int)PackageNodeCol.COL_AB_NAME, string.Empty);
            data.fileSize = packageReader.GetFloat(i, (int)PackageNodeCol.COL_FILESIZE, 0);
            if (!OtherResult.ContainsKey(data.assetBundlePatch))
            {
                OtherResult.Add(data.assetBundlePatch, data);
            }
            else
            {
                Debug.LogWarning("资源列表里有重复的文件:" + data.assetBundlePatch);
            }
        }

        #endregion
    }

    /// <summary>
    /// 卸载资源列表
    /// </summary>
    public static void ClearPackageList()
    {
        m_RespackageTable.Clear();
        m_OtherpackageTable.Clear();
        m_UnionDependenicesAssetBundleTable.Clear();
    }
}
