//using UnityEngine;
//using System.Collections;
//using System.Collections.Generic;
//using UnityEditor;

//[CustomEditor(typeof(HeroSelectConfig))]
//public class HeroSelectConfigEditor : Editor
//{
    
//    [MenuItem("Assets/Create/HeroSelectConfig")]
//    public static void Create()
//    {
//        HeroSelectConfig asset = ScriptableObjectUtility.CreateAsset<HeroSelectConfig>("HeroSelectConfig");
//        EditorUtility.FocusProjectWindow();
//        Selection.activeObject = asset;
//    }
//    private HeroSelectConfig m_Instance = null;

//    private Vector2 scrollpos = Vector2.zero;

//    private Dictionary<int, List<string>> AniNameTable = new Dictionary<int, List<string>>();
//    private Dictionary<int, List<string>> AniLengthTable = new Dictionary<int, List<string>>();

//    int idIndex = 1;
//    public void OnEnable()
//    {
//        m_Instance = Selection.activeObject as HeroSelectConfig;
//        Read_Data();
//    }

//   public void OnDisable()
//    {
//        Write_Data();
//    }
//    void Write_Data()
//    {
//        m_Instance.AnimationName.Clear();
//        m_Instance.AnimationLength.Clear();

//        for (int i = 0; i < m_Instance.HeroIDs.Count; i++)
//        {
//            int heroID = m_Instance.HeroIDs[i];
//            m_Instance.AnimationName.AddRange(AniNameTable[heroID]);
//            m_Instance.AnimationName.Add(HeroSelectConfig.HerosSplitKey);

//            m_Instance.AnimationLength.AddRange(AniLengthTable[heroID]);
//            m_Instance.AnimationLength.Add(HeroSelectConfig.HerosSplitKey);
//        }
//    }

//    void Read_Data()
//    {
//        AniNameTable.Clear();
//        AniLengthTable.Clear();
//        m_Instance.DecodeData(AniNameTable, AniLengthTable);
//    }
//    public override void  OnInspectorGUI()
//    {
//        scrollpos = GUILayout.BeginScrollView(scrollpos);

//        for (int i = 0; i < m_Instance.Heros.Count;i++ )
//        {
//            GUILayout.BeginVertical("Box");

//            GUILayout.Label("英雄ID:");
//            m_Instance.HeroIDs[i] = EditorGUILayout.IntField(m_Instance.HeroIDs[i]);
//            GUILayout.Label("英雄预制体:");
//            m_Instance.Heros[i] = EditorGUILayout.ObjectField(m_Instance.Heros[i], typeof(GameObject), false) as GameObject;
//            m_Instance.HerosRot[i] = EditorGUILayout.Vector3Field("角度:",m_Instance.HerosRot[i]);

//            GUILayout.Label("使用的动画列表:");
//            if (!AniNameTable.ContainsKey(m_Instance.HeroIDs[i]))
//            {
//                AniNameTable.Add(m_Instance.HeroIDs[i], new List<string>());
//                AniLengthTable.Add(m_Instance.HeroIDs[i], new List<string>());
//            }
//            List<string> aniName = AniNameTable[m_Instance.HeroIDs[i]];
//            List<string> aniLength = AniLengthTable[m_Instance.HeroIDs[i]];

//            if (aniName.Count > 0)
//            {
//                GUILayout.BeginVertical("Box");
//                for (int j = 0; j < aniName.Count; j++)
//                {
//                    GUILayout.BeginHorizontal("Box");
//                    EditorGUILayout.LabelField("动作名称:" + aniName[j]);
//                    if (GUILayout.Button("删除"))
//                    {
//                        aniName.RemoveAt(j);
//                        aniLength.RemoveAt(j);
//                        Write_Data();
//                    }
//                    GUILayout.EndHorizontal();
//                }
//                GUILayout.EndVertical();

//            }

