//using UnityEngine;
//using UnityEditor;
//using System.Collections.Generic;
//using System.Reflection;

//namespace Effect
//{
//    [CustomEditor(typeof(Decal))]
//    public class DecalEditor : Editor
//    {
//        Decal decal;
//        SerializedProperty OnlyThisGameObjectProperty = null;
//        public void OnEnable()
//        {
//            decal = (Decal)target;
//            OnlyThisGameObjectProperty = serializedObject.FindProperty("OnlyAffectThisGameObject");
//            decal.Init();
//        }

//        public override void OnInspectorGUI()
//        {

//            EditorGUILayout.TextField("按住左ctrl+左键可以创建decal,要选中自己,在要创建的物体上，必须要有collider!");
//            EditorGUILayout.Separator();

//            decal.DecalMaterial = EditorGUILayout.ObjectField("贴花材质:", decal.DecalMaterial, typeof(Material),false) as Material;
//            decal.DecalTexture = EditorGUILayout.ObjectField("贴花贴图:", decal.DecalTexture, typeof(Texture),false) as Texture;
//            EditorGUILayout.Separator();

//            decal.maxAngle = EditorGUILayout.FloatField("最大角度:", decal.maxAngle);
//            decal.maxAngle = Mathf.Clamp(decal.maxAngle, 1, 180);
//            EditorGUILayout.Separator();

//            decal.pushDistance = EditorGUILayout.FloatField("贴近距离:", decal.pushDistance);
//            decal.pushDistance = Mathf.Clamp(decal.pushDistance, 0.01f, 1);
//            EditorGUILayout.Separator();

//            decal.affectedLayers = LayerMaskField("影响哪些层:", decal.affectedLayers);

//            serializedObject.Update();
        
            
//            EditorGUI.BeginChangeCheck();
//            EditorGUILayout.PropertyField(OnlyThisGameObjectProperty, true);
//            if (EditorGUI.EndChangeCheck())
//                serializedObject.ApplyModifiedProperties();
//            if (decal.OnlyAffectThisGameObject.Count > 0)
//            {
//                EditorGUILayout.TextField("只影响列表里的物体，其他物体会被忽略!");
//            }

           
//            //showAffectedObject = EditorGUILayout.Foldout(showAffectedObject, "Affected Objects");
//            //if (showAffectedObject && affectedObjects != null)
//            //{
//            //    GUILayout.BeginHorizontal();
//            //    GUILayout.Space(15);
//            //    GUILayout.BeginVertical();
//            //    foreach (GameObject go in affectedObjects)
//            //    {
//            //        EditorGUILayout.ObjectField(go, typeof(GameObject), true);
//            //    }
//            //    GUILayout.EndVertical();
//            //    GUILayout.EndHorizontal();
//            //}
//            EditorGUILayout.Separator();

//            if (GUI.changed)
//            {
//                decal.UpdateDecal();
//            }
//        }


//        void OnSceneGUI()
//        {
//            Decal decal = (Decal)target;

//            if (Event.current.control)
//            {
//                HandleUtility.AddDefaultControl(GUIUtility.GetControlID(FocusType.Passive));
//            }

//            if (!EditorApplication.isPlaying)
//            {
//                if (Event.current.control && Event.current.type == EventType.MouseDown)
//                {
//                    Ray ray = HandleUtility.GUIPointToWorldRay(Event.current.mousePosition);
//                    RaycastHit hit = new RaycastHit();
//                    if (Physics.Raycast(ray, out hit, 50))
//                    {
//                        Vector3 old = hit.transform.eulerAngles;
//                        hit.transform.LookAt(hit.point);

//                        Vector3 LookDirection;
//                        LookDirection = hit.transform.forward;
//                        hit.transform.eulerAngles = old;

//                        decal.transform.position = hit.point;
//                        decal.transform.forward = -LookDirection;

//                        decal.UpdateDecal();
//                    }
//                }
//            }

//        }

//        private static LayerMask LayerMaskField(string label, LayerMask mask)
//        {
//            List<string> layers = new List<string>();
//            for (int i = 0; i < 32; i++)
//            {
//                string name = LayerMask.LayerToName(i);
//                if (name != "") layers.Add(name);
//            }
//            return EditorGUILayout.MaskField(label, mask, layers.ToArray());
//        }
//    }
//}
