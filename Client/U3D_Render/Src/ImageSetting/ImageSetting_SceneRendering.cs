/*******************************************************************
** 文件名:	ImageSetting_SceneRendering.cs  
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	谭强均
** 日  期:	2016-2-24
** 版  本:	1.0
** 描  述:	ImageSetting 图像设置面板 场景渲染

********************************************************************/
using UnityEngine;
using System;
using Effect;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using GameLogic;

/// <summary>
/// 画面设置
/// </summary>
public partial class ImageSetting : MonoBehaviour
{

    /// <summary>
    /// 场景渲染等级改变，前面是新等级，后面是旧等级
    /// </summary>
    public static Action<SceneRenderingQuality,SceneRenderingQuality> OnSceneRenderingQualityChanged = null;

    /// <summary>
    /// 设置纹理质量
    /// </summary>
    /// <param name="quality"></param>
    public static void SetSceneRenderingQuality(SceneRenderingQuality quality, bool bForce = false)
    {
        SetSceneRenderingQuality(SceneResInfoManager.AllInstance, quality, bForce);
    }

    /// <summary>
    /// 设置纹理质量
    /// </summary>
    /// <param name="quality"></param>
    public static void SetSceneRenderingQuality(IEnumerable<SceneResInfoManager> Instances, SceneRenderingQuality quality, bool bForce = false)
    {
        if(OldSceneRenderingQuality == quality && !bForce)
        {
            return;
        }

        switch (quality)
        {
            case SceneRenderingQuality.Fastest:
                foreach(SceneResInfoManager Instance in Instances)
                {
                    if(Instance)
                    {
                        Instance.OnSceneRenderingQuality_Fastest();
                    }
                }
                ImageEffects.ImageEffectManager.API_ImageQuality_Fastest();
                break;
            case SceneRenderingQuality.Good:
                foreach (SceneResInfoManager Instance in Instances)
                {
                    if (Instance)
                    {
                        Instance.OnSceneRenderingQuality_Good();
                    }
                }
                ImageEffects.ImageEffectManager.API_ImageQuality_Good();
                break;
            case SceneRenderingQuality.High:
                foreach (SceneResInfoManager Instance in Instances)
                {
                    if (Instance)
                    {
                        Instance.OnSceneRenderingQuality_High();
                    }
                }
                ImageEffects.ImageEffectManager.API_ImageQuality_High();
                break;
            case SceneRenderingQuality.Best:
                foreach (SceneResInfoManager Instance in Instances)
                {
                    if (Instance)
                    {
                        Instance.OnSceneRenderingQuality_Best();
                    }
                }
                ImageEffects.ImageEffectManager.API_ImageQuality_Best();
                break;
        }
        SetPixelLightQuality(quality);
        SetRealTimeReflectionProbe(quality);
        if (OnSceneRenderingQualityChanged != null)
        {
            OnSceneRenderingQualityChanged(quality, OldSceneRenderingQuality);
        }

        currData.currSceneRenderingQuality = quality;
        OldSceneRenderingQuality = quality;
       
    }

    /// <summary>
    /// 应用场景里的图像设置,应当在每次场景加载的时候都调用一次,并且强制调用
    /// </summary>
    public static void ApplySceneImageSetting(bool bForce = false)
    {
        ApplySceneImageSetting(SceneResInfoManager.AllInstance,bForce);
    }


    /// <summary>
    /// 应用场景里的图像设置,应当在每次场景加载的时候都调用一次,并且强制调用
    /// </summary>
    public static void ApplySceneImageSetting(SceneResInfoManager Instance, bool bForce = false)
    {
        SceneResInfoManager []InstanceList = new SceneResInfoManager[] { Instance };
        ApplySceneImageSetting(InstanceList, bForce);
    }

    /// <summary>
    /// 当场景加载完了之后，调用这个来设置场景的灯光和后处理等问题，因为场景默认是最高配的
    /// 在低端机上，如果不修改，会特别的卡,同时刷新摄像机配置
    /// </summary>
    public static void ApplySceneImageSettingWhenAfterLoadingScene()
    {
        switch (currData.currSceneRenderingQuality)
        {
            case SceneRenderingQuality.Fastest:
                SceneResInfoManager.CurrentActiveInstance.ApplyRenderingPath(RenderingPath.Forward);
                ImageEffects.ImageEffectManager.API_ImageQuality_Fastest();
                break;
            case SceneRenderingQuality.Good:
                SceneResInfoManager.CurrentActiveInstance.ApplyRenderingPath(RenderingPath.Forward);
                ImageEffects.ImageEffectManager.API_ImageQuality_Good();
                break;
            case SceneRenderingQuality.High:
                SceneResInfoManager.CurrentActiveInstance.ApplyRenderingPath(SceneResInfoManager.CurrentActiveInstance.OrginalRenderPath);
                ImageEffects.ImageEffectManager.API_ImageQuality_High();
                break;
            case SceneRenderingQuality.Best:
                SceneResInfoManager.CurrentActiveInstance.ApplyRenderingPath(SceneResInfoManager.CurrentActiveInstance.OrginalRenderPath);
                ImageEffects.ImageEffectManager.API_ImageQuality_Best();
                break;
        }
    }

    /// <summary>
    /// 应用场景里的图像设置,应当在每次场景加载的时候都调用一次,并且强制调用
    /// </summary>
    public static void ApplySceneImageSetting(IEnumerable<SceneResInfoManager> Instances, bool bForce = false)
    {
        SetSceneRenderingQuality(Instances, currData.currSceneRenderingQuality, bForce);
    }

    private static void ActiveSceneChaned(SceneResInfoManager manager)
    {
        ApplySceneImageSettingWhenAfterLoadingScene();
    }
}