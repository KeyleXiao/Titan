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
public partial class PerformanceCheckFlow_RenderingLevel : PerformanceCheckFlowBase
{
    FlowShareData shareData = null;
    PerformanceCheckManager.PerformanceCheckReslut reslutData = null;
    private int currentGeometricPassLevel = 0;
    private int currentImagePassLevel = 0;

    private int Geometric_LevelFastestShowCount = 50;
    private int Geometric_LevelGoodShowCount = 70;
    private int Geometric_LevelHighShowCount = 80;

    //private int ImageEffect_LevelFastestShowCount = 0; 最低级没有
    private int ImageEffect_LevelGoodShowCount = 8;//中级需要8个
    private int ImageEffect_LevelHighShowCount = 15;//高级需要glow+glow outline + global fog  + 2（HDR）= 15
    //private int ImageEffect_LevelBestShowCount = 0; 最高级跑全部

    private int currCheckStep = 0;

    private float timer = 0.0f;

    private bool finishRenderingTest = false;

    //需要等待多长时间
    private float NeedToWaitTime = 0.0f;
    private float perWaitTime = 0.98f;
    public override void OnEnter()
    {
        shareData = PerformanceCheckManager.Instance.flowShareData;
        reslutData = PerformanceCheckManager.Instance.ReslutData;
        reslutData.NotDoPerformanceCheck = false;
        reslutData.VeryLowMachine = false;
        reslutData.GeometricRenderLevel = 1;
        reslutData.ImageEffectRenderLevel = 1;
        currCheckStep = 0;
        timer = 0.0f;
        finishRenderingTest = false;
        NeedToWaitTime = -1;
    }


    public override void OnLeave()
    {
        
    }


    public override void OnUpdate(float dealTime)
    {
        if (finishRenderingTest)
        {
            return;
        }
        timer += dealTime;
        if (timer > NeedToWaitTime)
        {
            timer = 0;
            if(PerformanceCheck_FPS.currFPS >= PerformanceCheckManager.PassLevleFps)
            {
                InCreasTestLevel();
            }
            else
            {
                OnEndTest();
            }
        }
    }

    //结束测试
    private void OnEndTest()
    {
        finishRenderingTest = true;
        shareData.StartCheckTimer = false;
        reslutData.ImageEffectRenderLevel = currentImagePassLevel;
        reslutData.GeometricRenderLevel = currentGeometricPassLevel;
        //PerformanceCheckManager.Instance.flowShareData.CheckUI.gameObject.SetActive(false);
        PerformanceCheckManager.Instance.ChangeFlow(CheckFlow.Finish);
    }

    private void InCreasTestLevel()
    {
        currCheckStep++;
        switch (currCheckStep)
        {
            case 1:
                TestLevelFastest_Geometric();
                NeedToWaitTime = perWaitTime;
                PerformanceCheck_FPS.PauseWithChangedSettings();
                break;
            case 2:
                currentGeometricPassLevel++;
                //不支持RT,不需要做测试了
                if (reslutData.notSupportRT)
                {
                    OnEndTest();
                    return;
                }
                TestLevelFastest_ImageEffect();
                NeedToWaitTime = perWaitTime;
                PerformanceCheck_FPS.PauseWithChangedSettings();
                break;
            case 3:
                currentImagePassLevel++;
                TestLevelGood_Geometric();
                NeedToWaitTime = perWaitTime;
                PerformanceCheck_FPS.PauseWithChangedSettings();
                break;
            case 4:
                currentGeometricPassLevel++;

                TestLevelGood_ImageEffect();
                NeedToWaitTime = perWaitTime;
                PerformanceCheck_FPS.PauseWithChangedSettings();
                break;
            case 5:
                currentImagePassLevel++;
                //不支持高等级，不需要测试了
                if (reslutData.notSupportHighLevel)
                {
                    OnEndTest();
                    return;
                }
                TestLevelHigh_Geometric();
                NeedToWaitTime = perWaitTime;
                PerformanceCheck_FPS.PauseWithChangedSettings();
                break;
            case 6:
                currentGeometricPassLevel++;
                TestLevelHigh_ImageEffect();
                NeedToWaitTime = perWaitTime;
                PerformanceCheck_FPS.PauseWithChangedSettings();
                break;
            case 7:
                currentImagePassLevel++;
                TestLevelBest_Geometric();
                NeedToWaitTime = perWaitTime;
                PerformanceCheck_FPS.PauseWithChangedSettings();
                break;
            case 8:
                currentGeometricPassLevel++;
                TestLevelBest_ImageEffect();
                NeedToWaitTime = perWaitTime;
                PerformanceCheck_FPS.PauseWithChangedSettings();
                break;
            case 9:
                currentImagePassLevel++;
                OnEndTest();
                break;
        }
    }

    private void TestLevelBest_ImageEffect()
    {
        for (int i = 0; i < PerformanceCheckManager.MaxImageEffectCout; i++)
        {
            shareData.ImageEffectList[i].enabled = true;
        }
    }

