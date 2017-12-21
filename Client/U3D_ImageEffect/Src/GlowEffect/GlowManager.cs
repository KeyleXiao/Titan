using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using UnityEngine;
using UnityEngine.Rendering;

namespace ImageEffects
{
    public enum Resolution
    {
        Full = 1,
        Half = 2,
        Quarter = 4
    }

    public enum BlendMode
    {
        Additive,
        Screen
    }

    public enum GlowTarget
    {
        /// <summary>
        /// 自己
        /// </summary>
        Self,

        /// <summary>
        /// 代码调用
        /// </summary>
        OutSidePointed,
    }

    public enum BlurMode
    {
       // Advanced = 5,
        Default = 0,
        HighQuality = 10,
        UnityBlur = 100
    }
    
    [RequireComponent(typeof(Camera))]
    public partial class GlowManager : ImageEffectNode
    {
        public BlurMode _blurMode = BlurMode.Default;
        private bool _highPrecsion = false;
        private bool _highPrecsionActive = false;

        /// <summary>
        /// 调试模式
        /// </summary>
        public bool DebugMode = true;

        public BaseGlowCamera ActiveGlow;
        public Camera glowCam;
        private GameObject glowObj;

        public GlowSettings settings = new GlowSettings();
        private Camera HostCamera = null;
        //private bool HostCameraHDRStatus = false;
        private GlowCameraReuse m_GlowCamReuse = null;
        public static List<GlowManager> InstanceList = new List<GlowManager>(); 
        //Material mat = null;

        
        public override void Awake()
        {
            InstanceList.Add(this);
            glowObj = new GameObject();
            glowObj.name = "GlowCamera";
            glowObj.hideFlags = HideFlags.HideAndDontSave;
            glowCam = glowObj.AddComponent<Camera>();
            glowCam.cullingMask = (1 << LayerMask.NameToLayer("LightingEffectOnly"));
            glowCam.enabled = false;
            if (settings == null)
            {
                settings = new GlowSettings();
            }
            HostCamera = GetComponent<Camera>();
            highPrecision = _highPrecsion;
            
            if (Application.isPlaying)
            {
                // Shader shader = ImageEffectManager.API_GetShader("ImageEffect/GlowMask");
                // if (mat)
                // {
                //     Destroy(mat);
                //     mat = null;
                // }
                //mat = new Material(shader);
            }

            CheckSupportHightQualityGlow();

            //HostCameraHDRStatus = HostCamera.hdr;
            //if (GlowManager.isSupportedHighQualityGlow)
            //{
            //    HostCamera.hdr = true;
            //    Shader.EnableKeyword("HIGH_QUALITY_GLOW");
            //}
            //else
            //{
            //    HostCamera.hdr = HostCameraHDRStatus;
            //    Shader.DisableKeyword("HIGH_QUALITY_GLOW");
            //}

            
            base.Awake();
            InitRender();
            InitCamera();
        }

        public override bool CheckResources()
        {
            return true;
        }
       

        private void DestroyCamera()
        {
            if (Application.isEditor)
            {
                UnityEngine.Object.DestroyImmediate(ActiveGlow);
            }
            else
            {
                UnityEngine.Object.Destroy(ActiveGlow);
            }
            glowObj.SetActive(false);
        }

        IBlur blur = null;
        public void InitCamera()
        {
            if (ActiveGlow != null)
            {
                DestroyCamera();
            }
            glowObj.SetActive(true);

            switch (blurMode)
            {
                case BlurMode.Default:
                    blur = new DefaultBlur();
                    
                    break;

                //case BlurMode.Advanced:
                //    blur = new AdvancedBlur();
                //    break;

                //case BlurMode.HighQuality:
                //    blur = new HqBlur();
                //    break;

                //case BlurMode.UnityBlur:
                //    blur = new UnityBlur();
                //    break;
            }
            
            glowCam.enabled = false;
            m_GlowCamReuse = glowCam.gameObject.AddComponent<GlowCameraReuse>();

            ActiveGlow = m_GlowCamReuse;
            ActiveGlow.glowManager = this;
            ActiveGlow.highPrecision = _highPrecsionActive;
            ActiveGlow.parentCamera = HostCamera;
            ActiveGlow.blur = blur;
            ActiveGlow.settings = settings;
            ActiveGlow.Init();
        }

        public void OnDestroy()
        {
            InstanceList.Remove(this);
            DestroyCamera();
            if (glowCam != null)
            {
                if (Application.isEditor)
                {
                    UnityEngine.Object.DestroyImmediate(glowObj);
                }
                else
                {
                    UnityEngine.Object.DestroyImmediate(glowObj);
                }
            }

            if (Application.isPlaying)
            {
                ClearRender();
            }
         
        }

        public override void OnDeActive()
        {
           // DeAttachAllCameraRenderFlags();
            //DestroyCamera();
        }

        public override void OnActive()
        {
            //API_AttachCameraRenderFlags(DepthTextureMode.Depth);
        }

        private void Update()
        {
            if (CheckSupport())
            {
                if (QualitySettings.antiAliasing != 0)
                {
                    Debug.LogWarning("Glow不支持抗锯齿，请使用基于后处理的抗锯齿！");
                    enabled = false;
                }
            }
        }

        public BlurMode blurMode
        {
            get
            {
                return _blurMode;
            }
            set
            {
                _blurMode = value;
                InitCamera();
            }
        }

        public bool highPrecision
        {
            get
            {
                return _highPrecsion;
            }
            set
            {
                _highPrecsion = value;
                _highPrecsionActive = value && supportsHighPrecision;
                if (ActiveGlow != null)
                {
                    ActiveGlow.highPrecision = _highPrecsionActive;
                }
            }
        }

        public bool highPrecisionActive
        {
            get
            {
                return _highPrecsionActive;
            }
        }

        public bool supportsHighPrecision
        {
            get
            {
                return SystemInfo.SupportsRenderTextureFormat(RenderTextureFormat.ARGBHalf);
            }
        }
    }
}

//using GlowEffect.Blur;
//using System;
//using UnityEngine;
//namespace GlowEffect
//{
//    public class BaseGlowCamera : ImageEffectNode
//    {
//        internal IBlur _blur;
//        private Shader _glowOnly;
//        public GlowEffect.GlowManager glowManager;
//        public bool highPrecision;
//        public Camera parentCamera;
//        public GlowSettings settings;

//        internal virtual void Init()
//        {
//        }

//        internal IBlur blur
//        {
//            get
//            {
//                return _blur;
//            }
//            set
//            {
//                _blur = value;
//            }
//        }

//        protected Shader glowOnly
//        {
//            get
//            {
//                if (_glowOnly == null)
//                {
//                    _glowOnly = ImageEffectManager.API_GetShader("Effect/Glow/GlowObjects");
//                }
//                return _glowOnly;
//            }
//        }
//    }
//}
