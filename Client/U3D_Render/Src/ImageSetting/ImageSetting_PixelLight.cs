/*******************************************************************
** 文件名:	ImageSetting_Shadow.cs  
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	谭强均
** 日  期:	2016-2-24
** 版  本:	1.0
** 描  述:	ImageSetting 图像设置面板 其他

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
public partial class ImageSetting 
{


    /// <summary>
    /// 设置像素灯质量
    /// </summary>
    /// <param name="quality"></param>
    public static void SetPixelLightQuality(SceneRenderingQuality quality)
    {
        switch (quality)
        {
            case SceneRenderingQuality.Fastest:
                SetPixelLight_Fastest();
                break;
            case SceneRenderingQuality.Good:
                SetPixelLight_Good();
                break;
            case SceneRenderingQuality.High:
                SetPixelLight_High();
                break;
            case SceneRenderingQuality.Best:
                SetPixelLight_Best();
                break;
        }
    }

    private static void SetPixelLight_Fastest()
    {
        QualitySettings.pixelLightCount = 0;
    }

    private static void SetPixelLight_Good()
    {
        QualitySettings.pixelLightCount = 1;
    }

    private static void SetPixelLight_High()
    {
        QualitySettings.pixelLightCount = 2;
    }

    private static void SetPixelLight_Best()
    {
        QualitySettings.pixelLightCount = 4;
    }
}