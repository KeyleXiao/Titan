using System;
using UnityEditor;
using UnityEngine;
namespace ImageEffects
{
    /// <summary>
    /// GlowManager编辑器
    /// </summary>
    [CustomEditor(typeof(GlobalFog))]
    public class GlobalFogEditor : Editor
    {
        GlobalFog fogInstance;
        public void OnEnable()
        {
            fogInstance = target as GlobalFog;
        }

        public override void OnInspectorGUI()
        {
            fogInstance.fogColor = EditorGUILayout.ColorField("颜色:", fogInstance.fogColor);
            fogInstance.SkyBoxBlend = EditorGUILayout.Slider("天空盒混合:", fogInstance.SkyBoxBlend,0.0f,1.0f);
            EditorGUILayout.BeginVertical("box");
            EditorGUILayout.LabelField("距离雾:");
            fogInstance.distanceFogParam.enabled = EditorGUILayout.Toggle("启用:", fogInstance.distanceFogParam.enabled);
            fogInstance.distanceFogParam.fogMode = (FogMode)EditorGUILayout.EnumPopup("模式:", fogInstance.distanceFogParam.fogMode);
            if(fogInstance.distanceFogParam.fogMode != FogMode.Linear)
            {
                fogInstance.distanceFogParam.fogDenstiy = EditorGUILayout.FloatField("密度:", fogInstance.distanceFogParam.fogDenstiy);
            }

            if(fogInstance.distanceFogParam.fogMode == FogMode.Linear)
            {
                fogInstance.distanceFogParam.startDistance = EditorGUILayout.FloatField("开始距离:", fogInstance.distanceFogParam.startDistance);
                fogInstance.distanceFogParam.EndDistance = EditorGUILayout.FloatField("结束距离:", fogInstance.distanceFogParam.EndDistance);
            }
            else
            {
                fogInstance.distanceFogParam.startDistance = EditorGUILayout.FloatField("距离偏移:", fogInstance.distanceFogParam.startDistance);
            }
            fogInstance.distanceFogParam.useRadialDistance = EditorGUILayout.Toggle("径向距离   :", fogInstance.distanceFogParam.useRadialDistance);
            EditorGUILayout.EndVertical();


            GUILayout.Space(15.0f);
            EditorGUILayout.BeginVertical("box");
            EditorGUILayout.LabelField("高度雾:");
            fogInstance.heightFogParam.enabled = EditorGUILayout.Toggle("启用:", fogInstance.heightFogParam.enabled);
            fogInstance.heightFogParam.height = EditorGUILayout.FloatField("高度:", fogInstance.heightFogParam.height);
            fogInstance.heightFogParam.heightDensity = EditorGUILayout.FloatField("密度:", fogInstance.heightFogParam.heightDensity);
            EditorGUILayout.EndVertical();

            if(GUI.changed)
            {
                EditorUtility.SetDirty(this);
            }
        }
    }
}

