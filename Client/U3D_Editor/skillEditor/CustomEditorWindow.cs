using UnityEngine;
using UnityEditor;
using System.IO;
using System.Reflection;
using System.Collections.Generic;

/**
 * 自定义数据类型编辑窗口（模板）(具体使用方法参考SkillEditor.cs)
 *
 * 创建一个自定义类型的assert(相当于自定义类型的配置文件)
 * 读取方法:
 * 1.在需要引用数据的脚本中定义一个成员变量，然后在编辑器中选取绑定
 * 2.通过AssetBundle.Load动态加载(可以从Web端下载或本地读取)
 * **/

////////////////////////////////////////////////////////////////////////////////////////////////
/************************** 一定要注意的超级巨坑 **********************************************/
/********************* 要存储的类型T，必须要有同名的cs文件 ************************************/
/********************* 例如T为SkillData,则必须建一个SkillData.cs的文件*************************/
////////////////////////////////////////////////////////////////////////////////////////////////
public class CustomInspector<T> : Editor, ICustomEditorWindowCallback where T : ScriptableObject
{
    private static T oldDataInstance = null;
    public static T dataInstance;
    public static CustomEditorWindow editorInstance;               // 编辑器窗口对象
    protected static string strTypeName = typeof(T).ToString();    // 当前的类型名称
    protected GameObject preview_character = null;                 // 正在预览的角色

    static string prefabBasePath = "Assets/Prefabs/Effect/CeHua/";
    static string scemeBasePath = "Assets/Scheme/LightingEffect";
    private void GenerateAllResInfo()
    {
        //UnityEngine.Object[] effects = AssetBundleManagerEditor.LoadAllAssetOnEditorFromPath(scemeBasePath, typeof(LightingEffect));
        //foreach (UnityEngine.Object obj in effects)
        //{
        //    LightingEffect e = obj as LightingEffect;
        //    if (!e)
        //    {
        //        continue;
        //    }
        //    SerializedObject serObj = new SerializedObject(e);

        //    SerializedProperty prop = serObj.FindProperty("effectPrefb");
        //    //拖拽预制体的时候,同时赋值三个信息
        //    if (null != prop)
        //    {
        //        //如果是旧版的，用旧版的预制体即可
        //        if (prop.objectReferenceValue)
        //        {
        //            AssetImporter asp = AssetImporter.GetAtPath(AssetDatabase.GetAssetPath(prop.objectReferenceValue));
        //            SerializedProperty AssetGUID_prop = serObj.FindProperty("AssetGUID");
        //            if (asp && null != AssetGUID_prop)
        //            {
        //                AssetGUID_prop.stringValue = AssetBundleManagerEditor.GetAssetGUIDFromMeta(asp.assetPath);
        //                prop.objectReferenceValue = null;
        //            }
        //            else
        //            {
        //                Debug.LogError("获取资源信息失败!");
        //            }
        //        }
        //        //新版的预制体是为空的，要想重新刷新，就看GUID是不是空的，如果GUID也是空的，这个资源是丢失的
        //        else
        //        {
        //            SerializedProperty AssetGUID_prop = serObj.FindProperty("AssetGUID");

        //            if (null != AssetGUID_prop)
        //            {
        //                string guid = AssetGUID_prop.stringValue;
        //                if (guid.Equals(string.Empty))
        //                {
        //                    Debug.LogError(e.name + "获取资源信息失败!--GUID为空");
        //                    continue;
        //                }
        //                AssetBundleManagerEditor.AssetBundleEditorInfo info = AssetBundleImporter.MakePrefabAssetBundleInfo(AssetDatabase.GUIDToAssetPath(guid));
        //                if (info == null)
        //                {
        //                    Debug.LogError(e.name + "获取资源信息失败!--资源数据库找不到对应的GUID，原因可能是文件被删除或者GUID冲突");
        //                    continue;
        //                }
        //                AssetGUID_prop.stringValue = info.guid;
        //                prop.objectReferenceValue = null;
        //            }
        //            else
        //            {
        //                Debug.LogError("获取资源信息失败!");
        //            }
        //        }

        //    }

        //    serObj.ApplyModifiedProperties();
        //    EditorUtility.SetDirty(e);
        //}
        //AssetBundleManagerEditor.SaveAndRefershAssets();
    }

