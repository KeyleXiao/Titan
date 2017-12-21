using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

namespace Effect
{
    [CustomEditor(typeof(Effect_ProjectorModify))]
    public class Effect_ProjectorModifyEditor : Editor
    {
        Effect_ProjectorModify ProjectorModify;

        public void OnEnable()
        {
            ProjectorModify = target as Effect_ProjectorModify;
        }

        public override void OnInspectorGUI()
        {
            EditorGUILayout.BeginVertical("box");
            EditorGUILayout.LabelField("旋转:");
            DrawProjectorInfo(ProjectorModify.ProjectorRoationInfo);
            EditorGUILayout.EndVertical();
            GUILayout.Space(5.0f);


            EditorGUILayout.BeginVertical("box");
            EditorGUILayout.LabelField("大小:");
            DrawProjectorInfo(ProjectorModify.ProjectorSizeInfo);
            EditorGUILayout.EndVertical();
            GUILayout.Space(5.0f);

            if(GUILayout.Button("Bake"))
            {
                ProjectorModify.Bake();
            }

            if (GUILayout.Button("Play"))
            {
                ProjectorModify.Play();

            }
            if (GUILayout.Button("Stop"))
            {
                ProjectorModify.Stop();
            }


        }

        private void DrawProjectorInfo(ModifyInfo info)
        {
            EditorGUILayout.BeginVertical("box");
            info.blendMode = (ModifyBlendMode)EditorGUILayout.EnumPopup("混合模式:", info.blendMode);
            EditorGUILayout.BeginVertical("box");
            info.Duration = EditorGUILayout.FloatField("持续时间(小于0不起作用):", info.Duration);
            if (info.Duration > 0)
            {
                info.IsLoop = EditorGUILayout.Toggle( "循环:",info.IsLoop > 0) ? 1 : -1;
                info.Curve = EditorGUILayout.CurveField("曲线:", info.Curve);

                info.Scale = EditorGUILayout.FloatField( "曲线缩放:",info.Scale);
            }

            EditorGUILayout.EndVertical();
            GUILayout.Space(2.0f);

            EditorGUILayout.EndVertical();
        }
    }
}
