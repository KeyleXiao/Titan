using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using UnityEditor;
using UnityEngine;

class UFileData
{
    public bool isSelected = false;
    public int depth = 0;//文件的深度
    public GUIContent content;
    public string assetPath;
    public ResConfigData configData;
    public int packageID = -1;
    public List<UFileData> childs = new List<UFileData>();

    public UFileData()
    {
        configData = new ResConfigData();
    }
}




[CustomEditor(typeof(UnityEditor.DefaultAsset))]
class UImageInformationEditor :Editor
{
    UFileData data;
    public void OnEnable()
    {

        if (target != null && Directory.Exists(AssetDatabase.GetAssetPath(target)))
        {
            string activeObjectPath = AssetDatabase.GetAssetPath(Selection.activeObject);
            //选中路径为 Assets/Artist/UI/Dynamic 路径下的才会有效，其余的目前没必要
            string DynamicPath = UPostprocessTexture.UIRootPath + UPostprocessTexture.PathConnectionSymbol + UPostprocessTexture.DynamicPath;
            if (!activeObjectPath.Contains(DynamicPath))
                return;

            data = new UFileData();
            UImageInformationEditorWindow.nEditorInstance.LoadFiles(data, activeObjectPath);

            UImageInformationEditorWindow.nEditorInstance.TreeSelectData = data;
            //GenrateConfigID(data);
        }
    }

    void GenrateConfigID(UFileData da)
    {
        if(da.childs.Count <= 0)
        {
            UImageInformationEditorWindow.nEditorInstance.InsetConfig(da);
            return;
        }
        foreach(UFileData d in da.childs)
        {
            GenrateConfigID(d);
            
        }
    }

    /// <summary>
    /// 根据当前路径获取文件夹显示信息（文件夹名，文件夹图标） 
    /// </summary>
    /// <param name="path"></param>
    /// <returns></returns>
    GUIContent GetGUIContent(string path)
    {
        UnityEngine.Object asset = AssetDatabase.LoadAssetAtPath(path, typeof(UnityEngine.Object));
        if (asset)
        {
            Texture tex = AssetDatabase.GetCachedIcon(path);
            //获取不到缓存图片的加载
            if(tex.name == string.Empty)
            {
                tex = AssetDatabase.LoadAssetAtPath<Texture>(path);
            }
            //Debug.Log("Path:" + path + ",Textrue:" + tex);
            return new GUIContent(asset.name, tex);
        }
        return null;
    }


}

class UImageInformationEditorWindow : EditorWindow
{
    [MenuItem("USpeedUI/UImageInformationEditorWindow")]
    public static void AddWindow()
    {
        //创建窗口
        nEditorInstance.Show();
        nEditorInstance.TreeSelectData = new UFileData();
        nEditorInstance.LoadFiles(nEditorInstance.TreeSelectData, nEditorInstance.DynamicSpritePath());
        //UGUIPerfabConfigEditor.nInstance = Selection.activeObject as UGUIPerfabConfig;
        //UGUIPerfabConfigEditor.nInstance.Init();

    }
    public static void CloseWindow()
    {
        nEditorInstance.Close();
        nEditorInstance = null;
    }

    private static UImageInformationEditorWindow _nEditorInstance = null;
    public static UImageInformationEditorWindow nEditorInstance
    {
        get
        {
            if (UImageInformationEditorWindow._nEditorInstance == null)
            {
                Rect wr = new Rect(0, 0, WindowRect.x, WindowRect.y);
                UImageInformationEditorWindow window = (UImageInformationEditorWindow)EditorWindow.GetWindowWithRect(typeof(UImageInformationEditorWindow), wr, true, "UI动态图片查看器");
                UImageInformationEditorWindow._nEditorInstance = window;
                UImageInformationEditorWindow._nEditorInstance.autoRepaintOnSceneChange = true;
            }

            return UImageInformationEditorWindow._nEditorInstance;
        }

        private set { }
    }

