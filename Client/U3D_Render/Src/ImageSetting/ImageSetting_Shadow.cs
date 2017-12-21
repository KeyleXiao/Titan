/*******************************************************************
** 文件名:	ImageSetting_Shadow.cs  
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	谭强均
** 日  期:	2016-2-24
** 版  本:	1.0
** 描  述:	ImageSetting 图像设置面板 阴影

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
    /// 这个由于只能通过unity的质量等级设置，所以调用这个函数的时候会
    /// 同时设置贴图、像素灯数目、实时反射探针、各项异性
    /// </summary>
    /// <param name="quality"></param>
    public static void SetShadowQuality(ShadowQuality quality,bool bForce = false)
    {
        if (OldShadowQuality != quality || bForce)
        {
            OldShadowQuality = quality;
            currData.currShadowQuality = quality;
            switch (quality)
            {
                case ShadowQuality.Fastest:
                    SetShadow_Fastest();
                    break;
                case ShadowQuality.Good:
                    SetShadow_Good();
                    break;
                case ShadowQuality.High:
                    SetShadow_High();
                    break;
                case ShadowQuality.Best:
                    SetShadow_Best();
                    break;
            }
        }




        SetTextureQuality(currData.currTextureQuality, true);
        SetPixelLightQuality(currData.currSceneRenderingQuality);
        SetRealTimeReflectionProbe(currData.currSceneRenderingQuality);
        SetTextureAniso(currData.currTextureQuality);
    }


    private static void SetShadow_Fastest()
    {
        //阴影距离，shadowmap等级，阴影瀑布以及软和赢阴影都在这里设置了,谁让unity不开放接口出来呢.
        QualitySettings.SetQualityLevel(0, true);

        QualitySettings.shadowDistance = 1;
        QualitySettings.shadowResolution = ShadowResolution.Low;
        QualitySettings.shadowProjection = ShadowProjection.StableFit;
        QualitySettings.shadowCascades = 0;
    }


    private static void SetShadow_Good()
    {
        //阴影距离，shadowmap等级，阴影瀑布以及软和赢阴影都在这里设置了,谁让unity不开放接口出来呢.
        QualitySettings.SetQualityLevel(1, true);
        QualitySettings.shadowDistance = 70;
        QualitySettings.shadowResolution = ShadowResolution.Medium;
        QualitySettings.shadowProjection = ShadowProjection.StableFit;
        QualitySettings.shadowCascades = 0;
    }


    private static void SetShadow_High()
    {
        //阴影距离，shadowmap等级，阴影瀑布以及软和赢阴影都在这里设置了,谁让unity不开放接口出来呢.
        QualitySettings.SetQualityLevel(2, true);
        QualitySettings.shadowDistance = 100;
        QualitySettings.shadowResolution = ShadowResolution.High;
        QualitySettings.shadowProjection = ShadowProjection.StableFit;
        QualitySettings.shadowCascades = 0;
    }


    private static void SetShadow_Best()
    {
        //阴影距离，shadowmap等级，阴影瀑布以及软和赢阴影都在这里设置了,谁让unity不开放接口出来呢.
        QualitySettings.SetQualityLevel(3, true);
        QualitySettings.shadowDistance = 150;
        QualitySettings.shadowResolution = ShadowResolution.High;
        QualitySettings.shadowProjection = ShadowProjection.StableFit;
        QualitySettings.shadowCascades = 0;
    }

}