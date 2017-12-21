using System;
using UnityEditor;
using UnityEngine;
namespace ImageEffects
{
    /// <summary>
    /// GlowManager编辑器
    /// </summary>
    [CustomEditor(typeof(DissolveGlowManager))]
    public class DissolveManagerEditor : Editor
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

        DissolveGlowManager tar;
        public void OnEnable()
        {
            tar = target as DissolveGlowManager;
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
        }

        public override void OnInspectorGUI()
        {
            DissolveGlowManager target = tar;
            serObj.Update();
            if (!PlayerSettings.use32BitDisplayBuffer)
            {
                EditorGUILayout.HelpBox("It is recommended you use a 32-bit display buffer (can be set in the player settings).", MessageType.Warning);
            }

            target.DebugMode = EditorGUILayout.Toggle("调试模式", target.DebugMode, new GUILayoutOption[0]);
            if (target.DebugMode)
            {
                EditorGUILayout.LabelField("调试模式紧紧是给特效美术使用，\n其他情况请关闭！", GUILayout.Height(30));
            }
  
            GUILayout.BeginHorizontal(new GUILayoutOption[0]);
            innerStrength.floatValue = EditorGUILayout.Slider("内核强度:", innerStrength.floatValue, 0.01f, 10f, new GUILayoutOption[0]);
            if (GUILayout.Button("Reset", new GUILayoutOption[0]))
            {
                GUI.FocusControl("");
                innerStrength.floatValue = 1f;
            }
            GUILayout.EndHorizontal();

            if (target.blurMode == BlurMode.Default)
            {
                GUILayout.BeginHorizontal(new GUILayoutOption[0]);
                outerStrength.floatValue = EditorGUILayout.Slider("外核强度:", outerStrength.floatValue, 0.01f, 10f, new GUILayoutOption[0]);
                if (GUILayout.Button("Reset", new GUILayoutOption[0]))
                {
                    GUI.FocusControl("");
                    outerStrength.floatValue = 1f;
                }
                GUILayout.EndHorizontal();
            }
            GUILayout.BeginHorizontal(new GUILayoutOption[0]);
            boostStrength.floatValue = EditorGUILayout.Slider("总体强度:", boostStrength.floatValue, 0.01f, 10f, new GUILayoutOption[0]);
            if (GUILayout.Button("Reset", new GUILayoutOption[0]))
            {
                GUI.FocusControl("");
                boostStrength.floatValue = 1f;
            }
            GUILayout.EndHorizontal();
            EditorGUILayout.Space();

            if ((serObj.ApplyModifiedProperties()) && EditorApplication.isPaused)
            {
                tar.InitCamera();
            }
        }
    }
}