    string DynamicSpritePath()
    {
        return UPostprocessTexture.UIRootPath + UPostprocessTexture.PathConnectionSymbol + UPostprocessTexture.DynamicPath + UPostprocessTexture.PathConnectionSymbol;
    }

    public void LoadFiles(UFileData data, string currentPath, int depth = 0)
    {
        GUIContent content = GetGUIContent(currentPath);

        if (content != null)
        {
            data.depth = depth;
            data.content = content;
            string patch1 = currentPath.Replace("\\", "/");
            data.assetPath = patch1;
            if (patch1.Contains('.'))
            {
                ResConfigData config = AssetBundleImporter.MakeTextureAssetBundleFromReference(patch1, string.Empty);
                if(null == config)
                {
                    Debug.LogError("找不资源的,资源路径:" + patch1);
                }
                data.configData = config;
            }
        }
        //当前路径下所有文件信息
        foreach (var path in Directory.GetFiles(currentPath))
        {
            content = GetGUIContent(path);
            if (content != null)
            {
                UFileData child = new UFileData();
                child.depth = depth + 1;
                child.content = content;
                string patch1 = path.Replace("\\", "/");
                child.assetPath = patch1;
                if (patch1.Contains('.'))
                {
                    ResConfigData config = AssetBundleImporter.MakeTextureAssetBundleFromReference(patch1, string.Empty);
                    if (null == config)
                    {
                        Debug.LogError("找不资源的,资源路径:" + patch1);
                    }
                    child.configData = config;
                }
                data.childs.Add(child);
            }
        }

        //WillTodo:排序这里无法判断没ID的图片，先不用
        //data.childs.Sort((x, y) => 
        //{
        //    UFileData XData = x as UFileData;
        //    UFileData YData = y as UFileData;
        //    int idX = Convert.ToInt32(XData.assetPath.Substring(XData.assetPath.LastIndexOf('_') + 1, XData.assetPath.LastIndexOf('.') - XData.assetPath.LastIndexOf('_') - 1));
        //    int idY = Convert.ToInt32(YData.assetPath.Substring(YData.assetPath.LastIndexOf('_') + 1, YData.assetPath.LastIndexOf('.') - YData.assetPath.LastIndexOf('_') - 1));

        //    return idX.CompareTo(idY);

        //});

        foreach (var path in Directory.GetDirectories(currentPath))
        {
            UFileData childDir = new UFileData();
            data.childs.Add(childDir);
            LoadFiles(childDir, path, depth + 1);
        }
    }

    /// <summary>
    /// 根据当前路径获取文件夹显示信息（文件夹名，文件夹图标） 
    /// </summary>
    /// <param name="path"></param>
    /// <returns></returns>
    GUIContent GetGUIContent(string path)
    {
        UnityEngine.Object asset = AssetDatabase.LoadAssetAtPath(path, typeof(UnityEngine.Object));
        if (asset)
        {
            Texture tex = AssetDatabase.GetCachedIcon(path);
            //获取不到缓存图片的加载
            if (tex.name == string.Empty)
            {
                tex = AssetDatabase.LoadAssetAtPath<Texture>(path);
            }
            //Debug.Log("Path:" + path + ",Textrue:" + tex);
            return new GUIContent(asset.name, tex);
        }
        return null;
    }

    #region 属性
    //当前选择的树形区域数据
    public  UFileData TreeSelectData;
    //当前选择区域选中的数据
    private UFileData AreaSelectedData;
    /// <summary>
    /// 当前鼠标点击的数据
    /// </summary>
    private UFileData CurrentSelectedData;
    
    //树形滚动坐标
    private Vector2 scrollTreePosition;
    //选择图片区域滚动坐标
    private Vector2 scrollSelectedPosition;
    //窗口大小
    private static Vector2 WindowRect = new Vector2(1280,720);
    //各区域范围
    private Rect FolderTreeRect = new Rect();
    private Rect SelectedDataRect = new Rect();

    #region 资源属性

    private Rect ResInfoRect = new Rect();

    /// <summary>
    /// AB包名-UFileData
    /// </summary>
    private Dictionary<string, Dictionary<string, UFileData>> m_ConfigTable = new Dictionary<string, Dictionary<string, UFileData>>();

