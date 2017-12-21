using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEngine.Rendering;


public partial class Water : MonoBehaviour
{
    public static bool isHeroOnWater
    {
        get 
        {
            if(!EntityFactory.MainHero || WaterInstanceList.Count <= 0)
            {
                return false;
            }

            Vector3 mainHeroPos = EntityFactory.MainHero.transform.position;
            foreach(Water w in WaterInstanceList)
            {
                if(w.enabled)
                {
                    if(w.IsOnWater(mainHeroPos))
                    {
                        return true;
                    }
                }
            }
            return false;
        }
    }

	public int m_TextureSize = 512;
	public float m_ClipPlaneOffset = 0.07f;

    [Range(0,10.0f)]
    public float EdgeBlendStength = 2.0f;
	public LayerMask m_BakeCullMask = -1;
    
	private Dictionary<Camera, Camera> m_ReflectionCameras = new Dictionary<Camera, Camera>(); // Camera -> Camera table
	private Dictionary<Camera, Camera> m_RefractionCameras = new Dictionary<Camera, Camera>(); // Camera -> Camera table



    private RenderTexture m_ReflectionTexture = null;
    private RenderTexture m_RefractionTexture = null;


	private int m_OldReflectionTextureSize = 0;
	private int m_OldRefractionTextureSize = 0;
	
	private static bool s_InsideWater = false;
    private Renderer m_Renderer = null;
    private Camera lastReflectionCamera = null;
    private Camera lastRefractionCamera = null;
    private static List<Water> WaterInstanceList = new List<Water>();
    private static SceneRenderingQuality hardWateSupportQuality = SceneRenderingQuality.Fastest;
    private static SceneRenderingQuality lastQuality = SceneRenderingQuality.Invaild;
    ImageEffects.ImageEffectManager.CameraRenderFlagsNode nodes = null;

    public static void Init()
    {
        foreach(Water w in WaterInstanceList)
        {
            if(w)
            {
                w.ClearOptimitzed();
                w.InitOptimitzed();
            }
            
        }
        
    }

    public void Start()
    {
        WaterInstanceList.Add(this);
        if (!m_Renderer)
        {
            m_Renderer = GetComponent<Renderer>();
        }

        FindHardwareWaterSupport();
       
    }

    void OnEanble()
    {

    }

    private void ReleaseRelectionSource()
    {
        foreach (KeyValuePair<Camera, Camera> kvp in m_ReflectionCameras)
        {
            if ((kvp.Value))
            {
                kvp.Value.targetTexture = null;
                DestroyImmediate((kvp.Value).gameObject);
            }

        }

        m_ReflectionCameras.Clear();

        if (m_ReflectionTexture)
        {
            DestroyImmediate(m_ReflectionTexture);
            m_ReflectionTexture = null;
        }
    }

    private void ReleaseRefractionSource()
    {
        foreach (KeyValuePair<Camera, Camera> kvp in m_RefractionCameras)
        {
            if ((kvp.Value))
            {
                kvp.Value.targetTexture = null;
                DestroyImmediate((kvp.Value).gameObject);
            }

        }

        if (m_RefractionTexture)
        {
            DestroyImmediate(m_RefractionTexture);
            m_RefractionTexture = null;
        }
        m_RefractionCameras.Clear();
    }

    public void OnDisable()
    {
        ReleaseRelectionSource();
        ReleaseRefractionSource();

    }

    public void OnDestroy()
    {
        ClearOptimitzed();
        WaterInstanceList.Remove(this);



        lastRefractionCamera = null;
        lastReflectionCamera = null;
    }

    public bool IsOnWater(Vector3 pos)
    {
        if(!bBaked)
        {
            return false;
        }
        Vector3 center = pos;
        center.y += 0.3f;

        Vector3 pos1 = transform.position;
        pos1.y -= 0.3f;
        bool onWater = Physics.Linecast(center, pos1, 1 << LayerMask.NameToLayer("Water"));
        return onWater;
    }