    private void ConvertEffectCamera()
    {

        //UnityEngine.Object[] effects = AssetBundleManagerEditor.LoadAllAssetOnEditorFromPath(scemeBasePath, typeof(LightingEffect));
        //foreach (UnityEngine.Object obj in effects)
        //{
        //    LightingEffect_Camera e = obj as LightingEffect_Camera;
        //    if (!e)
        //    {
        //        continue;
        //    }

        //    string sourcePath = AssetDatabase.GetAssetPath(e);
        //    sourcePath = sourcePath.Replace(scemeBasePath + "/",string.Empty);
        //    sourcePath = sourcePath.Replace(Path.GetExtension(sourcePath), string.Empty);
        //    sourcePath = prefabBasePath + sourcePath + ".prefab";

        //    string folder = Path.GetDirectoryName(sourcePath);
        //    if(!System.IO.Directory.Exists(folder))
        //    {
        //        System.IO.Directory.CreateDirectory(folder);
        //    }

        //    GameObject go = new GameObject();
        //    go.name = e.name;
        //    Effect.Effect_CameraAnimation aniInstance = go.AddComponent<Effect.Effect_CameraAnimation>();

        //    aniInstance.animationClip = e.animationClip;
        //    aniInstance.cameraAxis = e.cameraAxis;

        //    go = PrefabUtility.CreatePrefab(sourcePath, go);
        //    //AssetBundleManagerEditor.SaveAndRefershAssets();
            
        //    e.AssetGUID = AssetDatabase.AssetPathToGUID(AssetDatabase.GetAssetPath(go));
        //    EditorUtility.SetDirty(e);
        //}
        //AssetBundleManagerEditor.SaveAndRefershAssets();
    }
    // 选中该对象时回调
    public override void OnInspectorGUI()
    {
        if (GUILayout.Button("Open Editor"))
        {
            Init();
        }

        //GUILayout.Space(30.0f);
        //if(GUILayout.Button("生成所有配置资源信息(不要手贱来点我)"))
        //{
        //    GenerateAllResInfo();
        //}

        GUILayout.Space(30.0f);
        if (GUILayout.Button("转换LightingEffectCamera(不要手贱来点我)"))
        {
            ConvertEffectCamera();
        }
        if (oldDataInstance != dataInstance)
        {
            Init();
            oldDataInstance = dataInstance;
        }
    }

    // 初始化编辑窗口
    public void Init()
    {
        editorInstance = EditorWindow.GetWindow<CustomEditorWindow>(false, strTypeName + " Editor", true);
        editorInstance.dataType = typeof(T);
        editorInstance.callback = (ICustomEditorWindowCallback)this;
        editorInstance.currDataSerializedObj = serializedObject;
        editorInstance.currentEffectPrefabProperty = serializedObject.FindProperty("effectPrefb");
        editorInstance.currentEffectPrefabAssetGUID = serializedObject.FindProperty("AssetGUID");
        editorInstance.Init();
        editorInstance.Show();
        editorInstance.Populate();
    }

    // 创建一个新的对象
    public static void CreateAsset()
    {
        T asset = ScriptableObjectUtility.CreateAsset<T>("New " + strTypeName);
        EditorUtility.FocusProjectWindow();
        Selection.activeObject = asset;
    }

    public void OnCreateAsset()
    {
        CreateAsset();
    }

    // 检查有没有选中对象
    public string GetAssetName()
    {
        if (dataInstance == null)
        {
            return "";
        }

        return dataInstance.name;
    }

    public void OnSelectAsset()
    {
        UnityEngine.Object[] selection = Selection.GetFiltered(typeof(T), SelectionMode.Assets);
        if (selection.Length > 0)
        {
            if (selection[0] == null)
                return;
            dataInstance = (T)selection[0];
        }
    }

