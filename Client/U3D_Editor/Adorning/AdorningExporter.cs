using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using System.IO;



public class AdorningExporter : Editor
{

   // [MenuItem("Tools/Adorning/Build")]
    static void Build()
    {
        AdorningManager.Init();
    }

    static Transform GetSubObjectFromPrefab(Transform tParent, string name)
    {
        Transform tFind = tParent.FindChild(name);
        if (tFind != null)
        {
            return tFind;
        }

        int n = tParent.childCount;
        for (int i = 0; i < n; i++)
        {
            Transform tChild = tParent.GetChild(i);
            if (tChild == null)
            {
                continue;
            }

            Transform tRet = GetSubObjectFromPrefab(tChild, name);
            if (tRet != null)
            {
                return tRet;
            }
        }

        return null;
    }


    //[MenuItem("Tools/Adorning/Import From Bundle")]
    static void ImportFromBundle()
    {
        Clear();
        ImportFun(true, BuildTarget.StandaloneWindows);
    }

    //[MenuItem("Tools/Adorning/Import From Prefab")]
    static void ImportFromPrefab()
    {
        Clear();
        ImportFun(false, BuildTarget.StandaloneWindows);
    }

    static void ImportFun(bool AdorningAssetBundleBuild, BuildTarget platform)
    {
        if (Application.isPlaying || !Application.isEditor)
        {
            Debug.LogError("Import Adorning Operation can not run in runtime, please do it in editor mode");
        }

        string scenename = "adorning_" + Path.GetFileNameWithoutExtension(UnityEditor.SceneManagement.EditorSceneManager.GetActiveScene().name);
        string absfolder = Application.dataPath + "/UnityScene/" + scenename;
        string folder = "Assets/UnityScene/" + scenename;
        if (Directory.Exists(absfolder) == false)
        {
            Directory.CreateDirectory(absfolder);
        }

   

        string[] abssubfolders = Directory.GetDirectories(absfolder);
        foreach (string abssubfolder in abssubfolders)
        {
            string amname=Path.GetFileName(abssubfolder);
            string subfolder = folder+"/"+ amname;

            GameObject goam = GameObject.Find(amname);
            if (goam == null)
            {
                continue; 
            }

            AdorningManager am = goam.GetComponent<AdorningManager>();
            if (am == null)
            {
                continue;
            }

            if (AdorningAssetBundleBuild)
            {
                string[] absfiles = Directory.GetFiles(abssubfolder, "*.unity3d");
                int nGroup = absfiles.Length;
                for (int iGroup = 0; iGroup < nGroup; iGroup++)
                {
                    string absfile = absfiles[iGroup];
                    string file = subfolder + "/" + Path.GetFileName(absfile);

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

                    Vector3 pos = groupgo.transform.localPosition;
                    Vector3 angle = groupgo.transform.localEulerAngles;
                    Vector3 scale = groupgo.transform.localScale;

                    groupgo.transform.parent = goam.transform;

                    groupgo.transform.localPosition = pos;
                    groupgo.transform.localEulerAngles = angle;
                    groupgo.transform.localScale = scale;

                    prefab = null;
                    assetbundle.Unload(false);

                    float fProgress = (float)iGroup / (float)nGroup;
                    EditorUtility.DisplayProgressBar("Import Adorning: " + amname, "Import AssetBundle: " + groupgo.name + ".unity3d...", fProgress);
                }
            }
            else
            {
                string[] absfiles = Directory.GetFiles(abssubfolder, "*.prefab");
                int nGroup = absfiles.Length;
                for (int iGroup = 0; iGroup < nGroup; iGroup++)
                {
                    string absfile = absfiles[iGroup];
                    string file = subfolder + "/" + Path.GetFileName(absfile);

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

                    Vector3 pos = groupgo.transform.localPosition;
                    Vector3 angle = groupgo.transform.localEulerAngles;
                    Vector3 scale = groupgo.transform.localScale;

                    groupgo.transform.parent = goam.transform;

                    groupgo.transform.localPosition = pos;
                    groupgo.transform.localEulerAngles = angle;
                    groupgo.transform.localScale = scale;

                    float fProgress = (float)iGroup / (float)nGroup;
                    EditorUtility.DisplayProgressBar("Import Adorning: " + amname, "Import Prefab: " + groupgo.name + ".prefab...", fProgress);
                }        
            }
        }

        EditorUtility.ClearProgressBar();
  
    }

   // [MenuItem("Tools/Adorning/Export To Bundle")]
    static void ExportToBundle()
    {
        ExportFun(true, BuildTarget.StandaloneWindows);
    }

   // [MenuItem("Tools/Adorning/Export To Prefab")]
    static void ExportToPrefab()
    {
        ExportFun(false, BuildTarget.StandaloneWindows);
    }