    public void RenderMap(SceneRenderingQuality mode, Camera cam, bool isBake = false)
    {
        if(!cam)
        {
            return;
        }

        //有多个相机的话，会触发多次，多个相机只绘制一次
        if (s_InsideWater)
            return;
        s_InsideWater = true;


        // 禁用像素灯，可以提高效率


        Camera reflectionCamera, refractionCamera;
        CreateWaterObjects(cam, out reflectionCamera, out refractionCamera);

        // 根据法线和坐标计算反射平面
        Vector3 pos = transform.position;
        Vector3 normal = transform.up;

        UpdateCameraModes(cam, reflectionCamera);
        UpdateCameraModes(cam, refractionCamera);

        if (reflectionCamera)
        {
            reflectionCamera.enabled = false;
        }

        if (refractionCamera)
        {
            refractionCamera.enabled = false;
        }
        

        // 渲染反射贴图
        if (mode >= SceneRenderingQuality.High)
        {
            
            if (lastReflectionCamera != reflectionCamera)
            {
                if (lastReflectionCamera)
                {
                    lastReflectionCamera.enabled = false;
                }

                lastReflectionCamera = reflectionCamera;
            }
            reflectionCamera.enabled = true;
            // 计算反射相机的反射平面
            float d = -Vector3.Dot(normal, pos) - m_ClipPlaneOffset;
            Vector4 reflectionPlane = new Vector4(normal.x, normal.y, normal.z, d);

            Matrix4x4 reflection = Matrix4x4.zero;
            CalculateReflectionMatrix(ref reflection, reflectionPlane);
            Vector3 oldpos = cam.transform.position;
            Vector3 newpos = reflection.MultiplyPoint(oldpos);
            reflectionCamera.worldToCameraMatrix = cam.worldToCameraMatrix * reflection;

            // 计算裁剪面
            Vector4 clipPlane = CameraSpacePlane(reflectionCamera, pos, normal, 1.0f);
            reflectionCamera.projectionMatrix = cam.CalculateObliqueMatrix(clipPlane);

            bool OldCulling = GL.invertCulling;
            GL.invertCulling = true;
               
            reflectionCamera.transform.SetPosition( newpos);
            Vector3 euler = cam.transform.eulerAngles;
            reflectionCamera.transform.eulerAngles = new Vector3(-euler.x, euler.y, euler.z);

            //reflectionCamera.transform.SetPosition( oldpos;
            GL.invertCulling = OldCulling;
        }

        // 渲染折射贴图
        if (mode >= SceneRenderingQuality.Best)
        {
            if (lastRefractionCamera != refractionCamera)
            {
                if (lastRefractionCamera)
                {
                    lastRefractionCamera.enabled = false;
                }
                lastRefractionCamera = refractionCamera;
            }

            lastRefractionCamera.enabled = true;
            refractionCamera.worldToCameraMatrix = cam.worldToCameraMatrix;
            // 裁剪面
            Vector4 clipPlane = CameraSpacePlane(refractionCamera, pos, normal, -1.0f);
            refractionCamera.projectionMatrix = cam.CalculateObliqueMatrix(clipPlane);

            refractionCamera.transform.SetPosition(cam.transform.position);
            refractionCamera.transform.rotation = cam.transform.rotation;

        }

        s_InsideWater = false;
    }


