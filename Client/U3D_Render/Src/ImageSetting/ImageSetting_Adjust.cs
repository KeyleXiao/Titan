/*******************************************************************
** 文件名:	ImageSetting_Adjust.cs  
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	谭强均
** 日  期:	2016-2-24
** 版  本:	1.0
** 描  述:	ImageSetting 图像设置面板 自动调整部分

**************************** 修改记录 ******************************
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
using USpeedUI;

/// <summary>
/// 画面设置
/// </summary>
public partial class ImageSetting : MonoBehaviour
{
    private static bool FirstLoadScene = true;
    //系统所能支持的最高画质等级，默认为Best
    private static ImageQuality HighestSupportQuality = ImageQuality.Best;
    public static void OnSceneLoadFinish()
    {
        if (FirstLoadScene)
        {
            //第一次场景要适配一次
            AdjustLogic(GameFrameManager.FpsMonitor.AvgFps);
            FirstLoadScene = false;
        }

    }

    public static void SetQuality(bool bClickAdjustBtn, ImageQuality quality)
    {
        SetImageQuality(quality);
        //ApplyUIData();
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SYSTEMSETTING_APPLYVEDIOSETTING, null);
        HighestSupportQuality = currData.currImageQuality;
    }

    private static void AdjustQuality(bool bClickAdjustBtn,ImageQuality quality)
    {
        //第一次进场景不需要弹窗直接更新，后续的才需要弹窗
        if (!FirstLoadScene && bClickAdjustBtn)
        {
            USpeedUI.TooltipAndDialog.UIDialogData data = new USpeedUI.TooltipAndDialog.UIDialogData(
                               title: "系统设置",
                               message: "已恢复系统默认设置",
                               buttons: new UIWidgets.DialogActions(){
                                     {"调整",() =>
                                        {
                                            SetQuality(bClickAdjustBtn,quality);
                                            return true;
                                        }
                                     },
                                     {"不调整",() => {return true;}}
                               }
                               );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
        }
        else if (FirstLoadScene)
        {
            SetQuality(bClickAdjustBtn, quality);
        }
    }

    /// <summary>
    /// 自动适配画质(自动适配有两种情况：1.通过按钮点击选择了开启自动适配，这种情况下有可能currImageQuality和当前需要调整的值适配，但分项不匹配，所以强制调整一下画质等级所有项目；
    /// 2.游戏开始或者FPS改变时，需要做一下适配，如果和当前画质等级不一致，并且低于系统所能支持的最高等级，则切换等级，并保存当前画质等级为系统所能支持的最高配。
    /// PS：游戏开始时，默认最高档，根据FPS值降档，如果从某档位掉下，则不能再升至此档位)
    /// </summary>
    /// <param name="avgFPS"></param>FPS值
    /// <param name="bClickAdjustBtn"></param>是否通过点击UI按钮选择了自动调整功能
    public static void AdjustLogic(float avgFPS, bool bClickAdjustBtn = false)
    {
        /*不在动态调整画质，改为性能检测*/
        ////没有开启自动调节或者在加载场景，不管
        //if (AutoAdjustQuality <= 0 || (!Initialize.isOnFocus && !Application.isEditor) || SceneManager.isLoadingScene)
        //{
        //    return;
        //}

        ////如果是通过UI按钮点击选择开启自动适配，则尝试把最高配先调到最高
        //if (bClickAdjustBtn)
        //    HighestSupportQuality = ImageQuality.Best;

        ////这样写不对，因为在当前是High的时候，平均帧率大于55，应该是保持，而不是Best！
        //ImageQuality appropriateQuality = HighestSupportQuality;
        //if (avgFPS < 30)
        //{
        //    appropriateQuality = HighestSupportQuality - 3;
        //}

        //if (avgFPS >= 30 && avgFPS < 40)
        //{
        //    appropriateQuality = HighestSupportQuality - 2;
        //}

        //if (avgFPS >= 40 && avgFPS < 55)
        //{
        //    appropriateQuality = HighestSupportQuality - 1;
        //}
        //if (appropriateQuality <= ImageQuality.Invaild)
        //    appropriateQuality = ImageQuality.Fastest;

        ////大于55保持当前设置
        //if (avgFPS < 55)
        //{
        //    AdjustQuality(bClickAdjustBtn, appropriateQuality);
        //}

    }
}