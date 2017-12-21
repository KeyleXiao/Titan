/*******************************************************************
** 文件名:	SkySystemManager.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/11/3
** 版  本:	1.0
** 描  述:	天空系统
** 应  用:  	天空系统管理器

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;
using ImageEffects;
namespace SkySystem
{
    /// <summary>
    /// 天空系统管理器
    /// </summary>

    public sealed class SkySystemManager : MonoBehaviour
    {
        public enum NightModes
        {
            Off = 1,
            Static = 2,
            Rotation = 3
        }


        #region 基础设置

        public bool SkyUpdate = true;

        /// <summary>
        /// "当前时间(24小时)
        /// </summary>
        [Range(0.0f, 24.0f)]
        public float Timeline = 17.0f;

        /// <summary>
        /// 太阳水平角度
        /// </summary>
        [Range(-180.0f, 180.0f)]
        public float Longitude = 45.0f;

        /// <summary>
        /// 整体亮度缩放(提高所有光的亮度)
        /// </summary>
        [Range(0, 5.0f)]
        public float Exposure = 1.0f;

        /// <summary>
        /// 可见光波长,不同的可见光会有不同的大气颜色，与SkyTint一起,对最后的天空颜色进行微调
        /// </summary>
        public Vector3 Wavelengths = new Vector3(680f, 550f, 440f);

        /// <summary>
        /// 与波长结合，对最后的天光颜色进行微调
        /// </summary>
        public Color SkyTint = new Color(0.5f, 0.5f, 0.5f, 1f);

        /// <summary>
        /// 天空底部颜色，如无特殊需求，默认
        /// </summary>
        public Color m_GroundColor = new Color(0.369f, 0.349f, 0.341f, 1f);

        /// <summary>
        /// 天空材质球，必须用指定的天空shader做的材质球
        /// </summary>
        public Material SkyboxMaterial;

        /// <summary>
        /// 是否是线性颜色空间,一定要根据实际情况选!,两个空间下的颜色，完全不一样的
        /// </summary>
        public bool LinearSpace = false;


        /// <summary>
        /// Tonemapping
        /// </summary>
        private bool Tonemapping = false;


        #endregion

        #region 太阳

        /// <summary>
        /// 瑞利散射(指8km高空微粒散射现象,可以模拟出真实的地球天空，日落时地平面上的红/黄色,增强晴天，旁晚的天空效果
        /// </summary>
        [Range(0.0f, 5.0f)]
        public float RayleighScattering = 1.0f;

        /// <summary>
        /// 米氏散射(指1.2km低空微粒的散射现象,在雾气比较大的情况下，会产生围绕太阳的光环，配合Fog使用,增强雾天天气效果
        /// </summary>
        [Range(0.0f, 5.0f)]
        public float MieScattering = 1.0f;

        /// <summary>
        /// 太阳各项异性(控制太阳光斑的大小)
        /// </summary>
        [Range(0.0f, 0.9995f)]
        public float SunAnisotropyFactor = 0.76f;


        /// <summary>
        /// 太阳大小
        /// </summary>
        public float SunSize = 1.0f;

        /// <summary>
        /// 太阳,指一个平行光用来代表太阳
        /// </summary>
        public GameObject m_sunLight = null;

        #endregion

        #region 月亮

        public NightModes NightSky = NightModes.Static;

        private bool EnableNightSky
        {
            get
            {
                return (NightSky == NightModes.Off) ? false : true;
            }
        }

        /// <summary>
        /// 夜晚天空颜色
        /// </summary>
        public Gradient NightZenithColor = new Gradient()
        {
            colorKeys = new GradientColorKey[] {
			new GradientColorKey(new Color32(050, 071, 099, 255), 0.225f),
			new GradientColorKey(new Color32(074, 107, 148, 255), 0.25f),
			new GradientColorKey(new Color32(074, 107, 148, 255), 0.75f),
			new GradientColorKey(new Color32(050, 071, 099, 255), 0.775f),
		},
            alphaKeys = new GradientAlphaKey[] {
			new GradientAlphaKey(1.0f, 0.0f),
			new GradientAlphaKey(1.0f, 1.0f)
		}
        };

        /// <summary>
        /// 夜晚水平面天空颜色
        /// </summary>
        public Color NightTintColor = new Color(0.43f, 0.47f, 0.5f, 1f);


        /// <summary>
        /// 星星密度
        /// </summary>
        [Range(0.0f, 5.0f)]
        public float StarIntensity = 1.0f;

        /// <summary>
        /// 夜晚，外空间强度，OuterSpaceCubeMap的强度
        /// </summary>
        [Range(0.0f, 2.0f)]
        public float OuterSpaceIntensity = 0.25f;

        /// <summary>
        /// 月亮内部的月晕颜色，alpha值控制晕的大小和强度
        /// </summary>
        public Color MoonInnerCorona = new Color(1f, 1f, 1f, 0.5f);

        /// <summary>
        /// 月亮外部的月晕颜色，Alpha控制晕的大小和强度
        /// </summary>
        public Color MoonOuterCorona = new Color(0.25f, 0.39f, 0.5f, 0.5f);


        /// <summary>
        /// 月亮大小
        /// </summary>
        [Range(0.0f, 1.0f)]
        public float MoonSize = 0.15f;

        /// <summary>
        /// 月亮，指另一个平行光
        /// </summary>
        public GameObject m_moonLight;
        #endregion

        [SerializeField]
        protected bool _AutoApplySkybox = true;
        public bool AutoApplySkybox
        {
            get { return _AutoApplySkybox; }
            set
            {
                if (value && SkyboxMaterial)
                {
                    if (RenderSettings.skybox != SkyboxMaterial)
                        RenderSettings.skybox = SkyboxMaterial;
                }
                _AutoApplySkybox = value;
            }
        }

        private Vector3 euler;
        private Matrix4x4 moon_wtl;

        /// <summary>
        /// 星星材质
        /// </summary>
        private Material m_starMaterial;
        private Material starMaterial
        {
            get
            {
                if (!m_starMaterial)
                {
                    m_starMaterial = new Material(ImageEffectManager.API_GetShader("Scene/SkySystem/SkyStars"));
                    //m_starMaterial.hideFlags = HideFlags.DontSave;
                }
                return m_starMaterial;
            }
        }

        private Mesh _starsMesh = null;
        public Mesh starsMesh
        {
            get
            {
                
                SkyStars Stars = new SkyStars();
                if (!_starsMesh)
                {
                    _starsMesh = Stars.InitializeStars();
                }
                return _starsMesh;
            }
        }

        public void OnEnable()
        {
            if (m_sunLight == null)
            {
                Debug.LogError("请指定太阳光!!!!");
                enabled = false;
            }
                

            detectColorSpace();
        }

        public void OnDisable()
        {
            if (starsMesh)
            {
                DestroyImmediate(starsMesh);
            }
            if (m_starMaterial)
            {
                DestroyImmediate(m_starMaterial);
            } 
        }

        private void detectColorSpace()
        {
            //移动不支持linearspace
           if(Application.platform == RuntimePlatform.Android ||
               Application.platform == RuntimePlatform.IPhonePlayer)
           {
               LinearSpace = false;
           }

           if (SkyboxMaterial != null)
           {
               UpdateMaterial(SkyboxMaterial);
           }
                
        }

        public void Start()
        {
            Longitude = 45.0f;
            UpdateSun();
            InitMoon();
            if (SkyboxMaterial != null)
            {
                UpdateMaterial(SkyboxMaterial);
            }
                

            AutoApplySkybox = _AutoApplySkybox;
        }

        public float Timeline01 { get { return Timeline / 24; } }

        public void Update()
        {
            if (SkyUpdate)
            {
                if (Application.isPlaying)
                {
                    //每4帧更新一次
                    if (Time.frameCount % 4 != 0)
                    {
                        return;
                    }
                }
                
                if (Timeline >= 24.0f)
                    Timeline = 0.0f;

                if (Timeline < 0.0f)
                    Timeline = 24.0f;

                
                if (SkyboxMaterial != null)
                {
                    UpdateSun();
                    InitMoon();
                    UpdateMaterial(SkyboxMaterial);
                }
            }

            AutoApplySkybox = _AutoApplySkybox;
            detectColorSpace();

            // 绘制星星
            if (EnableNightSky && starsMesh != null && starMaterial != null && SunDir.y < 0.2f)
            {
                Graphics.DrawMesh(starsMesh, Vector3.zero, Quaternion.identity, starMaterial, 0);
            }
                
        }

        /// <summary>
        /// 根据时间轴来旋转太阳，就是转灯光了
        /// </summary>
        void UpdateSun()
        {
            euler.x = Timeline * 360.0f / 24.0f - 90.0f;
            euler.y = Longitude;
            if (m_sunLight != null)
            {
                m_sunLight.transform.localEulerAngles = euler;
            }
               
        }

        /// <summary>
        /// 更新材质球参数
        /// </summary>
        /// <param name="mat"></param>
        public void UpdateMaterial(Material mat)
        {
            mat.SetVector("_SunDir", SunDir);
            mat.SetMatrix("_Moon_wtl", getMoonMatrix);

            mat.SetVector("_betaR", betaR_RayleighOffset);
            mat.SetVector("_betaM", BetaM);


            mat.SetVector("_SkyMultiplier", skyMultiplier);

            mat.SetFloat("_SunSize", 32.0f / SunSize);
            mat.SetVector("_mieConst", mieConst);
            mat.SetVector("_miePhase_g", miePhase_g);
            mat.SetVector("_GroundColor", bottomTint);

            mat.SetVector("_NightHorizonColor", getNightHorizonColor);
            mat.SetVector("_NightZenithColor", getNightZenithColor);
            mat.SetVector("_MoonInnerCorona", getMoonInnerCorona);
            mat.SetVector("_MoonOuterCorona", getMoonOuterCorona);
            mat.SetFloat("_MoonSize", MoonSize);
            mat.SetVector("_colorCorrection", ColorCorrection);


            if (Tonemapping)
            {
                mat.DisableKeyword("USKY_HDR_OFF");
                mat.EnableKeyword("USKY_HDR_ON");
            }
            else
            {
                mat.EnableKeyword("USKY_HDR_OFF");
                mat.DisableKeyword("USKY_HDR_ON");
            }
            if (EnableNightSky)
            {
                mat.DisableKeyword("NIGHTSKY_OFF");
            }
            else
            {
                mat.EnableKeyword("NIGHTSKY_OFF");
            }
                

            if (NightSky == NightModes.Rotation)
                mat.SetMatrix("rotationMatrix", m_sunLight.transform.worldToLocalMatrix);
            else
                mat.SetMatrix("rotationMatrix", Matrix4x4.identity);

            mat.SetFloat("_OuterSpaceIntensity", OuterSpaceIntensity);
            if (starMaterial != null)
            {
                starMaterial.SetFloat("StarIntensity", starBrightness);
                if (NightSky == NightModes.Rotation)
                {
                    starMaterial.SetMatrix("rotationMatrix", m_sunLight.transform.localToWorldMatrix);
                }

                else
                {
                    starMaterial.SetMatrix("rotationMatrix", Matrix4x4.identity);
                }
                   
            }

        }

        void InitMoon()
        {
            if (NightSky == NightModes.Rotation)
            {
                m_moonLight.transform.forward = m_sunLight.transform.forward * -1;
            }


        }

        public Vector3 SunDir
        {
            get { return (m_sunLight != null) ? m_sunLight.transform.forward * -1 : new Vector3(0.321f, 0.766f, -0.557f); }
        }

        private Matrix4x4 getMoonMatrix
        {
            get
            {
                if (m_moonLight == null)
                {
                    // 在斜上方
                    moon_wtl = Matrix4x4.TRS(Vector3.zero, new Quaternion(-0.9238795f, 8.817204e-08f, 8.817204e-08f, 0.3826835f), Vector3.one);
                }
                else if (m_moonLight != null)
                {
                    moon_wtl = m_moonLight.transform.worldToLocalMatrix;
                    moon_wtl.SetColumn(2, Vector4.Scale(new Vector4(1, 1, 1, -1), moon_wtl.GetColumn(2)));
                }

                return moon_wtl;
            }
        }

       

        private Vector3 variableRangeWavelengths
        {
            get
            {
                return new Vector3(Mathf.Lerp(Wavelengths.x + 150, Wavelengths.x - 150, SkyTint.r),
                                    Mathf.Lerp(Wavelengths.y + 150, Wavelengths.y - 150, SkyTint.g),
                                    Mathf.Lerp(Wavelengths.z + 150, Wavelengths.z - 150, SkyTint.b));
            }
        }

        public Vector3 BetaR
        {
            get
            {
                // Evaluate Beta Rayleigh function is based on A.J.Preetham

                Vector3 WL = variableRangeWavelengths * 1e-9f;

                const float Km = 1000.0f;
                const float n = 1.0003f;		// the index of refraction of air
                const float N = 2.545e25f;		// molecular density at sea level
                const float pn = 0.035f;		// depolatization factor for standard air

                Vector3 waveLength4 = new Vector3(Mathf.Pow(WL.x, 4), Mathf.Pow(WL.y, 4), Mathf.Pow(WL.z, 4));
                Vector3 theta = 3.0f * N * waveLength4 * (6.0f - 7.0f * pn);
                float ray = (8 * Mathf.Pow(Mathf.PI, 3) * Mathf.Pow(n * n - 1.0f, 2) * (6.0f + 3.0f * pn));
                return Km * new Vector3(ray / theta.x, ray / theta.y, ray / theta.z);
            }
        }

        private Vector3 betaR_RayleighOffset
        {
            get
            { 
                return BetaR * Mathf.Max(1e-3f, RayleighScattering); 
            }
        }

        private readonly Vector3 BetaM = new Vector3(4e-3f, 4e-3f, 4e-3f) * 0.9f; // cheaper


        public float uMuS 
        { 
            get
            {
                return Mathf.Atan(Mathf.Max(SunDir.y, -0.1975f) * 5.35f) / 1.1f + 0.739f;
            }
        }


        public float DayTime
        { 
            get 
            { 
                return Mathf.Clamp01(uMuS); 
            } 
        }

        public float SunsetTime
        {
            get 
            {
                return Mathf.Clamp01((uMuS - 1.0f) * (1.5f / Mathf.Pow(RayleighScattering, 4f)));
            }
        }

        public float NightTime
        {
            get 
            { 
                return 1 - DayTime; 
            }
        }

        public Vector3 miePhase_g
        {
            get
            {
                float g2 = SunAnisotropyFactor * SunAnisotropyFactor;
                float cs = LinearSpace && Tonemapping ? 2f : 1f;
                return new Vector3(cs * ((1.0f - g2) / (2.0f + g2)), 1.0f + g2, 2.0f * SunAnisotropyFactor);
            }
        }

        public Vector3 mieConst
        {
            get
            { 
                return new Vector3(1f, BetaR.x / BetaR.y, BetaR.x / BetaR.z) * BetaM.x * MieScattering; 
            }
        }

        public Vector3 skyMultiplier
        {
            get 
            { 
                return new Vector3(SunsetTime, Exposure * 4 * DayTime * Mathf.Sqrt(RayleighScattering), NightTime); 
            }
        }

        private Vector3 bottomTint
        {
            get
            {
                float cs = LinearSpace ? 1e-2f : 2e-2f;
                return new Vector3(betaR_RayleighOffset.x / (m_GroundColor.r * cs),
                                    betaR_RayleighOffset.y / (m_GroundColor.g * cs),
                                    betaR_RayleighOffset.z / (m_GroundColor.b * cs));
            }
        }

        public Vector2 ColorCorrection
        {
            get
            {
                return (LinearSpace && Tonemapping) ? new Vector2(0.38317f, 1.413f) : 
                    LinearSpace ? new Vector2(1f, 2.0f) : Vector2.one;
            }
        }

        public Color getNightHorizonColor
        {
            get
            { 
                return NightTintColor * NightTime;
            } 
        }

        public Color getNightZenithColor 
        { 
            get 
            {
                return NightZenithColor.Evaluate(Timeline01) * 1e-2f;
            }
        }

        private float moonCoronaFactor
        {
            get
            {
                float m = 0.0f;
                float dir = m_sunLight.transform.forward.y;
                if (NightSky == NightModes.Rotation)
                    m = NightTime * dir;
                else
                    m = NightTime;
                return m;
            }
        }

        private Vector4 getMoonInnerCorona
        {
            get
            {
                return new Vector4(MoonInnerCorona.r * moonCoronaFactor,
                                    MoonInnerCorona.g * moonCoronaFactor,
                                    MoonInnerCorona.b * moonCoronaFactor,
                                    4e2f / MoonInnerCorona.a);
            }
        }

        private Vector4 getMoonOuterCorona
        {
            get
            {
                float cs = LinearSpace ? Tonemapping ? 16f : 12f : 8f;
                return new Vector4(MoonOuterCorona.r * 0.25f * moonCoronaFactor,
                                    MoonOuterCorona.g * 0.25f * moonCoronaFactor,
                                    MoonOuterCorona.b * 0.25f * moonCoronaFactor,
                                    cs / MoonOuterCorona.a);
            }
        }

        private float starBrightness
        {
            get
            {
                float cs = LinearSpace ? 1f : 1.5f;
                return StarIntensity * NightTime * cs;
            }
        }
	
    }
}

