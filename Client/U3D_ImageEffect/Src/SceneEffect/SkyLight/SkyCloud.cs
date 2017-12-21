/*******************************************************************
** 文件名:	SkyCloud.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/11/3
** 版  本:	1.0
** 描  述:	天空系统
** 应  用:  	天空系统--云

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;

namespace SkySystem
{

	[RequireComponent (typeof (SkySystemManager),typeof (SkyLight))]
	public class SkyCloud : MonoBehaviour
    {

        SkySystemManager m_sKySystemManager;
        SkyLight m_sKyLight;

		public int cloudLayer = 18;

		const float NightBrightness = 0.25f; // override the brightness at night time

		public Material CloudMaterial;

		private Mesh skyCloudMesh;

        protected SkySystemManager skySystemManager
        {
			get
            {
				if (!m_sKySystemManager)
                {
                    m_sKySystemManager = GetComponent<SkySystemManager>();
				}
				return m_sKySystemManager;
			}
		}
        protected SkyLight skyLight
        {
			get
            {
				if (!m_sKyLight) 
                {
                    m_sKyLight = GetComponent<SkyLight>();
				}
				return m_sKyLight;
			}
		}

        private const string SkyCloudResName = "SkySystemResource/SkyCloud";

        /// <summary>
        /// 加载云用的网格
        /// </summary>
        /// <returns></returns>
		protected Mesh LoadSkyCloundMesh ()
        {

            Mesh Hemisphere = Resources.Load<Mesh>(SkyCloudResName) as Mesh;

			if (Hemisphere == null)
            {
				Debug.LogWarning ("SkySystem--找不到云文件!");
				return null;
			} 
            else
            {
				Mesh m = new Mesh ();
				Vector3[] verts = Hemisphere.vertices;
				int i = 0;

				while (i < verts.Length) 
                {
					verts[i].y *=  0.85f; 
					i++;
				}
				m.vertices = verts;
				m.triangles = Hemisphere.triangles;
				m.normals = Hemisphere.normals;
				m.uv = Hemisphere.uv;
				m.uv2 = Hemisphere.uv2;

				// over size mesh bounds to avoid camera frustum culling for Vertex transformation in shader 
				m.bounds = new Bounds (Vector3.zero, Vector3.one * 2e9f); // less than 2,147,483,648
				//m.hideFlags = HideFlags.DontSave; 
				m.name = "skyCloudMesh";

				return m;
			}
		}

        public void OnEnable()
        {
            if (skyCloudMesh == null)
            {
                skyCloudMesh = LoadSkyCloundMesh();
            }
				
		}

        public void OnDisable()
        {
            if (skyCloudMesh)
            {
                DestroyImmediate(skyCloudMesh);
            }
				
		}


        public void Start() 
        {

            if (skySystemManager && skyLight)
            {
                UpdateCloudMaterial(); 
            }
					
		}


        public void Update()
        {
            if (skySystemManager)
            {
                if (skySystemManager.SkyUpdate && skyLight)
                {
                    UpdateCloudMaterial();
                }
                    
            }
            if (skyCloudMesh && CloudMaterial)
            {
                Graphics.DrawMesh(skyCloudMesh, Vector3.zero, Quaternion.identity, CloudMaterial, transform.gameObject.layer);
            }
				
		}

        void UpdateCloudMaterial()
        {
            float Brightness = Mathf.Max(Mathf.Pow(NightBrightness, skySystemManager.LinearSpace ? 1.5f : 1f), skySystemManager.DayTime);
            Brightness *= Mathf.Sqrt(skySystemManager.Exposure);

            if (CloudMaterial)
            {
                CloudMaterial.SetVector("ShadeColorFromSun", skySystemManager.LinearSpace ? skyLight.CurrentLightColor.linear * Brightness : skyLight.CurrentLightColor * Brightness);
                CloudMaterial.SetVector("ShadeColorFromSky", skySystemManager.LinearSpace ? skyLight.CurrentSkyColor.linear * Brightness : skyLight.CurrentSkyColor * Brightness);

            }
        }
	}
}
