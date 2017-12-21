///*******************************************************************
//** 文件名:	PerformanceCheckFlow_HardWareSupport.cs
//** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
//** 创建人:	谭强均
//** 日  期:	2014/11/21
//** 版  本:	1.0
//** 描  述:   性能检测脚本
//** 应  用:  	跟随物体

//**************************** 修改记录 ******************************
//** 修改人: 
//** 日  期: 
//** 描  述: 
//********************************************************************/

//using UnityEngine;
//using System.Collections;
//using System.Collections.Generic;
//using ASpeedGame.Data.PlayerSettingLocalConfig;

//public partial class PerformanceCheckFlow_WaitingForSelect : PerformanceCheckFlowBase
//{
//    PerformanceCheckUI_Waiting ui_Waiting = null;
//    public override void OnEnter()
//    {
        
//        PlayerSettingLocalConfig.Instance.Load(ViewConfigManager.GetConfigFullPath("PlayerLocalSetting.csv", false));

//        ImageSetting.Init();

//        PerformanceCheckManager.Instance.flowShareData.oldAsyncCout = QualitySettings.vSyncCount;
//        PerformanceCheckManager.Instance.flowShareData.oldTargetFrame = Application.targetFrameRate;

//        QualitySettings.vSyncCount = 0;
//        Application.targetFrameRate = 0;

//        PerformanceCheckManager.Instance.flowShareData.CheckingBackground.gameObject.SetActive(true);
//        ui_Waiting = PerformanceCheckManager.Instance.flowShareData.ui_Waiting;
//        ui_Waiting.gameObject.SetActive(true);
//        ui_Waiting.Desc.text = "系统需要运行性能检测程序," +
//                     "以便为您设置合适的渲染质量，整个过程<color=red>不会超过" + PerformanceCheckManager.HoldDuringCheckTime + "秒</color>,您" +
//                     "可以随时中断这个过程，但渲染设置将默认为<color=red>" + PerformanceCheckManager.GetImageQualityDesc(PerformanceCheckManager.BreakCheckQuality) + "</color>,您" +
//                     "可以在<color=red>登入游戏后按【ESC】->视频设置</color>,调整游戏的渲染质量";
//        ui_Waiting.runCheckButton.onClick.AddListener(runCheckButtonClick);
//        ui_Waiting.BreakButton.onClick.AddListener(BreakButtonClick);


//    }

//    public override void OnLeave()
//    {
//        ui_Waiting.gameObject.SetActive(false);
//        ui_Waiting.runCheckButton.onClick.RemoveAllListeners();
//        ui_Waiting.BreakButton.onClick.RemoveAllListeners();
//    }

//    public override void OnUpdate(float dealTime)
//    {

//    }

//    public void runCheckButtonClick()
//    {
//        ImageSetting.SetResolution(ImageSetting.systemResolutions, true);
//        PerformanceCheckManager.Instance.ReslutData.NotDoPerformanceCheck = false;
//        PerformanceCheckManager.Instance.ChangeFlow(CheckFlow.HardWareSupport);
//        PerformanceCheckManager.Instance.flowShareData.currentCheckTime = PerformanceCheckManager.HoldDuringCheckTime;
//        PerformanceCheckManager.Instance.flowShareData.StartCheckTimer = true;
//        PerformanceCheckManager.Instance.flowShareData.ui_Checking.OnUpdate();
//        PerformanceCheckManager.Instance.flowShareData.ui_Checking.gameObject.SetActive(true);
//    }

//    public void BreakButtonClick()
//    {
//        PerformanceCheckManager.Instance.ReslutData.NotDoPerformanceCheck = true;
//        PerformanceCheckManager.Instance.ChangeFlow(CheckFlow.Finish);
//    }
//}



