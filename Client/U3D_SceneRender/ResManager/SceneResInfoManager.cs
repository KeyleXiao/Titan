/*******************************************************************
** 文件名:	SceneResInfoManager.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/7/20
** 版  本:	1.0
** 描  述:	场景资源信息 管理器
** 应  用:   场景资源信息，用于做场景资源管理

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public partial class SceneResInfoManager : MonoBehaviour
{
    public const int SceneSize = 6000;

    public GameObject SceneRoot = null;

    public GameObject SceneLightRoot = null;

    public Camera SceneCamera = null;

    public string sceneName = string.Empty;


    /// <summary>
    /// 更新间隔
    /// </summary>
    private const float UpdateIntervel = 0.05f;
    /// <summary>
    /// 更新间隔的一半
    /// </summary>
    private const float halfUpdateIntervel = UpdateIntervel * 0.5f;
    private WaitForSeconds halfupdateTimer = new WaitForSeconds(halfUpdateIntervel);

    /// <summary>
    /// 延迟多少秒执行，避免一开始的时候摄像机离物体很近，就开始加载东西，导致场景非常卡
    /// </summary>
    private const float DelayTime = 0.3f;

    /// <summary>
    /// 所有的Instance，因为场景不止一个
    /// </summary>
    public static HashSet<SceneResInfoManager> AllInstance = new HashSet<SceneResInfoManager>();


    /// <summary>
    /// 上一个加载的Instance
    /// </summary>
    public static SceneResInfoManager LastActiveInstance = null;

    /// <summary>
    /// 上一个加载的Instance
    /// </summary>
    public static SceneResInfoManager CurrentActiveInstance = null;

    public bool IsActive
    {
        get
        {
            return m_IsActive;
        }
    }

    public const string ResManagerTag = "SceneResManager";
    public const string ResManagerNamePre = "SceneResManager_";

    private bool isCanUpdate = false;
    private bool m_IsActive = true;

    private bool isNeedToReBakeScene = false;

    private Dictionary<Material, Material> RenderMaterialTable = new Dictionary<Material,Material>();

    public void PushRenderMaterial(Renderer r)
    {
        if (AssetBundleManager.isOnRunTimeMode)
        {
            return;
        }
        
        Material[] mats = r.sharedMaterials;
        Material[] tempMats = new Material[mats.Length];
        for (int i = 0; i < mats.Length; i++)
        {
            Material mat = mats[i];
            Material temp = null;
            if (!RenderMaterialTable.TryGetValue(mat, out temp))
            {
                temp = new Material(mat);
                temp.name = mat.name + "_EditorTemp";
                RenderMaterialTable.Add(mat, temp);
            }
            tempMats[i] = temp;
        }
        r.sharedMaterials = tempMats;
    }

    public Material GetRenderMaterial(Material mat)
    {
        if (AssetBundleManager.isOnRunTimeMode)
        {
            return mat;
        }

        Material Reslut = null;
        if(!mat)
        {
            return Reslut;
        }
        if(!RenderMaterialTable.TryGetValue(mat, out Reslut))
        {
            Debug.LogError("找不到材质:" + mat.name);
        }
        return Reslut;
    }

    public void ClearRenderMaterial()
    {
        RenderMaterialTable.Clear();
        RenderMaterialTable = null;
    }

    void OnResolutionChanged()
    {

        InitScreenInfo();
    }


    public static void ClearOnAppQuit()
    {
        ClearColliderDebugData();
        LastActiveInstance = null;
        CurrentActiveInstance = null;
    }

    public void DeActive()
    {
        if(SceneCamera)
        {
            SceneCamera.gameObject.SetActive(false);
        }

        if(SceneLightRoot)
        {
            SceneLightRoot.SetActive(false);
        }
        
        m_IsActive = false;
        
    }

    public void Active()
    {
        LastActiveInstance = CurrentActiveInstance;
        CurrentActiveInstance = this;
        //ImageEffects.ImageEffectManager.API_SetDirty(SceneCamera);
        if (SceneCamera)
        {
            SceneCamera.gameObject.SetActive(true);
            m_currentSceneCamera = SceneCamera;
            InvokeActiveSceneChangedEvent(this);
        }
        if (SceneLightRoot)
        {
            SceneLightRoot.SetActive(true);
        }

        ApplyColorCompensationFactor();
        ApplyEnvironmentLighting();
        m_IsActive = true;

    }

    void ApplyImageEffectSettings()
    {
        ImageEffects.ImageEffectManager.API_InitSceneCamera(SceneCamera);

        //默认关闭
        ImageEffects.ImageEffectManager.API_DisableImageEffect<ImageEffects.BloomAndFlares>(SceneCamera);
        ImageEffects.ImageEffectManager.API_DisableImageEffect<ImageEffects.SSAOPro>(SceneCamera);
        ImageEffects.ImageEffectManager.API_DisableImageEffect<ImageEffects.FXAA>(SceneCamera);

        ImageSettingData data = API_GetImageSettingData();
        if(null != data)
        {
            if (data.SSAOEnabled > 0)
            {
                ImageEffects.ImageEffectManager.API_EnableImageEffect<ImageEffects.SSAOPro>(SceneCamera);
            }
            if (data.FXAAEnabled > 0)
            {
                ImageEffects.ImageEffectManager.API_EnableImageEffect<ImageEffects.FXAA>(SceneCamera);
            }
            if (data.BloomAndFlaresEnabled > 0)
            {
                ImageEffects.ImageEffectManager.API_EnableImageEffect<ImageEffects.BloomAndFlares>(SceneCamera);
            }

            ImageEffects.ImageEffectManager.API_SetImagEffectParam<ImageEffects.ModifyGameBrightness>(ImageEffects.ModifyGameBrightness.ModifyGameBrightnessKey, data.currGameBrightness);
        }

        
    }

    public void Awake()
    {
        LoadTerrain();
        API_ConfirmLoadedSceneRes(this);
        LastActiveInstance = CurrentActiveInstance;
        CurrentActiveInstance = this;
        AllInstance.Add(this);

        InvokeActiveSceneChangedEvent(this);
        ApplyColorCompensationFactor();
        SceneCamera.nearClipPlane = 0.1f;
        SceneCamera.farClipPlane = Mathf.Min(SceneCamera.farClipPlane, 6000);
      
        if(SecondSunLight)
        {
            OrginalSecondSunLightEnabled = SecondSunLight.enabled;
            SecondSunLight.enabled = false;
        }
        
        if(ThirdSunLight)
        {
            OrginalThirdSunLightEnabled = ThirdSunLight.enabled;
            ThirdSunLight.enabled = false;
        }
       

        if (ScenereOcclusColliderRoot)
        {
            ScenereOcclusColliderRoot.SetActive(GoablEnabledOcclusCulling);
        }

        SceneResInfo.StartPreLoad();
        foreach (SceneResInfo res in ResInfoList)
        {
            if (!isVaildRes(res))
            {
                continue;
            }
           
            res.hostResManager = this;
            PushRenderMaterial(res.hostRender);
            res.OnAwake(SceneCamera);
            if(res.OcclusCullingCollider)
            {
                transform_ResTable.Add(res.OcclusCullingCollider.transform, res);
            }
            
        }

        foreach (SceneResInfo res in GrassResInfoList)
        {
            if (!isVaildGrassRes(res))
            {
                continue;
            }
           
            res.isGrass = true;
            res.hostResManager = this;
            PushRenderMaterial(res.hostRender);
            res.OnAwake(SceneCamera);
        }

        foreach (SceneLightInfo res in LightInfoList)
        {
            if (!isVaildLight(res))
            {
                continue;
            }

            res.OnAwake(SceneCamera);
        }

        foreach (SceneColliderInfo res in ColliderInfoList)
        {
            if (!isVaildCollider(res))
            {
                continue;
            }
            res.OnAwake(SceneCamera);
        }

        foreach (SceneEffectInfo res in EffectInfoList)
        {
            if (!isVaildEffect(res))
            {
                continue;
            }
            res.OnAwake(SceneCamera);
        }
    }

    public void Start()
    {
        ApplyImageEffectSettings();
        foreach (SceneResInfo res in ResInfoList)
        {
            if (!isVaildRes(res))
            {
                continue;
            }
            res.OnStart(SceneCamera);
        }

        foreach (SceneResInfo res in GrassResInfoList)
        {
            if (!isVaildGrassRes(res))
            {
                continue;
            }
            res.OnStart(SceneCamera);
        }

        foreach (SceneLightInfo res in LightInfoList)
        {
            if (!isVaildLight(res))
            {
                continue;
            }
            res.OnStart(SceneCamera);
        }

        foreach (SceneColliderInfo res in ColliderInfoList)
        {
            if (!isVaildCollider(res))
            {
                continue;
            }
            res.OnStart(SceneCamera);
        }

        foreach (SceneEffectInfo res in EffectInfoList)
        {
            if (!isVaildEffect(res))
            {
                continue;
            }
            res.OnStart(SceneCamera);
        }
        LoadSkyBoxTexture();
        

      
        BuildSceneQuadTree();
        
        StartCoroutine(InfoCheck());
        
        if(isNeedToReBakeScene)
        {
            Debug.LogWarning("场景:" + sceneName + "有部分资源损坏，会造成渲染不正确,请美术重新bake!!!!");
        }
        
    }

    public void BuildSceneQuadTree()
    {
        BuildResQuadTree();
        BuildGrassResQuadTree();
        BuildLightQuadTree();
        BuildColliderQuadTree();
        BuildEffectQuadTree();
        isCanUpdate = true;
    }

    public void ClearSceneQuadTree()
    {
        isCanUpdate = false;
        ClearBatchesRes();

        ClearQuadTree(ResRootParent);
        ClearQuadTree(GrassResRootParent);
        ClearQuadTree(LightRootParent);
        ClearQuadTree(ColliderRootParent);
        ClearQuadTree(EffectRootParent);
        ClearQuadTree(AlwaysVisibleResRootParent);

        unUsedEffectParent.Clear();
        unUsedColliderParent.Clear();
        unUsedResParent.Clear();
        unUsedGrassResParent.Clear();
        unUsedLightParent.Clear();
        unUsedAlwaysVisibleResParent.Clear();

        unUsedEffectLeafList.Clear();
        unUsedColliderLeafList.Clear();
        unUsedResLeafList.Clear();
        unUsedGrassResLeafList.Clear();
        unUsedLightLeafList.Clear();
        unUsedAlwaysVisibleResLeafList.Clear();

        ActiveEffectParent.Clear();
        ActiveColliderParent.Clear();
        ActiveResParent.Clear();
        ActiveGrassResParent.Clear();
        ActiveLightParent.Clear();
        ActiveAlwaysVisibleResParent.Clear();

        ActiveColliderLeafList.Clear();
        ActiveResLeafList.Clear();
        ActiveGrassResLeafList.Clear();
        ActiveLightLeafList.Clear();
        ActiveEffectLeafList.Clear();
        ActiveAlwaysVisibleResLeafList.Clear();

        //重置说有的状态
        foreach(SceneResInfo res in ResInfoList)
        {
            if (!isVaildRes(res))
            {
                continue;
            }
            res.ResetRendering();
            res.ResetMemory();
        }

        foreach (SceneResInfo res in GrassResInfoList)
        {
            if (!isVaildGrassRes(res))
            {
                continue;
            }
            res.ResetRendering();
            res.ResetMemory();
        }

        foreach (SceneLightInfo res in LightInfoList)
        {
            if (!isVaildLight(res))
            {
                continue;
            }
            res.ResetLight();
        }

        foreach (SceneColliderInfo res in ColliderInfoList)
        {
            if (!isVaildCollider(res))
            {
                continue;
            }
            res.ResetCollider();
        }

        foreach (SceneEffectInfo res in EffectInfoList)
        {
            if (!isVaildEffect(res))
            {
                continue;
            }
            res.ResetSceneEffect();
        }
    }

    public void OnDestroy()
    {
        UnLoadTerrain();
        ImageEffects.ImageEffectManager.API_ClearSceneCamera(SceneCamera);
        ClearSceneQuadTree();

        foreach (SceneResInfo res in ResInfoList)
        {
            if (!res)
            {
                continue;
            }
            res.OnClear(SceneCamera);
        }

        foreach (SceneResInfo res in GrassResInfoList)
        {
            if (!res)
            {
                continue;
            }
            res.OnClear(SceneCamera);
        }

        foreach (SceneLightInfo res in LightInfoList)
        {
            if (!res)
            {
                continue;
            }
            res.OnClear(SceneCamera);
        }

        foreach (SceneColliderInfo res in ColliderInfoList)
        {
            if (!res)
            {
                continue;
            }
            res.OnClear(SceneCamera);
        }

        foreach (SceneEffectInfo res in EffectInfoList)
        {
            if (!res)
            {
                continue;
            }
            res.OnClear(SceneCamera);
        }

        //如果当前场景被删除，当前场景的实例则变为上一个场景
        if(CurrentActiveInstance == this)
        {
            CurrentActiveInstance = LastActiveInstance;
        }
        AllInstance.Remove(this);
        UnLoadSkyBoxTexture();

        ResInfoList.Clear();
        LightInfoList.Clear();
        ColliderInfoList.Clear();
        EffectInfoList.Clear();
        StopAllCoroutines();
        OcclusCullingClear();
        ClearRenderMaterial();
    }

    private bool WaitngUpdate = false;

    IEnumerator InfoCheck()
    {
        if (DelayTime > 0.03f)
        {
            yield return new WaitForSeconds(DelayTime);
        }
        OcclusCullingInit();
        while (true)
        {
            //if (Application.isEditor)
            //{
            //    Shader.SetGlobalFloat("_SceneBrightScale", SceneBrightScale);
            //    Shader.SetGlobalFloat("_CharacterBrightScale", CharacterBrightScale);
            //}

            if(!WaitngUpdate)
            {
                if (BaseSoldierCamera.MainInstance().GetSoliderGo())
                {
                    yield return halfupdateTimer;
                    WaitngUpdate = true;
                }
            }

            if (isCanUpdate && m_IsActive && WaitngUpdate)
            {
                Vector3 pos = SceneRoot.transform.worldToLocalMatrix.MultiplyPoint(SceneCamera.transform.position);
                Vector2 pos2D = Vector2.zero;
                Vector2 Dir2D = Vector2.zero;

                Dir2D.x = SceneCamera.transform.forward.x;
                Dir2D.y = SceneCamera.transform.forward.z;

                pos2D.x = pos.x;
                pos2D.y = pos.z;

                GenerateActiveQuadParent(ResRootParent, SceneCamera.fieldOfView, pos2D, Dir2D, currResInfoCellSize, ref ActiveResLeafList, ref ActiveResParent, ref unUsedResLeafList, ref unUsedResParent);
                GenerateActiveQuadParent(GrassResRootParent, SceneCamera.fieldOfView, pos2D, Dir2D, currGrassResInfoCellSize, ref ActiveGrassResLeafList, ref ActiveGrassResParent, ref unUsedGrassResLeafList, ref unUsedGrassResParent);
                GenerateActiveQuadParent(LightRootParent, SceneCamera.fieldOfView, pos2D, Dir2D, currResLightCellSize, ref ActiveLightLeafList, ref ActiveLightParent, ref unUsedLightLeafList, ref unUsedLightParent);
                GenerateActiveQuadParent(ColliderRootParent, SceneCamera.fieldOfView, pos2D, Dir2D, currResColliderCellSize, ref ActiveColliderLeafList, ref ActiveColliderParent, ref unUsedColliderLeafList, ref unUsedColliderParent);
                GenerateActiveQuadParent(EffectRootParent, SceneCamera.fieldOfView, pos2D, Dir2D, currEffectCellSize, ref ActiveEffectLeafList, ref ActiveEffectParent, ref unUsedEffectLeafList, ref unUsedEffectParent);
                GenerateActiveQuadParent(AlwaysVisibleResRootParent, SceneCamera.fieldOfView, pos2D, Dir2D, currAlwaysVisibleResInfoCellSize, ref ActiveAlwaysVisibleResLeafList, ref ActiveAlwaysVisibleResParent, ref unUsedAlwaysVisibleResLeafList, ref unUsedAlwaysVisibleResParent);

                int ResallCount = ActiveResParent.Count;
                int ReshalfCount = ResallCount / 2;
                ReshalfCount = Mathf.Min(ReshalfCount, ResallCount);

                int GrassResallCount = ActiveGrassResParent.Count;
                int GrassReshalfCount = GrassResallCount / 2;
                GrassReshalfCount = Mathf.Min(GrassReshalfCount, GrassResallCount);

                int LightallCount = ActiveLightLeafList.Count;
                int LighthalfCount = LightallCount / 2;
                LighthalfCount = Mathf.Min(LighthalfCount, LightallCount);

                int ColliderallCount = ActiveColliderLeafList.Count;
                int ColliderhalfCount = ColliderallCount / 2;
                ColliderhalfCount = Mathf.Min(ColliderhalfCount, ColliderallCount);

                int EffectallCount = ActiveEffectLeafList.Count;
                int EffecthalfCount = EffectallCount / 2;
                EffecthalfCount = Mathf.Min(EffecthalfCount, EffectallCount);

                int AlwaysVisibleResallCount = ActiveAlwaysVisibleResLeafList.Count;
                int AlwaysVisibleReshalfCount = AlwaysVisibleResallCount / 2;
                AlwaysVisibleReshalfCount = Mathf.Min(AlwaysVisibleReshalfCount, AlwaysVisibleResallCount);





                int unUsedResallCount = unUsedResParent.Count;
                int unUsedReshalfCount = unUsedResallCount / 2;
                unUsedReshalfCount = Mathf.Min(unUsedReshalfCount, unUsedResallCount);

                int unUsedGrassResallCount = unUsedGrassResParent.Count;
                int unUsedGrassReshalfCount = unUsedGrassResallCount / 2;
                unUsedGrassReshalfCount = Mathf.Min(unUsedGrassReshalfCount, unUsedGrassResallCount);

                int unUsedLightallCount = unUsedLightLeafList.Count;
                int unUsedLighthalfCount = unUsedLightallCount / 2;
                unUsedLighthalfCount = Mathf.Min(unUsedLighthalfCount, unUsedLightallCount);

                int unUsedColliderallCount = unUsedColliderLeafList.Count;
                int unUsedColliderhalfCount = unUsedColliderallCount / 2;
                unUsedColliderhalfCount = Mathf.Min(unUsedColliderhalfCount, unUsedColliderallCount);

                int unUsedEffectallCount = unUsedEffectLeafList.Count;
                int unUsedEffecthalfCount = unUsedEffectallCount / 2;
                unUsedEffecthalfCount = Mathf.Min(unUsedEffecthalfCount, unUsedEffectallCount);

                int unUsedAlwaysVisibleResallCount = unUsedAlwaysVisibleResLeafList.Count;
                int unUsedAlwaysVisibleReshalfCount = unUsedAlwaysVisibleResallCount / 2;
                unUsedAlwaysVisibleReshalfCount = Mathf.Min(unUsedAlwaysVisibleReshalfCount, unUsedAlwaysVisibleResallCount);

                //先处理一半

                UpdateResInfo(0, ReshalfCount, ActiveResParent);
                UpdateGrassResInfo(0, GrassReshalfCount, ActiveGrassResParent);
                UpdateLightInfo(0, LighthalfCount, ActiveLightLeafList);
                UpdateColliderInfo(0, ColliderhalfCount, ActiveColliderLeafList);
                UpdateEffectInfo(0, EffecthalfCount, ActiveEffectLeafList);
                UpdateAlwaysVisibleResInfo(0, AlwaysVisibleReshalfCount, ActiveAlwaysVisibleResLeafList);

                ProcessUnUsedResInfo(0, unUsedReshalfCount, unUsedResParent);
                ProcessUnUsedGrassResInfo(0, unUsedGrassReshalfCount, unUsedGrassResParent);
                ProcessUnUsedLightInfo(0, unUsedLighthalfCount, unUsedLightLeafList);
                ProcessUnUsedColliderInfo(0, unUsedColliderhalfCount, unUsedColliderLeafList);
                ProcessUnUsedEffectInfo(0, unUsedEffecthalfCount, unUsedEffectLeafList);
                ProcessUnUsedAlwaysVisibleResInfo(0, unUsedAlwaysVisibleReshalfCount, unUsedAlwaysVisibleResLeafList);

                yield return halfupdateTimer;

                UpdateResInfo(ReshalfCount, ResallCount, ActiveResParent);
                UpdateGrassResInfo(GrassReshalfCount, GrassResallCount, ActiveGrassResParent);
                UpdateLightInfo(LighthalfCount, LightallCount, ActiveLightLeafList);
                UpdateColliderInfo(ColliderhalfCount, ColliderallCount, ActiveColliderLeafList);
                UpdateEffectInfo(EffecthalfCount, EffectallCount, ActiveEffectLeafList);
                UpdateAlwaysVisibleResInfo(AlwaysVisibleReshalfCount, AlwaysVisibleResallCount, ActiveAlwaysVisibleResLeafList);
                
                ProcessUnUsedResInfo(unUsedReshalfCount, unUsedResallCount, unUsedResParent);
                ProcessUnUsedGrassResInfo(unUsedGrassReshalfCount, unUsedGrassResallCount, unUsedGrassResParent);
                ProcessUnUsedLightInfo(unUsedLighthalfCount, unUsedLightallCount, unUsedLightLeafList);
                ProcessUnUsedColliderInfo(unUsedColliderhalfCount, unUsedColliderallCount, unUsedColliderLeafList);
                ProcessUnUsedEffectInfo(unUsedEffecthalfCount, unUsedEffectallCount, unUsedEffectLeafList);
                ProcessUnUsedAlwaysVisibleResInfo(unUsedAlwaysVisibleReshalfCount, unUsedAlwaysVisibleResallCount, unUsedAlwaysVisibleResLeafList);

            }

            yield return halfupdateTimer;
        }
    }

     public void LateUpdate()
    {
        if (!isCanUpdate || !m_IsActive || !WaitngUpdate)
        {
            return;
        }

        UpdateOcclusingCulling();
    }

}