	public void LateUpdate()
	{
		if( !enabled || !m_Renderer || !m_Renderer.sharedMaterial || !m_Renderer.enabled )
			return;

        Camera cam = Initialize.mainCam;
        if (!cam)
        {
            cam = Camera.current;
            if(!cam)
            {
                return;
            }
        }

        SceneRenderingQuality mode = GetWaterMode();
        if (mode != lastQuality)
        {
            lastQuality = mode;
            if (mode == SceneRenderingQuality.Fastest)
            {
                ImageEffects.ImageEffectManager.API_DeAttachCameraRenderFlags(cam, ref nodes);
                ResetAllRenders();
                ReleaseRelectionSource();
                ReleaseRefractionSource();
            }
            else
            {
                nodes = ImageEffects.ImageEffectManager.API_AttachCameraRenderFlags(cam, DepthTextureMode.Depth);
            }
        }
        UpdateOptimiezed(mode);
        RenderMap(mode, cam);

        if (mode >= SceneRenderingQuality.High)
        {
            m_Renderer.sharedMaterial.SetTexture("_ReflectionTex", m_ReflectionTexture);
        }
        if (mode >= SceneRenderingQuality.Best)
        {
            m_Renderer.sharedMaterial.SetTexture("_RefractionTex", m_RefractionTexture);
        }
       
        
		// 根据水模式设置Shader
		switch( mode )
		{
            case SceneRenderingQuality.Fastest:
			Shader.EnableKeyword( "WATER_SIMPLE" );
			Shader.DisableKeyword( "WATER_REFLECTIVE" );
			Shader.DisableKeyword( "WATER_REFRACTIVE" );
			break;
            case SceneRenderingQuality.High:
			Shader.DisableKeyword( "WATER_SIMPLE" );
			Shader.EnableKeyword( "WATER_REFLECTIVE" );
			Shader.DisableKeyword( "WATER_REFRACTIVE" );
			break;
            case SceneRenderingQuality.Best:
			Shader.DisableKeyword( "WATER_SIMPLE" );
			Shader.DisableKeyword( "WATER_REFLECTIVE" );
			Shader.EnableKeyword( "WATER_REFRACTIVE" );
			break;
		}
			

	}



    public void Update()
	{
		if( !m_Renderer )
			return;
        Material mat = m_Renderer.sharedMaterial;
		if( !mat )
			return;
			
		Vector4 waveSpeed = mat.GetVector( "WaveSpeed" );
		float waveScale = mat.GetFloat( "_WaveScale" );
		Vector4 waveScale4 = new Vector4(waveScale, waveScale, waveScale * 0.4f, waveScale * 0.45f);
		

		double t = Time.timeSinceLevelLoad / 20.0;
		Vector4 offsetClamped = new Vector4(
			(float)System.Math.IEEERemainder(waveSpeed.x * waveScale4.x * t, 1.0),
			(float)System.Math.IEEERemainder(waveSpeed.y * waveScale4.y * t, 1.0),
			(float)System.Math.IEEERemainder(waveSpeed.z * waveScale4.z * t, 1.0),
			(float)System.Math.IEEERemainder(waveSpeed.w * waveScale4.w * t, 1.0)
		);
		
		mat.SetVector( "_WaveOffset", offsetClamped );
		mat.SetVector( "_WaveScale4", waveScale4 );
			
		Vector3 waterSize = m_Renderer.bounds.size;		
		Vector3 scale = new Vector3( waterSize.x*waveScale4.x, waterSize.z*waveScale4.y, 1 );
		Matrix4x4 scrollMatrix = Matrix4x4.TRS( new Vector3(offsetClamped.x,offsetClamped.y,0), Quaternion.identity, scale );
		mat.SetMatrix( "_WaveMatrix", scrollMatrix );
				
		scale = new Vector3( waterSize.x*waveScale4.z, waterSize.z*waveScale4.w, 1 );
		scrollMatrix = Matrix4x4.TRS( new Vector3(offsetClamped.z,offsetClamped.w,0), Quaternion.identity, scale );
		mat.SetMatrix( "_WaveMatrix2", scrollMatrix );
        mat.SetFloat("_EdgeBlendStrength", EdgeBlendStength);
        
	}
	
	private void UpdateCameraModes( Camera src, Camera dest )
	{
		if( dest == null )
			return;

		dest.clearFlags = src.clearFlags;
		dest.backgroundColor = src.backgroundColor;		
        //if( src.clearFlags == CameraClearFlags.Skybox )
        //{
        //    Skybox sky = src.GetComponent(typeof(Skybox)) as Skybox;
        //    Skybox mysky = dest.GetComponent(typeof(Skybox)) as Skybox;
        //    if( !sky || !sky.material )
        //    {
        //        mysky.enabled = false;
        //    }
        //    else
        //    {
        //        mysky.enabled = true;
        //        mysky.material = sky.material;
        //    }
        //}

		dest.farClipPlane = src.farClipPlane;
		dest.nearClipPlane = src.nearClipPlane;
		dest.orthographic = src.orthographic;
		dest.fieldOfView = src.fieldOfView;
		dest.aspect = src.aspect;
		dest.orthographicSize = src.orthographicSize;
	}
	
