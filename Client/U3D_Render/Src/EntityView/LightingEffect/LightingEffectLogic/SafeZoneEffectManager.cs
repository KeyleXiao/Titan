using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using SPELL;

public class SafeZoneEffectManager
{
    struct MaterialThickNess
    {
        /// <summary>
        /// 贴图1
        /// </summary>
        public Vector2 minThickNess_1;
        public Vector2 maxThickNess_1;

        /// <summary>
        /// 贴图2
        /// </summary>
        public Vector2 minThickNess_2;
        public Vector2 maxThickNess_2;
    }
    private const string SafeZoneEffectGUID = "8c65805e9b59093449a7f0746854a0ed";
    private static ResNode SafeZoneEffectRes = null;
    private static Effect.EffectNode SafeZoneEffectInstance = null;
    private static Transform SafeZoneTransform = null;

    private static Material[] m_ZoneEffectMaterial = null;
    private static MaterialThickNess[] m_ZoneEffectThickNess = null;
    private static int mainZoneMaterialIndex = 0;
    private static bool isCanModifyMaterialThickNess = false;
    private static float maxRadiusZoneMaterialThickNess = 700;
    private static float minRadiusZoneMaterialThickNess = 10;

    private static bool isStarting = false;
    private static float showZoneDis = 350;
    private static bool isHaveData = false;
    private static float RadiusCorrectionFactor = 2.0f;


    public static void Init()
    {
        SceneManager.RegisterSceneLoadFinishEvent(OnSceneLoadFinish);
    }

    public static void Clear()
    {
        AssetBundleManager.DeleteAssets(ref SafeZoneEffectRes, true);
        SafeZoneEffectInstance = null;
        SceneManager.UnRegisterSceneLoadFinishEvent(OnSceneLoadFinish);
    }

    private static bool GenerateZoneMaterial(Effect.EffectNode end)
    {
        if (SafeZoneEffectInstance.FastestModlues.m_effect_Play.Length != 1)
        {
            Debug.LogError("毒圈光效m_effect_Play.Length != 1");
            return false;
        }

        if (SafeZoneEffectInstance.FastestModlues.m_effect_Play[0].m_MeshRender.Length != 2)
        {
            Debug.LogError("m_effect_Play[0].m_MeshRender.Length != 2");
            return false;
        }
        if (!SafeZoneEffectInstance.FastestModlues.m_effect_Play[0].m_MeshRender[0].sharedMaterial)
        {
            Debug.LogError("m_effect_Play[0].m_MeshRender[0].sharedMaterial is null");
            return false;
        }

        if (!SafeZoneEffectInstance.FastestModlues.m_effect_Play[0].m_MeshRender[1].sharedMaterial)
        {
            Debug.LogError("m_effect_Play[0].m_MeshRender[1].sharedMaterial is null");
            return false;
        }

        m_ZoneEffectMaterial = new Material[2];
        Material matTemp = SafeZoneEffectInstance.FastestModlues.m_effect_Play[0].m_MeshRender[0].sharedMaterial;
        if(matTemp.shader.name.Contains("VirusZone"))
        {
            mainZoneMaterialIndex = 0;
        }
        else
        {
            mainZoneMaterialIndex = 1;
        }
        m_ZoneEffectMaterial[0] = SafeZoneEffectInstance.FastestModlues.m_effect_Play[0].m_MeshRender[0].sharedMaterial;
        m_ZoneEffectMaterial[1] = SafeZoneEffectInstance.FastestModlues.m_effect_Play[0].m_MeshRender[1].sharedMaterial;

        m_ZoneEffectThickNess = new MaterialThickNess[2];

        m_ZoneEffectThickNess[mainZoneMaterialIndex].minThickNess_1 = new Vector2(10, 10);
        m_ZoneEffectThickNess[mainZoneMaterialIndex].maxThickNess_1 = new Vector2(100, 100);

        m_ZoneEffectThickNess[mainZoneMaterialIndex].minThickNess_2 = new Vector2(10, 10);
        m_ZoneEffectThickNess[mainZoneMaterialIndex].maxThickNess_2 = new Vector2(200, 200);

        isCanModifyMaterialThickNess = true;
        return true;
    }


