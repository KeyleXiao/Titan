//// CANDELA-SSRR SCREEN SPACE RAYTRACED REFLECTIONS
//// Copyright 2014 Livenda

//using System;
//using UnityEngine;


//namespace ImageEffects
//{

//    [ExecuteInEditMode]
//    [RequireComponent(typeof(Camera))]
//    public class CandelaSSRREx : ImageEffectNode
//    {
//        //-------Public Parameter-------------

//        [Range(0.0f, 4.0f)]
//        public float Strength=1.0f;

//        [Range(0.1f, 40.0f)]
//        public float GlobalScale = 10f;
//        [Range(1, 100)]
//        public int maxGlobalStep = 20;
//        [Range(1, 40)]
//        public int maxFineStep = 20;
//        [Range(0f, 0.001f)]
//        public float bias = 0.0001f;
//        [Range(0.02f, 12.0f)]
//        public float fadePower = 1.0f;
//        [Range(0f, 1f)]
//        public float maxDepthCull = 0.5f;
//        [Range(0f, 1f)]
//        public float reflectionMultiply = 1f;
//        [Range(0.0f, 10f)]
//        public float GlobalBlurRadius = 2.0f;
//        [Range(0.0f, 8f)]
//        public float DistanceBlurRadius = 4.0f;
//        [Range(0.0f, 10f)]
//        public float DistanceBlurStart = 3.0f;
//        [Range(0.0f, 1.0f)]
//        public float GrazeBlurPower = 0.0f;
//        public bool BlurQualityHigh = false;
//        [Range(1, 5)]
//        public int HQ_BlurIterations = 2;
//        public float HQ_DepthSensetivity = 0.42f;
//        public float HQ_NormalsSensetivity = 1.0f;
//        public bool ResolutionOptimized = false;
//        //ScreenFade Related Controls
//        public float DebugScreenFade = 0.0f;
//        [Range(0.0f, 10.0f)]
//        public float ScreenFadePower = 1.0f;
//        [Range(0.0f, 3.0f)]
//        public float ScreenFadeSpread = 0.7f;
//        [Range(0.0f, 4.0f)]
//        public float ScreenFadeEdge = 2.2f;
//        public float UseEdgeTexture = 0.0f;
//        public Texture2D EdgeFadeTexture;
//        public float SSRRcomposeMode = 1.0f;
//        public bool HDRreflections = false;
//        public bool UseCustomDepth = false;
//        public bool InvertRoughness = false;

//        //------Internal Use-------------------
//        private Shader CustomDepth_SHADER;
//        private Shader CustomNormal_SHADER;
//        private Camera RTcustom_CAMERA;
//        private Material SSRR_MATERIAL;
//        private Material POST_COMPOSE_MATERIAL;
//        private Material BLUR_MATERIALX;
//        private Material BLUR_MATERIALY;
//        private Material BLUR_MATERIALX_EA;
//        private Material BLUR_MATERIALY_EA;

//        private int validShaderLOD = 400;

//        public override bool IsOnValidLOD()
//        {
//            return ImageEffectManager.ImageEffectShaderLOD >= validShaderLOD;
//        }

//        private static void DestroyMaterial(Material mat)
//        {
//            if (mat)
//            {
//                DestroyImmediate(mat);
//                mat = null;
//            }
//        }

//        public override void Awake()
//        {
//            base.Awake();

//            CustomDepth_SHADER = ImageEffectManager.API_GetShader("Hidden/CustomDepthSSRR");
//            CustomNormal_SHADER = ImageEffectManager.API_GetShader("Hidden/CandelaWorldNormal");
//        }


//        public override void OnActive()
//        {
//            API_AttachCameraRenderFlags(DepthTextureMode.DepthNormals);

//            if (!RTcustom_CAMERA)
//            {
//                //Render Cam
//                GameObject go = new GameObject("RenderCamPos", typeof(Camera));
//                go.hideFlags = HideFlags.HideAndDontSave;
//                RTcustom_CAMERA = go.GetComponent<Camera>();
//                RTcustom_CAMERA.CopyFrom(m_Camera);
//                RTcustom_CAMERA.clearFlags = CameraClearFlags.Color;
//                RTcustom_CAMERA.renderingPath = RenderingPath.Forward;
//                RTcustom_CAMERA.backgroundColor = new Color(0, 0, 0, 0);
//                RTcustom_CAMERA.enabled = false;
//            }

