/*******************************************************************
** 文件名:	ImageSetting_Shadow.cs  
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	谭强均
** 日  期:	2016-2-24
** 版  本:	1.0
** 描  述:	ImageSetting 图像设置面板 其他

****************************** 修改记录 ****************************
** 修改人:  昔文博
** 日  期:  2016/7/21
** 描  述:  当图像等级与之前一样，但子项目有修改时也需要适配一次
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
    /// 设置实时反射探针
    /// </summary>
    /// <param name="bEnable"></param>
    public static void SetRealTimeReflectionProbe(SceneRenderingQuality quality)
    {
        QualitySettings.realtimeReflectionProbes = quality >= SceneRenderingQuality.Best;
    }

    /// <summary>
    /// 应用设置,必须在场景加载完成之后调用
    /// </summary>
    public static void Apply(bool bForce)
    {
        if (SceneManager.isLoadingScene)
        {
            return;
        }

        ApplySceneImageSetting(bForce);
        //会同时设置贴图、像素灯数目、实时反射探针、各项异性
        SetShadowQuality(currData.currShadowQuality, bForce);
        //SetRealTimeReflectionProbe(currRealTimeReflectionProbeEnabled, bForce);
        SetEffectQuality(currData.currEffectQuality, bForce);
        //SetWaterQuality(currWaterQuality, bForce);
    }

    public static void SetImageQualityData(ImageQuality quality)
    {
        OldImageQuality = quality;
        currData.currImageQuality = quality;
        switch (quality)
        {
            case ImageQuality.Fastest:
                SetImageQualityData_Fastest();
                break;
            case ImageQuality.Good:
                SetImageQualityData_Good();
                break;
            case ImageQuality.High:
                SetImageQualityData_High();
                break;
            case ImageQuality.Best:
                SetImageQualityData_Best();
                break;
        }
    }

    /// <summary>
    /// 设置图像等级
    /// </summary>
    /// <param name="quality"></param>
    public static void SetImageQuality(ImageQuality quality,bool bForce = false)
    {
        //if(OldImageQuality == quality)
        //{
        //    return;
        //}

        SetImageQualityData(quality);
        Apply(bForce);
    }

    private static void SetImageQualityData_Fastest()
    {
        currData.currShadowQuality = ShadowQuality.Fastest;
        currData.currTextureQuality = TextureQuality.Fastest;
        //currPixelLightQuality = PixelLightQuality.Fastest;
        currData.currSceneRenderingQuality = SceneRenderingQuality.Fastest;
        currData.currEffectQuality = EffectQuality.QUALITY_FASTEST;
        //currWaterQuality = WaterQuality.Fastest;
        //currRealTimeReflectionProbeEnabled = 0;
        //currAsinoEnabled = 0;

    }

    private static void SetImageQualityData_Good()
    {

        currData.currShadowQuality = ShadowQuality.Good;
        currData.currTextureQuality = TextureQuality.High;
        //currPixelLightQuality = PixelLightQuality.Good;
        currData.currSceneRenderingQuality = SceneRenderingQuality.Good;
        currData.currEffectQuality = EffectQuality.QUALITY_MEDIUM;
        //currWaterQuality = WaterQuality.Fastest;
        //currRealTimeReflectionProbeEnabled = 0;
        //currAsinoEnabled = 1;
    }

    private static void SetImageQualityData_High()
    {
        currData.currShadowQuality = ShadowQuality.High;
        currData.currTextureQuality = TextureQuality.High;
        //currPixelLightQuality = PixelLightQuality.High;
        currData.currSceneRenderingQuality = SceneRenderingQuality.High;
        currData.currEffectQuality = EffectQuality.QUALITY_GOOD;
        //currWaterQuality = WaterQuality.Middle;
        //currRealTimeReflectionProbeEnabled = 0;
        //currAsinoEnabled = 1;
    }

    private static void SetImageQualityData_Best()
    {
        currData.currShadowQuality = ShadowQuality.Best;
        currData.currTextureQuality = TextureQuality.Best;
        //currPixelLightQuality = PixelLightQuality.Best;
        currData.currSceneRenderingQuality = SceneRenderingQuality.Best;
        currData.currEffectQuality = EffectQuality.QUALITY_BEST;
        //currWaterQuality = WaterQuality.High;
        //currRealTimeReflectionProbeEnabled = 1;
        //currAsinoEnabled = 1;
    }

    public static void SetDefaultSettingData()
    {
        SetImageQualityData(ImageQuality.Good);
        ResetOthersSettings();
    }

    public static void ResetOthersSettings()
    {
        currData.SSAOEnabled = 0;
        currData.FXAAEnabled = 0;
        currData.BloomAndFlaresEnabled = 0;
        currData.AdvanceImageEffectEnabled = 0;
    }

    /// <summary>
    /// 重设为默认配置
    /// </summary>
    public static void SetDefaultSetting()
    {
        SetDefaultSettingData();
        Apply(true);
    }


    /// <summary>
    /// 在当前等级的基础上，递增一个层级
    /// </summary>
    private static void IncreaseQuality()
    {

        int max = (int)ImageQuality.Best;

        int val = (int)currData.currImageQuality;
        val++;
        val = Mathf.Min(val, max + 1);
        currData.currImageQuality = (ImageQuality)val;
        if (OldImageQuality == currData.currImageQuality)
        {
            return;
        }
        SetImageQuality(currData.currImageQuality);
    }


    /// <summary>
    /// 在当前等级的基础上，递减一个层级
    /// </summary>
    private static void DecreaseQulaity()
    {
        int min = (int)ImageQuality.Fastest;

        int val = (int)currData.currImageQuality;
        val--;
        val = Mathf.Max(val, min - 1);
        currData.currImageQuality = (ImageQuality)val;

        if (OldImageQuality == currData.currImageQuality)
        {
            return;
        }
        SetImageQuality(currData.currImageQuality);
    }
}