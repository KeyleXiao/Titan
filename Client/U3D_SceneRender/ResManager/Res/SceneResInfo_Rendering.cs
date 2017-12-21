/*******************************************************************
** 文件名:	SceneResInfo_Rendering.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/7/20
** 版  本:	1.0
** 描  述:	场景资源信息,渲染部分
** 应  用:   场景资源信息，用于做场景资源管理

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public partial class SceneResInfo : SceneInfo
{
    /// <summary>
    /// 优化距离，这个是根据物体的包围盒，再加上这个距离来做的。
    /// </summary>
    private static float RenderingOptimitzedDistance = 60.0f;

    private List<SceneResInfo> ShareResList = null;
    public float ObjectHeight = 9999;
    [System.NonSerialized]
    public List<int> BatchesIDs = new List<int>();

    [System.NonSerialized]
    public int OrginalLayer = 0;

    [System.NonSerialized]
    public Material[] OrginalMaterList = null;

    /// <summary>
    /// 原始的实例材质球，因为访问render.material会有实例出来,他与orginalmateriallist是一一对应的
    /// 用于半透
    /// </summary>
    [System.NonSerialized]
    public Material[] AlphaInstanceMaterList = null;

    public bool AlwaysUnOptimitzedMemory = false;

    public bool isTerrain = false;

    /// <summary>
    /// 是不是草，草不开阴影
    /// </summary>
    [System.NonSerialized]
    public bool isGrass = false;

    /// <summary>
    /// 是否被优化了，默认是已优化了的，因为bake是优化了。
    /// </summary>
    public bool bOptimitzedRendering = true;

    /// <summary>
    /// 永远不优化渲染
    /// </summary>
    public bool AlwaysVisible = false;

    /// <summary>
    /// 永远没有阴影
    /// </summary>
    public bool AlwaysNotShadow = false;

    /// <summary>
    /// 四叉树控制的可见性
    /// </summary>
    private bool QuadTreeNodeAcitiveVisible = false;


    /// <summary>
    /// 旧的阴影选项
    /// </summary>
    public UnityEngine.Rendering.ShadowCastingMode OrginalShadowCastingMode = UnityEngine.Rendering.ShadowCastingMode.Off;
    private Vector3 RenderingBounds_Min = Vector3.zero;
    private Vector3 RenderingBounds_Max = Vector3.zero;

    private HashSet<Material> supportOptimitzedMatList = new HashSet<Material>();


    /// <summary>
    /// 批次的实例材质球
    /// </summary>
    Material[] Batch_instanceMaterials = null;

    public static string CutOutKeyWorld = "_DisslovePower";


    private Ray CheckRay_Top = new Ray();
    private Ray CheckRay_Down = new Ray();

    /// <summary>
    /// 是否被物体挡住了_摄像机
    /// </summary>
    private bool isOcclusFadingEnabled_Camera = false;


    /// <summary>
    /// 是否被物体挡住了_上帝
    /// </summary>
    private bool isOcclusFadingEnabled_GodEye = false;

    private bool isOcclusFadingEnabled_ShareCollider = false;

    /// <summary>
    /// 有上面决定的值
    /// </summary>
    private bool isOcclusFadingEnabled = false;

    /// <summary>
    /// 被物体挡住效果是否有效
    /// </summary>
    private bool isOcclusValiding = false;


    private bool EnableOcclusFading_Camera = true;
    //private float RayCameraDistance = 0.0f;
    private float PlayerCameraDistance = 0.0f;
    private static float HightObj = 7.0f;
    private List<Collider> m_HostColliderList = new List<Collider>();

    /// <summary>
    /// 源材质和实例材质的表,用于半透
    /// </summary>
    private Dictionary<Material, Material> Orginal_AlphaInstanceTable = new Dictionary<Material, Material>();

    Collider[] HitColliderBuffer = new Collider[1];

    private void UpdateVisible()
    {
        bool bVisible = QuadTreeNodeAcitiveVisible && getOcclusVisible() || AlwaysVisible;
        if(hostRender)
        {
            hostRender.enabled = bVisible;
        }
    }

    /// <summary>
    /// 优化渲染
    /// </summary>
    /// <param name="matList"></param>
    public void OptimitzedRendering(List<Material> matList)
    {
        if (bOptimitzedRendering)
        {
            return;
        }

        bOptimitzedRendering = true;
        if(!AlwaysVisible)
        {
            //关闭阴影
            hostRender.shadowCastingMode = UnityEngine.Rendering.ShadowCastingMode.Off;
        }
        if(AlwaysVisible || !CheckSupportFading())
        {
            return;
        }

        QuadTreeNodeAcitiveVisible = false;
        DisableCullingCheck();
        if (hostResManager)
        {
            hostResManager.OnQuadTreeCull(this);
        }
        UpdateFadeOut(matList);  
    }

    public void ResetMaterial()
    {
        if (!hostRender)
        {
            return;
        }
        if (null != OrginalMaterList)
        {
            hostRender.sharedMaterials = OrginalMaterList;
        }

    }

    public void ResetRendering()
    {
        if(!hostRender)
        {
            return;
        }

        ResetMaterial();
        bOptimitzedRendering = true;
        if(!AlwaysVisible)
        {
            QuadTreeNodeAcitiveVisible = false;
            DisableCullingCheck();
            UpdateVisible();
        }

        isOcclusFadingEnabled_Camera = false;
        isOcclusFadingEnabled_GodEye = false;
        isOcclusFadingEnabled = false;
        isOcclusValiding = false;

        hostRender.shadowCastingMode = UnityEngine.Rendering.ShadowCastingMode.Off;
        Batch_instanceMaterials = null;
        BatchesIDs.Clear();
    }

    /// <summary>
    /// 不优化渲染
    /// </summary>
    public void UnOptimitzedRendering(List<Material> matList)
    {
        if (!bOptimitzedRendering)
        {
            return;
        }

        bOptimitzedRendering = false;
        if (AlwaysNotShadow)
        {
            hostRender.shadowCastingMode = UnityEngine.Rendering.ShadowCastingMode.Off;
        }
        else
        {
            if (isGrass && !SceneResInfoManager.enabledGrassShadow)
            {
                hostRender.shadowCastingMode = UnityEngine.Rendering.ShadowCastingMode.Off;
            }
            else
            {
                hostRender.shadowCastingMode = OrginalShadowCastingMode;
            }
            
        }
        if (AlwaysVisible || !CheckSupportFading())
        {
            return;
        }
        
        QuadTreeNodeAcitiveVisible = true;
        EnableCullingCheck();
        UpdateFadeIn(matList);

    }


    public Material[] UpdateFadeOut(List<Material> matList)
    {
        if (null != matList)
        {
            int cout = BatchesIDs.Count;
            int length = hostRender.sharedMaterials.Length;
            if (cout != length)
            {
                Debug.LogError("SceneResInfo:cout != length" + name);
                return null;
            }
            if (null == Batch_instanceMaterials)
            {
                Batch_instanceMaterials = new Material[length];
            }
            for (int i = 0; i < cout; i++)
            {
                Batch_instanceMaterials[i] = matList[BatchesIDs[i]];
            }
            hostRender.materials = Batch_instanceMaterials;
            return Batch_instanceMaterials; 
        }
        return null;
    }

    public Material[] UpdateFadeIn(List<Material> matList)
    {
        //淡入的话要立刻更新，要不然的话，会看不见淡入的效果
        UpdateVisible();
        if (null != matList)
        {
            int cout = BatchesIDs.Count;
            int length = hostRender.sharedMaterials.Length;
            if (cout != length)
            {
                Debug.LogError("SceneResInfo:cout != length" + name);
                return null;
            }
            if (null == Batch_instanceMaterials)
            {
                Batch_instanceMaterials = new Material[length];
            }
            for (int i = 0; i < cout; i++)
            {
                Batch_instanceMaterials[i] = matList[BatchesIDs[i]];
            }
            hostRender.materials = Batch_instanceMaterials;
            return Batch_instanceMaterials; 
        }
        return null;
    }

    public void CheckOcclus()
    {
        //只有战场才有这个功能
        if(GameLogicAPI.isInWarScene() <= 0 || !hostResManager.IsActive || BaseStageManager.Instance.CurSubState == EStage_SubState.ESSS_WAREND)
        {
            return;
        }
        //地形不做这个检测并且没有被优化的，因为优化的已经没了
        if (AlwaysVisible && !isTerrain && !bOptimitzedRendering && BaseSoldierCamera.MainInstance())
        {
            //已经有一个了，不需要进行下面的计算了
            if(isOcclusFadingEnabled_GodEye)
            {
                UpdateOcclusFading();
                return;
            }
            if(!EnableOcclusFading_Camera )
            {
                return;
            }
            Vector3 pos = mainCameraTransform.position;
            isOcclusFadingEnabled_Camera = false;
            GameObject view = BaseSoldierCamera.MainInstance().GetSoliderGo();
            float heightInMeters = BaseSoldierCamera.MainInstance().GetSoliderHeightInMeters();
            if(view == null)
            {
                return;
            }

            if (view != null && view.gameObject)
            {
               
                Vector3 mainPlayPos = view.transform.position;
                mainPlayPos.y += heightInMeters;
                CheckRay_Top.origin = mainPlayPos;
                Vector3 dir = pos - mainPlayPos;
                PlayerCameraDistance = Vector3.Distance(mainPlayPos, pos);

                CheckRay_Top.direction = dir.normalized;
                RaycastHit hitInfo;

                foreach (Collider col in m_HostColliderList)
                {
                    if (col.Raycast(CheckRay_Top, out hitInfo, PlayerCameraDistance))
                    {
                        isOcclusFadingEnabled_Camera = true;
                        UpdateOcclusFading();
                        UpdateShareCollider(true);
                        return;
                    }
                }

                mainPlayPos = view.gameObject.transform.position;
                mainPlayPos.y += heightInMeters * 0.2f;
                CheckRay_Down.origin = mainPlayPos;
                dir = pos - mainPlayPos ;
               
                PlayerCameraDistance = Vector3.Distance(mainPlayPos, pos);

                CheckRay_Down.direction = dir.normalized;
                if (!isOcclusFadingEnabled_Camera)
                {
                    foreach (Collider col in m_HostColliderList)
                    {
                        if (col.Raycast(CheckRay_Down, out hitInfo, PlayerCameraDistance))
                        {
                            isOcclusFadingEnabled_Camera = true;
                            UpdateOcclusFading();
                            UpdateShareCollider(true);
                            return;
                        }

                        int oldLayer = col.gameObject.layer;
                        col.gameObject.layer = LayerMask.NameToLayer(Config.LayerLightingEffectOnly);

                        int cout = Physics.OverlapSphereNonAlloc(pos, 1.5f, HitColliderBuffer, 1 << LayerMask.NameToLayer(Config.LayerLightingEffectOnly));
                        if (cout > 0)
                        {
                            isOcclusFadingEnabled_Camera = true;
                            UpdateOcclusFading();
                            UpdateShareCollider(true);
                            col.gameObject.layer = oldLayer;
                            return;
                        }
                        col.gameObject.layer = oldLayer;
                    }
                }
                UpdateOcclusFading();
                UpdateShareCollider(false);
            }
        }
    }

    public void UpdateShareCollider(bool isOcclus)
    {
        if(null != ShareResList && ShareResList.Count > 0)
        {
            foreach(SceneResInfo res in ShareResList)
            {
                if (isOcclus)
                {
                    res.EnableOcclusFadingFromShareCollider();
                }
                else
                {
                    res.DisableOcclusFadignFromShareCollider();
                }
            }
        }
    }

    private void EnableOcclusFadingFromShareCollider()
    {
        isOcclusFadingEnabled_ShareCollider = true;
        UpdateOcclusFading();
    }

    private void DisableOcclusFadignFromShareCollider()
    {
        isOcclusFadingEnabled_ShareCollider = false;
        UpdateOcclusFading();
    }

    public void UpdateOcclusFading()
    {
        isOcclusFadingEnabled = isOcclusFadingEnabled_Camera || isOcclusFadingEnabled_GodEye || isOcclusFadingEnabled_ShareCollider;
        if(isOcclusFadingEnabled)
        {
            inOcclusFading();
        }
        else
        {
            outOcclusFading();
        }
    }

    public void inOcclusFading()
    {
        if (isOcclusValiding)
        {
            return;
        }
        if (!AlwaysVisible || null == AlphaInstanceMaterList)
        {
            return;
        }
        isOcclusValiding = true;
        int count = AlphaInstanceMaterList.Length;
        if (count > 0)
        {
            hostRender.materials = AlphaInstanceMaterList;
        }
    }


    public void outOcclusFading()
    {
        if (!isOcclusValiding)
        {
            return;
        }
        if (!AlwaysVisible)
        {
            return;
        }
        if(null == AlphaInstanceMaterList)
        {
            return;
        }
        isOcclusValiding = false;
        int count = AlphaInstanceMaterList.Length;
        if (count > 0)
        {
            hostRender.materials = OrginalMaterList;
        }
    }

    /// <summary>
    /// 根据给的位置来判断
    /// </summary>
    /// <param name="pos"></param>
    /// <returns></returns>
    bool Contains_Rendering(Vector3 pos)
    {

        if (pos.x > RenderingBounds_Min.x && pos.y > RenderingBounds_Min.y && pos.z > RenderingBounds_Min.z &&
            pos.x < RenderingBounds_Max.x && pos.y < RenderingBounds_Max.y && pos.z < RenderingBounds_Max.z)
        {
            return true;
        }
        return false;
    }


    /// <summary>
    /// 在bake的时候创建这些
    /// </summary>
    public void CreateAlphaMaterial()
    {
        if (!AlwaysVisible)
        {
            return;
        }

        if (AlphaInstanceMaterList == null)
        {
            AlphaInstanceMaterList = new Material[hostRender.sharedMaterials.Length];
            for (int j = 0; j < AlphaInstanceMaterList.Length; j++)
            {
                if (!hostRender.sharedMaterials[j])
                {
                    AlphaInstanceMaterList[j] = null;
                    continue;
                }

                if (!Orginal_AlphaInstanceTable.TryGetValue(hostRender.sharedMaterials[j], out AlphaInstanceMaterList[j]))
                {
                    AlphaInstanceMaterList[j] = new Material(AssetBundleManager.GetShader("Scene/Optitmized/Transparent"));
                    AlphaInstanceMaterList[j].name += "-Instance";
                    AlphaInstanceMaterList[j].SetFloat(CutOutKeyWorld, 0.3f);
                   // SetInstanceMaterialParams_Alpha(AlphaInstanceMaterList[j]);

                    Orginal_AlphaInstanceTable.Add(hostRender.sharedMaterials[j], AlphaInstanceMaterList[j]);
                }




            }
        }
    }

    public void ClearAlphaMaterial()
    {
        if (!AlwaysVisible)
        {
            return;
        }

        if (AlphaInstanceMaterList != null)
        {
            for (int j = 0; j < AlphaInstanceMaterList.Length; j++)
            {
                ResNode.DestroyRes(ref AlphaInstanceMaterList[j]);
            }
            AlphaInstanceMaterList = null;
        }
    }

    


    /// <summary>
    /// 初始化渲染相关的东西
    /// </summary>
    void InitRendering()
    {
        OrginalLayer = hostRender.gameObject.layer;
        if (OrginalMaterList == null)
        {
            OrginalMaterList = hostRender.sharedMaterials;
            int cou = OrginalMaterList.Length;
            for(int i = 0;i < cou;i++)
            {
                SetMaterialParams(OrginalMaterList[i]);
            }
        }
        SceneShareCollider ShareColliderRender = GetComponent<SceneShareCollider>();
        if (ShareColliderRender)
        {
            if (null != ShareColliderRender.ShareList && ShareColliderRender.ShareList.Count > 0)
            {
                ShareResList = new List<SceneResInfo>();
                foreach (Renderer re in ShareColliderRender.ShareList)
                {
                    SceneResInfo sc = re.GetComponent<SceneResInfo>();
                    if (sc)
                    {
                        ShareResList.Add(sc);
                    }
                }
            }
        }


        UpdateCollider();
        CreateAlphaMaterial();

        UpdateVisible();
        if (BaseSoldierCamera.MainInstance())
        {
            BaseSoldierCamera.MainInstance().RegsiterCameraModeChangeEvent(OnCameraModeChange);
        }
    }

    public void UpdateCollider()
    {
        m_HostColliderList.Clear();
        m_HostColliderList.AddRange(GetComponentsInChildren<Collider>());
    }

    void ClearRendering()
    {
        if (BaseSoldierCamera.MainInstance())
        {
            BaseSoldierCamera.MainInstance().UnRegsiterCameraModeChangeEvent(OnCameraModeChange);
        }
        isOcclusFadingEnabled_Camera = false;
        isOcclusFadingEnabled_GodEye = false;
        isOcclusFadingEnabled = false;
        isOcclusValiding = false;

        HitColliderBuffer = null;
        ClearAlphaMaterial();
        OrginalMaterList = null;
        Orginal_AlphaInstanceTable.Clear();
        Batch_instanceMaterials = null;
        BatchesIDs.Clear();
       
        m_HostColliderList.Clear();
        supportOptimitzedMatList.Clear();
        if(null != ShareResList)
        {
            ShareResList.Clear();
        }


    }

    void OnCameraModeChange(CameraMode mode)
    {
        if (BaseStageManager.Instance.CurSubState == EStage_SubState.ESSS_WAREND)
            return;

        isOcclusFadingEnabled_GodEye = false;
        if(!hostResManager.IsActive)
        {
            UpdateOcclusFading();
            return;
        }
        if (GameLogicAPI.isInWarScene() > 0 && AlwaysVisible && !isTerrain)
        {
            if (ObjectHeight > HightObj)
            {
                if (mode == CameraMode.TopControl || mode == CameraMode.MapGodControl || mode == CameraMode.DeadControl)
                {
                    isOcclusFadingEnabled_GodEye = true;
                }
            }
        }

        UpdateOcclusFading();
    }

    /// <summary>
    /// 过度完成
    /// </summary>
    public void OnFadeFinish(Effect.SceneFadeInOutMaterialFloat FadeInOutMaterialFloat)
    {
        if(!CheckSupportFading())
        {
            return;
        }

        hostRender.materials = OrginalMaterList;

        if(bOptimitzedRendering)
        {
            QuadTreeNodeAcitiveVisible = false;
            UpdateVisible();
            InvokeFadeOutFinishEvent();
        }
        else
        {
            QuadTreeNodeAcitiveVisible = true;
            UpdateVisible();
            //激活一下，因为打包之后有的情况下，不激活物体有时候会看不见,哪怕原本就是激活的
            hostRender.gameObject.SetActive(false);
            hostRender.gameObject.SetActive(true);

            InvokeFadeInFinishEvent();
        }
        


    }

    public static void SetInstanceMaterialParams_Alpha(Material material)
    {

        string shaderName = material.shader.name;
        if (!GameUtil.isStandardShader(shaderName))
        {
            return;
        }

        //3是Transparent
        material.SetFloat("_Mode", 3.0f);

        material.SetOverrideTag("RenderType", "Transparent");
        material.SetInt("_SrcBlend", (int)UnityEngine.Rendering.BlendMode.One);
        material.SetInt("_DstBlend", (int)UnityEngine.Rendering.BlendMode.OneMinusSrcAlpha);
        material.SetInt("_ZWrite", 0);
        material.DisableKeyword("_ALPHATEST_ON");
        material.DisableKeyword("_ALPHABLEND_ON");
        material.EnableKeyword("_ALPHAPREMULTIPLY_ON");
        material.renderQueue = 3000;


        //if (shaderName.Contains("Standard (Specular setup)"))
        //{
        //    Texture t = material.GetTexture("_SpecGlossMap");
        //    if (t)
        //    {
        //        material.EnableKeyword("_SPECGLOSSMAP");
        //    }
        //    else
        //    {
        //        material.DisableKeyword("_SPECGLOSSMAP");
        //    }
        //}
        //else
        //{
        //    Texture t = material.GetTexture("_MetallicGlossMap");
        //    if (t)
        //    {
        //        material.EnableKeyword("_METALLICGLOSSMAP");
        //    }
        //    else
        //    {
        //        material.DisableKeyword("_METALLICGLOSSMAP");
        //    }
        //}
        //Color emissioncolor = material.GetColor("_EmissionColor");
        //bool shouldEmissionBeEnabled = emissioncolor.grayscale > (0.1f / 255.0f);

        //MaterialGlobalIlluminationFlags flags = material.globalIlluminationFlags;
        //if ((flags & (MaterialGlobalIlluminationFlags.BakedEmissive | MaterialGlobalIlluminationFlags.RealtimeEmissive)) != 0)
        //{
        //    flags &= ~MaterialGlobalIlluminationFlags.EmissiveIsBlack;
        //    if (!shouldEmissionBeEnabled)
        //        flags |= MaterialGlobalIlluminationFlags.EmissiveIsBlack;

        //    material.globalIlluminationFlags = flags;
        //}
    }

    public static Material CreateFadeingMaterial(Material source)
    {
        Material mat2 = new Material(source);
        mat2.SetTexture("_DissloveMap", AssetBundleManager.CommonDissloveTexture);
        SetInstanceMaterialParams_Disslove(mat2);
        return mat2;
    }

    public static void SetInstanceMaterialParams_Disslove(Material material)
    {
        // public enum BlendMode
        //{
        //    Opaque,
        //    Cutout,
        //    Fade,		// Old school alpha-blending mode, fresnel does not affect amount of transparency
        //    Transparent // Physically plausible transparency mode, implemented as alpha pre-multiply
        //}
        string shaderName = material.shader.name;
        if (GameUtil.isLightMappingShader(shaderName))
        {
            return;
        }
        if (!GameUtil.isStandardShader(shaderName))
        {
            return;
        }

        //对于原来不是cutout的，把cutoff设置为0
        if (!GameUtil.FloatEquls(material.GetFloat("_Mode"),1))
        {
            material.SetFloat("_Cutoff", 0.0f);
        }


        //1是cutout
        material.SetFloat("_Mode", 1.0f);
        
        material.SetOverrideTag("RenderType", "TransparentCutout");
        material.SetInt("_SrcBlend", (int)UnityEngine.Rendering.BlendMode.One);
        material.SetInt("_DstBlend", (int)UnityEngine.Rendering.BlendMode.Zero);
        material.SetInt("_ZWrite", 1);
        material.EnableKeyword("_ALPHATEST_ON");
        material.DisableKeyword("_ALPHABLEND_ON");
        material.DisableKeyword("_ALPHAPREMULTIPLY_ON");


        //if (shaderName.Contains("Standard (Specular setup)"))
        //{
        //    Texture t = material.GetTexture("_SpecGlossMap");
        //    if (t)
        //    {
        //        material.EnableKeyword("_SPECGLOSSMAP");
        //    }
        //    else
        //    {
        //        material.DisableKeyword("_SPECGLOSSMAP");
        //    }
        //}
        //else
        //{
        //    Texture t = material.GetTexture("_MetallicGlossMap");
        //    if (t)
        //    {
        //        material.EnableKeyword("_METALLICGLOSSMAP");
        //    }
        //    else
        //    {
        //        material.DisableKeyword("_METALLICGLOSSMAP");
        //    }
        //}
        //Color emissioncolor = material.GetColor("_EmissionColor");
        //bool shouldEmissionBeEnabled = emissioncolor.grayscale > (0.1f / 255.0f);

        //MaterialGlobalIlluminationFlags flags = material.globalIlluminationFlags;
        //if ((flags & (MaterialGlobalIlluminationFlags.BakedEmissive | MaterialGlobalIlluminationFlags.RealtimeEmissive)) != 0)
        //{
        //    flags &= ~MaterialGlobalIlluminationFlags.EmissiveIsBlack;
        //    if (!shouldEmissionBeEnabled)
        //        flags |= MaterialGlobalIlluminationFlags.EmissiveIsBlack;

        //    material.globalIlluminationFlags = flags;
        //}

        material.renderQueue = 2450;
    }

    public static void SetMaterialParams(Material material)
    {
        if(!material)
        {
            return;
        }

        material.DisableKeyword("_EMISSION");
        material.DisableKeyword("_SPECGLOSSMAP");
        material.DisableKeyword("_METALLICGLOSSMAP");
        material.DisableKeyword("_NORMALMAP");
        material.DisableKeyword("_PARALLAXMAP");
        material.DisableKeyword("_DETAIL_MULX2");
        ResetEmissonTexture(material);
        ResetSurfaceDefineTexture(material);

        //string shaderName = material.shader.name;
        //if (!GameUtil.isStandardShader(shaderName))
        //{
        //    return;
        //}

        ////0是Opaque
        //material.SetFloat("_Mode", 0.0f);

        //material.SetInt("_SrcBlend", (int)UnityEngine.Rendering.BlendMode.One);
        //material.SetInt("_DstBlend", (int)UnityEngine.Rendering.BlendMode.Zero);
        //material.SetInt("_ZWrite", 1);
        //material.DisableKeyword("_ALPHATEST_ON");
        //material.DisableKeyword("_ALPHABLEND_ON");
        //material.DisableKeyword("_ALPHAPREMULTIPLY_ON");
       

        //if (shaderName.Contains("Standard (Specular setup)"))
        //{
        //    Texture t = material.GetTexture("_SpecGlossMap");
        //    if (t)
        //    {
        //        material.EnableKeyword("_SPECGLOSSMAP");
        //    }
        //    else
        //    {
        //        material.DisableKeyword("_SPECGLOSSMAP");
        //    }
        //}
        //else
        //{
        //    Texture t = material.GetTexture("_MetallicGlossMap");
        //    if (t)
        //    {
        //        material.EnableKeyword("_METALLICGLOSSMAP");
        //    }
        //    else
        //    {
        //        material.DisableKeyword("_METALLICGLOSSMAP");
        //    }
        //}
        //Color emissioncolor = material.GetColor("_EmissionColor");
        //bool shouldEmissionBeEnabled = emissioncolor.grayscale > (0.1f / 255.0f);

        //MaterialGlobalIlluminationFlags flags = material.globalIlluminationFlags;
        //if ((flags & (MaterialGlobalIlluminationFlags.BakedEmissive | MaterialGlobalIlluminationFlags.RealtimeEmissive)) != 0)
        //{
        //    flags &= ~MaterialGlobalIlluminationFlags.EmissiveIsBlack;
        //    if (!shouldEmissionBeEnabled)
        //        flags |= MaterialGlobalIlluminationFlags.EmissiveIsBlack;

        //    material.globalIlluminationFlags = flags;
        //}
    }


}