    /// <summary>
    /// 包名 - ID
    /// </summary>
    private Dictionary<string, int> m_Package_IDTable = new Dictionary<string, int>();
    private int currentPackageID = 0;
    #endregion

    #endregion

    public void OnEnable()
    {
        PostprocessUITextureToCSV.nEditorInstance.ImportConfig();
    }

    public void OnGUI()
    {
        DrawTreeData();
        DrawSelectedData();
        DrawResInfo();
    }

    public void OnDestroy()
    {
        Debug.Log("UImageInformationEditorWindow ResNode.DestroyRes....这里没做啥释放操作");
    }

    #region 绘制选择的文件层级区域
    void DrawTreeData()
    {
        if (TreeSelectData != null)
        {
            GUI.enabled = true;
            EditorGUIUtility.SetIconSize(Vector2.one * 16);
            FolderTreeRect.x = 10;
            FolderTreeRect.y = 10;
            FolderTreeRect.width = WindowRect.x * 0.2f;
            FolderTreeRect.height = WindowRect.y * 0.9f;
            GUILayout.BeginArea(FolderTreeRect);
            GUILayout.BeginVertical("box");
            scrollTreePosition = GUILayout.BeginScrollView(scrollTreePosition);
            DrawData(TreeSelectData);
            GUILayout.EndScrollView();
            GUILayout.EndVertical();
            GUILayout.EndArea();
        }

    }

    void DrawData(UFileData data)
    {
        if (data.content != null)
        {
            EditorGUI.indentLevel = data.depth;
            DrawGUIData(data);

        }
        for (int i = 0; i < data.childs.Count; i++)
        {
            UFileData child = data.childs[i];
            if (child.content != null)
            {
                EditorGUI.indentLevel = child.depth;
                if (child.childs.Count > 0)
                    DrawData(child);
                else
                    DrawGUIData(child);
            }
        }
    }

    void DrawGUIData(UFileData data)
    {
        GUIStyle style = "Label";

        Rect rt = GUILayoutUtility.GetRect(data.content, style);
        if (data.isSelected)
        {
            EditorGUI.DrawRect(rt, Color.gray);
        }

        rt.x += (16 * EditorGUI.indentLevel);
        if (GUI.Button(rt, data.content, style))
        {
            if (AreaSelectedData != null)
            {
                AreaSelectedData.isSelected = false;
            }
            data.isSelected = true;
            CurrentSelectedData = AreaSelectedData = data;
            Debug.Log(data.assetPath);
        }
    }
    #endregion

    #region 绘制选择的文件区域
    void DrawSelectedData()
    {
        SelectedDataRect.x = FolderTreeRect.x + FolderTreeRect.width+10;
        SelectedDataRect.y = 10;
        SelectedDataRect.width = WindowRect.x * 0.535f;
        SelectedDataRect.height = WindowRect.y * 0.9f;
        GUILayout.BeginArea(SelectedDataRect);
        GUILayout.BeginVertical("box");
        scrollSelectedPosition = GUILayout.BeginScrollView(scrollSelectedPosition);

        if (AreaSelectedData != null)
        {
            if (AreaSelectedData.content.image.name.Contains("Folder"))
            {
                DrawSelectedFolderAllFiles();
            }
            else
            {
                DrawSelectedFile();
            }          
        }

        GUILayout.EndScrollView();
        GUILayout.EndVertical();
        GUILayout.EndArea();
    }

