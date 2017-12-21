//using System;
//using System.Collections;
//using System.Collections.Generic;
//using System.Runtime.CompilerServices;
//using UnityEngine;
//using UnityEngine.Rendering;

//namespace ImageEffects
//{
//    public class DynamicScatteringObject : GlowObject
//    {
//        public static Dictionary<Renderer, GlowObject> list = new Dictionary<Renderer, GlowObject>();

//        public float Turbidity = 8.0f;
//        public float WorldScale = 15.0f;
//        public float Rayleigh = 340.0f;
//        [Tooltip("Sun Scattering Size")]
//        [Range(0.0001f, 1.0f)]
//        public float ScatteringSize = 0.75f;
//        [Tooltip("Sun Scattering Intensity")]
//        [Range(1.0f, 200.0f)]
//        public float Intensity = 20.0f;
//        //[Tooltip("Boost scaterring color")]
//        [Range(0.0f, 10.0f)]
//        public float Bias = 1.2f;
//        [Range(0.0f, 10.0f)]
//        public float contrast = 5.0f;
//        public float luminance = 0.05f;
//        public float lumaAmount = 1f;
//        [HideInInspector]
//        public float Falloff = 0f;
//        [HideInInspector]
//        public float BoostColor = 1f;

//        public Vector3 LightAngle = new Vector3(5, 300, 0);

//        public void Awake()
//        {
//            Init(list);
//        }
//    }

//    [ExecuteInEditMode]
//    [RequireComponent(typeof(Camera))]
//    public class DynamicRayScattering : ImageEffectNode
//    {

//        public BlurMode _blurMode = BlurMode.Default;
//        public bool _highPrecsion = false;
//        public bool _highPrecsionActive = false;

//        /// <summary>
//        /// 调试模式
//        /// </summary>
//        public bool DebugMode = true;



//        public GlowSettings settings = new GlowSettings();

//        public Material mat = null;

//        private int validShaderLOD = 300;

//        public override bool IsOnValidLOD()
//        {
//            return ImageEffectManager.ImageEffectShaderLOD >= validShaderLOD;
//        }

//        public override void Awake()
//        {
//            if (settings == null)
//            {
//                settings = new GlowSettings();
//            }
//            highPrecision = _highPrecsion;

//            Shader shader = ImageEffectManager.API_GetShader("ImageEffect/DynamicScattering");
//            mat = new Material(shader);

//            base.Awake();
//        }

//        public override bool CheckResources()
//        {
//            return true;
//        }



//        IBlur blur = null;
//        public void RefreshBlur()
//        {
//            blur = null;
//            switch (blurMode)
//            {
//                case BlurMode.Default:
//                    blur = new DefaultBlur();

//                    break;

//                //case BlurMode.Advanced:
//                //    blur = new AdvancedBlur();
//                //    break;

//                case BlurMode.HighQuality:
//                    blur = new HqBlur();
//                    break;

//                case BlurMode.UnityBlur:
//                    blur = new UnityBlur();
//                    break;
//            }


//        }

//        public bool debug = false;
//        public bool batch = false;

//        public override bool ImageEffect_RenderImage(RenderTexture source, RenderTexture destination, RenderBuffer depthBuffer)
//        {

//            RenderTexture temp = RenderTexture.GetTemporary(source.width, source.height);

//            Graphics.SetRenderTarget(temp.colorBuffer, depthBuffer);
//            GL.Clear(false, true, Color.black);

//            CommandBuffer cb = null;
//            if (batch) cb = new CommandBuffer();
//            foreach (KeyValuePair<Renderer, GlowObject> pair in DynamicScatteringObject.list)
//            {
//                if (!batch) cb = new CommandBuffer();

//                Renderer r = pair.Key;
//                DynamicScatteringObject o = (DynamicScatteringObject)pair.Value;
//                if (r == null || o == null) continue;
//                if (o.enabled == false) continue;
//                mat.SetColor("_Color", o.color);
//                mat.SetFloat("_ColorStrength", o.strength);
//                mat.SetFloat("_Cutoff", o.AlphaCutoff);

//                Vector3 camPos = m_Camera.transform.position;
//                mat.SetVector("_CameraWS", camPos);

//                mat.SetMatrix("_world",r.localToWorldMatrix);

//                Vector4 SunTowards = new Vector4(10, 10, 10, 0);
//                Vector3 euler = o.LightAngle;
//                SunTowards = Quaternion.Euler(euler) * Vector3.forward;
//                SunTowards = -SunTowards;
//                mat.SetVector("SunTowards", SunTowards);

//                mat.SetTexture("_MainTex", r.material.mainTexture);

//                mat.SetFloat("_Size", o.ScatteringSize);
//                mat.SetFloat("turbid", o.Turbidity);
//                mat.SetFloat("worldscale", o.WorldScale);
//                mat.SetFloat("reileigh", o.Rayleigh);
//                mat.SetFloat("_Intensity", o.Intensity);
//                mat.SetFloat("_Falloff", o.Falloff);
//                mat.SetFloat("_ColBoost", o.BoostColor);
//                mat.SetFloat("luminance", o.luminance);
//                mat.SetFloat("lumamount", o.lumaAmount);
//                mat.SetFloat("bias", o.Bias);
//                mat.SetFloat("contrast", o.contrast);

//                cb.DrawRenderer(r, mat);

//                if (!batch) Graphics.ExecuteCommandBuffer(cb);
//            }
//            if (batch) Graphics.ExecuteCommandBuffer(cb);



//            if (debug)
//            {
//                Graphics.Blit(temp, destination);
//            }
//            else
//            {
//                Graphics.Blit(source, destination);
//                blur.BlurAndBlitBuffer(temp, destination, settings, highPrecision);
//            }

//            RenderTexture.ReleaseTemporary(temp);


          

//        }

//        public void OnDestroy()
//        {
      
//        }


//        public override void OnActive()
//        {
//            RefreshBlur();
//        }

//        private void Update()
//        {
//            if (CheckSupport())
//            {
//                if (QualitySettings.antiAliasing != 0)
//                {
//                    Debug.LogWarning("Glow不支持抗锯齿，请使用基于后处理的抗锯齿！");
//                    enabled = false;
//                }
//            }
//        }

//        public BlurMode blurMode
//        {
//            get
//            {
//                return _blurMode;
//            }
//            set
//            {
//                _blurMode = value;
//                RefreshBlur();
//            }
//        }

//        public bool highPrecision
//        {
//            get
//            {
//                return _highPrecsion;
//            }
//            set
//            {
//                _highPrecsion = value;
//                _highPrecsionActive = value && supportsHighPrecision;
//            }
//        }

//        public bool highPrecisionActive
//        {
//            get
//            {
//                return _highPrecsionActive;
//            }
//        }

//        public bool supportsHighPrecision
//        {
//            get
//            {
//                return SystemInfo.SupportsRenderTextureFormat(RenderTextureFormat.ARGBHalf);
//            }
//        }
//    }
//}


