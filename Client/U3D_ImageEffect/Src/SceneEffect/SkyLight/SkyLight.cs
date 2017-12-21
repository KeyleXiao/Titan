/*******************************************************************
** 文件名:	SkyLight.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/11/3
** 版  本:	1.0
** 描  述:	天空系统
** 应  用:  	天空系统--天空灯光

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using UnityEngine.Rendering;

namespace SkySystem
{

	[RequireComponent (typeof (SkySystemManager))]
	public class SkyLight : MonoBehaviour
    {

        #region 环境光设置
        /// <summary>
        /// 天空颜色
        /// </summary>
        public Gradient SkyColor = new Gradient()
        {
            colorKeys = new GradientColorKey[] {
				new GradientColorKey(new Color32(028, 032, 040, 255), 0.225f),
				new GradientColorKey(new Color32(055, 065, 063, 255), 0.25f),
				new GradientColorKey(new Color32(148, 179, 219, 255), 0.28f),
				new GradientColorKey(new Color32(148, 179, 219, 255), 0.72f),
				new GradientColorKey(new Color32(055, 065, 063, 255), 0.75f),
				new GradientColorKey(new Color32(028, 032, 040, 255), 0.775f),
			},
            alphaKeys = new GradientAlphaKey[] {
				new GradientAlphaKey(1.0f, 0.0f),
				new GradientAlphaKey(1.0f, 1.0f)
			}
        };

        /// <summary>
        /// 中部颜色
        /// </summary>
        public Gradient EquatorColor = new Gradient()
        {
            colorKeys = new GradientColorKey[] {
				new GradientColorKey(new Color32(020, 025, 036, 255), 0.225f),
				new GradientColorKey(new Color32(080, 070, 050, 255), 0.25f),
				new GradientColorKey(new Color32(102, 138, 168, 255), 0.28f),
				new GradientColorKey(new Color32(102, 138, 168, 255), 0.72f),
				new GradientColorKey(new Color32(080, 070, 050, 255), 0.75f),
				new GradientColorKey(new Color32(020, 025, 036, 255), 0.775f),
			},
            alphaKeys = new GradientAlphaKey[] {
				new GradientAlphaKey(1.0f, 0.0f),
				new GradientAlphaKey(1.0f, 1.0f)
			}
        };
        
        /// <summary>
        /// 底部颜色
        /// </summary>
        public Gradient GroundColor = new Gradient()
        {
            colorKeys = new GradientColorKey[] {
				new GradientColorKey(new Color32(020, 020, 020, 255), 0.24f),
				new GradientColorKey(new Color32(051, 051, 051, 255), 0.27f),
				new GradientColorKey(new Color32(051, 051, 051, 255), 0.73f),
				new GradientColorKey(new Color32(020, 020, 020, 255), 0.76f),
			},
            alphaKeys = new GradientAlphaKey[] {
				new GradientAlphaKey(1.0f, 0.0f),
				new GradientAlphaKey(1.0f, 1.0f)
			}
        };

        #endregion


        /// <summary>
        /// 太阳光强度
        /// </summary>
        [Range(0f, 4f)]
		public float SunIntensity = 1.0f;

		/// <summary>
        /// 月亮和太阳发出颜色, 中间部分是白天太阳发出的颜色，两边是晚上月亮发出的颜色
		/// </summary>
		public Gradient LightColor = new Gradient()
		{
			colorKeys = new GradientColorKey[] {
				new GradientColorKey(new Color32(055, 066, 077, 255), 0.23f),
				new GradientColorKey(new Color32(245, 173, 084, 255), 0.26f),
				new GradientColorKey(new Color32(249, 208, 144, 255), 0.32f),

				new GradientColorKey(new Color32(252, 222, 186, 255), 0.50f),
				new GradientColorKey(new Color32(249, 208, 144, 255), 0.68f),
				new GradientColorKey(new Color32(245, 173, 084, 255), 0.74f),

				new GradientColorKey(new Color32(055, 066, 077, 255), 0.77f),
			},
			alphaKeys = new GradientAlphaKey[] {
				new GradientAlphaKey(1.0f, 0.0f),
				new GradientAlphaKey(1.0f, 1.0f)
			}
		};
		/// <summary>
		/// 启用月亮
		/// </summary>
		public bool EnableMoonLighting = true;

        
        /// <summary>
        /// 月亮强度
        /// </summary
        [Range(0f, 2f)]
		public float MoonIntensity = 0.4f;

		
		private float currentTime	{ get { return (sKySystemManager != null)? sKySystemManager.Timeline01	: 1f; }}
		private float dayTime		{ get { return (sKySystemManager != null)? sKySystemManager.DayTime		: 1f; }} 
		private float nightTime		{ get { return (sKySystemManager != null)? sKySystemManager.NightTime		: 0f; }}
		private float sunsetTime 	{ get { return (sKySystemManager != null)? sKySystemManager.SunsetTime	: 1f; }}

        private SkySystemManager sKySystemManager
        {
            get
            {
                if (m_SkySystemManager == null)
                {
                    m_SkySystemManager = this.gameObject.GetComponent<SkySystemManager>();
                }
                return m_SkySystemManager;
            }
        }
		private SkySystemManager m_SkySystemManager = null;
        
		private GameObject sunLightObj
        {
			get{
                if (sKySystemManager != null)
                {
                    return (sKySystemManager.m_sunLight != null) ? sKySystemManager.m_sunLight : null;
                }
                else
                {
                    return null;
                }
					
			}
		}


		private GameObject moonLightObj
        {
			get
            {
                if (sKySystemManager != null)
                {
                    return (sKySystemManager.m_moonLight != null) ? sKySystemManager.m_moonLight : null;
                }
                else
                {
                    return null;
                }
					
			}
		}

        private Light sun_Light
        {
            get
            {
                if (sunLightObj)
                    _sun_Light = sunLightObj.GetComponent<Light>();
                if (_sun_Light)
                    return _sun_Light;
                else return null;
            }
        }
		private Light _sun_Light;


        private Light moon_Light
        {
            get
            {
                if (moonLightObj)
                    _moon_Light = moonLightObj.GetComponent<Light>();
                if (_moon_Light)
                    return _moon_Light;
                else return null;
            }
        }
		private Light _moon_Light;

        private float exposure
        {
            get
            {
                return (sKySystemManager) ? sKySystemManager.Exposure : 1.0f;
            }
        }

        private float rayleighSlider
        {
            get
            {
                return (sKySystemManager) ? sKySystemManager.RayleighScattering : 1.0f;
            }
        }

        public Color CurrentLightColor
        {
            get
            {
                return LightColor.Evaluate(currentTime);
            }
        }

        public Color CurrentSkyColor
        {
            get
            {
                return colorOffset(SkyColor.Evaluate(currentTime), 0.15f, 0.7f, false);
            }
        }

        public Color CurrentEquatorColor
        {
            get
            {
                return colorOffset(EquatorColor.Evaluate(currentTime), 0.15f, 0.9f, false);
            }
        }

        public Color CurrentGroundColor
        {
            get
            {
                return colorOffset(GroundColor.Evaluate(currentTime), 0.25f, 0.85f, true);
            }
        }


        public void Start() 
        {
            if (sKySystemManager)
            {
                RenderSettings.ambientMode = AmbientMode.Trilight;
                UpdateLights();
            }
				
		}


        public void Update()
        {
			if (sKySystemManager)
            {
                if (sKySystemManager.SkyUpdate)
                {
                    UpdateLights();
                }
					
			}
		}
		

		void UpdateLights ()
        {
			SunAndMoonLightUpdate ();

            if (RenderSettings.ambientMode == AmbientMode.Trilight)
            {
                AmbientGradientUpdate();
            }
            else
            {
                RenderSettings.ambientLight = CurrentSkyColor; // 更新云层的颜色
            }
				
		}
		

		void SunAndMoonLightUpdate ()
        {

			if (sunLightObj != null)
            {
				if (sun_Light != null)
                {
					sun_Light.intensity = sKySystemManager.Exposure * SunIntensity * dayTime;
					sun_Light.color = CurrentLightColor * dayTime;

					// 白天启用，晚上关闭
					sun_Light.enabled = (currentTime < 0.24f || currentTime > 0.76f) ? false : true;
				}
			}
            if (moonLightObj != null)
            {
                if (moon_Light != null)
                {
                    moon_Light.intensity = sKySystemManager.Exposure * MoonIntensity * nightTime;
                    moon_Light.color = CurrentLightColor * nightTime;

                    //晚上启用，白天关闭
                    moon_Light.enabled = (currentTime > 0.26f && currentTime < 0.74f || EnableMoonLighting == false) ?
                             false : (EnableMoonLighting) ? true : false;
                    if (!EnableMoonLighting)
                    {
                        forceSunEnableAtNight();
                    }

                }
            }
            else
            {
                forceSunEnableAtNight();
            }
				
			
		}


        /// <summary>
        /// 如果晚上没有灯光，就启用白天的灯，改一下颜色
        /// </summary>
		private void forceSunEnableAtNight ()
        {
            if (!sun_Light)
            {
                return;
            } 
			sun_Light.enabled = true;
			sun_Light.intensity = Mathf.Max (1e-3f, sun_Light.intensity);
			sun_Light.color = new Color(sun_Light.color.r, sun_Light.color.g, sun_Light.color.b,
			                            Mathf.Max (1e-2f,sun_Light.color.a ));
		}


 
        /// <summary>
        /// 更新环境光
        /// </summary>
		void AmbientGradientUpdate ()
		{
			RenderSettings.ambientSkyColor = CurrentSkyColor;
			RenderSettings.ambientEquatorColor = CurrentEquatorColor;
			RenderSettings.ambientGroundColor = CurrentGroundColor;
		}


		/// <summary>
        /// 根据SkySystemManager的颜色改变，取出近似值，输出给GradientColors
		/// </summary>
		/// <param name="currentColor"></param>
		/// <param name="offsetRange"></param>oppsite
		/// <param name="rayleighOffsetRange"></param>
		/// <param name="IsGround"></param>
		/// <returns></returns>
		private Color colorOffset ( Color currentColor, float offsetRange, float rayleighOffsetRange, bool IsGround )
        {
			Vector3 _betaR = (sKySystemManager)? sKySystemManager.BetaR * 1e3f : new Vector3 (5.81f, 13.57f, 33.13f);

			// 混合权重
			Vector3 t = new Vector3 (0.5f, 0.5f, 0.5f); // 中性颜色 


				t = new Vector3 (_betaR.x /  5.81f * 0.5f ,
			                     _betaR.y / 13.57f * 0.5f ,
			                     _betaR.z / 33.13f * 0.5f );

			// 混合白天的权重
            if (!IsGround)
            {
                t = Vector3.Lerp(new Vector3(Mathf.Abs(1 - t.x), Mathf.Abs(1 - t.y), Mathf.Abs(1 - t.z)), t, sunsetTime);
            }
			

			// 混合晚上的权重
			t = Vector3.Lerp (new Vector3 ( 0.5f, 0.5f, 0.5f), t, dayTime);

			Vector3 c0 = new Vector3();

            //混合_betaR
			c0 = new Vector3 (Mathf.Lerp (currentColor.r - offsetRange, currentColor.r + offsetRange, t.x),
			                          Mathf.Lerp (currentColor.g - offsetRange, currentColor.g + offsetRange, t.y),
			                          Mathf.Lerp (currentColor.b - offsetRange, currentColor.b + offsetRange, t.z));

            // 从rayleighSlider加上RayLeigh便宜 
			Vector3 c2 = new Vector3 (c0.x / _betaR.x, c0.y / _betaR.y, c0.z / _betaR.z) * 4f ;

			c0 = (rayleighSlider < 1.0f) ? Vector3.Lerp (Vector3.zero, c0, rayleighSlider) :
				Vector3.Lerp (c0, c2 , Mathf.Max(0f,(rayleighSlider - 1f)) / 4f * rayleighOffsetRange);

			return new Color(c0.x,c0.y,c0.z,1) * exposure;
		}

	}
}
