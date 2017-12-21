using UnityEngine;
using UnityEditor;

/// <summary>
/// 组件辅助类
/// </summary>
public class ComponentUtily
{
    [MenuItem("Tools/Fix Unity Dlls")]
    public static void ReimportUI()
    {
        var path = EditorApplication.applicationContentsPath + "/UnityExtensions";
        //var path = EditorApplication.applicationContentsPath + "/UnityExtensions/Unity/GUISystem/{1}";
        //var version = string.Empty;
        //string engineDll = string.Format(path, version, "UnityEngine.UI.dll");
        //string editorDll = string.Format(path, version, "Editor/UnityEditor.UI.dll");
        System.Collections.Generic.List<string> dllPatch = new System.Collections.Generic.List<string>();
        dllPatch.AddRange(System.IO.Directory.GetFiles(path, "*.dll", System.IO.SearchOption.AllDirectories));
        dllPatch.AddRange(System.IO.Directory.GetFiles(path, "*.dll", System.IO.SearchOption.TopDirectoryOnly));
        foreach(string engineDll in dllPatch)
        {
            ReimportDll(engineDll);
        }
        
        //ReimportDll(editorDll);

    }
    static void ReimportDll(string path)
    {
        if (System.IO.File.Exists(path))
            AssetDatabase.ImportAsset(path, ImportAssetOptions.ForceUpdate | ImportAssetOptions.DontDownloadFromCacheServer);
        else
            Debug.LogError(string.Format("DLL not found {0}", path));
    }
}