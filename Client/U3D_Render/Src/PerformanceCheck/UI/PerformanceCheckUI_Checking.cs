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
using UnityEngine.UI;
public class PerformanceCheckUI_Checking : MonoBehaviour
{
    public Button BreakCheckingButton = null;
    public Text FpsText = null;
    public Text TimeText = null;
    public Text HardWareInfo = null;
    public Text TipsText = null;

    private float currTime = 0;
    public void Init()
    {
        HardWareInfo.text = "处理器:" + SystemInfo.processorType + "\n" +
                            "显卡:" + SystemInfo.graphicsDeviceName + "\n" +
                            "显存:" + SystemInfo.graphicsMemorySize + "MB\n" +
                            "内存:" + SystemInfo.systemMemorySize + "MB\n" +
                            "如以上信息不正确，请联系官网客服.";
        TipsText.text = "正在检测.......检测过程中<color=red>请不要操作计算机</color>，以免影响检测结果";
        PerformanceCheck_FPS.onFpsChanged -= OnFpsChanged;
        PerformanceCheck_FPS.onFpsChanged += OnFpsChanged;
        BreakCheckingButton.onClick.AddListener(BreakButtonClick);
        currTime = 0;
    }

    public void BreakButtonClick()
    {
        PerformanceCheckManager.Instance.ReslutData.NotDoPerformanceCheck = true;
        PerformanceCheckManager.Instance.ChangeFlow(CheckFlow.Finish);
    }

    private void OnFpsChanged(float fps)
    {
        FpsText.text = "帧率:" + fps.ToString("F2");
    }

    public void OnUpdate()
    {
        TimeText.text = PerformanceCheckManager.Instance.flowShareData.currentCheckTime.ToString();
        //超时，直接去到完成，然后根据结果来设置
        if (PerformanceCheckManager.Instance.flowShareData.currentCheckTime <= 0)
        {
            PerformanceCheckManager.Instance.ChangeFlow(CheckFlow.Finish);
        }

    }

    public void Clear()
    {
        BreakCheckingButton.onClick.RemoveAllListeners();
        PerformanceCheck_FPS.onFpsChanged -= OnFpsChanged;
    }
}