    static void ExportFun(bool AdorningAssetBundleBuild, BuildTarget platform)
    {
        if (Application.isPlaying || !Application.isEditor)
        {
            Debug.LogError("Export Adorning Operation can not run in runtime, please do it in editor mode");
        }

        string scenename = "adorning_" + Path.GetFileNameWithoutExtension(UnityEditor.SceneManagement.EditorSceneManager.GetActiveScene().name);
        string absfolder = Application.dataPath + "/UnityScene/" + scenename;
        string folder = "Assets/UnityScene/" + scenename;
        if (Directory.Exists(absfolder) == false)
        {
            Directory.CreateDirectory(absfolder);
        }



        AdorningManager[] ams = GameObject.FindObjectsOfType<AdorningManager>();
        foreach (AdorningManager am in ams)
        {
            Transform tManager=am.gameObject.transform;

            string abssubfolder = absfolder + "/" + tManager.name;
            string subfolder = folder + "/" + tManager.name;
            if (Directory.Exists(abssubfolder) == false)
            {
                Directory.CreateDirectory(abssubfolder);
            }

            int nGroup = tManager.childCount;
            for (int iGroup = 0; iGroup < nGroup; iGroup++)
            {
                Transform tGroup = am.gameObject.transform.GetChild(iGroup);
 
                string prefabpath = subfolder + "/" + tGroup.name + ".prefab";
                GameObject filego=PrefabUtility.CreatePrefab(prefabpath, tGroup.gameObject);

                if (AdorningAssetBundleBuild)
                {
                    string assetbundlepath = subfolder + "/" + tGroup.name + ".unity3d";
                    GameObject[] filegos = new GameObject[] { filego };

                    if (BuildPipeline.BuildAssetBundle(filego,
                                                        filegos,
                                                        assetbundlepath,
                                                        BuildAssetBundleOptions.CollectDependencies | BuildAssetBundleOptions.CompleteAssets,
                                                        platform))
                    {
                        Debug.Log("Generate Asset Bundle OK:" + assetbundlepath);
                    }
                    else
                    {
                        Debug.Log("Generate Asset Bundle Fail:" + assetbundlepath);
                    }

                    File.Delete(prefabpath);
                }

                float fProgress = (float)iGroup / (float)nGroup;
                EditorUtility.DisplayProgressBar("Export Adorning: " + tManager.name, "Create Prefab: " + tGroup.name + ".prefab...", fProgress);
            }

        }


        EditorUtility.ClearProgressBar();
    }

   // [MenuItem("Tools/Adorning/Clear")]
    static void Clear()
    {
        if (Application.isPlaying || !Application.isEditor)
        {
            Debug.LogError("Clear Adorning Operation can not run in runtime, please do it in editor mode");
        }

        
        

        List<GameObject> deletelist = new List<GameObject>();

        AdorningManager[] ams = GameObject.FindObjectsOfType<AdorningManager>();
        foreach (AdorningManager am in ams)
        {
            Transform tManager = am.gameObject.transform;

            int nGroup = tManager.childCount;
            for (int iGroup = 0; iGroup < nGroup; iGroup++)
            {
                Transform tGroup = am.gameObject.transform.GetChild(iGroup);

                deletelist.Add(tGroup.gameObject);
           }
        }

        foreach (GameObject go in deletelist)
        {
            GameObject.DestroyImmediate(go);
        }

    }


    //[MenuItem("Tools/Adorning/ImportOld")]
    //static void ImportOld()
    //{
    //    if (Application.isPlaying || !Application.isEditor)
    //    {
    //        Debug.LogError("Import Adorning Operation can not run in runtime, please do it in editor mode");
    //    }

    //    string folder = Application.dataPath + "/UnityScene/" + Path.GetFileNameWithoutExtension(EditorApplication.currentScene);
    //    if (Directory.Exists(folder) == false)
    //    {
    //        Directory.CreateDirectory(folder);
    //    }

    //    string csvpath = folder + "/" + Application.loadedLevel.ToString() + ".csv";
    //    string text = System.IO.File.ReadAllText(csvpath);
    //    ScpReader sr = new ScpReader(text, csvpath);

    //    for (int i = 0; i < sr.GetRecordCount(); i++)
    //    {
    //        string command = sr.GetString(i, 0, "");
    //        if (command == "adorning")
    //        {
    //            string sManager = sr.GetString(i, 1, "");
    //            GameObject gManager = GameObject.Find(sManager);
    //            if (gManager == null)
    //            {
    //                continue;
    //            }
    //            AdorningManager am = gManager.GetComponent<AdorningManager>();
    //            if (am == null)
    //            {
    //                continue;
    //            }

    //            string sGroup = sr.GetString(i, 2, "");
    //            Transform tGroup = gManager.transform.FindChild(sGroup);
    //            if (tGroup == null)
    //            {
    //                continue;
    //            }
    //            AdorningGroup ag = tGroup.GetComponent<AdorningGroup>();
    //            if (ag == null)
    //            {
    //                continue;
    //            }

    //            string sName = sr.GetString(i, 3, "");
    //            string sPath = sr.GetString(i, 6, "");

    //            if (sPath.Length <= 0)
    //            {
    //                continue;
    //            }

    //            GameObject prefabparent = AssetDatabase.LoadAssetAtPath<GameObject>(sPath);
    //            if (prefabparent == null)
    //            {
    //                continue;
    //            }