	private void CreateWaterObjects( Camera currentCamera, out Camera reflectionCamera, out Camera refractionCamera )
	{
        SceneRenderingQuality mode = GetWaterMode();
		
		reflectionCamera = null;
		refractionCamera = null;

        if (mode >= SceneRenderingQuality.High)
		{
            // 反射相机
            m_ReflectionCameras.TryGetValue(currentCamera, out reflectionCamera);
            if (!reflectionCamera)
            {
                GameObject go = new GameObject("Water Refl Camera id" + GetInstanceID() + " for " + currentCamera.GetInstanceID(), typeof(Camera));
                reflectionCamera = go.GetComponent<Camera>();
                reflectionCamera.transform.SetPosition( transform.position);
                reflectionCamera.transform.rotation = transform.rotation;
                reflectionCamera.gameObject.AddComponent<FlareLayer>();
                reflectionCamera.renderingPath = RenderingPath.VertexLit;

                WaterHelper helper = reflectionCamera.gameObject.AddComponent<WaterHelper>();
                helper.hostWater = this;
                helper.hostCamera = reflectionCamera;

                WaterCullLayer = 0;
                WaterCullLayer |= 1 << WaterObjLayer;//反射水物体
                WaterCullLayer |= 1 << LayerMask.NameToLayer(Config.LayerPlayerEnemy); //反射友方玩家
                WaterCullLayer |= 1 << LayerMask.NameToLayer(Config.LayerPlayerFriend); //反射敌方玩家
                WaterCullLayer |= 1 << LayerMask.NameToLayer(Config.LayerHero); //反射主角玩家
                reflectionCamera.cullingMask = WaterCullLayer;

                int layer = currentCamera.cullingMask;
                layer = layer & ~(1 << WaterObjLayer);
                currentCamera.cullingMask = layer;
                reflectionCamera.targetTexture = m_ReflectionTexture;

                go.hideFlags = HideFlags.HideAndDontSave;
                m_ReflectionCameras[currentCamera] = reflectionCamera;
            }

			//反射RT
			if( !m_ReflectionTexture || m_OldReflectionTextureSize != m_TextureSize )
			{
				if( m_ReflectionTexture )
					DestroyImmediate( m_ReflectionTexture );
				m_ReflectionTexture = new RenderTexture( m_TextureSize, m_TextureSize, 16 );
				m_ReflectionTexture.name = "__WaterReflection" + GetInstanceID();
				m_ReflectionTexture.isPowerOfTwo = true;
				m_ReflectionTexture.hideFlags = HideFlags.DontSave;
				m_OldReflectionTextureSize = m_TextureSize;
                reflectionCamera.targetTexture = m_ReflectionTexture;
			}
			


		}

        if (mode >= SceneRenderingQuality.Best)
		{
            // 折射相机
            m_RefractionCameras.TryGetValue(currentCamera, out refractionCamera);
            if (!refractionCamera)
            {

                GameObject go = new GameObject("Water Refr Camera id" + GetInstanceID() + " for " + currentCamera.GetInstanceID(), typeof(Camera));
                refractionCamera = go.GetComponent<Camera>();
                refractionCamera.transform.SetPosition(transform.position);
                refractionCamera.transform.rotation = transform.rotation;
                refractionCamera.gameObject.AddComponent<FlareLayer>();
                refractionCamera.renderingPath = RenderingPath.VertexLit;

                WaterHelper helper = refractionCamera.gameObject.AddComponent<WaterHelper>();
                helper.hostWater = this;
                helper.hostCamera = refractionCamera;

                refractionCamera.targetTexture = m_RefractionTexture;
                WaterCullLayer = 0;
                WaterCullLayer |= 1 << WaterObjLayer;//反射水物体
                WaterCullLayer |= 1 << LayerMask.NameToLayer(Config.LayerPlayerEnemy); //反射友方玩家
                WaterCullLayer |= 1 << LayerMask.NameToLayer(Config.LayerPlayerFriend); //反射敌方玩家
                WaterCullLayer |= 1 << LayerMask.NameToLayer(Config.LayerHero); //反射主角玩家
                refractionCamera.cullingMask = WaterCullLayer;

                int layer = currentCamera.cullingMask;
                layer = layer & ~(1 << WaterObjLayer); ;
                currentCamera.cullingMask = layer;

                go.hideFlags = HideFlags.HideAndDontSave;
                m_RefractionCameras[currentCamera] = refractionCamera;
            }

			// 折射RT
			if( !m_RefractionTexture || m_OldRefractionTextureSize != m_TextureSize )
			{
				if( m_RefractionTexture )
					DestroyImmediate( m_RefractionTexture );
				m_RefractionTexture = new RenderTexture( m_TextureSize, m_TextureSize, 16);
				m_RefractionTexture.name = "__WaterRefraction" + GetInstanceID();
				m_RefractionTexture.isPowerOfTwo = true;
				m_RefractionTexture.hideFlags = HideFlags.DontSave;
				m_OldRefractionTextureSize = m_TextureSize;
                refractionCamera.targetTexture = m_RefractionTexture;
			}
			

		}
	}