    void DrawSelectedFolderAllFiles()
    {
        int index = 0;

        Vector2 IconSize = CalIconSize(true);

        int horiCound = (int)(SelectedDataRect.width / IconSize.x)-1;
        horiCound = horiCound == 0 ? 1 : horiCound;

        EditorGUIUtility.SetIconSize(IconSize);
        
        for (int i=0;i<AreaSelectedData.childs.Count;i+= horiCound)
        {
            GUILayout.BeginHorizontal();
            for (int j=0;j< horiCound; j++)
            {
                index = i + j;
                if ( (index < AreaSelectedData.childs.Count) && 
                    GUILayout.Button(AreaSelectedData.childs[index].content.image, GUILayout.Width(IconSize.x), GUILayout.Height(IconSize.y)))
                {
                    CurrentSelectedData = AreaSelectedData.childs[index];
                    Debug.Log("selectedData::" + AreaSelectedData.childs[index].assetPath);
                }
                    
            }
            GUILayout.EndHorizontal();
        }
    }
    void DrawSelectedFile()
    {
        Vector2 IconSize = CalIconSize(false);
        EditorGUIUtility.SetIconSize(IconSize);
        if (GUILayout.Button(AreaSelectedData.content.image, GUILayout.Width(IconSize.x), GUILayout.Height(IconSize.y)))
        {
            CurrentSelectedData = AreaSelectedData;
            Debug.Log("selectedData::" + AreaSelectedData.assetPath);
        }
            
    }
    Vector2 CalIconSize(bool bIsFolder)
    {
        Vector2 IconSize = new Vector2();
        int defaultSize = 50;
        if(bIsFolder)
        {
            if (AreaSelectedData.childs.Count > 0 && AreaSelectedData.childs[0].content.image.width > defaultSize*2+10)
            {
                IconSize.x = AreaSelectedData.childs[0].content.image.width * 0.3f;
                IconSize.y = AreaSelectedData.childs[0].content.image.height * 0.3f;
            }
            else
            {
                IconSize.x = defaultSize;
                IconSize.y = defaultSize;
            }
            
        }
        else
        {
            if (AreaSelectedData.content.image.width > defaultSize + 10)
            {
                IconSize.x = AreaSelectedData.content.image.width ;
                IconSize.y = AreaSelectedData.content.image.height;
                if(AreaSelectedData.content.image.width > 1000)
                {
                    IconSize.x = AreaSelectedData.content.image.width * 0.3f;
                    IconSize.y = AreaSelectedData.content.image.height * 0.3f;
                }
            }
            else
            {
                IconSize.x = defaultSize * 2;
                IconSize.y = defaultSize * 2;
            }
        }
        
        return IconSize;
    }
    #endregion



    #region 绘制资源信息

