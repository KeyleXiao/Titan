using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using System.IO;



public class AdorningExporterEx : Editor
{

    //[MenuItem("Tools/AdorningEx/Build")]
    //static void Build()
    //{
    //    AdorningManager.Init();
    //}

    public struct AdorningInfo
    {
        public Vector3 pos;
        public Vector3 angle;
        public Vector3 scale;
    }

    //[MenuItem("Tools/AdorningEx/Import From Bundle")]
    public static void ImportFromBundle()
    {
        Clear();
        ImportFun(true, BuildTarget.StandaloneWindows);
    }

    //[MenuItem("Tools/AdorningEx/Import From Prefab")]
    public static void ImportFromPrefab()
    {
        Clear();
        ImportFun(false, BuildTarget.StandaloneWindows);
    }

    public static void ImportFun(bool AdorningAssetBundleBuild, BuildTarget platform)
    {
        if (Application.isPlaying || !Application.isEditor)
        {
            Debug.LogError("Import Adorning Operation can not run in runtime, please do it in editor mode");
        }

        string scenepath = UnityEditor.SceneManagement.EditorSceneManager.GetActiveScene().name;
        string fullscenepath = Path.GetFullPath(scenepath);
        string absfolder = Path.GetDirectoryName(fullscenepath) + "/" + Path.GetFileNameWithoutExtension(fullscenepath) + "_temp";
        string folder = Path.GetDirectoryName(scenepath) + "/" + Path.GetFileNameWithoutExtension(scenepath) + "_temp";

        if (Directory.Exists(absfolder) == false)
        {
            Directory.CreateDirectory(absfolder);
        }


        if (AdorningAssetBundleBuild)
        {
            string[] absadorningfiles = Directory.GetFiles(absfolder, "*.adorningpkg");
            int nGroup = absadorningfiles.Length;
            for (int iGroup = 0; iGroup < nGroup; iGroup++)
            {

                string absfile = absadorningfiles[iGroup];
                string file = folder + "/" + Path.GetFileName(absfile);

                float fProgress = (float)iGroup / (float)nGroup;
                EditorUtility.DisplayProgressBar("Import Adorning ", "Import Asset Bundle: " + Path.GetFileName(absfile) + "...", fProgress);

                if (File.Exists(absfile) == false)
                {
                    continue;
                }

                byte[] data = File.ReadAllBytes(absfile);
                if (data == null || data.Length <= 0)
                {
                    continue;
                }

                AssetBundle assetbundle = AssetBundle.LoadFromMemory(data);
                if (assetbundle == null)
                {
                    continue;
                }

                GameObject prefab = (GameObject)assetbundle.mainAsset;
                if (prefab == null)
                {
                    continue;
                }

                data = null;
                System.GC.Collect();

                GameObject groupgo = GameObject.Instantiate(prefab);
                if (groupgo == null)
                {
                    continue;
                }
                groupgo.name = prefab.name;

                prefab = null;
                assetbundle.Unload(false);
            }

            List<AssetBundle> matbundles = new List<AssetBundle>();
            List<Object> matobjs = new List<Object>();
            string[] absmatfiles = Directory.GetFiles(absfolder, "*.matpkg");
            int nMatPkg=absmatfiles.Length;
            for (int iMatPkg = 0; iMatPkg < nMatPkg; iMatPkg++)
            {
                string absmatfile = absmatfiles[iMatPkg];
                string matname = Path.GetFileNameWithoutExtension(absmatfile);

                float fProgress = (float)iMatPkg / (float)nMatPkg;
                EditorUtility.DisplayProgressBar("Import Adorning ", "Import Asset Bundle: " + matname + "...", fProgress);

                byte[] data = File.ReadAllBytes(absmatfile);
                AssetBundle matbundle = AssetBundle.LoadFromMemory(data);
                Object[] preobjs = matbundle.LoadAllAssets();
                foreach (Object preobj in preobjs)
                {
                    matobjs.Add(preobj);
                }
                matbundles.Add(matbundle);
            }

            string[] absmeshfiles = Directory.GetFiles(absfolder, "*.meshpkg");
            int nMesh = absmeshfiles.Length;
            for (int iMesh = 0; iMesh < nMesh; iMesh++)
            {
                string absmeshfile = absmeshfiles[iMesh];
                string meshname = Path.GetFileNameWithoutExtension(absmeshfile);

                float fProgress = (float)iMesh / (float)nMesh;
                EditorUtility.DisplayProgressBar("Import Adorning ", "Import Asset Bundle: " + meshname + "...", fProgress);


                byte[] subdata = File.ReadAllBytes(absmeshfile);
                if (subdata == null || subdata.Length <= 0)
                {
                    continue;
                }

                AssetBundle subassetbundle = AssetBundle.LoadFromMemory(subdata);
                if (subassetbundle == null)
                {
                    continue;
                }

                GameObject subprefab = (GameObject)subassetbundle.mainAsset;
                if (subprefab == null)
                {
                    continue;
                }

                GameObject gosub = GameObject.Instantiate(subprefab);
                if (gosub == null)
                {
                    continue;
                }
                gosub.name = subprefab.name;

                subprefab = null;
                subassetbundle.Unload(false);
                subdata = null;
                System.GC.Collect();


            }

            foreach (AssetBundle matbundle in matbundles)
            {
                matbundle.Unload(false);
            }
            Resources.UnloadUnusedAssets();
            System.GC.Collect();
        }
        else
        {
            string[] absfiles = Directory.GetFiles(absfolder, "*.prefab");
            int nGroup = absfiles.Length;
            for (int iGroup = 0; iGroup < nGroup; iGroup++)
            {
                string absfile = absfiles[iGroup];
                string file = folder + "/" + Path.GetFileName(absfile);

                if (File.Exists(absfile) == false)
                {
                    continue;
                }

                GameObject prefab = (GameObject)AssetDatabase.LoadAssetAtPath(file, typeof(GameObject));
                if (prefab == null)
                {
                    continue;
                }
                GameObject groupgo = (GameObject)PrefabUtility.InstantiatePrefab(prefab);
                if (groupgo == null)
                {
                    continue;
                }

                float fProgress = (float)iGroup / (float)nGroup;
                EditorUtility.DisplayProgressBar("Import Adorning", "Import Prefab: " + groupgo.name + ".prefab...", fProgress);
            }
        }


        EditorUtility.ClearProgressBar();

    }

 

