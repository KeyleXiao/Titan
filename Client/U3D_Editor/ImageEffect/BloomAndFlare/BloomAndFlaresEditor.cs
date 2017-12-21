//using System;
//using UnityEditor;
//using UnityEngine;

//namespace ImageEffects
//{
//    [CustomEditor(typeof(BloomAndFlares))]
//    class BloomAndFlaresEditor : Editor
//    {
//        BloomAndFlares bloomInstance = null;
//        GUIStyle m_Background1;
//        GUIStyle m_Background2;
//        GUIStyle m_Background3;
//        bool ShowBloomlayers = false;
//        AnimationCurve m_FilmicCurve;

//        private Texture2D MakeTex(int width, int height, Color col)
//        {
//            Color[] pix = new Color[width * height];

//            for (int i = 0; i < pix.Length; i++)
//                pix[i] = col;

//            Texture2D result = new Texture2D(width, height);
//            result.hideFlags = HideFlags.HideAndDontSave;
//            result.SetPixels(pix);
//            result.Apply();

//            return result;
//        }

//        public void OnEnable()
//        {
//            bloomInstance = (BloomAndFlares)target;

//            m_Background1 = new GUIStyle();
//            m_Background1.normal.background = MakeTex(600, 1, new Color(1.0f, 1.0f, 1.0f, 0.1f));
//            m_Background2 = new GUIStyle();
//            m_Background2.normal.background = MakeTex(600, 1, new Color(1.0f, 1.0f, 1.0f, 0.0f));
//            m_Background3 = new GUIStyle();
//            m_Background3.normal.background = MakeTex(600, 1, new Color(1.0f, 1.0f, 1.0f, 0.05f));

//        }

//        public void OnDisable()
//        {
//            GameObject.DestroyImmediate(m_Background1.normal.background);
//            GameObject.DestroyImmediate(m_Background2.normal.background);
//            GameObject.DestroyImmediate(m_Background3.normal.background);
//        }


//        public override void OnInspectorGUI()
//        {
//            Undo.RecordObject(bloomInstance, "Bloom DELUXE");

//            GUILayout.BeginVertical("Box");
//            GUILayout.Label("Bloom Settings", EditorStyles.boldLabel);
//            bloomInstance.m_InvertImage = EditorGUILayout.Toggle("Flip Image", bloomInstance.m_InvertImage);


//            //Bloom选项
//            //bloomInstance.m_BloomIntensity = DoSlider("Bloom Master Intensity", bloomInstance.m_BloomIntensity, 0.0f, 5.0f);
//            bloomInstance.m_BloomIntensity = DofloatField("Bloom Master Intensity", bloomInstance.m_BloomIntensity);
//            bloomInstance.m_DownscaleCount = Mathf.Clamp(EditorGUILayout.IntField("Layers (Downscale Count)", bloomInstance.m_DownscaleCount), 1, 6);
//            if (GUILayout.Button((ShowBloomlayers ? "Hide Layers" : "Show Layers") + "[" + bloomInstance.m_DownscaleCount + "]"))
//            {
//                ShowBloomlayers = !ShowBloomlayers;
//            }


//            if (ShowBloomlayers)
//            {

//                for (int i = 0; i < bloomInstance.m_DownscaleCount; ++i)
//                {
//                    GUILayout.BeginVertical(i % 2 == 0 ? m_Background3 : m_Background1);
//                    int idx = i + 1;
//                    bloomInstance.m_BloomUsages[i] = EditorGUILayout.Toggle("  Layer " + idx + " Enabled", bloomInstance.m_BloomUsages[i]);
//                    bloomInstance.m_BloomIntensities[i] = DofloatField("  Layer " + idx + " Intensity", bloomInstance.m_BloomIntensities[i]);
//                    bloomInstance.m_BloomColors[i] = EditorGUILayout.ColorField("  Layer " + idx + " Tint", bloomInstance.m_BloomColors[i]);
//                    GUILayout.EndVertical();
//                }
//            }
//            GUILayout.Space(10.0f);
//            GUILayout.EndVertical();


//            //强度控制
//            GUILayout.BeginVertical("Box");
//            GUILayout.Label("Intensity Settings", EditorStyles.boldLabel);
//            BloomAndFlares.BloomIntensityType lastIm = bloomInstance.m_IntensityType;
//            bloomInstance.m_IntensityType = (BloomAndFlares.BloomIntensityType)EditorGUILayout.EnumPopup("Intensity Function", bloomInstance.m_IntensityType);
//            if (bloomInstance.m_IntensityType == BloomAndFlares.BloomIntensityType.Threshold)
//            {
//                bloomInstance.m_BloomThreshhold = DoSlider(" Threshold", bloomInstance.m_BloomThreshhold, 0.0f, 5.0f);
//            }
//            else
//            {
//                DrawFilmicCurve(bloomInstance.m_BloomCurve);
//            }
//            if (lastIm != bloomInstance.m_IntensityType)
//            {
//                bloomInstance.ReLoadRes();
//            }