    void DrawResInfo()
    {
        if(null == CurrentSelectedData)
        {
            return;
        }
        ResInfoRect.x = SelectedDataRect.x + SelectedDataRect.width + 10;
        ResInfoRect.y = 10;
        ResInfoRect.width = WindowRect.x * 0.235f;
        ResInfoRect.height = WindowRect.y * 0.9f;
        GUILayout.BeginArea(ResInfoRect);
        GUILayout.BeginVertical("box");
        //EditorGUILayout.TextField("资源包ID:", CurrentSelectedData.packageID.ToString());
        EditorGUILayout.TextField("所属资源包:", CurrentSelectedData.configData.AssetBundleName);
        EditorGUILayout.TextField("资源名:", CurrentSelectedData.configData.AssetName);
        EditorGUILayout.TextField("资源GUID:", CurrentSelectedData.configData.AssetGUID);
        
        //if (GUILayout.Button("导入UI配置"))
        //{
        //    ImportConfig();
        //}
        if(GUILayout.Button("复制资源包名"))
        {
            EditorGUIUtility.systemCopyBuffer = CurrentSelectedData.configData.AssetBundleName;
        }
        if(GUILayout.Button("复制资源名"))
        {
            EditorGUIUtility.systemCopyBuffer = CurrentSelectedData.configData.AssetName;
        }

        //if (GUILayout.Button("导出UI配置"))
        //{
        //    ExportConfig();
        //}
        GUILayout.EndVertical();
        GUILayout.EndArea();
    }
    /*
    void ExportConfig()
    {
        string patch = AssetBundleManager.GetUIConfigPatch();
        string haderLine1 = "资源包ID,资源包名,资源名,资源GUID";
        string haderLine2 = "int,string,string,string";

        StreamWriter writer = new StreamWriter(patch, false, new System.Text.UTF8Encoding(true));
        writer.WriteLine(haderLine1);
        writer.WriteLine(haderLine2);

        foreach (Dictionary<string, UFileData> ta in m_ConfigTable.Values)
        {
            foreach (KeyValuePair<string, UFileData> data in ta)
            {
                if (string.IsNullOrEmpty(data.Value.assetBundleName))
                {
                    Debug.LogWarning("配置ID:" + data.Value.packageID + ",没有资源包，导出失败!");
                    continue;
                }
                string s = "";
                s += data.Value.packageID;
                s += ",";
                s += data.Value.assetBundleName;
                s += ",";
                s += data.Value.assetName;
                s += ",";
                s += data.Value.assetGUID;
                writer.WriteLine(s);
            }
        }

        writer.Flush();
        writer.Close();
    }

    void ImportConfig()
    {
        string patch = AssetBundleManager.GetUIConfigPatch();
        if (!File.Exists(patch))
        {
            return;
        }
        m_ConfigTable.Clear();
        m_Package_IDTable.Clear();
        string stSchemeAllText = System.IO.File.ReadAllText(patch);

        //去掉\n,因为ScpReader以"\r"分割
        stSchemeAllText = stSchemeAllText.Replace("\n", "");
        //再干掉最后的"\r"
        stSchemeAllText = stSchemeAllText.Remove(stSchemeAllText.Length - 1, 1);
        ScpReader UIReader = new ScpReader(stSchemeAllText, "UIConfig", 2);

        // 遍歷整個表并存儲起來
        for (int i = 0; i < UIReader.GetRecordCount(); ++i)
        {
            UFileData data = new UFileData();
            //data.packageID = UIReader.GetInt(i, (int)AssetBundleManager.UIConfigCol.COL_ID, -1);
            currentPackageID = data.packageID;
            data.assetBundleName = UIReader.GetString(i, (int)AssetBundleManager.UIConfigCol.COL_AssetBundleName, "");
            data.assetName = UIReader.GetString(i, (int)AssetBundleManager.UIConfigCol.COL_AssetName, "");
            data.assetGUID = UIReader.GetString(i, (int)AssetBundleManager.UIConfigCol.COL_AssetGUID, "");

            //ID与包名对应表
            if (!m_Package_IDTable.ContainsKey(data.assetBundleName))
            {
                m_Package_IDTable.Add(data.assetBundleName, data.packageID);
            }


            //包名已经存在了
            if (m_ConfigTable.ContainsKey(data.assetBundleName))
            {
                Dictionary<string, UFileData> ta = m_ConfigTable[data.assetBundleName];
                if (ta.ContainsKey(data.assetName))
                {
                    Debug.LogError("重复的资源名:" + data.assetName + ",包:" + data.assetBundleName);
                    continue;
                }
                ta.Add(data.assetName, data);
            }
            else
            {
                Dictionary<string, UFileData> ta = new Dictionary<string, UFileData>();
                ta.Add(data.assetName, data);
                m_ConfigTable.Add(data.assetBundleName, ta);
            }
        }
        UIReader.Dispose();
        UIReader = null;
    }
    */
    int GetID(string assetBundleName)
    {
        if (assetBundleName.Equals(string.Empty))
        {
            return -1;
        }
        if (m_Package_IDTable.ContainsKey(assetBundleName))
        {
            return m_Package_IDTable[assetBundleName];
        }
        currentPackageID++;
        return currentPackageID;
    }

    public void InsetConfig(UFileData data)
    {
        if (!data.configData.AssetBundleName.Equals(string.Empty))
        {
            data.packageID = GetID(data.configData.AssetBundleName);
            //包名已经存在了
            if (m_ConfigTable.ContainsKey(data.configData.AssetBundleName))
            {
                Dictionary<string, UFileData> ta = m_ConfigTable[data.configData.AssetBundleName];
                if (ta.ContainsKey(data.configData.AssetName))
                {
                    return;
                }
                ta.Add(data.configData.AssetName, data);
            }
            else
            {
                Dictionary<string, UFileData> ta = new Dictionary<string, UFileData>();
                ta.Add(data.configData.AssetName, data);
                m_ConfigTable.Add(data.configData.AssetBundleName, ta);
            }
        }
    }
    #endregion
}

