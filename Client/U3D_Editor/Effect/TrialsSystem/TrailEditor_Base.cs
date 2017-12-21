using System.Collections.Generic;
using UnityEditor;
using UnityEngine;


namespace Effect
{
    namespace TrailsSystem
    {
        [CustomEditor(typeof(Effect_TrailRenderer_Base))]
        public class TrailEditor_Base : UnityEditor.Editor
        {


            public static TrailPreviewUtillity win;

            protected List<string> ExcludedDefaultPropties = new List<string>();
            private Effect_TrailRenderer_Base m_Instance;

            public void OnEnable()
            {
                m_Instance = target as Effect_TrailRenderer_Base;
            }

            public override void OnInspectorGUI()
            {


                float defaultLabelWidth = EditorGUIUtility.labelWidth;
                float defaultFieldWidth = EditorGUIUtility.fieldWidth;
                GUILayout.Space(5);

                GUILayout.BeginVertical();

                if (!ExcludedDefaultPropties.Contains("TrailData.Lifetime"))
                {
                    EditorGUILayout.PropertyField(serializedObject.FindProperty("TrailData.Lifetime"));
                }

                EditorGUIUtility.labelWidth = defaultLabelWidth;
                EditorGUIUtility.fieldWidth = defaultFieldWidth;


                GUILayout.BeginHorizontal();
                EditorGUIUtility.fieldWidth = defaultLabelWidth - 80;
                if (!ExcludedDefaultPropties.Contains("TrailData.TrailMaterial"))
                {
                    EditorGUILayout.PropertyField(serializedObject.FindProperty("TrailData.TrailMaterial"));
                }
                GUILayout.Space(10);
                EditorGUIUtility.labelWidth = 30;
                EditorGUIUtility.fieldWidth = 40;
                if (!ExcludedDefaultPropties.Contains("TrailData.MaterialTileLength"))
                {
                    EditorGUILayout.PropertyField(serializedObject.FindProperty("TrailData.MaterialTileLength"), new GUIContent("Tile"), GUILayout.Width(70));
                }
                GUILayout.EndHorizontal();

                EditorGUIUtility.labelWidth = defaultLabelWidth;
                EditorGUIUtility.fieldWidth = defaultFieldWidth;


                GUILayout.BeginHorizontal();
                if (!ExcludedDefaultPropties.Contains("TrailData.SizeOverLife"))
                {
                    if (m_Instance.TrailData.UsingSimpleSize)
                    {
                        EditorGUILayout.LabelField("Size Over Life", GUILayout.Width(defaultLabelWidth));
                        serializedObject.FindProperty("TrailData.SimpleSizeOverLifeStart").floatValue =
                            EditorGUILayout.FloatField(serializedObject.FindProperty("TrailData.SimpleSizeOverLifeStart").floatValue);
                        serializedObject.FindProperty("TrailData.SimpleSizeOverLifeEnd").floatValue =
                            EditorGUILayout.FloatField(serializedObject.FindProperty("TrailData.SimpleSizeOverLifeEnd").floatValue);

                    }
                    else
                    {
                        EditorGUIUtility.fieldWidth = defaultLabelWidth - 80;

                        EditorGUILayout.PropertyField(serializedObject.FindProperty("TrailData.SizeOverLife"));
                    }

                }

                GUILayout.Space(10);
                EditorGUIUtility.labelWidth = 50;
                EditorGUIUtility.fieldWidth = 20;
                if (!ExcludedDefaultPropties.Contains("TrailData.StretchSizeToFit"))
                {
                    EditorGUILayout.PropertyField(serializedObject.FindProperty("TrailData.StretchSizeToFit"), new GUIContent("Stretch"), GUILayout.Width(70));
                }
                GUILayout.EndHorizontal();

                EditorGUIUtility.labelWidth = defaultLabelWidth;
                EditorGUIUtility.fieldWidth = defaultFieldWidth;

                GUILayout.BeginHorizontal();
                EditorGUIUtility.fieldWidth = defaultLabelWidth - 80;

                if (!ExcludedDefaultPropties.Contains("TrailData.ColorOverLife"))
                {
                    if (m_Instance.TrailData.UsingSimpleColor)
                    {
                        EditorGUIUtility.fieldWidth = (defaultFieldWidth - 80) / 2f;

                        EditorGUILayout.LabelField("Color Over Life", GUILayout.Width(defaultLabelWidth));
                        serializedObject.FindProperty("TrailData.SimpleColorOverLifeStart").colorValue =
                            EditorGUILayout.ColorField(serializedObject.FindProperty("TrailData.SimpleColorOverLifeStart").colorValue);
                        serializedObject.FindProperty("TrailData.SimpleColorOverLifeEnd").colorValue =
                            EditorGUILayout.ColorField(serializedObject.FindProperty("TrailData.SimpleColorOverLifeEnd").colorValue);

                    }
                    else
                    {
                        EditorGUIUtility.fieldWidth = defaultLabelWidth - 80;

                        EditorGUILayout.PropertyField(serializedObject.FindProperty("TrailData.ColorOverLife"));
                    }

                }
                GUILayout.Space(10);
                EditorGUIUtility.labelWidth = 50;
                EditorGUIUtility.fieldWidth = 20;
                if (!ExcludedDefaultPropties.Contains("TrailData.StretchColorToFit"))
                {
                    EditorGUILayout.PropertyField(serializedObject.FindProperty("TrailData.StretchColorToFit"), new GUIContent("Stretch"), GUILayout.Width(70));
                }
                GUILayout.EndHorizontal();
                EditorGUIUtility.labelWidth = defaultLabelWidth;
                EditorGUIUtility.fieldWidth = defaultFieldWidth;

                if (!ExcludedDefaultPropties.Contains("TrailData.UseForwardOverride"))
                {
                    EditorGUILayout.PropertyField(serializedObject.FindProperty("TrailData.UseForwardOverride"));
                }
                if (m_Instance.TrailData.UseForwardOverride)
                {
                    EditorGUI.indentLevel++;

                    if (!ExcludedDefaultPropties.Contains("TrailData.ForwardOverride"))
                    {
                        EditorGUILayout.PropertyField(serializedObject.FindProperty("TrailData.ForwardOverride"));
                    }

                    if (!ExcludedDefaultPropties.Contains("TrailData.ForwardOverrideRelative"))
                    {
                        EditorGUILayout.PropertyField(serializedObject.FindProperty("TrailData.ForwardOverrideRelative"), new GUIContent("Override Relative"));
                    }

                    EditorGUI.indentLevel--;
                }


                DrawTrailSpecificGUI();

                m_Instance.UVScroll = EditorGUILayout.ObjectField("UV流动控制器:", m_Instance.UVScroll, typeof(Effect_UVScroll), true) as Effect_UVScroll;
                m_Instance.UVTextureAnimator = EditorGUILayout.ObjectField("UV动画控制器:", m_Instance.UVTextureAnimator, typeof(Effect_UVTextureAnimator), true) as Effect_UVTextureAnimator;
                m_Instance.FadeInOutMaterialFloat = EditorGUILayout.ObjectField("浮点数控制器:", m_Instance.FadeInOutMaterialFloat, typeof(Effect_FadeInOutMaterialFloat), true) as Effect_FadeInOutMaterialFloat;
                m_Instance.FadeInOutMaterialColor = EditorGUILayout.ObjectField("颜色控制器:", m_Instance.FadeInOutMaterialColor, typeof(Effect_FadeInOutMaterialColor), true) as Effect_FadeInOutMaterialColor;
                m_Instance.GradientColor = EditorGUILayout.ObjectField("颜色控制器(高级):", m_Instance.GradientColor, typeof(Effect_MaterialGradientColor), true) as Effect_MaterialGradientColor;
                if (!ExcludedDefaultPropties.Contains("Emit"))
                {
                    EditorGUILayout.PropertyField(serializedObject.FindProperty("Emit"));
                }

                GUILayout.EndVertical();




                serializedObject.ApplyModifiedProperties();
                serializedObject.UpdateIfDirtyOrScript();


                GUILayout.Space(5);
                if (GUILayout.Button("Open preview"))
                {
                    // Get existing open window or if none, make a new one:
                    win = (TrailPreviewUtillity)EditorWindow.GetWindow(typeof(TrailPreviewUtillity), true, "Normalized Trail Preview");
                    win.minSize = new Vector2(900, 140);
                    win.maxSize = new Vector2(900, 140);
                    win.Trail = m_Instance;
                }

            }

            protected virtual void DrawTrailSpecificGUI()
            { }
        }
    }
    
}
