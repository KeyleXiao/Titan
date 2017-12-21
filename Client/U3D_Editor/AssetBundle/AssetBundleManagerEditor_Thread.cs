/*******************************************************************
** 文件名:	AssetBundleManagerEditor_Extract.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	ab资源系统
** 应  用:  	ab资源管理器编辑器,提取资源更新

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
    /// <summary>
    /// 每个线程最少应该处理多少个数据
    /// </summary>
    const int MinThreadProcessCout = 30;
    private static bool IsEnableMultThread()
    {
        return false;// !File.Exists("PackingSingleThread.txt");
    }

    #region GeneratePackageNode
    private static bool IsPackageNodeFinish(IEnumerable<AssetPackageListNode> list)
    {
        if(null == list || list.Count<AssetPackageListNode>() <= 0)
        {
            return true;
        }
        bool bFinsh = true;
        foreach (AssetPackageListNode nod in list)
        {
            if (!IsPackageNodeFinish(nod))
            {
                bFinsh = false;
                break;
            }
        }
        return bFinsh;
    }

    private static bool IsPackageNodeFinish(AssetPackageListNode nod)
    {
        return null == nod || nod.isDone;
    }

    private static void CombinePakcageNodeTable(ref Dictionary<string, AssetBundleManager.PackageNode> reslut, IEnumerable<AssetPackageListNode> list)
    {
        if(null == list)
        {
            return;
        }
        foreach (AssetPackageListNode nod in list)
        {
            foreach (string key in nod.resultTable.Keys)
            {
                reslut.Add(key, nod.resultTable[key]);
            }
        }
    }

    /// <summary>
    /// 计算packageNode
    /// </summary>
    /// <param name="pathList"></param>
    /// <param name="resultTable"></param>
    private static void GeneratePackageNode(PackageNodeType packageType,List<string> pathList, ref Dictionary<string, AssetBundleManager.PackageNode> resultTable,
                                            string ignorePackageNameStr)
    {
        foreach (string s in pathList)
        {
            GeneratePackageNode(packageType,s, ref resultTable, ignorePackageNameStr);
        }
    }

    private static void GeneratePackageNode(PackageNodeType packageType,string path, ref Dictionary<string, AssetBundleManager.PackageNode> resultTable,
                                             string ignorePackageNameStr)
    {
        AssetFileInfo info = GetFileInfo(path);
        if (null == info)
        {
            return;
        }
        if (null == resultTable)
        {
            resultTable = new Dictionary<string, AssetBundleManager.PackageNode>();
        }

        AssetBundleManager.PackageNode nod = new AssetBundleManager.PackageNode();

        string packName = path.Replace("\\", "/");
        packName = packName.Replace(ignorePackageNameStr, string.Empty);


        nod.md5 = info.md5;
        nod.assetBundlePatch = packName;
        nod.fileSize = info.fileLengthInMB;

        if (packageType == PackageNodeType.AssetsBundleFile)
        {
             /*
             * 对已经不用的资源包进行删除操作
             */
            if (!IsConfigFile(path) && !IsOnAssetBundle(packName) && !path.EndsWith(AssetBundleManager.PrefabUnionVariant))
            {
                if (File.Exists(path))
                {
                    File.Delete(path);
                    if (File.Exists(path + ".meta"))
                    {
                        File.Delete(path + ".meta");
                    }

                    if (File.Exists(path + ".manifest"))
                    {
                        File.Delete(path + ".manifest");

                        if (File.Exists(path + ".manifest" + ".meta"))
                        {
                            File.Delete(path + ".manifest" + ".meta");
                        }
                    }
                }
                return;
            }
        }

        if (!path.Contains(AssetBundleManager.PrefabVariant))
        {
            resultTable.Add(packName, nod);
        }
    }

    /// <summary>
    /// 计算packageNode工作线程函数
    /// </summary>
    /// <param name="data"></param>
    private static void GeneratePackageNodeWorckFunc(System.Object data)
    {
        AssetPackageListNode node = (AssetPackageListNode)data;
        if (null == node)
        {
            return;
        }
        GeneratePackageNode(node.nodeType,node.paths, ref node.resultTable,node.ignorePackageNameStr);
        node.isDone = true;
    }

    /// <summary>
    /// 分发需要计算PackageNode的路径到各个线程
    /// </summary>
    /// <param name="allPath"></param>
    /// <param name="ThreadCount"></param>
    /// <returns></returns>
    private static List<AssetPackageListNode> DispatchPackageNode(PackageNodeType packageType, List<string> allPath, string ignorePackageNameStr, int ThreadCount)
    {
        if (null == allPath)
        {
            return null;
        }
        if (allPath.Count <= 0 )
        {
            return null;
        }

        List<AssetPackageListNode> reslut = new List<AssetPackageListNode>();
        //根据每个线程处理的最少数目，来分配需要多少个线程，最多只有ThreadCount个线程
        int realyThreadCount = Mathf.Clamp(allPath.Count / MinThreadProcessCout,1,ThreadCount);

        int clipCount = allPath.Count / realyThreadCount;
        //遇到有余数的，待会直接分配到最后一根线程
        int Remainder = allPath.Count - (realyThreadCount * clipCount);

        List<string>[] clipArray = new List<string>[realyThreadCount];


        for (int i = 0; i < realyThreadCount; i++)
        {
            AssetPackageListNode node = new AssetPackageListNode();
            node.isDone = false;
            for (int j = i * clipCount; j < (i + 1) * clipCount; j++)
            {
                node.paths.Add(allPath[j]);
            }
            node.ignorePackageNameStr = ignorePackageNameStr;
            node.nodeType = packageType;
            reslut.Add(node);
        }

        //余下的分配给最后一根线程
        for (int j = 0; j < Remainder; j++)
        {
            reslut[realyThreadCount - 1].paths.Add(allPath[realyThreadCount * clipCount + j]);
        }

        foreach (AssetPackageListNode node in reslut)
        {
            AssetBundleManager.PushWork(GeneratePackageNodeWorckFunc, node);
        }
        return reslut;
    }
    #endregion

    #region EncodeFile

    private static bool IsEncodeFileFinish(IEnumerable<EncodeFileNode> list)
    {
        if(null == list || list.Count<EncodeFileNode>() <= 0)
        {
            return true;
        }
        bool bFinsh = true;
        foreach (EncodeFileNode nod in list)
        {
            if (!IsEncodeFileFinish(nod))
            {
                bFinsh = false;
                break;
            }
        }
        return bFinsh;
    }

    private static bool IsEncodeFileFinish(EncodeFileNode nod)
    {
        return null == nod || nod.isDone;
    }

    private static void CombineEncodeFileTable(ref Dictionary<string, AssetBundleManager.PackageNode> reslut, IEnumerable<EncodeFileNode> list)
    {
        if(null == list)
        {
            return;
        }
        foreach (EncodeFileNode nod in list)
        {
            foreach (string key in nod.resultTable.Keys)
            {
                if(reslut.ContainsKey(key))
                {
                    reslut[key] = nod.resultTable[key];
                }
                else
                {
                    reslut.Add(key, nod.resultTable[key]);
                }
            }
        }
    }

    /// <summary>
    /// 计算packageNode
    /// </summary>
    /// <param name="pathList"></param>
    /// <param name="resultTable"></param>
    private static void ProcessEncodeFileNode(PackageNodeType packageType, List<string> ModifList,ref Dictionary<string, AssetBundleManager.PackageNode> resultTable)
    {
        for (int i = 0; i < ModifList.Count;i++ )
        {
            ProcessEncodeFileNode(packageType, ModifList[i], ref resultTable);
        }
    }

    private static void ProcessEncodeFileNode(PackageNodeType packageType, string ModifPath, ref Dictionary<string, AssetBundleManager.PackageNode> resultTable)
    {
        if(packageType == PackageNodeType.AssetsBundleFile)
        {
            string s = AssetBundleManager.FullAssetPackagePathRoot + ModifPath;
            string RunTimePath = GetRunTimeAssetPackagePatchRoot() + ModifPath;

            //配置文件直接
            if (IsConfigFile(s))
            {
                EnCodeAssetConfigFileHelper(s, RunTimePath);
            }
            else
            {
                EnCodeAssetFileHelper(s, RunTimePath);

            }
            AssetFileInfo info = GetFileInfo(RunTimePath);
            if (null == info)
            {
                return;
            }

            if(info.fileLengthInBytes <= 0)
            {
                EditorLog.LogFatleError("文件为空:" + RunTimePath);
                return;
            }

            AssetBundleManager.PackageNode nod;
            if (resultTable.TryGetValue(ModifPath, out nod))
            {
                nod.md5 = info.md5;
                nod.fileSize = info.fileLengthInMB;
                resultTable[ModifPath] = nod;
            }
            else
            {
                nod = new AssetBundleManager.PackageNode();
                nod.md5 = info.md5;
                nod.assetBundlePatch = ModifPath;
                nod.fileSize = info.fileLengthInMB;
                resultTable.Add(ModifPath, nod);
            }
        }
        else if(packageType == PackageNodeType.OtherFile)
        {
            string s = GetRunTimeBinPatchRoot() + ModifPath;

            //只对C#的代码进行加密
            if (IsManagedScript(s))
            {
                EnCodeScriptHelper(s, s);
            }


            AssetFileInfo info = GetFileInfo(s);
            if (null == info)
            {
                return;
            }
            if (info.fileLengthInBytes <= 0)
            {
                EditorLog.LogFatleError("文件为空:" + s);
                return;
            }
            AssetBundleManager.PackageNode nod;
            if (resultTable.TryGetValue(ModifPath, out nod))
            {
                nod.md5 = info.md5;
                nod.fileSize = info.fileLengthInMB;
                resultTable[ModifPath] = nod;
            }
            else
            {
                nod = new AssetBundleManager.PackageNode();
                nod.md5 = info.md5;
                nod.assetBundlePatch = ModifPath;
                nod.fileSize = info.fileLengthInMB;
                resultTable.Add(ModifPath, nod);
            }
        }
    }

    /// <summary>
    /// 计算packageNode工作线程函数
    /// </summary>
    /// <param name="data"></param>
    private static void EncodeFileWorckFunc(System.Object data)
    {
        EncodeFileNode node = (EncodeFileNode)data;
        if (null == node)
        {
            return;
        }
        ProcessEncodeFileNode(node.nodeType, node.ModifPath, ref node.resultTable);
        node.isDone = true;
    }

    /// <summary>
    /// 分发需加密的文件
    /// </summary>
    /// <param name="allPath"></param>
    /// <param name="ThreadCount"></param>
    /// <returns></returns>
    private static List<EncodeFileNode> DispatchEncodeFileNode(PackageNodeType packageType, List<string> ModifPath, Dictionary<string, AssetBundleManager.PackageNode> orginalTable, int ThreadCount)
    {
        if( null == ModifPath)
        {
            return null;
        }
        if(ModifPath.Count <= 0)
        {
            return null;
        }
        List<EncodeFileNode> reslut = new List<EncodeFileNode>();

        //根据每个线程处理的最少数目，来分配需要多少个线程，最多只有ThreadCount个线程
        int realyThreadCount = Mathf.Clamp(ModifPath.Count / MinThreadProcessCout, 1, ThreadCount);

        int clipCount = ModifPath.Count / realyThreadCount;
        //遇到有余数的，待会直接分配到最后一根线程
        int Remainder = ModifPath.Count - (realyThreadCount * clipCount);

        List<string>[] clipArray = new List<string>[realyThreadCount];

        List<string> keys = new List<string>();
        keys.AddRange(orginalTable.Keys);
        for (int i = 0; i < realyThreadCount; i++)
        {
            EncodeFileNode node = new EncodeFileNode();
            node.isDone = false;
            for (int j = i * clipCount; j < (i + 1) * clipCount; j++)
            {
                string key = ModifPath[j];
                node.ModifPath.Add(key);
                if (orginalTable.ContainsKey(key))
                {
                    node.resultTable.Add(key, orginalTable[key]);
                }
               
            }
            node.nodeType = packageType;
            reslut.Add(node);
        }

        //余下的分配给最后一根线程
        for (int j = 0; j < Remainder; j++)
        {
            string key = ModifPath[realyThreadCount * clipCount + j];
            reslut[realyThreadCount - 1].ModifPath.Add(key);
            if (orginalTable.ContainsKey(key))
            {
                reslut[realyThreadCount - 1].resultTable.Add(key, orginalTable[key]);
            }

            
        }

        foreach (EncodeFileNode node in reslut)
        {
            AssetBundleManager.PushWork(EncodeFileWorckFunc, node);
        }
        return reslut;
    }

    #endregion
}
