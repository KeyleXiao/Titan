using System;
using UnityEditor;
using UnityEngine;
namespace ImageEffects
{
    /// <summary>
    /// GlowManager编辑器
    /// </summary>
    [CustomEditor(typeof(GlowManager))]
    public class GlowManagerEditor : Editor
    {
        private SerializedProperty baseResolution;
        private SerializedProperty blendMode;
        private string[] blurModeOptions = new string[] { "Default", "High Quality", "Unity Blur" };
        private int[] blurModeValues = new int[] { 0, 10, 100 }; 
        private SerializedProperty blurSpread;
        private SerializedProperty boostStrength;
        private SerializedProperty downsampleBlendMode;
        private SerializedProperty downsampleResolution;
        private SerializedProperty downsampleSteps;
        private SerializedProperty falloff;
        private SerializedProperty falloffScale;
        private SerializedProperty innerStrength;
        private SerializedProperty iterations;
        private SerializedProperty normalize;
        private SerializedProperty outerStrength;
        private SerializedProperty radius;
        private SerializedObject serObj;

        private SerializedProperty highQualityGlowIntensity;
        private SerializedProperty highQualityGlowBlurSize;
       // private SerializedProperty useHighQuality;
        GlowManager tar;
        public void OnEnable()
        {
            tar = target as GlowManager;
            if (null == tar)
            {
                return;
            }
            serObj = new SerializedObject(tar);
            innerStrength = serObj.FindProperty("settings.innerStrength");
            outerStrength = serObj.FindProperty("settings.outerStrength");
            boostStrength = serObj.FindProperty("settings.boostStrength");
            downsampleSteps = serObj.FindProperty("settings.downsampleSteps");
            iterations = serObj.FindProperty("settings.iterations");
            blendMode = serObj.FindProperty("settings.blendMode");
            blurSpread = serObj.FindProperty("settings.blurSpread");
            downsampleBlendMode = serObj.FindProperty("settings.downsampleBlendMode");
            downsampleResolution = serObj.FindProperty("settings.downsampleResolution");
            baseResolution = serObj.FindProperty("settings.baseResolution");
            falloff = serObj.FindProperty("settings.falloff");
            falloffScale = serObj.FindProperty("settings.falloffScale");
            radius = serObj.FindProperty("settings.radius");
            normalize = serObj.FindProperty("settings.normalize");


            highQualityGlowIntensity = serObj.FindProperty("settings.HighQualityGlowIntensity");
            highQualityGlowBlurSize = serObj.FindProperty("settings.HighQualityGlowBlurSize");
            //useHighQuality = serObj.FindProperty("settings.UseHightQualityGlow");
        }