//            GUILayout.Space(10.0f);
//            GUILayout.EndVertical();


//            if (GUI.changed)
//            {
//                EditorUtility.SetDirty(target);
//            }
//        }



//        float DoSlider(string label, float value, float min, float max)
//        {
//            float v = value;
//            EditorGUILayout.BeginHorizontal();
//            v = Mathf.Clamp(EditorGUILayout.FloatField(label, v, GUILayout.ExpandWidth(false)), min, max);
//            v = GUILayout.HorizontalSlider(v, min, max);
//            EditorGUILayout.EndHorizontal();

//            return v;
//        }

//        float DofloatField(string label, float value)
//        {
//            float v = value;
//            EditorGUILayout.BeginHorizontal();
//            v = EditorGUILayout.FloatField(label, v, GUILayout.ExpandWidth(false));
//            EditorGUILayout.EndHorizontal();

//            return v;
//        }


//        void DrawFilmicCurve(FilmicCurve FilmicCurveInstance)
//        {
//            SetupFilmicCurve(FilmicCurveInstance);

//            float denom = FilmicCurveInstance.m_WhitePoint - FilmicCurveInstance.m_BlackPoint;

//            float co = (FilmicCurveInstance.m_CrossOverPoint - FilmicCurveInstance.m_BlackPoint) / denom;
//            if (Mathf.Abs(denom) < 0.001f)
//                co = 0.5f;

//            FilmicCurveInstance.m_BlackPoint = DoSlider("  Minimum Intensity", FilmicCurveInstance.m_BlackPoint, 0.0f, 10.0f);
//            FilmicCurveInstance.m_WhitePoint = DoSlider("  Maximum Intensity", FilmicCurveInstance.m_WhitePoint, 0.0f, 10.0f);
//            co = DoSlider("  Mid-value range", co, 0.0f, 1.0f);
//            FilmicCurveInstance.m_ToeStrength = -1.0f * DoSlider("  Dark Colors Intensity", -1.0f * FilmicCurveInstance.m_ToeStrength, -0.99f, 0.99f);
//            FilmicCurveInstance.m_ShoulderStrength = DoSlider("  Bright Colors Intensity", FilmicCurveInstance.m_ShoulderStrength, -0.99f, 0.99f);

//            Rect r = EditorGUILayout.BeginVertical(GUILayout.MinHeight(60));
//            Rect rr = GUILayoutUtility.GetRect(Mathf.Min(r.width, 60), 60);

//            EditorGUI.CurveField(rr, m_FilmicCurve);
//            EditorGUILayout.EndVertical();

//            FilmicCurveInstance.m_CrossOverPoint = co * (FilmicCurveInstance.m_WhitePoint - FilmicCurveInstance.m_BlackPoint) + FilmicCurveInstance.m_BlackPoint;
//            FilmicCurveInstance.UpdateCoefficients();
//        }


//        private static float CalculateLinearTangent(AnimationCurve curve, int index, int toIndex)
//        {
//            return (float)(((double)curve[index].value - (double)curve[toIndex].value) / ((double)curve[index].time - (double)curve[toIndex].time));
//        }

//        void SetupFilmicCurve(FilmicCurve FilmicCurveInstance)
//        {
//            m_FilmicCurve = new AnimationCurve();

//            FilmicCurve dt = FilmicCurveInstance;

//            float min = dt.m_BlackPoint;
//            float max = dt.m_WhitePoint;

//            int nbFrame = 40;
//            float step = (max - min) / nbFrame;

//            float curr = min;
//            float k = dt.ComputeK(dt.m_ToeStrength, dt.m_CrossOverPoint, dt.m_BlackPoint, dt.m_ShoulderStrength, dt.m_WhitePoint);

//            dt.StoreK();
//            dt.ComputeShaderCoefficients(dt.m_ToeStrength, dt.m_CrossOverPoint, dt.m_BlackPoint, dt.m_ShoulderStrength, dt.m_WhitePoint, k);

//            for (int i = 0; i < nbFrame + 1; ++i)
//            {
//                float value = dt.Graph(curr, dt.m_ToeStrength, dt.m_CrossOverPoint, dt.m_BlackPoint, dt.m_ShoulderStrength, dt.m_WhitePoint, k);
//                m_FilmicCurve.AddKey(new Keyframe(curr, value));