    public void OnSave()
    {
        DoSaveAction();
        Undo.RecordObject(dataInstance, strTypeName + " Editor Modify");
        EditorUtility.SetDirty(dataInstance);
    }

    public virtual void DoSaveAction()
    {

    }

    // 编辑一个成员变量,你可以重载这个函数，以修改某种特殊变量的编辑方式
    public void OnMemberEdit(MemberInfo info)
    {
        OnMemberGUI(info);
    }

    public virtual void OnMemberGUI(MemberInfo info)
    {
        DefaultMemberEdit(info);
    }

    // 常规类型成员编辑
    public void CommonMemberEdit<DATA>(MemberInfo info,ref DATA data)
    {
        if (info is FieldInfo)
        {
            FieldInfo field = (FieldInfo)info;
            object cur_value = field.GetValue(data);

            //获取中文Name
            string itemName = "";
            var nameObj = field.GetCustomAttributes(typeof(DescAttribute), true);
            if (nameObj.Length > 0)
            {
                itemName = nameObj[0].ToString();
            }

            //获取中文ToolTip
            string itemTooltip = "";
            var tooltipObj = field.GetCustomAttributes(typeof(TooltipAttribute), true);
            if (tooltipObj.Length > 0)
            {
                itemTooltip = tooltipObj[0].ToString();
            }
            GUIContent itemDescContent = new GUIContent(itemName, itemTooltip);
            if (cur_value is float)
            {
                float new_value = EditorGUILayout.FloatField(itemDescContent, (float)cur_value);
                field.SetValue(data, new_value);
                return;
            }

            if (cur_value is uint)
            {
                field.SetValue(data, (uint)EditorGUILayout.IntField(itemDescContent, (int)((uint)cur_value)));
                return;
            }

            if (cur_value is int)
            {
                field.SetValue(data, EditorGUILayout.IntField(itemDescContent, (int)(cur_value)));
                return;
            }

            if (cur_value is System.UInt32)
            {
                field.SetValue(data, (System.UInt32)EditorGUILayout.IntField(itemDescContent, (int)((System.UInt32)cur_value)));
                return;
            }

            if (cur_value is string)
            {
                field.SetValue(data, EditorGUILayout.TextField(itemDescContent, (string)cur_value));
                return;
            }

            if (cur_value is System.String)
            {
                field.SetValue(data, (System.String)EditorGUILayout.TextField(itemDescContent, (string)(System.String)cur_value));
                return;
            }

            if (cur_value is System.Enum)
            {
                System.Enum new_value = EditorGUILayout.EnumPopup(itemDescContent, (System.Enum)cur_value);
                field.SetValue(data, new_value);
                return;
            }

            if (cur_value is Color)
            {
                Color new_value = EditorGUILayout.ColorField(itemDescContent, (Color)cur_value);
                field.SetValue(data, new_value);
                return;
            }

            if (cur_value is Vector3)
            {
                Vector3 new_value = EditorGUILayout.Vector3Field(itemDescContent, (Vector3)cur_value);
                field.SetValue(data, new_value);
                return;
            }

            if (cur_value is GameObject)
            {
                GameObject new_value = (GameObject)EditorGUILayout.ObjectField("GameObject:", (GameObject)cur_value, typeof(GameObject), false);
                field.SetValue(data, new_value);
            }

            if (cur_value is ScriptableObject)
            {
                MonoScript script = null;
                if (cur_value!=null) script = MonoScript.FromScriptableObject((ScriptableObject)cur_value);
                script = (MonoScript)EditorGUILayout.ObjectField("Script:", script, typeof(MonoScript), false);
                if (script != null) field.SetValue(data, ScriptableObject.CreateInstance(script.GetClass()));
            }
        }
    }