/// <summary>
/// 导入UI贴图时把贴图的信息保存到配置表
/// </summary>
class PostprocessUITextureToCSV //: ScriptableObject
{

    private static PostprocessUITextureToCSV _nInstance = null;
    public static PostprocessUITextureToCSV nEditorInstance
    {
        get
        {
            if (PostprocessUITextureToCSV._nInstance == null)
            {
                PostprocessUITextureToCSV._nInstance = new PostprocessUITextureToCSV();//ScriptableObjectUtility.CreateAsset<PostprocessUITextureToCSV>(typeof(PostprocessUITextureToCSV).ToString());
                PostprocessUITextureToCSV._nInstance.ImportConfig();
            }

            return PostprocessUITextureToCSV._nInstance;
        }

        private set { }
    }

    private Dictionary<string, Dictionary<string, UFileData>> m_ConfigTable = new Dictionary<string, Dictionary<string, UFileData>>();

    public void InsetConfig(UFileData data)
    {
        if (!data.configData.AssetBundleName.Equals(string.Empty))
        {
            //包名已经存在了
            if (m_ConfigTable.ContainsKey(data.configData.AssetBundleName))
            {
                Dictionary<string, UFileData> ta = m_ConfigTable[data.configData.AssetBundleName];
                if (ta.ContainsKey(data.configData.AssetName))
                {
                    ta[data.configData.AssetName] = data;
                    return;
                }
                ta.Add(data.configData.AssetName, data);
            }
            else
            {
                Dictionary<string, UFileData> ta = new Dictionary<string, UFileData>();
                ta.Add(data.configData.AssetName, data);
                m_ConfigTable.Add(data.configData.AssetBundleName, ta);
            }
        }

        //EditorUtility.SetDirty(this);

    }
    public void RemoveConfig(string strRemoveGUID)
    {
        string strRemoveFileKey = string.Empty,strRemoveConfigKey = string.Empty;
        bool bFindData = false;

        foreach(var configData in m_ConfigTable)
        {
            foreach(var fileData in configData.Value)
            {
                if (fileData.Value.configData.AssetGUID == strRemoveGUID)
                {
                    bFindData = true;
                    strRemoveConfigKey = configData.Key;
                    strRemoveFileKey = fileData.Key;
                    break;
                }
            }
            if (bFindData)
                break;
        }
        if(bFindData)
        {
            m_ConfigTable[strRemoveConfigKey].Remove(strRemoveFileKey);
        }


    }
    public static string CombinAssetBundleName(string assetBundleName,string assetBundleVariant)
    {
        return assetBundleName + "." + assetBundleVariant;
    }
    public static string GetAssetName(string assetPath)
    {
        return Path.GetFileNameWithoutExtension(assetPath);
    }

    public static string GetGUID(string assetPath)
    {
        string ablotePatch = Application.dataPath;
        ablotePatch = ablotePatch.Replace("Assets", "");
        ablotePatch += assetPath;
        return AssetBundleManagerEditor.GetAssetGUIDFromMeta(ablotePatch);
    }

    public void UIAssetImport()
    {
        string DynamicUIPath = Application.dataPath + AssetBundleImporter.UIDynamicAssetPatch.Remove(0,AssetBundleImporter.UIDynamicAssetPatch.IndexOf('/'));
        
        if (Directory.Exists(DynamicUIPath))
        {
            DirectoryInfo fromDirInfo = new DirectoryInfo(DynamicUIPath);
            FileInfo[] allPngFiles = fromDirInfo.GetFiles("*.png", SearchOption.AllDirectories);

            m_ConfigTable.Clear();

            string tmpPath = string.Empty;
            
            foreach (var files in allPngFiles)
            {
                tmpPath = files.FullName.Remove(0, files.FullName.IndexOf("Assets"));
                UFileData data = new UFileData();
                data.configData = AssetBundleImporter.MakeTextureAssetBundleFromReference(tmpPath, string.Empty);
                if (null != data.configData)
                {
                    PostprocessUITextureToCSV.nEditorInstance.InsetConfig(data);
                }
                else
                {
                    Debug.LogError("取得UI资源失败:" + tmpPath);
                    continue;
                }
            }

            PostprocessUITextureToCSV.nEditorInstance.ExportConfig();
        }
    }