    //[MenuItem("Tools/AdorningEx/Export To Bundle")]
    public static void ExportToBundle()
    {
        ExportFun(true, BuildTarget.StandaloneWindows);
    }

    //[MenuItem("Tools/AdorningEx/Export To Prefab")]
    public static void ExportToPrefab()
    {
        ExportFun(false, BuildTarget.StandaloneWindows);
    }

    public static void ExportFun(bool AdorningAssetBundleBuild, BuildTarget platform)
    {
        if (Application.isPlaying || !Application.isEditor)
        {
            Debug.LogError("Export Adorning Operation can not run in runtime, please do it in editor mode");
        }


        string scenepath = EditorApplication.currentScene;
        string fullscenepath = Path.GetFullPath(scenepath);
        string absfolder = Path.GetDirectoryName(fullscenepath) + "/" + Path.GetFileNameWithoutExtension(fullscenepath) + "_temp";
        string folder = Path.GetDirectoryName(scenepath) + "/" + Path.GetFileNameWithoutExtension(scenepath) + "_temp";

        if (Directory.Exists(absfolder) == false)
        {
            Directory.CreateDirectory(absfolder);
        }

        string[] scenegroupnames = AdorningManager.scenegroupnames;
        int nAm = scenegroupnames.Length;
        for (int iAm = 0; iAm < nAm; iAm++)
        {
            string sAm = scenegroupnames[iAm];
            GameObject goam = GameObject.Find(sAm);
            if (goam == null)
            {
                continue;
            }
            Transform tam = goam.transform;

            string prefabpath = folder + "/" + sAm + ".prefab";
            GameObject filego = PrefabUtility.CreatePrefab(prefabpath, goam);

            if (AdorningAssetBundleBuild)
            {
                string assetbundlepath = folder + "/" + sAm + ".adorningpkg";
                GameObject[] filegos = new GameObject[] { filego };

                if (BuildPipeline.BuildAssetBundle(filego,
                                                    filegos,
                                                    assetbundlepath,
                                                    BuildAssetBundleOptions.CollectDependencies | BuildAssetBundleOptions.CompleteAssets,
                                                    platform))
                {
                    Debug.Log("Generate Adorning pkg OK:" + assetbundlepath);
                }
                else
                {
                    Debug.Log("Generate Adorning pkg Fail:" + assetbundlepath);
                }

                long filelength = 0;
                FileStream fs = File.Open(assetbundlepath, FileMode.Open, FileAccess.Read);
                filelength = fs.Length;
                fs.Close();
                fs.Dispose();

                int filelengthinmb = (int)(filelength / 1024 / 1024);
                if (filelengthinmb > 10)//如果一个包大于10m，则改为逐物件打包
                {
                    File.Delete(assetbundlepath);

                    BuildPipeline.PushAssetDependencies();

                    Dictionary<Object, Object> texdic = new Dictionary<Object, Object>();
                    Object[] dependencies = EditorUtility.CollectDependencies(new Object[] { goam });
                    foreach (Object dependencie in dependencies)
                    {
                        System.Type type = dependencie.GetType();
                        if (type == typeof(Texture2D) || type == typeof(Texture3D) || type == typeof(Shader))
                        {
                            texdic[dependencie] = dependencie;
                        }
                    }
                  
                    List<Object> texcol = new List<Object>();
                    foreach (KeyValuePair<Object, Object> keypair in texdic)
                    {
                        Object tex = keypair.Key;
                        texcol.Add(tex);
                    }

                    Object[] texs = texcol.ToArray();

                    int nDiv = 10;
                    int nNum = texs.Length / nDiv;
                    int nStart = 0;
                    for (int iDiv = 0; iDiv < nDiv; iDiv++)
                    {
                        if (nStart + nNum > texs.Length)
                        {
                            nNum = texs.Length - nStart;
                        }
                        List<Object> texlist = new List<Object>();
                        for (int k = 0; k < nNum; k++)
                        {
                            texlist.Add(texs[nStart + k]);
                        }
                        Object[] texarray = texlist.ToArray();
                        nStart += nNum;
                        string absmatpkgfile = absfolder + "/" + sAm + "_mat" + iDiv.ToString() + ".matpkg";
                        if (BuildPipeline.BuildAssetBundle(texarray[0],
                                                                     texarray,
                                                                     absmatpkgfile,
                                                                     BuildAssetBundleOptions.CollectDependencies | BuildAssetBundleOptions.CompleteAssets,
                                                                     platform))
                        {
                            Debug.Log("Generate Mat Pkg OK:" + absmatpkgfile);
                        }
                        else
                        {
                            Debug.Log("Generate Mat Pkg Fail:" + absmatpkgfile);
                        }
                    }

                    string subassetbundlepath = absfolder + "/" + sAm + "_mesh" + ".meshpkg";
                    if (BuildPipeline.BuildAssetBundle(filego,
                                       filegos,
                                       subassetbundlepath,
                                       BuildAssetBundleOptions.CollectDependencies | BuildAssetBundleOptions.CompleteAssets,
                                       platform))
                    {
                        Debug.Log("Generate Mesh Pkg OK:" + subassetbundlepath);
                    }
                    else
                    {
                        Debug.Log("Generate Mesh Pkg Fail:" + subassetbundlepath);
                    }


                    BuildPipeline.PopAssetDependencies();

                }

                File.Delete(prefabpath);

            }

            float fProgress = (float)iAm / (float)nAm;
            EditorUtility.DisplayProgressBar("Export Adorning", "Create Prefab: " + sAm + ".prefab...", fProgress);
        }

        EditorUtility.ClearProgressBar();
    }