    // 默认的成员编辑
    public void DefaultMemberEdit(MemberInfo info)
    {
        SerializedProperty prop = serializedObject.FindProperty(info.Name);
        if (prop == null)
        {
            return;
        }
        // 显示名字
        EditorGUILayout.BeginVertical(editorInstance.rootGroupStyle);
        {
            EditorGUILayout.BeginHorizontal();
            {
                // 根据info.Name和prop.type自己选择不同的编辑方式
                if (prop.type != "")
                {
                    //AssetBundleName和AssetName不可编辑
                    if (info.Name.Equals("AssetGUID"))
                    {
                        GUI.enabled = false;
                    }
                    EditorGUILayout.PropertyField(prop, true);
                    GUI.enabled = true;

                    //拖拽预制体的时候,同时赋值三个信息
                    if (info.Name.Equals("effectPrefb") && prop.objectReferenceValue)
                    {
                        AssetImporter asp = AssetImporter.GetAtPath(AssetDatabase.GetAssetPath(prop.objectReferenceValue));
                        SerializedProperty AssetGUID_prop = serializedObject.FindProperty("AssetGUID");
                        if (asp && null != AssetGUID_prop)
                        {
                            AssetGUID_prop.stringValue = AssetBundleManagerEditor.GetAssetGUIDFromMeta(asp.assetPath);
                            //prop.objectReferenceValue = null;
                        }
                        else
                        {
                            Debug.LogError("获取资源信息失败!");
                        }
                    }
                    serializedObject.ApplyModifiedProperties();
                }
            } EditorGUILayout.EndHorizontal();

        } EditorGUILayout.EndVertical();
    }

    // 记录数组字段编辑是否展开
    public Dictionary<string, bool> mTogglesState = new Dictionary<string, bool>();
    public delegate void ArrayItemEdit(int i);

    // 默认数组编辑
    public void DefaultArrayEdit<ITEM>(string strItemName, ref ITEM[] item_list, ArrayItemEdit func,ITEM empty)
    {
        if (!mTogglesState.ContainsKey(strItemName))
        {
            mTogglesState[strItemName]= false; // 默认不展开
        }

        EditorGUILayout.BeginVertical(editorInstance.subGroupStyle);
        {
            mTogglesState[strItemName] = EditorGUILayout.Foldout(mTogglesState[strItemName], strItemName+"s (" + item_list.Length + ")", EditorStyles.foldout);
            if (mTogglesState[strItemName])
            {
                EditorGUILayout.BeginVertical(editorInstance.subGroupStyle);
                {
                    for (int i = 0; i < item_list.Length; i++)
                    {
                        EditorGUILayout.Space();
                        EditorGUILayout.BeginVertical(editorInstance.subArrayElementStyle);
                        {
                            EditorGUILayout.Space();
                            EditorGUILayout.BeginHorizontal();
                            {
                                // 调用成员编辑函数
                                func(i);
                                
                                if (GUILayout.Button("", editorInstance.removeButtonStyle))
                                {
                                    item_list = RemoveElement<ITEM>(item_list, item_list[i]);
                                    return;
                                }
                            } EditorGUILayout.EndHorizontal();
                            EditorGUILayout.Space();
                        } EditorGUILayout.EndHorizontal();
                    }
                    EditorGUILayout.Space();
                    if (editorInstance.StyledButton("New "+strItemName))
                        item_list = AddElement<ITEM>(item_list, empty);

                    EditorGUILayout.Space();

                } EditorGUILayout.EndVertical();
            }
        } EditorGUILayout.EndVertical();
    }

    public void OnDisable()
    {
        ClosePreview();
    }

    public void OnDestroy()
    {
        ClosePreview();
    }

    void OnLostFocus()
    {
        ClosePreview();
    }


    public virtual void asset2csv()
    {       
    }

    public virtual void manager2asset()
    {
    }

    public virtual void loadFromCsv()
    {
    }

    public virtual void saveInstance(ScriptableObject data)
    {
        EditorUtility.SetDirty(data);
    }


    public virtual string getResPath(GameObject data)
    {
        string path = AssetDatabase.GetAssetPath(data);
        path = path.Replace(Path.GetFileName(AssetDatabase.GetAssetPath(data)), "");
        int resIndex = path.LastIndexOf("resources") + 10;
        string resPath = "";
        if (resIndex < path.Length - 2)
        {
            resPath = path.Substring(resIndex);
        }
        return resPath;
    }

