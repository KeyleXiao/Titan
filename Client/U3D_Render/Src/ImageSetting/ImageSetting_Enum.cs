/*******************************************************************
** 文件名:	ImageSetting_Enum.cs  
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	谭强均
** 日  期:	2016-2-24
** 版  本:	1.0
** 描  述:	ImageSetting 图像设置面板 枚举部分

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
    //public enum ConfigRow
    //{
    //    Shadow,
    //    Texture,
    //    PixelLight,
    //    SceneRendering,
    //    RealTimeReflectionProbe,
    //    Asino,
    //    AutoQuality,
    //    ImageLevel,
    //    EffectQuality,
    //    WaterQuality,
    //    Resolution,
    //    WindowType,
    //    SceneRenderingQuality,
    //}


    /// <summary>
    /// 图像等级
    /// </summary>
    public enum ImageQuality
    {
        Invaild,

        Fastest,
        Good,
        High,
        Best,

        MAX,
    }

    /// <summary>
    /// 阴影等级
    /// </summary>
    public enum ShadowQuality
    {
        Invaild,

        /// <summary>
        /// 关闭阴影
        /// </summary>
        Fastest,

        /// <summary>
        /// 只开启主阴影,阴影距离:30,ShadowMap为中,关闭阴影瀑布,只能使用硬阴影
        /// </summary>
        Good,

        /// <summary>
        /// 只开启主阴影,阴影距离:60,ShadowMap为高,两层阴影瀑布,使用硬阴影或者软阴影
        /// </summary>
        High,

        /// <summary>
        /// 只开启主和辅阴影,阴影距离:120,ShadowMap为非常高,四层阴影瀑布,使用硬阴影或者软阴影
        /// </summary>
        Best,

        MAX,
    }

    /// <summary>
    /// 贴图等级
    /// </summary>
    public enum TextureQuality
    {
        Invaild,

        /// <summary>
        /// 质量为原图的八分之一
        /// </summary>
        Fastest,

        /// <summary>
        /// 质量为原图的四分之一
        /// </summary>
        Good,

        /// <summary>
        /// 质量为原图的二分之一
        /// </summary>
        High,


        /// <summary>
        /// 原图
        /// </summary>
        Best,

        MAX,
    }

    ///// <summary>
    ///// 像素灯等级
    ///// </summary>
    //public enum PixelLightQuality
    //{
    //    Invaild,

    //    /// <summary>
    //    /// 0个像素灯
    //    /// </summary>
    //    Fastest,

    //    /// <summary>
    //    /// 二个像素灯
    //    /// </summary>
    //    Good,

    //    /// <summary>
    //    /// 三个像素灯
    //    /// </summary>
    //    High,


    //    /// <summary>
    //    /// 四个像素灯
    //    /// </summary>
    //    Best,

    //    MAX,
    //}

    /// <summary>
    /// 场景渲染等级
    /// </summary>
    public enum SceneRenderingQuality
    {
        Invaild = 0,

        Fastest = 1,
        Good = 2,
        High = 3,
        Best = 4,

        MAX = 5,
    }

    /// <summary>
    /// 特效等级
    /// </summary>
    public enum EffectQuality
    {
        Invalid = 0,

        /// <summary>
        /// 最快
        /// </summary>
        QUALITY_FASTEST = 1,

        /// <summary>
        /// 中
        /// </summary>
        QUALITY_MEDIUM = 2,

        /// <summary>
        /// 好
        /// </summary>
        QUALITY_GOOD = 3,

        /// <summary>
        /// 最好
        /// </summary>
        QUALITY_BEST = 4,

        Max = 5,
    }

    ///// <summary>
    ///// 水质量
    ///// </summary>
    //public enum WaterQuality
    //{
    //    Invalid,

    //    Fastest,// 最快，没有反射和折射
    //    Middle,//中，有反射
    //    High,//高,有反射折射

    //    Max,
    //}

    /// <summary>
    /// 窗口类型
    /// </summary>
    public enum WindowType
    {
        Invalid,

        FullScreen,
        Window,
        WindowMaxSize,

        Max,
    }

    public enum CameraRenderingPath
    {
        /// <summary>
        /// 强制使用VertexLit,
        /// </summary>
        VertexLit,

        /// <summary>
        /// 强制使用Forward
        /// </summary>
        Forward,

        /// <summary>
        /// 使用原本的设置
        /// </summary>
        UseOrginal,
    }

}