    //static void ExportFun(bool AdorningAssetBundleBuild)
    //{
    //    if (Application.isPlaying || !Application.isEditor)
    //    {
    //        Debug.LogError("Export Adorning Operation can not run in runtime, please do it in editor mode");
    //    }

    //    string scenename = "adorning_" + Path.GetFileNameWithoutExtension(EditorApplication.currentScene);
    //    string absfolder = Application.dataPath + "/UnityScene/" + scenename;
    //    string folder = "Assets/UnityScene/" + scenename;
    //    if (Directory.Exists(absfolder) == false)
    //    {
    //        Directory.CreateDirectory(absfolder);
    //    }

    //    string[] scenegroupnames = AdorningManager.scenegroupnames;
    //    int nAm = scenegroupnames.Length;
    //    for (int iAm = 0; iAm < nAm; iAm++)
    //    {
    //        string sAm = scenegroupnames[iAm];
    //        GameObject goam = GameObject.Find(sAm);
    //        if (goam == null)
    //        {
    //            continue;
    //        }

    //        string prefabpath = folder + "/" + sAm + ".prefab";
    //        GameObject filego = PrefabUtility.CreatePrefab(prefabpath, goam);

    //        if (AdorningAssetBundleBuild)
    //        {
    //            string assetbundlepath = folder + "/" + sAm + ".unity3d";
    //            GameObject[] filegos = new GameObject[] { filego };