    private void TestLevelHigh_ImageEffect()
    {
        for (int i = 0; i < ImageEffect_LevelHighShowCount; i++)
        {
            shareData.ImageEffectList[i].enabled = true;
        }
    }

    private void TestLevelGood_ImageEffect()
    {
        for (int i = 0; i < ImageEffect_LevelGoodShowCount; i++)
        {
            shareData.ImageEffectList[i].enabled = true;
        }
    }

    private void TestLevelFastest_ImageEffect()
    {

        for(int i = 0; i < shareData.ImageEffectList.Count;i++)
        {
            shareData.ImageEffectList[i].enabled = false;
        }
    }

    private void TestLevelBest_Geometric()
    {
        shareData.CheckUI.AllowSkip();
        ImageSetting.SetImageQuality(ImageQuality.Best);
        
        QualitySettings.vSyncCount = 0;
        Application.targetFrameRate = 0;
        shareData.shareCamera.depthTextureMode = DepthTextureMode.DepthNormals;
        shareData.shareCamera.renderingPath = RenderingPath.DeferredShading;
        shareData.shareSunLight.enabled = true;
        shareData.shareSunLight.shadows = LightShadows.Soft;
        int cout = PerformanceCheckManager.MaxOpaqueModelCount;
        for (int i = 0; i < cout; i++)
        {
            shareData.OpaqueModelList[i].enabled = true;
        }

        cout = PerformanceCheckManager.MaxTransparentModelCount;
        for (int i = 0; i < cout; i++)
        {
            shareData.TransparentModelList[i].enabled = true;
        }

        cout = PerformanceCheckManager.MaxSkinnedModelCount;
        for (int i = 0; i < cout; i++)
        {
            shareData.SkinnedObjList[i].SetActive(true);
        }
    }

    private void TestLevelHigh_Geometric()
    {
        ImageSetting.SetImageQuality(ImageQuality.High);
        shareData.shareCamera.depthTextureMode = DepthTextureMode.Depth;
        shareData.shareCamera.renderingPath = RenderingPath.DeferredShading;
        shareData.shareSunLight.enabled = true;
        shareData.shareSunLight.shadows = LightShadows.Hard;
        QualitySettings.vSyncCount = 0;
        Application.targetFrameRate = 0;
        int cout =  Geometric_LevelHighShowCount;
        for (int i = 0; i < cout; i++)
        {
            shareData.OpaqueModelList[i].enabled = true;
        }

        cout =  Geometric_LevelHighShowCount;
        for (int i = 0; i < cout; i++)
        {
            shareData.TransparentModelList[i].enabled = true;
        }

        cout = PerformanceCheckManager.MaxSkinnedModelCount ;
        for (int i = 0; i < cout; i++)
        {
            shareData.SkinnedObjList[i].SetActive(true);
        }
    }

    private void TestLevelGood_Geometric()
    {
        ImageSetting.SetImageQuality(ImageQuality.Good);
        shareData.shareCamera.renderingPath = RenderingPath.Forward;
        shareData.shareSunLight.enabled = true;
        shareData.shareCamera.depthTextureMode = DepthTextureMode.None;
        shareData.shareSunLight.shadows = LightShadows.None;
        QualitySettings.vSyncCount = 0;
        Application.targetFrameRate = 0;
        int cout =Geometric_LevelGoodShowCount;
        for (int i = 0; i < cout; i++)
        {
            shareData.OpaqueModelList[i].enabled = true;
        }

        cout =  Geometric_LevelGoodShowCount;
        for (int i = 0; i < cout; i++)
        {
            shareData.TransparentModelList[i].enabled = true;
        }

        cout = PerformanceCheckManager.MaxSkinnedModelCount;
        for (int i = 0; i < cout; i++)
        {
            shareData.SkinnedObjList[i].SetActive(true);
        }
    }

    private void TestLevelFastest_Geometric()
    {
        shareData.shareCamera.renderingPath = RenderingPath.Forward;
        shareData.shareSunLight.enabled = false;
        shareData.shareCamera.depthTextureMode = DepthTextureMode.None;
        ImageSetting.SetImageQuality(ImageQuality.Fastest);
        QualitySettings.vSyncCount = 0;
        Application.targetFrameRate = 0;
        int cout = Geometric_LevelFastestShowCount;
        for (int i = 0; i < cout; i++)
        {
            shareData.OpaqueModelList[i].enabled = true;
        }

        cout =  Geometric_LevelFastestShowCount;
        for (int i = 0; i < cout; i++)
        {
            shareData.TransparentModelList[i].enabled = true;
        }

        cout = PerformanceCheckManager.MaxSkinnedModelCount;
        for (int i = 0; i < cout; i++)
        {
            shareData.SkinnedObjList[i].SetActive(true);

        }

    }
}



