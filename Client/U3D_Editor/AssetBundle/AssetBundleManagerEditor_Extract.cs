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
    /// 是不是来自于程序版
    /// </summary>
    static bool fromTaiTanProgrma = false;

    private static void DotfuscatorScripts_Editor()
    {
        System.Diagnostics.Process dotProcess = null;
        if (File.Exists(GetEditorTimeDotPath()))
        {
            dotProcess = System.Diagnostics.Process.Start(GetDotExePath(), GetEditorTimeDotPath() + " /allstrings /m ../Assets/DotMapFile/EditorDotMapFile.xcmap");
        }

        while (true)
        {
            if (dotProcess == null)
            {
                break;
            }
            bool needToBread = true;
            if (null != dotProcess)
            {
                if (!dotProcess.HasExited)
                {
                    needToBread = false;
                }
            }

            if (needToBread)
            {
                break;
            }
        }
        AssetDatabase.Refresh();
    }

    private static void DotfuscatorScripts_RunTime()
    {
        if(File.Exists("notDAssmbly.txt"))
        {
            return;
        }
        System.Diagnostics.Process dotProcess = null;
        if (File.Exists(GetRunTimeDotPath()))
        {
            dotProcess = System.Diagnostics.Process.Start(GetDotExePath(), GetRunTimeDotPath() + " /allstrings /m ../Assets/DotMapFile/RunTimeDotMapFile.xcmap");
        }
        
        while(true)
        {
            if (dotProcess == null)
            {
                break;
            }
            bool needToBread = true;
            if (null != dotProcess)
            {
                if (!dotProcess.HasExited)
                {
                    needToBread = false;
                }
            }

            if(needToBread)
            {
                break;
            }
        }
    }

    /// <summary>
    /// 打包SCP文件
    /// </summary>
    private static void PackingScp()
    {
        //程序自己发布版本不打包SCP,改为直接复制文件夹过去，因为程序这边没有打包器
        if (fromTaiTanProgrma)
        {
            CopyDirectory(GetEdtiorScpPathRootNotLine(), GetRunTimeBinPatchRoot() + "Data");
            return;
        }
        if(!File.Exists(GetMPKPackerPath()))
        {
            Debug.LogError("找不到" + GetMPKPackerPath() + ",打包配置文件失败，请将文件放到指定路径下");
            return;
        }

       
       
        foreach (string str in NotPackingScpFile)
        {
            MoveFileHelper(GetEdtiorScpPathRootNotLine() + "/" + str, GetBuildFlowFloderPathRoot() + str);
        }

        string floder = System.IO.Path.GetDirectoryName( GetRunTimeScpMPKPath());
        if(!System.IO.Directory.Exists(floder))
        {
            System.IO.Directory.CreateDirectory(floder);
        }
        //删除runtime目录下的的SCP文件夹，如果有
        if (System.IO.Directory.Exists(GetRunTimeScpPatchNotLine()))
        {
            System.IO.Directory.Delete(GetRunTimeScpPatchNotLine(), true);
        }
        System.Diagnostics.Process dotProcess = null;
        dotProcess = System.Diagnostics.Process.Start(GetMPKPackerPath(), "batchmode" + " " + GetRunTimeScpMPKPath() + " " + GetEdtiorScpPathRootNotLine());
        while (true)
        {
            if (dotProcess == null)
            {
                break;
            }
            bool needToBread = true;
            if (null != dotProcess)
            {
                if (!dotProcess.HasExited)
                {
                    needToBread = false;
                }
            }

            if (needToBread)
            {
                break;
            }
        }

        foreach (string str in NotPackingScpFile)
        {
            MoveFileHelper(GetBuildFlowFloderPathRoot() + str, GetEdtiorScpPathRootNotLine() + "/" + str);
        }
    }

    /// <summary>
    /// 项目发布完成后
    /// </summary>
    /// <returns></returns>
    private static void OnPostBuildProject(bool debugVersion, bool forceEncodeFile, bool Is64bit)
    {

        EditorLog.LogInfo("正在计算发布后的文件差异....", true);
        GenerateOtherInfo(debugVersion, Is64bit);

        List<string> OtherModifList = null;
        List<string> OtherDeleteList = null;
        ReadDetailFile(GetOtherDetailFileNamePath(), ref OtherModifList, ref OtherDeleteList);

        //资源文件需要读进来，因为是发布前生成的
        List<string> ResModifList = null;
        List<string> ResDeleteList = null;
        ReadDetailFile(GetResDetailFileNamePath(), ref ResModifList, ref ResDeleteList);

        EditorLog.LogInfo("正在备份发布文件....", true);
        string BackUpFileFloder = "PackingFileBackUp/";
        BackUpFileFloder += DateTime.Now.Year + "-" + DateTime.Now.Month + "-" + DateTime.Now.Day + "-" + DateTime.Now.Hour + "-" + DateTime.Now.Minute + "/";

        BackUpFiles_First(BackUpFileFloder);
        
        Dictionary<string, AssetBundleManager.PackageNode>  NewResFileList = new Dictionary<string, AssetBundleManager.PackageNode>();
        Dictionary<string, AssetBundleManager.PackageNode>  NewOtherFileList = new Dictionary<string, AssetBundleManager.PackageNode>();
        AssetBundleManager.LoadPackageList(GetEditorAssetPackagePatchRoot() + AssetBundleManager.AssetPackageListPatch, NewResFileList, NewOtherFileList);

        CorrectRunTimeAssetFile(NewResFileList);

        EncodeFile(NewResFileList, NewOtherFileList, ResModifList, ResDeleteList, OtherModifList, OtherDeleteList,debugVersion, forceEncodeFile);

        NewResFileList.Clear();
        NewOtherFileList.Clear();

        DeleteFileHelper(GetResDetailFileNamePath());
        DeleteFileHelper(GetOtherDetailFileNamePath());
        BackUpFiles_Second(BackUpFileFloder);
    }

    private static void BackUpFiles_First(string BackUpFileFloder)
    {
        string otherDstFilePath = BackUpFileFloder + OtherDetailFileName;
        otherDstFilePath += backFileExe;

        string resDstFilePath = BackUpFileFloder + ResDetailFileName;
        resDstFilePath += backFileExe;

        string EditorPkgListDstFilePath = BackUpFileFloder + "Editor-" + AssetBundleManager.AssetPackageListPatch;
        EditorPkgListDstFilePath += backFileExe;

        string RunTimePkgListDstFilePath = BackUpFileFloder + "RunTime-" + AssetBundleManager.AssetPackageListPatch;
        RunTimePkgListDstFilePath += backFileExe;

        CopyFileHelper(GetOtherDetailFileNamePath(), otherDstFilePath);
        CopyFileHelper(GetResDetailFileNamePath(), resDstFilePath);
        CopyFileHelper(GetRunTimeAssetPackagePatchRoot() + AssetBundleManager.AssetPackageListPatch, RunTimePkgListDstFilePath);
        CopyFileHelper(GetEditorAssetPackagePatchRoot() + AssetBundleManager.AssetPackageListPatch, EditorPkgListDstFilePath);




    }

    private static void BackUpFiles_Second(string BackUpFileFloder)
    {
        string RunTimePkgListDstFilePath = BackUpFileFloder + "NewRunTime-" + AssetBundleManager.AssetPackageListPatch;
        RunTimePkgListDstFilePath += backFileExe;

        CopyFileHelper(GetRunTimeAssetPackagePatchRoot() + AssetBundleManager.AssetPackageListPatch, RunTimePkgListDstFilePath);
    }

    /// <summary>
    /// 要在exe发布之前调用
    /// </summary>
    private static void GenurateMDB()
    {
        GenurateMDBSigle("U3D_ImageEffect.bat");
        GenurateMDBSigle("U3D_Render.bat");
        GenurateMDBSigle("U3D_StandardAssets.bat");
        GenurateMDBSigle("U3D_UIFramework.bat");
    }

    private static void GenurateMDBSigle(string batName)
    {
        string patch = GetMDBGenuratorPathRoot() + batName;
        if (File.Exists(patch))
        {
            System.Diagnostics.Process dotProcess = null;
            dotProcess = System.Diagnostics.Process.Start(patch);
            while (true)
            {
                if (dotProcess == null)
                {
                    break;
                }
                bool needToBread = true;
                if (null != dotProcess)
                {
                    if (!dotProcess.HasExited)
                    {
                        needToBread = false;
                    }
                }

                if (needToBread)
                {
                    break;
                }
            }

        }
    }



    /// <summary>
    /// 拷贝UIDLL
    /// </summary>
    private static void CopyUIDll()
    {
        string UIDllSrcPath = GetClient_GamePathRoot() + "UnityEngine.UI_runtime.dll";
        string UIDllDstPath = GetRunTimeManagedScriptPathRoot() + "UnityEngine.UI.dll";

        if (File.Exists(UIDllSrcPath) && File.Exists(UIDllDstPath))
        {
            CopyFileHelper(UIDllSrcPath, UIDllDstPath);
        }
        else
        {
            Debug.LogError("找不到UnityEngine.UI_runtime.dll,请联系程序！！");
        }

    }

    /// <summary>
    /// 拷贝Mono
    /// </summary>
    private static void CopyMono(bool is64bit)
    {
        //拷贝mono
        string monoName = is64bit ? "mono_runtime64.dll" : "mono_runtime.dll";
        string MonoSrcPath = GetClient_GamePathRoot() + monoName;
        string MonoDstPath = GetRunTimeBinPatchRoot() + GameDataName + "/Mono/mono.dll";

        if (File.Exists(MonoSrcPath) && File.Exists(MonoDstPath))
        {
            File.Copy(MonoSrcPath, MonoDstPath, true);
            if (File.Exists("monodebug.txt"))
            {
                if (File.Exists(MonoSrcPath.Replace(".dll", ".pdb")))
                {
                    File.Copy(MonoSrcPath.Replace(".dll", ".pdb"), MonoDstPath.Replace(".dll", ".pdb"), true);
                }
            }
        }
        else
        {
            Debug.LogError("找不到Mono.dll,请联系程序！！");
        }

    }

    /// <summary>
    /// 拷贝unitydll
    /// </summary>
    /// <param name="dllName"></param>
    /// <param name="debugVersion"></param>
    private static void CopyUnityDll(string dllName,bool debugVersion)
    {
        foreach(string s in UnityDllsPath)
        {
            if(s.Contains(dllName))
            {
                string srcDllPath = GetClient_GamePathRoot() + s;
                string DstDllPath = GetRunTimeManagedScriptPathRoot() + dllName;
                CopyFileHelper(srcDllPath, DstDllPath);
                if(debugVersion)
                {
                    //mdb
                    CopyFileHelper(srcDllPath + ".mdb", DstDllPath + ".mdb");
                    //pdb
                    CopyFileHelper(srcDllPath.Replace(".dll", ".pdb"), DstDllPath.Replace(".dll", ".pdb"));
                }
                break;
            }
        }
    }

    /// <summary>
    /// 导出其他文件到runtime，其他文件是指除资源包以外的所有文件
    /// </summary>
    /// <param name="debugversion"></param>
    private static void ExportOhterFilesToRunTime(bool debugversion,bool Is64bit)
    {
        string exePath = GetRunTimeBinPatchRoot();
        if (!File.Exists(exePath + GameName + "." + GameExeName) ||
           !System.IO.Directory.Exists(exePath + GameDataName))
        {
            Debug.LogWarning("在:" + GetRunTimeBinPatchRoot() + "找不到TTClient.exe或者TTClient_Data,请将exe发布到这个目录下，否则要自己复制文件过去");
            return;
        }

        GenerateOtherInfo(debugversion,Is64bit);

        List<string> OtherModifList = null;
        List<string> OtherDeleteList = null;
        ReadDetailFile(GetOtherDetailFileNamePath(), ref OtherModifList, ref OtherDeleteList);
        UpdateRunTime_Other(OtherModifList, OtherDeleteList,true);

        CopyFileHelper(GetOtherDetailFileNamePath(), GetOtherDetailFileNamePath() + backFileExe);
        DeleteFileHelper(GetOtherDetailFileNamePath());
    }

    /// <summary>
    /// 将packagelist导出给编辑器，是不需要加密的
    /// </summary>
    /// <param name="path"></param>
    /// <param name="ResResult"></param>
    /// <param name="OtherResult"></param>
    private static void ExportPackageListToEditor(Dictionary<string, AssetBundleManager.PackageNode> ResResult,
                                       Dictionary<string, AssetBundleManager.PackageNode> OtherResult)
    {
        ExportPackageList(GetEditorAssetPackagePatchRoot() + AssetBundleManager.AssetPackageListPatch, ResResult, OtherResult);
    }
    /// <summary>
    /// 将packagelist导出给runtime，是加密的
    /// </summary>
    /// <param name="path"></param>
    /// <param name="ResResult"></param>
    /// <param name="OtherResult"></param>
    private static void ExportPackageListToRunTime(Dictionary<string, AssetBundleManager.PackageNode> ResResult,
                                       Dictionary<string, AssetBundleManager.PackageNode> OtherResult)
    {
        //然后再对文件列表进行加密进行加密
        string runTimeAssetPcakagePath = GetRunTimeAssetPackagePatchRoot() + AssetBundleManager.AssetPackageListPatch;
        ExportPackageList(runTimeAssetPcakagePath, ResResult, OtherResult);
        EnCodeAssetConfigFileHelper(runTimeAssetPcakagePath, runTimeAssetPcakagePath);
    }

    /// <summary>
    /// 纯粹的导出资源包
    /// </summary>
    /// <param name="path"></param>
    /// <param name="ResResult"></param>
    /// <param name="OtherResult"></param>
    private static void ExportPackageList(string path,Dictionary<string, AssetBundleManager.PackageNode> ResResult,
                                   Dictionary<string, AssetBundleManager.PackageNode> OtherResult)
    {
        StreamWriter writer = new StreamWriter(path, false, new System.Text.UTF8Encoding(true));

        //再写入资源
        WriteToFile(writer, ResResult);
        writer.WriteLine(AssetBundleManager.SplitCSVKey);

        //再写入其他文件
        WriteToFile(writer, OtherResult);

        writer.Flush();
        writer.Close();

    }

    /// <summary>
    /// 将table写入到文件
    /// </summary>
    /// <param name="writer"></param>
    /// <param name="table"></param>
    private static void WriteToFile(StreamWriter writer, Dictionary<string, AssetBundleManager.PackageNode> table)
    {
        if (table == null || table.Count <= 0)
        {
            return;
        }
        
        foreach (AssetBundleManager.PackageNode nod in table.Values)
        {
            string restult = nod.md5 + "," + nod.assetBundlePatch + "," + nod.fileSize.ToString("F3");
            writer.WriteLine(restult);
        }
    }

    /// <summary>
    /// 对客户端进行加密
    /// </summary>
    /// <param name="resultTable"></param>
    /// <param name="debugVersion"></param>
    private static void EncodeFile(Dictionary<string, AssetBundleManager.PackageNode> NewResFileList, Dictionary<string, AssetBundleManager.PackageNode> NewOtherFileList,
                                   List<string> ResModifList, List<string> ResDeleteList,
                                   List<string> OtherModifList, List<string> OtherDeleteList, 
                                   bool debugvision, bool forceEncode = false)
    {

        if (File.Exists("forceencode.txt") ||
            !System.IO.Directory.Exists(GetRunTimeAssetPackagePatchRoot()) || forceEncode)
        {
            EncodeFile_All(NewResFileList, NewOtherFileList, debugvision);
        }
        else
        {
            EncodeFile_Addtive(ResModifList, ResDeleteList, OtherModifList, OtherDeleteList,debugvision);
        }
        
    }

    /// <summary>
    /// 全部加密一遍
    /// </summary>
    private static void EncodeFile_All(Dictionary<string, AssetBundleManager.PackageNode> ResFileList, Dictionary<string, AssetBundleManager.PackageNode> OtherFileList,bool debugvision)
    {

        List<string> tempList = new List<string>();
        #region Res
        EditorLog.LogInfo("正在进行资源文件加密....", true);
        tempList.AddRange(ResFileList.Keys);
        if (!IsEnableMultThread())
        {
            ProcessEncodeFileNode(PackageNodeType.AssetsBundleFile, tempList, ref ResFileList);
        }
        else
        {
            List<EncodeFileNode> AsyncList = DispatchEncodeFileNode(PackageNodeType.AssetsBundleFile, tempList, ResFileList, AssetBundleManager.MaxActiveThread);

            while (!IsEncodeFileFinish(AsyncList)) ;

            CombineEncodeFileTable(ref ResFileList, AsyncList);
        }
        #endregion

        #region other
        EditorLog.LogInfo("正在进行代码、配置文件加密....", true);
        tempList.Clear();
        tempList.AddRange(OtherFileList.Keys);

        if (!IsEnableMultThread())
        {
            ProcessEncodeFileNode(PackageNodeType.OtherFile, tempList, ref OtherFileList);
        }
        else
        {
            List<EncodeFileNode> AsyncList = DispatchEncodeFileNode(PackageNodeType.OtherFile, tempList, OtherFileList, AssetBundleManager.MaxActiveThread);

            while (!IsEncodeFileFinish(AsyncList)) ;

            CombineEncodeFileTable(ref OtherFileList, AsyncList);
        }
        #endregion
        ExportPackageListToRunTime(ResFileList, OtherFileList);

    }

    /// <summary>
    /// 增量加密，只对修改的做压缩
    /// </summary>
    private static void EncodeFile_Addtive(List<string> ResModifList, List<string> ResDeleteList
                                   ,List<string> OtherModifList, List<string> OtherDeleteList,
                                   bool debugvision)
    {
        if(null == ResModifList && null == OtherModifList )
        {
            return;
        }
        if(ResModifList.Count <= 0 && OtherModifList.Count <= 0 )
        {
            return;
        }

        ExportPackageListToRunTime(UpdateRunTime_Res(ResModifList, ResDeleteList), UpdateRunTime_Other(OtherModifList,OtherDeleteList));
    }

    /// <summary>
    /// 更新runtime的资源
    /// </summary>
    /// <param name="ResModifList">资源列表</param>
    /// <param name="ResDeleteList">其他列表</param>
    /// <param name="needToExportFile">是否需要导出文件,如果为true的话，会调用ExportPackageListToRunTime</param>
    /// <returns>更新后的资源列表</returns>
    private static Dictionary<string, AssetBundleManager.PackageNode> UpdateRunTime_Res(List<string> ResModifList,List<string> ResDeleteList,bool needToExportFile = false)
    {

        string runTimeAssetPcakagePath = GetRunTimeAssetPackagePatchRoot();
        Dictionary<string, AssetBundleManager.PackageNode> runTimeResFileList = new Dictionary<string, AssetBundleManager.PackageNode>();
        Dictionary<string, AssetBundleManager.PackageNode> runTimeOhterFilerList = new Dictionary<string, AssetBundleManager.PackageNode>(); ;

        EditorLog.LogInfo("正在进行资源文件加密....", true);
        //加载runtime的进来
        AssetBundleManager.ForceOpenDeCode = true;
        AssetBundleManager.LoadPackageList(GetRunTimeAssetPackagePatchRoot() + AssetBundleManager.AssetPackageListPatch, runTimeResFileList, runTimeOhterFilerList);
        AssetBundleManager.ForceOpenDeCode = false;

        if (null == ResModifList)
        {
            return runTimeResFileList;
        }
        if (ResModifList.Count <= 0 )
        {
            return runTimeResFileList;
        }

        if (!IsEnableMultThread())
        {
            ProcessEncodeFileNode(PackageNodeType.AssetsBundleFile, ResModifList, ref runTimeResFileList);
        }
        else
        {
            List<EncodeFileNode> AsyncList = DispatchEncodeFileNode(PackageNodeType.AssetsBundleFile, ResModifList, runTimeResFileList, AssetBundleManager.MaxActiveThread);

            while (!IsEncodeFileFinish(AsyncList)) ;

            CombineEncodeFileTable(ref runTimeResFileList, AsyncList);
        }

        if(null != ResDeleteList)
        {
            foreach (string str in ResDeleteList)
            {
                if (runTimeResFileList.ContainsKey(str))
                {
                    runTimeResFileList.Remove(str);
                }
            }
        }


        if(needToExportFile)
        {
            ExportPackageListToRunTime(runTimeResFileList,runTimeOhterFilerList);
        }
        
        runTimeOhterFilerList.Clear();
        return runTimeResFileList;
    }


    /// <summary>
    /// 更新runtime的其他文件
    /// </summary>
    /// <param name="OtherModifList">修改列表</param>
    /// <param name="OtherDeleteList">删除列表</param>
    /// <param name="needToExportFile">是否需要导出文件,如果为true的话，会调用ExportPackageListToRunTime</param>
    /// <returns>更新后的资源列表</returns>
    private static Dictionary<string, AssetBundleManager.PackageNode> UpdateRunTime_Other(List<string> OtherModifList, List<string> OtherDeleteList, bool needToExportFile = false)
    {


        string runTimeAssetPcakagePath = GetRunTimeAssetPackagePatchRoot();
        Dictionary<string, AssetBundleManager.PackageNode> runTimeResFileList = new Dictionary<string,AssetBundleManager.PackageNode>();
        Dictionary<string, AssetBundleManager.PackageNode> runTimeOhterFilerList = new Dictionary<string, AssetBundleManager.PackageNode>();

        EditorLog.LogInfo("正在进行代码、配置文件加密....", true);

        //加载runtime的进来
        AssetBundleManager.ForceOpenDeCode = true;
        AssetBundleManager.LoadPackageList(GetRunTimeAssetPackagePatchRoot() + AssetBundleManager.AssetPackageListPatch, runTimeResFileList, runTimeOhterFilerList);
        AssetBundleManager.ForceOpenDeCode = false;

        if (OtherModifList.Count <= 0)
        {
            return runTimeOhterFilerList;
        }
        if (!IsEnableMultThread())
        {
            ProcessEncodeFileNode(PackageNodeType.OtherFile, OtherModifList, ref runTimeOhterFilerList);
        }
        else
        {
            List<EncodeFileNode> AsyncList = DispatchEncodeFileNode(PackageNodeType.OtherFile, OtherModifList, runTimeOhterFilerList, AssetBundleManager.MaxActiveThread);

            while (!IsEncodeFileFinish(AsyncList)) ;

            CombineEncodeFileTable(ref runTimeOhterFilerList, AsyncList);
        }

        if (null != OtherDeleteList)
        {
            foreach (string str in OtherDeleteList)
            {
                if (runTimeOhterFilerList.ContainsKey(str))
                {
                    runTimeOhterFilerList.Remove(str);
                }
            }
        }

        if (needToExportFile)
        {
            ExportPackageListToRunTime(runTimeResFileList, runTimeOhterFilerList);
        }
        runTimeResFileList.Clear();
        return runTimeOhterFilerList;
        
        
    }



    /// <summary>
    /// 计算资源文件列表，资源包部分
    /// </summary>
    /// <param name="resultTable"></param>
    /// <param name="debugVersion"></param>
    private static void GenerateAssetPackageList_Res(string AssetPackageListPatch,ref Dictionary<string, AssetBundleManager.PackageNode> resultTable, bool debugVersion)
    {
        List<string> folders = new List<string>();

        if( null == resultTable)
        {
            resultTable = new Dictionary<string, AssetBundleManager.PackageNode>();
        }
        /*除了meta、资源列表、mainifest，全都要更新
         * 去掉对PrefabVariant的版本记载,改用合并的
         */
        folders.AddRange(System.IO.Directory.GetFiles(AssetPackageListPatch, "*.*", SearchOption.AllDirectories).Where(
                        str =>
                        {
                            foreach(string excPath in ResExcPathList)
                            {
                                if(str.Contains(excPath))
                                {
                                    return false;
                                }
                            }
                            return true;
                        })
                    );


        if (!IsEnableMultThread())
        {
            GeneratePackageNode(PackageNodeType.AssetsBundleFile, folders, ref resultTable, AssetBundleManager.FullAssetPackagePathRoot);
        }
        else
        {
            List<AssetPackageListNode> AsyncList = DispatchPackageNode(PackageNodeType.AssetsBundleFile,folders, AssetBundleManager.FullAssetPackagePathRoot, AssetBundleManager.MaxActiveThread);

            while (!IsPackageNodeFinish(AsyncList)) ;

            CombinePakcageNodeTable(ref resultTable, AsyncList);
        }
    }

    /// <summary>
    /// 计算出具体修改了或者删除了哪些
    /// </summary>
    /// <param name="newResTable"></param>
    /// <param name="newOtherTable"></param>
    /// <param name="reslutResTable"></param>
    /// <param name="reslutOtherTable"></param>
    private static void GenerateDetail(Dictionary<string, AssetBundleManager.PackageNode> newResTable, Dictionary<string, AssetBundleManager.PackageNode> oldResTable,
                                           ref List<string> modifList,ref List<string> deleteList)
    {
        //把旧的加载进来

        if (null == modifList)
        {
            modifList = new List<string>();
        }

        if (null == deleteList)
        {
            deleteList = new List<string>();
        }
        modifList.Clear();
        deleteList.Clear();

        //遍历旧的包
        foreach(string s in oldResTable.Keys)
        {
            //如果新的包没有这个，那就是被删除了
            if(!newResTable.ContainsKey(s))
            {
                deleteList.Add(s);
            }
            else
            {

                //他们的GUID不一样或者是C#脚本（C#代码unity会自动复制过去，也就是没有加密的，所以每次导出都是有修改的）
                if (!newResTable[s].md5.Equals(oldResTable[s].md5) || IsManagedScript(s) )
                {
                    modifList.Add(s);
                }
            }
        }

        //遍历新的包
        foreach (string s in newResTable.Keys)
        {
            //如果旧的包没有这个，那就是新增的
            if (!oldResTable.ContainsKey(s))
            {
                modifList.Add(s);
            }
        }
    }

    /// <summary>
    /// 将修改的细节写入到文件
    /// </summary>
    /// <param name="dealitFileName">名称</param>
    /// <param name="modifList"></param>
    /// <param name="deleteList"></param>
    private static void WriteDetailFile(string dealitFileName, List<string> modifList, List<string> deleteList)
    {
        if (null == modifList || deleteList == null)
        {
            return;
        }

        if (modifList.Count <= 0 && deleteList.Count <= 0)
        {
            if (File.Exists(dealitFileName))
            {
                File.Delete(dealitFileName);
            }
            return;
        }

        StreamWriter writer = new StreamWriter(dealitFileName, false, new System.Text.UTF8Encoding(true));
        foreach(string s in modifList)
        {
            writer.WriteLine(s);
        }
        writer.WriteLine(AssetBundleManager.SplitCSVKey);
        foreach (string s in deleteList)
        {
            writer.WriteLine(s);
        }
        writer.Flush();
        writer.Close();
    }

    /// <summary>
    /// 读取细节文件
    /// </summary>
    /// <param name="dealitFileName">名称</param>
    /// <param name="modifList"></param>
    /// <param name="deleteList"></param>
    private static void ReadDetailFile(string dealitFileName, ref List<string> modifList, ref List<string> deleteList)
    {
        if (null == modifList)
        {
            modifList = new List<string>();
        }

        if (null == deleteList)
        {
            deleteList = new List<string>();
        }

        if (!File.Exists(dealitFileName))
        {
            return;
        }

        string stSchemeAllText = File.ReadAllText(dealitFileName);

        if (stSchemeAllText.Equals(string.Empty))
        {
            return;
        }

        //去掉\n,因为ScpReader以"\r"分割
        stSchemeAllText = stSchemeAllText.Replace("\n", "");
        //再干掉最后的"\r"
        stSchemeAllText = stSchemeAllText.Remove(stSchemeAllText.Length - 1, 0);
        ScpReader packageReader = new ScpReader(stSchemeAllText, "packageList", 0);

        int currentIndex = 0;
        //先读修改的
        for (int i = currentIndex; i < packageReader.GetRecordCount(); ++i)
        {
            string SplitKey = packageReader.GetString(i, 0, string.Empty);
            if (SplitKey.Contains(AssetBundleManager.SplitCSVKey))
            {
                currentIndex = i + 1;
                break;
            }
            modifList.Add(SplitKey);
        }


        //再读取删除的
        for (int i = currentIndex; i < packageReader.GetRecordCount(); ++i)
        {
            string SplitKey = packageReader.GetString(i, 0, string.Empty);
            if (SplitKey.Contains(AssetBundleManager.SplitCSVKey))
            {
                currentIndex = i + 1;
                break;
            }

              deleteList.Add(SplitKey);
        }
        packageReader = null;
    }


    /// <summary>
    /// 除了资源包以外的所有文件
    /// </summary>
    /// <param name="resultTable"></param>
    /// <param name="debugVersion"></param>
    private static void GenerateAssetPackageList_Other(ref Dictionary<string, AssetBundleManager.PackageNode> resultTable, bool debugVersion)
    {
        GenerateAssetPackageList_Other_BinFloderOnly(ref resultTable, debugVersion);
        GenerateAssetPackageList_Other_BinFolder(ref resultTable, debugVersion);
    }

    /// <summary>
    /// 提取其他文件-Bin文件夹下的文件
    /// </summary>
    /// <param name="resultTable"></param>
    /// <param name="debugVersion"></param>
    private static void GenerateAssetPackageList_Other_BinFloderOnly(ref Dictionary<string, AssetBundleManager.PackageNode> resultTable, bool debugVersion)
    {
        List<string> folders = new List<string>();

        if (null == resultTable)
        {
            resultTable = new Dictionary<string, AssetBundleManager.PackageNode>();
        }
        /*Bin文件夹下不要txt，不要html，不要Unity的DLL，不要XML，不要CSV,不要dmp，不要log
         */
        folders.AddRange(System.IO.Directory.GetFiles(GetRunTimeBinPatchRoot(), "*.*", SearchOption.TopDirectoryOnly).Where(
                        str =>
                        {
                            string st = str.Replace("\\", "/");
                            foreach (string exc in OtherTopFolderExcPathList)
                            {
                                if (st.Contains(exc))
                                {
                                    return false;
                                }

                                if (str.Contains(".pdb") || str.Contains(".mdb"))
                                {
                                    if (!st.Contains("U3D_") && !st.Contains("GameAPI_"))
                                    {
                                        return debugVersion;
                                    }
                                    return false;
                                }
  
                            }
                            return true;
                        })
                        );

        if (!IsEnableMultThread())
        {
            GeneratePackageNode(PackageNodeType.OtherFile, folders, ref resultTable, GetRunTimeBinPatchRoot());
        }
        else
        {
            List<AssetPackageListNode> AsyncList = DispatchPackageNode(PackageNodeType.OtherFile, folders, GetRunTimeBinPatchRoot(), AssetBundleManager.MaxActiveThread);

            while (!IsPackageNodeFinish(AsyncList)) ;

            CombinePakcageNodeTable(ref resultTable, AsyncList);
        }

        
    }

    /// <summary>
    /// 提取bin下的文件夹中的文件
    /// </summary>
    /// <param name="resultTable"></param>
    /// <param name="debugVersion"></param>
    private static void GenerateAssetPackageList_Other_BinFolder(ref Dictionary<string, AssetBundleManager.PackageNode> resultTable, bool debugVersion)
    {
        List<string> folders = new List<string>();
        //取得bin文件夹下的所有文件夹
        folders.AddRange(System.IO.Directory.GetDirectories(GetRunTimeBinPatchRoot()));
        if (null == resultTable)
        {
            resultTable = new Dictionary<string, AssetBundleManager.PackageNode>();
        }
        List<string> fileList = new List<string>();
        foreach(string folder in folders)
        {
            fileList.AddRange(System.IO.Directory.GetFiles(folder, "*.*", SearchOption.AllDirectories).Where(
                            str =>
                            {
                                string st = str.Replace("\\", "/");
                                foreach (string exc in OtherFloderExcPathList)
                                {
                                    if (st.Contains(exc))
                                    {
                                        return false;
                                    }
                                    if (str.Contains(".pdb") || str.Contains(".mdb"))
                                    {
                                        return debugVersion;
                                    }
   
                                }

                                if (st.Contains(GetRunTimeAssetPackagePatchRoot()))
                                {
                                    return false;
                                }

                                return true;
                            })
                        );
        }
        if (!IsEnableMultThread())
        {
            GeneratePackageNode(PackageNodeType.OtherFile, fileList, ref resultTable, GetRunTimeBinPatchRoot());
        }
        else
        {
            List<AssetPackageListNode> AsyncList = DispatchPackageNode(PackageNodeType.OtherFile, fileList, GetRunTimeBinPatchRoot(), AssetBundleManager.MaxActiveThread);

            while (!IsPackageNodeFinish(AsyncList)) ;

            CombinePakcageNodeTable(ref resultTable, AsyncList);
        }
    }
}
