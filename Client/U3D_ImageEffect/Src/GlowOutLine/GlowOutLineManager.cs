using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using UnityEngine;
using UnityEngine.Rendering;

namespace ImageEffects
{
    [RequireComponent(typeof(Camera))]
    public partial class GlowOutLineManager : ImageEffectNode
    {

        public BlurMode _blurMode = BlurMode.Default;
        public bool _highPrecsion = false;
        public bool _highPrecsionActive = false;

        /// <summary>
        /// 调试模式
        /// </summary>
        public bool DebugMode = false;

        public GlowOutLineCamera ActiveGlowOutLine;
        public Camera GlowOutLineCam;
        
        private GameObject GlowOutLineObj;

        public GlowSettings settings = new GlowSettings();
        private Camera HostCamera = null;
        private GlowOutLineCameraReuse m_GlowOutLineCamReuse = null;

        public static List<GlowOutLineManager> InstanceList = new List<GlowOutLineManager>(); 
        //Material mat = null;

        public override void Awake()
        {
            InstanceList.Add(this);
            GlowOutLineObj = new GameObject();
            GlowOutLineObj.name = "GlowOutLineCamera";
            GlowOutLineObj.hideFlags = HideFlags.HideAndDontSave;
            GlowOutLineCam = GlowOutLineObj.AddComponent<Camera>();
            GlowOutLineCam.cullingMask = (1 << LayerMask.NameToLayer("LightingEffectOnly"));
            GlowOutLineCam.enabled = false;
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
                //Shader shader = ImageEffectManager.API_GetShader("ImageEffect/GlowOutlineMask");
                //if (mat)
                //{
                //    Destroy(mat);
                //    mat = null;
                //}
                //mat = new Material(shader);
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
                UnityEngine.Object.DestroyImmediate(ActiveGlowOutLine);
            }
            else
            {
                UnityEngine.Object.Destroy(ActiveGlowOutLine);
            }
            GlowOutLineObj.SetActive(false);
        }

        IBlur blur = null;
        public void InitCamera()
        {
            if (ActiveGlowOutLine != null)
            {
                DestroyCamera();
            }
            
            GlowOutLineObj.SetActive(true);

            //switch (blurMode)
            //{
            //    case BlurMode.Default:
            //        blur = new DefaultBlur();
                    
            //        break;

            //    //case BlurMode.Advanced:
            //    //    blur = new AdvancedBlur();
            //    //    break;

            //    case BlurMode.HighQuality:
            //        blur = new HqBlur();
            //        break;

            //    case BlurMode.UnityBlur:
            //        blur = new UnityBlur();
            //        break;
            //}

            blur = new GlowOutLineBlur(); 
            GlowOutLineCam.enabled = false;
            m_GlowOutLineCamReuse = GlowOutLineCam.gameObject.AddComponent<GlowOutLineCameraReuse>();

            ActiveGlowOutLine = m_GlowOutLineCamReuse;
            ActiveGlowOutLine.glowOutLineManager = this;
            ActiveGlowOutLine.highPrecision = _highPrecsionActive;
            ActiveGlowOutLine.parentCamera = HostCamera;
            ActiveGlowOutLine.blur = blur;
            ActiveGlowOutLine.settings = settings;
            ActiveGlowOutLine.Init();
        }

        public void OnDestroy()
        {
            DestroyCamera();
            InstanceList.Remove(this);
            if (GlowOutLineCam != null)
            {
                if (Application.isEditor)
                {
                    UnityEngine.Object.DestroyImmediate(GlowOutLineObj);
                }
                else
                {
                    UnityEngine.Object.DestroyImmediate(GlowOutLineObj);
                }
            }

            if (Application.isPlaying)ClearRender();
        }

        public override void OnDeActive()
        {
            //DeAttachAllCameraRenderFlags();
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
                if (ActiveGlowOutLine != null)
                {
                    ActiveGlowOutLine.highPrecision = _highPrecsionActive;
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