        public override void OnInspectorGUI()
        {
            GlowManager target = tar;
            serObj.Update();

            //target.ImageOrder = EditorGUILayout.IntField("ImageOrder", target.ImageOrder);
            //EditorUtility.SetDirty(target);
            if (!PlayerSettings.use32BitDisplayBuffer)
            {
                EditorGUILayout.HelpBox("It is recommended you use a 32-bit display buffer (can be set in the player settings).", MessageType.Warning);
            }


            target.DebugMode = EditorGUILayout.Toggle("调试模式", target.DebugMode, new GUILayoutOption[0]);
            if (target.DebugMode)
            {
                EditorGUILayout.LabelField("调试模式紧紧是给特效美术使用，\n其他情况请关闭！", GUILayout.Height(30));
            }

            //useHighQuality.boolValue = EditorGUILayout.Toggle("高级Glow:", useHighQuality.boolValue, new GUILayoutOption[0]);
            //bool flag = false;
            //if (!useHighQuality.boolValue)
            //{
            //    if (target.highPrecision && !target.highPrecisionActive)
            //    {
            //        EditorGUILayout.HelpBox("Your graphics card doesn't support High Precision.", MessageType.Warning);
            //    }
            //    if (QualitySettings.antiAliasing != 0)
            //    {
            //        GUI.enabled = false;
            //    }

            //    GUI.enabled = true;
            //    EditorGUILayout.Space();
            //    //BlurMode mode = (BlurMode)EditorGUILayout.IntPopup("Blur Mode", (int)target.blurMode, blurModeOptions, blurModeValues, new GUILayoutOption[0]);
            //    //EditorGUI.indentLevel++;
            //    //if (mode != target.blurMode)
            //    //{
            //    //    Undo.RecordObject(target, "Blur Mode");
            //    //    target.blurMode = mode;
            //    //    EditorUtility.SetDirty(target);
            //    //    flag = true;
            //    //}

            //    if (((target.blurMode == BlurMode.HighQuality) || (target.blurMode == BlurMode.UnityBlur)) /*|| (target.blurMode == BlurMode.Advanced)*/)
            //    {
            //        GUILayout.BeginHorizontal(new GUILayoutOption[0]);
            //        iterations.intValue = EditorGUILayout.IntSlider("Iterations", iterations.intValue, 1, 20, new GUILayoutOption[0]);
            //        if (GUILayout.Button("Reset", new GUILayoutOption[0]))
            //        {
            //            GUI.FocusControl("");
            //            iterations.intValue = 3;
            //        }
            //        GUILayout.EndHorizontal();
            //    }
            //    if (target.blurMode == BlurMode.UnityBlur)
            //    {
            //        GUILayout.BeginHorizontal(new GUILayoutOption[0]);
            //        blurSpread.floatValue = EditorGUILayout.Slider("Blur Spread", blurSpread.floatValue, 0.01f, 10f, new GUILayoutOption[0]);
            //        if (GUILayout.Button("Reset", new GUILayoutOption[0]))
            //        {
            //            GUI.FocusControl("");
            //            blurSpread.floatValue = 0.6f;
            //        }
            //        GUILayout.EndHorizontal();
            //    }
            //    if (((target.blurMode == BlurMode.Default) || (target.blurMode == BlurMode.HighQuality)) /*|| (target.blurMode == BlurMode.Advanced)*/)
            //    {
            //        EditorGUILayout.PropertyField(baseResolution, new GUILayoutOption[0]);
            //    }
            //    if (target.blurMode == BlurMode.Default)
            //    {
            //        GUILayout.BeginHorizontal(new GUILayoutOption[0]);
            //        downsampleSteps.intValue = EditorGUILayout.IntSlider("Downsample Steps", downsampleSteps.intValue, 0, 10, new GUILayoutOption[0]);
            //        if (GUILayout.Button("Reset", new GUILayoutOption[0]))
            //        {
            //            GUI.FocusControl("");
            //            downsampleSteps.intValue = 2;
            //        }
            //        GUILayout.EndHorizontal();
            //        GUI.enabled = downsampleSteps.intValue >= 1;
            //        EditorGUILayout.PropertyField(downsampleResolution, new GUILayoutOption[0]);
            //        EditorGUILayout.PropertyField(downsampleBlendMode, new GUILayoutOption[0]);
            //        GUI.enabled = true;
            //    }
            //    //if (target.blurMode != BlurMode.Advanced)
            //    {
            //        GUILayout.BeginHorizontal(new GUILayoutOption[0]);
            //        innerStrength.floatValue = EditorGUILayout.Slider("Inner Strength", innerStrength.floatValue, 0.01f, 10f, new GUILayoutOption[0]);
            //        if (GUILayout.Button("Reset", new GUILayoutOption[0]))
            //        {
            //            GUI.FocusControl("");
            //            innerStrength.floatValue = 1f;
            //        }
            //        GUILayout.EndHorizontal();
            //    }
            //    if (target.blurMode == BlurMode.Default)
            //    {
            //        GUILayout.BeginHorizontal(new GUILayoutOption[0]);
            //        outerStrength.floatValue = EditorGUILayout.Slider("Outer Strength", outerStrength.floatValue, 0.01f, 10f, new GUILayoutOption[0]);
            //        if (GUILayout.Button("Reset", new GUILayoutOption[0]))
            //        {
            //            GUI.FocusControl("");
            //            outerStrength.floatValue = 1f;
            //        }
            //        GUILayout.EndHorizontal();
            //    }
            //    GUILayout.BeginHorizontal(new GUILayoutOption[0]);
            //    boostStrength.floatValue = EditorGUILayout.Slider("Boost Strength", boostStrength.floatValue, 0.01f, 10f, new GUILayoutOption[0]);
            //    if (GUILayout.Button("Reset", new GUILayoutOption[0]))
            //    {
            //        GUI.FocusControl("");
            //        boostStrength.floatValue = 1f;
            //    }
            //    GUILayout.EndHorizontal();
            //    EditorGUI.indentLevel--;
            //    EditorGUILayout.Space();
            //    EditorGUILayout.PropertyField(blendMode, new GUILayoutOption[0]);
            //}
            //else
            {
                if(!GlowManager.CheckSupportHightQualityGlow())
                {
                    EditorGUILayout.HelpBox("不支持高质量glow，使用普通glow.", MessageType.Warning);
                    return;
                }
                GUILayout.BeginHorizontal(new GUILayoutOption[0]);
                highQualityGlowIntensity.floatValue = EditorGUILayout.Slider("强度:", highQualityGlowIntensity.floatValue, 0.01f, 0.6f, new GUILayoutOption[0]);
                if (GUILayout.Button("Reset", new GUILayoutOption[0]))
                {
                    GUI.FocusControl("");
                    boostStrength.floatValue = 0.05f;
                }
                GUILayout.EndHorizontal();

                //GUILayout.BeginHorizontal(new GUILayoutOption[0]);
                //highQualityGlowBlurSize.floatValue = EditorGUILayout.Slider("光晕尺寸:", highQualityGlowBlurSize.floatValue, 0.1f, 10.0f, new GUILayoutOption[0]);
                //if (GUILayout.Button("Reset", new GUILayoutOption[0]))
                //{
                //    GUI.FocusControl("");
                //    boostStrength.floatValue = 4.0f;
                //}
                //GUILayout.EndHorizontal();
            }


            if ((serObj.ApplyModifiedProperties()) && EditorApplication.isPaused)
            {
                tar.InitCamera();
            }
        }
    }
}

