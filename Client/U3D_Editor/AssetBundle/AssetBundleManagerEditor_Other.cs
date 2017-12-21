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
using System.Text;
using System.Linq;

public partial class AssetBundleManagerEditor : EditorWindow
{
    enum PackageNodeType
    {
        /// <summary>
        /// 值unity的打包文件
        /// </summary>
        AssetsBundleFile,

        /// <summary>
        /// 除了上面的其他文件
        /// </summary>
        OtherFile,
    }

    public class AssetBundleEditorInfo
    {
        public string assetName = string.Empty;
        public string guid = string.Empty;
        public float Asset_X = 0.0f;
        public float Asset_Y = 0.0f;
        /// <summary>
        /// 完整的ab名，ab+variant
        /// </summary>
        public string FullAssetBundleName
        {
            get
            {
                return AssetBundleName + "." + AssetBundleVarian;
            }
        }

        /// <summary>
        /// AB名字，小写
        /// </summary>
        public string AssetBundleName
        {
            get
            {
                return _assetBundleName;
            }

            set
            {
                _assetBundleName = value.ToLower();
            }
        }
        private string _assetBundleName = string.Empty;

        /// <summary>
        /// variant 小写
        /// </summary>
        public string AssetBundleVarian
        {
            get
            {
                return _assetBundleVarian;
            }

            set
            {
                _assetBundleVarian = value.ToLower();
            }
        }

        private string _assetBundleVarian = string.Empty;
        public static AssetBundleEditorInfo Empty
        {
            get
            {
                if(_empty == null)
                {
                    _empty = new AssetBundleEditorInfo();
                }
                return _empty;
            }
        }
        private static AssetBundleEditorInfo _empty = null;

        public ResConfigData ConvertToResConfigData()
        {
            ResConfigData instance = new ResConfigData();
            instance.AssetBundleName = FullAssetBundleName;
            instance.AssetName = assetName;
            instance.AssetGUID = guid;
            instance.AssetSize_X = Asset_X;
            instance.AssetSize_Y = Asset_Y;
            return instance;
        }

        public UnionResConfigData ConvertToUnionResConfigData()
        {
            UnionResConfigData instance = new UnionResConfigData();
            instance.AssetBundleName = FullAssetBundleName;
            instance.AssetName = assetName;
            instance.AssetGUID = guid;
            instance.unionIndx = -1;
            instance.dataLength = -1;
            instance.startIndex = -1;
            return instance;
        }
    }

    class AssetPackageListNode : AssetBundleManager.BaseMultThreadWorkData
    {
        public PackageNodeType nodeType = PackageNodeType.AssetsBundleFile;
        public string ignorePackageNameStr = string.Empty;
        public List<string> paths = new List<string>();
        public bool isDone = false;
        public Dictionary<string, AssetBundleManager.PackageNode> resultTable = new Dictionary<string, AssetBundleManager.PackageNode>();
    }

    class EncodeFileNode : AssetBundleManager.BaseMultThreadWorkData
    {
        public PackageNodeType nodeType = PackageNodeType.AssetsBundleFile;
        public List<string> ModifPath = new List<string>();
        public bool isDone = false;
        public Dictionary<string, AssetBundleManager.PackageNode> resultTable = new Dictionary<string, AssetBundleManager.PackageNode>();
    }

    static string []DefaultResourcesGUID = new string[]{"0000000000000000e000000000000000"/*unity default resources*/,"0000000000000000f000000000000000"/*unity_builtin_extra*/};
    private static void DestroyAllBuiltInMaterial()
    {
        foreach(string guid in DefaultResourcesGUID)
        {
            UnityEngine.Object[] mat = AssetDatabase.LoadAllAssetsAtPath(AssetDatabase.GUIDToAssetPath(guid));
            foreach(UnityEngine.Object ob in mat)
            {
                if(ob as Material)
                {
                    GameObject.DestroyImmediate(ob, true);
                }

            }
        }
    }

    private static void DestroyAllBuiltInShader()
    {
        //foreach (string guid in DefaultResourcesGUID)
        //{
        //    UnityEngine.Object[] mat = AssetDatabase.LoadAllAssetsAtPath(AssetDatabase.GUIDToAssetPath(guid));
        //    foreach (UnityEngine.Object ob in mat)
        //    {
        //        Shader sh = ob as Shader;
        //        if (sh)
        //        {
        //            if (sh.name.Contains("Particles/") || sh.name.Contains("Legacy Shaders/") || sh.name.Equals("Standard") || sh.name.Equals("Standard (Specular setup)"))
        //            {
        //                  GameObject.DestroyImmediate(ob, true);
        //            }
                  
        //        }

        //    }
        //}
    }

    private static bool IsConfigFile(string fileName)
    {
        return fileName.EndsWith(".manifest") || fileName.EndsWith(AssetBundleManager.unityCSVExe) ||
                             fileName.EndsWith(AssetBundleManager.unityBinExe) || !fileName.Contains('.') ||
                             fileName.EndsWith(".gocof");
    }



