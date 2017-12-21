/*******************************************************************
** 文件名:	ImageSetting_Effect.cs  
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	谭强均
** 日  期:	2016-2-24
** 版  本:	1.0
** 描  述:	ImageSetting 图像设置面板 特效

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
    /// 设置特效质量
    /// </summary>
    /// <param name="quality"></param>
    public static void SetEffectQuality(EffectQuality quality,bool bForce = false)
    {
        if (OldEffectQuality == quality && !bForce)
        {
            return;
        }
        OldEffectQuality = quality;
        currData.currEffectQuality = quality;
        Effect.EffectNode.ChangeEffectQualityLevel();
    }
}