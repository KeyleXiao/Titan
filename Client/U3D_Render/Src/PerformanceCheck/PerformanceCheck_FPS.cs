/*******************************************************************
** 文件名:	PerformanceCheck_FPS.cs
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

public class PerformanceCheck_FPS : MonoBehaviour
{
    private float updateInterval = 0.3F;

    private float accum = 0;
    private int frames = 0;
    private float timeleft; 

    public static float currFPS = 0.0f;
    public static System.Action<float> onFpsChanged = null;
    public static int IgonreTime = 0;

    /// <summary>
    /// 切换等级的时候比较卡，这个时候不计算帧数，以免影响结果
    /// </summary>
    public static void PauseWithChangedSettings()
    {
        IgonreTime = 5;
    }

    public void Start()
    {
        timeleft = updateInterval;
    }

    public void Update()
    {
        if (IgonreTime > 0)
        {
            IgonreTime--;
            return;
        }
        timeleft -= Time.unscaledDeltaTime;
        accum +=  1 / Time.unscaledDeltaTime;
        ++frames;
        if (timeleft <= 0.0)
        {
            currFPS = accum / frames;
            accum = 0;
            frames = 0;
            timeleft = updateInterval;
            if(null != onFpsChanged)
            {
                onFpsChanged(currFPS);
            }
        }
    }
}