    private static bool IsManagedScript(string s)
    {
        return (s.Contains("U3D_") || s.Contains("GameAPI_")) && s.EndsWith(".dll");
    }

    private static bool IsUnionGo(string s)
    {
        return s.EndsWith(AssetBundleManager.PrefabUnionVariant);
    }

    private static void MoveFileHelper(string SrcPath,string DestPath)
    {
        if (!File.Exists(SrcPath))
        {
            return;
        }

        string folderPath = System.IO.Path.GetDirectoryName(DestPath);
        if (!folderPath.Equals(string.Empty) && !System.IO.Directory.Exists(folderPath))
        {
            System.IO.Directory.CreateDirectory(folderPath);
        }

        DeleteFileHelper(DestPath);
        File.Move(SrcPath, DestPath);
    }

    private static void CopyFileHelper(string SrcPath, string DestPath)
    {
        if (!File.Exists(SrcPath))
        {
            return;
        }

        string folderPath = System.IO.Path.GetDirectoryName(DestPath);
        if (!folderPath.Equals(string.Empty) && !System.IO.Directory.Exists(folderPath))
        {
            System.IO.Directory.CreateDirectory(folderPath);
        }

        File.Copy(SrcPath, DestPath,true);
    }

    /// <summary>
    /// 创建文件
    /// </summary>
    /// <param name="path"></param>
    /// <param name="NeedToClose"></param>
    /// <returns></returns>
    private static FileStream CreateFileHelper(string path, bool NeedToClose = true)
    {

        string folderPath = System.IO.Path.GetDirectoryName(path);
        if (!folderPath.Equals(string.Empty) && !System.IO.Directory.Exists(folderPath))
        {
            System.IO.Directory.CreateDirectory(folderPath);
        }

        FileStream str = File.Create(path);
        if (NeedToClose)
        {
            str.Flush();
            str.Close();
            str = null;
        }
        return str;
    }

    /// <summary>
    /// 删除文件
    /// </summary>
    /// <param name="dealitFileName">名称</param>
    /// <param name="modifList"></param>
    /// <param name="deleteList"></param>
    private static void DeleteFileHelper(string detailFileName)
    {
        if (File.Exists(detailFileName))
        {
            File.Delete(detailFileName);
        }
    }


    public static string ConverRelativePatchToObslotePatch(string ObslotePatch)
    {
        string result = string.Empty;

        result = ObslotePatch;
        result = result.Replace("Assets/", "");
        result = Application.dataPath + "/" + result;
        return result;
    }


    public static bool Is64BitNativeDll(string filePath)
    {
        ushort architecture = 0;
        using (System.IO.FileStream fStream = new System.IO.FileStream(filePath, System.IO.FileMode.Open, System.IO.FileAccess.Read))
        {
            using (System.IO.BinaryReader bReader = new System.IO.BinaryReader(fStream))
            {
                if (bReader.ReadUInt16() == 23117) 
                {
                    fStream.Seek(0x3A, System.IO.SeekOrigin.Current); 
                    fStream.Seek(bReader.ReadUInt32(), System.IO.SeekOrigin.Begin); 
                    if (bReader.ReadUInt32() == 17744) 
                    {
                        fStream.Seek(20, System.IO.SeekOrigin.Current); 
                        architecture = bReader.ReadUInt16();
                    }
                }
            }
        }
        return architecture == 0x20b;
    }

    /// <summary>
    /// 拷贝文件夹
    /// </summary>
    /// <param name="srcdir"></param>
    /// <param name="desdir"></param>
    private static void CopyDirectory(string srcdir, string desdir)
    {

        string folderName = srcdir.Substring(srcdir.LastIndexOf("/") + 1);

        string desfolderdir = desdir + "/" + folderName;

        if (desdir.LastIndexOf("/") == (desdir.Length - 1))
        {
            desfolderdir = desdir + folderName;
        }
        string[] filenames = Directory.GetFileSystemEntries(srcdir);

        foreach (string file in filenames)// 遍历所有的文件和目录
        {
            string strfile = file.Replace("\\", "/");
            if (Directory.Exists(strfile))// 先当作目录处理如果存在这个目录就递归Copy该目录下面的文件
            {

                string currentdir = desfolderdir + "/" + strfile.Substring(strfile.LastIndexOf("/") + 1);
                if (!Directory.Exists(currentdir))
                {
                    Directory.CreateDirectory(currentdir);
                }

                CopyDirectory(strfile, desfolderdir);
            }

            else // 否则直接copy文件
            {
                string srcfileName = strfile.Substring(strfile.LastIndexOf("/") + 1);

                srcfileName = desfolderdir + "/" + srcfileName;


                if (!Directory.Exists(desfolderdir))
                {
                    Directory.CreateDirectory(desfolderdir);
                }


                File.Copy(strfile, srcfileName, true);
            }
        }
    }
}
