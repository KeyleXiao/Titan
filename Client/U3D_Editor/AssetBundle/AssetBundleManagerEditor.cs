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
    public static bool Is64bitEditor
    {
        get
        {
            return IntPtr.Size == 8;
        }
    }

    private static HashSet<string> currAssetBundleNames = new HashSet<string>();

    static string ReuireVersion = "5.4.5p3";
    static AssetBundleManagerEditor()
    {
        EditorApplication.update += EditorUpdate;
        AssetBundleManager.LoadAssetOnEditorInstance = null;
        AssetBundleManager.LoadSceneOnEditorInstance = null;
        AssetBundleManager.LoadAssetOnEditorFromAbInstance = null;
        AssetBundleManager.CheckAssetPatchOnEditorInstance = null;
        AssetBundleManager.LoadAllAssetOnEditorFromPathInstance = null;
        

        AssetBundleManager.LoadAssetOnEditorInstance += LoadAssetOnEditor;
        AssetBundleManager.LoadSceneOnEditorInstance += LoadSceneOnEditor;
        AssetBundleManager.LoadAssetOnEditorFromAbInstance += LoadAssetOnEditorFromAb;
        AssetBundleManager.CheckAssetPatchOnEditorInstance += CheckAssetPatchOnEditor;
        AssetBundleManager.LoadAllAssetOnEditorFromPathInstance += LoadAllAssetOnEditorFromPath;
    }

    static void EditorUpdate()
    {
        if (!Application.unityVersion.Equals(ReuireVersion))
        {
            EditorUtility.DisplayDialog("错误","Unity版本不一致，当前版本:" + Application.unityVersion + ",需要版本:" + ReuireVersion + ",请升级Unity版本","确定");
            EditorApplication.Exit(0);
        }
        EditorApplication.update -= EditorUpdate;

    }

    //~AssetBundleManagerEditor()
    //{
    //    AssetBundleManager.LoadAssetOnEditorInstance = null;
    //    AssetBundleManager.LoadSceneOnEditorInstance = null;
    //    AssetBundleManager.LoadAssetOnEditorFromAbInstance = null;
    //    AssetBundleManager.CheckAssetPatchOnEditorInstance = null;
    //}

    public static UnityEngine.Object[] LoadAssetOnEditorFromAb(string assetBundleName, string assetName, System.Type t)
    {
        string []patchs = AssetDatabase.GetAssetPathsFromAssetBundleAndAssetName(assetBundleName, assetName);

        if(patchs.Length <= 0)
        {
            return null;
        }
        return new UnityEngine.Object[] { AssetDatabase.LoadAssetAtPath(patchs[0],t) };
    }

    public static AsyncOperation LoadSceneOnEditor(string guid, bool bAsync,bool bAdditive)
    {
        string scenePath = AssetDatabase.GUIDToAssetPath(guid);
        if (bAsync)
        {
            if(bAdditive)
            {
                return EditorApplication.LoadLevelAdditiveAsyncInPlayMode(scenePath);
            }
            else
            {
                return EditorApplication.LoadLevelAsyncInPlayMode(scenePath);
            }
            
        }
        else
        {
            if(bAdditive)
            {
                EditorApplication.LoadLevelAdditiveInPlayMode(scenePath);
            }
            else
            {
                EditorApplication.LoadLevelInPlayMode(scenePath);
            }
            
            return null;
        }
    }

    public static UnityEngine.Object[] LoadAssetOnEditor(ResConfigData config, System.Type t)
    {
        string asspatch = AssetDatabase.GUIDToAssetPath(config.AssetGUID);
        if (asspatch.Equals(string.Empty))
        {
            Debug.LogWarning("资源数据库中没有这个GUID，请尝试重新导入:" + config.AssetName);
            return new UnityEngine.Object[0];
        }

        if (typeof(FBXAssets) == t)
        {
            List<UnityEngine.Object> objslst = new List<UnityEngine.Object>();
            //objslst.AddRange(AssetDatabase.LoadAllAssetsAtPath(asspatch));
            //for (int i = 0; i < objslst.Count; i++)
            //{
            //    if (objslst[i].GetType() != typeof(Mesh) &&
            //       objslst[i].GetType() != typeof(AnimationClip) &&
            //       objslst[i].GetType() != typeof(Avatar))
            //    {
            //        objslst.RemoveAt(i);
            //        i--;
            //        continue;
            //    }

            //    if (objslst[i].name.Contains("preview"))
            //    {
            //        Resources.UnloadAsset(objslst[i]);
            //        objslst.RemoveAt(i);
            //        i--;
            //        continue;
            //    }
            //}
            objslst.Add(AssetDatabase.LoadAssetAtPath<GameObject>(asspatch));
            return objslst.ToArray();
        }

        UnityEngine.Object[] assetsArray = new UnityEngine.Object[] { AssetDatabase.LoadAssetAtPath(asspatch, t) };
        return assetsArray;
    }

    public static UnityEngine.Object[] LoadAllAssetOnEditorFromPath(string path,Type t)
    {
        List<string> folders = new List<string>();
        folders.AddRange(AssetDatabase.FindAssets("t:" + t.Name, new string[] { path }));
        List<UnityEngine.Object> reslut = new List<UnityEngine.Object>();
        foreach(string str in folders)
        {
            string patch = AssetDatabase.GUIDToAssetPath(str);
            UnityEngine.Object asset = AssetDatabase.LoadAssetAtPath(patch, t);
            if(asset)
            {
                reslut.Add(asset);
            }
        }

        return reslut.ToArray();
    }
    public static bool CheckAssetPatchOnEditor(string assetBundleName, string assetName)
    {
        return AssetDatabase.GetAssetPathsFromAssetBundleAndAssetName(assetBundleName, assetName).Length > 0;
    }

    private static void GetAssetBundNameFromAssetFilePath(string path, out string assetbundleName)
    {
        assetbundleName = string.Empty;
        string pa = path.ToLower().Replace("\\", "/");
        string pat = AssetBundleManager.FullAssetPackagePathRoot.ToLower();
        pa = pa.Replace(pat, string.Empty);
        assetbundleName = pa;
    }


    public static AssetBundleEditorInfo GetAssetBundleEditorInfo(string path)
    {
        if (path.Equals(string.Empty))
        {
            return null;
        }

        
        string assetBundleName;
        string assetBundleVartian;
        string guid;
        AssetBundleManagerEditor.GetAssetBundleInfoFromMeta(path, out assetBundleName, out assetBundleVartian, out guid);
        if (assetBundleName.Equals(string.Empty) || assetBundleVartian.Equals(string.Empty) || guid.Equals(string.Empty))
        {
            return null;
        }

        string assetName = Path.GetFileNameWithoutExtension(path);

        if (assetName.Equals(string.Empty))
        {
            return null;
        }
        AssetBundleEditorInfo info = new AssetBundleEditorInfo();
        info.AssetBundleName = assetBundleName;
        info.AssetBundleVarian = assetBundleVartian;
        info.guid = guid;
        info.assetName = assetName;
        return info;
    }

    /// <summary>
    /// 取得资源的meta
    /// </summary>
    /// <param name="assetPath">patch为绝对路径，如:E/Speed/XXX</param>
    /// <returns></returns>
    public static string GetAssetGUIDFromMeta(string assetPath)
    {
        string ResultGuid = string.Empty;

        string metaPatch = AssetDatabase.GetTextMetaFilePathFromAssetPath(assetPath);
        if (string.IsNullOrEmpty(metaPatch))
        {
            Debug.LogError("找不到资源meta!,资源路径:" + assetPath);
            return ResultGuid;
        }

        System.IO.StreamReader metaReader = new System.IO.StreamReader(metaPatch, System.Text.UTF8Encoding.UTF8);
        string metaFileText = metaReader.ReadLine();
        while (!string.IsNullOrEmpty(metaFileText))
        {
            metaFileText = metaFileText.ToLower();
            //取出GUID
            if (metaFileText.Contains("guid"))
            {
                string[] metaArrays = metaFileText.Split(':');
                //第一个是GUID
                ResultGuid = metaArrays[1];

                //移除第0个，因为第0个是空格
                ResultGuid = ResultGuid.Trim();
                break;
            }
            metaFileText = metaReader.ReadLine();
        }
        metaReader.Close();
        metaReader = null;

        return ResultGuid;
    }

    /// <summary>
    /// ab信息从meta中获取
    /// </summary>
    /// <param name="assetImporter"></param>
    public static void GetAssetBundleInfoFromMeta(AssetImporter assetImporter, out string AssetBundleName, out string AssetBundleVariant, out string guid)
    {
        AssetBundleName = string.Empty;
        AssetBundleVariant = string.Empty;
        guid = string.Empty;
        if (!assetImporter)
        {
            return;
        }

        GetAssetBundleInfoFromMeta(assetImporter.assetPath, out AssetBundleName, out AssetBundleVariant, out guid);
    }

    /// <summary>
    /// ab信息从meta中获取
    /// </summary>
    /// <param name="assetImporter"></param>
    public static void GetAssetBundleInfoFromMeta(string assetPath, out string AssetBundleName, out string AssetBundleVariant,out string guid)
    {
        AssetBundleName = string.Empty;
        AssetBundleVariant = string.Empty;
        guid = string.Empty;
        if (assetPath.Equals(string.Empty))
        {
            return;
        }

        string filePatch = AssetBundleManagerEditor.ConverRelativePatchToObslotePatch(assetPath);
        filePatch += ".meta";

        if (!File.Exists(filePatch))
        {
            Debug.LogError("找不到文件，请确认文件路径:" + assetPath);
            return;
        }

        string text = System.IO.File.ReadAllText(filePatch);
        List<string> alltext = new List<string>();
        alltext.AddRange(text.Split('\n'));
        string strAbVar = "  assetBundleVariant: ";
        string StrAbNa = "  assetBundleName: ";
        string StrGuid = "guid: ";
        for (int i = 0; i < alltext.Count; i++)
        {
            string s = alltext[i];
            if (s.Contains(StrAbNa))
            {
                AssetBundleName = s.Replace(StrAbNa,string.Empty);
                AssetBundleName = AssetBundleName.Trim();
            }
            if (s.Contains(strAbVar))
            {
                AssetBundleVariant = s.Replace(strAbVar, string.Empty);
                AssetBundleVariant = AssetBundleVariant.Trim();
            }
            if (s.Contains(StrGuid))
            {
                guid = s.Replace(StrGuid, string.Empty);
                guid = guid.Trim();
            }
        }
    }

    /// <summary>
    /// 删除manifest文件
    /// </summary>
    static void DeleteMainfainstFile(string patch)
    {
        List<string> folders = new List<string>();

        if (!System.IO.Directory.Exists(patch))
        {
            return;
        }
        //取得包目录下的所有文件
        folders.AddRange(System.IO.Directory.GetFiles(patch, "*.*", SearchOption.AllDirectories));
        foreach (string s in folders)
        {
            if (s.Contains("manifest"))
            {
                DeleteFileHelper(s);
            }
        }
    }

    static void DeleteOtherFiles(string patch,bool isDebug)
    {
        if (!System.IO.Directory.Exists(patch))
        {
            return;
        }
        List<string> folders = new List<string>();
        //取得包目录下的所有文件
        folders.AddRange(System.IO.Directory.GetFiles(patch, "*.*", SearchOption.TopDirectoryOnly));
        foreach (string s in folders)
        {
            if(!isDebug)
            {
                //删除pdb
                if (s.EndsWith(".pdb"))
                {
                    System.IO.File.Delete(s);
                    continue;
                }
            }


            if(s.EndsWith(".lastcodeanalysissucceeded"))
            {
                System.IO.File.Delete(s);
                continue;
            }
            if (s.EndsWith(".metagen"))
            {
                System.IO.File.Delete(s);
                continue;
            }
            if (s.Contains("Mono"))
            {
                System.IO.File.Delete(s);
                continue;
            }
            if (s.Contains("GameAPI_Wrapper"))
            {
                System.IO.File.Delete(s);
                continue;
            }
            if (s.Contains("U3D_"))
            {
                System.IO.File.Delete(s);
                continue;
            }
            if (s.Contains("ICSharpCode"))
            {
                System.IO.File.Delete(s);
                continue;
            }
            if (s.Contains("Unity"))
            {
                System.IO.File.Delete(s);
                continue;
            }
            if(!isDebug)
            {
                if (s.Contains(".txt"))
                {
                    System.IO.File.Delete(s);
                    continue;
                }
            }

            if (s.Contains(".csv"))
            {
                System.IO.File.Delete(s);
                continue;
            }
            if (s.Contains(".xml"))
            {
                System.IO.File.Delete(s);
                continue;
            }
            if (s.Contains(".html"))
            {
                System.IO.File.Delete(s);
                continue;
            }
            if (s.Contains(".dmp"))
            {
                System.IO.File.Delete(s);
                continue;
            }
        }

        if (!System.IO.Directory.Exists(patch + "/" + GameResPatch))
        {
            return;
        }

        //删除prefab文件
        folders.Clear();
        folders.AddRange(System.IO.Directory.GetFiles(patch + "/" + GameResPatch, "*." + AssetBundleManager.PrefabVariant, SearchOption.AllDirectories));

        foreach (string s in folders)
        {
            System.IO.File.Delete(s);
            if (File.Exists(s + ".meta"))
            {
                System.IO.File.Delete(s + ".meta");
            }
        }

        if(!isDebug)
        {
            folders.Clear();
            folders.AddRange(System.IO.Directory.GetFiles(patch, "*.*", SearchOption.AllDirectories).Where(
                            str =>
                            {
                                if (str.Contains(".pdb") || str.Contains(".mdb"))
                                {
                                    return true;
                                }
                                return false;
                            })
                        );

            foreach (string s in folders)
            {
                System.IO.File.Delete(s);
            }
        }

    }


    /// <summary>
    /// 相对路径
    /// </summary>
    /// <param name="patch"></param>
    /// <returns></returns>
    public static ResConfigData GetResConfigDataFromPatch(string path)
    {
        ResConfigData conf = null;
        AssetBundleEditorInfo info = GetAssetBundleEditorInfo(path);
        if(null == info)
        {
            return conf;
        }
        conf = info.ConvertToResConfigData();
        return conf;
    }

    /// <summary>
    /// 相对路径
    /// </summary>
    /// <param name="patch"></param>
    /// <returns></returns>
    public static UnionResConfigData GetUnionResConfigDataFromPatch(string path)
    {
        UnionResConfigData conf = null;
        AssetBundleEditorInfo info = GetAssetBundleEditorInfo(path);
        if (null == info)
        {
            return conf;
        }
        conf = info.ConvertToUnionResConfigData();
        return conf;
    }

    public static void SaveAndRefershAssets()
    {
        AssetDatabase.Refresh();
        AssetDatabase.SaveAssets();
        AssetDatabase.Refresh();
    }

}
