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

    // 选中该对象时回调
    public override void OnInspectorGUI()
    {
        if (GUILayout.Button("Open Editor"))
        {
            Init();
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

            if (cur_value is float)
            {
                float new_value = EditorGUILayout.FloatField(info.Name, (float)cur_value);
                field.SetValue(data, new_value);
                return;
            }

            if (cur_value is uint)
            {
                field.SetValue(data, (uint)EditorGUILayout.IntField(info.Name, (int)((uint)cur_value)));
                return;
            }

            if (cur_value is int)
            {
                field.SetValue(data, EditorGUILayout.IntField(info.Name, (int)(cur_value)));
                return;
            }

            if (cur_value is System.UInt32)
            {
                field.SetValue(data, (System.UInt32)EditorGUILayout.IntField(info.Name, (int)((System.UInt32)cur_value)));
                return;
            }

            if (cur_value is string)
            {
                field.SetValue(data, EditorGUILayout.TextField(info.Name, (string)cur_value));
                return;
            }

            if (cur_value is System.String)
            {
                field.SetValue(data, (System.String)EditorGUILayout.TextField(info.Name, (string)(System.String)cur_value));
                return;
            }

            if (cur_value is System.Enum)
            {
                System.Enum new_value = EditorGUILayout.EnumPopup(info.Name, (System.Enum)cur_value);
                field.SetValue(data, new_value);
                return;
            }

            if (cur_value is Color)
            {
                Color new_value = EditorGUILayout.ColorField(info.Name, (Color)cur_value);
                field.SetValue(data, new_value);
                return;
            }

            if (cur_value is Vector3)
            {
                Vector3 new_value = EditorGUILayout.Vector3Field(info.Name, (Vector3)cur_value);
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
                    EditorGUILayout.PropertyField(prop, true);
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

    void OnDisable()
    {
        ClosePreview();
    }

    void OnDestroy()
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

    void OnSelectionChange()
    {
        Populate();
        Repaint();
    }

    void OnEnable()
    {
        Populate();
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
                {
                    MemberInfo[] memberInfo = dataType.BaseType.GetMembers((BindingFlags)(-1));
                    foreach(MemberInfo info in memberInfo)
                    {
                        // (将当前成员显示到检视面板上)
                        callback.OnMemberEdit(info);
                    }
                }
                {
                    MemberInfo[] memberInfo = dataType.GetMembers((BindingFlags)(-1));
                    foreach(MemberInfo info in memberInfo)
                    {
                        // (将当前成员显示到检视面板上)
                        callback.OnMemberEdit(info);
                    }
                }
            } EditorGUILayout.EndVertical();
        } EditorGUILayout.EndScrollView();

        if (StyledButton("Apply Changes"))
        {
            callback.OnSave();
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