//            //Create The Materials Required
//            SSRR_MATERIAL = new Material(ImageEffectManager.API_GetShader("Hidden/CandelaSSRRv1"));
//            SSRR_MATERIAL.hideFlags = HideFlags.HideAndDontSave;
//            POST_COMPOSE_MATERIAL = new Material(ImageEffectManager.API_GetShader("Hidden/CandelaComposeEx"));
//            POST_COMPOSE_MATERIAL.hideFlags = HideFlags.HideAndDontSave;
//            BLUR_MATERIALX = new Material(ImageEffectManager.API_GetShader("Hidden/CanBlurX"));
//            BLUR_MATERIALX.hideFlags = HideFlags.HideAndDontSave;
//            BLUR_MATERIALY = new Material(ImageEffectManager.API_GetShader("Hidden/CanBlurY"));
//            BLUR_MATERIALY.hideFlags = HideFlags.HideAndDontSave;
//            BLUR_MATERIALX_EA = new Material(ImageEffectManager.API_GetShader("Hidden/dephNormBlurX"));
//            BLUR_MATERIALX_EA.hideFlags = HideFlags.HideAndDontSave;
//            BLUR_MATERIALY_EA = new Material(ImageEffectManager.API_GetShader("Hidden/dephNormBlurY"));
//            BLUR_MATERIALY_EA.hideFlags = HideFlags.HideAndDontSave;


//        }

//        public override void OnDeActive()
//        {
//            DeAttachAllCameraRenderFlags();
//            DestroyImmediate(RTcustom_CAMERA);
//            DestroyMaterial(SSRR_MATERIAL);
//            DestroyMaterial(POST_COMPOSE_MATERIAL);
//            DestroyMaterial(BLUR_MATERIALX);
//            DestroyMaterial(BLUR_MATERIALY);
//            DestroyMaterial(BLUR_MATERIALX_EA);
//            DestroyMaterial(BLUR_MATERIALY_EA);
//        }

//        public override bool CheckResources()
//        {
//            CheckSupport(true); // only requires depth, not HDR

//            if (!isSupported)
//                ReportAutoDisable();

//            return isSupported;		  
//        }

//        public void OnPreRender()
//        {
//            if (RTcustom_CAMERA)
//            {
//                RTcustom_CAMERA.CopyFrom(m_Camera);
//                RTcustom_CAMERA.renderingPath = RenderingPath.Forward;
//                RTcustom_CAMERA.clearFlags = CameraClearFlags.Color;

//                if (UseCustomDepth || (m_Camera.renderingPath == RenderingPath.Forward))
//                {
//                    RTcustom_CAMERA.backgroundColor = new Color(1, 1, 1, 1);
//                    RenderTexture camRT = RenderTexture.GetTemporary(Screen.width, Screen.height, 24, RenderTextureFormat.RFloat);
//                    camRT.filterMode = FilterMode.Point;
//                    RTcustom_CAMERA.targetTexture = camRT;
//                    RTcustom_CAMERA.RenderWithShader(CustomDepth_SHADER, "");
//                    camRT.SetGlobalShaderProperty("_depthTexCustom");
//                    RenderTexture.ReleaseTemporary(camRT);
//                }

//                //---------------------------------------
//                if (m_Camera.renderingPath == RenderingPath.Forward)
//                {
//                    RTcustom_CAMERA.backgroundColor = new Color(0, 0, 0, 0);
//                    RenderTexture camRT2 = RenderTexture.GetTemporary(Screen.width, Screen.height, 24, RenderTextureFormat.ARGBFloat);
//                    RTcustom_CAMERA.targetTexture = camRT2;
//                    RTcustom_CAMERA.RenderWithShader(CustomNormal_SHADER, "");
//                    camRT2.SetGlobalShaderProperty("_CameraNormalsTexture");
//                    RenderTexture.ReleaseTemporary(camRT2);
//                }
//                //---------------------------------------
//                RTcustom_CAMERA.targetTexture = null;

//            }

//        }





//        public override void ImageEffect_RenderImage(RenderTexture source, RenderTexture destination, RenderBuffer depthBuffer)
//        {	
//            //SSRR RELATED PARAMETERS
//            SSRR_MATERIAL.SetFloat("_stepGlobalScale", this.GlobalScale);
//            SSRR_MATERIAL.SetFloat("_bias", this.bias);
//            SSRR_MATERIAL.SetFloat("_maxStep", (float)this.maxGlobalStep);
//            SSRR_MATERIAL.SetFloat("_maxFineStep", (float)this.maxFineStep);
//            SSRR_MATERIAL.SetFloat("_maxDepthCull", this.maxDepthCull);
//            SSRR_MATERIAL.SetFloat("_fadePower", (float)this.fadePower);


//            Matrix4x4 P = m_Camera.projectionMatrix;
//            bool d3d = SystemInfo.graphicsDeviceVersion.IndexOf("Direct3D") > -1;
//            if (d3d)
//            {
//                for (int i = 0; i < 4; i++)
//                {
//                    P[2, i] = P[2, i] * 0.5f + P[3, i] * 0.5f;
//                }
//            }

