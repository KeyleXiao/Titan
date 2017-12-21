using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using System.IO;

public class ResourceExEditor
{
    //static void ProgressImp(string msg, int progress)
    //{
    //    EditorUtility.DisplayProgressBar("GenerateResourceTable", msg, progress / 100.0f);
    //}

    //[MenuItem("Resource/Export List")]
    //static void ExportResourceTable()
    //{
    //    string[] allpaths = AssetDatabase.GetAllAssetPaths();
    //    Dictionary<string, List<string>> pathmap = new Dictionary<string, List<string>>();
    //    foreach (string path in allpaths)
    //    { 
    //        string assetname=Path.GetFileNameWithoutExtension(path);
    //        if (pathmap.ContainsKey(assetname) == false)
    //        {
    //            pathmap[assetname] = new List<string>();
    //        }
    //        pathmap[assetname].Add(path);
    //    }

    //    AssetEx.AssetFileEx.ProgressEvent -= ProgressImp;
    //    AssetEx.AssetFileEx.ProgressEvent += ProgressImp;
    //    ResourceEx.GenerateResourceTable();
    //    EditorUtility.ClearProgressBar();

    //    Dictionary<string, ResourceInfoEx> textable = ResourceEx.resourcetable["Texture2D"];
    //    foreach (KeyValuePair<string, ResourceInfoEx> pair in textable)
    //    {
    //        string objectname = pair.Key;
    //        ResourceInfoEx infoex = pair.Value;
    //        if (pathmap.ContainsKey(objectname))
    //        {
    //            List<string> pathlist = pathmap[objectname];
    //            foreach (string path in pathlist)
    //            {
    //                string ext = Path.GetExtension(path);
    //                if (ext!=null)ext=ext.ToLower();
    //                if (ext == ".jpg" || ext == ".png" || ext == ".tga" || ext == ".bmp" || ext == ".dds")
    //                {
    //                    infoex.path = path;
    //                    infoex.guid = AssetDatabase.AssetPathToGUID(path);
    //                }
    //            }
    //        }
    //    }


    //    string resourcelistpath = "D:\\project\\speedex\\Bin\\Client\\Bin\\test_Data\\resourcelist.txt";
    //    StreamWriter sw = new StreamWriter(resourcelistpath, false);
    //    sw.WriteLine("objectname,assetfilename,classname,typename,path,guid");
    //    sw.WriteLine("string,string,string,string,string,string");
    //    foreach (KeyValuePair<string, ResourceInfoEx> pair in textable)
    //    {
    //        string objectname = pair.Key;
    //        ResourceInfoEx infoex = pair.Value;
    //        sw.WriteLine(infoex.objectname + "," 
    //            + infoex.assetfilename + ","
    //            + infoex.classname + ","
    //            + infoex.typename + ","
    //            + infoex.path + ","
    //            + infoex.guid);
    //    }
    //    sw.Close();
    //    sw.Dispose();

    //    Debug.Log(allpaths.Length);
    //}
}