    //            Transform tprefabsub = GetSubObjectFromPrefab(prefabparent.transform, sName);
    //            if (tprefabsub == null)
    //            {
    //                continue;
    //            }
    //            GameObject prefabsub = tprefabsub.gameObject;

    //            Vector3 pos;
    //            pos.x = sr.GetFloat(i, 7, 0.0f);
    //            pos.y = sr.GetFloat(i, 8, 0.0f);
    //            pos.z = sr.GetFloat(i, 9, 0.0f);
    //            Vector3 angle;
    //            angle.x = sr.GetFloat(i, 10, 0.0f);
    //            angle.y = sr.GetFloat(i, 11, 0.0f);
    //            angle.z = sr.GetFloat(i, 12, 0.0f);
    //            Vector3 scale;
    //            scale.x = sr.GetFloat(i, 13, 0.0f);
    //            scale.y = sr.GetFloat(i, 14, 0.0f);
    //            scale.z = sr.GetFloat(i, 15, 0.0f);

    //            //GameObject go = (GameObject)PrefabUtility.InstantiatePrefab(prefabsub);
    //            GameObject go = (GameObject)GameObject.Instantiate(prefabsub);
    //            go.name = sName;
    //            go.transform.parent = tGroup;
    //            go.transform.position = pos;
    //            go.transform.eulerAngles = angle;
    //            go.transform.localScale = scale;

    //        }
    //    }
    //    sr.Dispose();
    //}

    //[MenuItem("Tools/Adorning/ExportOld")]
    //static void ExportOld()
    //{
    //    if (Application.isPlaying || !Application.isEditor)
    //    {
    //        Debug.LogError("Export Adorning Operation can not run in runtime, please do it in editor mode");
    //    }

    //    string folder = Application.dataPath + "/UnityScene/" + Path.GetFileNameWithoutExtension(EditorApplication.currentScene);
    //    if (Directory.Exists(folder) == false)
    //    {
    //        Directory.CreateDirectory(folder);
    //    }

    //    string spath = folder + "/" + Application.loadedLevel.ToString() + ".csv";
    //    StreamWriter sw = new StreamWriter(spath, false);
    //    sw.WriteLine("#,manager,group,name,typeindex,typename,path,posx,posy,posz,anglex,angley,anglez,scalex,scaley,scalez");

    //    List<GameObject> deletelist = new List<GameObject>();

    //    AdorningManager[] ams = GameObject.FindObjectsOfType<AdorningManager>();
    //    foreach (AdorningManager am in ams)
    //    {
    //        Transform tManager = am.gameObject.transform;

    //        int nGroup = tManager.childCount;
    //        for (int iGroup = 0; iGroup < nGroup; iGroup++)
    //        {
    //            Transform tGroup = am.gameObject.transform.GetChild(iGroup);
    //            AdorningGroup ag = tGroup.GetComponent<AdorningGroup>();
    //            if (ag == null)
    //            {
    //                continue;
    //            }

    //            int nAdorning = tGroup.childCount;
    //            for (int iAdorning = 0; iAdorning < nAdorning; iAdorning++)
    //            {
    //                Transform tAdorning = tGroup.GetChild(iAdorning);
    //                Adorning aa = tAdorning.GetComponent<Adorning>();
    //                if (aa == null)
    //                {
    //                    continue;
    //                }
    //                GameObject go = aa.gameObject;
    //                PrefabType ptype = PrefabUtility.GetPrefabType(go);
    //                int ntype = (int)ptype;

    //                string opath = "";

    //                GameObject gs = (GameObject)PrefabUtility.GetPrefabParent(go);
    //                if (gs != null)
    //                {
    //                    opath = AssetDatabase.GetAssetPath(gs);
    //                }

    //                if (opath.Length > 0)
    //                {
    //                    string sline = "adorning" + ",";
    //                    sline += am.name + ",";
    //                    sline += ag.name + ",";
    //                    sline += aa.name + ",";
    //                    sline += ntype.ToString() + ",";
    //                    sline += ptype.ToString() + ",";
    //                    sline += opath + ",";
    //                    sline += aa.transform.position.x.ToString() + ",";
    //                    sline += aa.transform.position.y.ToString() + ",";
    //                    sline += aa.transform.position.z.ToString() + ",";
    //                    sline += aa.transform.eulerAngles.x.ToString() + ",";
    //                    sline += aa.transform.eulerAngles.y.ToString() + ",";
    //                    sline += aa.transform.eulerAngles.z.ToString() + ",";
    //                    sline += aa.transform.localScale.x.ToString() + ",";
    //                    sline += aa.transform.localScale.y.ToString() + ",";
    //                    sline += aa.transform.localScale.z.ToString() + ",";

    //                    sw.WriteLine(sline);
    //                    sw.Flush();

    //                    deletelist.Add(go);
    //                }
    //            }
    //        }
    //    }

    //    foreach (GameObject go in deletelist)
    //    {
    //        GameObject.DestroyImmediate(go);
    //    }

    //    sw.Close();

    //    EditorUtility.ClearProgressBar();
    //}
}