    public void ImportConfig()
    {
        ImportBin();
        //ImportCSV();
    }

    public void ExportConfig()
    {
        //ExportCSV();
        ExportBin();
    }

    private void ImportCSV()
    {
        string patch = AssetBundleManager.GetUICSVConfigPatch();
        if (!File.Exists(patch))
        {
            return;
        }
        m_ConfigTable.Clear();
        string stSchemeAllText = System.IO.File.ReadAllText(patch);

        //去掉\n,因为ScpReader以"\r"分割
        stSchemeAllText = stSchemeAllText.Replace("\n", "");
        //再干掉最后的"\r"
        stSchemeAllText = stSchemeAllText.Remove(stSchemeAllText.Length - 1, 1);
        ScpReader UIReader = new ScpReader(stSchemeAllText, "UIConfig", 2);

        // 遍歷整個表并存儲起來
        for (int i = 0; i < UIReader.GetRecordCount(); ++i)
        {
            UFileData data = new UFileData();
            //data.packageID = UIReader.GetInt(i, (int)AssetBundleManager.UIConfigCol.COL_ID, -1);
            data.configData.AssetBundleName = UIReader.GetString(i, (int)AssetBundleManager.AssetConfigCol.COL_AssetBundleName, "");
            data.configData.AssetName = UIReader.GetString(i, (int)AssetBundleManager.AssetConfigCol.COL_AssetName, "");
            data.configData.AssetBundleName = UIReader.GetString(i, (int)AssetBundleManager.AssetConfigCol.COL_AssetGUID, "");
            data.configData.AssetSize_X = UIReader.GetFloat(i, (int)AssetBundleManager.AssetConfigCol.COL_AssetSize_X, -1);
            data.configData.AssetSize_Y = UIReader.GetFloat(i, (int)AssetBundleManager.AssetConfigCol.COL_AssetSize_Y, -1);
            //包名已经存在了
            if (m_ConfigTable.ContainsKey(data.configData.AssetBundleName))
            {
                Dictionary<string, UFileData> ta = m_ConfigTable[data.configData.AssetBundleName];
                if (ta.ContainsKey(data.configData.AssetName))
                {
                    Debug.LogError("重复的资源名:" + data.configData.AssetName + ",包:" + data.configData.AssetBundleName);
                    continue;
                }
                ta.Add(data.configData.AssetName, data);
            }
            else
            {
                Dictionary<string, UFileData> ta = new Dictionary<string, UFileData>();
                ta.Add(data.configData.AssetName, data);
                m_ConfigTable.Add(data.configData.AssetBundleName, ta);
            }
        }
        UIReader.Dispose();
        UIReader = null;
    }

