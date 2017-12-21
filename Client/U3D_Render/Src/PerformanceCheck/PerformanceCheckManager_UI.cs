/*******************************************************************
** 文件名:	PerformanceCheckManager.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2014/11/21
** 版  本:	1.0
** 描  述:   性能检测脚本
** 应  用:  	跟随物体

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public partial class PerformanceCheckManager : MonoBehaviour
{

    public static string GetImageQualityDesc(ImageSetting.ImageQuality quality)
    {
        switch (quality)
        {
            case ImageSetting.ImageQuality.Fastest:
                return "低级画质";
            case ImageSetting.ImageQuality.Good:
                return "中级画质";
            case ImageSetting.ImageQuality.High:
                return "高级画质";
            case ImageSetting.ImageQuality.Best:
                return "极高画质";
        }

        return "未知";
    }

    public static string GetDownImageQualityDesc(ImageSetting.ImageQuality quality)
    {
        int min = (int)ImageSetting.ImageQuality.Invaild;

       int val = (int)quality;
        val--;
        val = Mathf.Max(val, min - 1);
        quality = (ImageSetting.ImageQuality)val;

        return GetDownImageQualityDesc(quality);
    }
    private void InitUI()
    {
        flowShareData.ui_Reslut.gameObject.SetActive(false);
        flowShareData.ui_Checking.gameObject.SetActive(false);
        flowShareData.ui_Waiting.gameObject.SetActive(false);
        flowShareData.CheckingBackground.gameObject.SetActive(false);
        flowShareData.breakTips.gameObject.SetActive(false);

        flowShareData.ui_Waiting.Init();
        flowShareData.ui_Checking.Init();
        flowShareData.ui_Reslut.Init();
        flowShareData.CheckingBackground.Init();
        flowShareData.breakTips.Init();
    }

    private void ClearUI()
    {
        flowShareData.ui_Waiting.Clear();
        flowShareData.ui_Checking.Clear();
        flowShareData.ui_Reslut.Clear();
        flowShareData.CheckingBackground.Clear();
        flowShareData.breakTips.Clear();
    }


}



