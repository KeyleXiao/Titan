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

public partial class PerformanceCheckFlow_HardWareSupport : PerformanceCheckFlowBase
{
    int MemoryRequire = 2000;
    int GraphsicMemoryRequire = 384;
    int shaderAPIRequire = 20;

    int highShaderAPIRequire = 30;
    int highRTCout = 2;
    int highMemoryRequire = 4000;
    int highGraphiscMemoryRequire = 768;
    public override void OnEnter()
    {
        PlayerSettingLocalConfig.Instance.Load(ViewConfigManager.GetConfigFullPath("PlayerLocalSetting.csv", false));

        ImageSetting.Init();

        PerformanceCheckManager.Instance.flowShareData.oldAsyncCout = QualitySettings.vSyncCount;
        PerformanceCheckManager.Instance.flowShareData.oldTargetFrame = Application.targetFrameRate;

        QualitySettings.vSyncCount = 0;

        ImageSetting.SetResolution(ImageSetting.systemResolutions, true,true);
        PerformanceCheckManager.Instance.flowShareData.currentCheckTime = PerformanceCheckManager.HoldDuringCheckTime;
        PerformanceCheckManager.Instance.flowShareData.StartCheckTimer = true;
        PerformanceCheckManager.Instance.flowShareData.CheckUI.OnUpdate();
    }

    public override void OnLeave()
    {
        
    }

    public override void OnUpdate(float dealTime)
    {
        //达不到最低配的要求
        if (SystemInfo.systemMemorySize < MemoryRequire ||
            SystemInfo.graphicsMemorySize < GraphsicMemoryRequire ||
            SystemInfo.graphicsShaderLevel < shaderAPIRequire)
        {
            //PerformanceCheckManager.Instance.flowShareData.ui_Checking.gameObject.SetActive(false);
            PerformanceCheckManager.Instance.ReslutData.VeryLowMachine = true;
            PerformanceCheckManager.Instance.ChangeFlow(CheckFlow.Finish);
            return;
        }

        //查看是否支持RT，不支持的话，中级渲染也不行,中级渲染需要2个rt
        Resolution res = ImageSetting.GetCurrentResolution();

        RenderTexture rt1 = new RenderTexture(res.width, res.height, 24);
        if(!rt1.Create())
        {
            PerformanceCheckManager.Instance.ReslutData.notSupportRT = true;
            //PerformanceCheckManager.Instance.flowShareData.ui_Checking.gameObject.SetActive(false);
            PerformanceCheckManager.Instance.ChangeFlow(CheckFlow.Finish);
            return;
        }

        RenderTexture rt2 = new RenderTexture(res.width, res.height, 24);
        if (!rt2.Create())
        {
            rt1.Release();
            GameObject.Destroy(rt1);
            rt1 = null;
            PerformanceCheckManager.Instance.ReslutData.notSupportRT = true;
            //PerformanceCheckManager.Instance.flowShareData.ui_Checking.gameObject.SetActive(false);
            PerformanceCheckManager.Instance.ChangeFlow(CheckFlow.Finish);
            return;
        }

        rt1.Release();
        GameObject.Destroy(rt1);
        rt1 = null;
        rt2.Release();
        GameObject.Destroy(rt2);
        rt2 = null;

        //不支持高等级渲染，硬件限制
        if (SystemInfo.graphicsShaderLevel < highShaderAPIRequire || SystemInfo.supportedRenderTargetCount < highRTCout
            || SystemInfo.systemMemorySize < highMemoryRequire || SystemInfo.graphicsMemorySize < highGraphiscMemoryRequire)
        {
            PerformanceCheckManager.Instance.ReslutData.notSupportHighLevel = true;
        }
        PerformanceCheckManager.Instance.ReslutData.VeryLowMachine = false;
        PerformanceCheckManager.Instance.ChangeFlow(CheckFlow.GeometricRender);
    }
}