    public SceneRenderingQuality GetWaterMode()
	{
        if (!WaterRefrctionObjectShader)
        {
            return SceneRenderingQuality.Fastest;
        }

        if (hardWateSupportQuality < ImageSetting.GetData().currSceneRenderingQuality)
            return hardWateSupportQuality;
		else
            return ImageSetting.GetData().currSceneRenderingQuality;
	}

    private void FindHardwareWaterSupport()
	{
        hardWateSupportQuality = SceneRenderingQuality.Fastest;
        if (!SystemInfo.supportsRenderTextures || !m_Renderer)
        {
            return;
        }

            

        Material mat = m_Renderer.sharedMaterial;
		if( !mat )
        {
            return;
        }
            
			
		string mode = mat.GetTag("WATERMODE", false);
		if( mode == "Refractive" )
        {
            hardWateSupportQuality = SceneRenderingQuality.High;
        }
	}
	
	private static float sgn(float a)
	{
        if (a > 0.0f) return 1.0f;
        if (a < 0.0f) return -1.0f;
        return 0.0f;
	}
	

	private Vector4 CameraSpacePlane (Camera cam, Vector3 pos, Vector3 normal, float sideSign)
	{
		Vector3 offsetPos = pos + normal * m_ClipPlaneOffset;
		Matrix4x4 m = cam.worldToCameraMatrix;
		Vector3 cpos = m.MultiplyPoint( offsetPos );
		Vector3 cnormal = m.MultiplyVector( normal ).normalized * sideSign;
		return new Vector4( cnormal.x, cnormal.y, cnormal.z, -Vector3.Dot(cpos,cnormal) );
	}
	

	private static void CalculateReflectionMatrix (ref Matrix4x4 reflectionMat, Vector4 plane)
	{

	    reflectionMat.m00 = (1F - 2F*plane[0]*plane[0]);
	    reflectionMat.m01 = (-2F*plane[0]*plane[1]);
	    reflectionMat.m02 = (-2F*plane[0]*plane[2]);
	    reflectionMat.m03 = (-2F*plane[3]*plane[0]);

	    reflectionMat.m10 = (-2F*plane[1]*plane[0]);
	    reflectionMat.m11 = (1F-2F*plane[1]*plane[1]);
	    reflectionMat.m12 = (-2F*plane[1]*plane[2]);
	    reflectionMat.m13 = (-2F*plane[3]*plane[1]);
	
    	reflectionMat.m20 = (-2F*plane[2]*plane[0]);
    	reflectionMat.m21 = (-2F*plane[2]*plane[1]);
    	reflectionMat.m22 = (1F-2F*plane[2]*plane[2]);
    	reflectionMat.m23 = (-2F*plane[3]*plane[2]);

    	reflectionMat.m30 = 0F;
    	reflectionMat.m31 = 0F;
    	reflectionMat.m32 = 0F;
    	reflectionMat.m33 = 1F;
	}
}
