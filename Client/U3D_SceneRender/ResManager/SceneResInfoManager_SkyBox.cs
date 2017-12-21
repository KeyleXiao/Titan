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
    public SceneSkyBoxMaterialConfig SkyBoxMaterialConfig = null;

    public SceneEnvironmentLightingInfo enviromentInfo = null;

    private List<ResNode> SkyBoxRes = new List<ResNode>();
    private Material SkyBoxMaterial = null;
    private Material SkyBoxBackUp = null;

    void LoadSkyBoxTexture()
    {
        if (enviromentInfo.skybox == null)
            return;

        SkyBoxBackUp = enviromentInfo.skybox;

        if (!SkyBoxMaterial)
        {
            SkyBoxMaterial = new Material(SkyBoxBackUp);
        }

        SkyBoxMaterial.CopyPropertiesFromMaterial(SkyBoxBackUp);
        RenderSettings.skybox = SkyBoxMaterial;

        if (SkyBoxMaterialConfig != null)
        {
            foreach (SceneTextureConfigData cof in SkyBoxMaterialConfig.TextureConfigList)
            {
                ResNode rs = AssetBundleManager.GetAssets(AssetType.Asset_Texture, cof);
                if (rs != null)
                {
                    SkyBoxMaterial.SetTexture(cof.ShaderKeyWordName, rs.ConvertRes<Texture>());
                }
                SkyBoxRes.Add(rs);
            }
        }
    }

    void UnLoadSkyBoxTexture()
    {
        RenderSettings.skybox = SkyBoxBackUp;
        if (SkyBoxMaterial)
        {
            ResNode.DestroyRes(ref SkyBoxMaterial);
            SkyBoxMaterial = null;
        }
        foreach (ResNode res in SkyBoxRes)
        {
            ResNode temp = res;
            AssetBundleManager.DeleteAssets(ref temp, true);
        }
        SkyBoxRes.Clear();
    }

    void ApplyEnvironmentLighting()
    {
        RenderSettings.ambientEquatorColor = enviromentInfo.ambientEquatorColor;
        RenderSettings.ambientGroundColor = enviromentInfo.ambientGroundColor;
        RenderSettings.ambientIntensity = enviromentInfo.ambientIntensity;
        RenderSettings.ambientLight = enviromentInfo.ambientLight;
        RenderSettings.ambientMode = enviromentInfo.ambientMode;
        RenderSettings.ambientProbe = enviromentInfo.ambientProbe;
        RenderSettings.ambientSkyColor = enviromentInfo.ambientSkyColor;
        RenderSettings.customReflection = enviromentInfo.customReflection;
        RenderSettings.defaultReflectionMode = enviromentInfo.defaultReflectionMode;
        RenderSettings.defaultReflectionResolution = enviromentInfo.defaultReflectionResolution;
        RenderSettings.flareFadeSpeed = enviromentInfo.flareFadeSpeed;
        RenderSettings.flareStrength = enviromentInfo.flareStrength;
        RenderSettings.fog = enviromentInfo.fog;
        RenderSettings.fogColor = enviromentInfo.fogColor;
        RenderSettings.fogDensity = enviromentInfo.fogDensity;
        RenderSettings.fogEndDistance = enviromentInfo.fogEndDistance;
        RenderSettings.fogMode = enviromentInfo.fogMode;
        RenderSettings.fogStartDistance = enviromentInfo.fogStartDistance;
        RenderSettings.haloStrength = enviromentInfo.haloStrength;
        RenderSettings.reflectionBounces = enviromentInfo.reflectionBounces;

        RenderSettings.skybox = SkyBoxMaterial;
    }
}