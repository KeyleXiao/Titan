/*******************************************************************
** 文件名:	SkyLightEditor.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/11/3
** 版  本:	1.0
** 描  述:	天空系统
** 应  用:  	天空系统--天空灯光辑器脚本

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Reflection;
using UnityEditor;

namespace SkySystem
{
    [CustomEditor(typeof(SkyLight))]
    public class SkyLightEditor : Editor
    {

        SkyLight m_skyLight = null;

        SerializedProperty SkyColor;
        SerializedProperty EquatorColor;
        SerializedProperty GroundColor;
        SerializedProperty LightColor;

        //bool ambientSettings = false;

        public void OnEnable()
        {
            SkyColor = serializedObject.FindProperty("SkyColor");
            EquatorColor = serializedObject.FindProperty("EquatorColor");
            GroundColor = serializedObject.FindProperty("GroundColor");
            LightColor = serializedObject.FindProperty("LightColor");
            m_skyLight = target as SkyLight;
        }

        public override void OnInspectorGUI()
        {
            //ambientSettings = EditorGUILayout.Foldout(ambientSettings,"环境光设置");
            //if (ambientSettings)
            {
                EditorGUILayout.LabelField("环境光设置:");

                EditorGUI.BeginChangeCheck();
                EditorGUILayout.PropertyField(SkyColor, new GUIContent("天空颜色:", "天空的上部分颜色"), false);
                EditorGUILayout.PropertyField(EquatorColor, new GUIContent("中部颜色:", "天空的中部分颜色"), false);
                EditorGUILayout.PropertyField(GroundColor, new GUIContent("底部颜色:", "天空的低部分颜色"), false);

                if (EditorGUI.EndChangeCheck())
                {
                    serializedObject.ApplyModifiedProperties();
                }
            }

            EditorGUILayout.Space();

            EditorGUI.BeginChangeCheck();
            EditorGUILayout.PropertyField(LightColor, new GUIContent("月亮和太阳发出颜色:", "中间部分是白天太阳发出的颜色，两边是晚上月亮发出的颜色"), false);
            if (EditorGUI.EndChangeCheck())
            {
                serializedObject.ApplyModifiedProperties();
            }

            m_skyLight.SunIntensity = EditorGUILayout.Slider("太阳光强度:", m_skyLight.SunIntensity, 0.0f, 4.0f);
            m_skyLight.EnableMoonLighting = EditorGUILayout.Toggle("启用月亮灯光:", m_skyLight.EnableMoonLighting);
            if (m_skyLight.EnableMoonLighting)
            {
                m_skyLight.MoonIntensity = EditorGUILayout.Slider("月亮光强度:", m_skyLight.MoonIntensity, 0.0f, 2.0f);
            }

        }
    }
}
