using UnityEngine;
using System.Collections;
using UnityEditor;
namespace Effect
{
    [CustomEditor(typeof(Effect_VerticesModify))]
    public class Effect_VerticesModifyEditor : Editor
    {

        private Effect_VerticesModify tar;
        public void OnEnable()
        {
            tar = target as Effect_VerticesModify;
        }

        public override void OnInspectorGUI()
        {
            tar.TargetType = (MaterialOpRunType)EditorGUILayout.EnumPopup("作用模式:", tar.TargetType);
            if (tar.TargetType == MaterialOpRunType.PointedModel)
            {
                tar.TargetModel = EditorGUILayout.ObjectField("指定模型:", tar.TargetModel, typeof(GameObject), true) as GameObject;
            }
            if (tar.TargetType == MaterialOpRunType.EntityID)
            {
                tar.AffectRenderName = EditorGUILayout.TextField("渲染器名:", tar.AffectRenderName);
                if (string.IsNullOrEmpty(tar.AffectRenderName))
                {
                    EditorGUILayout.HelpBox("不填渲染器名字的话，游戏里不会有效果的哦~", MessageType.Error);
                }
            }

            EditorGUILayout.TextField("标示名:", tar.MeshKeyName);
            EditorGUILayout.TextField("数据路径:", tar.DataPatchName);

            if (GUI.changed)
            {
                EditorUtility.SetDirty(tar);
            }

            if (GUILayout.Button("Init"))
            {
                if (EditorApplication.isPlaying)
                {
                    tar.Init();
                }
               
            }

            if (GUILayout.Button("Play"))
            {
                if (EditorApplication.isPlaying)
                {
                    tar.Play();
                }

            }

            if (GUILayout.Button("Stop"))
            {
                if (EditorApplication.isPlaying)
                {
                    tar.Stop();
                }
                
            }
        }

    }
}

