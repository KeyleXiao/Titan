using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class CharacterShadow : MonoBehaviour
{
	[System.Serializable] 
    public class FocusSetup
    {
		public bool autoFocus;
		public float autoFocusRadiusBias;
		public Transform target;
		public Vector3 offset;
		public float radius = 5f;
		public float depthBias = 0.001f;
		public float sceneCaptureDistance = 0f;
	}

    public Renderer BodyRender = null;
    private List<Renderer> allRender = null;
    public FocusSetup[] shadowFoci;

	public enum Dimension
    {
		x256	= 256,
		x512	= 512,
		x1024	= 1024,
		//x2048	= 2048,
		//x4096	= 4096,
		//x8192	= 8192,
	}

    private Dimension shadowMapSize = Dimension.x1024;
    private float cullingDistance = 15f;
    private LayerMask cullingMask = 14;//LayerMask.NameToLayer( Config.LayerLightingEffectOnly);

    /// <summary>
    /// 接收所有物体投影
    /// </summary>
    private bool useSceneCapture = true;


    private float blockerSearchDistance = 24f;
    private float blockerDistanceScale = 1f;
    private float lightNearSize = 4f;
    private float lightFarSize = 22f;
    private float fallbackFilterWidth = 6f;

    private int startFocus;

	int m_downscale = 0;

	int m_activeFocus = -1;
	Light m_lightSource;
	List<Material> m_AllMaterial;

	Texture2D	ms_shadowTextureFakePoint;
	static int			ms_shadowMatrixID, ms_shadowTextureID;

	RenderTexture m_shadowTexture;
	Matrix4x4 m_shadowMatrix;
	Camera m_shadowCamera;
	Matrix4x4 m_shadowSpaceMatrix;
    Shader CharacterShadowDepthShader;
    
    bool isValid = false;
    bool bInit = false;

    Camera hostSceneCamera = null;


    private List<Material> OrginalMaterial = null;
    private List<Material> ShadowMaterials = null;

    private bool bApplyhadowMaterial = false;
    Mesh SharedMesh = null;
    MeshFilter mf = null;
    MeshRenderer meshRender = null;

    private List<int> OldBodyRenderLayer = null;
    public void Bake(Transform target, Renderer BodyRender)
    {

        if (!BodyRender)
        {
            return;
        }
        shadowFoci = new FocusSetup[1];
        FocusSetup instanceSetUp = new FocusSetup();

        Vector3 oldPos = BodyRender.transform.localPosition;
        Quaternion oldRot = BodyRender.transform.localRotation;
        Vector3 oldScale = BodyRender.transform.localScale;
        BodyRender.transform.localPosition = Vector3.zero;
        BodyRender.transform.localRotation = Quaternion.identity;
        BodyRender.transform.localScale = Vector3.one;

        SphereCollider collider = BodyRender.gameObject.AddComponent<SphereCollider>();
        shadowFoci[0] = instanceSetUp;
        instanceSetUp.autoFocus = false;
        instanceSetUp.autoFocusRadiusBias = 0.0f;
        instanceSetUp.target = target;
        instanceSetUp.radius = collider.radius;
        instanceSetUp.offset = collider.center;
        instanceSetUp.depthBias = 0.001f;
        instanceSetUp.sceneCaptureDistance = 0f;

        this.BodyRender = null;
        this.BodyRender = BodyRender;

        GameObject.DestroyImmediate(collider);
        BodyRender.transform.localRotation = oldRot;
        BodyRender.transform.localPosition = oldPos;
        BodyRender.transform.localScale = oldScale;

        if(!Initialize.isOnGameRuning)
        {
            Debug.Log("CharacterShadow-Bake成功");
        }


    }

    private void CheckValid()
    {
        if (!BodyRender)
        {
            isValid = false;
            Debug.LogWarning("渲染器为空");
            return;
        }

        Renderer re = GetComponent<Renderer>();
        if(re)
        {
            isValid = false;
            Debug.LogWarning("物体上不能挂在任意的Renderer组件，如MeshRender，SkinnedMeshRendere等");
            return;
        }

        MeshFilter mf = GetComponent<MeshFilter>();
        if (re)
        {
            isValid = false;
            Debug.LogWarning("物体上不能挂在任意的MeshFilter组件");
            return;
        }
        if (shadowFoci == null)
        {
            isValid = false;
            Debug.LogWarning("ShowFoucs 为空");
            return;
        }
        if(shadowFoci.Length <= 0)
        {
            isValid = false;
            Debug.LogWarning("ShowFoucs 为空");
            return;
        }

        if(!shadowFoci[0].target)
        {
            isValid = false;
            Debug.LogWarning("ShowFoucs target为空");
            return;

        }
        //if (!BodyRender.receiveShadows)
        //{
        //    return;
        //}
        isValid = true;
    }

    private Shader GetShadowShader(string sourceShaderName)
    {
        if (sourceShaderName.Contains("Standard (Specular setup)"))
        {
            return AssetBundleManager.GetShader("Character/SelfShadow/Standard (Specular setup)");
        }
        else if (sourceShaderName.Contains("Standard"))
        {
            return AssetBundleManager.GetShader("Character/SelfShadow/Standard");
        }
        return null;
    }

    public void Start()
    {
        Init(Initialize.mainCam ? Initialize.mainCam : Camera.main);
        ApplyShadowMaterial();
    }

    public void Init(Camera hostCamera)
    {
        if (bInit)
        {
            return;
        }
        bInit = true;
        this.hostSceneCamera = hostCamera;
        CheckValid();

        if(!isValid)
        {
            return;
        }
        if (!CharacterShadowDepthShader)
        {
            CharacterShadowDepthShader = AssetBundleManager.GetShader("Character/SelfShadow/DepthOnly");
        }
		if(!ms_shadowTextureFakePoint) 
        {
			ms_shadowTextureFakePoint = new Texture2D(1, 1, TextureFormat.Alpha8, false, true);
			ms_shadowTextureFakePoint.filterMode = FilterMode.Point;
			ms_shadowTextureFakePoint.SetPixel(0, 0, new Color(0f, 0f, 0f, 0f));
			ms_shadowTextureFakePoint.Apply(false, true);

			ms_shadowMatrixID = Shader.PropertyToID("u_CharacterShadowMatrix");
			ms_shadowTextureID = Shader.PropertyToID("u_CharacterShadowTexture");
		}

		EnsureLightSource();

		m_shadowMatrix = Matrix4x4.identity;
        GameObject shadowCameraGO = new GameObject("#> _CharacterShadow Camera < " + this.GetInstanceID());
		shadowCameraGO.hideFlags = HideFlags.HideAndDontSave;
		m_shadowCamera = shadowCameraGO.AddComponent<Camera>();
		m_shadowCamera.renderingPath = RenderingPath.Forward;
		m_shadowCamera.clearFlags = CameraClearFlags.Depth;
		m_shadowCamera.depthTextureMode = DepthTextureMode.None;
		m_shadowCamera.useOcclusionCulling = false;
		m_shadowCamera.cullingMask = -1;
        m_shadowCamera.cullingMask &= ~(1 << LayerMask.NameToLayer(Config.LayerLightingEffectOnly));
		m_shadowCamera.orthographic = true;
		m_shadowCamera.depth = -100;
		m_shadowCamera.aspect = 1f;
		m_shadowCamera.SetReplacementShader(CharacterShadowDepthShader, "RenderType");
		m_shadowCamera.enabled = false;
		
		SetFocus(startFocus);


        OnValidate();
	}

    private void InitRender()
    {

        if(null != allRender)
        {
            return;
        }
        m_AllMaterial = new List<Material>();

        allRender = new List<Renderer>();
        List<Renderer> tempRenderList = new List<Renderer>();
        tempRenderList.AddRange(GetComponentsInChildren<SkinnedMeshRenderer>());
        tempRenderList.AddRange(GetComponentsInChildren<MeshRenderer>());
        OrginalMaterial = new List<Material>();
        ShadowMaterials = new List<Material>();
        OldBodyRenderLayer = new List<int>();
        foreach (Renderer re in tempRenderList)
        {
            Material tOrginalMaterial = re.sharedMaterial;
            if(!tOrginalMaterial)
            {
                continue;
            }
            Shader shader = GetShadowShader(tOrginalMaterial.shader.name);
            if(!shader)
            {
                continue;
            }
            Material tShadowMaterials = new Material(tOrginalMaterial);
            tShadowMaterials.name = tOrginalMaterial.name + " (CharacterShadowMaterial)";
           
            tShadowMaterials.shader = shader ? shader : tOrginalMaterial.shader;

            tShadowMaterials.CopyPropertiesFromMaterial(tOrginalMaterial);
            ApplyShaderParams(tShadowMaterials);

            allRender.Add(re);
            m_AllMaterial.Add(tShadowMaterials);
            OrginalMaterial.Add(tOrginalMaterial);
            ShadowMaterials.Add(tShadowMaterials);
            OldBodyRenderLayer.Add(re.gameObject.layer);
        }
        tempRenderList.Clear();


        if (m_AllMaterial.Count > 0)
        {
            SharedMesh = new Mesh();
            SharedMesh.bounds = new Bounds(Vector3.zero, Vector3.one * 1000f);
            SharedMesh.hideFlags = HideFlags.HideAndDontSave;
            mf = gameObject.AddComponent<MeshFilter>();
            mf.sharedMesh = SharedMesh;
            meshRender = gameObject.AddComponent<MeshRenderer>();
            meshRender.shadowCastingMode = UnityEngine.Rendering.ShadowCastingMode.Off;
            meshRender.reflectionProbeUsage = UnityEngine.Rendering.ReflectionProbeUsage.Off;
            meshRender.lightProbeUsage = UnityEngine.Rendering.LightProbeUsage.Off;
        }
    }

    public void ResetMaterial()
    {

        if (!isValid)
        {
            return;
        }
        if (!bApplyhadowMaterial)
        {
            return;
        }
       
        if(BodyRender)
        {
            ReleaseTarget();
            ApplyCharacterShadowStauts();

            for (int i = 0; i < allRender.Count;i++ )
            {
                allRender[i].gameObject.layer = OldBodyRenderLayer[i];
                allRender[i].sharedMaterial = OrginalMaterial[i];
            }

            bApplyhadowMaterial = false;
        }
    }

    public void ApplyShadowMaterial()
    {

        if (!isValid)
        {
            return;
        }
        if (bApplyhadowMaterial)
        {
            return;
        }
        if (!CharacterShadowSun.instance)
        {
            return;
        }
        if(!hostSceneCamera)
        {
            return;
        }
        if (hostSceneCamera != Initialize.mainCam)
        {
            return;
        }
        //距离太远，不要
        if(GameUtil.GetSqrDistanceWithOutY(hostSceneCamera.transform.position,BodyRender.transform.position) > 200f)
        {
            return;
        }

        if (BodyRender)
        {
            InitRender();
            ReleaseTarget();
            AllocateTarget();
            ApplyCharacterShadowStauts();
            
            for (int i = 0; i < allRender.Count; i++)
            {
                OldBodyRenderLayer[i] = allRender[i].gameObject.layer ;
                allRender[i].gameObject.layer = LayerMask.NameToLayer(Config.LayerDefault);
                allRender[i].sharedMaterial = ShadowMaterials[i];
            }
            bApplyhadowMaterial = true;
        }
    }

	public void OnEnable() 
    {
        if (!isValid)
        {
            return;
        }

        hostSceneCamera = Initialize.mainCam;
        ApplyShadowMaterial();
	}

    public void OnDisable()
    {
        if (!isValid)
        {
            return;
        }
        ResetMaterial();
	}

    public void OnDestroy()
    {
        bInit = false;
        if (!isValid)
        {
            return;
        }
        if (SharedMesh)
        {
            Object.DestroyImmediate(SharedMesh);
        }
        if (mf)
        {
            Object.Destroy(mf);
        }
        if (meshRender)
        {
            Object.Destroy(meshRender);
        }
		if(m_shadowCamera)
        {
            Object.Destroy(m_shadowCamera.gameObject);
        }

        if(ShadowMaterials != null)
        {
            foreach(Material mat in ShadowMaterials)
            {
                Object.Destroy(mat);
            }
            ShadowMaterials.Clear();
            ShadowMaterials = null;
        }

        if (OldBodyRenderLayer != null)
        {
            OldBodyRenderLayer.Clear();
            OldBodyRenderLayer = null;
        }

        if(null != allRender)
        {
            allRender.Clear();
            allRender = null;
        }
        if (ms_shadowTextureFakePoint)
        {
            Object.DestroyImmediate(ms_shadowTextureFakePoint);
        }
	}

    public void OnValidate()
    {
        if (!Application.isPlaying || !m_shadowCamera)
            return;

        ReleaseTarget();
        AllocateTarget();

        if (m_AllMaterial != null)
            for (int i = 0, n = m_AllMaterial.Count; i < n; ++i)
                ApplyShaderParams(m_AllMaterial[i]);
        m_shadowCamera.cullingMask = -1;
        m_shadowCamera.cullingMask &= ~(1 << LayerMask.NameToLayer(Config.LayerLightingEffectOnly));

        SetFocus(m_activeFocus >= 0 ? m_activeFocus : startFocus);
        ApplyCharacterShadowStauts();
    }

	void AllocateTarget() 
    {
        if (!isValid)
        {
            return;
        }
        if(!m_shadowTexture)
        {
            m_shadowTexture = new RenderTexture((int)shadowMapSize >> m_downscale, (int)shadowMapSize >> m_downscale, 16, RenderTextureFormat.Shadowmap, RenderTextureReadWrite.Linear);
            m_shadowTexture.filterMode = FilterMode.Bilinear;
            m_shadowTexture.useMipMap = false;
            m_shadowTexture.generateMips = false;
            m_shadowCamera.targetTexture = m_shadowTexture;
        }

	}

	void ReleaseTarget()
    {
        if (!isValid)
        {
            return;
        }
        if (m_shadowTexture)
        {
            m_shadowCamera.targetTexture = null;
            Object.DestroyImmediate(m_shadowTexture);
            m_shadowTexture = null;
        }

	}

	void ApplyCharacterShadowStauts()
    {
 
	}

	void ApplyShaderParams(Material m)
    {
        if (!isValid)
        {
            return;
        }
		m.SetTexture("u_CharacterShadowTextureFakePoint", ms_shadowTextureFakePoint);

		// We want the same 'softness' regardless of texture resolution.
		float texelsInMap = (float)(int)shadowMapSize;
        float relativeTexelSize = texelsInMap / 2048f;

		m.SetVector("u_CharacterShadowFilterWidth", new Vector2(1f / (float)(int)shadowMapSize, 1f / (float)(int)shadowMapSize) * fallbackFilterWidth * relativeTexelSize);

        float characterShadowBlockerWidth = relativeTexelSize * blockerSearchDistance / texelsInMap;
		m.SetVector("u_CharacterShadowBlockerWidth", Vector4.one * characterShadowBlockerWidth);

		// This needs to run each frame if we start using multiple foci.
		FocusSetup focus = shadowFoci[m_activeFocus];
        float characterShadowBlockerDistanceScale = blockerDistanceScale * focus.radius * 0.5f / 10f; // 10 samples in shader
		m.SetFloat("u_CharacterShadowBlockerDistanceScale", characterShadowBlockerDistanceScale);
		
		Vector2 CharacterShadowLightWidth = new Vector2(lightNearSize, lightFarSize) * relativeTexelSize / texelsInMap;
		m.SetVector("u_CharacterShadowLightWidth", CharacterShadowLightWidth);
	}

    /// <summary>
    /// 灯光检测
    /// </summary>
    /// <returns></returns>
	bool EnsureLightSource() 
    {
        if (!isValid)
        {
            return false;
        }
		bool hadValidLight = m_lightSource;
		bool hadCookie = m_lightSource && m_lightSource.cookie;
        m_lightSource = CharacterShadowSun.instance;

		// Only capture shadows from the light's culling mask.
		if(useSceneCapture && m_lightSource && m_shadowCamera)
        {
            m_shadowCamera.cullingMask = -1;
            m_shadowCamera.cullingMask &= ~(1 << LayerMask.NameToLayer(Config.LayerLightingEffectOnly));
            //m_shadowCamera.cullingMask = m_lightSource.cullingMask;
        }
		return hadValidLight != m_lightSource || hadCookie != (m_lightSource && m_lightSource.cookie);
	}

	void UpdateAutoFocus(FocusSetup focus) 
    {
        if (!isValid)
        {
            return;
        }
		if(!focus.autoFocus)
        {
            return;
        }


		var targetPos = focus.target.position + focus.target.right * focus.offset.x
			+ focus.target.up * focus.offset.y + focus.target.forward * focus.offset.z;

        Bounds bounds = new Bounds(targetPos, Vector3.one * 0.1f);
        bounds.Encapsulate(BodyRender.bounds);
		focus.offset = bounds.center - focus.target.position;
		focus.radius = focus.autoFocusRadiusBias + bounds.extents.magnitude;
	}

	void SetFocus(int idx)
    {
        if (!isValid)
        {
            return;
        }
		if(idx < 0 || idx >= shadowFoci.Length)
        {
			Debug.LogError("Invalid active focus: " + m_activeFocus);
			return;
		}
		
		m_activeFocus = idx;
		
		FocusSetup focus = shadowFoci[m_activeFocus];
		UpdateAutoFocus(focus);

		m_shadowCamera.orthographicSize = focus.radius;
		m_shadowCamera.nearClipPlane = useSceneCapture ? -focus.sceneCaptureDistance : 0f;
		m_shadowCamera.farClipPlane = focus.radius * 2f;
		m_shadowCamera.projectionMatrix = GL.GetGPUProjectionMatrix(Matrix4x4.Ortho(-focus.radius, focus.radius, -focus.radius, focus.radius, 0f, focus.radius * 2f), false);

		bool isD3D9 = SystemInfo.graphicsDeviceType == UnityEngine.Rendering.GraphicsDeviceType.Direct3D9;
        bool isD3D = isD3D9 || SystemInfo.graphicsDeviceType == UnityEngine.Rendering.GraphicsDeviceType.Direct3D11;
		float to = isD3D9 ? 0.5f / (float)(int)shadowMapSize : 0f;
		float zs = isD3D ? 1f : 0.5f, zo = isD3D ? 0f : 0.5f;
		float db = -focus.depthBias;
		m_shadowSpaceMatrix.SetRow(0, new Vector4(0.5f, 0.0f, 0.0f, 0.5f + to));
		m_shadowSpaceMatrix.SetRow(1, new Vector4(0.0f, 0.5f, 0.0f, 0.5f + to));
		m_shadowSpaceMatrix.SetRow(2, new Vector4(0.0f, 0.0f,   zs,   zo + db));
		m_shadowSpaceMatrix.SetRow(3, new Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	}

    /// <summary>
    /// 更新焦点
    /// </summary>
	void UpdateFocus()
    {
        if (!isValid)
        {
            return;
        }

		FocusSetup focus = shadowFoci[m_activeFocus];
		
		Vector3 targetPos = focus.target.position + focus.target.right * focus.offset.x
			+ focus.target.up * focus.offset.y + focus.target.forward * focus.offset.z;
        Vector3 lightDir = m_lightSource.transform.forward;
        Quaternion lightOri = m_lightSource.transform.rotation;

        m_shadowCamera.transform.position = targetPos - lightDir * focus.radius;
		m_shadowCamera.transform.rotation = lightOri;

		Matrix4x4 shadowViewMat = m_shadowCamera.worldToCameraMatrix;
        Matrix4x4 shadowProjMat = GL.GetGPUProjectionMatrix(m_shadowCamera.projectionMatrix, false);
		m_shadowMatrix = m_shadowSpaceMatrix * shadowProjMat * shadowViewMat;
	}

    /// <summary>
    /// 是否可见
    /// </summary>
    /// <param name="cam"></param>
    /// <returns></returns>
	bool CheckVisibility(Camera cam)
    {
        if (!isValid)
        {
            return false;
        }
		FocusSetup focus = shadowFoci[m_activeFocus];
		UpdateAutoFocus(focus);

		var targetPos = focus.target.position + focus.target.right * focus.offset.x
			+ focus.target.up * focus.offset.y + focus.target.forward * focus.offset.z;
		var bounds = new Bounds(targetPos, Vector3.one * focus.radius * 2f);

		return (targetPos - cam.transform.position).sqrMagnitude < (cullingDistance * cullingDistance)
			&& GeometryUtility.TestPlanesAABB(GeometryUtility.CalculateFrustumPlanes(cam), bounds);
	}

    /// <summary>
    /// 检查一下是不是主摄像机
    /// </summary>
    /// <param name="cam"></param>
    /// <returns></returns>
	bool CheckCamera(Camera cam)
    {

        Camera TargetCam = Initialize.mainCam ? Initialize.mainCam : Camera.main;
        if (cam == TargetCam)
        {
            return true;
        }

		return false;
	}

    public void OnWillRenderObject()
    {
        if (!isValid)
        {
            return;
        }

        if(!CharacterShadowSun.instance)
        {
            return;
        }

        if(hostSceneCamera != Initialize.mainCam)
        {
            return;
        }
		if(EnsureLightSource())
        {
            ApplyCharacterShadowStauts();
        }


		if(!m_lightSource)
			return;

        var cam = Camera.current;
		if(!CheckCamera(cam))
        {
            return;
        }


		if(!CheckVisibility(cam))
        {
            return;
        }


		UpdateFocus();

		var shadowDistance = QualitySettings.shadowDistance;
		QualitySettings.shadowDistance = 0f;
		m_shadowCamera.Render();

		QualitySettings.shadowDistance = shadowDistance;

		for(int i = 0, n = m_AllMaterial.Count; i < n; ++i) 
        {
			var m = m_AllMaterial[i];
			m.SetTexture(ms_shadowTextureID, m_shadowTexture);
			m.SetMatrix(ms_shadowMatrixID, m_shadowMatrix);
		}
	}

    public void OnDrawGizmosSelected()
    {
		if(shadowFoci == null)
			return;

		foreach(var f in shadowFoci)
        {
			if(f.target == null)
            {
                continue;
            }
			Gizmos.color = f.autoFocus ? Color.cyan : Color.green;

			var p = f.target.position + f.target.right * f.offset.x	+ f.target.up * f.offset.y + f.target.forward * f.offset.z;
			Gizmos.DrawWireSphere(p, f.radius + (f.autoFocus ? f.autoFocusRadiusBias : 0f));
		}
	}
}