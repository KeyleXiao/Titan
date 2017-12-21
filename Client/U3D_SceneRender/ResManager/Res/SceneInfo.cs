/*******************************************************************
** 文件名:	SceneInfo.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/7/20
** 版  本:	1.0
** 描  述:	场景信息
** 应  用:   场景信息

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using Effect;
using UnityEngine.Rendering;

/// <summary>
/// 场景环境光信息
/// </summary>
[System.Serializable]
public class SceneEnvironmentLightingInfo
{
    public Color ambientEquatorColor = Color.black;
    public Color ambientGroundColor = Color.black;
    public float ambientIntensity = 0;
    public Color ambientLight = Color.black;
    public AmbientMode ambientMode = AmbientMode.Flat;
    public SphericalHarmonicsL2 ambientProbe;
    public Color ambientSkyColor = Color.black;
    public Cubemap customReflection = null;
    public DefaultReflectionMode defaultReflectionMode = DefaultReflectionMode.Skybox;
    public int defaultReflectionResolution = 0;
    public float flareFadeSpeed = 0;
    public float flareStrength = 0;
    public bool fog = false;
    public Color fogColor = Color.black;
    public float fogDensity = 0;
    public float fogEndDistance = 0;
    public FogMode fogMode = FogMode.Linear;
    public float fogStartDistance = 0;
    public float haloStrength = 0;
    public int reflectionBounces = 0;
    public float reflectionIntensity = 0;
    public Material skybox = null;
}


public abstract class SceneInfo : MonoBehaviour
{
    public virtual void OnAwake(Camera sceneCamera)
    {

    }

    public virtual void OnStart(Camera sceneCamera)
    {

    }

    public virtual void OnUpdate(Camera sceneCamera)
    {

    }

    public virtual void OnClear(Camera sceneCamera)
    {

    }
}
