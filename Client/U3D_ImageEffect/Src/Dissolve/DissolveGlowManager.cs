using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using UnityEngine;
using UnityEngine.Rendering;

namespace ImageEffects
{
    [RequireComponent(typeof(Camera))]
    public partial class DissolveGlowManager : ImageEffectNode
    {


        public BlurMode _blurMode = BlurMode.Default;
        public bool _highPrecsion = false;
        public bool _highPrecsionActive = false;

        /// <summary>
        /// 调试模式
        /// </summary>
        public bool DebugMode = false;

        public DissloveGlowCamera ActiveDissloveGlow;
        public Camera DissloveCam;
        private GameObject DissloveObj;

        public GlowSettings settings = new GlowSettings();
        private Camera HostCamera = null;
        private DissloveGlowCameraReuse m_DissloveGlowCamReuse = null;

        public static List<DissolveGlowManager> InstanceList = new List<DissolveGlowManager>(); 
        Material mat = null;

        public override void Awake()
        {
            InstanceList.Add(this);
            DissloveObj = new GameObject();
            DissloveObj.name = "DissloveGlowCamera";
            DissloveObj.hideFlags = HideFlags.HideAndDontSave;
            DissloveCam = DissloveObj.AddComponent<Camera>();
            DissloveCam.cullingMask = (1 << LayerMask.NameToLayer("LightingEffectOnly"));
            DissloveCam.enabled = false;
            if (settings == null)
            {
                settings = new GlowSettings();
            }
            settings.baseResolution = Resolution.Half;
            settings.boostStrength = 3;
            settings.downsampleSteps = 1;

            HostCamera = GetComponent<Camera>();
            highPrecision = _highPrecsion;

            if(Application.isPlaying)
            {
                Shader shader = ImageEffectManager.API_GetShader("ImageEffect/DissloveGlowMask");
                if (mat)
                {
                    Destroy(mat);
                    mat = null;
                }
                mat = new Material(shader);
            }

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
                UnityEngine.Object.DestroyImmediate(ActiveDissloveGlow);
            }
            else
            {
                UnityEngine.Object.Destroy(ActiveDissloveGlow);
            }
            DissloveObj.SetActive(false);
        }

        IBlur blur = null;

        public void InitCamera()
        {
            if (ActiveDissloveGlow != null)
            {
                DestroyCamera();
            }
            
            DissloveObj.SetActive(true);

            switch (blurMode)
            {
                case BlurMode.Default:
                    blur = new DissloveDefaultBlur();
                    
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
            
            DissloveCam.enabled = false;
            m_DissloveGlowCamReuse = DissloveCam.gameObject.AddComponent<DissloveGlowCameraReuse>();

            ActiveDissloveGlow = m_DissloveGlowCamReuse;
            ActiveDissloveGlow.dissolveGlowManager = this;
            ActiveDissloveGlow.highPrecision = _highPrecsionActive;
            ActiveDissloveGlow.parentCamera = HostCamera;
            ActiveDissloveGlow.blur = blur;
            ActiveDissloveGlow.settings = settings;
            ActiveDissloveGlow.Init();
        }

        public void OnDestroy()
        {
            DestroyCamera();
            InstanceList.Remove(this);
            if (DissloveCam != null)
            {
               UnityEngine.Object.DestroyImmediate(DissloveObj);
            }

            if (Application.isPlaying) ClearRender();
        }


        public override void OnDeActive()
        {
            //DeAttachAllCameraRenderFlags();
        }

        public override void OnActive()
        {
           // API_AttachCameraRenderFlags(DepthTextureMode.Depth);
        }

        private void Update()
        {
            if (CheckSupport())
            {
                if (QualitySettings.antiAliasing != 0)
                {
                    Debug.LogWarning("DissloveGlow不支持抗锯齿，请使用基于后处理的抗锯齿！");
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
                if (ActiveDissloveGlow != null)
                {
                    ActiveDissloveGlow.highPrecision = _highPrecsionActive;
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