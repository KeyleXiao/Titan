/*******************************************************************
** 文件名:	ImageSetting_Texture.cs  
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	谭强均
** 日  期:	2016-2-24
** 版  本:	1.0
** 描  述:	ImageSetting 图像设置面板 纹理

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
    /// 设置纹理质量
    /// </summary>
    /// <param name="quality"></param>
    public static void SetTextureQuality(TextureQuality quality,bool bForce = false)
    {
        if (OldTextureQuality == quality && !bForce)
        {
            return;
        }
        currData.currTextureQuality = quality;
        OldTextureQuality = quality;
        switch (quality)
        {
            case TextureQuality.Fastest:
                SetTexture_Fastest();
                break;
            case TextureQuality.Good:
                SetTexture_Good();
                break;
            case TextureQuality.High:
                SetTexture_High();
                break;
            case TextureQuality.Best:
                SetTexture_Best();
                break;
        }
        SetTextureAniso(quality);
    }


    /// <summary>
    /// 设置是否开启纹理的各项异性
    /// </summary>
    /// <param name="bEnable"></param>
    public static void SetTextureAniso(TextureQuality quality)
    {
        QualitySettings.anisotropicFiltering = quality >= TextureQuality.High ? AnisotropicFiltering.ForceEnable : AnisotropicFiltering.Disable ;
    }

    private static void SetTexture_Fastest()
    {
        QualitySettings.masterTextureLimit = 2;
    }

    private static void SetTexture_Good()
    {
        QualitySettings.masterTextureLimit = 1;
    }

    private static void SetTexture_High()
    {
        QualitySettings.masterTextureLimit = 0;
    }

    private static void SetTexture_Best()
    {
        QualitySettings.masterTextureLimit = 0;
    }
}