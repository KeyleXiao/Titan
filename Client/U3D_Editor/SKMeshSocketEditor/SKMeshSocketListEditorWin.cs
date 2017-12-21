/*******************************************************************
** 文件名:	SKMeshSocketListEditorWin.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2015/2/4
** 版  本:	1.0
** 描  述:	带骨骼模型创建插槽窗口
** 应  用:  
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using UnityEditor;
using System.Collections;
using System.Collections.Generic;

[CustomEditor(typeof(SKMeshSocketListInfo))]
public class SocketListEditor : Editor
{
    public override void OnInspectorGUI()
    {
        if (GUILayout.Button("Open SKMesh Socket Editor "))
            SKMeshSocketListEditorWin.Init();

    }
}

public class SKMeshSocketListEditorWin : EditorWindow
{
    public static SKMeshSocketListEditorWin SlotGroupEditorWin;

    private bool characterPreviewToggle;//是否打开角色预览
    private bool bSlotGroupToggle;//是否打开插槽组
    private bool bHasPreviewPrefab;
    private SKMeshSocketListInfo slotGroupInformation;
    private GameObject character;

    private bool characterWarning;
    private string errorMsg;

    //Editor UI
    private Vector2 scrollPos;
    //private string titleStyle;
    private string removeButtonStyle;
    private string addButtonStyle;
    private string rootGroupStyle;
    private string subGroupStyle;
    private string subArrayElementStyle;
    //private string arrayElementStyle;
    private string foldStyle;
    //private string enumStyle;
    private GUIStyle labelStyle;


    [MenuItem("Window/SKMesh Socket List Editor")]
    public static void Init()
    {
        SlotGroupEditorWin = EditorWindow.GetWindow<SKMeshSocketListEditorWin>(false, "SKMesh Socket List", true);
        SlotGroupEditorWin.Show();
        SlotGroupEditorWin.Populate();
    }

    void Populate()
    {
        // Style Definitions
         //titleStyle = "MeTransOffRight";
        removeButtonStyle = "TL SelectionBarCloseButton";
        addButtonStyle = "CN CountBadge";
        rootGroupStyle = "GroupBox";
        subGroupStyle = "ObjectFieldThumb";
        subArrayElementStyle = "HelpBox";
        //arrayElementStyle = "flow overlay box";
        foldStyle = "Foldout";
        // enumStyle = "MiniPopup";

        labelStyle = new GUIStyle();
        labelStyle.alignment = TextAnchor.MiddleCenter;
        labelStyle.fontStyle = FontStyle.Bold;
        labelStyle.normal.textColor = Color.white;
        
        Object[] selection = Selection.GetFiltered(typeof(SKMeshSocketListInfo), SelectionMode.Assets);
        if (selection.Length > 0)
        {
            if (selection[0] == null)
                return;
            //获取选择的SlotGroupInformation
            slotGroupInformation = (SKMeshSocketListInfo)selection[0];

        }
        
    }

    public void OnGUI()
    {
        if (slotGroupInformation == null)
        {
            GUILayout.BeginHorizontal("GroupBox");
            GUILayout.Label("Select a character file or create a new character.", "CN EntryInfo");
            GUILayout.EndHorizontal();
            EditorGUILayout.Space();
            if (GUILayout.Button("Create new character"))
                ScriptableObjectUtility.CreateAsset<SKMeshSocketListInfo>("charactor_socketList");
            return;
        }

        GUIStyle fontStyle = new GUIStyle();
        fontStyle.font = (Font)EditorGUIUtility.Load("EditorFont.TTF");
        fontStyle.fontSize = 30;
        fontStyle.alignment = TextAnchor.UpperCenter;
        fontStyle.normal.textColor = Color.white;
        fontStyle.hover.textColor = Color.white;



      //  EditorGUILayout.LabelField("Mouse Position: ", Event.current.mousePosition.ToString());
        scrollPos = EditorGUILayout.BeginScrollView(scrollPos);
        {
            EditorGUILayout.BeginVertical(rootGroupStyle);
            {
                EditorGUILayout.BeginHorizontal();
                bHasPreviewPrefab = EditorGUILayout.Foldout(bHasPreviewPrefab, "Socket Component", foldStyle);
                EditorGUILayout.EndHorizontal();

                //有模型预览才能进行编辑
                if (bHasPreviewPrefab)
                {
                    EditorGUILayout.BeginVertical(subGroupStyle);
                    {
                        slotGroupInformation.characterPrefab = (GameObject)EditorGUILayout.ObjectField("Character Prefab", slotGroupInformation.characterPrefab, typeof(UnityEngine.GameObject), true);

                        if (slotGroupInformation.characterPrefab != null)
                        {
                            if (PrefabUtility.GetPrefabType(slotGroupInformation.characterPrefab) != PrefabType.Prefab)
                            {
                                characterWarning = true;
                                errorMsg = "This character is not a prefab.";
                                slotGroupInformation.characterPrefab = null;
                                ClosePreview();
                            }
                            else if (slotGroupInformation.characterPrefab.GetComponent<SKMeshSocketsMgr>() == null)
                            {
                                characterWarning = true;
                                errorMsg = "This character doesn't have CharacterSlotManager!\n Please add the CharacterSlotManager Script and try again.";
                                slotGroupInformation.characterPrefab = null;
                                ClosePreview();
                            }
                            else
                            {
                                //选择的characterPrefab名字和character名字不一致
                                characterWarning = false;
                                if (character != null && slotGroupInformation.characterPrefab.name != character.name)
                                    ClosePreview();
                            }
                        }

                        if (characterWarning)
                        {
                            GUILayout.BeginHorizontal("GroupBox");
                            GUILayout.Label(errorMsg, "CN EntryWarn");
                            GUILayout.EndHorizontal();
                        }

                        /*
                         * 具体编辑操作
                        */

                        //显示character预览
                        if (slotGroupInformation.characterPrefab != null)
                        {
                            //判断是否已经打开character预览
                            if (!characterPreviewToggle)
                            {
                                if (StyledButton("Open Character "))
                                {
                                    EditorWindow.FocusWindowIfItsOpen<SceneView>();
                                    PreviewCharacter();
                                }
                            }
                            else
                            {
                                if (StyledButton("Close Character"))
                                    ClosePreview();
                            }

                            //显示Socket Object
                            if (character!=null)
                            {
                                //bSlotGroupToggle = EditorGUILayout.Foldout(bSlotGroupToggle, "Socket List", foldStyle);
                                //if (bSlotGroupToggle)
                                //{
                               // Debug.Log(UnityEditor.Selection.activeGameObject);
                                    EditorGUILayout.BeginVertical();
                                    {
                                        SKMeshSocketsMgr characterSlotMgr = character.GetComponent<SKMeshSocketsMgr>();

                                        //Edit List Name
                                        // characterSlotMgr.SlotGroupName = (string)EditorGUILayout.TextField("List Name:", characterSlotMgr.SlotGroupName);


                                        for (int i = 0; i < characterSlotMgr.SocketList.Length; i++)
                                        {
                                            //移除按钮
                                            EditorGUILayout.BeginVertical(subArrayElementStyle);
                                            {
                                                EditorGUILayout.BeginHorizontal();
                                                {
                                                    if (GUILayout.Button("", removeButtonStyle))
                                                    {
                                                        characterSlotMgr.SocketList[i].Destroy();
                                                        characterSlotMgr.SocketList = RemoveElement<SkeletalMeshSocket>(characterSlotMgr.SocketList, characterSlotMgr.SocketList[i]);
                                                        return;
                                                    }
                                                }
                                                EditorGUILayout.EndHorizontal();

                                                //填写插槽名字才能进行余下操作
                                                string SocketObjName = characterSlotMgr.SocketList[i].SocketObjectName;
                                                SocketObjName = (string)EditorGUILayout.TextField("Socket Object Name:", SocketObjName);

                                                //判断有没有更改SocketObjectName
                                                if (!string.Equals(SocketObjName, characterSlotMgr.SocketList[i].SocketObjectName))
                                                    // characterSlotMgr.SocketList[i].SocketObjectName = SocketObjName;
                                                    characterSlotMgr.ChangeSocketObjectName(characterSlotMgr.SocketList[i], SocketObjName);

                                                //插槽绑定的骨骼
                                                if (characterSlotMgr.SocketList[i].SocketObjectName != null && characterSlotMgr.SocketList[i].SocketObjectName.Length > 0)
                                                    characterSlotMgr.SocketList[i].SocketAttachBone = (Transform)EditorGUILayout.ObjectField(characterSlotMgr.SocketList[i].SocketAttachBone, typeof(UnityEngine.Transform), true);

                                                if (characterSlotMgr.SocketList[i].SocketAttachBone != null )
                                                {
                                                    //创建插槽对象
                                                    if (characterSlotMgr.SocketList[i].SocketGameObject == null && characterSlotMgr.SocketList[i].SocketObjectName != null)
                                                        characterSlotMgr.CreateSocketGameObject(characterSlotMgr.SocketList[i].SocketObjectName, characterSlotMgr.SocketList[i].SocketAttachBone);



                                                    /*
                                                    characterSlotMgr.SocketList[i].bUsePreviewObject = GUILayout.Toggle(characterSlotMgr.SocketList[i].bUsePreviewObject, characterSlotMgr.SocketList[i].bUsePreviewObject ? ON : OFF );
                                                    if (characterSlotMgr.SocketList[i].bUsePreviewObject)
                                                        characterSlotMgr.CreatePreviewComponent(characterSlotMgr.SocketList[i].SlotObjectName);
                                                    else
                                                        characterSlotMgr.DestroyPreviewComponent(characterSlotMgr.SocketList[i].SlotObjectName);
                                                    */
                                                    //characterSlotMgr.SocketList[i].PreViewSkinnedMesh = (SkinnedMeshRenderer)EditorGUILayout.ObjectField(characterSlotMgr.SocketList[i].PreViewSkinnedMesh, typeof(SkinnedMeshRenderer), true);
                                                    //characterSlotMgr.SocketList[i].SlotMesh = (Mesh)EditorGUILayout.ObjectField(characterSlotMgr.SocketList[i].SlotMesh, typeof(UnityEngine.Mesh), true);
                                                    //characterSlotMgr.SocketList[i].SlotMeshMat = (Material)EditorGUILayout.ObjectField(characterSlotMgr.SocketList[i].SlotMeshMat, typeof(UnityEngine.Material), true);
                                                    // characterSlotMgr.SlotGroup[i].position = EditorGUILayout.Vector3Field("Position:", characterSlotMgr.SlotGroup[i].position);
                                                    // characterSlotMgr.SlotGroup[i].rotation = (Quaternion)EditorGUILayout.Vector3Field("Rotation:", characterSlotMgr.SlotGroup[i].rotation, true);
                                                }
                                            }
                                            EditorGUILayout.EndVertical();
                                            EditorGUILayout.Space();
                                        }
                                    

                                    //创建新的slot
                                    if (StyledButton("New Socket Object"))
                                        characterSlotMgr.SocketList = AddElement<SkeletalMeshSocket>(characterSlotMgr.SocketList, new SkeletalMeshSocket());
                                    }
                                    EditorGUILayout.EndVertical();
                               // }


                            }
                        }

                    }
                    EditorGUILayout.EndVertical();
                }
            }
            EditorGUILayout.EndVertical();
        }

        //编辑器有参数改变时，重新保存到磁盘
        if (StyledButton("Apply"))
        {
            //globalUIInfoList.AddElementToDic();
            Undo.RecordObject(slotGroupInformation, "Socket List Editor Modify");
            EditorUtility.SetDirty(slotGroupInformation);
            //保存预制
            PrefabUtility.ReplacePrefab(character, PrefabUtility.GetPrefabParent(character), ReplacePrefabOptions.ConnectToPrefab);
        } 

        EditorGUILayout.EndScrollView();
    }

    public void Update()
    {
        if (EditorApplication.isPlayingOrWillChangePlaymode && character != null)
        {
            ClosePreview();
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

    void OnFocus()
    {
        Populate();
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
        //ClosePreview();
    }
    /// <summary>
    /// 显示要操作的character prefab
    /// </summary>
    public void PreviewCharacter()
    {
        characterPreviewToggle = true;
        if (character == null)
        {
            character = (GameObject)PrefabUtility.InstantiatePrefab(slotGroupInformation.characterPrefab);
            /*关闭编辑模块角色功能
            //判断是否为模块角色
            ModularPawnMgr IsModularPawn = character.GetComponent<ModularPawnMgr>();
            if (IsModularPawn != null)
            {
                IsModularPawn.InitInEditMode();
            }
             * */
        }
    }
    /// <summary>
    /// 关闭character prefabe 
    /// </summary>
    public void ClosePreview()
    {
        characterPreviewToggle = false;

        if (character != null)
        {
            /* 关闭编辑模块角色功能
            //判断是否为模块角色
            ModularPawnMgr IsModularPawn = character.GetComponent<ModularPawnMgr>();
            if (IsModularPawn != null)
            {
                IsModularPawn.RemoveEditObject();

                //globalUIInfoList.AddElementToDic();
                Undo.RecordObject(slotGroupInformation, "Socket List Editor Modify");
                EditorUtility.SetDirty(slotGroupInformation);
                //保存预制
                PrefabUtility.ReplacePrefab(character, PrefabUtility.GetPrefabParent(character), ReplacePrefabOptions.ConnectToPrefab);

            }
             * */
            DestroyImmediate(character);
            character = null;
        }

    }

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

    public T[] RemoveElement<T>(T[] elements, T element)
    {
        List<T> elementsList = new List<T>(elements);
        elementsList.Remove(element);
        return elementsList.ToArray();
    }

    public T[] AddElement<T>(T[] elements, T element)
    {
        List<T> elementsList = new List<T>(elements);
        elementsList.Add(element);
        return elementsList.ToArray();
    }
}