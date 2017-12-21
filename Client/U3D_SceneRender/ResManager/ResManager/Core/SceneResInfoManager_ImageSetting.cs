/*******************************************************************
** 文件名:	SceneResInfoManager_ImageSetting.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/7/20
** 版  本:	1.0
** 描  述:	场景资源信息 管理器
** 应  用:   场景资源信息，用于做场景的画面设置

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

    public static bool enabledGrassShadow = false;

    /// <summary>
    /// 用于场景低配的颜色补偿
    /// </summary>
    public float SceneColorCompensationFactor_Fastest = 1.0f;

    /// <summary>
    /// 用于场景中配颜色补偿
    /// </summary>
    public float SceneColorCompensationFactor_Good = 1.0f;

    private const string SceneColorCompensationFactorKey_Fastest = "_SceneColorCompensationFactor_Fastest";
    private const string SceneColorCompensationFactorKey_Good = "_SceneColorCompensationFactor_Good";


    public Light FirstSunLight = null;
    public Light SecondSunLight = null;
    public Light ThirdSunLight = null;

    public bool OrginalHDR = true;
    public RenderingPath OrginalRenderPath = RenderingPath.DeferredShading;

    public LightShadows OrginalFirstSunLightShadowType = LightShadows.None;
    public LightShadows OrginalSecondSunLightShadowType = LightShadows.None;
    public LightShadows OrginalThirdSunLightShadowType = LightShadows.None;


    [System.NonSerialized]
    public bool OrginalSecondSunLightEnabled = false;
    [System.NonSerialized]
    public bool OrginalThirdSunLightEnabled = false;

    //最高配情况下的参数
    private const int ResColliderCellSize = 20;
    private const int ResLightCellSize = 40;
    private const int ResInfoCellSize = 40;
    private const int ResGrassInfoCellSize = 40;
    private const int AlwaysVisibleResInfoCellSize = 50;
    private const int EffectCellSize = 30;


    public int currResColliderCellSize = 6;
    public int currResLightCellSize = 22;
    public int currResInfoCellSize = 16;
    public int currAlwaysVisibleResInfoCellSize = 20;
    public int currEffectCellSize = 12;
    public int currGrassResInfoCellSize = 40;


    public void ApplyColorCompensationFactor()
    {
        Shader.SetGlobalFloat(SceneColorCompensationFactorKey_Fastest, SceneColorCompensationFactor_Fastest);
        Shader.SetGlobalFloat(SceneColorCompensationFactorKey_Good, SceneColorCompensationFactor_Good);
    }

    public void ApplyRenderingPath(RenderingPath pat)
    {
        SceneCamera.renderingPath = pat;
    }

    public void RecaluCellSize(float scale)
    {
        currResColliderCellSize = (int)(ResColliderCellSize * scale);
        currResLightCellSize = (int)(ResLightCellSize * scale); ;
        currResInfoCellSize = (int)(ResInfoCellSize * scale); ;
        currAlwaysVisibleResInfoCellSize = (int)(AlwaysVisibleResInfoCellSize * scale); ;
        currEffectCellSize = (int)(EffectCellSize * scale);

        //草做特殊处理，不参与品级调整
        currGrassResInfoCellSize = ResGrassInfoCellSize;

    }

    public void OnSceneRenderingQuality_Fastest()
    {
        enabledOcclusCulling = false;
        ClearOcclusCacheData();

        ClearSceneQuadTree();
        RecaluCellSize(0.25f);
        BuildSceneQuadTree();



        

        ApplyRenderingPath(RenderingPath.Forward);
        OnSceneLight_Fastest();
        ApplyTerrainParams_Fastest();
        enabledGrassShadow = false;
    }

    public void OnSceneRenderingQuality_Good()
    {
        enabledOcclusCulling = false;
        ClearOcclusCacheData();

        ClearSceneQuadTree();
        RecaluCellSize(0.5f);
        BuildSceneQuadTree();



        ApplyRenderingPath(RenderingPath.Forward);

        OnSceneLight_Good();
        ApplyTerrainParams_Good();
        enabledGrassShadow = false;

    }

    public void OnSceneRenderingQuality_High()
    {
        enabledOcclusCulling = true;
        ClearOcclusCacheData();
        BuildOcclusCacheData();

        ClearSceneQuadTree();
        RecaluCellSize(0.8f);
        BuildSceneQuadTree();



        ApplyRenderingPath(OrginalRenderPath);
        OnSceneLight_High();
        ApplyTerrainParams_High();
        enabledGrassShadow = false;
    }

    public void OnSceneRenderingQuality_Best()
    {
        enabledOcclusCulling = true;
        ClearOcclusCacheData();
        BuildOcclusCacheData();

        ClearSceneQuadTree();
        RecaluCellSize(1.1f);
        BuildSceneQuadTree();


        ApplyRenderingPath(OrginalRenderPath);
        OnSceneLight_Best();
        ApplyTerrainParams_Best();
        enabledGrassShadow = true;
    }


    private void OnSceneLight_Fastest()
    {
        if (FirstSunLight)
        {
            FirstSunLight.shadows = LightShadows.None;
        }

        if (SecondSunLight)
        {
            SecondSunLight.shadows = LightShadows.None;
            SecondSunLight.enabled = false;
        }

        if(ThirdSunLight)
        {
            ThirdSunLight.shadows = LightShadows.None;
            ThirdSunLight.enabled = false;
        }

        SceneCamera.hdr = false;
        foreach (SceneLightInfo lig in LightInfoList)
        {
            if (isVaildLight(lig))
            {
                lig.lastOptimitzedLight = lig.OptimitzedLight;
                lig.AlwaysOptimitzed = true;
                lig.Optimitzed();
            }
        }

    }

    private void OnSceneLight_Good()
    {
        SceneCamera.hdr = false;
        foreach (SceneLightInfo lig in LightInfoList)
        {
            if (isVaildLight(lig))
            {
                lig.lastOptimitzedLight = lig.OptimitzedLight;
                lig.AlwaysOptimitzed = true;
                lig.Optimitzed();
            }
        }

        if (FirstSunLight)
        {
            FirstSunLight.shadows = LightShadows.None;
        }

        if (SecondSunLight)
        {
            SecondSunLight.shadows = LightShadows.None;
            SecondSunLight.enabled = true;
        }

        if (ThirdSunLight)
        {
            ThirdSunLight.shadows = LightShadows.None;
            ThirdSunLight.enabled = true;
        }
    }

    private void OnSceneLight_High()
    {
        SceneCamera.hdr = OrginalHDR;
        if (FirstSunLight)
        {
            FirstSunLight.shadows = OrginalFirstSunLightShadowType;
        }

        if (SecondSunLight)
        {
            SecondSunLight.shadows = LightShadows.None;
            SecondSunLight.enabled = OrginalSecondSunLightEnabled;
        }

        if (ThirdSunLight)
        {
            ThirdSunLight.shadows = LightShadows.None;
            ThirdSunLight.enabled = OrginalThirdSunLightEnabled;
        }

        foreach (SceneLightInfo lig in LightInfoList)
        {
            if (isVaildLight(lig))
            {
                lig.AlwaysOptimitzed = false;
                if (!lig.lastOptimitzedLight)
                {
                    lig.UnOptimitzed();
                }
            }
        }
    }

    private void OnSceneLight_Best()
    {
        SceneCamera.hdr = OrginalHDR;
        if (FirstSunLight)
        {
            FirstSunLight.shadows = OrginalFirstSunLightShadowType;
        }

        if (SecondSunLight)
        {
            if (OrginalSecondSunLightShadowType != LightShadows.None)
            {
                SecondSunLight.shadows = LightShadows.None;
            }
            SecondSunLight.enabled = OrginalSecondSunLightEnabled;
        }

        if (ThirdSunLight)
        {
            ThirdSunLight.shadows = LightShadows.None;
            ThirdSunLight.enabled = OrginalThirdSunLightEnabled;
        }

        foreach (SceneLightInfo lig in LightInfoList)
        {
            if (isVaildLight(lig))
            {
                lig.AlwaysOptimitzed = false;
                if (!lig.lastOptimitzedLight)
                {
                    lig.UnOptimitzed();
                }
            }
        }
    }
}
