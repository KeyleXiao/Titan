using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;


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


/// <summary>
/// 游戏定义
/// </summary>
public class ImageSettingData
{
    /// <summary>
    /// 当前图像等级
    /// </summary>
    public ImageQuality currImageQuality = ImageQuality.Fastest;

    /// <summary>
    /// 当前阴影等级
    /// </summary>
    public ShadowQuality currShadowQuality = ShadowQuality.Fastest;

    /// <summary>
    /// 阴影质量描述
    /// </summary>
    public const string ShadowQualityDesc = "降低这个品级可用大幅度提高游戏的流畅度，以及略微降低内存的消耗";
    /// <summary>
    /// 当前贴图等级
    /// </summary>
    public TextureQuality currTextureQuality = TextureQuality.Fastest;

    /// <summary>
    /// 贴图质量描述
    /// </summary>
    public const string TextureQualityDesc = "降低这个品级可用大幅度降低内存消耗以及提高游戏的流畅度";


    /// <summary>
    /// 像素光描述
    /// </summary>
    public const string PixelLightDesc = "降低这个品级可用提升游戏流畅度";

    /// <summary>
    /// 当前场景渲染等级
    /// </summary>
    public SceneRenderingQuality currSceneRenderingQuality = SceneRenderingQuality.Fastest;
    /// <summary>
    /// 场景渲染描述
    /// </summary>
    public string SceneRenderingDesc = "降低这个品级可以大幅度提高游戏流畅度";

    public int SSAOEnabled = 0;
    public int FXAAEnabled = 0;
    public int BloomAndFlaresEnabled = 0;

    /// <summary>
    /// 开启进阶后处理，比如fxaa、ssao之类的
    /// </summary>
    public int AdvanceImageEffectEnabled = 0;

    /// <summary>
    /// 当前光效等级
    /// </summary>
    public EffectQuality currEffectQuality = EffectQuality.QUALITY_FASTEST;

    //当前游戏亮度
    public float currGameBrightness = 1.0f;
    public float MaxGameBrightness = 1.9f;
    public float MinGameBrightness = 0.7f;


    /// <summary>
    /// 窗口类型
    /// </summary>
    public WindowType currWindowType = WindowType.Window;


    /// <summary>
    /// 各项异性描述
    /// </summary>
    public const string EffectQualityDesc = "降低这个品级可用大幅度提升游戏流畅度";

    /// <summary>
    /// 分辨率
    /// </summary>
    public string currResolution = string.Empty;
}