    // 创建一个预览对象
    public void PreviewCharacter(UnityEngine.Object prefab)
    {
        EditorCamera.SetPosition(Vector3.up * 3.5f);
        EditorCamera.SetRotation(Quaternion.identity);
        EditorCamera.SetOrthographic(true);
        EditorCamera.SetSize(8);
        if (preview_character == null)
        {
            preview_character = (GameObject)PrefabUtility.InstantiatePrefab(prefab);
            preview_character.transform.position = new Vector3(-2, 0, 0);
        }
    }

    public void ClosePreview()
    {
        if (preview_character != null)
        {
            DestroyImmediate(preview_character);
            preview_character = null;
        }
    }

    public ITEM[] RemoveElement<ITEM>(ITEM[] elements, ITEM element)
    {
        List<ITEM> elementsList = new List<ITEM>(elements);
        elementsList.Remove(element);
        return elementsList.ToArray();
    }

    public ITEM[] AddElement<ITEM>(ITEM[] elements, ITEM element)
    {
        List<ITEM> elementsList = new List<ITEM>(elements);
        elementsList.Add(element);
        return elementsList.ToArray();
    }
}

////////////////////////////////////////////////////////////////////////////////////
// 因为C#不支持多重继承，所以定个接口曲线实现一下,使用者不用管这个细节
////////////////////////////////////////////////////////////////////////////////////
public interface ICustomEditorWindowCallback
{
    void OnMemberEdit(MemberInfo info);

    void OnCreateAsset();

    string GetAssetName();

    void OnSelectAsset();

    void OnSave();

    void asset2csv();

    void manager2asset();

    void loadFromCsv();
}

////////////////////////////////////////////////////////////////////////////////////
// 自定义属性编辑窗口实现，因这个类模版化后无法构造，所以用比较别扭的方式实现
////////////////////////////////////////////////////////////////////////////////////
public class CustomEditorWindow : EditorWindow
{
    public ICustomEditorWindowCallback callback;
    public System.Type      dataType;                         // 正在编辑的数据类型
                               
    protected Vector2 scrollPos;                              // 窗口显示风格相关的一些东西
    public string titleStyle;
    public string removeButtonStyle;
    public string addButtonStyle;
    public string rootGroupStyle;
    public string subGroupStyle;
    public string subArrayElementStyle;
    public string foldStyle;
    public string enumStyle;
    public GUIStyle labelStyle;
    public GUIStyle fontStyle;
    public SerializedProperty currentEffectPrefabProperty = null; //当前的effetPrefab属性
    public SerializedProperty currentEffectPrefabAssetGUID = null; //当前的effetPrefab的GUID属性
    public SerializedObject currDataSerializedObj = null;

    public void Init()
    {
        if (currentEffectPrefabAssetGUID != null)
        {
            string v = currentEffectPrefabAssetGUID.stringValue;
            if(!v.Equals(string.Empty))
            {
                currentEffectPrefabProperty.objectReferenceValue = AssetDatabase.LoadAssetAtPath<GameObject>(AssetDatabase.GUIDToAssetPath(v));
            }
            
        }
    }

    void OnSelectionChange()
    {
        Populate();
        Repaint();
    }



    public void OnEnable()
    {
        Populate();
    }

    public void OnDisable()
    {
        if (currDataSerializedObj != null && currentEffectPrefabProperty != null && currentEffectPrefabAssetGUID != null)
        {
            if (currentEffectPrefabProperty != null && currentEffectPrefabProperty.objectReferenceValue != null)
            {
                if (currDataSerializedObj != null)
                {
                    currentEffectPrefabProperty.objectReferenceValue = null;
                    currDataSerializedObj.ApplyModifiedProperties();
                    callback.OnSave();
                    AssetDatabase.SaveAssets();
                }

            }
        }
    }
    void OnFocus()
    {
        Populate();
    }

    // 漂亮一点的按钮形式，也可以直接用GUILayout.Button,就是难看一点
    public bool StyledButton(string label)
    {
        EditorGUILayout.Space();
        GUILayoutUtility.GetRect(1, 20);
        EditorGUILayout.BeginHorizontal();
        GUILayout.FlexibleSpace();
        bool clickResult = GUILayout.Button(label, addButtonStyle);
        GUILayout.FlexibleSpace();
        EditorGUILayout.EndHorizontal();
        EditorGUILayout.Space();
        return clickResult;
    }

