using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
// 定义实体ID
using ENTITY_ID = System.Int32;


/// <summary>
/// 游戏定义
/// </summary>
public class Config
{
    public const string LayerPlayerFriend           = "Player_Friend";
    public const string LayerPlayerEnemy            = "Player_Enemy";
    public const string LayerMonsterFriend          = "Monster_Friend";
    public const string LayerMonsterEnemy           = "Monster_Enemy";
    public const string LayerScreenRaycastFriend    = "ScreenRaycast_Friend";
    public const string LayerScreenRaycastEnemy     = "ScreenRaycast_Enemy";
    public const string LayerHero                   = "Hero";    
    public const string LayerLightingEffectOnly     = "LightingEffectOnly";
    public const string LayerNeutrality             = "Neutrality";
    public const string LayerDefault                = "Default";
    public const string LayerTransparentFX          = "TransparentFX";
    public const string LayerIngoreRayCast          = "Ignore Raycast";
    public const string LayerWater                  = "Water";
    public const string LayerWard_Friend            = "Ward_Friend";
    public const string LayerWard_Enemy             = "Ward_Enemy";
    public const string LayerBuilding               = "Building";
    public const string LayerUI                     = "UI";
    public const string LayerOcclusCulling          = "OcclusCulling";

    public const string LayerTopName = "TopName";

    public const string SortingLayerDefault         = "Default";
    public const string SortingLayerUI_FIXED        = "UI_Fixed";
    public const string SortingLayerUI_POPUP01      = "UI_Popup01";
    public const string SortingLayerUI_FILL         = "UI_Fill";
    public const string SortingLayerUI_POPUP02      = "UI_Popup02";
    public const string SortingLayerUI_OVERHEAD     = "UI_OverHead";
    public const string SortingLayerUI              = "UI";

    public const bool UseOldSlot = false;
    public const bool UseNewSlot = true;

    // 登录的账号名称
    public static string AccountName                = "";
    // 充值相关链接
    public const string PayUrl                      = "http://pay.tt.q1.com";
    public const string PayQRUrl                    = "http://m.pay.q1.com/index.html";
    public const string QRBuildUrl                  = "http://m.q1.com/qr.aspx?data=";
}

public struct RECTAPI
{
    public Int32 left, top, right, bottom;

    public override string ToString()
    {
        return ("Left:" + left + ",Top:" + top + ",Right:" + right + ",Bottom:" + bottom);
    }
}
public struct POINTAPI
{
    public Int32 x;
    public Int32 y;

    public override string ToString()
    {
        return ("x:" + x + ",y:" + y);
    }
}

public class UIBillboardQueryResult
{
    public ENTITY_ID entityID;
    public float x;
    public float y;
    public float scale;
    public uint distance;
    public int angle;
}

public enum EStage_SubState
{
    ESSS_NONE,          // 空
    ESSS_WAR,           // 战场中
    ESSS_WAREND,        // 战场结束
}

public enum EStage_PermissionState
{
    ESPS_NORMAL,       // 普通玩家
    ESPS_OB,           // GM_OB
}

#region 任务栏闪烁相关
/// <summary>
/// 任务栏闪烁
/// </summary>
public struct FLASHWINFO
{
    /// <summary>
    /// 结构大小
    /// </summary>
    public uint cbSize;
    /// <summary>
    /// 要闪烁或停止的窗口句柄
    /// </summary>
    public IntPtr hwnd;
    /// <summary>
    ///  闪烁的类型
    /// </summary>
    public uint dwFlags;
    /// <summary>
    /// 闪烁窗口的次数
    /// </summary>
    public uint uCount;
    /// <summary>
    /// 窗口闪烁的频度，毫秒为单位；若该值为0，则为默认图标的闪烁频度
    /// </summary>
    public uint dwTimeout;
}
public enum FlashWType : uint
{
    FLASHW_STOP = 0, //停止闪烁
    FALSHW_CAPTION = 1, //只闪烁标题
    FLASHW_TRAY = 2, //只闪烁任务栏
    FLASHW_ALL = 3, //标题和任务栏同时闪烁
    FLASHW_PARAM1 = 4,
    FLASHW_PARAM2 = 12,
    FLASHW_TIMER = FLASHW_TRAY | FLASHW_PARAM1, //无条件闪烁任务栏直到发送停止标志或者窗口被激活，如果未激活，停止时高亮
    FLASHW_TIMERNOFG = FLASHW_TRAY | FLASHW_PARAM2 //未激活时闪烁任务栏直到发送停止标志或者窗体被激活，停止后高亮
}
#endregion


#region 系统参数相关
public enum ESystemParameters : uint
{
    SPI_SETMOUSESPEED = 0x0071,
    SPI_GETMOUSESPEED = 0x0070,
}

public enum ESystemMetrics : int
{
    SM_CYCAPTION = 4,
    SM_CXFRAME = 32,
}
#endregion