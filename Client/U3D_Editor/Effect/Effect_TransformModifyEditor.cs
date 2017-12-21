using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

namespace Effect
{
    [CustomEditor(typeof(Effect_TransformModify))]
    public class Effect_TransformModifyEditor : Editor
    {
        Effect_TransformModify transformModify;

        public void OnEnable()
        {
            transformModify = target as Effect_TransformModify;
        }

        public override void OnInspectorGUI()
        {
            EditorGUILayout.BeginVertical("box");
            EditorGUILayout.LabelField("位置:");
            DrawModifyInfo(transformModify.PositionInfo);
            EditorGUILayout.EndVertical();
            GUILayout.Space(5.0f);

            EditorGUILayout.BeginVertical("box");
            EditorGUILayout.LabelField("旋转:");
            DrawModifyInfo(transformModify.RoationInfo);
            EditorGUILayout.EndVertical();
            GUILayout.Space(5.0f);

            EditorGUILayout.BeginVertical("box");
            EditorGUILayout.LabelField("缩放:");
            DrawModifyInfo(transformModify.ScaleInfo);
            EditorGUILayout.EndVertical();

            if (GUILayout.Button("Play"))
            {
                transformModify.Play();

            }
            if (GUILayout.Button("Stop"))
            {
                transformModify.Stop();
            }


        }

        private void DrawModifyInfo(ModifyInfo3 info)
        {
            EditorGUILayout.BeginVertical("box");
            info.blendMode = (ModifyBlendMode)EditorGUILayout.EnumPopup("混合模式:", info.blendMode);
            EditorGUILayout.BeginVertical("box");
            info.Duration.x = EditorGUILayout.FloatField("X持续时间(小于0不起作用):", info.Duration.x);
            if (info.Duration.x > 0)
            {
                info.IsLoop.x = EditorGUILayout.Toggle( "循环:",info.IsLoop.x > 0) ? 1 : -1;
                info.CurveX = EditorGUILayout.CurveField("曲线:", info.CurveX);

                info.Scale.x = EditorGUILayout.FloatField( "曲线缩放:",info.Scale.x);
            }

            EditorGUILayout.EndVertical();
            GUILayout.Space(2.0f);

            EditorGUILayout.BeginVertical("box");
            info.Duration.y = EditorGUILayout.FloatField( "Y持续时间(小于0不起作用):",info.Duration.y);
            if (info.Duration.y > 0)
            {
                info.IsLoop.y = EditorGUILayout.Toggle( "循环:",info.IsLoop.y > 0) ? 1 : -1;
                info.CurveY = EditorGUILayout.CurveField("曲线:", info.CurveY);
                info.Scale.y = EditorGUILayout.FloatField( "曲线缩放:",info.Scale.y);
            }
            EditorGUILayout.EndVertical();
            GUILayout.Space(2.0f);

            EditorGUILayout.BeginVertical("box");
            info.Duration.z = EditorGUILayout.FloatField("Z持续时间(小于0不起作用):", info.Duration.z);
            if (info.Duration.z > 0)
            {
                info.IsLoop.z = EditorGUILayout.Toggle( "循环:",info.IsLoop.z > 0) ? 1 : -1;
                info.CurveZ = EditorGUILayout.CurveField("曲线:", info.CurveZ);
                info.Scale.z = EditorGUILayout.FloatField("曲线缩放:", info.Scale.z);
            }
            EditorGUILayout.EndVertical();


            EditorGUILayout.EndVertical();
        }
    }
}