//                curr += step;
//            }

//            for (int i = 0; i < m_FilmicCurve.keys.Length - 1; ++i)
//            {
//                float tangent = CalculateLinearTangent(m_FilmicCurve, i, i + 1);
//                m_FilmicCurve.keys[i].inTangent = tangent;
//                m_FilmicCurve.keys[i].outTangent = tangent;

//                m_FilmicCurve.SmoothTangents(i, 0.0f);
//            }
//        }
//    }

//    //[CustomEditor (typeof(BloomAndFlares))]
//    //class BloomAndFlaresEditor : Editor
//    //{
//    //    SerializedProperty tweakMode;
//    //    SerializedProperty screenBlendMode;

//    //    SerializedObject serObj;

//    //    SerializedProperty hdr;
//    //    SerializedProperty sepBlurSpread;
//    //    SerializedProperty useSrcAlphaAsMask;

//    //    SerializedProperty bloomIntensity;
//    //    SerializedProperty bloomthreshold;
//    //    SerializedProperty bloomBlurIterations;

//    //    SerializedProperty lensflares;

//    //    SerializedProperty hollywoodFlareBlurIterations;

//    //    SerializedProperty lensflareMode;
//    //    SerializedProperty hollyStretchWidth;
//    //    SerializedProperty lensflareIntensity;
//    //    SerializedProperty lensflarethreshold;
//    //    SerializedProperty flareColorA;
//    //    SerializedProperty flareColorB;
//    //    SerializedProperty flareColorC;
//    //    SerializedProperty flareColorD;

//    //    SerializedProperty lensFlareVignetteMask;

//    //    public void OnEnable () {
//    //        serObj = new SerializedObject (target);

//    //        screenBlendMode = serObj.FindProperty("screenBlendMode");
//    //        hdr = serObj.FindProperty("hdr");

//    //        sepBlurSpread = serObj.FindProperty("sepBlurSpread");
//    //        useSrcAlphaAsMask = serObj.FindProperty("useSrcAlphaAsMask");

//    //        bloomIntensity = serObj.FindProperty("bloomIntensity");
//    //        bloomthreshold = serObj.FindProperty("bloomThreshold");
//    //        bloomBlurIterations = serObj.FindProperty("bloomBlurIterations");

//    //        lensflares = serObj.FindProperty("lensflares");

//    //        lensflareMode = serObj.FindProperty("lensflareMode");
//    //        hollywoodFlareBlurIterations = serObj.FindProperty("hollywoodFlareBlurIterations");
//    //        hollyStretchWidth = serObj.FindProperty("hollyStretchWidth");
//    //        lensflareIntensity = serObj.FindProperty("lensflareIntensity");
//    //        lensflarethreshold = serObj.FindProperty("lensflareThreshold");
//    //        flareColorA = serObj.FindProperty("flareColorA");
//    //        flareColorB = serObj.FindProperty("flareColorB");
//    //        flareColorC = serObj.FindProperty("flareColorC");
//    //        flareColorD = serObj.FindProperty("flareColorD");
//    //        lensFlareVignetteMask = serObj.FindProperty("lensFlareVignetteMask");

//    //        tweakMode = serObj.FindProperty("tweakMode");
//    //    }


//    //    public override void OnInspectorGUI () {
//    //        serObj.Update();

//    //        GUILayout.Label("HDR " + (hdr.enumValueIndex == 0 ? "auto detected, " : (hdr.enumValueIndex == 1 ? "forced on, " : "disabled, ")) + (useSrcAlphaAsMask.floatValue < 0.1f ? " ignoring alpha channel glow information" : " using alpha channel glow information"), EditorStyles.miniBoldLabel);

//    //        EditorGUILayout.PropertyField (tweakMode, new GUIContent("Tweak mode"));
//    //        EditorGUILayout.PropertyField (screenBlendMode, new GUIContent("Blend mode"));
//    //        EditorGUILayout.PropertyField (hdr, new GUIContent("HDR"));

//    //        // display info text when screen blend mode cannot be used
//    //        Camera cam = (target as BloomAndFlares).GetComponent<Camera>();
//    //        if (cam != null) {
//    //            if (screenBlendMode.enumValueIndex==0 && ((cam.hdr && hdr.enumValueIndex==0) || (hdr.enumValueIndex==1))) {
//    //                EditorGUILayout.HelpBox("Screen blend is not supported in HDR. Using 'Add' instead.", MessageType.Info);
//    //            }
//    //        }

