/*******************************************************************
** 文件名:	Effect_FadeInOutMaterialFloatEditor.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/8/4
** 版  本:	1.0
** 描  述:	特效功能脚本
** 应  用:  	float淡入淡出编辑器

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using UnityEditor;

namespace Effect
{
    [CustomEditor(typeof(Effect_MaterialGradientColor))]
    public class Effect_MaterialGradientColorEditor : Editor
    {
        Effect_MaterialGradientColor pa;
        SerializedProperty gradPro;
        public void OnEnable()
        {
            pa = target as Effect_MaterialGradientColor;
            gradPro = serializedObject.FindProperty("gradColor");
        }

        public override void OnInspectorGUI()
        {
            GUILayout.TextField("所有跟时间有关的变量小于0.03秒，等同于无效!");
            pa.ColorName = EditorGUILayout.TextField("属性名:",pa.ColorName);
            pa.Loop = EditorGUILayout.Toggle("循环:",pa.Loop);

            GUI.enabled = !EditorApplication.isPlaying;
            pa.RunMode = (EffectRunMode)EditorGUILayout.EnumPopup("运行方式:", pa.RunMode);
            if (pa.RunMode == EffectRunMode.Self)
            {
                pa.UseSharedMaterial = EditorGUILayout.Toggle("使用共享材质:", pa.UseSharedMaterial);
            }
            GUI.enabled = true;
            CurveUI();
            if (GUILayout.Button("Play"))
            {
                pa.Play();
            }
            if (GUILayout.Button("Stop"))
            {
                pa.Stop();
            }
        }


        void CurveUI()
        {
            pa.DuartionTime = EditorGUILayout.FloatField("Color Time Life-武蘅说的(必须大于0.03):", pa.DuartionTime);
            pa.DelayTime = EditorGUILayout.FloatField("延迟(必须大于0.03):", pa.DelayTime);
            if (pa.DuartionTime < 0.03)
            {
                pa.DuartionTime = 0.03f;
            }

            EditorGUILayout.PropertyField(gradPro,new GUIContent("颜色:"));
            serializedObject.ApplyModifiedProperties();
        }
    }
}
