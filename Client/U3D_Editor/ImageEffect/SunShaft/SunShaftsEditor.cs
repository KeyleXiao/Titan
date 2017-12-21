using System;
using UnityEditor;
using UnityEngine;

namespace ImageEffects
{
    [CustomEditor(typeof(SunShafts))]
    class SunShaftsEditor : Editor
    {
        SerializedObject serObj;

        SerializedProperty sunTransform;
        SerializedProperty radialBlurIterations;
        SerializedProperty sunColor;
        SerializedProperty sunThreshold;
        SerializedProperty sunShaftBlurRadius;
        SerializedProperty sunShaftIntensity;
        SerializedProperty resolution;
        SerializedProperty maxRadius;

        SerializedProperty maxDistance;

        SerializedProperty skyMask;

        SerializedProperty area_u0;
        SerializedProperty area_u1;
        SerializedProperty area_Center;
        SerializedProperty area_e0;
        SerializedProperty area_e1;
        SerializedProperty hasArea;

        public static bool isShow = false;
        public static GameObject areaObj = null;

        public void OnEnable()
        {
            serObj = new SerializedObject(target);


            sunTransform = serObj.FindProperty("sunTransform");
            sunColor = serObj.FindProperty("sunColor");
            sunThreshold = serObj.FindProperty("sunThreshold");

            sunShaftBlurRadius = serObj.FindProperty("sunShaftBlurRadius");
            radialBlurIterations = serObj.FindProperty("radialBlurIterations");

            sunShaftIntensity = serObj.FindProperty("sunShaftIntensity");

            resolution = serObj.FindProperty("resolution");

            maxRadius = serObj.FindProperty("maxRadius");

            maxDistance = serObj.FindProperty("maxDistance");

            skyMask = serObj.FindProperty("skyMask");

            hasArea = serObj.FindProperty("hasArea");

            area_u0 = serObj.FindProperty("area_u0");
            area_u1 = serObj.FindProperty("area_u1");
            area_Center = serObj.FindProperty("area_Center");
            area_e0 = serObj.FindProperty("area_e0");
            area_e1 = serObj.FindProperty("area_e1");
        }

        public override void OnInspectorGUI()
        {
            serObj.Update();

            EditorGUILayout.BeginHorizontal();

            EditorGUILayout.EndHorizontal();

            EditorGUILayout.PropertyField(resolution, new GUIContent("Resolution"));

            EditorGUILayout.Separator();

            EditorGUILayout.BeginHorizontal();

            EditorGUILayout.PropertyField(sunTransform, new GUIContent("Shafts caster", "Chose a transform that acts as a root point for the produced sun shafts"));

            EditorGUILayout.EndHorizontal();

            EditorGUILayout.Separator();

            EditorGUILayout.PropertyField(sunThreshold, new GUIContent("Threshold color"));
            EditorGUILayout.PropertyField(sunColor, new GUIContent("Shafts color"));
            maxRadius.floatValue = 1.0f - EditorGUILayout.Slider("Max falloff radius", 1.0f - maxRadius.floatValue, 0.1f, 1.0f);

            maxDistance.floatValue = EditorGUILayout.Slider("Max falloff distance", maxDistance.floatValue, 0.1f, 50.0f);

            EditorGUILayout.Separator();

            sunShaftBlurRadius.floatValue = EditorGUILayout.Slider("Blur size", sunShaftBlurRadius.floatValue, 1.0f, 10.0f);
            radialBlurIterations.intValue = EditorGUILayout.IntSlider("Blur iterations", radialBlurIterations.intValue, 1, 3);

            EditorGUILayout.Separator();

            EditorGUILayout.PropertyField(sunShaftIntensity, new GUIContent("Intensity"));

            EditorGUILayout.PropertyField(skyMask, new GUIContent("Sky Mask", "sun shaft mask"));

            hasArea.boolValue = EditorGUILayout.Toggle("need Area£∫", hasArea.boolValue);
            isShow = EditorGUILayout.Toggle("is Show Area£∫", isShow);
            if (hasArea.boolValue && isShow)
            {

                if (areaObj == null)
                {
                    areaObj = GameObject.CreatePrimitive(PrimitiveType.Cube);
                    areaObj.name = "SunShaftGameObj";
                    //areaObj.transform.parent = Camera.main.transform;
                    areaObj.hideFlags = HideFlags.DontSaveInBuild;
                    areaObj.GetComponent<MeshRenderer>().enabled = false;

                    Collider c = areaObj.AddComponent<BoxCollider>();
                    c.hideFlags = HideFlags.HideAndDontSave;
                    SetTranByOBB(areaObj.transform);
                }
                else
                {
                    EditorGUILayout.ObjectField(areaObj, typeof(GameObject), false);
                    if (GUILayout.Button("±£¥Ê«¯”Ú"))
                    {
                        SetAreaOBBByTran(areaObj.transform);
                        isShow = false;
                    }
                }

            }
            else
            {
                if (areaObj != null)
                {
                    DestroyImmediate(areaObj);
                    areaObj = null;
                }
            }
            serObj.ApplyModifiedProperties();
        }

        private void SetAreaOBBByTran(Transform tran)
        {
            area_u0.vector3Value = Quaternion.AngleAxis(tran.eulerAngles.y, Vector3.up) * Vector2.right;
            area_u1.vector3Value = Quaternion.AngleAxis(tran.eulerAngles.y - 90, Vector3.up) * Vector2.right;
            area_Center.vector3Value = tran.position;
            area_e0.floatValue = tran.localScale.x / 2;
            area_e1.floatValue = tran.localScale.z / 2;
        }

        private void SetTranByOBB(Transform tran)
        {
            tran.localScale = new Vector3(area_e0.floatValue * 2, 1, area_e1.floatValue * 2);
            tran.position = area_Center.vector3Value;
            tran.eulerAngles = Vector3.up * Vector3.Angle(area_u0.vector3Value, Vector2.right);
        }
    }
}