//            Matrix4x4 viewProjInverse = (P * m_Camera.worldToCameraMatrix).inverse;
//            Shader.SetGlobalMatrix("_ViewProjectInverse", viewProjInverse);
//            Shader.SetGlobalFloat("_DistanceBlurRadius", DistanceBlurRadius);
//            Shader.SetGlobalFloat("_GrazeBlurPower", GrazeBlurPower);
//            Shader.SetGlobalFloat("_DistanceBlurStart", DistanceBlurStart);
//            Shader.SetGlobalFloat("_SSRRcomposeMode", SSRRcomposeMode);


//            SSRR_MATERIAL.SetMatrix("_ProjMatrix", P);
//            SSRR_MATERIAL.SetMatrix("_ProjectionInv", P.inverse);
//            SSRR_MATERIAL.SetMatrix("_ViewMatrix", m_Camera.worldToCameraMatrix.inverse.transpose);
//            //SIMPLE BLUR PARAMETERS
//            BLUR_MATERIALX.SetFloat("_BlurRadius", GlobalBlurRadius);
//            BLUR_MATERIALY.SetFloat("_BlurRadius", GlobalBlurRadius);
//            //EDGE AWARE BLUR PARAMETERS
//            Vector2 sensitivity = new Vector2(HQ_DepthSensetivity, HQ_NormalsSensetivity);
//            BLUR_MATERIALX_EA.SetVector("_Sensitivity", new Vector4(sensitivity.x, sensitivity.y, 1.0f, sensitivity.y));
//            BLUR_MATERIALX_EA.SetFloat("_blurSampleRadius", GlobalBlurRadius);
//            BLUR_MATERIALY_EA.SetVector("_Sensitivity", new Vector4(sensitivity.x, sensitivity.y, 1.0f, sensitivity.y));
//            BLUR_MATERIALY_EA.SetFloat("_blurSampleRadius", GlobalBlurRadius);
//            //SCREEN FADE CONTROLS
//            POST_COMPOSE_MATERIAL.SetVector("_ScreenFadeControls", new Vector4(DebugScreenFade, ScreenFadePower, ScreenFadeSpread, ScreenFadeEdge));
//            POST_COMPOSE_MATERIAL.SetFloat("_UseEdgeTexture", UseEdgeTexture);
//            POST_COMPOSE_MATERIAL.SetTexture("_EdgeFadeTexture", EdgeFadeTexture);
//            POST_COMPOSE_MATERIAL.SetFloat("_reflectionMultiply", this.reflectionMultiply);

//            float renderPathForward = 0.0f;
//            if (m_Camera.renderingPath == RenderingPath.Forward) renderPathForward = 1.0f;
//            POST_COMPOSE_MATERIAL.SetFloat("_IsInForwardRender", renderPathForward);
//            POST_COMPOSE_MATERIAL.SetFloat("_Strength", Strength);

//            //DO SSRR RENDERING
//            int sWidth = source.width;
//            int sHeight = source.height;

//            if (ResolutionOptimized)
//            {
//                sWidth /= 2;
//                sHeight /= 2;
//            }

//            //HDRreflections
//            RenderTextureFormat tmpFormat = RenderTextureFormat.ARGB32;
//            if (HDRreflections) tmpFormat = RenderTextureFormat.DefaultHDR;

//            RenderTexture reflectionTexture = RenderTexture.GetTemporary(sWidth, sHeight, 0, tmpFormat);
//            RenderTexture bluredTexture = RenderTexture.GetTemporary(sWidth, sHeight, 0, tmpFormat);

//            if (UseCustomDepth || (m_Camera.renderingPath == RenderingPath.Forward))
//                Graphics.Blit(source, reflectionTexture, SSRR_MATERIAL, 0);
//            else
//                Graphics.Blit(source, reflectionTexture, SSRR_MATERIAL, 1);

//            int roughpass = 0;

//            if (InvertRoughness) roughpass = 1;

//            if (!BlurQualityHigh && GlobalBlurRadius > 0)
//            {
//                for (int i = 0; i < HQ_BlurIterations; i++)
//                {
//                    Graphics.Blit(reflectionTexture, bluredTexture, BLUR_MATERIALX, roughpass);
//                    Graphics.Blit(bluredTexture, reflectionTexture, BLUR_MATERIALY, roughpass);
//                }
//            }
//            else if (BlurQualityHigh && GlobalBlurRadius > 0)
//            {
//                for (int i = 0; i < HQ_BlurIterations; i++)
//                {
//                    Graphics.Blit(reflectionTexture, bluredTexture, BLUR_MATERIALX_EA, roughpass);
//                    Graphics.Blit(bluredTexture, reflectionTexture, BLUR_MATERIALY_EA, roughpass);
//                }
//            }

//            reflectionTexture.SetGlobalShaderProperty("_SSRtexture");

//            Graphics.Blit(source, destination, POST_COMPOSE_MATERIAL);

//            RenderTexture.ReleaseTemporary(reflectionTexture);
//            RenderTexture.ReleaseTemporary(bluredTexture);

//        }
//    }

//}