    private static void ModifyMaterialThickNess()
    {
        if(!isCanModifyMaterialThickNess)
        {
            return;
        }


        float t = 1;
        float temp = maxRadiusZoneMaterialThickNess - minRadiusZoneMaterialThickNess;
        if(SafeZone.currRadius >= maxRadiusZoneMaterialThickNess)
        {
            t = 1;
        }
        else if(SafeZone.currRadius <= minRadiusZoneMaterialThickNess)
        {
            t = 0;
        }
        else
        {
            if (temp > 0)
            {
                t = SafeZone.currRadius / temp;
            }
        }

        Material mat = m_ZoneEffectMaterial[mainZoneMaterialIndex];
        Vector2 currThickNess_1 = Vector2.Lerp(m_ZoneEffectThickNess[mainZoneMaterialIndex].minThickNess_1, m_ZoneEffectThickNess[mainZoneMaterialIndex].maxThickNess_1, t);
        Vector2 currThickNess_2 = Vector2.Lerp(m_ZoneEffectThickNess[mainZoneMaterialIndex].minThickNess_2, m_ZoneEffectThickNess[mainZoneMaterialIndex].maxThickNess_2, t);
        mat.SetTextureScale("_MainTex", currThickNess_1);
        mat.SetTextureScale("_DistortTex", currThickNess_2);
    }

    private static void OnSceneLoadFinish(string sceneName)
    {
        isHaveData = false;
        //如果是非吃鸡场景，卸载.
        if (GameLogicAPI.getCurGameMode() != Game_ManagedDef.EGameMode.EGM_MCAMP)
        {
            AssetBundleManager.DeleteAssets(ref SafeZoneEffectRes, true);
            SafeZoneEffectInstance = null;
            return;
        }
        if (null == SafeZoneEffectRes)
        {
            SafeZoneEffectRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, SafeZoneEffectGUID, true);
            if (!ResNode.isNullOrEmpty(SafeZoneEffectRes))
            {
                SafeZoneEffectInstance = SafeZoneEffectRes.InstanceMainRes<Effect.EffectNode>();
                if(SafeZoneEffectInstance)
                {
                    SafeZoneEffectInstance.Init();
                    SafeZoneEffectInstance.StopEffect();
                    SafeZoneTransform = SafeZoneEffectInstance.transform;
                    SafeZoneTransform.SetParent(EntityFactory.EffectGroup.transform);

                    GenerateZoneMaterial(SafeZoneEffectInstance);
                }

            }
        }

        if(SafeZoneEffectInstance)
        {
            SafeZoneEffectInstance.StopEffect();
        }
    }

    public static void Update()
    {
        if (!SafeZoneEffectInstance || !isHaveData)
        {
            return;
        }
        if(!Initialize.mainCam)
        {
            return;
        }

        UpdateZoneVisible();
        if (!isStarting)
        {
            return;
        }
        
        
        SafeZoneEffectInstance.SetScale(SafeZone.currRadius * RadiusCorrectionFactor);
        SafeZoneTransform.SetPosition(SafeZone.currPos);
        ModifyMaterialThickNess();


        isStarting = SafeZone.isSpreadStarted;
    }

    private static void UpdateZoneVisible()
    {
        if (U3D_Render.EntityView.isNullOrEmpty(EntityFactory.MainHeroView) || !isHaveData)
        {
            return;
        }

        Vector3 CamerPos = EntityFactory.MainHeroView.transform.position;
        Vector3 dir = (CamerPos - SafeZone.currPos).normalized;
        Vector3 SphrePos = SafeZone.currPos + dir * SafeZone.currRadius;

        float dis = GameUtil.GetSqrDistanceWithOutY(CamerPos, SphrePos);
        if (dis  < showZoneDis * showZoneDis)
        {
            SafeZoneEffectInstance.PlayEffect();
        }
        else
        {
            SafeZoneEffectInstance.StopEffect();
        }
    }

    /// <summary>
    /// 开始扩散毒圈
    /// </summary>
    /// <param name="safeCenter"></param>
    /// <param name="safeRadius"></param>
    /// <param name="NoxSafeCenter"></param>
    /// <param name="NoxSafeRadius"></param>
    /// <param name="SpreadTimeInSecond"></param>
    public static void StartSpreadNoxSafeZone()
    {
        if(!SafeZoneEffectInstance)
        {
            return;
        }
        SafeZoneEffectInstance.PlayEffect();
        SafeZoneEffectInstance.SetScale(SafeZone.currRadius * RadiusCorrectionFactor);
        SafeZoneTransform.SetPosition(SafeZone.currPos);
        ModifyMaterialThickNess();   
        isStarting = true;
        isHaveData = true;
    }

    /// <summary>
    /// 设置毒圈范围
    /// </summary>
    /// <param name="Center"></param>
    /// <param name="Radius"></param>
    public static void SetNoxSafeZone()
    {
        if (!SafeZoneEffectInstance)
        {
            return;
        }
        SafeZoneEffectInstance.PlayEffect();

        SafeZoneEffectInstance.SetScale(SafeZone.currRadius * RadiusCorrectionFactor);
        SafeZoneTransform.SetPosition(SafeZone.currPos);
        ModifyMaterialThickNess();
        isStarting = false;
        isHaveData = true;
    }

}