//    //        if (1 == tweakMode.intValue)
//    //            EditorGUILayout.PropertyField (lensflares, new GUIContent("Cast lens flares"));

//    //        EditorGUILayout.Separator ();

//    //        EditorGUILayout.PropertyField (bloomIntensity, new GUIContent("Intensity"));
//    //        bloomthreshold.floatValue = EditorGUILayout.Slider ("threshold", bloomthreshold.floatValue, -0.05f, 4.0f);
//    //        bloomBlurIterations.intValue = EditorGUILayout.IntSlider ("Blur iterations", bloomBlurIterations.intValue, 1, 4);
//    //        sepBlurSpread.floatValue = EditorGUILayout.Slider ("Blur spread", sepBlurSpread.floatValue, 0.1f, 10.0f);

//    //        if (1 == tweakMode.intValue)
//    //            useSrcAlphaAsMask.floatValue = EditorGUILayout.Slider (new  GUIContent("Use alpha mask", "Make alpha channel define glowiness"), useSrcAlphaAsMask.floatValue, 0.0f, 1.0f);
//    //        else
//    //            useSrcAlphaAsMask.floatValue = 0.0f;

//    //        if (1 == tweakMode.intValue) {
//    //            EditorGUILayout.Separator ();

//    //            if (lensflares.boolValue) {

//    //                // further lens flare tweakings
//    //                if (0 != tweakMode.intValue)
//    //                    EditorGUILayout.PropertyField (lensflareMode, new GUIContent("Lens flare mode"));
//    //                else
//    //                    lensflareMode.enumValueIndex = 0;

//    //                EditorGUILayout.PropertyField(lensFlareVignetteMask, new GUIContent("Lens flare mask", "This mask is needed to prevent lens flare artifacts"));

//    //                EditorGUILayout.PropertyField (lensflareIntensity, new GUIContent("Local intensity"));
//    //                lensflarethreshold.floatValue = EditorGUILayout.Slider ("Local threshold", lensflarethreshold.floatValue, 0.0f, 1.0f);

//    //                if (lensflareMode.intValue == 0) {
//    //                    // ghosting
//    //                    EditorGUILayout.BeginHorizontal ();
//    //                    EditorGUILayout.PropertyField (flareColorA, new GUIContent("1st Color"));
//    //                    EditorGUILayout.PropertyField (flareColorB, new GUIContent("2nd Color"));
//    //                    EditorGUILayout.EndHorizontal ();

//    //                    EditorGUILayout.BeginHorizontal ();
//    //                    EditorGUILayout.PropertyField (flareColorC, new GUIContent("3rd Color"));
//    //                    EditorGUILayout.PropertyField (flareColorD, new GUIContent("4th Color"));
//    //                    EditorGUILayout.EndHorizontal ();
//    //                }
//    //                else if (lensflareMode.intValue == 1) {
//    //                    // hollywood
//    //                    EditorGUILayout.PropertyField (hollyStretchWidth, new GUIContent("Stretch width"));
//    //                    hollywoodFlareBlurIterations.intValue = EditorGUILayout.IntSlider ("Blur iterations", hollywoodFlareBlurIterations.intValue, 1, 4);

//    //                    EditorGUILayout.PropertyField (flareColorA, new GUIContent("Tint Color"));
//    //                }
//    //                else if (lensflareMode.intValue == 2) {
//    //                    // both
//    //                    EditorGUILayout.PropertyField (hollyStretchWidth, new GUIContent("Stretch width"));
//    //                    hollywoodFlareBlurIterations.intValue = EditorGUILayout.IntSlider ("Blur iterations", hollywoodFlareBlurIterations.intValue, 1, 4);

//    //                    EditorGUILayout.BeginHorizontal ();
//    //                    EditorGUILayout.PropertyField (flareColorA, new GUIContent("1st Color"));
//    //                    EditorGUILayout.PropertyField (flareColorB, new GUIContent("2nd Color"));
//    //                    EditorGUILayout.EndHorizontal ();

//    //                    EditorGUILayout.BeginHorizontal ();
//    //                    EditorGUILayout.PropertyField (flareColorC, new GUIContent("3rd Color"));
//    //                    EditorGUILayout.PropertyField (flareColorD, new GUIContent("4th Color"));
//    //                    EditorGUILayout.EndHorizontal ();
//    //                }
//    //            }
//    //        } else
//    //            lensflares.boolValue = false; // disable lens flares in simple tweak mode

//    //        serObj.ApplyModifiedProperties();
//    //    }
//    //}
//}