//            GUILayout.Label("未使用的动画列表:");
//            if (null != m_Instance.Heros[i])
//            {
//                if (null != m_Instance.Heros[i].GetComponent<Animator>())
//                {
//                    string patch = AssetDatabase.GetAssetPath(m_Instance.Heros[i].GetComponent<Animator>().runtimeAnimatorController);
//                    UnityEditor.Animations.AnimatorController col = AssetDatabase.LoadAssetAtPath<UnityEditor.Animations.AnimatorController>(patch);
//                    if (null != col)
//                    {
//                        foreach (UnityEditor.Animations.AnimatorControllerLayer lay in col.layers)
//                        {
//                            UnityEditor.Animations.AnimatorStateMachine machine = lay.stateMachine;
//                            foreach (UnityEditor.Animations.ChildAnimatorStateMachine stm in machine.stateMachines)
//                            {
//                                if (stm.stateMachine.name.Equals("acttack"))
//                                {
//                                    GUILayout.BeginVertical("Box");
//                                    InsertFixedMotion(aniName, aniLength);
//                                    foreach (UnityEditor.Animations.ChildAnimatorState st in stm.stateMachine.states)
//                                    {
//                                        if (!aniName.Contains(st.state.name))
//                                        {
//                                            GUILayout.BeginHorizontal("Box");
//                                            EditorGUILayout.LabelField("动作名称:" + st.state.name);
//                                            if (GUILayout.Button("插入"))
//                                            {
//                                                aniName.Add(st.state.name);
//                                                aniLength.Add(st.state.motion.averageDuration.ToString("F3"));
//                                                Write_Data();
//                                            }
//                                            GUILayout.EndHorizontal();
//                                        }

//                                    }
//                                    GUILayout.EndVertical();
//                                }
//                            }
//                        }

//                    }
//                }
//            }

            
//            if (GUILayout.Button("删除"))
//            {
//                m_Instance.HeroIDs.RemoveAt(i);
//                m_Instance.Heros.RemoveAt(i);
//                m_Instance.HerosRot.RemoveAt(i);  
//            }
//            GUILayout.EndVertical();
//        }
//        GUILayout.EndScrollView();


//        m_Instance.SelectedPos = EditorGUILayout.Vector3Field("选中位置", m_Instance.SelectedPos);
        
//        if (m_Instance.HeroIDs.Count > 0)
//        {
//            idIndex = m_Instance.HeroIDs[m_Instance.HeroIDs.Count - 1];
//        }
        
//        if (GUILayout.Button("增加一组配置"))
//        {
//            idIndex++;
//            m_Instance.HeroIDs.Add(idIndex);
//            m_Instance.Heros.Add(null);
//            m_Instance.HerosRot.Add(Vector3.zero);  

//        }
//        m_Instance.UpdateData();
//        EditorUtility.SetDirty(m_Instance);
//    }


//    /// <summary>
//    /// 插入固有动画
//    /// </summary>
//    private void InsertFixedMotion(List<string> aniName, List<string> aniLength)
//    {
//        if (!aniName.Contains(HeroSelectStateMachine.MoveFornt))
//        {
//            GUILayout.BeginHorizontal("Box");
//            EditorGUILayout.LabelField("动作名称:" + HeroSelectStateMachine.MoveFornt);
//            if (GUILayout.Button("插入"))
//            {
//                aniName.Add(HeroSelectStateMachine.MoveFornt);
//                aniLength.Add("-1.00");
//                Write_Data();
//            }
//            GUILayout.EndHorizontal();
//        }

//        if (!aniName.Contains(HeroSelectStateMachine.MoveBack))
//        {
//            GUILayout.BeginHorizontal("Box");
//            EditorGUILayout.LabelField("动作名称:" + HeroSelectStateMachine.MoveBack);
//            if (GUILayout.Button("插入"))
//            {
//                aniName.Add(HeroSelectStateMachine.MoveBack);
//                aniLength.Add("-1.00");
//                Write_Data();
//            }
//            GUILayout.EndHorizontal();
//        }

//        if (!aniName.Contains(HeroSelectStateMachine.MoveLeft))
//        {
//            GUILayout.BeginHorizontal("Box");
//            EditorGUILayout.LabelField("动作名称:" + HeroSelectStateMachine.MoveLeft);
//            if (GUILayout.Button("插入"))
//            {
//                aniName.Add(HeroSelectStateMachine.MoveLeft);
//                aniLength.Add("-1.00");
//                Write_Data();
//            }
//            GUILayout.EndHorizontal();
//        }

//        if (!aniName.Contains(HeroSelectStateMachine.MoveRight))
//        {
//            GUILayout.BeginHorizontal("Box");
//            EditorGUILayout.LabelField("动作名称:" + HeroSelectStateMachine.MoveRight);
//            if (GUILayout.Button("插入"))
//            {
//                aniName.Add(HeroSelectStateMachine.MoveRight);
//                aniLength.Add("-1.00");
//                Write_Data();
                
//            }
//            GUILayout.EndHorizontal();
//        }

//    }
//}