    //            if (BuildPipeline.BuildAssetBundle(filego,
    //                                                filegos,
    //                                                assetbundlepath,
    //                                                BuildAssetBundleOptions.CollectDependencies | BuildAssetBundleOptions.CompleteAssets,
    //                                                BuildTarget.StandaloneWindows))
    //            {
    //                Debug.Log("Generate Asset Bundle OK:" + assetbundlepath);
    //            }
    //            else
    //            {
    //                Debug.Log("Generate Asset Bundle Fail:" + assetbundlepath);
    //            }

    //            long filelength = 0;
    //            FileStream fs = File.Open(assetbundlepath, FileMode.Open, FileAccess.Read);
    //            filelength = fs.Length;
    //            fs.Close();
    //            fs.Dispose();

    //            int filelengthinmb= (int)(filelength / 1024 / 1024);
    //            if (filelengthinmb > 10)
    //            {
    //                byte[] data = File.ReadAllBytes(assetbundlepath);
    //                AssetBundle afterbundle = AssetBundle.CreateFromMemoryImmediate(data);
    //                Object[] objs = afterbundle.LoadAllAssets();

    //                int nDiv = filelengthinmb/3;
    //                int nNum = objs.Length / nDiv;
    //                int nStart = 0;
    //                for (int iDiv = 0; iDiv < nDiv; iDiv++)
    //                {
    //                    List<Object> objlist = new List<Object>();
    //                    if (nStart + nNum > objs.Length)
    //                    {
    //                        nNum = objs.Length - nStart;
    //                    }
    //                    for (int k = 0; k < nNum; k++)
    //                    {
    //                        Object obj = objs[nStart + k];
    //                        objlist.Add(obj);
    //                    }

    //                    Object[] objarray = objlist.ToArray();
    //                    string assetbundlepathex = folder + "/" + sAm + "_" + iDiv.ToString() + ".unity3d";

    //                    if (BuildPipeline.BuildAssetBundle(null,
    //                                                        objarray,
    //                                                        assetbundlepathex,
    //                                                        BuildAssetBundleOptions.CollectDependencies,
    //                                                        BuildTarget.StandaloneWindows))
    //                    {
    //                        Debug.Log("Generate Asset Bundle OK:" + assetbundlepathex);
    //                    }
    //                    else
    //                    {
    //                        Debug.Log("Generate Asset Bundle Fail:" + assetbundlepathex);
    //                    }

    //                    nStart += nNum;
    //                }

    //                File.Delete(assetbundlepath);
    //            }

    //        }

    //        float fProgress = (float)iAm / (float)nAm;
    //        EditorUtility.DisplayProgressBar("Export Adorning", "Create Prefab: " + sAm + ".prefab...", fProgress);
    //    }

    //    EditorUtility.ClearProgressBar();
    //}

    //[MenuItem("Tools/AdorningEx/Clear")]
    public static void Clear()
    {
        if (Application.isPlaying || !Application.isEditor)
        {
            Debug.LogError("Clear Adorning Operation can not run in runtime, please do it in editor mode");
        }

        List<GameObject> deletelist = new List<GameObject>();

        string[] scenegroupnames = AdorningManager.scenegroupnames;
        int nAm = scenegroupnames.Length;
        for (int iAm = 0; iAm < nAm; iAm++)
        {
            string sAm = scenegroupnames[iAm];
            GameObject goam = GameObject.Find(sAm);
            if (goam == null)
            {
                continue;
            }

            deletelist.Add(goam);
        }

        foreach (GameObject go in deletelist)
        {
            GameObject.DestroyImmediate(go);
        }

    }

}