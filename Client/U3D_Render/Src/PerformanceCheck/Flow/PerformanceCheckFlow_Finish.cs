/*******************************************************************
** 文件名:	PerformanceCheckFlow_HardWareSupport.cs
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
using ASpeedGame.Data.PlayerSettingLocalConfig;

public partial class PerformanceCheckFlow_Finish : PerformanceCheckFlowBase
{
    enum ReslutImageFlags
    {
        None,

        /// <summary>
        /// 不支持全屏下的最高配后处理
        /// </summary>
        NotSupportFullBestImageEffect,

        /// <summary>
        /// 不支持全屏下的高配后处理
        /// </summary>
        NotSupportFullHighImageEffect,

        /// 不支持全屏下的中配后处理
        /// </summary>
        NotSupportFullGoodImageEffect,


        /// <summary>
        /// 开启其他后处理
        /// </summary>
        OpenOhtersImageEffects,
    }

    //private bool WaitingSelect = false;
    //private float currWaitingTime = 0;
    //private int AutoSelectTime = 30;
    private string TargetResolution = string.Empty;
    ReslutImageFlags flag = ReslutImageFlags.None;

    //System.Action currAutoSelect = null;

    public override void OnEnter()
    {
        //currAutoSelect = null;
        PerformanceCheckManager.Instance.flowShareData.StartCheckTimer = false;
        //PerformanceCheckManager.Instance.flowShareData.ui_Checking.gameObject.SetActive(false);

        if(PerformanceCheckManager.Instance.ReslutData.NotDoPerformanceCheck)
        {
            FinishWithNotDoCheck();
        }
        else if(PerformanceCheckManager.Instance.ReslutData.VeryLowMachine)
        {
            FinishWithVeryLowMachine();
        }
        else if(PerformanceCheckManager.Instance.ReslutData.notSupportRT)
        {
            FinishWithnotSupportRT();
        }
        else
        {
            FihishWithNormal();
        }
        ExportInfo();
    }

    void ExportInfo()
    {
        PerformanceCheckManager.PerformanceCheckReslut result = PerformanceCheckManager.Instance.ReslutData;
        string path = ViewConfigManager.GetConfigFullPath(PerformanceCheckManager.confingFileName, false);
        System.IO.StreamWriter writer = new System.IO.StreamWriter(path, false, new System.Text.UTF8Encoding(true));

        
        string s = "";
        s = "PerformanceCheckVersion" + "," + PerformanceCheckManager.currPerformanceCheckVersion;
        writer.WriteLine(s);

        s = "Fps" + "," + PerformanceCheck_FPS.currFPS;
        writer.WriteLine(s);

        s = "NotDoPerformanceCheck" + "," + result.NotDoPerformanceCheck;
        writer.WriteLine(s);

        s = "VeryLowMachine" + "," + result.VeryLowMachine;
        writer.WriteLine(s);

        s = "notSupportRT" + "," + result.notSupportRT;
        writer.WriteLine(s);

        s = "notSupportHighLevel" + "," + result.notSupportHighLevel;
        writer.WriteLine(s);

        s = "GeometricRenderLevel" + "," + result.GeometricRenderLevel;
        writer.WriteLine(s);

        s = "ImageEffectRenderLevel" + "," + result.ImageEffectRenderLevel;
        writer.WriteLine(s);

        s = "ReslutImageFlags" + "," + flag;
        writer.WriteLine(s);

        writer.Flush();
        writer.Close();
        writer = null;
    }

    void FinishWithNotDoCheck()
    {
        ImageSetting.SetImageQuality(PerformanceCheckManager.BreakCheckQuality);
        ImageSetting.ResetOthersSettings();
        //PerformanceCheckUI_BreakTips uiInstance = PerformanceCheckManager.Instance.flowShareData.breakTips;
        //uiInstance.gameObject.SetActive(true);
        //uiInstance.Desc.text = "您以跳过性能检测，游戏默认<color=red>" + PerformanceCheckManager.GetImageQualityDesc(PerformanceCheckManager.BreakCheckQuality) + 
        //                       ",</color>可在<color=red>登入游戏后按[ESC]->视频设置</color>调节画质。正在进入游戏.....";

        ImageSetting.Clear();
        EndPerformanceCheck();
    }

    void FinishWithVeryLowMachine()
    {
        ImageSetting.SetImageQuality(ImageQuality.Fastest);
        ImageSetting.ResetOthersSettings();
        UpdateReslutUI(ImageQuality.Fastest);
    }

    void FinishWithnotSupportRT()
    {
        ImageSetting.SetImageQuality(ImageQuality.Fastest);
        ImageSetting.ResetOthersSettings();
        UpdateReslutUI(ImageQuality.Fastest);
    }

    void FihishWithNormal()
    {
        //看看几何等级
        PerformanceCheckManager.PerformanceCheckReslut reslut = PerformanceCheckManager.Instance.ReslutData;
        if(reslut.GeometricRenderLevel <= 1 || reslut.ImageEffectRenderLevel <= 1)//最低级
        {
            ImageSetting.SetImageQuality(ImageQuality.Fastest);
            ImageSetting.ResetOthersSettings();
            UpdateReslutUI(ImageQuality.Fastest);
            return;
        }

        //中级情况1，所有都通过了
        if(reslut.GeometricRenderLevel == 2 && reslut.ImageEffectRenderLevel == 2)
        {
            ImageSetting.SetImageQuality(ImageQuality.Good);
            ImageSetting.ResetOthersSettings();
            UpdateReslutUI(ImageQuality.Good);
            return;
        }

        //中级情况2，通过了几何，没有通过后处理测试
        if (reslut.GeometricRenderLevel == 2 )
        {
            ImageSetting.SetImageQuality(ImageQuality.Good);
            ImageSetting.ResetOthersSettings();
            flag = ReslutImageFlags.NotSupportFullGoodImageEffect;
            UpdateReslutUI(ImageQuality.Good, flag);
            return;
        }

        //高级情况1，所有都通过了
        if (reslut.GeometricRenderLevel == 3 && reslut.ImageEffectRenderLevel == 3)
        {
            ImageSetting.SetImageQuality(ImageQuality.High);
            ImageSetting.ResetOthersSettings();
            UpdateReslutUI(ImageQuality.High);
            return;
        }

        //高级情况2，通过了几何，没有通过后处理测试
        if (reslut.GeometricRenderLevel == 3 )
        {
            ImageSetting.SetImageQuality(ImageQuality.High);
            ImageSetting.ResetOthersSettings();
            flag = ReslutImageFlags.NotSupportFullHighImageEffect;
            UpdateReslutUI(ImageQuality.High, flag);
            return;
        }

        //最高级情况1，所有测试都通过了
        if (reslut.GeometricRenderLevel == 4 && reslut.ImageEffectRenderLevel == 4)
        {
            //再看一下帧数，帧数超过70fps，可以把抗锯齿和Bloom开启来
            //如果真是超过80fps，可以把抗锯齿，bloom和ao开起来
            ImageSetting.SetImageQuality(ImageQuality.Best);
            ImageSetting.ResetOthersSettings();
            ImageSetting.GetData().AdvanceImageEffectEnabled = 1;
            if (PerformanceCheck_FPS.currFPS > PerformanceCheckManager.PassLevleFps * 1.3f)
            {
                ImageSetting.GetData().FXAAEnabled = 1;
                ImageSetting.GetData().BloomAndFlaresEnabled = 1;
                flag = ReslutImageFlags.OpenOhtersImageEffects;

                if (PerformanceCheck_FPS.currFPS > PerformanceCheckManager.PassLevleFps * 1.4f)
                {
                    ImageSetting.GetData().SSAOEnabled = 1;
                }
            }


            UpdateReslutUI(ImageQuality.Best, flag);
            return;
        }

        //最高级情况2，只通过了几何图形测试，没有通过后处理，这种情况是显卡的能力不足,给予两种选择
        //1。降低分辨率
        //2.关闭后处理
        if (reslut.GeometricRenderLevel == 4)
        {
            ImageSetting.SetImageQuality(ImageQuality.Best);
            ImageSetting.ResetOthersSettings();

            flag = ReslutImageFlags.NotSupportFullBestImageEffect;
            UpdateReslutUI(ImageQuality.Best, flag);
            return;
        }
    }

    void UpdateReslutUI(ImageQuality quality,ReslutImageFlags flag = ReslutImageFlags.None)
    {
        //PerformanceCheckUI_Reslut uiInstance = PerformanceCheckManager.Instance.flowShareData.ui_Reslut;
        //uiInstance.gameObject.SetActive(true);
       

        if(flag == ReslutImageFlags.OpenOhtersImageEffects)
        {
            //uiInstance.tips.text = "您的检测结果为<color=red>" + PerformanceCheckManager.GetImageQualityDesc(quality) + ",</color>由于您的计算机性能较高，系统为您开启了一些额外的渲染效果," +
            //           "您可以在<color=red>登入游戏后按[ESC]->视频设置</color>中修改您的选项。正在进入游戏.....";

            ImageSetting.Clear();
            EndPerformanceCheck();
        }
        else if(flag == ReslutImageFlags.NotSupportFullGoodImageEffect)
        {
            UpdateReslut_NotSupportGoodImangeEffect(quality);
        }
        else if (flag == ReslutImageFlags.NotSupportFullBestImageEffect)
        {
            UpdateReslut_NotSupportBestImangeEffect(quality);
        }
        else if(flag == ReslutImageFlags.NotSupportFullHighImageEffect)
        {
            UpdateReslut_NotSupportHighImangeEffect(quality);
        }
        else
        {
            //uiInstance.tips.text = "您的检测结果为<color=red>" + PerformanceCheckManager.GetImageQualityDesc(quality) + ",</color>" +
            //           "您可以在<color=red>登入游戏后按[ESC]->视频设置</color>中修改您的选项。正在进入游戏.....";

            ImageSetting.Clear();
            EndPerformanceCheck();
        }
    }

    #region NotSupportGoodImangeEffect
    void UpdateReslut_NotSupportGoodImangeEffect(ImageQuality quality)
    {
        NotSupportGoodImangeEffect_ChooseDownQuality();
        //PerformanceCheckUI_Reslut uiInstance = PerformanceCheckManager.Instance.flowShareData.ui_Reslut;
        //uiInstance.tips.text = "您的检测结果为<color=red>" + PerformanceCheckManager.GetImageQualityDesc(quality) + ",</color>但由于你的计算机性能较低,一些额外的渲染效果没有开启，您可以从以下方案中选择一个";

        //uiInstance.ConfigTwoRoot.SetActive(true);
        //uiInstance.ConfigOneRoot.SetActive(true);

        //uiInstance.ConfigOneBtn.onClick.AddListener(NotSupportGoodImangeEffect_ChooseDownResolution);
        //uiInstance.ConfigTwoBtn.onClick.AddListener(NotSupportGoodImangeEffect_ChooseDownQuality);

        //currAutoSelect = NotSupportGoodImangeEffect_ChooseDownResolution;
        //uiInstance.ConfigOneBtnDesc.text = "确认(" + AutoSelectTime + ")";
        //uiInstance.ConfigTwoBtnDesc.text = "确认";

        //int pixel = ImageSetting.GetResolutionPixels(ImageSetting.currResolution);

        ////计算出帧数差
        //float dealt = (PerformanceCheckManager.PassLevleFps - PerformanceCheck_FPS.currFPS);
        //dealt /= PerformanceCheckManager.PassLevleFps;

        //TargetResolution = ImageSetting.AdjustResolutionFromPixels(pixel, 1 - dealt);

        //uiInstance.ConfigOneDesc.text = "此方案，您将会<color=red>获得完整的画面效果</color>,但是您的游戏将会在<color=red>窗口模式下," +
        //                               "以" + TargetResolution + "分辨率</color>运行,您可以再在<color=red>登入游戏后按[ESC]->视频设置->全屏</color>以全屏模式运行游戏";

        //uiInstance.ConfigTwoDesc.text = "此方案您将在全屏模式下进行游戏，但是会<color=red>将渲染质量设为" + PerformanceCheckManager.GetDownImageQualityDesc(quality) + "</color>来提升性能," +
        //                                "您可以再在<color=red>登入游戏后按[ESC]->视频设置</color>修改它";
        //WaitingSelect = true;
    }

    void NotSupportGoodImangeEffect_ChooseDownQuality()
    {
        //WaitingSelect = false;
        //PerformanceCheckUI_Reslut uiInstance = PerformanceCheckManager.Instance.flowShareData.ui_Reslut;
        //uiInstance.tips.text = "测试完成,您可以在<color=red>登入游戏后按[ESC]->视频设置</color>中修改您的选项。正在进入游戏.....";

        //uiInstance.ConfigOneBtn.onClick.RemoveAllListeners();
        //uiInstance.ConfigTwoBtn.onClick.RemoveAllListeners();

        //uiInstance.ConfigTwoRoot.SetActive(false);
        //uiInstance.ConfigOneRoot.SetActive(false);

        ImageSetting.SetImageQuality(ImageQuality.Fastest);
        ImageSetting.ResetOthersSettings();

        ImageSetting.Clear();
        EndPerformanceCheck();
    }

    void NotSupportGoodImangeEffect_ChooseDownResolution()
    {
        //WaitingSelect = false;
        //PerformanceCheckUI_Reslut uiInstance = PerformanceCheckManager.Instance.flowShareData.ui_Reslut;
        //uiInstance.tips.text = "测试完成,您可以在<color=red>登入游戏后按[ESC]->视频设置</color>中修改您的选项。正在进入游戏.....";

        //uiInstance.ConfigOneBtn.onClick.RemoveAllListeners();
        //uiInstance.ConfigTwoBtn.onClick.RemoveAllListeners();
        //uiInstance.ConfigTwoRoot.SetActive(false);
        //uiInstance.ConfigOneRoot.SetActive(false);

        ImageSetting.ResetOthersSettings();
        ImageSetting.SetResolution(TargetResolution, false);

        ImageSetting.Clear();
        EndPerformanceCheck();
    }

    #endregion

    #region NotSupportHighImangeEffect
    void UpdateReslut_NotSupportHighImangeEffect(ImageQuality quality)
    {
        //PerformanceCheckUI_Reslut uiInstance = PerformanceCheckManager.Instance.flowShareData.ui_Reslut;
        //uiInstance.tips.text = "您的检测结果为<color=red>" + PerformanceCheckManager.GetImageQualityDesc(quality) + ",</color>但由于你的计算机性能较低,一些额外的渲染效果没有开启，您可以从以下方案中选择一个";

        //uiInstance.ConfigTwoRoot.SetActive(true);
        //uiInstance.ConfigOneRoot.SetActive(true);

        //uiInstance.ConfigOneBtn.onClick.AddListener(NotSupportHighImangeEffect_ChooseDownResolution);
        //uiInstance.ConfigTwoBtn.onClick.AddListener(NotSupportHighImangeEffect_ChooseDownQuality);

        //currAutoSelect = NotSupportHighImangeEffect_ChooseDownResolution;
        //uiInstance.ConfigOneBtnDesc.text = "确认(" + AutoSelectTime + ")";
        //uiInstance.ConfigTwoBtnDesc.text = "确认";

        //int pixel = ImageSetting.GetResolutionPixels(ImageSetting.currResolution);

        ////计算出帧数差
        //float dealt = (PerformanceCheckManager.PassLevleFps - PerformanceCheck_FPS.currFPS);
        //dealt /= PerformanceCheckManager.PassLevleFps;

        //TargetResolution = ImageSetting.AdjustResolutionFromPixels(pixel, 1 - dealt);

        //uiInstance.ConfigOneDesc.text = "此方案，您将会<color=red>获得完整的画面效果</color>,但是您的游戏将会在<color=red>窗口模式下," +
        //                               "以" + TargetResolution + "分辨率</color>运行,您可以再在<color=red>登入游戏后按[ESC]->视频设置->全屏</color>以全屏模式运行游戏";

        //uiInstance.ConfigTwoDesc.text = "此方案您将在全屏模式下进行游戏，但是会<color=red>将渲染质量设为" + PerformanceCheckManager.GetDownImageQualityDesc(quality) + "</color>来提升性能," +
        //                                "您可以再在<color=red>登入游戏后按[ESC]->视频设置</color>修改它";
        //WaitingSelect = true;
        NotSupportHighImangeEffect_ChooseDownQuality();
    }

    void NotSupportHighImangeEffect_ChooseDownQuality()
    {
        //WaitingSelect = false;
        //PerformanceCheckUI_Reslut uiInstance = PerformanceCheckManager.Instance.flowShareData.ui_Reslut;
        //uiInstance.tips.text = "测试完成,您可以在<color=red>登入游戏后按[ESC]->视频设置</color>中修改您的选项。正在进入游戏.....";

        //uiInstance.ConfigOneBtn.onClick.RemoveAllListeners();
        //uiInstance.ConfigTwoBtn.onClick.RemoveAllListeners();

        //uiInstance.ConfigTwoRoot.SetActive(false);
        //uiInstance.ConfigOneRoot.SetActive(false);

        ImageSetting.SetImageQuality(ImageQuality.Good);
        ImageSetting.ResetOthersSettings();

        ImageSetting.Clear();
        EndPerformanceCheck();
    }

    void NotSupportHighImangeEffect_ChooseDownResolution()
    {
        //WaitingSelect = false;
        //PerformanceCheckUI_Reslut uiInstance = PerformanceCheckManager.Instance.flowShareData.ui_Reslut;
        //uiInstance.tips.text = "测试完成,您可以在<color=red>登入游戏后按[ESC]->视频设置</color>中修改您的选项。正在进入游戏.....";

        //uiInstance.ConfigOneBtn.onClick.RemoveAllListeners();
        //uiInstance.ConfigTwoBtn.onClick.RemoveAllListeners();
        //uiInstance.ConfigTwoRoot.SetActive(false);
        //uiInstance.ConfigOneRoot.SetActive(false);

        ImageSetting.ResetOthersSettings();
        ImageSetting.SetResolution(TargetResolution, false);

        ImageSetting.Clear();
        EndPerformanceCheck();
    }

    #endregion


    #region NotSupportBestImangeEffect

    void UpdateReslut_NotSupportBestImangeEffect(ImageQuality quality)
    {
        //PerformanceCheckUI_Reslut uiInstance = PerformanceCheckManager.Instance.flowShareData.ui_Reslut;
        //uiInstance.tips.text = "您的检测结果为<color=red>" + PerformanceCheckManager.GetImageQualityDesc(quality) + ",</color>但由于你的计算机性能较低,一些额外的渲染效果没有开启，您可以从以下方案中选择一个";

        //uiInstance.ConfigTwoRoot.SetActive(true);
        //uiInstance.ConfigOneRoot.SetActive(true);

        //uiInstance.ConfigOneBtn.onClick.AddListener(NotSupportBestImangeEffect_ChooseDownResolution);
        //uiInstance.ConfigTwoBtn.onClick.AddListener(NotSupportBestImangeEffect_ChooseCloseImangeEffect);
        //currAutoSelect = NotSupportBestImangeEffect_ChooseDownResolution;
        //uiInstance.ConfigOneBtnDesc.text = "确认(" + AutoSelectTime + ")";
        //uiInstance.ConfigTwoBtnDesc.text = "确认";

        //int pixel = ImageSetting.GetResolutionPixels(ImageSetting.currResolution);
        
        ////计算出帧数差
        //float dealt = (PerformanceCheckManager.PassLevleFps - PerformanceCheck_FPS.currFPS);
        //dealt /= PerformanceCheckManager.PassLevleFps;

        //TargetResolution = ImageSetting.AdjustResolutionFromPixels(pixel, 1 - dealt);

        //uiInstance.ConfigOneDesc.text = "此方案，您将会<color=red>获得完整的画面效果</color>,但是您的游戏将会在<color=red>窗口模式下," +
        //                               "以" + TargetResolution + "分辨率</color>运行,您可以再在<color=red>登入游戏后按[ESC]->视频设置->全屏</color>以全屏模式运行游戏";

        //uiInstance.ConfigTwoDesc.text = "此方案您将在全屏模式下进行游戏，但是会<color=red>关闭图像后处理</color>来提升性能," +
        //                                "您可以再在<color=red>登入游戏后按[ESC]->视频设置->图像后处理</color>开启它";
        //WaitingSelect = true;
        NotSupportBestImangeEffect_ChooseCloseImangeEffect();
    }

    void NotSupportBestImangeEffect_ChooseCloseImangeEffect()
    {
        //WaitingSelect = false;
        //PerformanceCheckUI_Reslut uiInstance = PerformanceCheckManager.Instance.flowShareData.ui_Reslut;
        //uiInstance.tips.text = "测试完成,您可以在<color=red>登入游戏后按[ESC]->视频设置</color>中修改您的选项。正在进入游戏.....";

        //uiInstance.ConfigOneBtn.onClick.RemoveAllListeners();
        //uiInstance.ConfigTwoBtn.onClick.RemoveAllListeners();

        //uiInstance.ConfigTwoRoot.SetActive(false);
        //uiInstance.ConfigOneRoot.SetActive(false);

        ImageSetting.GetData().AdvanceImageEffectEnabled = 0;

        ImageSetting.Clear();
        EndPerformanceCheck();
    }

    void NotSupportBestImangeEffect_ChooseDownResolution()
    {
        //WaitingSelect = false;
        //PerformanceCheckUI_Reslut uiInstance = PerformanceCheckManager.Instance.flowShareData.ui_Reslut;
        //uiInstance.tips.text = "测试完成,您可以在<color=red>登入游戏后按[ESC]->视频设置</color>中修改您的选项。正在进入游戏.....";

        //uiInstance.ConfigOneBtn.onClick.RemoveAllListeners();
        //uiInstance.ConfigTwoBtn.onClick.RemoveAllListeners();
        //uiInstance.ConfigTwoRoot.SetActive(false);
        //uiInstance.ConfigOneRoot.SetActive(false);

        ImageSetting.GetData().AdvanceImageEffectEnabled = 1;
        ImageSetting.SetResolution(TargetResolution, false);
        ImageSetting.Clear();
        EndPerformanceCheck();
    }
    #endregion

    public void EndPerformanceCheck()
    {
        int cout = PerformanceCheckManager.Instance.flowShareData.OpaqueModelList.Count;
        for (int i = 0; i < cout; i++)
        {
            MeshRenderer re = PerformanceCheckManager.Instance.flowShareData.OpaqueModelList[i];
            re.enabled = false;
        }

        QualitySettings.vSyncCount = PerformanceCheckManager.Instance.flowShareData.oldAsyncCout;
        Application.targetFrameRate = PerformanceCheckManager.Instance.flowShareData.oldTargetFrame;

        PlayerSettingLocalConfig.Instance.UnLoad();
        PerformanceCheckManager.Instance.StartCoroutine(EnterGame());
    }

    IEnumerator EnterGame()
    {
        yield return new WaitForSeconds(2.0f);
        if (AssetBundleManager.isCanLoadLevel(PerformanceCheckManager.LoginSceneName))
        {
            AssetBundleManager.LoadLevelAsync(PerformanceCheckManager.LoginSceneName);
        }
    }

    public override void OnLeave()
    {

    }

    public override void OnUpdate(float dealTime)
    {
        //if(!WaitingSelect)
        //{
        //    return;
        //}

        //PerformanceCheckUI_Reslut uiInstance = PerformanceCheckManager.Instance.flowShareData.ui_Reslut;
        //currWaitingTime += dealTime;
        //if(currWaitingTime > 0.98)
        //{
        //    AutoSelectTime--;
        //    if(AutoSelectTime < 0)
        //    {
        //        if (null != currAutoSelect)
        //        {
        //            currAutoSelect();
        //        }
        //        return;
        //    }
        //    uiInstance.ConfigOneBtnDesc.text = "确认(" + AutoSelectTime + ")";
        //}
    }

}