    private void ImportBin()
    {
        string patch = AssetBundleManager.GetUIBinConfigPatch();
        if (!File.Exists(patch))
        {
            return;
        }

        m_ConfigTable.Clear();
        FileStream _readObj = null;
        _readObj = new FileStream(patch, FileMode.Open, FileAccess.Read);

        int cout = 0;
        AssetBundleManager.ReadStruct(ref cout, _readObj);

        if (cout <= 0)
        {
            return;
        }

        // 遍歷整個表并存儲起來
        for (int i = 0; i < cout; ++i)
        {
            UFileData data = new UFileData();

            data.configData.AssetBundleName = AssetBundleManager.ReadString(_readObj);
            data.configData.AssetName = AssetBundleManager.ReadString(_readObj);
            data.configData.AssetGUID = AssetBundleManager.ReadString(_readObj);
            data.configData.AssetSize_X = float.TryParse(AssetBundleManager.ReadString(_readObj), out data.configData.AssetSize_X) ? data.configData.AssetSize_X : -1;
            data.configData.AssetSize_Y = float.TryParse(AssetBundleManager.ReadString(_readObj), out data.configData.AssetSize_Y) ? data.configData.AssetSize_Y : -1;

            //包名已经存在了
            if (m_ConfigTable.ContainsKey(data.configData.AssetBundleName))
            {
                Dictionary<string, UFileData> ta = m_ConfigTable[data.configData.AssetBundleName];
                if (ta.ContainsKey(data.configData.AssetName))
                {
                    Debug.LogError("重复的资源名:" + data.configData.AssetName + ",包:" + data.configData.AssetBundleName);
                    continue;
                }
                ta.Add(data.configData.AssetName, data);
            }
            else
            {
                Dictionary<string, UFileData> ta = new Dictionary<string, UFileData>();
                ta.Add(data.configData.AssetName, data);
                m_ConfigTable.Add(data.configData.AssetBundleName, ta);
            }
        }

        _readObj.Flush();
        _readObj.Close();
        _readObj = null;
    }
    [MenuItem("USpeedUI/ExportCSVUITexture")]
    public static void ExportUICSV()
    {
        nEditorInstance.ExportCSV();
    }
    public void ExportCSV()
    {
        string patch = AssetBundleManager.GetUICSVConfigPatch();
        string haderLine1 = "资源包名,资源名,资源GUID";
        string haderLine2 = "string,string,string";

        StreamWriter writer = new StreamWriter(patch, false, new System.Text.UTF8Encoding(true));
        writer.WriteLine(haderLine1);
        writer.WriteLine(haderLine2);

        foreach (Dictionary<string, UFileData> ta in m_ConfigTable.Values)
        {
            foreach (KeyValuePair<string, UFileData> data in ta)
            {
                if (string.IsNullOrEmpty(data.Value.configData.AssetBundleName))
                {
                    Debug.LogWarning("配置ID:" + data.Value.packageID + ",没有资源包，导出失败!");
                    continue;
                }
                string s = "";
                //s += data.Value.packageID;
                //s += ",";
                s += data.Value.configData.AssetBundleName;
                s += ",";

                s += data.Value.configData.AssetName;
                s += ",";

                s += data.Value.configData.AssetGUID;
                s += ",";

                s += data.Value.configData.AssetSize_X.ToString("F3");
                s += ",";

                s += data.Value.configData.AssetSize_Y.ToString("F3");
                writer.WriteLine(s);
            }
        }

        writer.Flush();
        writer.Close();
    }

    private void ExportBin()
    {
        string patch = AssetBundleManager.GetUIBinConfigPatch();
        
        int cout = m_ConfigTable.Count;
        if(cout <= 0)
        {
            return;
        }
        FileStream _writerObj = null;
        _writerObj = new FileStream(patch, FileMode.Create, FileAccess.Write);

        //有多少条配置
        cout = 0;
        foreach (Dictionary<string, UFileData> ta in m_ConfigTable.Values)
        {
            cout += ta.Count;
        }
        //先写入数目
        AssetBundleManager.WriteStruct(cout, _writerObj);

        foreach (Dictionary<string, UFileData> ta in m_ConfigTable.Values)
        {
            foreach (KeyValuePair<string, UFileData> data in ta)
            {
                if (string.IsNullOrEmpty(data.Value.configData.AssetBundleName))
                {
                    Debug.LogWarning("配置ID:" + data.Value.packageID + ",没有资源包，导出失败!");
                    continue;
                }
                AssetBundleManager.WriteString(data.Value.configData.AssetBundleName, _writerObj);
                AssetBundleManager.WriteString(data.Value.configData.AssetName, _writerObj);
                AssetBundleManager.WriteString(data.Value.configData.AssetGUID, _writerObj);
                AssetBundleManager.WriteString(data.Value.configData.AssetSize_X.ToString("F3"), _writerObj);
                AssetBundleManager.WriteString(data.Value.configData.AssetSize_Y.ToString("F3"), _writerObj);
            }
        }
        _writerObj.Flush();
        _writerObj.Close();
        _writerObj = null;
    }


}