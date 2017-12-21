/*******************************************************************
 * 文件名:	FrameManager.cs  
 * 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
 * 创建人:	李界华
 * 日  期:	2016-1-7
 * 版  本:	1.0
 * 描  述:	帧管理

********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.RegularExpressions;
using System;



/// <summary>
/// 游戏帧管理
/// </summary>
public static class GameFrameManager
{
    public static FPSAvgMonitor FpsMonitor;
    private static FPSMonitorAlert fspAlert;

    static GameFrameManager()
    {
        FpsMonitor = new FPSAvgMonitor();
        fspAlert = new FPSMonitorAlert();
    }
    
    public static void OnUpdate()
    {
        FPS.OnUpdate();
        FpsMonitor.OnUpdate();
    }
}