    // 初始化界面风格，另外抓取当前选择的对象,关联到dataIntance中
    public void Populate()
    {
        // Style Definitions
        titleStyle = "MeTransOffRight";
        removeButtonStyle = "TL SelectionBarCloseButton";
        addButtonStyle = "CN CountBadge";
        rootGroupStyle = "GroupBox";
        subGroupStyle = "ObjectFieldThumb";
        subArrayElementStyle = "HelpBox";
        foldStyle = "Foldout";
        enumStyle = "MiniPopup";

        labelStyle = new GUIStyle();
        labelStyle.alignment = TextAnchor.MiddleCenter;
        labelStyle.fontStyle = FontStyle.Bold;
        labelStyle.normal.textColor = Color.white;

        fontStyle = new GUIStyle();
        fontStyle.font = (Font)EditorGUIUtility.Load("EditorFont.TTF");
        fontStyle.fontSize = 30;
        fontStyle.alignment = TextAnchor.UpperCenter;
        fontStyle.normal.textColor = Color.white;
        fontStyle.hover.textColor = Color.white;

        if (callback!=null)
        {
            callback.OnSelectAsset();
        }
    }

    // 编辑窗口显示,如果你要自定义某个属性的编辑方式，可以重载OnMemberGUI()
    public void OnGUI()
    {
        if ( callback==null )
        {
            return;
        }

        // 没有选中对象的时候
        string name = callback.GetAssetName();
        if ( name=="" )
        {
            GUILayout.BeginHorizontal("GroupBox");
            GUILayout.Label("Select a " + dataType.ToString() + " file or create a new.", "CN EntryInfo");
            GUILayout.EndHorizontal();
            EditorGUILayout.Space();
            if (GUILayout.Button("Create new " + dataType.ToString()))
            {
                callback.OnCreateAsset();
                callback.OnSelectAsset();
            }

            return;
        }

        // 显示正在编辑的对象的名称(数据类型默认必须有一个name属性)
        EditorGUILayout.BeginVertical(titleStyle);
        {
            EditorGUILayout.BeginHorizontal();
            {
                EditorGUILayout.LabelField("", name, fontStyle, GUILayout.Height(32));
            } EditorGUILayout.EndHorizontal();
        } EditorGUILayout.EndVertical();

        // 属性编辑区域（带滚动条)
        scrollPos = EditorGUILayout.BeginScrollView(scrollPos);
        {
            EditorGUILayout.BeginVertical(rootGroupStyle);
            {
                System.Type baseType=dataType.BaseType;
                if (baseType!=null)
                {
                    MemberInfo[] memberInfo = baseType.GetMembers((BindingFlags)(-1));
                    foreach (MemberInfo info in memberInfo)
                    {
                        // (将当前成员显示到检视面板上)
                        callback.OnMemberEdit(info);
                    }
                }
                {
                    MemberInfo[] memberInfo = dataType.GetMembers((BindingFlags)(-1));
                    foreach (MemberInfo info in memberInfo)
                    {
                        // (将当前成员显示到检视面板上)
                        callback.OnMemberEdit(info);
                    }
                }         
            } EditorGUILayout.EndVertical();
        } EditorGUILayout.EndScrollView();

        //按钮点击里不用再处理，按钮点击了GUI.changed就会触发就会触发后面的callback.OnSave，不要重复触发
        if (StyledButton("Apply Changes")) 
        {
            //callback.OnSave();
        }

        if (GUILayout.Button("export asset to csv"))
        {
            callback.asset2csv();
        }

        if (GUILayout.Button("export csv to asset"))
        {
            callback.manager2asset();
        }

        if (GUILayout.Button("import csv to manager"))
        {
            callback.loadFromCsv();
        }

        // 保存数据
        if (GUI.changed)
        {
            callback.OnSave(); 
        }
    }
}